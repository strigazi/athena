/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// sTgcDigitizationTool
// ------------
// Authors:  Nectarios Benekos  <nectarios.benekos@cern.ch>
//           Jiaming Yu  <jiaming.yu@cern.ch>  
////////////////////////////////////////////////////////////////////////////////

#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"
 
//Outputs
#include "MuonDigitContainer/sTgcDigitContainer.h"

//sTGC digitization includes
#include "sTGC_Digitization/sTgcDigitizationTool.h"
#include "sTGC_Digitization/sTgcDigitMaker.h"
#include "sTGC_Digitization/sTgcVMMSim.h"
#include "sTGC_Digitization/sTgcSimDigitData.h"

//Geometry
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonSimEvent/sTgcHitIdHelper.h"
#include "MuonSimEvent/sTgcSimIdToOfflineId.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkSurfaces/Surface.h"

//Pile-up
#include "PileUpTools/PileUpMergeSvc.h"

//Truth
#include "CLHEP/Units/PhysicalConstants.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "HepMC/GenParticle.h"

//Random Numbers
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"

#include <sstream>
#include <iostream>
#include <fstream>

#include <TFile.h>
#include <TH2.h>
#include <TH1.h>

namespace Trk{
   class PlaneSurface;
}

using namespace MuonGM;

typedef struct {
  uint16_t bcTag; // 001 --> PREVIOUS BC, 010 --> CURRENT BC, 100 --> NEXT BC ==> if digitTime inside the overlap of the PREVIOUS and CURRENT BC, bcTag = 011 (=3)
  float charge;  // for pad/wire response, charge = 0. 
  int channelType; // 0 --> pad, 1 --> strip,  2 --> wire
  MuonSimData::Deposit Dep;
  double Edep; 
  int keep;  // a flag used to label this digit object is kept or not (could be removed by the electronics threshold / deadtime) 0 --> do not keep, 1 --> keep if the strip is turned on by neighborOn mode; 2 --> keep because of a signal over threshold 
  bool isDead;
  bool isPileup;
} structDigitType; 

typedef std::pair<float, structDigitType> tempDigitType; // pair<float digitTime, structDigitType>  

typedef struct {
  int readLevel;
  float deadtimeStart;
  float neighborOnTime;
} structReadoutElement;
typedef std::map<Identifier,std::pair<structReadoutElement, std::vector<tempDigitType> > > tempDigitCollectionType; // map<ReadoutElementID, pair< read or not,  all DigitObject with the identical ReadoutElementId but at different time>>; for the int(read or not) : 0 --> do not read this strip, 1 --> turned on by neighborOn mode; 2 --> this channel has signal over threshold
typedef std::map<IdentifierHash, tempDigitCollectionType> tempDigitContainerType; // use IdentifierHashId, similar structure as the <sTgcDigitCollection>
typedef std::map<sTGCSimHit*, int> tempHitEventMap; // use IdentifierHashId, similar structure as the <sTgcDigitCollection>

inline bool sort_EarlyToLate(tempDigitType a, tempDigitType b){
  return a.first < b.first;
}

//inline bool sort_digitsEarlyToLate(sTgcDigit a, sTgcDigit b){
//  return a.time() < b.time();
//}

inline bool sort_digitsEarlyToLate(sTgcSimDigitData a, sTgcSimDigitData b){
  return (a.getSTGCDigit()).time() < (b.getSTGCDigit()).time();
}

/*******************************************************************************/
sTgcDigitizationTool::sTgcDigitizationTool(const std::string& type, const std::string& name, const IInterface* parent)
  : PileUpToolBase(type, name, parent),
    m_mergeSvc(0), 
    m_rndmEngine(0),
    m_rndmSvc("AtRndmGenSvc", name),
    m_rndmEngineName("MuonDigitization"),
    m_sgSvc("StoreGateSvc", name), 
    m_activeStore(0), 
    m_hitIdHelper(0), 
    m_digitContainer(0),
    m_idHelper(0),
    m_mdManager(0),
    m_digitizer(0),
    m_thpcsTGC(0),
    m_sdoContainer(0), 
    m_inputHitCollectionName("sTGCSensitiveDetector"),
    m_outputDigitCollectionName("sTGC_DIGITS"),
    m_outputSDO_CollectionName("sTGC_SDO"),
    m_doToFCorrection(0),
    m_doChannelTypes(3),
    m_readoutThreshold(0),
    m_neighborOnThreshold(0),
    m_saturation(0),
	m_deadtimeON(1),
	m_produceDeadDigits(0),
    m_deadtimeStrip(50.),
    m_deadtimePad(5.),
    m_deadtimeWire(5.),
    m_readtimeStrip(6.25),
    m_readtimePad(6.25),
    m_readtimeWire(6.25),
    m_timeWindowOffsetPad(0),
    m_timeWindowOffsetStrip(0),
    m_timeWindowPad(30.),
    m_timeWindowStrip(30.),
    m_bunchCrossingTime(0),
    m_timeJitterElectronicsStrip(0),
    m_timeJitterElectronicsPad(0),
    m_hitTimeMergeThreshold(0),
    m_energyDepositThreshold(300.0*CLHEP::eV)

    //m_file(0),
    //m_SimHitOrg(0),
    //m_SimHitMerged(0),
    //m_SimHitDigitized(0),
    //m_SimHitDigitizedwPad(0),
    //m_SimHitDigitizedwoPad(0),
    //m_kineticEnergy(0),
    //m_EnergyDeposit(0)


