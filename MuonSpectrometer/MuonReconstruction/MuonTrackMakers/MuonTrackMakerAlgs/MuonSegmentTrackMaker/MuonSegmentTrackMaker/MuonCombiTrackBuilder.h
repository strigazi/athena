/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBITRACKBUILDER_H
#define MUONCOMBITRACKBUILDER_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "TrkTrack/TrackCollection.h"

namespace Muon {
  class IMuonCombiTrackMaker;
}

class MuonCombiTrackBuilder : public AthAlgorithm
{
 public:
  MuonCombiTrackBuilder(const std::string& name, ISvcLocator* pSvcLocator);

 public:
  virtual ~MuonCombiTrackBuilder();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:

  SG::ReadHandle<MuonSegmentCombinationCollection>        m_segmentCombiLocation;     //!< Location of input MuonSegmentCombination collection
  SG::WriteHandle<TrackCollection>                        m_trackLocation;            //!< Location of the track output location

  // member set by Joboptions 
  ToolHandle<Muon::IMuonCombiTrackMaker> m_trackMaker;
  
};
  


#endif 
