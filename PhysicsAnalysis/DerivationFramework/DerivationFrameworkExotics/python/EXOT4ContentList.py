# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Basic content to be written in EXOT4

EXOT4Content = []
EXOT4TruthContent = []

# smart collections
EXOT4SmartCollections = [
    "Electrons",
    "Muons",
    "InDetTrackParticles",
    "PrimaryVertices",
    "AntiKt4EMTopoJets",
    "AntiKt4LCTopoJets",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
    "BTagging_AntiKt4EMTopo",
    "BTagging_AntiKt2Track",
    "BTagging_AntiKt4Track",
    "MET_Reference_AntiKt4EMTopo",
    "MET_Reference_AntiKt4LCTopo"
]

# detailed list of extra variables to be used
EXOT4ExtraVariables = [
    "Electrons.author.Medium.Tight.Loose.charge.truthOrigin.truthType.classifierParticleOrigin.classifierParticleType.classifierParticleOutCome",
    "Muons.charge.truthOrigin.truthType.classifierParticleOrigin.classifierParticleType.classifierParticleOutCome",
    "AntiKt10TruthJets.pt.eta.phi.m",
    "AntiKt2PV0TrackJets.pt.eta.phi.m.constituentLinks.btaggingLink.GhostBHadronsFinal.GhostBHadronsInitial.GhostBQuarksFinal.GhostCHadronsFinal.GhostCHadronsInitial.GhostCQuarksFinal.GhostHBosons.GhostPartons.GhostTQuarksFinal.GhostTausFinal.GhostWBosons.GhostZBosons.GhostTruth.OriginVertex.GhostAntiKt3TrackJet.GhostAntiKt4TrackJet.GhostTrack.GhostTruthAssociationLink",
    "AntiKt4PV0TrackJets.pt.eta.phi.m.constituentLinks.btaggingLink.GhostBHadronsFinal.GhostBHadronsInitial.GhostBQuarksFinal.GhostCHadronsFinal.GhostCHadronsInitial.GhostCQuarksFinal.GhostHBosons.GhostPartons.GhostTQuarksFinal.GhostTausFinal.GhostWBosons.GhostZBosons.GhostTruth.OriginVertex.GhostTrack.GhostTruthAssociationLink",
    "CaloCalTopoClusters.calE.calEta.calPhi.calM.rawM.rawE.rawEta.rawPhi.e_sampl.eta_sampl.etaCalo.phiCalo",
    "AntiKt4TruthJets.pt.eta.phi.m",
    "AntiKt4TruthWZJets.pt.eta.phi.m",
    #"CamKt15LCTopoJets.pt.eta.phi.m.ECF1.ECF2.ECF3.Tau1_wta.Tau2_wta.Tau3_wta.Split12.Split23.NTrimSubjets.Parent.GhostAntiKt2TrackJet.NTrimSubjets.JetConstitScaleMomentum_pt.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_m.constituentLinks",#FIX #ATLJETMET-744
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.constituentLinks.Qw.ThrustMin.ThrustMaj.Angularity.PlanarFlow.Aplanarity.FoxWolfram2.FoxWolfram0.ZCut12.KtDR.Sphericity.Dip12.EMFrac",
    "AntiKt10TruthTrimmedPtFrac5SmallR20Jets.pt.eta.phi.m.Tau2_wta.Tau3_wta.ECF1.ECF2.ECF3",
    "AntiKt4EMTopoJets.EMFrac",
    "AntiKt4LCTopoJets.EMFrac"
]

EXOT4AllVariables = []

# all variables in the truth containers are kept, but they are thinned
EXOT4AllVariablesTruth = [
    "TruthParticles",
    "TruthEvents",
    "TruthVertices"
]
