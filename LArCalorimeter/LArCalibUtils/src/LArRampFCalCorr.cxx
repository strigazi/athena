/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//
// LArRampFCalCorr.cxx - Algorithm to normalize FCal Ramps and correct 
//                       for baseplane faults.
//
// Author: Alan Robinson <fbfree@interchange.ubc.ca>
// Created: August 28, 2007
//
//***************************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "Identifier/IdentifierHash.h"

#include "CaloIdentifier/CaloGain.h"

#include "LArCalibUtils/LArRampFCalCorr.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRawConditions/LArRampComplete.h"

#include "StoreGate/StoreGateSvc.h"
#include <cmath>
using std::pow;

#include <cmath>

LArRampFCalCorr::LArRampFCalCorr(const std::string& name,ISvcLocator* pSvcLocator)
  :Algorithm(name, pSvcLocator),
   m_detStore(0),
   m_onlineHelper(0)
{
  declareProperty("Threshold", m_threshold = 1.0);  // Baseplane problem threshold at HIGH gain.
}

LArRampFCalCorr::~LArRampFCalCorr() {}

StatusCode LArRampFCalCorr::initialize(){
  // acquire LArRampComplete
  MsgStream  log(msgSvc(),name());
  log << MSG::DEBUG << " in initialize() " <<endreq;

  StatusCode sc = service("DetectorStore", m_detStore);
  if (sc.isFailure()) {
    log << MSG::FATAL << " Cannot locate DetectorStore " << endreq;
    return StatusCode::FAILURE;
  }

  // Online Helper
  sc = m_detStore->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    log <<MSG::ERROR <<"Could not get LArOnlineID helper"<<endreq;
    return sc;
  }
  return StatusCode::SUCCESS;
}

StatusCode LArRampFCalCorr::execute() {return StatusCode::SUCCESS;}

StatusCode LArRampFCalCorr::stop(){

  MsgStream  log(msgSvc(),name());
  log << MSG::DEBUG << " in stop() " <<endreq;

  const LArRampComplete* ramp;
  StatusCode sc = m_detStore->retrieve(ramp);
  if (sc.isFailure() || !ramp) {
    log << MSG::WARNING << "Unable to retrieve LArRampComplete from DetectorStore" << endreq;
    return sc;
  }
  else log << MSG::DEBUG << "found LarRampComplete" << endreq;

  

  HWIdentifier chid;
  std::vector<int> badChan;
  int channel, module;
  int slot = 0;
  double avg[3];
  int numChan[3];
  // float gainRatio[3] = {1, 9.3, 93}; // Gain ratio high:medium:low

  for ( int gain = 0 ; gain < 3 ; gain++ ){ // loop on possible gains
    badChan.clear();

    for (int i=0; i<3; i++){
      avg[i] = 0.; numChan[i] = 0;
    }

  // Flag bad channels
    for ( LArRampIt it = ramp->begin(gain); it != ramp->end(gain); ++it) {
      chid = it.channelId();
      module = this->toMod(slot);
      if (it->m_vRamp.size() != 2 || !m_onlineHelper->isFCALchannel(chid)
         || module == -1) continue;
      slot = m_onlineHelper->slot(chid);
      channel = m_onlineHelper->channel(chid);
      log << MSG::VERBOSE << slot << " " << channel << endreq;
      if (it->m_vRamp[1] > m_threshold*std::pow(10.0,(int)gain)){ // Note: ramp contains inverse of slope.  Therefore, baseplane problems have a high value.
	log << MSG::DEBUG << "Bad chan: slot" << slot << " chan: " << channel
           << " amp " << it->m_vRamp[1] << endreq;
        badChan.push_back(channel); // Record baseplane faults.
      }
    }

  // Create normalization average
    for ( LArRampIt it = ramp->begin(gain); it != ramp->end(gain); ++it) {
      chid = it.channelId();
      module = this->toMod(slot);
      if (it->m_vRamp.size() != 2 || !m_onlineHelper->isFCALchannel(chid)
         || module == -1) continue;
      slot = m_onlineHelper->slot(chid);
      channel = m_onlineHelper->channel(chid);
      if (std::find(badChan.begin(), badChan.end(), channel)==badChan.end()){ // If this calib line doen't have a baseplane fault.
        avg[module] += 1.0 / it->m_vRamp[1];
        numChan[module]++;
      }
    }

    for (int i=0; i<3; i++) // Average slope over each module
      if (numChan[i] != 0 && avg[i] != 0.0)
        avg[i] /= numChan[i]; // avg[i] *= gainRatio[gain] / numChan[i];
      else
        avg[i] = 1.0; // avg[i] = gainRatio[gain];

    log << MSG::DEBUG << "Averages modules 1: " << avg[0]  << " 2: " << avg[1] 
       << " 3: " << avg[2] << endreq;
    log << MSG::DEBUG << "NumChan modules 1: " << numChan[0]  << " 2: " << numChan[1]
       << " 3: " << numChan[2] << endreq;

  // Apply corrections
    for ( LArRampIt it = ramp->begin(gain); it != ramp->end(gain); ++it) {
      chid = it.channelId();
      module = this->toMod(slot);
      if (it->m_vRamp.size() == 2 && m_onlineHelper->isFCALchannel(chid) && module != -1){
        //LArRampP& rampP = const_cast<LArRampP&> (*it); // avoid direct use of payload object!
        LArRampComplete::LArCondObj& rampP = const_cast<LArRampComplete::LArCondObj&> (*it);
	slot = m_onlineHelper->slot(chid);
        channel = m_onlineHelper->channel(chid);
        if (std::find(badChan.begin(), badChan.end(), channel)==badChan.end())
          rampP.m_vRamp[1] *= avg[module];  // Normalize the ramp
        else if (rampP.m_vRamp[1] > m_threshold*pow(10.,gain))
          rampP.m_vRamp[1] = 1.0;  // Write default value
        else
          rampP.m_vRamp[1] *= 1.027 * avg[module];  // Correct the amplitude and normalize
      } else
	log << MSG::DEBUG << "Channel 0x" << std::hex << chid << std::dec
           << " Slot " << slot << " Chan " << channel
           << " No normalization applied." << endreq;
      //larRampComplete->set(chid, gain, rampP.m_vRamp);  // Rerecords all channels.  Including those not normalized
    }
  } // end loop over gains
  log << MSG::INFO << "Completed LArRampFCalCorr" << endreq;

  return StatusCode::SUCCESS;
}

// Map slot number to an FCal module
int LArRampFCalCorr::toMod(int& slot){
  if (slot == 9 || (slot > 0 && slot < 8))
    return 0;
  else if (slot >= 10 && slot < 14)
    return 1;
  else if (slot == 14 || slot == 15)
    return 2;
  else return -1;
}
