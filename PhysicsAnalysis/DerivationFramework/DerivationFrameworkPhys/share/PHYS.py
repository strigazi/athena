# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#====================================================================
# DAOD_PHYS.py
# This defines DAOD_PHYS, an unskimmed DAOD format for Run 3.
# It contains the variables and objects needed for the large majority 
# of physics analyses in ATLAS.
# It requires the reductionConf flag PHYS in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
from DerivationFrameworkMuons.MuonsCommon import *
from TriggerMenu.api.TriggerAPI import TriggerAPI
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_PHYSStream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_PHYSStream )
PHYSStream = MSMgr.NewPoolRootStream( streamName, fileName )
PHYSStream.AcceptAlgs(["PHYSKernel"])

### Thinning and augmentation tools lists
thinningTools       = []
AugmentationTools   = []

# Special sequence 
SeqPHYS = CfgMgr.AthSequencer("SeqPHYS")

#====================================================================
# MONTE CARLO TRUTH
#====================================================================
if (DerivationFrameworkIsMonteCarlo):
   from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addMiniTruthCollectionLinks,addHFAndDownstreamParticles,addPVCollection
   #import DerivationFrameworkHiggs.TruthCategories
   # Add charm quark collection
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMaker
   PHYSTruthCharmTool = DerivationFramework__TruthCollectionMaker(name                    = "PHYSTruthCharmTool",
                                                                  NewCollectionName       = "TruthCharm",
                                                                  KeepNavigationInfo      = False,
                                                                  ParticleSelectionString = "(abs(TruthParticles.pdgId) == 4)",
                                                                  Do_Compress             = True)
   ToolSvc += PHYSTruthCharmTool
   #from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
   SeqPHYS += CfgMgr.DerivationFramework__CommonAugmentation("PHYSTruthCharmKernel",AugmentationTools=[PHYSTruthCharmTool])
   # Add HF particles
   addHFAndDownstreamParticles(SeqPHYS)
   # Add standard truth
   addStandardTruthContents(SeqPHYS,prefix='PHYS_')
   # Update to include charm quarks and HF particles - require a separate instance to be train safe
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthNavigationDecorator
   PHYSTruthNavigationDecorator = DerivationFramework__TruthNavigationDecorator( name="PHYSTruthNavigationDecorator",
          InputCollections=["TruthElectrons", "TruthMuons", "TruthPhotons", "TruthTaus", "TruthNeutrinos", "TruthBSM", "TruthBottom", "TruthTop", "TruthBoson","TruthCharm","TruthHFWithDecayParticles"])
   ToolSvc += PHYSTruthNavigationDecorator
   SeqPHYS.PHYS_MCTruthNavigationDecoratorKernel.AugmentationTools = [PHYSTruthNavigationDecorator]
   # Re-point links on reco objects
   addMiniTruthCollectionLinks(SeqPHYS)
   addPVCollection(SeqPHYS)
   # Set appropriate truth jet collection for tau truth matching
   ToolSvc.DFCommonTauTruthMatchingTool.TruthJetContainerName = "AntiKt4TruthDressedWZJets"
   # SUSY signal
   from DerivationFrameworkSUSY.DecorateSUSYProcess import IsSUSYSignal
   if IsSUSYSignal():
      from DerivationFrameworkSUSY.SUSYWeightMetadata import *
   # Add sumOfWeights metadata for LHE3 multiweights =======
   #from DerivationFrameworkCore.LHE3WeightMetadata import *



