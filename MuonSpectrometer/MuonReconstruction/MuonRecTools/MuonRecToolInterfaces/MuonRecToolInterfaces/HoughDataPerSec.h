/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRECTOOLINTERFACES_HOUGHDATAPERSEC_H
#define MUONRECTOOLINTERFACES_HOUGHDATAPERSEC_H

#include "AthenaKernel/CLASS_DEF.h"
#include "MuonLayerHough/MuonLayerHough.h"
#include "MuonLayerHough/MuonPhiLayerHough.h"
#include "MuonLayerHough/MuonRegionHough.h"
#include "MuonClusterization/TgcHitClustering.h"
#include <map>
#include <set>
#include <vector>
#include <memory>


namespace Muon {
  struct HoughDataPerSec {
    typedef std::vector<MuonHough::Hit*>    HitVec;
    typedef std::vector< HitVec >           RegionHitVec;
    typedef std::vector<MuonHough::PhiHit*> PhiHitVec;
    typedef std::vector< PhiHitVec >        RegionPhiHitVec;
    typedef std::vector<MuonHough::MuonLayerHough::Maximum*>    MaximumVec;
    typedef std::vector<MuonHough::MuonPhiLayerHough::Maximum*> PhiMaximumVec; 
    typedef std::map<MuonHough::MuonLayerHough::Maximum*, MaximumVec > MaximumAssociationMap;
    typedef std::vector< MaximumVec >       RegionMaximumVec;
    typedef std::vector< PhiMaximumVec >    RegionPhiMaximumVec;

    HoughDataPerSec() {
      sector = -1;
      hitVec.resize(MuonStationIndex::sectorLayerHashMax());
      maxVec.resize(MuonStationIndex::sectorLayerHashMax());
      phiHitVec.resize(MuonStationIndex::DetectorRegionIndexMax);
      phiMaxVec.resize(MuonStationIndex::DetectorRegionIndexMax);
      nlayersWithMaxima.resize(MuonStationIndex::DetectorRegionIndexMax);
      nphilayersWithMaxima.resize(MuonStationIndex::DetectorRegionIndexMax);
      nmaxHitsInRegion.resize(MuonStationIndex::DetectorRegionIndexMax);
      nphimaxHitsInRegion.resize(MuonStationIndex::DetectorRegionIndexMax);
    }

    ~HoughDataPerSec() {
      cleanUp();
    }

    void cleanUp() {
      for(RegionHitVec::iterator it=hitVec.begin();it!=hitVec.end();++it)
        for( HitVec::iterator it2=it->begin();it2!=it->end();++it2 ) delete *it2;
      hitVec.clear();

      for(RegionPhiHitVec::iterator it=phiHitVec.begin();it!=phiHitVec.end();++it)
        for( PhiHitVec::iterator it2=it->begin();it2!=it->end();++it2 ) delete *it2;
      phiHitVec.clear();

      for(RegionMaximumVec::iterator it=maxVec.begin();it!=maxVec.end();++it)
        for( MaximumVec::iterator it2=it->begin();it2!=it->end();++it2 ) delete *it2;
      maxVec.clear();

      for(RegionPhiMaximumVec::iterator it=phiMaxVec.begin();it!=phiMaxVec.end();++it)
        for( PhiMaximumVec::iterator it2=it->begin();it2!=it->end();++it2 ) delete *it2;
      phiMaxVec.clear();
    }

    int                   sector;
    RegionHitVec          hitVec; // Owns the contained objects
    RegionPhiHitVec       phiHitVec; // Owns the contained objects
    RegionMaximumVec      maxVec; // Owns the contained objects
    RegionPhiMaximumVec   phiMaxVec; // Owns the contained objects
    std::vector<int>      nlayersWithMaxima;
    std::vector<int>      nphilayersWithMaxima;
    std::vector<int>      nmaxHitsInRegion;
    std::vector<int>      nphimaxHitsInRegion;
    MaximumAssociationMap maxAssociationMap; // stores association of a given maximium with other maxima in neighbouring sectors
    std::set<MuonHough::MuonLayerHough::Maximum*> associatedToOtherSector; // used to flagged maxima that were already associated to another sector
    // returns the number of phi and eta hits in the region with most eta hits
    // regions with phi hits are always prefered over regions without
    int maxEtaHits() const{
      return std::max( nmaxHitsInRegion[0], std::max( nmaxHitsInRegion[1], nmaxHitsInRegion[2] ) );
    }
  };

  struct HoughDataPerSectorVec
  {
    std::vector<HoughDataPerSec> vec;
    MuonHough::MuonDetectorHough detectorHoughTransforms; // Kept with the vec because it has references to these objects
    std::vector<std::unique_ptr<TgcHitClusteringObj>> tgcClusteringObjs; // Kept with the vec because it has references to these objects
  };
}

CLASS_DEF(Muon::HoughDataPerSec, 163257499, 1)
CLASS_DEF(Muon::HoughDataPerSectorVec, 61014906, 1)

#endif
