/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class CombinedScintillatorSD.
// Sensitive detector for the Scintillator between LAr/Tile
//
// Author: franck Martin <Franck.Martin@cern.ch>
// january 12, 2004
//
//************************************************************

#include "CombinedScintillator/CombinedScintillatorSD.hh"
#include "CombinedScintillator/CombinedScintillatorSDMessenger.hh"
#include "CombinedScintillator/CombinedScintillatorDescription.hh"

#include "TileG4DetDescr/DetectorDescriptionStore.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"

#include "G4HCofThisEvent.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4Trd.hh"

#include "G4UImanager.hh"
#include "G4ios.hh"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/TileTBID.h"
#include "TileSimEvent/TileHitVector.h"

static SensitiveDetectorEntryT<CombinedScintillatorSD> combinedscintillatorSD("CombinedScintillatorSD");

CombinedScintillatorSD::CombinedScintillatorSD(G4String name)
  : FadsSensitiveDetector(name),
    detectorDescription(0),    
    combinedscintillatorSDMessenger(0),
    m_iamowner(false),
    m_hit()
{  
  combinedscintillatorSDMessenger = new CombinedScintillatorSDMessenger(this);
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode status = svcLocator->service("MessageSvc", m_msgSvc);
  MsgStream log(m_msgSvc, "CombinedScintillatorSD");

  if(status.isFailure())
    log << MSG::FATAL << "MessageSvc not found!" << endreq;
  else
    log << MSG::DEBUG << "MessageSvc initialized" << endreq;

  status = svcLocator->service("StoreGateSvc", m_sgSvc);
  if(status.isFailure())
    log << MSG::FATAL << "StoreGateSvc not found!" << endreq;
  else
    log << MSG::DEBUG << "StoreGateSvc initialized" << endreq;

  status = svcLocator->service("DetectorStore", m_detStore);
  if(status.isFailure())
    log << MSG::FATAL << "DetectorStoreSvc not found!" << endreq;
  else
    log << MSG::DEBUG << "DetectorStoreSvc initialized" << endreq;

  status = m_detStore->retrieve(m_tileTBID);
  if(status.isFailure())
    log << MSG::FATAL << "No TileTBID helper" << endreq;
  else
    log << MSG::DEBUG << "TileTBID helper retrieved" << endreq;

  int type=TileTBID::ADC_TYPE, module=TileTBID::CRACK_WALL;
  for (int channel=0; channel<nCell; ++channel) {
    m_id[channel] = m_tileTBID->channel_id(type,module,channel);
  }
}

CombinedScintillatorSD::~CombinedScintillatorSD()
{
  delete combinedscintillatorSDMessenger;
}

void CombinedScintillatorSD::Description(std::string descr_name)
{
  MsgStream log(m_msgSvc, "CombinedScintillatorSD"); 
  log << MSG::DEBUG << "CombinedScintillatorSD, Description" << endreq; 
  DetectorDescriptionStore *st=DetectorDescriptionStore::GetDetectorDescriptionStore();
  DetectorDescription *dd=st->GetDetectorDescription(descr_name);
  CombinedScintillatorDescription* i=dynamic_cast<CombinedScintillatorDescription *>(dd);
  if (i)
    detectorDescription = i;
  else
    log << MSG::FATAL << "Description" << descr_name << "is not of right type combinedScintillatorDescription!" << endreq;
}

void CombinedScintillatorSD::Initialize(G4HCofThisEvent* /* HCE */)
{
  MsgStream log(m_msgSvc, "CombinedScintillatorSD"); 
  log << MSG::DEBUG << "Initializing SD" << endreq;

  memset(m_nhits,0,sizeof(m_nhits));
}

G4bool CombinedScintillatorSD::ProcessHits(G4Step* aStep,G4TouchableHistory* /* ROhist */)
{
  MsgStream log(m_msgSvc, "CombinedScintillatorSD : Process hit");
  MSG::Level logLevel = log.level();
  if (logLevel <= MSG::VERBOSE)
    log << MSG::VERBOSE << "i am in comb. Scinti process hit" << endreq; 

  G4TouchableHistory* theTouchable = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
  G4VPhysicalVolume* physVol = theTouchable->GetVolume();
  G4LogicalVolume* logiVol   = physVol->GetLogicalVolume();
  G4String nameLogiVol       = logiVol->GetName();
  G4int nScinti              = physVol->GetCopyNo(); 

  G4double edep  = aStep->GetTotalEnergyDeposit();
  G4double stepl = 0.;
  G4ThreeVector pStep = aStep->GetDeltaPosition();
  
  if (aStep->GetTrack()->GetDefinition()->GetPDGCharge() != 0.) stepl = aStep->GetStepLength();
    
  if ((edep==0.)&&(stepl==0.)) return false;

  if(nameLogiVol.find("CScintillatorLayer") !=G4String::npos) 
  {
    int ind = nScinti; // Only one scintillator at the test beam
                       // No copy nScinti == 0 !!

    if ( m_nhits[ind] > 0 )
    { 
      if (logLevel <= MSG::VERBOSE)
        log << MSG::VERBOSE << "Additional hit in Combined scint " << nScinti
            << " ene=" << edep << endreq;
      m_hit[ind]->add(edep,0.0,0.0);
    } else
    {
      // First hit in a cell
      if (logLevel <= MSG::VERBOSE)
        log << MSG::VERBOSE << "First hit in Combined scint " << nScinti
            << " ene=" << edep << endreq;
      m_hit[ind] = new TileSimHit(m_id[ind],edep,0.0,0.0);  
    }
    ++m_nhits[ind];
  }  
  return true;
}

void CombinedScintillatorSD::EndOfEvent(G4HCofThisEvent* /* HCE */)
{
  MsgStream log(m_msgSvc, "CombinedScintillatorSD"); 

  TileHitVector* ExtraHitsCollection;   

  if (! m_iamowner) {
    const TileHitVector* ExtraColl;
    StatusCode status = m_sgSvc->retrieve(ExtraColl,"TileTBHits");
    m_iamowner = (status.isFailure());
    ExtraHitsCollection = (TileHitVector *)ExtraColl;
  }

  if (m_iamowner) {
    ExtraHitsCollection = new TileHitVector("TileTBHits"); 
    StatusCode status = m_sgSvc->record(ExtraHitsCollection,ExtraHitsCollection->Name(),true);
    if (status.isFailure())
      log << MSG::FATAL << "Can't initialize TileTBHits vector" << endreq; 
    else
      log << MSG::DEBUG << "I have initialized TileTBHits vector" << endreq; 
  }
  
  for (int ind=0; ind<nCell; ++ind) {
    int nhit= m_nhits[ind];
    if (nhit>0) {
      log << MSG::DEBUG
          << "Cell id=" << m_tileTBID->to_string(m_id[ind])
          << " nhit=" << nhit << " ene=" << m_hit[ind]->energy()
          << endreq;
      ExtraHitsCollection->Insert(TileHit(m_hit[ind]));
      delete m_hit[ind];
    } else {
      log << MSG::VERBOSE
          << "Cell id=" << m_tileTBID->to_string(m_id[ind])
          << " nhit=0"  << endreq;
    }
  }

  log << MSG::DEBUG << "Total number of hits is " << ExtraHitsCollection->size() << endreq;

  if (m_iamowner) {
    StatusCode status = m_sgSvc->setConst(ExtraHitsCollection);
    if(status.isFailure())
      log << MSG::ERROR << "Can't lock Collection 'TileTBHits'" << endreq;
    else
      log << MSG::DEBUG << "Collection 'TileTBHits' is locked " << endreq;
  }
}