{
  declareInterface<IMuonDigitizationTool>(this);
  declareProperty("RndmSvc",                 m_rndmSvc,                                             "Random Number Service used in Muon digitization");
  declareProperty("RndmEngine",              m_rndmEngineName,                                      "Random engine name");
  declareProperty("MCStore",                 m_sgSvc,                                               "Simulated Data Event Store"); 
  declareProperty("InputObjectName",         m_inputHitCollectionName    = "sTGCSensitiveDetector", "name of the input object");
  declareProperty("OutputObjectName",        m_outputDigitCollectionName = "sTGC_DIGITS",           "name of the output object");
  declareProperty("OutputSDOName",           m_outputSDO_CollectionName  = "sTGC_SDO"); 
  declareProperty("doToFCorrection",         m_doToFCorrection); 
  declareProperty("doChannelTypes",          m_doChannelTypes); 
  declareProperty("DeadtimeElectronicsStrip",m_deadtimeStrip); 
  declareProperty("DeadtimeElectronicsPad",  m_deadtimePad); 
  declareProperty("timeWindowPad",           m_timeWindowPad); 
  declareProperty("timeWindowStrip",         m_timeWindowStrip); 
  declareProperty("energyDepositThreshold",  m_energyDepositThreshold,           "Minimum energy deposit considered for digitization");
}
/*******************************************************************************/
// member function implementation
//--------------------------------------------
StatusCode sTgcDigitizationTool::initialize() {

  StatusCode status(StatusCode::SUCCESS);

  ATH_MSG_INFO (" sTgcDigitizationTool  retrieved");
  ATH_MSG_INFO ( "Configuration  sTgcDigitizationTool" );
  ATH_MSG_INFO ( "RndmSvc                " << m_rndmSvc             );
  ATH_MSG_INFO ( "RndmEngine             " << m_rndmEngineName      );
  ATH_MSG_INFO ( "MCStore                " << m_sgSvc               );
  ATH_MSG_INFO ( "InputObjectName        " << m_inputHitCollectionName );
  ATH_MSG_INFO ( "OutputObjectName       " << m_outputDigitCollectionName );
  ATH_MSG_INFO ( "OutputSDOName          " << m_outputSDO_CollectionName  ); 

  // initialize transient event store 
  if(m_sgSvc.retrieve().isFailure()) { 
    ATH_MSG_FATAL("Could not retrieve StoreGateSvc!");
    return StatusCode::FAILURE; 
  } 

  status = service("ActiveStoreSvc", m_activeStore);
  if(!status.isSuccess()) { 
    msg(status.isFailure() ? MSG::FATAL : MSG::ERROR) << "Could not get active store service" << endmsg; 
    return status;
  }

  // retrieve MuonDetctorManager from DetectorStore
  status = detStore()->retrieve(m_mdManager);
  if(status.isFailure()) return status;
  ATH_MSG_DEBUG("Retrieved MuonDetectorManager from DetectorStore.");
  
  //initialize the sTgcIdHelper
  m_idHelper = m_mdManager->stgcIdHelper();
  if(!m_idHelper) {
    ATH_MSG_WARNING("sTgcIdHelper could not be retrieved.");
    return status;
  }
  
  // sTgcHitIdHelper
  m_hitIdHelper = sTgcHitIdHelper::GetHelper();

  // check the input object name
  if(m_inputHitCollectionName=="") {
    ATH_MSG_FATAL("Property InputObjectName not set !");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Input objects: '" << m_inputHitCollectionName << "'");
  }

  // check the output object name
  if(m_outputDigitCollectionName=="") {
    ATH_MSG_FATAL("Property OutputObjectName not set !");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Output digits: '" << m_outputDigitCollectionName << "'");
  }
  
  // initialize class to execute digitization 
  m_digitizer = new sTgcDigitMaker(m_hitIdHelper, m_mdManager);
  m_digitizer->setMessageLevel(static_cast<MSG::Level>(msgLevel()));
  if(!m_rndmSvc.retrieve().isSuccess()) {
    ATH_MSG_FATAL(" Could not initialize Random Number Service");
    return StatusCode::FAILURE;
  } 
    
  // getting our random numbers stream
  ATH_MSG_DEBUG("Getting random number engine : <" << m_rndmEngineName << ">");
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if(m_rndmEngine==0) {
    ATH_MSG_FATAL("Could not find RndmEngine : " << m_rndmEngineName);
    return StatusCode::FAILURE;
  }
		
  status = m_digitizer->initialize(m_rndmEngine, m_doChannelTypes);
  if(status.isFailure()) {
    ATH_MSG_FATAL("Fail to initialize sTgcDigitMaker");
    return status;
  }

  readDeadtimeConfig();

  // initialize digit container
  m_digitContainer = new sTgcDigitContainer(m_idHelper->detectorElement_hash_max());
  m_digitContainer->addRef();


  // initialize digit parameters
  //m_noiseFactor = 0.09;
  m_readoutThreshold = 0.05; 
  m_neighborOnThreshold = 0.01;
  m_saturation = 1.75; // = 3500. / 2000.;
  m_hitTimeMergeThreshold = 1; //1ns = resolution of peak finding descriminator
  //m_ADC = 0.00171; // = m_saturation / (TMath::Power(2, 10));
  //m_ADC = 0.02734; // = m_saturation / (TMath::Power(2, 6));
  //m_deadtimeStrip = 50.; // 50ns deadtime of electronics after peak found (for strip readout) 
  //m_deadtimePad = 5.; // 50ns deadtime of electronics after peak found (for strip readout) 
  m_timeWindowOffsetPad    = 0.;
  m_timeWindowOffsetStrip   = 25.;
  //m_timeWindowPad          = 30.; // TGC  29.32; // 29.32 ns = 26 ns +  4 * 0.83 ns
  //m_timeWindowStrip         = 30.; // TGC  40.94; // 40.94 ns = 26 ns + 18 * 0.83 ns
  m_bunchCrossingTime       = 24.95; // 24.95 ns =(40.08 MHz)^(-1)
  m_timeJitterElectronicsPad = 2.; //ns
  m_timeJitterElectronicsStrip= 2.; //ns

  //m_file = new TFile("sTGC_Digit_plot.root", "RECREATE");
  //m_SimHitOrg = new TH2F("SimHitOrg", "OrgSimHit;HitX;HitY", 1000, -5000., 5000., 1000, -5000., 5000.);
  //m_SimHitMerged = new TH2F("SimHitMerged", "SimHitMerged;HitX;HitY", 1000, -5000., 5000., 1000, -5000., 5000.);
  //m_SimHitDigitized = new TH2F("SimHitDigitized", "SimHitDigitized;HitX;HitY", 1000, -5000., 5000., 1000, -5000., 5000.);
  //m_SimHitDigitizedwPad = new TH2F("SimHitDigitizedwPad", "SimHitDigitizedwPad;HitX;HitY", 1000, -5000., 5000., 1000, -5000., 5000.);
  //m_SimHitDigitizedwoPad = new TH2F("SimHitDigitizedwoPad", "SimHitDigitizedwoPad;HitX;HitY", 1000, -5000., 5000., 1000, -5000., 5000.);
  //m_kineticEnergy = new TH1F("kineticEnergy", "kineticEnergy;kineticEnergy (GeV);", 1000, 0., 200.);
  //m_EnergyDeposit = new TH1F("EnergyDeposit", "EnergyDeposit;EnergyDeposit (MeV);", 1000, 0., 0.01);

  return status;
}
/*******************************************************************************/ 
StatusCode sTgcDigitizationTool::prepareEvent(unsigned int nInputEvents) {

  ATH_MSG_DEBUG("sTgcDigitizationTool::prepareEvent() called for " << nInputEvents << " input events" );
  m_STGCHitCollList.clear();

  return StatusCode::SUCCESS;
}
/*******************************************************************************/

