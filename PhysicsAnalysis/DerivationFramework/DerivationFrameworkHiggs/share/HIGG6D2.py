#********************************************************************
# HIGG6D2.py (for H+ -> tau-lep)
# reductionConf flag HIGG6D2 in Reco_tf.py   
#********************************************************************

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#Include common variables from CP groups
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *

from DerivationFrameworkHiggs.TruthCategories import *

from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
applyJetCalibration_xAODColl(jetalg="AntiKt4EMTopo")

import AthenaCommon.SystemOfUnits as Units
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties

# testing globalflags
is_MC = (globalflags.DataSource()=='geant4')
print "is_MC = ",is_MC
print "HIGG6D2.py globalflags.DataSource()", globalflags.DataSource()
print "HIGG6D2.py jobproperties.Beam.energy()", jobproperties.Beam.energy()

if is_MC:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
  from DerivationFrameworkCore.LHE3WeightMetadata import *


 
#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG6D2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG6D2Stream )
HIGG6D2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG6D2Stream.AcceptAlgs(["HIGG6D2Kernel"])

  
#======================================================================
# AUGMENTATION
#=======================================================================


# TAU SELECTOR TOOL 
#=======================================================================
augmentationTools = []
from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauSelectionWrapper
HIGG6D2TauWrapper = DerivationFramework__TauSelectionWrapper(
  name="HIGG6D2TauSelectionWrapper",
  #IsTauFlag=IsTauFlag.JetBDTSigLoose,
  IsTauFlag=19,
  CollectionName="TauJets",
  StoreGateEntryName="HIGG6D2JetBDTSigLoose")

ToolSvc += HIGG6D2TauWrapper
augmentationTools.append(HIGG6D2TauWrapper)

#======================================================================
# SKIMMING 
#=======================================================================


# Tau truth matching tool
#=======================================================================

#truth matching
#if is_MC:
#    from TauAnalysisTools.TauAnalysisToolsConf import TauAnalysisTools__TauTruthMatchingTool
#    HIGG6D2TauTruthMatchingTool = TauAnalysisTools__TauTruthMatchingTool(name="HIGG6D2TauTruthMatchingTool",
#                                                                         WriteTruthTaus = True)
#
#
#    ToolSvc += HIGG6D2TauTruthMatchingTool
#
#    from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauTruthMatchingWrapper
#    HIGG6D2TauTruthMatchingWrapper = DerivationFramework__TauTruthMatchingWrapper( name = "HIGG6D2TauTruthMatchingWrapper",
#                                                                                TauTruthMatchingTool = HIGG6D2TauTruthMatchingTool,
#                                                                                TauContainerName     = "TauJets")
#
#    ToolSvc += HIGG6D2TauTruthMatchingWrapper
#    augmentationTools.append(HIGG6D2TauTruthMatchingWrapper)


# MC selection 
#====================================================================
MCselection = '1'
#if is_MC: MCselection = '(EventInfo.eventTypeBitmask==1)'

# tau selection - tau pt>25GeV, with Ntracks=1,3 and |q_\tau|=1 and |eta|<2.6
#====================================================================
tauSel = "(TauJets.pt > 25*GeV"\
         "&& (abs(TauJets.eta)<2.6)"\
         "&& (abs(TauJets.charge)==1 || abs(TauJets.charge)==3)"\
         "&& ((TauJets.nTracks == 1) || (TauJets.nTracks == 3) ) )"
         
