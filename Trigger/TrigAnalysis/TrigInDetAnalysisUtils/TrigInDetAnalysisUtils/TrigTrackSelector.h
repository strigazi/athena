/* emacs: this is -*- c++ -*- */
/**
 **     @file    TrigTrackSelector.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:39 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TIDAUTILS_TRIGTRACKSELECTOR_H
#define TIDAUTILS_TRIGTRACKSELECTOR_H

#include "TrigInDetAnalysisUtils/TIDA_newtracking.h"

#ifndef  TIDA_NEWTRACKING_H
#include "TrigInDetAnalysisUtils/TrigTrackSelector_old.h"
#else

/// L2 tracks
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
///TruthMap
#include "TrigInDetTruthEvent/TrigInDetTrackTruthMap.h"

/// offline and EF
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

/// MC truth
#include "McParticleEvent/TruthParticleContainer.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"
#include "xAODTruth/TruthParticleContainer.h"


///// FrameWork includes
/// #include "GaudiKernel/ServiceHandle.h"
/// #include "GaudiKernel/IPartPropSvc.h"
/// absolutely pathetic!! write my own class

#include "TrigInDetAnalysisUtils/particleType.h"

/// TrigInDetAnalysis classes
#include "TrigInDetAnalysis/TrackSelector.h"
#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TrackFilter.h"

#include "TrkParameters/TrackParameters.h"
// #include "TrkParameters/Perigee.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/InDetTrackSummary.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "GaudiKernel/ToolHandle.h" 
#include "TMath.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

class TrigTrackSelector : public TrackSelector { 

public:

  /// use a radius of 47 mm corresponding to the Run 1 pixel inner radius
  /// For the IBL it should be 32 mm, but this was kept as 47 mm for consistency 
  /// of the definition. this should be changed to 32 mm for Run 3
  TrigTrackSelector( TrackFilter* selector, double radius=47 );

  ~TrigTrackSelector() { clear(); }


  void setBeamline( double x, double y, double z=0) { m_xBeam = x; m_yBeam = y; m_zBeam=z; }

  double getBeamX() const { return m_xBeam; } 
  double getBeamY() const { return m_yBeam; } 
  double getBeamZ() const { return m_zBeam; } 

  void correctTracks(bool b=true) { m_correctTrkTracks = b; } 

  virtual void clear() { for ( size_t i=m_tracks.size() ; i-- ; ) delete m_tracks[i]; m_tracks.clear(); }   


  bool selectTrack( const TrigInDetTrack* track, const TrigInDetTrackTruthMap* truthMap=0 );


  // extract all the tracks from a TrigInDetTrack collection and associated TruthMap and convert them
  void selectTracks( const TrigInDetTrackCollection* trigtracks, const TrigInDetTrackTruthMap* truthMap=0 );

  // add a TrackParticle 
  bool selectTrack( const Rec::TrackParticle* track );
  

  // extract all the tracks from a TrackParticle collection and add them
  void selectTracks( const Rec::TrackParticleContainer* trigtracks );


  // extract all the tracks from a TrackParticle collection and add them
  void selectTracks( const TruthParticleContainer* truthtracks );

  // extract all the tracks from a TrackParticle collection and add them
  void selectTracks( const xAOD::TruthParticleContainer* truthtracks );


  bool selectTrack( const HepMC::GenParticle* track );


  // add a TruthParticle 
  bool selectTrack( const TruthParticle& track );

  // add a TruthParticle 
  bool selectTrack( const TruthParticle* track );

  bool selectTrack( const xAOD::TruthParticle* track );


  // make a TIDA::Track from a GenParticle 
  TIDA::Track* makeTrack( const HepMC::GenParticle* track );

  // make a TIDA::Track from a TruthParticle 
  TIDA::Track* makeTrack( const TruthParticle* track, unsigned long tid=0 );


  // add a Trk::Track
  bool selectTrack( const Trk::Track* track );

  
  // extract all the tracks from a TrackCollection and add them
  void selectTracks( const TrackCollection* trigtracks );


  /// legacy run 2 selector access

  bool selectTrack( const xAOD::TrackParticle* track, void* =0);

  void selectTracks( const xAOD::TrackParticleContainer* tracks, void* =0);

  /// MT selector access

  void selectTracks( xAOD::TrackParticleContainer::const_iterator trackitr, 
		     xAOD::TrackParticleContainer::const_iterator trackend, void* =0);



  void correctToBeamline( double& z0,    double& dz0, 
			  double& d0,    double& dd0, 
			  double  theta, double  phi );

  

private:

  unsigned long m_id;

  double m_xBeam;
  double m_yBeam;
  double m_zBeam;

  bool   m_correctTrkTracks;

  double m_radius;

};


#endif //   TIDA_NEWTRACKING_H
#endif // TIDAUTILS_TRIGTRACKSELECTOR_H
