/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// ElectronsDirectionTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (james.catmore@cern.ch)
//

#include "DerivationFrameworkEGamma/ElectronsDirectionTool.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include <vector>
#include <string>

namespace DerivationFramework {
    
    ElectronsDirectionTool::ElectronsDirectionTool(const std::string& t,
                                                         const std::string& n,
                                                         const IInterface* p) :
    AthAlgTool(t,n,p),
    m_sgEta(""),
    m_sgPhi(""),
    m_sgEt(""),
    m_sgPt(""),
    m_sgnPrecisionHits(""),
    m_collName("Electrons")
    {
        declareInterface<DerivationFramework::IAugmentationTool>(this);
	declareProperty("EtaSGEntry",m_sgEta);
	declareProperty("PhiSGEntry",m_sgPhi);
	declareProperty("EtSGEntry",m_sgEt);
	declareProperty("PtSGEntry",m_sgPt);
	declareProperty("nPrecisionHitsSGEntry",m_sgnPrecisionHits);
	declareProperty("ElectronContainer",m_collName);
    }
    
    StatusCode ElectronsDirectionTool::initialize()
    {
      if (m_sgEta.empty() && m_sgPhi.empty() && m_sgEt.empty() && m_sgPt.empty()) {
		ATH_MSG_ERROR("You are requesting the ElectronsDirectionTool but have provided no SG names for the results");
		return StatusCode::FAILURE;
	}
        return StatusCode::SUCCESS;
    }
    
    StatusCode ElectronsDirectionTool::finalize()
    {
        return StatusCode::SUCCESS;
    }
    
    StatusCode ElectronsDirectionTool::addBranches() const
    {
	
        // Retrieve data
	const xAOD::ElectronContainer* electrons =  evtStore()->retrieve< const xAOD::ElectronContainer >(m_collName);
	if( !electrons ) {
	  ATH_MSG_ERROR("Couldn't retrieve electron container with key: " << m_collName);
	  return StatusCode::FAILURE;
     	}
	
	std::vector<float> *recEta(0);
	std::vector<float> *recPhi(0);
	std::vector<float> *recEt(0);
	std::vector<float> *recPt(0);
	std::vector<int> *recnPrecisionHits(0);
	
	if (!m_sgEta.empty()) {
	  if (evtStore()->contains<std::vector<float> >(m_sgEta)) {
	    ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgEta << " which already exists. Please use a different key");
	    return StatusCode::FAILURE;
	  }
	  recEta = new std::vector<float>();
	  CHECK(evtStore()->record(recEta, m_sgEta));  
	}
       
        if (!m_sgPhi.empty()) {
	  if (evtStore()->contains<std::vector<float> >(m_sgPhi)) {
	    ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgPhi << " which already exists. Please use a different key");
	    return StatusCode::FAILURE;
	  }
	  recPhi = new std::vector<float>();
	  CHECK(evtStore()->record(recPhi, m_sgPhi));	 
	}
	
	if (!m_sgEt.empty()) {
	  if (evtStore()->contains<std::vector<float> >(m_sgEt)) {
	    ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgEt << " which already exists. Please use a different key");
	    return StatusCode::FAILURE;
	  }
	  recEt = new std::vector<float>();    
	  CHECK(evtStore()->record(recEt, m_sgEt));	 
	}
	
	if (!m_sgPt.empty()) {
	  if (evtStore()->contains<std::vector<float> >(m_sgPt)) {
	    ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgPt << " which already exists. Please use a different key");
	    return StatusCode::FAILURE;
	  }
	  recPt = new std::vector<float>();    
	  CHECK(evtStore()->record(recPt, m_sgPt));
	}
  
	if (!m_sgnPrecisionHits.empty()) {
	  if (evtStore()->contains<std::vector<float> >(m_sgnPrecisionHits)) {
	    ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgnPrecisionHits << " which already exists. Please use a different key");
	    return StatusCode::FAILURE;
	  }
	  recnPrecisionHits = new std::vector<int>();
	  CHECK(evtStore()->record(recnPrecisionHits, m_sgnPrecisionHits));
	}

        // LOOP OVER ELECTRONS
	for (xAOD::ElectronContainer::const_iterator eIt = electrons->begin(); eIt!=electrons->end(); ++eIt) {           
 
            // Prepare variables
            float mElectron(0.511); 
            float eta(0.0), phi(0.0), et(0.0),e(0.0),pt(0.0); 
            const xAOD::TrackParticle* eTrack(0);
	    const xAOD::CaloCluster* eCluster(0);
	    if ( (*eIt)->nTrackParticles() > 0) {eTrack = (*eIt)->trackParticle(0);}
	    else {continue;}
	    if ( (*eIt)->nCaloClusters() > 0) {eCluster = (*eIt)->caloCluster(0);}		
	    else {continue;}
            uint8_t numberOfPixelHits(0);
	    if( !eTrack->summaryValue(numberOfPixelHits,xAOD::numberOfPixelHits) ){
        	ATH_MSG_WARNING("Unable to retrieve numberOfPixelHits");
       	    }
	    uint8_t numberOfSCTHits(0);
	    if( !eTrack->summaryValue(numberOfSCTHits,xAOD::numberOfSCTHits) ){
                ATH_MSG_WARNING("Unable to retrieve numberOfSCTHits");
            }	
            if ( numberOfPixelHits+numberOfSCTHits >= 4.0) {
	      eta = eTrack->eta(); 
	      phi = eTrack->phi();
	      e = eCluster->e() ;
	      et = eCluster->e()/cosh(eta);
	      pt = sqrt( e*e - mElectron*mElectron )/cosh(eta);
	    } else {
	      eta = eCluster->eta();
	      phi = eCluster->phi();
	      e = eCluster->e() ;
	      et = eCluster->e()/cosh(eta);
	      pt = sqrt( e*e - mElectron*mElectron )/cosh(eta);
	    }
	    if(recEta){
	      recEta->push_back(eta);
	    }
	    if(recPhi){
	      recPhi->push_back(phi);
	    }
	    if(recEt){
	      recEt->push_back(et);
	    }
	    if(recPt){
	      recPt->push_back(pt);		            
	    }
	    if(recnPrecisionHits){
	      int nPrecisionHits(0);
	      nPrecisionHits = numberOfPixelHits + numberOfSCTHits;
	      recnPrecisionHits->push_back(nPrecisionHits);
	    }
	} // end of loop over electrons

        // Write SG for access by downstream algs
       
        return StatusCode::SUCCESS; 
    }
    
} // end of namespace