StatusCode sTgcDigitizationTool::processBunchXing(int bunchXing,
						  SubEventIterator bSubEvents,
						  SubEventIterator eSubEvents) {
  ATH_MSG_DEBUG ( "sTgcDigitizationTool::in processBunchXing()" );
  if(!m_thpcsTGC) {
    m_thpcsTGC = new TimedHitCollection<sTGCSimHit>();
  }

  typedef PileUpMergeSvc::TimedList<sTGCSimHitCollection>::type TimedHitCollList;
  TimedHitCollList hitCollList;

  if (!(m_mergeSvc->retrieveSubSetEvtData(m_inputHitCollectionName, hitCollList, bunchXing,
                                          bSubEvents, eSubEvents).isSuccess()) &&
      hitCollList.size() == 0) {
    ATH_MSG_ERROR("Could not fill TimedHitCollList");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_VERBOSE(hitCollList.size() << " sTGCSimHitCollection with key " <<
                    m_inputHitCollectionName << " found");
  }

  TimedHitCollList::iterator iColl(hitCollList.begin());
  TimedHitCollList::iterator endColl(hitCollList.end());

  // Iterating over the list of collections
  for( ; iColl != endColl; iColl++){

    sTGCSimHitCollection *hitCollPtr = new sTGCSimHitCollection(*iColl->second);
    PileUpTimeEventIndex timeIndex(iColl->first);

    ATH_MSG_DEBUG("sTGCSimHitCollection found with " << hitCollPtr->size() <<
                  " hits");
    ATH_MSG_VERBOSE("time index info. time: " << timeIndex.time()
                    << " index: " << timeIndex.index()
                    << " type: " << timeIndex.type());

    m_thpcsTGC->insert(timeIndex, hitCollPtr);
    m_STGCHitCollList.push_back(hitCollPtr);

  }

  return StatusCode::SUCCESS;
}
/*******************************************************************************/
StatusCode sTgcDigitizationTool::getNextEvent() {

  ATH_MSG_DEBUG ( "sTgcDigitizationTool::getNextEvent()" );
	 
  if (!m_mergeSvc) {
    const bool CREATEIF(true);
    if (!(service("PileUpMergeSvc", m_mergeSvc, CREATEIF)).isSuccess() ||
	0 == m_mergeSvc) {
      ATH_MSG_ERROR ("Could not find PileUpMergeSvc" );
      return StatusCode::FAILURE;
    }
  }

  // initialize pointer
  //m_thpcsTGC = 0;
 
  //  get the container(s)
  typedef PileUpMergeSvc::TimedList<sTGCSimHitCollection>::type TimedHitCollList;
 
  //this is a list<info<time_t, DataLink<sTGCSimHitCollection> > >
  TimedHitCollList hitCollList;
	 
  if (!(m_mergeSvc->retrieveSubEvtsData(m_inputHitCollectionName, hitCollList).isSuccess()) ) {
    ATH_MSG_ERROR ( "Could not fill TimedHitCollList" );
    return StatusCode::FAILURE;
  }
  if (hitCollList.size()==0) {
    ATH_MSG_ERROR ( "TimedHitCollList has size 0" );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG ( hitCollList.size() << " sTGC SimHitCollections with key " << m_inputHitCollectionName << " found" );
  }
	 
  // create a new hits collection
  //m_thpcsTGC = new TimedHitCollection<sTGCSimHit>() ;
  
  //Perform null check on m_thpcsTGC. If pointer is not null throw error
  if(!m_thpcsTGC) { 
        m_thpcsTGC = new TimedHitCollection<sTGCSimHit>();
  }else{
 	ATH_MSG_ERROR ( "m_thpcsTGC is not null" );
	return StatusCode::FAILURE;	
  }
	 
  //now merge all collections into one
  TimedHitCollList::iterator iColl(hitCollList.begin());
  TimedHitCollList::iterator endColl(hitCollList.end());
  while (iColl != endColl) {
    const sTGCSimHitCollection* p_collection(iColl->second);  
    m_thpcsTGC->insert(iColl->first, p_collection);
    ATH_MSG_DEBUG ( "sTGC SimHitCollection found with " << p_collection->size() << " hits"  );
    ++iColl;
  }
  
  return StatusCode::SUCCESS;
}
/*******************************************************************************/
StatusCode sTgcDigitizationTool::mergeEvent() {

  StatusCode status = StatusCode::SUCCESS;

  ATH_MSG_DEBUG ( "sTgcDigitizationTool::in mergeEvent()" );

  // Cleanup and record the Digit container in StoreGate
  status = recordDigitAndSdoContainers();
  if(!status.isSuccess()) {
    ATH_MSG_FATAL("sTgcDigitizationTool::recordDigitAndSdoContainers failed.");
    return StatusCode::FAILURE; 
  }
    
  status = doDigitization();
  if (status.isFailure())  {
    ATH_MSG_ERROR ( "doDigitization Failed" );
    //return StatusCode::FAILURE;
  }

  // reset the pointer (delete null pointer should be safe)
  delete m_thpcsTGC; 
  m_thpcsTGC = 0;

	
  std::list<sTGCSimHitCollection*>::iterator STGCHitColl = m_STGCHitCollList.begin();
  std::list<sTGCSimHitCollection*>::iterator STGCHitCollEnd = m_STGCHitCollList.end();
  while(STGCHitColl!=STGCHitCollEnd) {
    delete (*STGCHitColl);
    ++STGCHitColl;
  }
  m_STGCHitCollList.clear(); 


  return status;
}
/*******************************************************************************/
StatusCode sTgcDigitizationTool::recordDigitAndSdoContainers() { 
  // cleanup digit container
  m_digitContainer->cleanup();
  
  // record the digit container in StoreGate
  m_activeStore->setStore(&*m_sgSvc);
  StatusCode status = m_sgSvc->record(m_digitContainer, m_outputDigitCollectionName);
  if(status.isFailure()) {
    ATH_MSG_FATAL("Unable to record sTGC digit container in StoreGate");
    return status;
  }
  ATH_MSG_DEBUG("sTgcDigitContainer recorded in StoreGate.");
  
  // create and record the SDO container in StoreGate
  m_sdoContainer = new MuonSimDataCollection();
  status = m_sgSvc->record(m_sdoContainer, m_outputSDO_CollectionName);
  if(status.isFailure())  {
    ATH_MSG_FATAL("Unable to record sTGC SDO collection in StoreGate");
    return status;
  } else {
    ATH_MSG_DEBUG("sTgcSDOCollection recorded in StoreGate.");
  }

  return status;
}
/*******************************************************************************/
StatusCode sTgcDigitizationTool::digitize() {
  return this->processAllSubEvents(); 
} 
/*******************************************************************************/
StatusCode sTgcDigitizationTool::processAllSubEvents() {
  // 
  StatusCode status = StatusCode::SUCCESS;
  //m_thpcsTGC = new TimedHitCollection<sTGCSimHit>();
  ATH_MSG_DEBUG (" sTgcDigitizationTool::processAllSubEvents()" );
  
  status = recordDigitAndSdoContainers();
  /*if(!status.isSuccess()) {
    ATH_MSG_FATAL("sTgcDigitizationTool::recordDigitAndSdoContainers failed.");
    return StatusCode::FAILURE;
  }
  */
  //merging of the hit collection in getNextEvent method    	
  if (0 == m_thpcsTGC) {
    status = getNextEvent();
    if (StatusCode::FAILURE == status) {
      ATH_MSG_INFO ( "There are no sTGC hits in this event" );
      return status;
    }
  }
  status = doDigitization();
  if (status.isFailure())  {
    ATH_MSG_ERROR ( "doDigitization() Failed" );
  }   
  
  // reset the pointer (delete null pointer should be safe)
  delete m_thpcsTGC; 
  m_thpcsTGC = 0;


  return status;
}
/*******************************************************************************/
StatusCode sTgcDigitizationTool::finalize() {
  ATH_MSG_INFO("sTgcDigitizationTool::finalize() ---- m_digitContainer->digit_size() = "<<m_digitContainer->digit_size()); 
  m_digitContainer->release(); 
  delete m_digitizer; 
  m_digitizer = 0;

  //m_SimHitOrg->Write();
  //m_SimHitMerged->Write();
  //m_SimHitDigitized->Write();
  //m_SimHitDigitizedwPad->Write();
  //m_SimHitDigitizedwoPad->Write();
  //m_kineticEnergy->Write();
  //m_EnergyDeposit->Write();

  //m_file->Close();

  return StatusCode::SUCCESS;
}
/*******************************************************************************/
StatusCode sTgcDigitizationTool::doDigitization() {
  
  ATH_MSG_DEBUG ("sTgcDigitizationTool::doDigitization()" );
    
  TimedHitCollection<sTGCSimHit>::const_iterator i, e; 

  // Collections of digits by digit type associated with a detector element
  //std::map< IdentifierHash, std::map< Identifier, std::vector<sTgcDigit> > > unmergedPadDigits;
  //std::map< IdentifierHash, std::map< Identifier, std::vector<sTgcDigit> > > unmergedStripDigits;
  //std::map< IdentifierHash, std::map< Identifier, std::vector<sTgcDigit> > > unmergedWireDigits;
  std::map< IdentifierHash, std::map< Identifier, std::vector<sTgcSimDigitData> > > unmergedPadDigits;
  std::map< IdentifierHash, std::map< Identifier, std::vector<sTgcSimDigitData> > > unmergedStripDigits;
  std::map< IdentifierHash, std::map< Identifier, std::vector<sTgcSimDigitData> > > unmergedWireDigits;
  
  std::map< IdentifierHash, std::map< Identifier, std::vector<sTgcDigit> > > outputDigits;
  //std::map< IdentifierHash, std::map< Identifier, std:vector<sTgcSimDigitData> > > outputDigits;

  sTgcDigitCollection* digitCollection = 0;  //output digits

  ATH_MSG_DEBUG("create PRD container of size " << m_idHelper->detectorElement_hash_max());
  
  IdContext tgcContext = m_idHelper->module_context();
  
  float earliestEventTime = 9999;

  // nextDetectorElement-->sets an iterator range with the hits of current detector element , returns a bool when done
  while(m_thpcsTGC->nextDetectorElement(i, e)) {
	  int nhits = 0;
      ATH_MSG_VERBOSE("Next Detector Element");
	  while(i != e){ //loop through the hits on this Detector Element
          ATH_MSG_VERBOSE("Looping over hit " << nhits+1 << " on this Detector Element." );
          
          nhits++;
		  TimedHitPtr<sTGCSimHit> phit = *i++;
		  const sTGCSimHit& hit = *phit;
          ATH_MSG_VERBOSE("Hit Particle ID : " << hit.particleEncoding() );
          float eventTime = phit.eventTime(); 
          if(eventTime < earliestEventTime) earliestEventTime = eventTime;
	      // Cut on energy deposit of the particle
	      if(hit.depositEnergy() < m_energyDepositThreshold) {
	        ATH_MSG_VERBOSE("Hit with Energy Deposit of " << hit.depositEnergy() << " less than 300.eV  Skip this hit." );
	        continue;
          }
          if(eventTime != 0){
             msg(MSG::DEBUG) << "Updated hit global time to include off set of " << eventTime << " ns from OOT bunch." << endmsg;
          }
          else {
              msg(MSG::DEBUG) << "This hit came from the in time bunch." << endmsg;
          }
		  sTgcSimIdToOfflineId simToOffline(*m_idHelper);
		  const int idHit = hit.sTGCId();
          ATH_MSG_VERBOSE("Hit ID " << idHit );
		  Identifier layid = simToOffline.convert(idHit);
          ATH_MSG_VERBOSE("Layer ID[" << layid.getString() << "]");
          int eventId = phit.eventId();
          std::string stationName= m_idHelper->stationNameString(m_idHelper->stationName(layid));
		  int isSmall = stationName[2] == 'S';
		  int gasGap = m_idHelper->gasGap(layid);
          ATH_MSG_VERBOSE("Gas Gap " << gasGap );

		  const MuonGM::sTgcReadoutElement* detEL = m_mdManager->getsTgcReadoutElement(layid);  //retreiving the sTGC this hit is located in
		  if( !detEL ){
			msg(MSG::WARNING) << "Failed to retrieve detector element for: isSmall " << isSmall << " eta " << m_idHelper->stationEta(layid) << " phi " << m_idHelper->stationPhi(layid) << " ml " << m_idHelper->multilayer(layid)  << endmsg;
			continue;
		  }

          
          // project the hit position to wire surface (along the incident angle)
          ATH_MSG_VERBOSE("Projecting hit to Wire Surface" );
          Amg::Vector3D HPOS(hit.globalPosition().x(),hit.globalPosition().y(),hit.globalPosition().z());  //Global position of the hit
          const Amg::Vector3D GLOBAL_ORIG(0., 0., 0.);
		  const Amg::Vector3D GLOBAL_Z(0., 0., 1.);
		  const Amg::Vector3D GLODIRE(hit.globalDirection().x(), hit.globalDirection().y(), hit.globalDirection().z());

          ATH_MSG_VERBOSE("Global Z " << GLOBAL_Z );
          ATH_MSG_VERBOSE("Global Direction " << GLODIRE );
          ATH_MSG_VERBOSE("Global Position " << HPOS );
          
          int surfHash_wire =  detEL->surfaceHash(gasGap, 2);
          ATH_MSG_VERBOSE("Surface Hash for wire plane" << surfHash_wire );
		  const Trk::PlaneSurface&  SURF_WIRE = detEL->surface(surfHash_wire);  //Plane of the wire surface in this gasGap
          ATH_MSG_VERBOSE("Wire Surface Defined " << SURF_WIRE.center() );

		  Amg::Vector3D LOCAL_Z = SURF_WIRE.transform().inverse()*GLOBAL_Z - SURF_WIRE.transform().inverse()*GLOBAL_ORIG;
		  Amg::Vector3D LOCDIRE = SURF_WIRE.transform().inverse()*GLODIRE - SURF_WIRE.transform().inverse()*GLOBAL_ORIG;

		  Amg::Vector3D LPOS = SURF_WIRE.transform().inverse() * HPOS;  //Position of the hit on the wire plane in local coordinates
          
          ATH_MSG_VERBOSE("Local Z " << LOCAL_Z );
          ATH_MSG_VERBOSE("Local Direction " << LOCDIRE );
          ATH_MSG_VERBOSE("Local Position " << LPOS );

		  double e = 1e-5;

		  bool X_1 = std::abs( std::abs(LOCAL_Z.x()) - 1. ) < e;
		  bool Y_1 = std::abs( std::abs(LOCAL_Z.y()) - 1. ) < e;
		  bool Z_1 = std::abs( std::abs(LOCAL_Z.z()) - 1. ) < e;
		  bool X_s = std::abs(LOCAL_Z.x()) < e;
		  bool Y_s = std::abs(LOCAL_Z.y()) < e;
		  bool Z_s = std::abs(LOCAL_Z.z()) < e;

		  double scale = 0;
		  if(X_1 && Y_s && Z_s)
			  scale = -LPOS.x() / LOCDIRE.x();
		  if(X_s && Y_1 && Z_s)
			  scale = -LPOS.y() / LOCDIRE.y();
		  if(X_s && Y_s && Z_1)
			  scale = -LPOS.z() / LOCDIRE.z();
		  else
			  ATH_MSG_ERROR(" Wrong scale! ");

		  Amg::Vector3D HITONSURFACE_WIRE = LPOS + scale * LOCDIRE;  //Hit on the wire surface attached to the closest wire in local coordinates
		  Amg::Vector3D G_HITONSURFACE_WIRE = SURF_WIRE.transform() * HITONSURFACE_WIRE;  //The hit on the wire in Global coordinates
          
          ATH_MSG_VERBOSE("Local Hit on Wire Surface " << HITONSURFACE_WIRE );
          ATH_MSG_VERBOSE("Global Hit on Wire Surface " << G_HITONSURFACE_WIRE );
          
        ATH_MSG_DEBUG("sTgcDigitizationTool::doDigitization hits mapped");
    
    	const sTGCSimHit temp_hit(hit.sTGCId(), hit.globalTime(), 
    			 G_HITONSURFACE_WIRE,
			     hit.particleEncoding(),
			     hit.globalDirection(),
			     hit.depositEnergy(),
			     hit.particleLink()
			     );


    	float globalHitTime = temp_hit.globalTime() + eventTime;
    	float tof = temp_hit.globalPosition().mag()/CLHEP::c_light;
    	float bunchTime = globalHitTime - tof;

		sTgcDigitCollection* digiHits = 0;
        
    	digiHits = m_digitizer->executeDigi(&temp_hit, globalHitTime);  //Create all the digits for this particular Sim Hit
		if(!digiHits)
			continue;

		sTgcDigitCollection::const_iterator it_digiHits;
        ATH_MSG_VERBOSE("Hit produced " << digiHits->size() << " digits." );
		for(it_digiHits=digiHits->begin(); it_digiHits!=digiHits->end(); ++it_digiHits) {
			/*
			   NOTE:
			   -----
			   Since not every hit might end up resulting in a
			   digit, this construction might take place after the hit loop
			   in a loop of its own!
			*/
			// make new sTgcDigit
			Identifier newDigitId = (*it_digiHits)->identify(); //This Identifier should be sufficient to determine which RE the digit is from
			float newTime = (*it_digiHits)->time();
			int newChannelType = m_idHelper->channelType((*it_digiHits)->identify());

			float timeJitterElectronicsStrip = CLHEP::RandGauss::shoot(m_rndmEngine, 0, m_timeJitterElectronicsStrip);
			float timeJitterElectronicsPad = CLHEP::RandGauss::shoot(m_rndmEngine, 0, m_timeJitterElectronicsPad);
			if(newChannelType==1)
				newTime += timeJitterElectronicsStrip;
			else
				newTime += timeJitterElectronicsPad;
			uint16_t newBcTag = bcTagging(newTime+bunchTime, newChannelType);

			if(m_doToFCorrection)
				newTime += bunchTime;
			else
				newTime += globalHitTime;

			float newCharge = -1.;
			if(newChannelType==1)
				newCharge = (*it_digiHits)->charge();
			Identifier elemId = m_idHelper->elementID(newDigitId);

			if(newChannelType!=0 && newChannelType!=1 && newChannelType!=2) {
				ATH_MSG_WARNING( "Wrong channelType " << newChannelType );
			}

			bool isDead = 0;
			bool isPileup = 0;
            ATH_MSG_VERBOSE("Hit is from the main signal subevent if eventId is zero, eventId = " << eventId << " newTime: " << newTime);
			if(eventId != 0)  //hit not from the main signal subevent
				  isPileup = 1;
                  
            ATH_MSG_VERBOSE("...Check time 5: " << newTime );
            // Create a new digit with updated time and BCTag
            sTgcDigit* newDigit = new sTgcDigit(newDigitId, newBcTag, newTime, newCharge, isDead, isPileup);  
			IdentifierHash coll_hash;  //Hash defining the detector element
            ATH_MSG_VERBOSE("Unmerged Digit") ;
            ATH_MSG_VERBOSE(" BC tag = "    << newDigit->bcTag()) ;
			ATH_MSG_VERBOSE(" digitTime = " << newDigit->time()) ;
			ATH_MSG_VERBOSE(" charge = "    << newDigit->charge()) ;

            // Create a MuonSimData (SDO) corresponding to the digit
            MuonSimData::Deposit deposit(trklink, MuonMCData(hit.depositEnergy(), tof));
            std::vector<MuonSimData::Deposit> deposits;
            deposits.push_back(deposit);
            MuonSimData simData(deposits, hit.particleEncoding());
            simData.setPosition(hit.globalPosition());
            simData.setTime(globalHitTime);

            // Associate the digit to the appropriate readout channel
			m_idHelper->get_detectorElement_hash(elemId, coll_hash);
            if(newChannelType == 0){ //Pad Digit
                //unmergedPadDigits[coll_hash][newDigitId].push_back(*newDigit);  //Put the digit in a vector associated with the RE 
                //Put the hit and digit in a vector associated with the RE
                unmergedPadDigits[coll_hash][newDigitId].push_back(sTgcSimDigitData(simData, *newDigit));   
            }
            else if(newChannelType == 1){ //Strip Digit
                //unmergedStripDigits[coll_hash][newDigitId].push_back(*newDigit);  //Put the digit in a vector associated with the RE 
                //Put the hit and digit in a vector associated with the RE
                unmergedStripDigits[coll_hash][newDigitId].push_back(sTgcSimDigitData(simData, *newDigit));   
            }
            else if(newChannelType == 2){ //Wire Digit
                //unmergedWireDigits[coll_hash][newDigitId].push_back(*newDigit);  //Put the digit in a vector associated with the RE 
                //Put the hit and digit in a vector associated with the RE
                unmergedWireDigits[coll_hash][newDigitId].push_back(sTgcSimDigitData(simData, *newDigit));   
            }
            delete newDigit;
		} // end of loop digiHits
		delete digiHits;
		digiHits = 0;
	  } // end of while(i != e)
  } //end of while(m_thpcsTGC->nextDetectorElement(i, e))
  
       /*********************
       * Process Pad Digits *
       *********************/
        /*Comments from Jesse Heilman, Apr 12, 2017:
        * Handing Pad digits is fairly striaghtforward: loop over the digits, merge any that are within the allowed
        * time window, sort the digits by time and then determine if any digits fall in the deadtime window of 
        * previous digits.  This can be done in one pass because there is no interaction between pads.
        */
   ATH_MSG_VERBOSE("Processing Pad Digits");
   int nPadDigits = 0;
   //for (std::map< IdentifierHash, std::map< Identifier, std::vector<sTgcDigit> > >::iterator it_DETEL = unmergedPadDigits.begin(); it_DETEL!= unmergedPadDigits.end(); ++it_DETEL) {
   for (std::map< IdentifierHash, std::map< Identifier, std::vector<sTgcSimDigitData> > >::iterator it_DETEL = unmergedPadDigits.begin(); it_DETEL!= unmergedPadDigits.end(); ++it_DETEL) {
   //for (std::map< Identifier, std::vector<sTgcDigit> >::iterator it_REID = it_DETEL->second.begin(); it_REID != it_DETEL->second.end(); ++it_REID) {  //loop on Pads
   for (std::map< Identifier, std::vector<sTgcSimDigitData> >::iterator it_REID = it_DETEL->second.begin(); it_REID != it_DETEL->second.end(); ++it_REID) {  //loop on Pads
       std::stable_sort(it_REID->second.begin(), it_REID->second.end(), sort_digitsEarlyToLate);  //Sort digits on this RE in time
       
       /*******************
       * Merge Pad Digits *
       *******************/       
   
       //std::vector<sTgcDigit>::iterator i = it_REID->second.begin();
       //std::vector<sTgcDigit>::iterator e = it_REID->second.end();
       std::vector<sTgcSimDigitData>::iterator i = it_REID->second.begin();
       std::vector<sTgcSimDigitData>::iterator e = it_REID->second.end();
       e--;  //decrement e to be the last element and not the beyond the last element iterator
       
       while( i!=e ) { 
           //sTgcDigit digit1 = (*i);
           //sTgcDigit digit2 = (*(i+1));
           sTgcDigit digit1 = i->getSTGCDigit();
           sTgcDigit digit2 = (i+1)->getSTGCDigit();
           if(digit2.time() - digit1.time() < m_hitTimeMergeThreshold ) { //two consecutive hits are close enough for merging
               ATH_MSG_VERBOSE("Merging Digits on REID[" << it_REID->first.getString() << "]");
               ATH_MSG_VERBOSE("digit1: " << digit1.time() << ", " << digit1.charge());
               ATH_MSG_VERBOSE("digit2: " << digit2.time() << ", " << digit2.charge());

               // Update the digit info
               bool mergedIsPileup = (digit1.isPileup() && digit2.isPileup());
               //i->set_isPileup(mergedIsPileup);
               digit1.set_charge( digit1.charge()+digit2.charge() );
               digit1.set_isPileup(mergedIsPileup);
               i->setSTGCDigit(digit1);

               it_REID->second.erase (i+1); //remove the later time digit
               e = it_REID->second.end();  //update the end iterator
               e--; //decrement e to be the last element and not the beyond the last element iterator
               ATH_MSG_VERBOSE(it_REID->second.size() << " digits on the channel after merge step");
           }
           else {
               ++i; //There was not a hit to merge: move onto the next one
           }
       }
       ATH_MSG_VERBOSE("Merging complete for Pad REID[" << it_REID->first.getString() << "]");
       //ATH_MSG_VERBOSE(it_REID->second.size() << " digits on the channel after merging");
       ATH_MSG_VERBOSE(it_REID->second.size() << " digits on the channel after merging");

        // Record SimData for pad channels and reate a temporary container 
        // to store merged digits for VMM operation
        std::vector<sTgcDigit> merged_pad_digits;
        for ( const sTgcSimDigitData& digit_hit: it_REID->second) {
            // Add element to SDO container
            Identifier channel_id = (digit_hit.getSTGCDigit()).identify();
            m_sdoContainer->insert(std::make_pair(channel_id, digit_hit.getSimData()));

            // List of digits for VMM
            merged_pad_digits.push_back( digit_hit.getSTGCDigit() );
        }
       unsigned int size_mergedDigits = merged_pad_digits.size();
       unsigned int size_channelDigits = it_REID->second.size();
       if (size_mergedDigits != size_channelDigits) {
            ATH_MSG_WARNING("Critical: Number of merged pad digits (" << size_mergedDigits << ") is not equal to the number of digits on pad channel (" << size_channelDigits << ") after merging. Please verify.");
        }


       /*************************
       * Calculate Pad deadtime *
       **************************/
       ATH_MSG_VERBOSE("Calculating deadtime for Pad REID[" << it_REID->first.getString() << "]");
       
       //float vmmStartTime = (*(it_REID->second.begin())).time();
       float vmmStartTime = (*(merged_pad_digits.begin())).time();
       
       //sTgcVMMSim* theVMM = new sTgcVMMSim(it_REID->second, vmmStartTime, m_deadtimePad, m_readtimePad, m_produceDeadDigits, 0);  // object to simulate the VMM response
       sTgcVMMSim* theVMM = new sTgcVMMSim(merged_pad_digits, vmmStartTime, m_deadtimePad, m_readtimePad, m_produceDeadDigits, 0);  // object to simulate the VMM response
       theVMM->setMessageLevel(static_cast<MSG::Level>(msgLevel()));
       theVMM->initialReport();
       
       bool vmmControl = true;
       
       
       while(vmmControl)
       {
           ATH_MSG_VERBOSE("Tick on Pad REID[" << it_REID->first.getString() << "]");
           vmmControl = theVMM->tick(); //advance the clock.  returns false if no more digits in 
           ATH_MSG_VERBOSE("Tick returned " << vmmControl);
           if(vmmControl) {
               ATH_MSG_VERBOSE("Tock on Pad REID[" << it_REID->first.getString() << "]");
               theVMM->tock(); //update readout status
               sTgcDigit* flushedDigit = theVMM->flush(); // Flush the digit buffer
               if(flushedDigit) {
                   outputDigits[it_DETEL->first][it_REID->first].push_back(*flushedDigit);  // If a digit was in the buffer: store it to the RDO
                   nPadDigits++;
                   ATH_MSG_VERBOSE("Flushed Digit") ;
                   ATH_MSG_VERBOSE(" BC tag = "    << flushedDigit->bcTag()) ;
                   ATH_MSG_VERBOSE(" digitTime = " << flushedDigit->time()) ;
                   ATH_MSG_VERBOSE(" charge = "    << flushedDigit->charge()) ;

                }
               else ATH_MSG_VERBOSE("No digit for this timestep on Pad REID[" << it_REID->first.getString() << "]");
           }
       }
   }
   }
   ATH_MSG_VERBOSE("There are " << nPadDigits << " flushed pad digits in this event.");
   
       /***********************
       * Process Strip Digits *
       ***********************/

       /*Comments from Jesse Heilman, Apr 12, 2017:
        * Handing Strip digits is a particular challenge due to the NeighborOn functionality of the strip Readout.
        * This means that the deadtime for strips can not just happen simply from whenever a strip goes over the 
        * threshold, as happens in the Pads (where the threshold is any charge).  When a strip goes over threshold
        * in NeighborOn mode, it triggers the readout of its neighboring strips at the same time.  If these strips
        * then go over threshold, they trigger their neighbors and so on until a neighbor does not cross the 
        * threshold.  Practically this means that we must loop over all the strip digits, keeping track of their
        * readout triggers and propogating these triggers to neighboring strips before we can assign deadtime windows
        * and decide which digits to keep.
        */
   ATH_MSG_VERBOSE("Processing Strip Digits");
   std::map< IdentifierHash, std::map< Identifier, std::pair <bool, sTgcVMMSim* > > > vmmArray; // map holding the VMMSim objects and a bool indicating if the channel is done processing
   int nStripDigits = 0;
    //for (std::map< IdentifierHash, std::map< Identifier, std::vector<sTgcDigit> > >::iterator it_DETEL = unmergedStripDigits.begin(); it_DETEL!= unmergedStripDigits.end(); ++it_DETEL) {
    //for (std::map< Identifier, std::vector<sTgcDigit> >::iterator it_REID = it_DETEL->second.begin(); it_REID != it_DETEL->second.end(); ++it_REID) {  //loop on Pads
    for (std::map< IdentifierHash, std::map< Identifier, std::vector<sTgcSimDigitData> > >::iterator it_DETEL = unmergedStripDigits.begin(); it_DETEL!= unmergedStripDigits.end(); ++it_DETEL) {
    for (std::map< Identifier, std::vector<sTgcSimDigitData> >::iterator it_REID = it_DETEL->second.begin(); it_REID != it_DETEL->second.end(); ++it_REID) {  //loop on Pads
       std::stable_sort(it_REID->second.begin(), it_REID->second.end(), sort_digitsEarlyToLate);  //Sort digits on this RE in time
       
       /*******************
       * Merge Strip Digits *
       *******************/       
   
       std::vector<sTgcSimDigitData>::iterator i = it_REID->second.begin();
       std::vector<sTgcSimDigitData>::iterator e = it_REID->second.end();
       e--;

       while( i!=e ) { 
           sTgcDigit digit1 = i->getSTGCDigit();
           sTgcDigit digit2 = (i+1)->getSTGCDigit();
           if(digit2.time() - digit1.time() < m_hitTimeMergeThreshold ) { //two consecutive hits are close enough for merging
               ATH_MSG_VERBOSE("Merging Digits on REID[" << it_REID->first.getString() << "]");
               //float mergedCharge = digit1.charge() + digit2.charge();
               bool mergedIsPileup = (digit1.isPileup() && digit2.isPileup());
               digit1.set_charge( digit1.charge()+digit2.charge() );
               digit1.set_isPileup(mergedIsPileup);
               i->setSTGCDigit(digit1);

               it_REID->second.erase (i+1); //remove the later time digit
               e = it_REID->second.end();  //update the end iterator
               e--; //decrement e to be the last element and not the beyond the last element iterator
               ATH_MSG_VERBOSE(it_REID->second.size() << " digits on the channel after merge step");
           }
           else {
               ++i; //There was not a hit to merge: move onto the next one
           }
       }

        // Record SimData for pad channels and create a temporary container 
        // to store merged digits for VMM operation
        std::vector<sTgcDigit> merged_strip_digits;
        for ( const sTgcSimDigitData& digit_hit: it_REID->second) {
            // Add element to SDO container
            Identifier channel_id = (digit_hit.getSTGCDigit()).identify();
            m_sdoContainer->insert(std::make_pair(channel_id, digit_hit.getSimData()));

            // Save digit for further processing with VMM
            merged_strip_digits.push_back( digit_hit.getSTGCDigit() );
        }
       unsigned int size_mergedDigits = merged_strip_digits.size();
       unsigned int size_channelDigits = it_REID->second.size();
       if (size_mergedDigits != size_channelDigits) {
            ATH_MSG_WARNING("Number of merged pad digits (" << size_mergedDigits << ") is not equal to the number of digits on pad channel (" << size_channelDigits << ") after merging. Please verify.");
        }


       ATH_MSG_VERBOSE("Merging complete for Strip REID[" << it_REID->first.getString() << "]");
       ATH_MSG_VERBOSE(it_REID->second.size() << " digits on the channel after merging");
//       float vmmStartTime = (*(it_REID->second.begin())).time();
       vmmArray[it_DETEL->first][it_REID->first].first = true;
       vmmArray[it_DETEL->first][it_REID->first].second = new sTgcVMMSim(merged_strip_digits, (earliestEventTime-25), m_deadtimeStrip, m_readtimeStrip, m_produceDeadDigits, 1);  // object to simulate the VMM response
       vmmArray[it_DETEL->first][it_REID->first].second->setMessageLevel(static_cast<MSG::Level>(msgLevel()));
       ATH_MSG_VERBOSE("VMM instantiated for Strip REID[" << it_REID->first.getString() << "]");
    }
    }
   for(std::map< IdentifierHash, std::map< Identifier, std::pair <bool, sTgcVMMSim* > > >::iterator it_DETEL = vmmArray.begin(); it_DETEL != vmmArray.end(); ++it_DETEL) {
   bool vmmTerminateSignal = false; // when all channels are done processing this will flip true
   while(true){
       vmmTerminateSignal = false;
       for (std::map< Identifier, std::pair <bool, sTgcVMMSim* > >::iterator it_VMM = it_DETEL->second.begin(); it_VMM != it_DETEL->second.end(); ++it_VMM) { // Loop over VMM ticks
          ATH_MSG_VERBOSE("Strip REID[" << it_VMM->first.getString() << "]");
          if (it_VMM->second.first) it_VMM->second.first = it_VMM->second.second->tick();  // returns true if there are still digits to process as well as advances the clock
          vmmTerminateSignal = (vmmTerminateSignal || it_VMM->second.first);  // record the presence of at least one channel with digits to process
       }
       vmmTerminateSignal = !vmmTerminateSignal; // flip the signal bit so it terminates if all channels reported false
       if(vmmTerminateSignal) {
           ATH_MSG_VERBOSE("No more digits to process.  Breaking VMM Loop.");
           break; //Break the Loop if there are no more digits to process
       }
       for (std::map< Identifier, std::pair <bool, sTgcVMMSim* > >::iterator it_VMM = it_DETEL->second.begin(); it_VMM != it_DETEL->second.end(); ++it_VMM) { // Loop over VMM tocks
          bool thresholdCrossed = false;
          if (it_VMM->second.first) {
              ATH_MSG_VERBOSE("Strip REID[" << it_VMM->first.getString() << "]");
              thresholdCrossed = it_VMM->second.second->tock();  // returns true if an overthreshold digit was in the buffer
          }
          
          if(thresholdCrossed) {  // If a strip crosses threshold activate neighbor channels
              Identifier parentId = m_idHelper->parentID(it_VMM->first);
              int multiPlet = m_idHelper->multilayer(it_VMM->first);
              int gasGap = m_idHelper->gasGap(it_VMM->first);
              int channelType = m_idHelper->channelType(it_VMM->first);
              int stripNumber = m_idHelper->channel(it_VMM->first);
              
              bool isValid = false;
              // Alexandre Laurier 2017-11-17: This block can create issues.
              // We need to make sure we dont try to add out of range neighbors to the most extreme strips, ie first and last
              // Currently, we only check to not add strip # 0
              Identifier neighborPlusId = m_idHelper->channelID(parentId, multiPlet, gasGap, channelType, stripNumber+1, true, &isValid);
              if(isValid && it_DETEL->second.count(neighborPlusId) == 1) //The neighbor strip exists and has had a vmm made for it
              {
                  ATH_MSG_VERBOSE("Neighbor Trigger on REID[" << neighborPlusId.getString() << "]");
                  it_DETEL->second[neighborPlusId].second->neighborTrigger();
                  
              }
              if (stripNumber!=1){
                Identifier neighborMinusId = m_idHelper->channelID(parentId, multiPlet, gasGap, channelType, stripNumber-1, true, &isValid);
                if(isValid && it_DETEL->second.count(neighborMinusId) == 1) //The neighbor strip exists and has had a vmm made for it
                {
                    ATH_MSG_VERBOSE("Neighbor Trigger on REID[" << neighborMinusId.getString() << "]");
                    it_DETEL->second[neighborMinusId].second->neighborTrigger();
                  
                }
              }
          }
       }
       
       for (std::map< Identifier, std::pair <bool, sTgcVMMSim* > >::iterator it_VMM = it_DETEL->second.begin(); it_VMM != it_DETEL->second.end(); ++it_VMM) { // Loop over VMM flushes
          sTgcDigit* flushedDigit = it_VMM->second.second->flush();  //Readout the digit buffer
          if(flushedDigit) {
              outputDigits[it_DETEL->first][it_VMM->first].push_back(*flushedDigit);  // If a digit was in the buffer: store it to the RDO
              nStripDigits++;
          }
          else ATH_MSG_VERBOSE("No digit for this timestep on Strip REID[" << it_VMM->first.getString() << "]");
       }
   }
   }
   ATH_MSG_VERBOSE("There are " << nStripDigits << " flushed strip digits in this event.");
   
   /**********************************
    * WIRE DIGIT CODE SHOULD GO HERE *
    *********************************/
   /*** Comment by Chav Chhiv Chau, October 25 2017
    * First implementation of wire response is simple.
    * - When a wire is fired, a digit is recorded for the corresponding
    *   wiregroup. The response implemented is similar to that for pad.
    * TODO: a wiregroup digit should be the sum of currents read by wires
    *       part of that wiregroup
    */
   
   int nWGDigits = 0;
   for (std::map< IdentifierHash, std::map< Identifier, std::vector<sTgcSimDigitData> > >::iterator it_DETEL = unmergedWireDigits.begin(); it_DETEL!= unmergedWireDigits.end(); ++it_DETEL) {
     // loop on digits of same wiregroup
     for (std::map< Identifier, std::vector<sTgcSimDigitData> >::iterator it_REID = it_DETEL->second.begin(); it_REID != it_DETEL->second.end(); ++it_REID) {
         sort(it_REID->second.begin(), it_REID->second.end(), sort_digitsEarlyToLate);  //Sort digits on this RE in time

         /*************************
         * Merge wiregroup Digits *
         *************************/
         // No merging is done, just removing subsequent digits close in time
         // to the first digit. TODO: Have to be updated

         std::vector<sTgcSimDigitData>::iterator i = it_REID->second.begin();
         std::vector<sTgcSimDigitData>::iterator e = it_REID->second.end();
         e--;  //decrement e to be the last element and not the beyond the last element iterator

         while( i!=e ) {
             sTgcDigit digit1 = i->getSTGCDigit();
             sTgcDigit digit2 = (i+1)->getSTGCDigit();
             if(digit2.time() - digit1.time() < m_hitTimeMergeThreshold ) { //two consecutive hits are close enough for merging
                 ATH_MSG_VERBOSE("Merging Digits on REID[" << it_REID->first.getString() << "]");
                 ATH_MSG_VERBOSE("digit1: " << digit1.time() << ", " << digit1.charge());
                 ATH_MSG_VERBOSE("digit2: " << digit2.time() << ", " << digit2.charge());

                 bool mergedIsPileup = (digit1.isPileup() && digit2.isPileup());
                 digit1.set_charge( digit1.charge()+digit2.charge() );
                 digit1.set_isPileup(mergedIsPileup);
                 i->setSTGCDigit(digit1);

                 it_REID->second.erase (i+1); //remove the later time digit
                 e = it_REID->second.end();  //update the end iterator
                 e--; //decrement e to be the last element and not the beyond the last element iterator
                 ATH_MSG_VERBOSE(it_REID->second.size() << " digits on the channel after merge step");
             }
             else {
                 ++i; //There was not a hit to merge: move onto the next one
             }
         }

        // Record SimData for pad channels and create a temporary container to 
        // store merged digits for VMM operation
        std::vector<sTgcDigit> merged_wire_digits;
        for ( const sTgcSimDigitData& digit_hit: it_REID->second) {
            Identifier channel_id = (digit_hit.getSTGCDigit()).identify();
            m_sdoContainer->insert(std::make_pair(channel_id, digit_hit.getSimData()));

            // Save digit for further processing with VMM
            merged_wire_digits.push_back( digit_hit.getSTGCDigit() );
        }
       unsigned int size_mergedDigits = merged_wire_digits.size();
       unsigned int size_channelDigits = it_REID->second.size();
       if (size_mergedDigits != size_channelDigits) {
            ATH_MSG_WARNING("Number of merged pad digits (" << size_mergedDigits << ") is not equal to the number of digits on pad channel (" << size_channelDigits << ") after merging. Please verify.");
        }


         ATH_MSG_VERBOSE("Merging complete for Wiregroup REID[" << it_REID->first.getString() << "]");
         ATH_MSG_VERBOSE(it_REID->second.size() << " digits on the channel after merging");

         /*******************************
         * Calculate wiregroup deadtime *
         *******************************/
         ATH_MSG_VERBOSE("Calculating deadtime for wiregroup REID[" << it_REID->first.getString() << "]");

         float vmmStartTime = (*(merged_wire_digits.begin())).time();

         sTgcVMMSim* theVMM = new sTgcVMMSim(merged_wire_digits, vmmStartTime, m_deadtimeWire, m_readtimeWire, m_produceDeadDigits, 2);  // object to simulate the VMM response
         theVMM->setMessageLevel(msgLevel());
         theVMM->initialReport();

         bool vmmControl = true;


         while(vmmControl)
         {
             ATH_MSG_VERBOSE("Tick on wiregroup REID[" << it_REID->first.getString() << "]");
             vmmControl = theVMM->tick(); //advance the clock.  returns false if no more digits in
             ATH_MSG_VERBOSE("Tick returned " << vmmControl);
             if(vmmControl) {
                 ATH_MSG_VERBOSE("Tock on wiregroup REID[" << it_REID->first.getString() << "]");
                 theVMM->tock(); //update readout status
                 sTgcDigit* flushedDigit = theVMM->flush(); // Flush the digit buffer
                 if(flushedDigit) {
                     outputDigits[it_DETEL->first][it_REID->first].push_back(*flushedDigit);  // If a digit was in the buffer: store it to the RDO
                     nWGDigits++;
                     ATH_MSG_VERBOSE("Flushed wiregroup digit") ;
                     ATH_MSG_VERBOSE(" BC tag = "    << flushedDigit->bcTag()) ;
                     ATH_MSG_VERBOSE(" digitTime = " << flushedDigit->time()) ;
                     ATH_MSG_VERBOSE(" charge = "    << flushedDigit->charge()) ;

                  }
                 else ATH_MSG_VERBOSE("No digit for this timestep on wiregroup REID[" << it_REID->first.getString() << "]");
             }
         }
     }
   }
   ATH_MSG_VERBOSE("There are " << nWGDigits << " flushed wiregroup digits in this event.");
   
   /*************************************************
    * Output the digits to the StoreGate collection *
    ************************************************/
  m_activeStore->setStore(&*m_sgSvc);

  for(std::map< IdentifierHash, std::map< Identifier, std::vector<sTgcDigit> > >::iterator it_coll = outputDigits.begin(); it_coll != outputDigits.end(); ++it_coll){
	  IdentifierHash coll = it_coll->first;
	  msg(MSG::VERBOSE) << "coll = "<< coll << endmsg;
	  digitCollection = new sTgcDigitCollection(it_coll->second.begin()->first, coll);

	  for(std::map< Identifier, std::vector<sTgcDigit> >::iterator it_REID = it_coll->second.begin(); it_REID != it_coll->second.end(); ++it_REID){
			for(std::vector< sTgcDigit >::iterator it_digit = it_REID->second.begin(); it_digit != it_REID->second.end(); ++it_digit){
                        sTgcDigit* finalDigit = new sTgcDigit(it_digit->identify(), it_digit->bcTag(), it_digit->time(), it_digit->charge(), it_digit->isDead(), it_digit->isPileup());
						digitCollection->push_back(finalDigit);
                        ATH_MSG_VERBOSE("Final Digit") ;
                        ATH_MSG_VERBOSE(" BC tag = "    << finalDigit->bcTag()) ;
                        ATH_MSG_VERBOSE(" digitTime = " << finalDigit->time()) ;
                        ATH_MSG_VERBOSE(" charge = "    << finalDigit->charge()) ;
			} // end of loop for all the digit object of the same ReadoutElementID

	  } // end of loop for all the ReadoutElementID

	  if(digitCollection->size()){
		  ATH_MSG_VERBOSE("push the collection to m_digitcontainer : HashId = " << digitCollection->identifierHash() );
		  if(m_digitContainer->addCollection(digitCollection, digitCollection->identifierHash()).isFailure())
			  ATH_MSG_WARNING("Failed to add collection with hash " << digitCollection->identifierHash());
              
              ATH_MSG_VERBOSE("Collection added to m_digitcontainer : HashId = " << digitCollection->identifierHash() << " of size " << digitCollection->size());

		  for(sTgcDigitCollection::iterator it_stgcDigit_final = digitCollection->begin(); it_stgcDigit_final != digitCollection->end(); ++it_stgcDigit_final) {
//			  ATH_MSG_VERBOSE("push to StoreGate : Digit Id= " << ((*it_stgcDigit_final)->identify()).getString()) ;
              ATH_MSG_VERBOSE(" BC tag = "    << (*it_stgcDigit_final)->bcTag()) ;
			  ATH_MSG_VERBOSE(" digitTime = " << (*it_stgcDigit_final)->time()) ;
			  ATH_MSG_VERBOSE(" charge_6bit = "    << (*it_stgcDigit_final)->charge_6bit()) ;
			  ATH_MSG_VERBOSE(" charge_10bit = "    << (*it_stgcDigit_final)->charge_10bit()) ;
//			  ATH_MSG_VERBOSE(" channelType = " << m_idHelper->channelType((*it_stgcDigit_final)->identify())) ;
                      
		  }
	  }
	  else {
		  delete digitCollection;
		  digitCollection = 0;
	  }
  } 

  return StatusCode::SUCCESS;
}

