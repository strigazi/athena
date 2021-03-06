/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @class  : TrigRNNOutputCnv_p1
 *
 * @brief transient persistent converter for TrigRNNOutput
 *
 * @author Danilo Enoque Ferreira de Lima  <dferreir@mail.cern.ch> - UFRJ
 *
 * File and Version Information:
 * $Id$
 **********************************************************************************/
#ifndef TRIGEVENTTPCNV_TRIGRNNOUTPUTCNV_P1_H
#define TRIGEVENTTPCNV_TRIGRNNOUTPUTCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigCaloEvent/TrigRNNOutput.h"
#include "TrigCaloEventTPCnv/TrigRNNOutput_p1.h"

#include "TrigCaloEventTPCnv/TrigRNNOutputConverterBase.h"

#include "AthLinks/ElementLink.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"

class MsgStream;

template <>
class TrigRNNOutputConverterBase<TrigRNNOutput_p1>  : public T_AthenaPoolTPCnvConstBase<TrigRNNOutput, TrigRNNOutput_p1>  {

 public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TrigRNNOutputConverterBase() { }

  virtual
  void persToTrans(const TrigRNNOutput_p1 *persObj,  TrigRNNOutput *transObj, MsgStream &log) const override;
  virtual
  void transToPers(const TrigRNNOutput *transObj, TrigRNNOutput_p1 *persObj, MsgStream &log) const override;

 protected:
  ElementLinkCnv_p1< ElementLink<TrigEMClusterContainer> > m_ELinkTrigEMClusterCnv;
};

typedef TrigRNNOutputConverterBase<TrigRNNOutput_p1> TrigRNNOutputCnv_p1;

#endif

