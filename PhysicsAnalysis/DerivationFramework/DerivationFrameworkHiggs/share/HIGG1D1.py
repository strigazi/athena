#====================================================================
# HIGG1D1.py
# This requires the reductionConf flag HIGG1D1 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

import AthenaCommon.SystemOfUnits as Units
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties

if globalflags.DataSource()=='geant4':
    from DerivationFrameworkHiggs.TruthCategories import *

from DerivationFrameworkCore.LHE3WeightMetadata import *

#====================================================================
# SKIMMING TOOLS 
#====================================================================


print "HIGG1D1.py jobproperties.Beam.energy()", jobproperties.Beam.energy()
SkipTriggerRequirement=((globalflags.DataSource()=='geant4') and (jobproperties.Beam.energy()==4000000.0))
# 8 TeV MC does not have trigger information
print "HIGG1D1.py SkipTriggerRequirement", SkipTriggerRequirement
TriggerExp = []
if not SkipTriggerRequirement:
    if jobproperties.Beam.energy()==4000000.0:
        #  8 TeV data
        TriggerExp               = ["EF_g35_loose_g25_loose"]
    if jobproperties.Beam.energy()==6500000.0:
        # 13 TeV MC
        # TriggerTP               = ["HLT_e28_tight_iloose", "HLT_e60_medium", "HLT_mu26_imedium", "HLT_mu50"]
        TriggerExp               = ["HLT_g35_loose_g25_loose","HLT_g35_medium_g25_medium","HLT_2g50_loose","HLT_2g20_tight","HLT_2g22_tight","HLT_2g20_tight_icalovloose_L12EM15VHI","HLT_2g20_tight_icalotight_L12EM15VHI","HLT_2g22_tight_L12EM15VHI","HLT_2g22_tight_icalovloose_L12EM15VHI","HLT_2g22_tight_icalotight_L12EM15VHI","HLT_2g22_tight_icalovloose","HLT_2g25_tight_L12EM20VH","HLT_2g20_loose","HLT_2g20_loose_L12EM15","HLT_g35_medium_g25_medium","HLT_g35_medium_g25_medium_L12EM15VH","HLT_g35_loose_g25_loose","HLT_g35_loose_g25_loose_L12EM15VH"]
print "HIGG1D1.py TriggerExp", TriggerExp

KeepTrigger=not SkipTriggerRequirement
#KeepTrigger=False

print "HIGG1D1.py KeepTrigger", KeepTrigger

from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG1
SkimmingToolHIGG1D1 = DerivationFramework__SkimmingToolHIGG1(
                                 name = "SkimmingToolHIGG1D1",
                                 RequireGRL = True,
                                 ReqireLArError = True,
                                 RequireTrigger = KeepTrigger,
                                 RequirePreselection = False,
                                 RequireKinematic = False,
                                 RequireQuality = False,
                                 RequireIsolation = False,
                                 RequireInvariantMass = False,
                                 Triggers = TriggerExp,
                                 IncludeSingleElectronPreselection = False,
                                 IncludeDoubleElectronPreselection = False,
                                 IncludeSingleMuonPreselection = False)

ToolSvc += SkimmingToolHIGG1D1
print SkimmingToolHIGG1D1

#================
# THINNING
#================
thinningTools=[]
# MET/Jet tracks
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG1D1MetTPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "HIGG1D1MetTPThinningTool",
                                                                       ThinningService         = "HIGG1D1ThinningSvc",
                                                                       SelectionString         = thinning_expression,
                                                                       InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                       ApplyAnd                = True)
ToolSvc += HIGG1D1MetTPThinningTool
print HIGG1D1MetTPThinningTool
#thinningTools.append(HIGG1D1MetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG1D1JetLCTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "HIGG1D1JetLCTPThinningTool",
                                                                            ThinningService         = "HIGG1D1ThinningSvc",
                                                                            JetKey                  = "AntiKt4LCTopoJets",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            ApplyAnd                = True)