/*******************************************************************************/
void sTgcDigitizationTool::readDeadtimeConfig()
{
  const char* const fileName = "sTGC_Digitization_deadtime.config";
  std::string fileWithPath = PathResolver::find_file (fileName, "DATAPATH");

  ATH_MSG_INFO("Reading deadtime config file");

  std::ifstream ifs;
  if (fileWithPath != "") {
    ifs.open(fileWithPath.c_str(), std::ios::in);
  }
  else {
    ATH_MSG_FATAL("readDeadtimeConfig(): Could not find file " << fileName );
    exit(-1);
  }

  if(ifs.bad()){
    ATH_MSG_FATAL("readDeadtimeConfig(): Could not open file "<< fileName );
    exit(-1);
  }

  std::string var;
  float value;

  while(ifs.good()){
	  ifs >> var >> value;
	  	  if(var.compare("deadtimeON") == 0){
	  		  m_deadtimeON = (bool)value;
	  		  ATH_MSG_INFO("m_deadtimeON = " << (bool)value);
	  		  continue;
	  	  }
	  	  if(var.compare("produceDeadDigits") == 0){
	  		  m_produceDeadDigits = (bool)value;
	  		  ATH_MSG_INFO("m_produceDeadDigits = " << (bool)value);
	  		  continue;
	  	  }
	  	  if(var.compare("deadtimeStrip") == 0){
	  		  m_deadtimeStrip = value;
	  		  ATH_MSG_INFO("m_deadtimeStrip = " << value);
	  		  continue;
	  	  }
	  	  if(var.compare("deadtimePad") == 0){
	  		  m_deadtimePad = value;
	  		  ATH_MSG_INFO("m_deadtimePad = " << value);
	  		  continue;
	  	  }
		  if(var.compare("deadtimeWire") == 0){
			  m_deadtimeWire = value;
			  ATH_MSG_INFO("m_deadtimeWire = " << value);
			  continue;
		  }
          if(var.compare("readtimePad") == 0){
	  		  m_readtimePad = value;
	  		  ATH_MSG_INFO("m_readtimePad = " << value);
	  		  continue;
	  	  }
          if(var.compare("readtimeStrip") == 0){
	  		  m_readtimeStrip = value;
	  		  ATH_MSG_INFO("m_readtimeStrip = " << value);
	  		  continue;
	  	  }
	  if(var.compare("readtimeWire") == 0){
			  m_readtimeWire = value;
			  ATH_MSG_INFO("m_readtimeWire = " << value);
			  continue;
		  }
	  	  ATH_MSG_WARNING("Unknown value encountered reading deadtime.config");
  }

  ifs.close();

}

