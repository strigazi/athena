/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetHypoAllTE.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHORS:   John Alison   johnda@uchicago.edu
// 
// ************************************************

#include "TrigBjetHypo/TrigBjetHypoAllTE.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBase/IParticleContainer.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

using std::vector;


TrigBjetHypoAllTE::TrigBjetHypoAllTE(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::AllTEAlgo(name, pSvcLocator)
{
  declareProperty("BTaggingKey", m_btaggingKey      = "");
  declareProperty ("UseBeamSpotFlag",       m_useBeamSpotFlag       = false);
  declareProperty ("OverrideBeamSpotValid", m_overRideBeamSpotValid = false);
  declareProperty ("Tagger",                m_tagger = "MV2c10_discriminant");

  declareProperty("EtThresholds",   m_EtThresholds   ); 
  declareProperty("BTagMin",        m_BTagMin       );
  declareProperty("BTagMax",        m_BTagMax       );
  declareProperty("Multiplicities", m_Multiplicities );

  declareProperty("EtThresholdsOR",   m_EtThresholdsOR   ); 
  declareProperty("BTagMinOR",        m_BTagMinOR       );
  declareProperty("BTagMaxOR",        m_BTagMaxOR       );
  declareProperty("MultiplicitiesOR", m_MultiplicitiesOR );

  declareMonitoredVariable("CutCode",    m_cutCode);
  declareMonitoredVariable("BSCode",     m_BSCode);
  declareMonitoredVariable("nInputJets", m_nInputJets);
}


//
// Destruction
//
TrigBjetHypoAllTE::~TrigBjetHypoAllTE() {}


//
// hltInitialize
//
HLT::ErrorCode TrigBjetHypoAllTE::hltInitialize() {

  if (msgLvl() <= MSG::INFO) msg() << MSG::INFO << "Initializing TrigBjetHypoAllTE" << endmsg;
    
  //
  // declareProperty overview
  //
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endmsg;
    msg() << MSG::DEBUG << " BTaggingKey = "   << m_btaggingKey << endmsg;
  }

  //
  //  Configure the AND requrements
  //

  unsigned int nRequirementsAND = m_EtThresholds.size();

  if(nRequirementsAND != m_BTagMin.size()){
    msg() << MSG::ERROR << "Et and btagging requirement have different sizes! Please Fix" << endmsg;
    return HLT::ERROR;
  }

  if(nRequirementsAND != m_BTagMax.size()){
    msg() << MSG::ERROR << "Et and max btagging requirement have different sizes! Please Fix" << endmsg;
    return HLT::ERROR;
  }

  if(nRequirementsAND != m_Multiplicities.size()){
    msg() << MSG::ERROR << "Et and multiplicities requirement have different sizes! Please Fix" << endmsg;
    return HLT::ERROR;
  }

  for(unsigned int iReq = 0; iReq < nRequirementsAND; ++iReq){
    m_triggerReqsAND.push_back(triggerRequirement(m_EtThresholds.at(iReq), m_BTagMin.at(iReq), m_BTagMax.at(iReq), m_Multiplicities.at(iReq)));
  }


  //
  //  Configure the OR
  //
  unsigned int nRequirementsOR = m_EtThresholdsOR.size();

  if(nRequirementsOR != m_BTagMinOR.size()){
    msg() << MSG::ERROR << "Et and btagging OR requirement have different sizes! Please Fix" << endmsg;
    return HLT::ERROR;
  }

  if(nRequirementsOR != m_BTagMaxOR.size()){
    msg() << MSG::ERROR << "Et and max btagging OR requirement have different sizes! Please Fix" << endmsg;
    return HLT::ERROR;
  }

  if(nRequirementsOR != m_MultiplicitiesOR.size()){
    msg() << MSG::ERROR << "Et and multiplicities OR requirement have different sizes! Please Fix" << endmsg;
    return HLT::ERROR;
  }

  for(unsigned int iOR = 0 ; iOR < nRequirementsOR; ++iOR){
    unsigned int nSubRequirement = m_EtThresholdsOR.at(iOR).size();    

    if(nSubRequirement != m_BTagMinOR.at(iOR).size()){
      msg() << MSG::ERROR << "Et and btagging OR sub requirement have different sizes! Please Fix" << endmsg;
      return HLT::ERROR;
    }

    if(nSubRequirement != m_BTagMaxOR.at(iOR).size()){
      msg() << MSG::ERROR << "Et and max btagging OR sub requirement have different sizes! Please Fix" << endmsg;
      return HLT::ERROR;
    }

    if(nSubRequirement != m_MultiplicitiesOR.at(iOR).size()){
      msg() << MSG::ERROR << "Et and multiplicities OR sub requirement have different sizes! Please Fix" << endmsg;
      return HLT::ERROR;
    }

    m_triggerReqsOR.push_back(vector<triggerRequirement>());
    msg() << MSG::DEBUG << "TrigBjetHypoAllTE::"  << name() << " or Requirements " << endmsg;
    for(unsigned int iSub = 0 ; iSub < nSubRequirement; ++iSub){
      m_triggerReqsOR.back().push_back(triggerRequirement(m_EtThresholdsOR  .at(iOR).at(iSub), 
							  m_BTagMinOR       .at(iOR).at(iSub), 
							  m_BTagMaxOR       .at(iOR).at(iSub), 
							  m_MultiplicitiesOR.at(iOR).at(iSub)));
      msg() << MSG::DEBUG << "\t (" 
	    << m_EtThresholdsOR.at(iOR).at(iSub) 
	    << " " << m_BTagMinOR.at(iOR).at(iSub) 
	    << " " << m_BTagMaxOR.at(iOR).at(iSub) 
	    << " " << int(m_MultiplicitiesOR.at(iOR).at(iSub) )
	    << ")";
    }
    msg() << MSG::DEBUG << endmsg;
  }



  return HLT::OK;
}


