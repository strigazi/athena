/*
  General-purpose view creation algorithm <bwynne@cern.ch>
  
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventViewCreatorAlgorithm.h"
#include "AthLinks/ElementLink.h"
#include "AthViews/ViewHelper.h"
#include "AthViews/View.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/HLTIdentifier.h"


EventViewCreatorAlgorithm::EventViewCreatorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : InputMakerBase( name, pSvcLocator ) {}


EventViewCreatorAlgorithm::~EventViewCreatorAlgorithm(){}


StatusCode EventViewCreatorAlgorithm::initialize()
{
  ATH_MSG_DEBUG("Will produce views="<<m_viewsKey<<" roIs="<<m_inViewRoIs );
  CHECK( m_viewsKey.initialize() );
  CHECK( m_inViewRoIs.initialize() );
  CHECK( m_scheduler.retrieve() );
  return StatusCode::SUCCESS;
}


StatusCode EventViewCreatorAlgorithm::finalize() {
    ATH_MSG_INFO( "Finalizing " << name() << "..." );
    return StatusCode::SUCCESS;
}


StatusCode EventViewCreatorAlgorithm::execute_r( const EventContext& context ) const
{
  const EventContext& ctx = getContext();
 
  auto outputHandles = decisionOutputs().makeHandles(context);

  size_t validInput=0;
    for ( auto inputKey: decisionInputs() ) {
      auto inputHandle = SG::makeHandle( inputKey, context );
      ATH_MSG_DEBUG(" "<<inputKey.key()<<(inputHandle.isValid()? "valid": "not valid" ) );
      if (inputHandle.isValid()) validInput++;
    }
    ATH_MSG_DEBUG( "number of implicit ReadHandles is " << decisionInputs().size() <<", "<< validInput<<" are valid" );

   
    // make the views
  auto viewVector = std::make_unique< ViewContainer >();
  auto contexts = std::vector<EventContext>( );
  unsigned int viewCounter = 0;
  unsigned int conditionsRun = getContext().getExtension<Atlas::ExtendedEventContext>()->conditionsRun();

  //  const TrigRoiDescriptor* previousRoI = 0;
  // mapping RoI with index of the View in the vector
  // This is used to link the same view to differnt decisions that come from the same RoI
  std::map <const TrigRoiDescriptor*, int> viewMap;
  std::map <const TrigRoiDescriptor*, int>::iterator itViewMap;
  size_t outputIndex = 0;
 // Loop over all input containers, which are of course TrigComposites, and request their features
  // this is the same as InputMaker, apart from the view creation. The loop can be splitted in two loops, to have one common part
  for ( auto inputKey: decisionInputs() ) {
    auto inputHandle = SG::makeHandle( inputKey, context );
    if( not inputHandle.isValid() ) {
      ATH_MSG_DEBUG( "Got no decisions from input "<< inputKey.key());
      outputIndex++;
      continue;
    }
    
    if( inputHandle->size() == 0){ // input filtered out
      ATH_MSG_ERROR( "Got 0 decisions from valid input "<< inputKey.key()<<". Is it expected?");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Got input "<< inputKey.key()<<" with " << inputHandle->size() << " elements" );
    
     // prepare output decisions
    auto OutputDecisions = std::make_unique<TrigCompositeUtils::DecisionContainer>();
    auto dec_aux = std::make_unique<TrigCompositeUtils::DecisionAuxContainer>();
    OutputDecisions->setStore( dec_aux.get() );

    size_t input_counter = 0;
    for ( auto Idecision: *inputHandle ) {
      //make one TC decision output per input and connect to previous
      auto newd = TrigCompositeUtils::newDecisionIn( OutputDecisions.get(), name() );
      TrigCompositeUtils::linkToPrevious( newd, inputKey.key(), input_counter );
      {
	//copy decisions ID
	TrigCompositeUtils::DecisionIDContainer objDecisions;      
	TrigCompositeUtils::decisionIDs( Idecision, objDecisions );
	for ( const HLT::Identifier& id: objDecisions ) {
	  TrigCompositeUtils::addDecisionID( id, newd );
	}
      }	
      
      // pull RoI descriptor
      TrigCompositeUtils::LinkInfo<TrigRoiDescriptorCollection> roiELInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>(Idecision, m_roisLink );
      ATH_CHECK( roiELInfo.isValid() );
      // associate this RoI to output decisions
      auto roiDescriptor = *roiELInfo.link;
      ATH_MSG_DEBUG( "Placing TrigRoiDescriptor " << *roiDescriptor );
      newd->setObjectLink( "initialRoI", roiELInfo.link );

      // search for existing view
      itViewMap = viewMap.find(roiDescriptor);
      if (itViewMap != viewMap.end()){
	int iview=itViewMap->second;
	newd->setObjectLink( "view", ElementLink< ViewContainer >(m_viewsKey.key(), iview ));//adding view to TC
	ATH_MSG_DEBUG( "Adding already mapped view " << iview << " in ViewVector , to new decision");
	//	need to check if this View has parent views? can we have more than one parent views?
      }
      else{

      // this is added for EVCreator explicitally, differently from InputMaker:
      //   if ( previousRoI == roiDescriptor ) {
	// TODO here code supporting the case wnen we have many decisions associated to a single RoI
	// continue;
      //}
	//      previousRoI = roiDescriptor;
	
	ATH_MSG_DEBUG( "Positive decisions on RoI, preparing view" );
	// fill the RoI output collection
	auto oneRoIColl = std::make_unique< ConstDataVector<TrigRoiDescriptorCollection> >();    
	oneRoIColl->clear( SG::VIEW_ELEMENTS ); //Don't delete the RoIs
	oneRoIColl->push_back( roiDescriptor );
	
	// make the view
	ATH_MSG_DEBUG( "Making the View" );
	auto newview = ViewHelper::makeView( name()+"_view", viewCounter++, m_viewFallThrough ); //pointer to the view
	viewVector->push_back( newview );
	contexts.emplace_back( ctx );
	contexts.back().setExtension( Atlas::ExtendedEventContext( viewVector->back(), conditionsRun ) );
	
	// link decision to this view
	newd->setObjectLink( "view", ElementLink< ViewContainer >(m_viewsKey.key(), viewVector->size()-1 ));//adding view to TC
      	viewMap[roiDescriptor]=viewVector->size()-1;
	ATH_MSG_DEBUG( "Adding new view to new decision; storing view in viewVector component "<<viewVector->size()-1);
      
	// see if there is a view linked to the decision object, if so link it to the view that is just made
	TrigCompositeUtils::LinkInfo<ViewContainer> parentViewLinkInfo = TrigCompositeUtils::findLink<ViewContainer>(Idecision, "view" );
	if ( parentViewLinkInfo.isValid() ) {
	  CHECK( parentViewLinkInfo.link.isValid() );
	  auto parentView = *parentViewLinkInfo.link;
	  viewVector->back()->linkParent( parentView );
	  ATH_MSG_DEBUG( "Parent view linked" );
	} else {
	  if ( m_requireParentView ) {
	    ATH_MSG_ERROR( "Parent view not linked because it could not be found" );
	    ATH_MSG_ERROR( TrigCompositeUtils::dump( Idecision, [](const xAOD::TrigComposite* tc){ 
		  return "TC " + tc->name() + ( tc->hasObjectLink("view") ? " has view " : " has no view " );
		    } ) );
	    return StatusCode::FAILURE;
	  }

	}
	
	//store the RoI in the view
	auto handle = SG::makeHandle( m_inViewRoIs, contexts.back() );
	CHECK( handle.setProxyDict( viewVector->back() ) );
	CHECK( handle.record( std::move( oneRoIColl ) ) );
      }
      input_counter++;      
    }

    ATH_MSG_DEBUG( "Recording output key " <<  decisionOutputs()[ outputIndex ].key() <<" of size "<<OutputDecisions->size()  <<" at index "<< outputIndex);
    CHECK( outputHandles[outputIndex].record( std::move( OutputDecisions ), std::move( dec_aux ) ) );
    outputIndex++;
  }

  
  ATH_MSG_DEBUG( "Launching execution in " << viewVector->size() << " views" );
  CHECK( ViewHelper::ScheduleViews( viewVector.get(),           // Vector containing views
         m_viewNodeName,                                   // CF node to attach views to
         ctx,                                              // Source context
         m_scheduler.get() ) );

  
  // store views
  auto viewsHandle = SG::makeHandle( m_viewsKey );
  CHECK( viewsHandle.record(  std::move( viewVector ) ) );
  ATH_MSG_DEBUG( "Store "<< viewsHandle->size() <<" Views");

  //debug - the same as inputmaker
  size_t validOutput=0;
  for ( auto outHandle: outputHandles ) {
    if( not outHandle.isValid() ) continue;
    validOutput++;
  }
  ATH_MSG_DEBUG("Produced "<<validOutput<<" decisions containers");
  if(validInput != validOutput) {
    ATH_MSG_ERROR("Found "<<validInput<<" inputs and "<<validOutput<<" outputs");
  }
  
  for ( auto outHandle: outputHandles ) {
    if( not outHandle.isValid() ) continue;
    ATH_MSG_DEBUG(outHandle.key() <<" with "<< outHandle->size() <<" decisions:");
    for (auto outdecision:  *outHandle){
      TrigCompositeUtils::DecisionIDContainer objDecisions;      
      TrigCompositeUtils::decisionIDs( outdecision, objDecisions );
      
      ATH_MSG_DEBUG("Number of positive decisions for this output: " << objDecisions.size() );
      
      for ( TrigCompositeUtils::DecisionID id : objDecisions ) {
	ATH_MSG_DEBUG( " ---  decision " << HLT::Identifier( id ) );
      }  
    }
  }
  

 
  return StatusCode::SUCCESS;
}