ToolSvc += HIGG1D1JetLCTPThinningTool
print HIGG1D1JetLCTPThinningTool
#thinningTools.append(HIGG1D1JetLCTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG1D1JetEMTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "HIGG1D1JetEMTPThinningTool",
                                                                            ThinningService         = "HIGG1D1ThinningSvc",
                                                                            JetKey                  = "AntiKt4EMTopoJets",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            ApplyAnd                = True)
ToolSvc += HIGG1D1JetEMTPThinningTool
print HIGG1D1JetEMTPThinningTool
#thinningTools.append(HIGG1D1JetEMTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG1D1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                    = "HIGG1D1MuonTPThinningTool",
                                                                            ThinningService         = "HIGG1D1ThinningSvc",
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D1MuonTPThinningTool
print HIGG1D1MuonTPThinningTool
#thinningTools.append(HIGG1D1MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG1D1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "HIGG1D1ElectronTPThinningTool",
                                                                                  ThinningService         = "HIGG1D1ThinningSvc",
                                                                                  SGKey                   = "Electrons",
                                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D1ElectronTPThinningTool
print HIGG1D1ElectronTPThinningTool
#thinningTools.append(HIGG1D1ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG1D1PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "HIGG1D1PhotonTPThinningTool",
                                                                                  ThinningService         = "HIGG1D1ThinningSvc",
                                                                                  SGKey                   = "Photons",
                                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D1PhotonTPThinningTool
print HIGG1D1PhotonTPThinningTool
#thinningTools.append(HIGG1D1PhotonTPThinningTool)

# Tracks themselves
HIGG1D1TPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "HIGG1D1TPThinningTool",
                                                                    ThinningService         = "HIGG1D1ThinningSvc",
                                                                    SelectionString             = "abs( DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta)) < 3.0",
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D1TPThinningTool
print HIGG1D1TPThinningTool
#thinningTools.append(HIGG1D1TPThinningTool)


# Truth thinning
truth_cond_1    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_2 = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_3  = "((abs(TruthParticles.pdgId) ==  6))"                                     # Top quark
truth_cond_4 = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
truth_cond_5 = "(abs(TruthParticles.pdgId) >=  1000000)" # BSM
truth_cond_finalState = '(TruthParticles.status == 1 && TruthParticles.barcode < 200000)' # stable particles
truth_expression = '('+truth_cond_1+' || '+truth_cond_2 +' || '+truth_cond_3 +' || '+truth_cond_4+ ' || '+truth_cond_5+') || ('+truth_cond_finalState+')'

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG1D1TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "HIGG1D1TruthThinningTool", 
                                                                      ThinningService         = "HIGG1D1ThinningSvc",
                                                                      ParticleSelectionString = truth_expression,
                                                                      PreserveDescendants     = False,
                                                                      PreserveGeneratorDescendants     = True,
                                                                      PreserveAncestors      = True)

from AthenaCommon.GlobalFlags import globalflags
print "HIGG1D1.py globalflags.DataSource()", globalflags.DataSource()

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG1D1TruthThinningTool
    thinningTools.append(HIGG1D1TruthThinningTool)
print "HIGG1D1.py thinningTools", thinningTools

#====================================================================
# Diphoton vertex decoration tool
#====================================================================

from RecExConfig.RecFlags  import rec
from egammaRec.Factories import ToolFactory, AlgFactory
import PhotonVertexSelection.PhotonVertexSelectionConf as PVS 

PhotonPointingTool = ToolFactory(PVS.CP__PhotonPointingTool, isSimulation = rec.doTruth() )
PhotonVertexSelectionTool = ToolFactory(PVS.CP__PhotonVertexSelectionTool)

from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__DiphotonVertexDecorator
DiphotonVertexDecorator = ToolFactory(DerivationFramework__DiphotonVertexDecorator, PhotonVertexSelectionTool = PhotonVertexSelectionTool,PhotonPointingTool = PhotonPointingTool)()