//
//  hltExecute
//
HLT::ErrorCode TrigBjetHypoAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE, unsigned int output) {

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Executing TrigBjetHypoAllTE" << endmsg;

  beforeExecMonitors().ignore();

  //
  //  Init to invalid numbers (These should all be set to != -1 later) 
  //
  m_cutCode      = -1;
  m_BSCode       = -1;
  m_nInputJets   = -1;

  bool pass = false;

  //
  // Retrieve beamspot information 
  //
  if (m_useBeamSpotFlag) {

    IBeamCondSvc* iBeamCondSvc; 
    StatusCode sc = service("BeamCondSvc", iBeamCondSvc);
    
    if (sc.isFailure() || iBeamCondSvc == 0) {
      if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Could not retrieve Beam Conditions Service. " << endmsg;
      m_BSCode = 3;
    } else {

      int beamSpotStatus = 0;
      int beamSpotBitMap = iBeamCondSvc->beamStatus();    

      beamSpotStatus = ((beamSpotBitMap & 0x4) == 0x4);  
      if (beamSpotStatus) beamSpotStatus = ((beamSpotBitMap & 0x3) == 0x3);
      
      if (!beamSpotStatus){
	if (!m_overRideBeamSpotValid) {
	  m_BSCode = 1;
	  m_cutCode    = 2;
	  pass = false;
	  return HLT::OK;
	} else{
	  m_BSCode = 2;
	}
      }else{
	m_BSCode = 0;
      }

    }
  } else {
    m_BSCode = 4;
  }

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG  << "Number of input TEs is " <<  inputTE.size() << endmsg;


  if (inputTE.size() < 1) {
    msg() << MSG::WARNING << "Number of input TEs is " <<  inputTE.size() << " and not 1. Configuration problem." << endmsg;  
    afterExecMonitors().ignore();
    m_cutCode    = 3;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }

  if (inputTE.size() > 2) {
    msg() << MSG::WARNING << "Too many TEs passed as input" << endmsg;
    afterExecMonitors().ignore();
    m_cutCode    = 4;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }


  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG 
				    << " TE0: " << inputTE.at(0).size() 
				    << " TE1: " << inputTE.at(1).size() <<  endmsg;  

  //
  // Retrieve the BTagging container
  //
  std::vector<HLT::TriggerElement*>& btaggingTEs = inputTE.at(0); 

  if (btaggingTEs.size() == 0) {
    msg() << MSG::WARNING << "Got an empty inputTE (btagging)" << endmsg;
    afterExecMonitors().ignore();
    m_cutCode    = 5;
    return HLT::MISSING_FEATURE; 
  }

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " btaggingTE.size() " << btaggingTEs.size() << endmsg;

  //
  // Do the multiplicity requrements
  //
  clearCounters();
  
  for(HLT::TriggerElement* btagTE : btaggingTEs){

    const xAOD::BTagging* btagInfo = getBTaggingPtr(btagTE);    
    if (!btagInfo) {
      if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to retrieve features (btagging)" << endmsg;
      m_cutCode    = 6;
      return HLT::MISSING_FEATURE;
    }

    const xAOD::Jet* jet = getJetPtr(btagInfo);
    if(!jet) {
      if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (jet)." << endmsg;
      m_cutCode    = 7;
      return HLT::MISSING_FEATURE;
    }
    
    double btagWeight = btagInfo->auxdata<double>(m_tagger);  
    double  btagEt     = jet->p4().Et();
    for(triggerRequirement& trigReq:  m_triggerReqsAND){
      trigReq.countJet(btagEt, btagWeight);
    }

    for(vector<triggerRequirement>& ORReq:  m_triggerReqsOR){
      for(triggerRequirement& subORReq:  ORReq){
	subORReq.countJet(btagEt, btagWeight);
      }
    }

    //
    // Check if event passes after each jet
    //  If so, pass trigger and move on
    //
    if(eventPassesTrigger()){
      pass = true;
      break;
    }

  }
  
  if (pass) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Accepting the event" << endmsg;
    m_cutCode    = 1; 
  } else {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Rejecting the event" << endmsg;
    m_cutCode    = 0; 
  }
  

  //
  // Setting the output TE
  //
  HLT::TEVec allTEs;
  allTEs.reserve(btaggingTEs.size());
  if ((inputTE.size()>0)){
    for(HLT::TriggerElement* btagTE : btaggingTEs){
      allTEs.push_back(btagTE);
    }
  }

  //
  // monitoring
  //
  m_nInputJets = btaggingTEs.size();

  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, output);
  outputTE->setActiveState(false); 

  if (pass==true){
    outputTE->setActiveState(true);
  }
 
  afterExecMonitors().ignore();

  return HLT::OK;  
}

