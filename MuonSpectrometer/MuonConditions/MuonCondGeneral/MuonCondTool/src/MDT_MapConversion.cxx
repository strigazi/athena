/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"
#include "SGTools/TransientAddress.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "PathResolver/PathResolver.h"
#include <fstream>
#include <string>
#include <stdio.h>
#include <map>

#include "MuonCondTool/MDT_MapConversion.h"


#include "MuonCondSvc/MdtStringUtils.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the MDT Map Info from ORACLE DB
//* retrieving mapping conversion tables from DB
//* convert method online name into offline ID 
//*********************************************************


MDT_MapConversion::MDT_MapConversion (const std::string& type,
				    const std::string& name,
				    const IInterface* parent)
	  : AlgTool(type, name, parent) 
{
  
  declareInterface< IMDT_MapConversion >(this);
  
}


StatusCode MDT_MapConversion::initialize()
{
  MsgStream log(msgSvc(), name());
 
   
  StatusCode sc = serviceLocator()->service("DetectorStore", m_detStore);
  if ( sc.isSuccess() ) {
    log << MSG::DEBUG << "Retrieved DetectorStore" << endreq;
  }else{
    log << MSG::ERROR << "Failed to retrieve DetectorStore" << endreq;
    return sc;
  }
  


  sc = m_detStore->retrieve(m_mdtIdHelper, "MDTIDHELPER" );
  if (sc.isFailure())
    {
      log << MSG::FATAL << " Cannot retrieve MdtIdHelper " << endreq;
      return sc;
    }
  
    
  if(sc.isFailure()) return StatusCode::FAILURE;
  
  
  
  // initialize the chrono service
  sc = service("ChronoStatSvc",m_chronoSvc);
  if (sc != StatusCode::SUCCESS) {
    log << MSG::ERROR << "Could not find the ChronoSvc" << endreq;
    return sc;
  }
	
 
  
  if(sc.isFailure()) return StatusCode::FAILURE;

  
  IGeoModelSvc *geoModel;
  service ("GeoModelSvc",geoModel); 
  
  std::string AtlasVersion = geoModel->atlasVersion();
  std::string MuonVersion = geoModel->muonVersionOverride();
  std::string detectorKey = MuonVersion.empty() ? AtlasVersion : MuonVersion;
  std::string detectorNode = MuonVersion.empty() ? "ATLAS" : "MuonSpectrometer";

 
  IRDBAccessSvc *accessSvc;
  service("RDBAccessSvc",accessSvc);
  accessSvc->connect();
  
  

//   const IRDBRecordset *switchSet = accessSvc->getRecordset("HwSwIdMapping", detectorKey, detectorNode);
 
//   if ((*switchSet).size()==0) {
//     log<< MSG::WARNING <<"Old Atlas Version :  "<< AtlasVersion << " Only Online Identifier"<<endreq;
//     return StatusCode::SUCCESS;
    
//   }
 

  const IRDBRecordset *switchSet = accessSvc->getRecordset("HwSwIdMapping", detectorKey, detectorNode);

  if ((*switchSet).size()==0) {
    log<< MSG::WARNING <<"Old Atlas Version : "<< AtlasVersion << " Only Online Identifier. Falling back to HwSwIdMapping-00 tag"<<endreq;
    switchSet = accessSvc->getRecordset("HwSwIdMapping","HwSwIdMapping-00");
  } 
  


  for (unsigned int irow=0; irow<(*switchSet).size(); ++irow){
    
    const IRDBRecord *switches = (*switchSet)[irow];
    std::string hardwareName = switches->getString("HARDNAME");
    std::string stName = switches->getString("SOFTNAME");
    int stPhi = switches->getInt("SOFTOCTANT");
    int stEta = switches->getInt("SOFTIZ");
    // log << MSG::INFO << "*********************" <<hardwareName<< endreq; 
    //log << MSG::INFO << "*********************" <<stName<< endreq;
    //log << MSG::INFO << "*********************" <<stPhi<< endreq;
    // log << MSG::INFO << "*********************" <<stEta<< endreq;
    Identifier ChamberId = m_mdtIdHelper->elementID(stName,stEta,stPhi);
    //log << MSG::INFO << "#### Chamber Name Offline" << ChamberId<< endreq;
    
    Chamber_Map.insert(std::make_pair(hardwareName,ChamberId));
    
  }
	
    accessSvc->disconnect(); 
    log << MSG::INFO << "#### Chamber Map SIZE" << Chamber_Map.size()<< endreq;
  return StatusCode::SUCCESS;
 
  
}

const Identifier& MDT_MapConversion::ConvertToOffline(const std::string &OnlineId) const
{
  MsgStream log(msgSvc(), name());
//  int size = Chamber_Map.size();
  //log << MSG::VERBOSE << "*************** size is\n" << size<< endreq;
  std::map<std::string, Identifier>::iterator iter;
  
  //const Identifier m_Online_empty;
  
  if (Chamber_Map.size()!=0){
    const Identifier & m_OfflineName = (Chamber_Map.find(OnlineId)->second);
    return m_OfflineName;
  }
  else return m_Online_empty;
  
  
}





