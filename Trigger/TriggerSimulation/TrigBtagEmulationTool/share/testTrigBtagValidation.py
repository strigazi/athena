# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
from AthenaPoolCnvSvc import ReadAthenaPool
from AthenaCommon.AppMgr import ToolSvc
import glob

from AthenaCommon.Constants import VERBOSE,DEBUG,INFO

from TrigBjetHypo.TrigBjetFexTuningGrade1_IP1D import *
from TrigBjetHypo.TrigBjetFexTuningGrade1_IP2D import *
from TrigBjetHypo.TrigBjetFexTuningGrade1_IP3D import *
from TrigBjetHypo.TrigBjetFexTuningGrade2_IP1D import *
from TrigBjetHypo.TrigBjetFexTuningGrade2_IP2D import *
from TrigBjetHypo.TrigBjetFexTuningGrade2_IP3D import *
from TrigBjetHypo.TrigBjetFexTuning_EVTX import *
from TrigBjetHypo.TrigBjetFexTuning_MVTX import *
from TrigBjetHypo.TrigBjetFexTuning_NVTX import *


rec.readAOD=True
acf.EvtMax=10

lista = glob.glob('/eos/atlas/atlascerngroupdisk/trig-bjet/Year-2018/TrigBtagEmulationTool/mc16_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e5602_s3126_r9703_r9728/*')
acf.FilesInput.set_Value_and_Lock(lista) 

rec.doCBNT=False

from RecExConfig.RecFlags import rec
rec.doTrigger=True
from RecExConfig.RecAlgsFlags  import recAlgs
recAlgs.doTrigger=True
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doTriggerConfigOnly=True

rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False
rec.doDPD=False 
rec.doESD=False
rec.doCBNT = False
rec.doHist = False  #default (True)

doTAG=False

rec.doTruth=False

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

# abort when there is an unchecked status code
StatusCodeSvc.AbortOnError=False

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# TDT
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
ToolSvc.TrigDecisionTool.TrigDecisionKey='xTrigDecision'

# OFFLINE BTAGGING
from BTagging.BTaggingFlags import BTaggingFlags
from BTagging.BTaggingConfiguration import getConfiguration
## Comment for untuned offline tools / uncomment for tuned ones
# # BTaggingFlags.TrigCalibrationFolderRoot = '/GLOBAL/TrigBTagCalib/'
# # BTaggingFlags.TrigCalibrationTag = 'TrigBTagCalibRUN12Onl-08-10'
BTagConfig = getConfiguration("Trig")
BTagConfig.PrefixxAODBaseName(False)
BTagConfig.PrefixVertexFinderxAODBaseName(False)
BTagConfig.setupJetBTaggerTool(ToolSvc, "AntiKt4EMTopo", SetupScheme="Trig", TaggerList=BTaggingFlags.TriggerTaggers)


# Define Triggers To be Emulated
toBeEmulatedTriggers = [
    ["L1_MJJ-100",
     "EMUL_L1_MJJ-100"],
    ["HLT_10j40_L14J15",
     "EMUL_L1_4J15",
     "EMUL_HLT_10j40"],
    ["HLT_2j15_gsc35_bmv2c1070_split_2j15_gsc35_bmv2c1085_split_L14J15.0ETA25",
     "EMUL_L1_4J15.0ETA25",
     "EMUL_HLT_2j15_gsc35_bmv2c1070_split",
     "EMUL_HLT_4j15_gsc35_bmv2c1085_split"]
]

# ONLINE EMULATOR
from TrigBtagEmulationTool.TrigBtagEmulationToolConf import Trig__TrigBtagEmulationTool
emulator = Trig__TrigBtagEmulationTool()
trackAssocTool = BTagConfig.getJetCollectionMainAssociatorTool("AntiKt4EMTopo")

emulator.OutputLevel = DEBUG

trackAssocTool.TrackContainerName = "TrigBtagEmulationTool_RetaggingTracks" # Necessary for new track associator in 21.2
trackAssocTool.MuonContainerName  = "" # Necessary for new track associator in 21.2  

emulator.BTagTrackAssocTool = trackAssocTool
emulator.BTagTool           = BTagConfig.getJetCollectionTool("AntiKt4EMTopo")
emulator.BTagSecVertexing   = BTagConfig.getJetCollectionSecVertexingTool("AntiKt4EMTopo")
emulator.TagOfflineWeights = True
emulator.EmulatedChainDefinitions = toBeEmulatedTriggers
ToolSvc += emulator

# TEST ALGORITHM
from TrigBtagEmulationTool.TrigBtagEmulationToolConf import Trig__TrigBtagValidationTest
test = Trig__TrigBtagValidationTest()
test.TrigBtagEmulationTool = emulator
test.ToBeEmulatedTriggers = [[x[0]] for x in toBeEmulatedTriggers]
test.RetrieveRetaggedJets = False #True
test.OutputLevel = 0
theJob += test

## Define your output root file using MultipleStreamManager
## ====================================================================
#from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
#MyFirstHistoXAODStream = MSMgr.NewRootStream( rootStreamName, rootFileName )

ServiceMgr.MessageSvc.defaultLimit = 9999999999