# Trigger selection 
#====================================================================
if jobproperties.Beam.energy()==6500000.0:
  # 13 TeV name should be HLT_xxx
  singleElectronTriggerRequirement = "(HLT_e24_lhmedium_L1EM20VH"\
                                     "|| HLT_e60_lhmedium"\
                                     "|| HLT_e120_lhloose"\
                                     "|| HLT_e24_lhtight_nod0_ivarloose"\
                                     "|| HLT_e60_lhmedium_nod0"\
                                     "|| HLT_e60_medium"\
                                     "|| HLT_e140_lhloose_nod0"\
                                     "|| HLT_e300_etcut"\
                                     "|| HLT_e26_lhtight_nod0_ivarloose)"
  
  singleMuonTriggerRequirement = "(HLT_mu20_iloose_L1MU15"\
                                 "|| HLT_mu40"\
                                 "|| HLT_mu60_0eta105_msonly"\
                                 "|| HLT_mu24_iloose"\
                                 "|| HLT_mu24_ivarloose"\
                                 "|| HLT_mu40"\
                                 "|| HLT_mu50"\
                                 "|| HLT_mu24_ivarmedium"\
                                 "|| HLT_mu24_imedium"\
                                 "|| HLT_mu26_ivarmedium)"

  singleLepTrigger='('+singleElectronTriggerRequirement+'||'+singleMuonTriggerRequirement+')'
    
  TauMETtrigSel = "(HLT_tau35_medium1_tracktwo_L1TAU20_xe70_L1XE45"\
                  "|| HLT_tau35_medium1_tracktwo_xe70_L1XE45"\
                  "|| HLT_xe70"\
                  "|| HLT_xe80"\
                  "|| HLT_xe100"\
                  "|| HLT_j30_xe10_razor170"\
                  "|| HLT_j30_xe10_razor185"\
                  "|| HLT_j30_xe60_razor170 )"
  
  BjetTrigger = 'HLT_j75_bmedium_3j75'
 
  
# lepton selection
#========================================================================
electronRequirements = "( (Electrons.pt > 25*GeV)"\
                       "&& (abs(Electrons.eta) < 2.6)"\
                       "&& (Electrons.DFCommonElectronsLHLoose))"

muonRequirements = "( (Muons.pt > 25*GeV)"\
                   "&& (abs(Muons.eta) < 2.6)"\
                   "&& (Muons.DFCommonMuonsPreselection) )"

lepSel = "( ( (count({0}) >=1)  || (count({1}) >=1)))".format(electronRequirements, muonRequirements)
lepSel2 = "(( count({0}) + count({1}) ) >=2 )".format(electronRequirements, muonRequirements)


# jet selection: jets with pt>20 and |eta|<2.6, nJet>=1 + nEl + nTau
#====================================================================

jetSelEM = "count(({0}.pt > 20.0*GeV) && (abs({0}.eta) < 2.6))"\
           " >= (1 + count({1}) + count({2}))".format(
             "AntiKt4EMTopoJets", electronRequirements, tauSel)

jetSelEMCalib = "count(({0}.DFCommonJets_Calib_pt > 20.0*GeV)&& (abs({0}.DFCommonJets_Calib_eta) < 2.6))"\
                " >= (1 + count({1}) + count({2}) )".format(
                  "AntiKt4EMTopoJets", electronRequirements, tauSel)

jetSel = "({0}) || ({1})".format(jetSelEM, jetSelEMCalib)


# - - - - - - - -  SKIMMING TOOL 
expression = "({0}) &&  ( ( ({1}) && (count({2}) >= 1) ) || ({3}) ) && ({4}) && ({5})".format(
  jetSel, lepSel, tauSel, lepSel2, MCselection, singleLepTrigger)

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG6D2SkimmingTool = DerivationFramework__xAODStringSkimmingTool(
  name="HIGG6D2SkimmingTool1",
  expression=expression)

ToolSvc += HIGG6D2SkimmingTool
print "HIGG6D2 SKIMMING TOOL: ", HIGG6D2SkimmingTool
print "HIGG6D2 SKIMMING TOOL EXPRESSION:", expression


#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools = []
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HIGG6D2ThinningHelper = ThinningHelper( "HIGG6D2ThinningHelper" )

# - - - - trigger Thinning Tool
HIGG6D2ThinningHelper.TriggerChains = 'HLT_e.*|HLT_mu.*|HLT_tau.*|HLT_xe.*'
HIGG6D2ThinningHelper.AppendToStream(HIGG6D2Stream)