/*******************************************************************************/
uint16_t sTgcDigitizationTool::bcTagging(const float digitTime, const int /*channelType*/) const {

  uint16_t bctag = 0;

//  if(-m_bunchCrossingTime+offset < digitTime && digitTime < -m_bunchCrossingTime+offset+window) {
//    bctag = (bctag | 0x1);
//  }
//  if(                     offset < digitTime && digitTime <                      offset+window) {
//    bctag = (bctag | 0x2);
//  }
//  if(+m_bunchCrossingTime+offset < digitTime && digitTime < +m_bunchCrossingTime+offset+window) {
//    bctag = (bctag | 0x4);
//  }

   int bunchInteger;  //Define the absolute distance from t0 in units of BX
   if(digitTime > 0) bunchInteger = (int)(abs(digitTime/25.0));  //absolute bunch for future bunches
   else bunchInteger = (int)(abs(digitTime/25.0)) + 1; //The absolute bunch for negative time needs to be shifted by 1 as there is no negative zero bunch
   bctag = (bctag | bunchInteger);  //Store bitwise the abs(BX).  This should be equivalent to regular variable assignment
   if(digitTime < 0) bctag = ~bctag;  //If from a PREVIOUS BX, apply bitwise negation

  return bctag;
}

int sTgcDigitizationTool::humanBC(uint16_t bctag) {
    if(bctag << 15 == 1) return ~bctag;
    else return bctag;
}
