/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibUtils/L1CaloTriggerTowerSelector.h"

#include "TrigT1CaloEvent/TriggerTowerCollection.h"

L1CaloTriggerTowerSelector::L1CaloTriggerTowerSelector(const std::string& name, ISvcLocator* pSvcLocator): Algorithm(name, pSvcLocator), m_storeGate(0), m_towerKey(0)
{
    //setting selection range to the whole TTs by default
    m_vEtaSelectionRange.push_back(-5.);
    m_vEtaSelectionRange.push_back(5.);

    m_vPhiSelectionRange.push_back(0.);
    m_vPhiSelectionRange.push_back(7.);

    declareProperty("OutputTriggerTowerLocation", m_inputTriggerTowerLocation = "LVL1TriggerTowers");
    declareProperty("InputTriggerTowerLocation", m_outputTriggerTowerLocation = "SelectedLVL1TriggerTowers");

    declareProperty("EtaSelectionRange", m_vEtaSelectionRange);
    declareProperty("PhiSelectionRange", m_vPhiSelectionRange);
}

L1CaloTriggerTowerSelector::~L1CaloTriggerTowerSelector()
{
	 delete m_towerKey;
}

//-------------------------------------------
// Initialize
//-------------------------------------------
StatusCode L1CaloTriggerTowerSelector::initialize()
{
    MsgStream log( msgSvc(), name() );
    log << MSG::INFO <<"From Initialize..."<<endreq;

    StatusCode sc;

    //get a pointer to Event StoreGate services
    sc = service("StoreGateSvc", m_storeGate);
    if (sc.isFailure()) {
        log << MSG::ERROR << "Cannot access StoreGate" << endreq;
        return StatusCode::FAILURE;
    }

    m_towerKey = new LVL1::TriggerTowerKey();

    return StatusCode::SUCCESS;
}

//----------------------------------------
// Execute
//----------------------------------------
StatusCode L1CaloTriggerTowerSelector::execute()
{
    MsgStream log( msgSvc(), name() );
    StatusCode sc;

    // retrieve triggertowers container from storegate
    const TriggerTowerCollection* ttCollection = 0;
    sc = m_storeGate->retrieve(ttCollection, m_inputTriggerTowerLocation);
    if (sc.isFailure() || !ttCollection) {
        log << MSG::ERROR << "No Trigger Towers found" << endreq;
        return StatusCode::SUCCESS;
    } else log << MSG::DEBUG <<"TriggerTowerCollection retrieved from StoreGate, size: "<< ttCollection->size() <<endreq;

	// create TriggerTowerCollection that will hold the selected TT
	TriggerTowerCollection* outputTTCollection = new TriggerTowerCollection();

    //---
    //loop on trigger towers
    TriggerTowerCollection::const_iterator pos = ttCollection->begin();
    TriggerTowerCollection::const_iterator pos_end = ttCollection->end();

		for(;pos!=pos_end;++pos) {
 	  	LVL1::TriggerTower* ptt = (*pos);

			// Compute TowerTrigger Key out of its coordinates
			double eta = ptt->eta();
	    double phi = ptt->phi();

	    if (this->isSelected(eta, phi)) {
	    	LVL1::TriggerTower* selectedTriggerTower = new LVL1::TriggerTower(*ptt);
	    	outputTTCollection->push_back(selectedTriggerTower);
	    }

  	} // -- End of trigger tower loop --

  	sc = m_storeGate->record(outputTTCollection, m_outputTriggerTowerLocation);
  	if (sc.isFailure()) {
    	log << MSG::ERROR << "Failed to record output TriggerTowerCollection to TDS with key: "<< m_outputTriggerTowerLocation << endreq;
    	return StatusCode::SUCCESS;
  	} else log << MSG::DEBUG <<"TriggerTowerCollection recorded to TDS with key: "<< m_outputTriggerTowerLocation<<", size: "<< outputTTCollection->size() <<endreq;

    return StatusCode::SUCCESS;
}

//-----------------------------------------
// Finalize
//-----------------------------------------
StatusCode L1CaloTriggerTowerSelector::finalize()
{
    return StatusCode::SUCCESS;
}

bool L1CaloTriggerTowerSelector::isSelected(double eta, double phi) {
	return (eta<m_vEtaSelectionRange.at(1)&&eta>m_vEtaSelectionRange.at(0))&&(phi<m_vPhiSelectionRange.at(1)&&phi>m_vPhiSelectionRange.at(0));
}
