# steering file for ESD->ESD step
# see myTopOptions.py for more info
from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock(False)
rec.readESD.set_Value_and_Lock(True)
rec.doWriteESD.set_Value_and_Lock(True)
rec.doAOD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
# doCaloTopoCluster=True
#jp.AthenaCommonFlags.PoolESDInput=["ESD.pool.root"]
if not 'jp.AthenaCommonFlags.PoolESDOutput' in dir():
    jp.AthenaCommonFlags.PoolESDOutput="copy_ESD.pool.root"


include ("RecExCond/RecExCommon_flags.py")
rec.AutoConfiguration = ['everything']
import RecExConfig.AutoConfiguration as auto
auto.ConfigureFromListOfKeys(rec.AutoConfiguration())


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

from RecExConfig.hideInput import hideInput

hideInput ('CaloCellContainer', 'TauCommonPi0Cells')
hideInput ('CaloClusterCellLinkContainer', 'CaloCalTopoClusters_links')
hideInput ('CaloClusterCellLinkContainer', 'ForwardElectronClusters_links')
hideInput ('CaloClusterCellLinkContainer', 'InDetTrackParticlesAssociatedClusters_links')
hideInput ('CaloClusterCellLinkContainer', 'LArClusterEM7_11Nocorr_links')
hideInput ('CaloClusterCellLinkContainer', 'MuonClusterCollection_links')
hideInput ('CaloClusterCellLinkContainer', 'TauPi0Clusters_links')
hideInput ('CaloClusterCellLinkContainer', 'egammaClusters_links')
hideInput ('DetailedTrackTruthCollection', 'CombinedMuonTracksTruth')
hideInput ('DetailedTrackTruthCollection', 'ExtrapolatedMuonTracksTruth')
hideInput ('DetailedTrackTruthCollection', 'MuonSpectrometerTracksTruth')
hideInput ('MuonCaloEnergyContainer', 'MuonCaloEnergyCollection')
#hideInput ('SegmentCollection', 'MuonSegments')
hideInput ('SegmentCollection', 'MuonTruthSegments')
hideInput ('TrackCollection', 'CombinedInDetTracks')
hideInput ('TrackCollection', 'CombinedMuonTracks')
hideInput ('TrackCollection', 'ExtrapolatedMuonTracks')
hideInput ('TrackCollection', 'GSFTracks')
hideInput ('TrackCollection', 'MuonSpectrometerTracks')
hideInput ('TrackCollection', 'MuonTruthTracks')
hideInput ('TrackCollection', 'DisappearingTracks')
hideInput ('TrackTruthCollection', 'DisappearingTracksTruthCollection')
hideInput ('TrackTruthCollection', 'ResolvedForwardTracksTruthCollection')
hideInput ('TrackTruthCollection', 'TrackTruthCollection')
hideInput ('xAOD::BTagVertexContainer',  'BTagging_AntiKt4EMTopoJFVtx')
hideInput ('xAOD::CaloClusterContainer', 'ForwardElectronClusters')
hideInput ('xAOD::CaloClusterContainer', 'InDetTrackParticlesAssociatedClusters')
hideInput ('xAOD::CaloClusterContainer', 'MuonClusterCollection')
hideInput ('xAOD::CaloClusterContainer', 'TauPi0Clusters')
hideInput ('xAOD::CaloClusterContainer', 'egammaClusters')
hideInput ('xAOD::CaloClusterContainer', 'LArClusterEM7_11Nocorr')
hideInput ('xAOD::CaloClusterContainer', 'CaloCalTopoClusters')
hideInput ('xAOD::CaloRingsContainer', 'ElectronCaloRings')
hideInput ('xAOD::DiTauJetContainer', 'DiTauJets')
hideInput ('xAOD::ElectronContainer', 'Electrons')
hideInput ('xAOD::ElectronContainer', 'ForwardElectrons')
hideInput ('xAOD::EventShape', 'Kt4EMPFlowEventShape')
hideInput ('xAOD::EventShape', 'Kt4EMTopoOriginEventShape')
hideInput ('xAOD::EventShape', 'Kt4LCTopoOriginEventShape')
hideInput ('xAOD::EventShape', 'NeutralParticleFlowIsoCentralEventShape')
hideInput ('xAOD::EventShape', 'NeutralParticleFlowIsoForwardEventShape')
hideInput ('xAOD::EventShape', 'ParticleFlowIsoCentralEventShape')
hideInput ('xAOD::EventShape', 'ParticleFlowIsoForwardEventShape')
hideInput ('xAOD::EventShape', 'TopoClusterIsoCentralEventShape')
hideInput ('xAOD::EventShape', 'TopoClusterIsoForwardEventShape')
hideInput ('xAOD::EventShape', 'TopoClusterIsoVeryForwardEventShape')
hideInput ('xAOD::JetContainer', 'AntiKt10LCTopoJets')
hideInput ('xAOD::JetContainer', 'AntiKt10TruthJets')
hideInput ('xAOD::JetContainer', 'AntiKt10TruthWZJets')
hideInput ('xAOD::JetContainer', 'AntiKt2PV0TrackJets')
hideInput ('xAOD::JetContainer', 'AntiKt4EMPFlowJets')
hideInput ('xAOD::JetContainer', 'AntiKt4EMTopoJets')
hideInput ('xAOD::JetContainer', 'AntiKt4LCTopoJets')
hideInput ('xAOD::JetContainer', 'AntiKt4PV0TrackJets')
hideInput ('xAOD::JetContainer', 'AntiKt4TruthJets')
hideInput ('xAOD::JetContainer', 'AntiKt4TruthWZJets')
hideInput ('xAOD::MissingETContainer', 'MET_Reference_AntiKt4EMPFlow')
hideInput ('xAOD::MissingETContainer', 'MET_Reference_AntiKt4EMTopo')
hideInput ('xAOD::MissingETContainer', 'MET_Reference_AntiKt4LCTopo')
hideInput ('xAOD::BTaggingContainer', 'BTagging_AntiKt4EMTopo')
hideInput ('xAOD::MuonContainer', 'Muons')
hideInput ('xAOD::MuonContainer', 'Staus')
#hideInput ('xAOD::MuonSegmentContainer', 'MuonSegments')
hideInput ('xAOD::MuonSegmentContainer', 'MuonSegments.truthSegmentLink')
hideInput ('xAOD::MuonSegmentContainer', 'NCB_MuonSegments')
hideInput ('xAOD::PFOContainer', 'JetETMissChargedParticleFlowObjects')
hideInput ('xAOD::PFOContainer', 'JetETMissNeutralParticleFlowObjects')
hideInput ('xAOD::PFOContainer', 'TauChargedParticleFlowObjects')
hideInput ('xAOD::PFOContainer', 'TauHadronicParticleFlowObjects')
hideInput ('xAOD::PFOContainer', 'TauNeutralParticleFlowObjects')
hideInput ('xAOD::PFOContainer', 'TauShotParticleFlowObjects')
hideInput ('xAOD::PFOContainer', 'finalTauPi0s')
hideInput ('xAOD::PFOContainer', 'CHSNeutralParticleFlowObjects')
hideInput ('xAOD::PFOContainer', 'CHSChargedParticleFlowObjects')
hideInput ('xAOD::ParticleContainer', 'finalTauPi0s')
hideInput ('xAOD::PhotonContainer', 'Photons')
hideInput ('xAOD::RingSetContainer', 'ElectronRingSets')
hideInput ('xAOD::SlowMuonContainer', 'SlowMuons')
hideInput ('xAOD::TauJetContainer', 'TauJets')
hideInput ('xAOD::TauTrackContainer', 'TauTracks')
hideInput ('xAOD::TrackParticleClusterAssociationContainer', 'InDetTrackParticlesClusterAssociations')
hideInput ('xAOD::TrackParticleContainer', 'CombinedMuonTrackParticles')
hideInput ('xAOD::TrackParticleContainer', 'CombinedStauTrackParticles')
hideInput ('xAOD::TrackParticleContainer', 'ExtrapolatedMuonTrackParticles')
hideInput ('xAOD::TrackParticleContainer', 'ExtrapolatedStauTrackParticles')
hideInput ('xAOD::TrackParticleContainer', 'GSFTrackParticles')
hideInput ('xAOD::TrackParticleContainer', 'InDetForwardTrackParticles')
hideInput ('xAOD::TrackParticleContainer', 'InDetDisappearingTrackParticles')
hideInput ('xAOD::TrackParticleContainer', 'InDetTrackParticles')
hideInput ('xAOD::TrackParticleContainer', 'MSOnlyExtrapolatedMuonTrackParticles')
hideInput ('xAOD::TrackParticleContainer', 'MSonlyTracklets')
hideInput ('xAOD::TrackParticleContainer', 'MuonSpectrometerTrackParticles')
hideInput ('xAOD::TruthParticleContainer', 'egammaTruthParticles')
hideInput ('xAOD::VertexContainer', 'GSFConversionVertices')
hideInput ('xAOD::VertexContainer', 'MSDisplacedVertex')
hideInput ('xAOD::VertexContainer', 'PrimaryVertices')
hideInput ('xAOD::VertexContainer', 'TauSecondaryVertices')
hideInput ('xAOD::VertexContainer', 'BTagging_AntiKt4EMTopoSecVtx')
hideInput ('xAOD::MuonSegmentContainer', 'MuonTruthSegments.recoSegmentLink')
hideInput ('xAOD::TrackParticleContainer', 'MuonSpectrometerTrackParticles')
hideInput ('xAOD::TruthParticleContainer', 'MuonTruthParticles.recoMuonLink')