#====================================================================
# Max Cell sum decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
HIGG1D1_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name = "HIGG1D1_MaxCellDecoratorTool",
                                                                      SGKey_electrons = "Electrons",
                                                                      SGKey_photons   = "Photons"
                                                                      )
ToolSvc += HIGG1D1_MaxCellDecoratorTool

#====================================================================
# Gain and cluster energies per layer decoration tool
#====================================================================

from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations, getClusterEnergyPerLayerDecorator, getClusterEnergyPerLayerDecorations
HIGG1D1_GainDecoratorTool = GainDecorator()

cluster_sizes = (3,5), (5,7), (7,7), (7,11)
HIGG1D1_ClusterEnergyPerLayerDecorators = [getClusterEnergyPerLayerDecorator(neta, nphi)() for neta, nphi in cluster_sizes]

# Create private sequence
# The name of the kernel  must be unique to this derivation
HIGG1D1Seq = CfgMgr.AthSequencer("HIGG1D1Sequence")

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG1D1Kernel",
                                                                       SkimmingTools = [SkimmingToolHIGG1D1],
                                                                       AugmentationTools = [HIGG1D1_MaxCellDecoratorTool,HIGG1D1_GainDecoratorTool, DiphotonVertexDecorator ] + HIGG1D1_ClusterEnergyPerLayerDecorators,
                                                                       ThinningTools = thinningTools
                                                                       )

#MET associated to HggPrimaryVertices
from DerivationFrameworkJetEtMiss import METCommon
METCommon.scheduleMETCustomVertex ( "Hgg", "AntiKt4EMTopo",outputlist = "HggMET" )
scheduleMETAssocAlg(HIGG1D1Seq)

DerivationFrameworkJob += HIGG1D1Seq
#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG1D1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG1D1Stream )
HIGG1D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG1D1Stream.AcceptAlgs(["HIGG1D1Kernel"])

## Do not use this variable at the derivation stage
#HIGG1D1Stream.AddItem("std::vector<int>#leadingV")
# I would want to add this container to the derivations...
#HIGG1D1Stream.AddItem("xAOD::VertexContainer#HggPrimaryVertices")

# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HIGG1D1ThinningSvc", outStreams=[evtStream] )

 #====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG1D1SlimmingHelper = SlimmingHelper("HIGG1D1SlimmingHelper")

HIGG1D1Stream.AddItem("xAOD::VertexContainer#HggPrimaryVertices")
HIGG1D1Stream.AddItem("xAOD::VertexAuxContainer#HggPrimaryVerticesAux.")
HIGG1D1Stream.AddItem("xAOD::EventShape_v1#*")
HIGG1D1Stream.AddItem("xAOD::EventShapeAuxInfo_v1#*")
#HIGG1D1SlimmingHelper.AppendToDictionary = {'HggPrimaryVertices': 'xAOD::VertexContainer'}

HIGG1D1SlimmingHelper.SmartCollections = ["Electrons",
                                          "Photons",
                                          "Muons",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "AntiKt4EMTopoJets",
                                          "AntiKt4EMPFlowJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "InDetTrackParticles",
                                          "PrimaryVertices" ]

HIGG1D1SlimmingHelper.AllVariables = ["Electrons","Photons","egammaClusters","GSFConversionVertices","TruthEvents", "TruthParticles", "TruthVertices", "AntiKt4TruthJets","AntiKt4TruthWZJets","PrimaryVertices","MET_Truth", "MET_Track","egammaTruthParticles","CaloCalTopoClusters","HggPrimaryVertices"]