#====================================================================
# TRIGGER CONTENT   
#====================================================================
## Lines commented out temporarily 29th July 2020 to allow validation to proceed. Will need a new MR to fix.
## See https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerAPI
## Get single and multi mu, e, photon triggers
## Jet, tau, multi-object triggers not available in the matching code
#allperiods = TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.y2018 | TriggerPeriod.future2e34
#trig_el  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el,  livefraction=0.8)
#trig_mu  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu,  livefraction=0.8)
#trig_g   = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.g,   livefraction=0.8)
#trig_tau = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.tau, livefraction=0.8)
## Add cross-triggers for some sets
#trig_em = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.mu,  livefraction=0.8)
#trig_et = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.tau, livefraction=0.8)
#trig_mt = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu, additionalTriggerType=TriggerType.tau, livefraction=0.8)
## Note that this seems to pick up both isolated and non-isolated triggers already, so no need for extra grabs
#trig_txe = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.tau, additionalTriggerType=TriggerType.xe, livefraction=0.8)
#
## Merge and remove duplicates
#trigger_names_full_notau = list(set(trig_el+trig_mu+trig_g+trig_em+trig_et+trig_mt))
#trigger_names_full_tau = list(set(trig_tau+trig_txe))
#
## Now reduce the list...
#from RecExConfig.InputFilePeeker import inputFileSummary
#trigger_names_notau = []
#trigger_names_tau = []
#for trig_item in inputFileSummary['metadata']['/TRIGGER/HLT/Menu']:
#    if not 'ChainName' in trig_item: continue
#    if trig_item['ChainName'] in trigger_names_full_notau: trigger_names_notau += [ trig_item['ChainName'] ]
#    if trig_item['ChainName'] in trigger_names_full_tau:   trigger_names_tau   += [ trig_item['ChainName'] ]


#====================================================================
# INNER DETECTOR TRACK THINNING
#====================================================================
# See recommedations here: 
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/DaodRecommendations

# Inner detector group recommendations for indet tracks in analysis
PHYS_thinning_expression = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && InDetTrackParticles.pt > 10*GeV"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
PHYSTrackParticleThinningTool = DerivationFramework__TrackParticleThinning(name                    = "PHYSTrackParticleThinningTool",
                                                                           StreamName              = PHYSStream.Name, 
                                                                           SelectionString         = PHYS_thinning_expression,
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += PHYSTrackParticleThinningTool
thinningTools.append(PHYSTrackParticleThinningTool)

# Include inner detector tracks associated with muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
PHYSMuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "PHYSMuonTPThinningTool",
                                                                        StreamName              = PHYSStream.Name,
                                                                        MuonKey                 = "Muons",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += PHYSMuonTPThinningTool
thinningTools.append(PHYSMuonTPThinningTool)

# TauJets thinning
tau_thinning_expression = "(TauJets.ptFinalCalib >= 13.*GeV) && (TauJets.nTracks>=1) && (TauJets.nTracks<=3) && (TauJets.RNNJetScoreSigTrans>0.01)"
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
PHYSTauJetsThinningTool = DerivationFramework__GenericObjectThinning(name            = "PHYSTauJetsThinningTool",
                                                                     StreamName      = PHYSStream.Name,
                                                                     ContainerName   = "TauJets",
                                                                     SelectionString = tau_thinning_expression)
ToolSvc += PHYSTauJetsThinningTool
thinningTools.append(PHYSTauJetsThinningTool)

# Only keep tau tracks (and associated ID tracks) classified as charged tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
PHYSTauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "PHYSTauTPThinningTool",
                                                                      StreamName             = PHYSStream.Name,
                                                                      TauKey                 = "TauJets",
                                                                      InDetTrackParticlesKey = "InDetTrackParticles",
                                                                      SelectionString        = tau_thinning_expression,
                                                                      DoTauTracksThinning    = True,
                                                                      TauTracksKey           = "TauTracks")
ToolSvc += PHYSTauTPThinningTool
thinningTools.append(PHYSTauTPThinningTool)