# GenericTrackParticleThinning
#====================================================================
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV)"\
                      "&& (InDetTrackParticles.numberOfPixelHits > 0)"\
                      "&& (InDetTrackParticles.numberOfSCTHits > 5)"\
                      "&& (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG6D2GenTPThinningTool = DerivationFramework__TrackParticleThinning(
  name="HIGG6D2GenTPThinningTool",
  ThinningService=HIGG6D2ThinningHelper.ThinningSvc(),
  SelectionString=thinning_expression,
  InDetTrackParticlesKey ="InDetTrackParticles",
  ApplyAnd=True
)

thinningTools.append(HIGG6D2GenTPThinningTool)
ToolSvc += HIGG6D2GenTPThinningTool

# Tracks themselves
#====================================================================
HIGG6D2TPThinningTool = DerivationFramework__TrackParticleThinning(
  name="HIGG6D2TPThinningTool",
  ThinningService=HIGG6D2ThinningHelper.ThinningSvc(),
  SelectionString="abs( DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta)) < 3.0",
  InDetTrackParticlesKey ="InDetTrackParticles"
)

thinningTools.append(HIGG6D2TPThinningTool)
ToolSvc += HIGG6D2TPThinningTool


# EMTopoJetTrackParticleThinning
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG6D2EMTopoJetTPThinningTool = DerivationFramework__JetTrackParticleThinning(
  name="HIGG6D2EMTopoJetTPThinningTool",
  ThinningService=HIGG6D2ThinningHelper.ThinningSvc(),
  JetKey="AntiKt4EMTopoJets",
  InDetTrackParticlesKey="InDetTrackParticles",
  #SelectionString="AntiKt4EMTopoJets.pt > 10*GeV", #<! doesn't make a big difference
  ApplyAnd=True)

thinningTools.append(HIGG6D2EMTopoJetTPThinningTool)
ToolSvc += HIGG6D2EMTopoJetTPThinningTool


# LCTopoJetTrackParticleThinning
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG6D2LCTopoJetTPThinningTool = DerivationFramework__JetTrackParticleThinning(
  name="HIGG6D2LCTopoJetTPThinningTool",
  ThinningService=HIGG6D2ThinningHelper.ThinningSvc(),
  JetKey="AntiKt4LCTopoJets",
  InDetTrackParticlesKey="InDetTrackParticles",
  #SelectionString="AntiKt4LCTopoJets.pt > 10*GeV", #<! doesn't make a big difference
  ApplyAnd=True)

thinningTools.append(HIGG6D2LCTopoJetTPThinningTool)
ToolSvc += HIGG6D2LCTopoJetTPThinningTool


# TauTrackParticleThinning
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
tauThinningRequirements = "(TauJets.pt > 10*GeV"\
                          "&& (abs(TauJets.eta)<2.6)"\
                          "&& (abs(TauJets.charge)==1 || abs(TauJets.charge)==3)"\
                          "&& ((TauJets.nTracks == 1) || (TauJets.nTracks == 3)) )"
HIGG6D2TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(
  name= "HIGG6D2TauTPThinningTool",
  ThinningService=HIGG6D2ThinningHelper.ThinningSvc(),
  TauKey="TauJets",
  InDetTrackParticlesKey="InDetTrackParticles",
  SelectionString="(TauJets.pt > 20*GeV)",
  ConeSize = 0.6,
  ApplyAnd = False)

thinningTools.append(HIGG6D2TauTPThinningTool)
ToolSvc += HIGG6D2TauTPThinningTool

# Clusters for Tau TES
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
HIGG6D2CaloClusterThinning = DerivationFramework__CaloClusterThinning(
  name="HIGG6D2PClusterThinning",
  ThinningService=HIGG6D2ThinningHelper.ThinningSvc(),
  SGKey="TauJets",
  TopoClCollectionSGKey="CaloCalTopoClusters",
  SelectionString= "(TauJets.pt > 20*GeV)",
  ApplyAnd=False,
)
ToolSvc += HIGG6D2CaloClusterThinning
thinningTools.append(HIGG6D2CaloClusterThinning)

# MuonTrackParticleThinning
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
muonThinningRequirements = "( (Muons.pt > 5*GeV)"\
                           "&& (abs(Muons.eta) < 2.6)"\
                           "&& (Muons.DFCommonGoodMuon) )"
HIGG6D2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(
  name="HIGG6D2MuonTPThinningTool",
  ThinningService=HIGG6D2ThinningHelper.ThinningSvc(),
  MuonKey= "Muons",
  InDetTrackParticlesKey="InDetTrackParticles",
  SelectionString="(Muons.pt > 5*GeV)",
  ConeSize=0.4,
  ApplyAnd=False)

thinningTools.append(HIGG6D2MuonTPThinningTool)
ToolSvc += HIGG6D2MuonTPThinningTool

# EgammaTrackParticleThinning
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
electronThinningRequirements = "( (Electrons.pt > 10*GeV)"\
                               "&& (abs(Electrons.eta) < 2.6)"\
                               "&& (Electrons.DFCommonElectronsLHLoose))"
HIGG6D2ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(
  name= "HIGG6D2ElectronTPThinningTool",
  ThinningService=HIGG6D2ThinningHelper.ThinningSvc(),
  SGKey= "Electrons",
  InDetTrackParticlesKey="InDetTrackParticles",
  SelectionString="(Electrons.pt > 10*GeV)",
  ConeSize=0.4,
  ApplyAnd=False)

thinningTools.append(HIGG6D2ElectronTPThinningTool)
ToolSvc += HIGG6D2ElectronTPThinningTool


# Truth particles thinning
#====================================================================

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
HIGG6D2TruthThinningTool = DerivationFramework__MenuTruthThinning(
  name                       = "HIGG6D2TruthThinningTool",
  ThinningService            = "HIGG6D2ThinningSvc",
  WritePartons               = False,
  WriteHadrons               = False,
  WriteBHadrons              = True,
  WriteGeant                 = False,
  GeantPhotonPtThresh        = -1.0,
  WriteTauHad                = True,
  PartonPtThresh             = -1.0,
  WriteBSM                   = True,
  WriteBosons                = True,
  WriteBSMProducts           = True,
  WriteTopAndDecays          = True,
  WriteEverything            = False,
  WriteAllLeptons            = False,
  WriteLeptonsNotFromHadrons = True,
  WriteStatus3               = True,
  WriteFirstN                = -1,
  PreserveGeneratorDescendants = True,
  PreserveDescendants = False)

if is_MC:
    ToolSvc += HIGG6D2TruthThinningTool
    thinningTools.append(HIGG6D2TruthThinningTool)


#===========================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#===========================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "HIGG6D2Kernel",
  AugmentationTools = augmentationTools,
  SkimmingTools = [HIGG6D2SkimmingTool],
  ThinningTools = thinningTools
)

