/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFHLTHTHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigEFCaloHypo
//
// AUTHOR: Anna Sfyrla, Josh McFayden
//
// ********************************************************************
//
#include <list>
#include <iterator>
#include <sstream>
#include <cmath>
#include <string>
#include <vector>

//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"

//#include "TrigConfHLTData/HLTTriggerElement.h"

// #include "TrigSteeringEvent/TriggerElement.h"
// #include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigHLTJetHypo/TrigEFHLTHTHypo.h"

// #include "JetEvent/JetCollection.h"
// #include "JetEvent/Jet.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

//PS #include "FourMomUtils/P4DescendingSorters.h"
#include "CLHEP/Units/SystemOfUnits.h"

//PS #include "TrigSteeringEvent/TrigPassBits.h"
#include "xAODTrigger/TrigPassBits.h"


class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFHLTHTHypo::TrigEFHLTHTHypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  m_timersvc = nullptr;

  m_accepted=0;
  m_rejected=0;
  m_errors=0;

  declareProperty("Etcut",        m_EtCut        = 30*CLHEP::GeV  );   // Default: 30 GeV
  declareProperty("HTcut",        m_HTCut        = 350*CLHEP::GeV );   // Default: 350 GeV
  declareProperty("etaMincut",    m_etaMinCut    = 0.     );   // Default: 0
  declareProperty("etaMaxcut",    m_etaMaxCut    = 3.2    ); // Default: 3.2 - central jets
  declareProperty("doMonitoring", m_doMonitoring = false  );

  // Monitored variables...
  declareMonitoredVariable("cutCounter"  , m_cutCounter  );
  
  
  declareMonitoredStdContainer("phi"	 , m_jetphi      );
  declareMonitoredStdContainer("et" 	 , m_jetet       );
  declareMonitoredStdContainer("eta"	 , m_jeteta      );

  declareMonitoredStdContainer("passedJetCuts_phi", m_passedJetCuts_jetphi  );
  declareMonitoredStdContainer("passedJetCuts_eta", m_passedJetCuts_jeteta  );
  declareMonitoredStdContainer("passedJetCuts_et" , m_passedJetCuts_jetet   );

  
  declareMonitoredVariable("phi1"	 , m_jetphi1      );
  declareMonitoredVariable("et1" 	 , m_jetet1       );
  declareMonitoredVariable("eta1"	 , m_jeteta1      );
  declareMonitoredVariable("phi2"	 , m_jetphi2      );
  declareMonitoredVariable("et2" 	 , m_jetet2       );
  declareMonitoredVariable("eta2"	 , m_jeteta2      );
  declareMonitoredVariable("HT"	         , m_HT           );
  declareMonitoredVariable("HT_passedHT" , m_HT_passedHT  );
  declareMonitoredVariable("nJets"	         , m_nJets            );
  declareMonitoredVariable("nJets_passedCuts"	 , m_nJets_passedCuts );
  declareMonitoredVariable("nJets_passedHT"	 , m_nJets_passedHT   );
  declareMonitoredVariable("nJets_failedHT"	 , m_nJets_failedHT   );

}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFHLTHTHypo::~TrigEFHLTHTHypo()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFHLTHTHypo::hltInitialize()
  // ----------------------------------------------------------------------
{
  ATH_MSG_VERBOSE("in initialize()");

  // Initialize timing service
  //------------------------------
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
    ATH_MSG_WARNING(name() << ": Unable to locate TrigTimer Service");
  }
  if (m_timersvc) {

    TrigTimer* tmp = m_timersvc->addItem("TrigEFHLTHTHypo.TrigEFHLTHTHypoTot");
    m_timers.push_back(tmp);
  }

  ATH_MSG_DEBUG("Initialization:");
  ATH_MSG_DEBUG("ptJetCut: " << m_EtCut);

  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFHLTHTHypo::hltFinalize(){
  // ----------------------------------------------------------------------

  ATH_MSG_VERBOSE("in finalize()");
  ATH_MSG_VERBOSE("Events accepted/rejected/errors:  "
               << m_accepted <<" / "<<m_rejected<< " / "<< m_errors);
  return HLT::OK;
}

