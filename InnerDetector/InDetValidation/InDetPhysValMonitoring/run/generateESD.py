# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.FilesInput=["/afs/cern.ch/atlas/offline/test/mc11_valid.105200.T1_McAtNlo_Jimmy.digit.RDO.e835_s1310_s1300_d622.RDO.10evts.pool.root"]

#attempting to get Zmumu file #1
athenaCommonFlags.FilesInput=["root://eosatlas//eos/atlas/atlasgroupdisk/perf-idtracking/dq2/rucio/mc15_13TeV/e0/5c/RDO.06718162._000011.pool.root.1"]

#Zmumu file #2 (500 events)
#athenaCommonFlags.FilesInput = ["root://eosatlas//eos/atlas/atlasgroupdisk/perf-idtracking/dq2/rucio/mc15_13TeV/64/3c/RDO.07497175._000001.pool.root.1"]

#Control the number of events, -1 means "ALL"
athenaCommonFlags.EvtMax = 5

from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.TrackParticleSlimmer=False 
AODFlags.TrackParticleLastHitAndPerigeeSlimmer=False

from RecExConfig.RecFlags import rec
rec.doTrigger=False

#from JetRec.JetRecFlags import jetFlags
#jetFlags.applyCalibration = False 

#Custom kalman vertex updator
#from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__KalmanVertexUpdator
#myVertexUpdator = Trk__KalmanVertexUpdator()
#ToolSvc += myVertexUpdator

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

ToolSvc.TileCellBuilder.maskBadChannels=False

#Enable debug
#InDetVxFitterTool.OutputLevel=VERBOSE
#InDetPriVxFinderTool.OutputLevel=VERBOSE
