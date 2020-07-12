# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory     import CompFactory

def InDetCosmicsEventPhaseToolCfg(flags, name='InDetCosmicsEventPhaseTool', **kwargs) :
	#
	# --- load tool
	#
	acc = ComponentAccumulator()
	if( not flags.Input.isMC ):
		if flags.Beam.Type =='cosmics':
			kwargs.setdefault("GlobalOffset"       , 8)
		else:
			kwargs.setdefault("GlobalOffset"       , 0)
	else:
		kwargs.setdefault("GlobalOffset"       , -3.125)
	# CalDb tool
	InDetTRTCalDbTool = CompFactory.TRT_CalDbTool(name = "TRT_CalDbTool")
	kwargs.setdefault("UseNewEP"       , True)
	kwargs.setdefault("TRTCalDbTool"   , InDetTRTCalDbTool)

	acc.setPrivateTools(CompFactory.InDet__InDetCosmicsEventPhaseTool(name = name, **kwargs))
	return acc


def InDetFixedWindowTrackTimeToolCfg(flags, name='InDetFixedWindowTrackTimeTool', **kwargs) :
	#
	# --- load tool
	#
	acc = ComponentAccumulator()
	if( not flags.Input.isMC ):
		if flags.Beam.Type =='cosmics':
			kwargs.setdefault("GlobalOffset"       , 8)
		else:
			kwargs.setdefault("GlobalOffset"       , 0)
	else:
		kwargs.setdefault("GlobalOffset"       , -3.125)
	# CalDb tool
	InDetTRTCalDbTool = CompFactory.TRT_CalDbTool(name = "TRT_CalDbTool")
	cutWindowCenter  = -8.5
	cutWindowSize    = 7
	kwargs.setdefault("UseNewEP"       , True)
	kwargs.setdefault("WindowCenter"   , cutWindowCenter)
	kwargs.setdefault("WindowSize"   , cutWindowSize)
	kwargs.setdefault("TRTCalDbTool"   , InDetTRTCalDbTool)


	acc.setPrivateTools(CompFactory.InDet__InDetFixedWindowTrackTimeTool(name = name, **kwargs))
	return acc


def InDetSlidingWindowTrackTimeToolCfg(flags, name='InDetSlidingWindowTrackTimeTool', **kwargs) :
	#
	# --- load tool
	#
	acc = ComponentAccumulator()
	if( not flags.Input.isMC ):
		if flags.Beam.Type =='cosmics':
			kwargs.setdefault("GlobalOffset"       , 8)
		else:
			kwargs.setdefault("GlobalOffset"       , 0)
	else:
		kwargs.setdefault("GlobalOffset"       , -3.125)
	# CalDb tool
	InDetTRTCalDbTool = CompFactory.TRT_CalDbTool(name = "TRT_CalDbTool")
	numberIterations = 5
	cutWindowSize    = 7
	kwargs.setdefault("UseNewEP"       , True)
	kwargs.setdefault("NumberIterations"   , numberIterations)
	kwargs.setdefault("WindowSize"   , cutWindowSize)
	kwargs.setdefault("TRTCalDbTool"   , InDetTRTCalDbTool)

	acc.setPrivateTools(CompFactory.InDet__InDetSlidingWindowTrackTimeTool(name = name, **kwargs))
	return acc


def InDetCosmicsEventPhaseCfg(flags, InputTrackCollections, name = 'InDetCosmicsEventPhase', **kwargs):
	#
	# --- load algorithm
	#
	acc = ComponentAccumulator()

	InDetCosmicsEventPhaseTool = acc.popToolsAndMerge(InDetCosmicsEventPhaseToolCfg(flags))
	acc.addPublicTool(InDetCosmicsEventPhaseTool)

	InDetSlidingWindowTrackTimeTool = acc.popToolsAndMerge(InDetSlidingWindowTrackTimeToolCfg(flags))
	acc.addPublicTool(InDetSlidingWindowTrackTimeTool)

	from TrackingCommonConfig import InDetTrackSummaryToolCfg
	InDetTrackSummaryTool = acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags))
	acc.addPublicTool(InDetTrackSummaryTool)
	# CalDb tool
	InDetTRTCalDbTool = CompFactory.TRT_CalDbTool(name = "TRT_CalDbTool")
	kwargs.setdefault("InputTracksNames"       , InputTrackCollections)
	kwargs.setdefault("TrackSummaryTool"       , InDetTrackSummaryTool)
	kwargs.setdefault("TRTCalDbTool"       , InDetTRTCalDbTool)
	kwargs.setdefault("EventPhaseTool"       , InDetSlidingWindowTrackTimeTool)

	if flags.Beam.Type =='cosmics':
		kwargs.setdefault("EventPhaseTool"       , InDetCosmicsEventPhaseTool)

	acc.addEventAlgo(CompFactory.InDet__InDetCosmicsEventPhase(name = name, **kwargs))
	return acc

# --------------------------------------------------------------------------------
#
# --- TRT phase calculation
#
# --------------------------------------------------------------------------------
def TRTPhaseCfg(flags, self, InputTrackCollections = [], **kwargs):
	acc = ComponentAccumulator()
	if flags.InDet.doPRDFormation: ##maybe add later 'and TRT_on' from ConfiguredNewTrackingCuts.py
		#    
		# --- calculation of the event phase from all 3 input collections
		#
		acc.merge(InDetCosmicsEventPhaseCfg(flags, InputTrackCollections, **kwargs))
	return acc

if __name__ == "__main__":
	from AthenaCommon.Configurable import Configurable
	Configurable.configurableRun3Behavior=1

	from AthenaConfiguration.AllConfigFlags import ConfigFlags

	numThreads=1
	ConfigFlags.Concurrency.NumThreads=numThreads
	ConfigFlags.Concurrency.NumConcurrentEvents=numThreads # Might change this later, but good enough for the moment.

	from AthenaConfiguration.TestDefaults import defaultTestFiles
	ConfigFlags.Input.Files = defaultTestFiles.RDO
	ConfigFlags.lock()
	ConfigFlags.dump()

	from AthenaConfiguration.MainServicesConfig import MainServicesCfg
	top_acc = MainServicesCfg(ConfigFlags)

	msgService = top_acc.getService('MessageSvc')
	msgService.Format = "S:%s E:%e % F%138W%S%7W%R%T  %0W%M"
	
	from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
	top_acc.merge(PoolReadCfg(ConfigFlags))

	from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
	top_acc.merge(TRT_GeometryCfg( ConfigFlags ))

	top_acc.merge(TRTPhaseCfg(ConfigFlags, ['TRTTracks_Phase', 'ExtendedTracksPhase'])) ## read from InDetKeys.TRT_Tracks_Phase, InDetKeys.ExtendedTracksPhase

	iovsvc = top_acc.getService('IOVDbSvc')
	iovsvc.OutputLevel=5
	top_acc.run(25)
	##top_acc.store(open("test_TRTPhaseConfig.pkl", "w"))  ##comment out to store top_acc into pkl file