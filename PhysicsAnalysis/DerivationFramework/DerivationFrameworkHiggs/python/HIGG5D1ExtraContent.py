# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content

import HIGG5Common
ExtraContent=HIGG5Common.getHIGG5Common()
ExtraContent+=[
    "Photons.f3core",
    "AntiKt4EMTopoJets.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.NumTrkPt500PV.PartonTruthLabelID",
    "AntiKt4EMPFlowJets.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.NumTrkPt500PV.PartonTruthLabelID",
    "AntiKt4EMTopoJets.ConstituentScale.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.InputType.AlgorithmType.SizeParameter.btaggingLink.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_m.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_pt.JetGhostArea.JetLCScaleMomentum_eta.JetLCScaleMomentum_m.JetLCScaleMomentum_phi.JetLCScaleMomentum_pt.LArBadHVEnergyFrac.LArBadHVNCell.LeadingClusterCenterLambda.LeadingClusterPt.LeadingClusterSecondLambda.LeadingClusterSecondR.N90Constituents.NegativeE.NumTrkPt1000.NumTrkPt500.OotFracClusters10.OotFracClusters5.OriginCorrected.OriginVertex.PartonTruthLabelID.PileupCorrected.SumPtTrkPt1000.SumPtTrkPt500.Timing.TrackSumMass.TrackSumPt.TrackWidthPt1000.TrackWidthPt500.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.Width.WidthPhi.ActiveArea.AverageLArQF.BchCorrCell.CentroidR.Charge.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.ConeTruthLabelID.DFCommonJets_Calib_eta.DFCommonJets_Calib_m.DFCommonJets_Calib_phi.DFCommonJets_Calib_pt.DetectorEta.DetectorPhi.ECPSFraction.EMFrac.EnergyPerSampling.FracSamplingMax.FracSamplingMaxIndex.GhostAntiKt2TrackJet.GhostAntiKt2TrackJetCount.GhostAntiKt2TrackJetPt.GhostAntiKt4TrackJet.GhostAntiKt4TrackJetCount.GhostAntiKt4TrackJetPt.GhostBHadronsFinal.GhostBHadronsFinalCount.GhostBHadronsFinalPt.GhostBHadronsInitial.GhostBHadronsInitialCount.GhostBHadronsInitialPt.GhostBQuarksFinal.GhostBQuarksFinalCount.GhostBQuarksFinalPt.GhostCHadronsFinal.GhostCHadronsFinalCount.GhostCHadronsFinalPt.GhostCHadronsInitial.GhostCHadronsInitialCount.GhostCHadronsInitialPt.GhostCQuarksFinal.GhostCQuarksFinalCount.GhostCQuarksFinalPt.GhostHBosons.GhostHBosonsCount.GhostHBosonsPt.GhostMuonSegment.GhostMuonSegmentCount.GhostPartons.GhostPartonsCount.GhostPartonsPt.GhostTQuarksFinal.GhostTQuarksFinalCount.GhostTQuarksFinalPt.GhostTausFinal.GhostTausFinalCount.GhostTausFinalPt.GhostTrack.GhostTrackAssociationFraction.GhostTrackAssociationLink.GhostTrackCount.GhostTrackPt.GhostTruth.GhostTruthAssociationFraction.GhostTruthAssociationLink.GhostTruthCount.GhostTruthPt.GhostWBosons.GhostWBosonsCount.GhostWBosonsPt.GhostZBosons.GhostZBosonsCount.GhostZBosonsPt.HECFrac.HECQuality.HadronConeExclExtendedTruthLabelID.HadronConeExclTruthLabelID.HighestJVFVtx.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_MVfJVT.DFCommonJets_MVfJVT_Sumcle.DFCommonJets_MVfJVT_SumclIso.DFCommonJets_MVfJVT_SumclEMprob.DFCommonJets_MVfJVT_LeadclWidth.DFCommonJets_MVfJVT_LeadclSecondLambda",
    "AntiKt4EMPFlowJets.btagging.btaggingLink.GhostTruthAssociationLink.HadronConeExclTruthLabelID.PartonTruthLabelID.Timing.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_m.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_pt,TruthLabelID.constituentLinks.GhostBHadronsFinal.GhostBHadronsInitial.GhostBQuarksFinal.GhostCHadronsFinal.GhostCHadronsInitial.GhostCQuarksFinal.GhostHBosons.GhostPartons.GhostTQuarksFinal.GhostTausFinal.GhostWBosons.GhostZBosons.GhostTruth.OriginVertex.GhostAntiKt3TrackJet.GhostAntiKt4TrackJet.GhostMuonSegment.HighestJVFVtx.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.HighestJVFLooseVtx.GhostAntiKt2TrackJet.JvtJvfcorr.SumPtTrkPt1000.TrackWidthPt500.NegativeE,BTagging_AntiKt4EMPFlow.MV1_discriminant.MV1c_discriminant.SV1_pb.SV1_pu.IP3D_pb.IP3D_pu.MV2c00_discriminant.MV2c10_discriminant.MV2c20_discriminant.MVb_discriminant.MSV_vertices.SV0_badTracksIP.SV0_vertices.SV1_badTracksIP.SV1_vertices.BTagTrackToJetAssociator.BTagTrackToJetAssociatorBB.JetFitter_JFvertices.JetFitter_tracksAtPVlinks.MSV_badTracksIP.MV2c100_discriminant.MV2m_pu.MV2m_pc.MV2m_pb.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_eta",
    ]

ExtraContentTruth=HIGG5Common.getHIGG5CommonTruth()

ExtraContainers=[]
ExtraContainersTruth=HIGG5Common.getHIGG5CommonTruthContainers()
