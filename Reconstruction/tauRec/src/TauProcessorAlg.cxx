/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ListItem.h"

#include "tauRec/TauProcessorAlg.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"


#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauDefs.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauTrackAuxContainer.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "CaloInterface/ICaloCellMakerTool.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
TauProcessorAlg::TauProcessorAlg(const std::string &name,
    ISvcLocator * pSvcLocator) :
AthAlgorithm(name, pSvcLocator),
m_tools(this), //make tools private
m_maxEta(2.5),
m_minPt(10000),
m_cellMakerTool("",this)
{
  declareProperty("Tools", m_tools);
  declareProperty("MaxEta", m_maxEta);
  declareProperty("MinPt", m_minPt);
  declareProperty("CellMakerTool", m_cellMakerTool);
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
TauProcessorAlg::~TauProcessorAlg() {
}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode TauProcessorAlg::initialize() {
  ATH_CHECK( detStore()->retrieve(m_cellID) );
    
    ATH_CHECK( m_jetInputContainer.initialize() );
    ATH_CHECK( m_tauOutputContainer.initialize() );
    ATH_CHECK( m_tauTrackOutputContainer.initialize() );
    ATH_CHECK( m_tauShotClusOutputContainer.initialize() );
    ATH_CHECK( m_tauShotPFOOutputContainer.initialize() );
    ATH_CHECK( m_tauPi0CellOutputContainer.initialize() );
    ATH_CHECK( m_pixelDetEleCollKey.initialize() ); 
    ATH_CHECK( m_SCTDetEleCollKey.initialize() ); 

    ATH_CHECK( m_cellMakerTool.retrieve() );

    //-------------------------------------------------------------------------
    // No tools allocated!
    //-------------------------------------------------------------------------
    if (m_tools.size() == 0) {
        ATH_MSG_ERROR("no tools given!");
        return StatusCode::FAILURE;
    }

    //-------------------------------------------------------------------------
    // Allocate tools
    //-------------------------------------------------------------------------
    ATH_CHECK( m_tools.retrieve() );
    ToolHandleArray<ITauToolBase> ::iterator itT = m_tools.begin();
    ToolHandleArray<ITauToolBase> ::iterator itTE = m_tools.end();
    ATH_MSG_INFO("List of tools in execution sequence:");
    ATH_MSG_INFO("------------------------------------");

    unsigned int tool_count = 0;

    for (; itT != itTE; ++itT) {
      ++tool_count;
      ATH_MSG_INFO((*itT)->type() << " - " << (*itT)->name());
    }
    ATH_MSG_INFO(" ");
    ATH_MSG_INFO("------------------------------------");

    if (tool_count == 0) {
        ATH_MSG_ERROR("could not allocate any tool!");
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------
StatusCode TauProcessorAlg::finalize() {

  StatusCode sc;

  //-----------------------------------------------------------------
  // Loop stops when Failure indicated by one of the tools
  //-----------------------------------------------------------------
  ToolHandleArray<ITauToolBase> ::iterator itT = m_tools.begin();
  ToolHandleArray<ITauToolBase> ::iterator itTE = m_tools.end();
  for (; itT != itTE; ++itT) {
    ATH_MSG_VERBOSE("Invoking tool " << (*itT)->name());
    sc = (*itT)->finalize();
    if (sc.isFailure())
      break;
  }

  if (sc.isSuccess()) {
    ATH_MSG_VERBOSE("All the invoded tools are finilized successfully.");
  } 

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauProcessorAlg::execute() {
  const EventContext& ctx = Gaudi::Hive::currentContext();

    //-------------------------------------------------------------------------                         
    // Create and Record containers
    //-------------------------------------------------------------------------                 
    auto pContainer = std::make_unique<xAOD::TauJetContainer>();
    auto pAuxContainer = std::make_unique<xAOD::TauJetAuxContainer>();
    pContainer->setStore( pAuxContainer.get() );
    
    auto pTauTrackCont = std::make_unique<xAOD::TauTrackContainer>();
    auto pTauTrackAuxCont = std::make_unique<xAOD::TauTrackAuxContainer>();
    pTauTrackCont->setStore( pTauTrackAuxCont.get() );

    // Declare write handles
    SG::WriteHandle<xAOD::TauJetContainer> tauHandle( m_tauOutputContainer, ctx );
    SG::WriteHandle<xAOD::TauTrackContainer> tauTrackHandle( m_tauTrackOutputContainer, ctx );

    //---------------------------------------------------------------------                                                    
    // Retrieve seed Container from TDS, return `failure if no                                        
    // existing                                                                                                                        
    //---------------------------------------------------------------------                                                       
    SG::ReadHandle<xAOD::JetContainer> jetHandle( m_jetInputContainer, ctx );
    if (!jetHandle.isValid()) {
      ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << jetHandle.key());
      return StatusCode::FAILURE;
    }
    const xAOD::JetContainer *pSeedContainer = 0;
    pSeedContainer = jetHandle.cptr();

    // The calo cluster containter must be registered to storegate here, in order to set links in shot finder tool
    // Will still allow changes to the container within this algorithm
    SG::WriteHandle<xAOD::CaloClusterContainer> tauShotClusHandle( m_tauShotClusOutputContainer, ctx );
    xAOD::CaloClusterContainer* tauShotClusContainer = new xAOD::CaloClusterContainer();
    xAOD::CaloClusterAuxContainer* tauShotClusAuxStore = new xAOD::CaloClusterAuxContainer();
    tauShotClusContainer->setStore(tauShotClusAuxStore);
    ATH_MSG_DEBUG("  write: " << tauShotClusHandle.key() << " = " << "..." );
    ATH_CHECK(tauShotClusHandle.record(std::unique_ptr<xAOD::CaloClusterContainer>{tauShotClusContainer}, std::unique_ptr<xAOD::CaloClusterAuxContainer>{tauShotClusAuxStore}));

    SG::WriteHandle<xAOD::PFOContainer> tauShotPFOHandle( m_tauShotPFOOutputContainer, ctx );
    xAOD::PFOContainer* tauShotPFOContainer = new xAOD::PFOContainer();
    xAOD::PFOAuxContainer* tauShotPFOAuxStore = new xAOD::PFOAuxContainer();
    tauShotPFOContainer->setStore(tauShotPFOAuxStore);
    ATH_MSG_DEBUG("  write: " << tauShotPFOHandle.key() << " = " << "..." );
    ATH_CHECK(tauShotPFOHandle.record(std::unique_ptr<xAOD::PFOContainer>{tauShotPFOContainer}, std::unique_ptr<xAOD::PFOAuxContainer>{tauShotPFOAuxStore}));

    SG::WriteHandle<CaloCellContainer> tauPi0CellHandle( m_tauPi0CellOutputContainer, ctx );
    CaloCellContainer* Pi0CellContainer = new CaloCellContainer();
    ATH_MSG_DEBUG("  write: " << tauPi0CellHandle.key() << " = " << "..." );
    ATH_CHECK(tauPi0CellHandle.record(std::unique_ptr<CaloCellContainer>(Pi0CellContainer)));

    //---------------------------------------------------------------------                                                        
    // Loop over seeds
    //---------------------------------------------------------------------                                                 
    xAOD::JetContainer::const_iterator itS = pSeedContainer->begin();
    xAOD::JetContainer::const_iterator itSE = pSeedContainer->end();

    ATH_MSG_VERBOSE("Number of seeds in the container: " << pSeedContainer->size());
    
    /// Initialize the cell map per event, used to avoid dumplicate cell  in TauPi0CreateROI
    IdentifierHash hashMax = m_cellID->calo_cell_hash_max(); 
    ATH_MSG_DEBUG("CaloCell Hash Max: " << hashMax);
    std::vector<CaloCell*> addedCellsMap;
    addedCellsMap.resize(hashMax,NULL);

    for (; itS != itSE; ++itS) {

      const xAOD::Jet *pSeed = (*itS);
      ATH_MSG_VERBOSE("Seeds eta:" << pSeed->eta() << ", pt:" << pSeed->pt());

      if (fabs(pSeed->eta()) > m_maxEta) {
	ATH_MSG_VERBOSE("--> Seed rejected, eta out of range!");
	continue;
      }

      if (fabs(pSeed->pt()) < m_minPt) {
	ATH_MSG_VERBOSE("--> Seed rejected, pt out of range!");
	continue;
      }

      //-----------------------------------------------------------------                                                                 
      // Seed passed cuts --> create tau candidate
      //-----------------------------------------------------------------                                                                           
      xAOD::TauJet* pTau = new xAOD::TauJet();
      pContainer->push_back( pTau );
      pTau->setJet(pSeedContainer, pSeed);

      // This sets one track and link. Need to have at least 1 track linked to retrieve track container
      setEmptyTauTrack(pTau, pTauTrackCont.get());
      
      //-----------------------------------------------------------------
      // Loop stops when Failure indicated by one of the tools
      //-----------------------------------------------------------------
      StatusCode sc;
      ToolHandleArray<ITauToolBase> ::iterator itT = m_tools.begin();
      ToolHandleArray<ITauToolBase> ::iterator itTE = m_tools.end();
      for (; itT != itTE; ++itT) {
	ATH_MSG_DEBUG("ProcessorAlg Invoking tool " << (*itT)->name());

        if ((*itT)->type() == "TauVertexFinder" ) { 
          sc = (*itT)->executeVertexFinder(*pTau);
        }
        else if ( (*itT)->type() == "TauTrackFinder") { 
          sc = (*itT)->executeTrackFinder(*pTau);
        }
        else if ( (*itT)->name().find("ShotFinder") != std::string::npos){
	  sc = (*itT)->executeShotFinder(*pTau, *tauShotClusContainer, *tauShotPFOContainer);
	}
	else if ( (*itT)->name().find("Pi0ClusterFinder") != std::string::npos){
	  sc = (*itT)->executePi0CreateROI(*pTau, *Pi0CellContainer, addedCellsMap);
	}
	else {
	  sc = (*itT)->execute(*pTau);
	}
	if (sc.isFailure())
	  break;
      }

      if (sc.isSuccess()) {
	ATH_MSG_VERBOSE("The tau candidate has been registered");
      } 
      else {
	//remove orphaned tracks before tau is deleted via pop_back
	xAOD::TauJet* bad_tau = pContainer->back();
	ATH_MSG_DEBUG("Deleting " << bad_tau->nAllTracks() << "Tracks associated with tau: ");
	pTauTrackCont->erase(pTauTrackCont->end()-bad_tau->nAllTracks(), pTauTrackCont->end());

	pContainer->pop_back();
      } 
    }// loop through seeds

    // Check this is needed for the cell container?
    // symlink as INavigable4MomentumCollection (as in CaloRec/CaloCellMaker)
    ATH_CHECK(evtStore()->symLink(Pi0CellContainer, static_cast<INavigable4MomentumCollection*> (0)));
    
    // sort the cell container by hash
    ATH_CHECK( m_cellMakerTool->process(static_cast<CaloCellContainer*> (Pi0CellContainer), ctx) );

  ATH_MSG_VERBOSE("The tau candidate container has been modified");
  
  // Write the completed tau and track containers
  ATH_MSG_DEBUG("  write: " << tauHandle.key() << " = " << "..." );
  ATH_CHECK(tauHandle.record( std::move(pContainer), std::move(pAuxContainer) ));
  ATH_MSG_DEBUG("  write: " << tauTrackHandle.key() << " = " << "..." );  
  ATH_CHECK(tauTrackHandle.record( std::move(pTauTrackCont), std::move(pTauTrackAuxCont) ));

  return StatusCode::SUCCESS;
}


void TauProcessorAlg::setEmptyTauTrack(xAOD::TauJet* &pTau, 
				       xAOD::TauTrackContainer* tauTrackContainer)
{  
  // Make a new tau track, add to container
  xAOD::TauTrack* pTrack = new xAOD::TauTrack();
  tauTrackContainer->push_back(pTrack);
    
  // Create an element link for that track
  ElementLink<xAOD::TauTrackContainer> linkToTauTrack;
  linkToTauTrack.toContainedElement(*tauTrackContainer, pTrack);
  pTau->addTauTrackLink(linkToTauTrack);
}

