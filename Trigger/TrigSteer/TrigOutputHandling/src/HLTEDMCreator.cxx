
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


//#include "GaudiKernel/IToolSvc.h"
#include "AthViews/ViewHelper.h"
#include "HLTEDMCreator.h"
#include "StoreGate/WriteDecorHandle.h"

HLTEDMCreator::HLTEDMCreator( const std::string& type, 
            const std::string& name,
            const IInterface* parent )
  : base_class( type, name, parent ) {}

template<typename T>
StatusCode HLTEDMCreator::initHandles( const HandlesGroup<T>&  handles ) {
  CHECK( handles.out.initialize() );
  renounceArray( handles.out );
  CHECK( handles.in.initialize() );
  renounceArray( handles.in );
  CHECK( handles.views.initialize() );  
  renounceArray( handles.views );

  // the case w/o reading from views, both views handles and collection in views should be empty
  if ( handles.views.size() == 0 ) {
    CHECK( handles.in.size() == 0 );
  } else {
    // the case with views, for every output we expect an input View and an input collection inside that View
    CHECK( handles.out.size() == handles.in.size() );
    CHECK( handles.in.size()  == handles.views.size() );
  }
  return StatusCode::SUCCESS;
}


StatusCode HLTEDMCreator::initialize()
{  
  if ( m_fixLinks.size() > 0 ) {
    // Confirm that m_fixLinks is a sub-set of m_TrigCompositeContainer
    for (const std::string& entry : m_fixLinks) {
      const bool found = std::any_of(m_TrigCompositeContainer.begin(),
                                     m_TrigCompositeContainer.end(), [&](const auto& writeHandleKey) { return writeHandleKey.key() == entry; } );
      if (!found) {
        ATH_MSG_ERROR("FixLinks contains the entry " << entry << ", however this is not one of this EDMCreator tool's managed TrigCompositeContainers.");
        ATH_MSG_ERROR("Configure FixLinks to be a sub-set of TrigCompositeContainer");
        return StatusCode::FAILURE;
      }
    }
    // Set up the write decorate handles to hold the remapped data
    for ( const auto& writeHandleKey: m_TrigCompositeContainer ) {
      const bool doFixLinks = std::any_of(m_fixLinks.begin(), m_fixLinks.end(), [&](const std::string& s) { return s == writeHandleKey.key(); } );
      if (doFixLinks) {
        // This writeHandleKey is being included in the element link remapping
        m_remapLinkColKeys.emplace_back( writeHandleKey.key()+".remap_linkColKeys" );
        m_remapLinkColIndices.emplace_back( writeHandleKey.key()+".remap_linkColIndices" );
      }
    }
    ATH_CHECK( m_remapLinkColKeys.initialize() ) ;
    renounceArray( m_remapLinkColKeys ); // This is not strictly necessary however, since we have many of these collection and no consumer for it we can spare scheduler headache renouncing them
    ATH_CHECK( m_remapLinkColIndices.initialize() );
    renounceArray( m_remapLinkColIndices );
  }

  // this section has to appear after the above initialisation of DecorHandles, else the renounce of TrigComposite does not work as expected

#define INIT(__TYPE) \
  CHECK( initHandles( HandlesGroup<__TYPE>( m_##__TYPE, m_##__TYPE##InViews, m_##__TYPE##Views ) ) );

#define INIT_XAOD(__TYPE) \
  CHECK( initHandles( HandlesGroup<xAOD::__TYPE>( m_##__TYPE, m_##__TYPE##InViews, m_##__TYPE##Views ) ) );

  INIT( TrigRoiDescriptorCollection );
  INIT_XAOD( TrigCompositeContainer );
    
  INIT_XAOD( TrigEMClusterContainer );
  INIT_XAOD( TrigCaloClusterContainer );
  INIT_XAOD( TrigElectronContainer ); 
  INIT_XAOD( ElectronContainer ); 
  INIT_XAOD( PhotonContainer ); 
  INIT_XAOD( TrigPhotonContainer );
  INIT_XAOD( TrackParticleContainer );
  INIT_XAOD( TrigMissingETContainer );

  INIT_XAOD( L2StandAloneMuonContainer );
  INIT_XAOD( L2CombinedMuonContainer );
  INIT_XAOD( L2IsoMuonContainer );
  INIT_XAOD( MuonContainer );
  INIT_XAOD( TauJetContainer );
  INIT_XAOD( JetContainer );
  INIT_XAOD( VertexContainer );
  INIT_XAOD( TrigBphysContainer );  

  INIT_XAOD( CaloClusterContainer );

#undef INIT
#undef INIT_XAOD



  
  return StatusCode::SUCCESS;
}

template<class T>
struct plainGenerator {
  std::unique_ptr<T> data;
  void create() {
    data = std::make_unique<T>();
  }
  StatusCode record( SG::WriteHandle<T>& h ) {    
    return h.record( std::move( data ) );
  } 
};

template<class T, class STORE>
struct xAODGenerator {
  std::unique_ptr<T> data;
  std::unique_ptr<STORE> store;

  void create() {
    data  = std::make_unique<T>();
    store = std::make_unique<STORE>();
    data->setStore( store.get() );      
  }

  StatusCode record ( SG::WriteHandle<T>& h ) {
    return h.record( std::move( data ), std::move( store ) );
  } 
};

template<typename T>
StatusCode  HLTEDMCreator::noMerge( ViewContainer const&, const SG::ReadHandleKey<T>&,
            EventContext const&, T & ) const {
  //  if we are called it means views merging is requested but Type T does not support it (i.e. missing copy c'tor)
  return StatusCode::FAILURE;

}

template<typename T>
StatusCode  HLTEDMCreator::viewsMerge( ViewContainer const& views, const SG::ReadHandleKey<T>& inViewKey,
               EventContext const& context, T & output ) const {
  
  typedef typename T::base_value_type type_in_container;
  StoreGateSvc* sg = evtStore().operator->(); // why the get() method is returing a null ptr is a puzzle, we have to use this ugly call to operator instead of it
  CHECK( sg != nullptr );
  ViewHelper::ViewMerger merger( sg, msg() );
  merger.mergeViewCollection<type_in_container>( views, inViewKey, context, output );

  return StatusCode::SUCCESS;
}

 
StatusCode HLTEDMCreator::fixLinks() const {
  if ( m_fixLinks.size() == 0 ) {
    ATH_MSG_DEBUG("fixLinks: No collections defined for this tool");
    return StatusCode::SUCCESS;
  }

  static const SG::AuxElement::ConstAccessor< std::vector< uint32_t > > keyAccessor( "linkColKeys" );
  static const SG::AuxElement::ConstAccessor< std::vector< uint16_t > > indexAccessor( "linkColIndices" );

  ATH_MSG_DEBUG("fixLinks called for " << m_fixLinks.size() << " of " << m_TrigCompositeContainer.size() << " collections");

  // Do the remapping
  int writeHandleArrayIndex = -1;
  for ( const auto& writeHandleKey: m_TrigCompositeContainer ) {
    // Check if we are re-mapping this handle
    const bool doFixLinks = std::any_of(m_fixLinks.begin(), m_fixLinks.end(), [&](const std::string& s) { return s == writeHandleKey.key(); } );
    if ( not doFixLinks ) {
      ATH_MSG_DEBUG("Not requested to fix TrigComposite ElementLinks for " << writeHandleKey.key());
      continue;
    }

    // Only increment this index for the sub-set of the TrigComposite collections that we are fixing. Mirror the initialize() logic.
    ++writeHandleArrayIndex;

    ATH_MSG_DEBUG("Fixing links: confirm collection is there: " << writeHandleKey.key() << ", write handle array index: " << writeHandleArrayIndex);
    SG::ReadHandle<xAOD::TrigCompositeContainer> readHandle( writeHandleKey.key() );
    if ( not readHandle.isValid() ) { // object missing, this is now an error as we should have literally just created it
      ATH_MSG_ERROR("  Collection is not present. " << writeHandleKey.key() << " should have been created by createIfMissing.");
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Collection exists with size " << readHandle->size() << " Decision objects" );
    ATH_MSG_DEBUG("Adding decorations: " << m_remapLinkColKeys.at( writeHandleArrayIndex ).key() << " and " << m_remapLinkColIndices.at( writeHandleArrayIndex ).key() );
    
    SG::WriteDecorHandle<xAOD::TrigCompositeContainer, std::vector<uint32_t> > keyDecor( m_remapLinkColKeys.at( writeHandleArrayIndex ) );
    SG::WriteDecorHandle<xAOD::TrigCompositeContainer, std::vector<uint16_t> > indexDecor( m_remapLinkColIndices.at( writeHandleArrayIndex ) );

    // Examine each input TC
    int decisionObjectIndex = -1;
    for ( auto inputDecision : *( readHandle.cptr() ) ) {
      ++decisionObjectIndex;

      // Retrieve the link information for remapping
      std::vector< uint32_t > remappedKeys = keyAccessor( *inputDecision );
      std::vector< uint16_t > remappedIndexes = indexAccessor( *inputDecision );

      // Search the linked collections for remapping
      size_t const collectionTotal = inputDecision->linkColNames().size();
      ATH_MSG_DEBUG("  Decision object #" << decisionObjectIndex << " has " << collectionTotal << " links");
      for ( size_t elementLinkIndex = 0; elementLinkIndex < collectionTotal; ++elementLinkIndex ) {

        // Load ElementLink identifiers (except for CLID)
        std::string const collectionName = inputDecision->linkColNames().at(elementLinkIndex);
        uint32_t const collectionKey = remappedKeys.at(elementLinkIndex); //Note: This is the existing before-remap key
        std::string const keyString = *( evtStore()->keyToString( collectionKey ) );
        uint16_t const collectionIndex = remappedIndexes.at(elementLinkIndex); //Note: This is the existing before-remap index

        // Check for remapping in a merge
        uint32_t newKey = 0;
        size_t newIndex = 0;
        bool isRemapped = evtStore()->tryELRemap( collectionKey, collectionIndex, newKey, newIndex);
        if ( isRemapped ) {

          ATH_MSG_DEBUG( "    Remap link [" << collectionName <<"] from " << keyString << " to " << *( evtStore()->keyToString( newKey ) ) << ", from index " << collectionIndex << " to index " << newIndex );
          remappedKeys[ elementLinkIndex ] = newKey;
          remappedIndexes[ elementLinkIndex ] = newIndex;

        } else {

          ATH_MSG_DEBUG( "    StoreGate did not remap link [" << collectionName << "] from  " << keyString << " index " << collectionIndex );

        }

      }
      
      // Save the remaps
      keyDecor( *inputDecision ) = remappedKeys;
      indexDecor( *inputDecision ) = remappedIndexes;

    }    
  }  
  
  return StatusCode::SUCCESS;
}

template<typename T, typename G, typename M>
StatusCode HLTEDMCreator::createIfMissing( const EventContext& context, const ConstHandlesGroup<T>& handles, G& generator, M merger ) const {

  for (size_t i = 0; i < handles.out.size(); ++i) {
    SG::WriteHandleKey<T> writeHandleKey = handles.out.at(i); 

    // Note: This is correct. We are testing if we can read, and if we cannot then we write.
    // What we write will either be a dummy (empty) container, or be populated from N in-View collections.
    SG::ReadHandle<T> readHandle( writeHandleKey.key() );

    if ( readHandle.isValid() ) {
      ATH_MSG_DEBUG( "The " << writeHandleKey.key() << " already present" );
    } else {
      ATH_MSG_DEBUG( "The " << writeHandleKey.key() << " was absent, creating it" );
      generator.create();      
      if ( handles.views.size() != 0 ) {

        SG::ReadHandleKey<ViewContainer> viewsReadHandleKey = handles.views.at(i);
        ATH_MSG_DEBUG("Will be trying to merge from the " << viewsReadHandleKey.key() << " view container into that output");

        auto viewsHandle = SG::makeHandle( viewsReadHandleKey, context );
        if ( viewsHandle.isValid() ) {
          SG::ReadHandleKey<T> inViewReadHandleKey = handles.in.at(i);
          ATH_MSG_DEBUG("Will be merging from " << viewsHandle->size() << " views using in-view key " << inViewReadHandleKey.key() );
          CHECK( (this->*merger)( *viewsHandle, inViewReadHandleKey , context, *generator.data.get() ) );
        } else {
          ATH_MSG_DEBUG("Views " << viewsReadHandleKey.key() << " are missing. Will leave " << writeHandleKey.key() << " output collection empty.");
        }

      }
      auto writeHandle = SG::makeHandle( writeHandleKey, context );
      CHECK( generator.record( writeHandle ) );
    }
  }
  return StatusCode::SUCCESS;
}



StatusCode HLTEDMCreator::createOutput(const EventContext& context) const {
  ATH_MSG_DEBUG("Confirming / Creating this tool's output");
  if ( m_dumpSGBefore )  
    ATH_MSG_DEBUG( evtStore()->dump() );

#define CREATE(__TYPE) \
    {                 \
      plainGenerator<__TYPE> generator;         \
      CHECK( createIfMissing<__TYPE>( context, ConstHandlesGroup<__TYPE>( m_##__TYPE, m_##__TYPE##InViews, m_##__TYPE##Views ), generator, &HLTEDMCreator::noMerge<__TYPE> ) ); \
    }

  CREATE( TrigRoiDescriptorCollection )

#undef CREATE

#define CREATE_XAOD(__TYPE, __STORE_TYPE) \
  { \
    xAODGenerator<xAOD::__TYPE, xAOD::__STORE_TYPE> generator; \
    CHECK( createIfMissing<xAOD::__TYPE>( context, ConstHandlesGroup<xAOD::__TYPE>( m_##__TYPE, m_##__TYPE##InViews, m_##__TYPE##Views ), generator, &HLTEDMCreator::viewsMerge<xAOD::__TYPE> )  ); \
  }


#define CREATE_XAOD_NO_MERGE(__TYPE, __STORE_TYPE)      \
  { \
    xAODGenerator<xAOD::__TYPE, xAOD::__STORE_TYPE> generator; \
    CHECK( createIfMissing<xAOD::__TYPE>( context, ConstHandlesGroup<xAOD::__TYPE>( m_##__TYPE, m_##__TYPE##InViews, m_##__TYPE##Views ), generator, &HLTEDMCreator::noMerge<xAOD::__TYPE> )  ); \
  }
  
  CREATE_XAOD_NO_MERGE( TrigCompositeContainer, TrigCompositeAuxContainer )
  CREATE_XAOD( TrigElectronContainer, TrigElectronAuxContainer )
  CREATE_XAOD( ElectronContainer, ElectronAuxContainer )
  CREATE_XAOD( PhotonContainer, PhotonAuxContainer )
  CREATE_XAOD( TrigPhotonContainer, TrigPhotonAuxContainer )
  CREATE_XAOD( TrigEMClusterContainer, TrigEMClusterAuxContainer )
  CREATE_XAOD( TrigCaloClusterContainer, TrigCaloClusterAuxContainer )
  CREATE_XAOD( TrackParticleContainer, TrackParticleAuxContainer )
  CREATE_XAOD( TrigMissingETContainer, TrigMissingETAuxContainer )

  CREATE_XAOD( L2StandAloneMuonContainer, L2StandAloneMuonAuxContainer );
  CREATE_XAOD( L2CombinedMuonContainer, L2CombinedMuonAuxContainer );
  CREATE_XAOD( L2IsoMuonContainer, L2IsoMuonAuxContainer );
  CREATE_XAOD( MuonContainer, MuonAuxContainer );
  CREATE_XAOD( TauJetContainer, TauJetAuxContainer );

  CREATE_XAOD( CaloClusterContainer, CaloClusterTrigAuxContainer ); // NOTE: Difference in interface and aux
  // After view collections are merged, need to update collection links

  CREATE_XAOD( JetContainer, JetAuxContainer );
  CREATE_XAOD( VertexContainer,VertexAuxContainer );
  CREATE_XAOD( TrigBphysContainer, TrigBphysAuxContainer );

  ATH_CHECK( fixLinks() );
  
#undef CREATE_XAOD
#undef CREATE_XAOD_NO_MERGE

  if ( m_dumpSGAfter )  
    ATH_MSG_DEBUG( evtStore()->dump() );


  return StatusCode::SUCCESS;
}