#====================================================================
# Add the containers to the output stream - slimming done here (smart slimming)
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG6D2SlimmingHelper = SlimmingHelper("HIGG6D2SlimmingHelper")

## Smart Slimming
HIGG6D2SlimmingHelper.SmartCollections = [
  "Electrons",
  "Muons",
  "TauJets",
  "MET_Reference_AntiKt4LCTopo",
  "MET_Reference_AntiKt4EMTopo",
  "AntiKt4LCTopoJets",
  "AntiKt4EMTopoJets",
  "TauMVATESJets",
  "BTagging_AntiKt4LCTopo",
  "BTagging_AntiKt4EMTopo",
  "InDetTrackParticles",
  "PrimaryVertices",
]

## Add extra variables
HIGG6D2SlimmingHelper.ExtraVariables += ["AntiKt4EMTopoJets.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta"]
HIGG6D2SlimmingHelper.ExtraVariables += ["BTagging_AntiKt4EMTopo.MV2cl100_discriminant"]
HIGG6D2SlimmingHelper.ExtraVariables += [
  "TauJets.ptDetectorAxis.etaDetectorAxis.phiDetectorAxis."\
  "mDetectorAxis.BDTEleScore.pantau_CellBasedInput_isPanTauCandidate.pantau_CellBasedInput_DecayMode"\
  ".ptPanTauCellBased.etaPanTauCellBased.phiPanTauCellBased.mPanTauCellBased."\
  "pantau_CellBasedInput_BDTValue_1p0n_vs_1p1n.pantau_CellBasedInput_BDTValue_1p1n_vs_1pXn.pantau_CellBasedInput_BDTValue_3p0n_vs_3pXn",
  "TauNeutralParticleFlowObjects.pt.eta.phi.m.rapidity.bdtPi0Score"]

