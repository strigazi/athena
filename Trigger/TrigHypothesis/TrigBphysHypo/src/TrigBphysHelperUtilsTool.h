/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////// -*- C++ -*- /////////////////////////////
// TrigBphysHelperUtilsTool.h 
// Header file for class TrigBphysHelperUtilsTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef TRIGBPHYSHYPO_TRIGBPHYSHELPERUTILSTOOL_H
#define TRIGBPHYSHYPO_TRIGBPHYSHELPERUTILSTOOL_H 1

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/Muon.h"
#include "xAODTrigBphys/TrigBphys.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "BeamSpotConditionsData/BeamSpotData.h"

// Forward declaration
namespace Trk {
    class Track;
}

static const InterfaceID IID_TrigBphysHelperUtilsTool("TrigBphysHelperUtilsTool", 1 , 0);

class TrigBphysHelperUtilsTool: virtual public ::AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  TrigBphysHelperUtilsTool( const std::string& type,
	     const std::string& name, 
	     const IInterface* parent );

  /// Destructor: 
  virtual ~TrigBphysHelperUtilsTool(); 

  // Athena algtool's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  finalize();

    static const InterfaceID& interfaceID() {return IID_TrigBphysHelperUtilsTool;}
    
  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

    static int sumCharge(const xAOD::TrackParticle *p1, const xAOD::TrackParticle* p2);

    double deltaPhi( double phi1, double phi2) const;
    double deltaEta( double eta1, double eta2) const;

    double absDeltaPhi( double phi1, double phi2) const { return fabs( deltaPhi(phi1,phi2)); }
    double absDeltaEta( double eta1, double eta2) const { return fabs( deltaEta(eta1,eta2)); }

    double deltaR(double eta1, double phi1, double eta2, double phi2) const;
    double deltaR(double deta, double dphi) const;

    const xAOD::EventInfo* getEventInfo() const;
    StatusCode getRunEvtLb(uint32_t & run, uint32_t & evt,uint32_t & lb) const;
    
    
  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
    void addUnique(const xAOD::Muon* muon, std::vector<const xAOD::Muon*> & output,
                   double dEtaCut = 0.005, double dPhiCut=0.005, double dPtCut =-1,
                   xAOD::Muon::TrackParticleType ptype= xAOD::Muon::InnerDetectorTrackParticle) const; ///
    
    bool areUnique(const xAOD::TrackParticle* t0, const xAOD::TrackParticle* t1, double dEtaCut = 0.005, double dPhiCut=0.005, double dPtCut =-1) const;
    
    StatusCode buildDiMu(const std::vector<ElementLink<xAOD::TrackParticleContainer> > & particles,
                         xAOD::TrigBphys *& result,
                         xAOD::TrigBphys::pType ptype,
                         xAOD::TrigBphys::levelType plevel);
    
    StatusCode vertexFit(xAOD::TrigBphys * result,
                         const std::vector<ElementLink<xAOD::TrackParticleContainer> > &particles,
                         const std::vector<double>& inputMasses) const;

    StatusCode vertexFit(xAOD::TrigBphys * result,
                         const std::vector<ElementLink<xAOD::TrackParticleContainer> > &particles,
                         const std::vector<double>& inputMasses,
                         Trk::IVKalState& istate) const;


    StatusCode vertexFit(xAOD::TrigBphys * result,
		       const std::vector<const xAOD::TrackParticle*> &trks,
		       const std::vector<double>& inputMasses) const;

    // simplify this using templates perhaps? 
    double invariantMass(const xAOD::IParticle *p1, const xAOD::IParticle* p2, double m1, double m2) const;
    double invariantMass(const xAOD::TrackParticle *p1, const xAOD::TrackParticle* p2, double m1, double m2) const;
    double invariantMassIP(const std::vector<const xAOD::IParticle*>&ptls, const std::vector<double> & masses) const;
    double invariantMass(const std::vector<const xAOD::TrackParticle*>&ptls, const std::vector<double> & masses) const;

    template<size_t N>
    static double invariantMass(const std::array<const xAOD::TrackParticle*, N> &tracks, const std::array<double, N> &masses);

    /// Fill an xAOD object with pt, rap,phi
    void fillTrigObjectKinematics(xAOD::TrigBphys* bphys, const std::vector<const xAOD::TrackParticle*> &ptls);
    
    Amg::Vector3D getBeamSpot(const EventContext& ctx) const;

    /// Use the fitted position and the beamline to determine lxy, tau, etc.
    /// call after setting the kinematic values, to do ok.
    /// Try to factor getBeamSpot outside of loop.
    void setBeamlineDisplacement(xAOD::TrigBphys* bphys,const std::vector<const xAOD::TrackParticle*> &ptls,
                       const Amg::Vector3D& beamSpot);

    std::unique_ptr<Trk::IVKalState> makeVKalState() const;

    
  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 
  static double invariantMassInternal(const xAOD::TrackParticle* const* tracks , const double*  masses , size_t N);
  /// Default constructor: 
  TrigBphysHelperUtilsTool();

  // Containers
  
    // tools
    ToolHandle < Trk::IVertexFitter  >       m_fitterSvc;
    Trk::TrkVKalVrtFitter*              m_VKVFitter;
    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
    static constexpr double s_massMuon = 105.6583715;
};

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

template<size_t N>
double TrigBphysHelperUtilsTool::invariantMass(const std::array<const xAOD::TrackParticle*, N> &tracks, const std::array<double, N> &masses){
   return invariantMassInternal(tracks.data(), masses.data(), N);
}


#endif //> !TRIGBPHYSHYPO_TRIGBPHYSHELPERUTILSTOOL_H
