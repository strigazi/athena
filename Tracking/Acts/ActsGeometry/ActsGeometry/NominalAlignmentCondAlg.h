/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

// ATHENA
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "EventInfo/EventInfo.h"
#include "GaudiKernel/ICondSvc.h"
#include "StoreGate/StoreGateSvc.h"

// PACKAGE
#include "ActsGeometry/IActsTrackingGeometrySvc.h"
#include "ActsGeometry/ActsAlignmentStore.h"

// STL
#include <string>


/// @class NominalAlignmentCondAlg
/// Conditions algorithm which produces an (effectively)
/// infinitely valid ActsAlignmentStore which has
/// nominal alignments (= identity deltas)
///
class NominalAlignmentCondAlg  :  public AthAlgorithm {
  
public:
    
  NominalAlignmentCondAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~NominalAlignmentCondAlg();
  
  virtual bool isClonable() const override { return true; }

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

private:
  
  SG::WriteCondHandleKey<ActsAlignmentStore> m_wchk {this, "PixelAlignmentKey", "PixelAlignment", "cond handle key"};

  ServiceHandle<ICondSvc> m_cs;
  ServiceHandle<IActsTrackingGeometrySvc> m_trackingGeometrySvc;

};