# ID tracks associated with high-pt di-tau
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__DiTauTrackParticleThinning
PHYSDiTauTPThinningTool = DerivationFramework__DiTauTrackParticleThinning(name                    = "PHYSDiTauTPThinningTool",
                                                                          StreamName              = PHYSStream.Name,
                                                                          DiTauKey                = "DiTauJets",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += PHYSDiTauTPThinningTool
thinningTools.append(PHYSDiTauTPThinningTool)

#====================================================================
# JET/MET   
#====================================================================

OutputJets["PHYS"] = ["AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]
reducedJetList = ["AntiKt2PV0TrackJets","AntiKt4PV0TrackJets"]

if (DerivationFrameworkIsMonteCarlo):
   OutputJets["PHYS"].append("AntiKt10TruthTrimmedPtFrac5SmallR20Jets")

replaceAODReducedJets(reducedJetList,SeqPHYS,"PHYS")
add_largeR_truth_jets = DerivationFrameworkIsMonteCarlo and not hasattr(SeqPHYS,'jetalgAntiKt10TruthTrimmedPtFrac5SmallR20')
addDefaultTrimmedJets(SeqPHYS,"PHYS",dotruth=add_largeR_truth_jets)

# Add large-R jet truth labeling
if (DerivationFrameworkIsMonteCarlo):
   addJetTruthLabel(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=SeqPHYS,algname="JetTruthLabelingAlg",labelname="R10TruthLabel_R21Consolidated")

addQGTaggerTool(jetalg="AntiKt4EMTopo",sequence=SeqPHYS,algname="QGTaggerToolAlg")
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=SeqPHYS,algname="QGTaggerToolPFAlg")

# fJVT
# getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=SeqPHYS, algname='PHYSJetForwardPFlowJvtToolAlg')

#====================================================================
# EGAMMA
#====================================================================

if DerivationFrameworkIsMonteCarlo:
   # Schedule the two energy density tools for running after the pseudojets are created.
   for alg in ['EDTruthCentralAlg', 'EDTruthForwardAlg']:
      if hasattr(topSequence, alg):
         edtalg = getattr(topSequence, alg)
         delattr(topSequence, alg)
         SeqPHYS += edtalg

#====================================================================
# Add our sequence to the top sequence
#====================================================================
# Ideally, this should come at the end of the job
DerivationFrameworkJob += SeqPHYS

#====================================================================
# Tau   
#====================================================================
# Add low-pt di-tau reconstruction
from DerivationFrameworkTau.TauCommon import addDiTauLowPt
addDiTauLowPt(Seq=SeqPHYS)

# Low-pt di-tau thinning
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
PHYSDiTauLowPtThinningTool = DerivationFramework__GenericObjectThinning(name            = "PHYSDiTauLowPtThinningTool",
                                                                        StreamName      = PHYSStream.Name,
                                                                        ContainerName   = "DiTauJetsLowPt",
                                                                        SelectionString = "DiTauJetsLowPt.nSubjets > 1")
ToolSvc += PHYSDiTauLowPtThinningTool
thinningTools.append(PHYSDiTauLowPtThinningTool)

# ID tracks associated with low-pt ditau
PHYSDiTauLowPtTPThinningTool = DerivationFramework__DiTauTrackParticleThinning(name                    = "PHYSDiTauLowPtTPThinningTool",
                                                                               StreamName              = PHYSStream.Name,
                                                                               DiTauKey                = "DiTauJetsLowPt",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                               SelectionString         = "DiTauJetsLowPt.nSubjets > 1")
ToolSvc += PHYSDiTauLowPtTPThinningTool
thinningTools.append(PHYSDiTauLowPtTPThinningTool)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================
# Add the kernel for thinning (requires the objects be defined)
#from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
SeqPHYS += CfgMgr.DerivationFramework__DerivationKernel("PHYSKernel",
                                                        ThinningTools = thinningTools)


#====================================================================
# FLAVOUR TAGGING   
#====================================================================

from DerivationFrameworkFlavourTag.FtagRun3DerivationConfig import FtagJetCollection

FtagJetCollection('AntiKt4EMPFlowJets',SeqPHYS)


#====================================================================
# TC-LVT Vertices 
#====================================================================

# from SoftBVrtClusterTool.SoftBVrtConfig import addSoftBVrt
# addSoftBVrt(SeqPHYS,'Loose')
# addSoftBVrt(SeqPHYS,'Medium')
# addSoftBVrt(SeqPHYS,'Tight')

#====================================================================
# CONTENTS   
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
PHYSSlimmingHelper = SlimmingHelper("PHYSSlimmingHelper")

PHYSSlimmingHelper.SmartCollections = ["Electrons",
                                       "Photons",
                                       "Muons",
                                       "PrimaryVertices",
                                       "InDetTrackParticles",
                                       "AntiKt4EMTopoJets",
                                       "AntiKt4EMPFlowJets",
                                       "BTagging_AntiKt4EMPFlow",
                                       #"MET_Baseline_AntiKt4EMTopo",
                                       #"MET_Baseline_AntiKt4EMPFlow",
                                       "TauJets",
                                       "DiTauJets",
                                       "DiTauJetsLowPt",
                                       "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                       #"AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903",
                                       #"BTagging_AntiKtVR30Rmax4Rmin02Track_201903"
                                      ]

excludedVertexAuxData = "-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"
StaticContent = []
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Tight_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Tight_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Medium_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Medium_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Loose_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Loose_VerticesAux." + excludedVertexAuxData]

PHYSSlimmingHelper.StaticContent = StaticContent

# Trigger content
PHYSSlimmingHelper.IncludeTriggerNavigation = False
PHYSSlimmingHelper.IncludeJetTriggerContent = False
PHYSSlimmingHelper.IncludeMuonTriggerContent = False
PHYSSlimmingHelper.IncludeEGammaTriggerContent = False
PHYSSlimmingHelper.IncludeJetTauEtMissTriggerContent = False
PHYSSlimmingHelper.IncludeTauTriggerContent = False
PHYSSlimmingHelper.IncludeEtMissTriggerContent = False
PHYSSlimmingHelper.IncludeBJetTriggerContent = False
PHYSSlimmingHelper.IncludeBPhysTriggerContent = False
PHYSSlimmingHelper.IncludeMinBiasTriggerContent = False

# Add the jet containers to the stream (defined in JetCommon if import needed)
#addJetOutputs(PHYSSlimmingHelper,["PHYS"])

# Truth containers
if DerivationFrameworkIsMonteCarlo:
   PHYSSlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
                                            'MET_Truth':'xAOD::MissingETContainer','MET_TruthAux':'xAOD::MissingETAuxContainer',
                                            'TruthElectrons':'xAOD::TruthParticleContainer','TruthElectronsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthMuons':'xAOD::TruthParticleContainer','TruthMuonsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthPhotons':'xAOD::TruthParticleContainer','TruthPhotonsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthTaus':'xAOD::TruthParticleContainer','TruthTausAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthNeutrinos':'xAOD::TruthParticleContainer','TruthNeutrinosAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthForwardProtons':'xAOD::TruthParticleContainer','TruthForwardProtonsAux':'xAOD::TruthParticleAuxContainer',
                                            'BornLeptons':'xAOD::TruthParticleContainer','BornLeptonsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBosonsWithDecayParticles':'xAOD::TruthParticleContainer','TruthBosonsWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBosonsWithDecayVertices':'xAOD::TruthVertexContainer','TruthBosonsWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'TruthBSMWithDecayParticles':'xAOD::TruthParticleContainer','TruthBSMWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSMWithDecayVertices':'xAOD::TruthVertexContainer','TruthBSMWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'HardScatterParticles':'xAOD::TruthParticleContainer','HardScatterParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'HardScatterVertices':'xAOD::TruthVertexContainer','HardScatterVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'TruthHFWithDecayParticles':'xAOD::TruthParticleContainer','TruthHFWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthHFWithDecayVertices':'xAOD::TruthVertexContainer','TruthHFWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'TruthCharm':'xAOD::TruthParticleContainer','TruthCharmAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthPrimaryVertices':'xAOD::TruthVertexContainer','TruthPrimaryVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'AntiKt10TruthTrimmedPtFrac5SmallR20Jets':'xAOD::JetContainer', 'AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux':'xAOD::JetAuxContainer'
                                           }

   from DerivationFrameworkMCTruth.MCTruthCommon import addTruth3ContentToSlimmerTool
   addTruth3ContentToSlimmerTool(PHYSSlimmingHelper)
   PHYSSlimmingHelper.AllVariables += ['TruthHFWithDecayParticles','TruthHFWithDecayVertices','TruthCharm']

PHYSSlimmingHelper.ExtraVariables += ["AntiKt10TruthTrimmedPtFrac5SmallR20Jets.Tau1_wta.Tau2_wta.Tau3_wta.D2.GhostBHadronsFinalCount",
                                      "Electrons.TruthLink",
                                      "Muons.TruthLink",
                                      "Photons.TruthLink",
                                      "AntiKt2PV0TrackJets.pt.eta.phi.m",
                                      "AntiKt4EMTopoJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID",
                                      "AntiKt4EMPFlowJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID.DFCommonJets_fJvt",
                                      "TruthPrimaryVertices.t.x.y.z"]


# Final construction of output stream
PHYSSlimmingHelper.AppendContentToStream(PHYSStream)