struct DescendingEt:std::binary_function<const xAOD::Jet*,
                                         const xAOD::Jet*,
                                         bool> {
  bool operator()(const xAOD::Jet* l, const xAOD::Jet* r)  const {
    return l->p4().Et() > r->p4().Et();
  }
};

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFHLTHTHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  // ----------------------------------------------------------------------


  // Time total TrigEFHLTHTHypo execution time.
  // -------------------------------------
  //  if (m_timersvc) m_timers[0]->start();

  m_cutCounter = -1;

  pass=false;
  
  const xAOD::JetContainer* outJets(0);
  HLT::ErrorCode ec = getFeature(outputTE, outJets);

  if(ec!=HLT::OK) {
    ATH_MSG_WARNING(" Failed to get JetContainer");
    return ec;
  }else{
    ATH_MSG_DEBUG("Obtained JetContainer");
  }

  m_cutCounter = 0;

  if(outJets == 0){
    ATH_MSG_WARNING("JetContainer pointer is 0");
    m_errors++;
    return HLT::ERROR;
  }

  //check size of JetCollection not empty
  if( outJets->size() == 0){
    ATH_MSG_DEBUG("JetContainer is empty");
    m_errors++;
    return HLT::OK;
  }

  ///  now add the TrigPassBits for the jets to specify whether each jet 
  ///  passes the hypo etc
  // TrigPassBits* bits = HLT::makeTrigPassBits(outJets);
  std::unique_ptr<xAOD::TrigPassBits> xBits = 
    xAOD::makeTrigPassBits<xAOD::JetContainer>(outJets);

  // JetCollection -> jetcolletcion_t
  std::vector<const xAOD::Jet*> theJets(outJets->begin(), outJets->end());

  m_cutCounter = 1;


  m_nJets            = 0   ;
  m_nJets_passedCuts = 0   ;
  m_nJets_passedHT   = 0   ;
  m_nJets_failedHT   = 0   ;

  m_HT            = -999.;
  m_HT_passedHT   = -999.;

  m_jetet1        = -999; 
  m_jetphi1       = -999;
  m_jeteta1       = -999;

  m_jetet2        = -999; 
  m_jetphi2       = -999;
  m_jeteta2       = -999;

  //clear vectors
  m_jetphi.clear();
  m_jetet.clear();
  m_jeteta.clear();
  
  m_passedJetCuts_jetphi.clear();
  m_passedJetCuts_jeteta.clear();
  m_passedJetCuts_jetet.clear();
  
  
  //Ordering Jets
  std::sort (theJets.begin(), theJets.end(), DescendingEt());

  for (const xAOD::Jet* aJet : theJets) {
    double jetet    = -999; 
    double jetphi   = -999;
    double jeteta   = -999;
    
    jetet  = aJet->p4().Et();
    jetphi = aJet->phi();
    jeteta = aJet->eta();
    
    //for monitoring
    m_jetet.push_back(jetet/CLHEP::GeV);  
    m_jetphi.push_back(jetphi); 
    m_jeteta.push_back(jeteta); 
    
    if (jetet > m_EtCut && std::fabs(jeteta) >= m_etaMinCut && std::fabs(jeteta) < m_etaMaxCut){

      /// flag these jets as having passed
      // HLT::markPassing( bits, aJet, outJets ); 
      xBits->markPassing(aJet, outJets, true); 

      //for monitoring
      m_passedJetCuts_jetet.push_back(jetet/CLHEP::GeV);  
      m_passedJetCuts_jetphi.push_back(jetphi); 
      m_passedJetCuts_jeteta.push_back(jeteta); 
      
      m_HT += jetet;
      m_nJets_passedCuts++;
    }
    
    m_nJets++;
    
    if (m_nJets==1) { // for monitoring
      m_jetet1  = jetet;
      m_jetphi1 = jetphi;
      m_jeteta1 = jeteta;
    }
    else if (m_nJets==2) { // for monitoring
      m_jetet2  = jetet;
      m_jetphi2 = jetphi;
      m_jeteta2 = jeteta;
    }
  }

  if (m_HT > m_HTCut) {
    m_nJets_passedHT = m_nJets_passedCuts;
    m_HT_passedHT = m_HT;

    pass = true;
    m_accepted++;
    ATH_MSG_DEBUG(">>>>> TRIGGER PASSED <<<<<       HT: "
                  << m_HT
                  << " PASSED HT cut of "
                  << m_HTCut);
    m_cutCounter = 2;
  }else{
    pass = false;
    m_nJets_failedHT = m_nJets_passedCuts;
    m_rejected++;
    if(msgLvl() <= MSG::DEBUG){
      ATH_MSG_DEBUG(">>>>> TRIGGER FAILED <<<<<       HT: "
                    << m_HT 
                    << " FAILED HT cut of " 
                    << m_HTCut);
    }
  }

  m_HT=m_HT/CLHEP::GeV;
  m_HT_passedHT=m_HT_passedHT/CLHEP::GeV;

  if(msgLvl() <= MSG::DEBUG){
    int nJet=0;
    for (const xAOD::Jet* testJet : theJets) {
      ATH_MSG_DEBUG(" Jet "
                    << (nJet+1) 
                    << " | TrigPassBit: " 
                    // << HLT::isPassing( bits, testJet, outJets ) 
                    << xBits->isPassing( testJet, outJets ) 
                    << " Et: "
                    << testJet->p4().Et()
                    << " Eta: "
                    << testJet->eta());
      nJet++;
    }
    
  }
  

  // attach the bits

  if(attachFeature(outputTE, xBits.release(), "passbits") != HLT::OK){
    ATH_MSG_ERROR("Could not store TrigPassBits! ");
  }

  // Time total TrigEFCaloHypo execution time.
  // -------------------------------------

  if (m_timersvc) m_timers[0]->stop();

  return HLT::OK;

}