HIGG6D2SlimmingHelper.ExtraVariables += [
  "Electrons.DFCommonElectronsLHLoose.DFCommonElectronsLHMedium."\
  "DFCommonElectronsLHTight.DFCommonElectronsML.author.OQ.charge.LHLoose.LHMedium.LHTight.LHValue"
]
HIGG6D2SlimmingHelper.ExtraVariables += [
  "Muons.DFCommonGoodMuons",
  "CombinedMuonTrackParticles.d0.z0.vz",
  "InDetTrackParticles.numberOfTRTHits.numberOfTRTOutliers"
]
HIGG6D2SlimmingHelper.ExtraVariables += ["PrimaryVertices.x.y.z.vertexType"]

HIGG6D2SlimmingHelper.AllVariables = [
  "CaloCalTopoClusters",
  "TauChargedParticleFlowObjects"
]

if is_MC:
  HIGG6D2SlimmingHelper.StaticContent = [
    "xAOD::TruthParticleContainer#TruthMuons",
    "xAOD::TruthParticleAuxContainer#TruthMuonsAux.",
    "xAOD::TruthParticleContainer#TruthElectrons",
    "xAOD::TruthParticleAuxContainer#TruthElectronsAux.",
    "xAOD::TruthParticleContainer#TruthPhotons",
    "xAOD::TruthParticleAuxContainer#TruthPhotonsAux.",
    "xAOD::TruthParticleContainer#TruthNeutrinos",
    "xAOD::TruthParticleAuxContainer#TruthNeutrinosAux.",
    "xAOD::TruthParticleContainer#TruthTaus",
    "xAOD::TruthParticleAuxContainer#TruthTausAux."
  ]  
  HIGG6D2SlimmingHelper.AllVariables = [
    "TruthParticles",
    "TruthEvents",
    "MET_Truth",
    "METMap_Truth",
    "MET_Track",
    "TruthVertices",
    "CaloCalTopoClusters",
    "TauChargedParticleFlowObjects",
    #"AntiKt4TruthJets"
  ]
  HIGG6D2SlimmingHelper.ExtraVariables += ["AntiKt4LCTopoJets.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T"]
  HIGG6D2SlimmingHelper.ExtraVariables += ["AntiKt4EMTopoJets.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T"]
  HIGG6D2SlimmingHelper.ExtraVariables += ["Electrons.truthOrigin.truthType.truthParticleLink","MuonTruthParticles.truthOrigin.truthType"]
  HIGG6D2SlimmingHelper.ExtraVariables += ["TauJets.IsTruthMatched.truthParticleLink.truthJetLink"]

# Add MET_RefFinalFix
## FIX ME: this one adds AllVariables to the above collections which is not allowed !! 
# addMETOutputs(HIGG6D2SlimmingHelper,["AntiKt4LCTopo","AntiKt4EMTopo","Track"])

HIGG6D2SlimmingHelper.IncludeMuonTriggerContent = True
HIGG6D2SlimmingHelper.IncludeEGammaTriggerContent = True
HIGG6D2SlimmingHelper.IncludeTauTriggerContent = True
HIGG6D2SlimmingHelper.IncludeEtMissTriggerContent = True

# HIGG6D2SlimmingHelper.IncludeJetTriggerContent = True


HIGG6D2SlimmingHelper.AppendContentToStream(HIGG6D2Stream)