HIGG1D1SlimmingHelper.ExtraVariables = ["Muons.quality.EnergyLoss.energyLossType",
                                        "GSFTrackParticles.parameterY.parameterZ.vx.vy",
                                        "InDetTrackParticles.vx.vy",
                                        "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.Jvt.JVFCorr.JvtRpt.ConstituentScale",
                                        "AntiKt4EMPFlowJets.JetEMScaleMomentum_eta.JetEMScaleMomentum_m.JetEMScaleMomentum_phi.JetEMScaleMomentum_pt.JetLCScaleMomentum_eta.JetLCScaleMomentum_m.JetLCScaleMomentum_phi.JetLCScaleMomentum_pt.JetOriginConstitScaleMomentum_eta.JetOriginConstitScaleMomentum_m.JetOriginConstitScaleMomentum_phi.JetOriginConstitScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_m.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_pt.JVF.Jvt.JVFCorr.JvtRpt.LArQuality.LeadingClusterCenterLambda.LeadingClusterPt.LeadingClusterSecondLambda.LeadingClusterSecondR.N90Constituents.NegativeE.OotFracClusters10.OotFracClusters5.OriginCorrected.OriginVertex_.OriginVertex_m_persIndex.OriginVertex_m_persKey.PartonTruthLabelID.PileupCorrected.SumPtTrkPt500.Timing.Width.AverageLArQF.btaggingLink_.btaggingLink_m_persIndex.btaggingLink_m_persKey.CentroidR.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.ConeTruthLabelID.DetectorEta.ECPSFraction.EMFrac.FracSamplingMax.FracSamplingMaxIndex.GhostAntiKt2TrackJet.GhostAntiKt3TrackJet.GhostAntiKt4TrackJet.GhostBHadronsFinal.GhostBHadronsInitial.GhostBQuarksFinal.GhostCHadronsFinal.GhostCHadronsInitial.GhostCQuarksFinal.GhostHBosons.GhostPartons.GhostTausFinal.GhostTQuarksFinal.GhostTrack.GhostTruth.GhostWBosons.GhostZBosons.HadronConeExclTruthLabelID.HECFrac.HECQuality.HighestJVFVtx_.HighestJVFVtx_m_persIndex.HighestJVFVtx_m_persKey.ConstituentScale",
                                        "JetETMissChargedParticleFlowObjects.pt.eta.phi.m.DFCommonPFlow_PVMatched",
                                        "JetETMissNeutralParticleFlowObjects.pt.eta.phi.m.centerMag",
                                        "CombinedMuonTrackParticles.z0.vz.definingParametersCovMatrix", 
                                        "ExtrapolatedMuonTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "MuonTruthParticles.truthOrigin.truthType",
                                        "TauChargedParticleFlowObjects.pt.eta.phi.m.e.rapidity.bdtPi0Score",
                                        "TauHadronicParticleFlowObjects.pt.eta.phi.m.e.rapidity.bdtPi0Score",
                                        "TauNeutralParticleFlowObjects.pt.eta.phi.m.e.rapidity.bdtPi0Score",
                                        "TauShotParticleFlowObjects.pt.eta.phi.m.e.rapidity.bdtPi0Score",
                                        "Electrons.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z",
                                        "Photons.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z",
                                        "TopoClusterIsoCentralEventShape.DensitySigma.Density.DensityArea",
                                        "TopoClusterIsoForwardEventShape.DensitySigma.Density.DensityArea",
                                        "TruthIsoCentralEventShape.DensitySigma.Density.DensityArea",
                                        "TruthIsoForwardEventShape.DensitySigma.Density.DensityArea",
                                        "BTagging_AntiKt4EMTopo.MV2cl100_discriminant"]

HIGG1D1SlimmingHelper.ExtraVariables.extend( getGainDecorations(HIGG1D1_GainDecoratorTool) )
for tool in HIGG1D1_ClusterEnergyPerLayerDecorators:
  HIGG1D1SlimmingHelper.ExtraVariables.extend( getClusterEnergyPerLayerDecorations( tool ) )

addMETOutputs(HIGG1D1SlimmingHelper,["HggMET"])
addMETOutputs(HIGG1D1SlimmingHelper,["AntiKt4EMPFlow"])
HIGG1D1SlimmingHelper.IncludeEGammaTriggerContent = True

HIGG1D1SlimmingHelper.AppendContentToStream(HIGG1D1Stream)

