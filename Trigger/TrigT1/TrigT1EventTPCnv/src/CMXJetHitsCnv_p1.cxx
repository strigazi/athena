/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigT1CaloEvent/CMXJetHits.h"
#undef private
#undef protected


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/CMXJetHits_p1.h"
#include "TrigT1EventTPCnv/CMXJetHitsCnv_p1.h"

/*
CMXJetHitsCnv_p1::CMXJetHitsCnv_p1()
  : T_AthenaPoolTPCnvBase< CMXJetHits, CMXJetHits_p1 >() {

}
*/

void CMXJetHitsCnv_p1::persToTrans( const CMXJetHits_p1* persObj, CMXJetHits* transObj, MsgStream &log ) {

  //log << MSG::INFO << "Converting CMXJetHits from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the CMXJetHits
  // 
  transObj->m_crate  = persObj->m_crate;
  transObj->m_source = persObj->m_source;
  transObj->m_peak   = persObj->m_peak; 
  transObj->m_hits0  = persObj->m_hits0;
  transObj->m_hits1  = persObj->m_hits1;
  transObj->m_error0 = persObj->m_error0;
  transObj->m_error1 = persObj->m_error1;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted CMXJetHits from persistent state [OK]" << endreq;

  return;

}

void CMXJetHitsCnv_p1::transToPers( const CMXJetHits* transObj, CMXJetHits_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of CMXJetHits..." << endreq;

  persObj->m_crate    = transObj->m_crate;
  persObj->m_source   = transObj->m_source;
  persObj->m_peak     = transObj->m_peak;
  persObj->m_hits0    = transObj->m_hits0;
  persObj->m_hits1    = transObj->m_hits1;
  persObj->m_error0   = transObj->m_error0;
  persObj->m_error1   = transObj->m_error1;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of CMXJetHits [OK]" << endreq;

  return;

}