//
// Check event decision
//
bool TrigBjetHypoAllTE::eventPassesTrigger(){
  bool passAND = true;
  for(triggerRequirement& trigReq:  m_triggerReqsAND){
    if(!trigReq.pass())
      passAND = false;
  }

  bool passOR = false;
  if(!m_triggerReqsOR.size()){
    passOR = true;
  }else{
    for(vector<triggerRequirement>& ORReq:  m_triggerReqsOR){
      bool passThisOR = true;
      for(triggerRequirement& subORReq:  ORReq){
	if(!subORReq.pass())
	  passThisOR = false;
      }
      if(passThisOR) passOR = true;
    }
  }
  
  return passAND && passOR;
}

//
// Clear counters
//
void TrigBjetHypoAllTE::clearCounters(){

  for(triggerRequirement& trigReq:  m_triggerReqsAND){
    trigReq.m_count = 0;
  }

  for(vector<triggerRequirement>& ORReq:  m_triggerReqsOR){
    for(triggerRequirement& subORReq:  ORReq){
      subORReq.m_count = 0;
    }
  }

}

//
// hltFinalize
//
HLT::ErrorCode TrigBjetHypoAllTE::hltFinalize() {

  msg() << MSG::INFO << "Finalizing TrigBjetHypoAllTE" << endmsg;

  return HLT::OK;
}


//
// hltEndEvent
//
HLT::ErrorCode TrigBjetHypoAllTE::hltEndEvent() {

  if ( !AllTEAlgo::hltEndEvent() ) return false;
  AllTEAlgo::hltEndEvent();

  return true;
}



//
// Retrieve the btagging info
//
const xAOD::BTagging* TrigBjetHypoAllTE::getBTaggingPtr(HLT::TriggerElement* btagTE) {

  const xAOD::BTaggingContainer* bTaggingContainer=0;
  HLT::ErrorCode statusBTagging = getFeature(btagTE, bTaggingContainer, m_btaggingKey); 

  if (statusBTagging != HLT::OK) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to retrieve features (btagging)" << endmsg;
    return nullptr;
  }

  if(bTaggingContainer==0) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (btagging)." << endmsg;
    return nullptr;
  }

  if(bTaggingContainer->size() != 1) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Error btagging container size " << bTaggingContainer->size() << endmsg;
    return nullptr;
  }

  return bTaggingContainer->at(0);
}

//
// Retrieve the jet info
//
const xAOD::Jet* TrigBjetHypoAllTE::getJetPtr(const xAOD::BTagging* btagInfo) {
  
  std::vector< ElementLink< xAOD::IParticleContainer > >  matchedJets = btagInfo->auxdata< std::vector< ElementLink< xAOD::IParticleContainer > > >("BTagBtagToJetAssociator");  
      
  if(matchedJets.size() != 1){
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (jet element link) size=" << matchedJets.size() << endmsg;
    return nullptr;
  }

  const xAOD::IParticleContainer* jetCol = matchedJets.at(0).getStorableObjectPointer();
  if(jetCol->size() != 1){
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (IParticleContainer) size=" << jetCol->size() << endmsg;
    return nullptr;
  }
      
  const xAOD::JetContainer* jets = dynamic_cast<const xAOD::JetContainer*>(jetCol);
  if(!jets){
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (jetContainer) size=" << jets->size() << endmsg;
    return nullptr;
  }
    
  if(jets->size() != 1){
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (jets) size=" << jets->size() << endmsg;
    return nullptr;
  }
  
  return jets->at(0);
}
