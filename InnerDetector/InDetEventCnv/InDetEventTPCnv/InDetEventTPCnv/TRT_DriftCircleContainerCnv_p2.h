// -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DRIFTCIRCLECONTAINERCNV_P2_H
#define TRT_DRIFTCIRCLECONTAINERCNV_P2_H

// TRT_DriftCircleContainerCnv_p2, T/P separation of TRT PRD
// author D.Costanzo <davide.costanzo@cern.ch>

#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetEventTPCnv/TRT_DriftCircleContainer_p2.h"
#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"
#include "StoreGate/ReadCondHandleKey.h"
class TRT_ID;
class StoreGateSvc;

class TRT_DriftCircleContainerCnv_p2 : 
public T_AthenaPoolTPCnvBase<InDet::TRT_DriftCircleContainer, InDet::TRT_DriftCircleContainer_p2>
{
 public:
  TRT_DriftCircleContainerCnv_p2() :
    m_trtId{nullptr},
    m_storeGate{nullptr},
    m_isInitialized{false}, 
    m_trtDetEleContKey{"TRT_DetElementContainer"},
    m_useDetectorElement{true}
    {};

  virtual void persToTrans(const InDet::TRT_DriftCircleContainer_p2* persCont,
                           InDet::TRT_DriftCircleContainer* transCont,
                           MsgStream &log) ;
  virtual void transToPers(const InDet::TRT_DriftCircleContainer* transCont,
                           InDet::TRT_DriftCircleContainer_p2* persCont,
                           MsgStream &log) ;

  virtual InDet::TRT_DriftCircleContainer* createTransient(const InDet::TRT_DriftCircleContainer_p2* persObj, MsgStream& log);

  void setIdHelper(const TRT_ID* trt_id);
  void setUseDetectorElement(const bool useDetectorElement);

 private:
   const TRT_ID *m_trtId;
   StoreGateSvc *m_storeGate;
   bool m_isInitialized;
   SG::ReadCondHandleKey<InDetDD::TRT_DetElementContainer> m_trtDetEleContKey;
   bool m_useDetectorElement;
   StatusCode initialize(MsgStream &log);


};

#endif
