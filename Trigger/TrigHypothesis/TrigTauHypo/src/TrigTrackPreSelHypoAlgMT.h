/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGTAUHYPO_TRIGTRACKPRESELHYPOALG_H
#define TRIGTAUHYPO_TRIGTRACKPRESELHYPOALG_H

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthViews/View.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/HypoBase.h"

#include "ITrigTrackPreSelHypoTool.h"

class TrigTrackPreSelHypoAlgMT : public ::HypoBase {
 public: 

  TrigTrackPreSelHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrigTrackPreSelHypoAlgMT(); 

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;
  virtual StatusCode  finalize() override;
 
 private: 
  TrigTrackPreSelHypoAlgMT();
  ToolHandleArray< ITrigTrackPreSelHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
     
  SG::ReadHandleKey< TrackCollection > m_fastTracksKey { this, "trackcollection", "trackcollection", "fast tracks in view" };
  

}; 

#endif //> !TRIGTAUHYPO_TRIGTRACKPRESELHYPOALG_H
