/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZmumuMuonTPSelectionTool.h
 *
 *  Created on: Aug 26, 2014
 *      Author: goblirsc<at>CERN.CH
 */

#ifndef ZMUMUMUONTPSELECTIONTOOL_H_
#define ZMUMUMUONTPSELECTIONTOOL_H_

/// This is the implementation of the IMuonTPSelectionTool for the Z->mm tag and probe
/// efficiency measurement.
#include "MuonTPTools/MuonTPSelectionTool.h"
#include "AsgTools/AsgTool.h"

class ZmumuMuonTPSelectionTool:
        virtual public IMuonTPSelectionTool,
        virtual public asg::AsgTool,
        public MuonTPSelectionTool{
      ASG_TOOL_CLASS(ZmumuMuonTPSelectionTool, IMuonTPSelectionTool)
public:
    ZmumuMuonTPSelectionTool(std::string myname);

    virtual StatusCode initialize();

    /// Select Probes
    ProbeContainer* selectProbes(const xAOD::MuonContainer*, const xAOD::IParticleContainer*) const;

    /// Get Efficiency Flag
    std::string efficiencyFlag() {return m_efficiencyFlag;}

    /// Check if Probe and Tag are the same object
    bool isTag(const xAOD::Muon* tag, const xAOD::IParticle* probe) const;

    // helper method to check if the probe is the right type
    bool isRightType(const xAOD::IParticle* probe) const;

    bool PassProbeKinematics (const xAOD::IParticle* probe) const;

    bool PassProbeIPCuts(const xAOD::IParticle* probe) const;
    bool PassTagIPCuts(const xAOD::Muon* tag) const;

    bool PassProbeIsoCuts (const xAOD::IParticle* probe) const;
    bool PassTagIsoCuts (const xAOD::Muon* tag) const;

private:

    bool m_accept_sameCharge;
    bool m_accept_oppCharge;

    bool m_probe_ID_hits;

    double m_tagPtConeIso;
    double m_probePtConeIso;
    double m_tag_antiPtConeIso;
    double m_probe_antiPtConeIso;

    double m_tagEtConeIso;
    double m_probeEtConeIso;
    double m_tag_antiEtConeIso;
    double m_probe_antiEtConeIso;

    bool m_IDProbe;
    bool m_CaloProbe;
    bool m_CBProbe;
    bool m_MSProbe;
    bool m_TruthProbe;

    bool m_only_A_side;
    bool m_only_C_side;
    double m_probe_abseta_min;
    double m_probe_abseta_max;

    double m_tag_d0;
    double m_tag_d0Sign;
    double m_tag_z0;
    double m_probe_d0;
    double m_probe_d0Sign;
    double m_probe_z0;


      double m_deltaPhiCut;

};


#endif /* ZMUMUMUONTPSELECTIONTOOL_H_ */
