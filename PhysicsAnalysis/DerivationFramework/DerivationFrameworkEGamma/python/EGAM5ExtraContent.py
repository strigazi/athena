# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#Content included in addition to the Smart Slimming Content

ExtraContentMuons=[
    "Muons.DFCommonGoodMuon",
    "Muons.DFCommonMuonsLoose",
    "Muons.DFCommonMuonsMedium",
    "Muons.DFCommonMuonsTight",
    "Muons.ptcone20",
    "Muons.ptcone30",
    "Muons.ptcone40",
    "Muons.etcone20",
    "Muons.etcone30",
    "Muons.etcone40"
]

ExtraMuonsTruth=[
    "MuonTruthParticles.truthOrigin",
    "MuonTruthParticles.truthType"
]

ExtraContentPhotons=[
]

ExtraContentElectrons=[
]

ExtraPhotonsTruth=[
    "Photons.truthOrigin",
    "Photons.truthType",
    "Photons.truthParticleLink"
]

ExtraContentGSFConversionVertices=[
	"GSFConversionVertices.x",
	"GSFConversionVertices.y",
	"GSFConversionVertices.z",
	"GSFConversionVertices.px",
	"GSFConversionVertices.py",
	"GSFConversionVertices.pz",
	"GSFConversionVertices.pt1",
	"GSFConversionVertices.pt2",
	"GSFConversionVertices.etaAtCalo",
	"GSFConversionVertices.phiAtCalo",
	"GSFConversionVertices.trackParticleLinks"
]

cells = ("Cells5x5","Cells3x5","Cells3x7","Cells7x11")
layers_gains =  (	"_Lr0", "_Lr1", "_Lr2", "_Lr3",
					"_Lr0_LwG", "_Lr1_LwG", "_Lr2_LwG", "_Lr3_LwG",
					"_Lr0_LwG", "_Lr1_MdG", "_Lr2_MdG", "_Lr3_MdG",
					"_Lr0_LwG", "_Lr1_HiG", "_Lr2_HiG", "_Lr3_HiG" )

for cell in cells:
	ExtraContentPhotons.append("Photons."+cell)
	for layer in layers_gains:
		ExtraContentPhotons.append("Photons."+cell+layer)

for cell in cells:
	ExtraContentElectrons.append("Electrons."+cell)
	for layer in layers_gains:
		ExtraContentElectrons.append("Electrons."+cell+layer)

ExtraContentAll=ExtraContentElectrons+ExtraContentMuons+ExtraContentPhotons+ExtraContentGSFConversionVertices
ExtraContentAllTruth=ExtraMuonsTruth+ExtraPhotonsTruth

ExtraContainersTruth=["TruthEvents", 
                      "TruthParticles",
                      "TruthVertices",
                      "AntiKt4TruthJets",
		      "egammaTruthParticles"
                      #,"BTagging_AntiKt4TruthWZ"
                      #,"AntiKt4TruthWZJets"
                      ]

ExtraContainersElectrons=["Electrons",
                          "GSFTrackParticles",
                          "egammaClusters"
                          ]

#should probably slim electron/fwdelectrons/cluster collections and keep only relevant subset of variables..
# no ForwardElectrons, InDetTrackParticlesForward, ForwardElectronClusters, CaloCalTopoCluster
