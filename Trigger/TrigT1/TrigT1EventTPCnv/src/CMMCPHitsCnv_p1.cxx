/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigT1CaloEvent/CMMCPHits.h"
#undef private
#undef protected


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/CMMCPHits_p1.h"
#include "TrigT1EventTPCnv/CMMCPHitsCnv_p1.h"

/*
CMMCPHitsCnv_p1::CMMCPHitsCnv_p1()
  : T_AthenaPoolTPCnvBase< CMMCPHits, CMMCPHits_p1 >() {

}
*/

void CMMCPHitsCnv_p1::persToTrans( const CMMCPHits_p1* persObj, CMMCPHits* transObj, MsgStream &log ) {

  //log << MSG::INFO << "Converting CMMCPHits from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the CMMCPHits
  // 
  transObj->m_crate  = persObj->m_crate;
  transObj->m_dataID = persObj->m_dataID;
  transObj->m_peak   = persObj->m_peak; 
  transObj->m_hits0  = persObj->m_hits0;
  transObj->m_hits1  = persObj->m_hits1;
  transObj->m_error0 = persObj->m_error0;
  transObj->m_error1 = persObj->m_error1;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted CMMCPHits from persistent state [OK]" << endreq;

  return;

}

void CMMCPHitsCnv_p1::transToPers( const CMMCPHits* transObj, CMMCPHits_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of CMMCPHits..." << endreq;

  persObj->m_crate    = transObj->m_crate;
  persObj->m_dataID   = transObj->m_dataID;
  persObj->m_peak     = transObj->m_peak;
  persObj->m_hits0    = transObj->m_hits0;
  persObj->m_hits1    = transObj->m_hits1;
  persObj->m_error0   = transObj->m_error0;
  persObj->m_error1   = transObj->m_error1;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of CMMCPHits [OK]" << endreq;

  return;

}
