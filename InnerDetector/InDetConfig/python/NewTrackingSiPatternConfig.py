# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory     import CompFactory
from InDetRecExample.InDetKeys		      import InDetKeys
import TrackingCommonConfig                     as   TC
import AthenaCommon.SystemOfUnits               as   Units

################################################### Configuration KalmanFitter ############################################################

def Trk__KalmanOutlierLogicCfg(name="InDetKOL", **kwargs) :
	acc = ComponentAccumulator()
	kwargs.setdefault("TrackChi2PerNDFCut"       , 17.0)
	kwargs.setdefault("StateChi2PerNDFCut"       , 12.5)

	PublicKOL = CompFactory.Trk__KalmanOutlierLogic
	InDetKOL = PublicKOL(name = name, **kwargs)
	acc.setPrivateTools(InDetKOL)
	return acc

def Trk__KalmanSmootherCfg(name="InDetBKS", **kwargs) :
	acc = ComponentAccumulator()
	kwargs.setdefault("InitialCovarianceSeedFactor"       , 200.0)
	#kwargs.setdefault("ExtrapolationEngine"       , ATLAS_FatrasExEngine) #was commented
	kwargs.setdefault("UseExtrapolationEngine"       , True)


	PublicBKS = CompFactory.Trk__KalmanSmoother
	InDetBKS = PublicBKS(name = name, **kwargs)
	acc.setPrivateTools(InDetBKS)
	return acc

def Trk__ForwardKalmanFitterCfg(name="InDetFKF", **kwargs) :
	acc = ComponentAccumulator()
	kwargs.setdefault("StateChi2PerNDFPreCut"       , 30.0)
	#kwargs.setdefault("ExtrapolationEngine"       , ATLAS_FatrasExEngine) #was commented
	kwargs.setdefault("UseExtrapolationEngine"       , True)

	PublicFKF = CompFactory.Trk__ForwardKalmanFitter
	InDetFKF = PublicFKF(name = name, **kwargs)
	acc.setPrivateTools(InDetFKF)
	return acc

def Trk__KalmanUpdatorCfg(name="InDetUpdator", **kwargs) :
	acc = ComponentAccumulator()
	InDetUpdator = CompFactory.Trk__KalmanUpdator(name = name)
	acc.setPrivateTools(InDetUpdator)
	return acc

def SCT_PlanarClusterOnTrackToolCfg(name="SCT_PlanarClusterOnTrackTool", **kwargs) :
	acc = ComponentAccumulator()
	SCT_PlanarClusterOnTrackTool = CompFactory.iFatras__PlanarClusterOnTrackTool(name = name)
	acc.setPrivateTools(SCT_PlanarClusterOnTrackTool)
	return acc

def PixelPlanarClusterOnTrackToolCfg(name="PixelPlanarClusterOnTrackTool", **kwargs) :
	acc = ComponentAccumulator()
	PixelPlanarClusterOnTrackTool = CompFactory.iFatras__PlanarClusterOnTrackTool(name = name)
	acc.setPrivateTools(PixelPlanarClusterOnTrackTool)
	return acc

def Trk__RIO_OnTrackCreatorCfg(name="InDetRotCreator", **kwargs) :
	acc = ComponentAccumulator()
	#
	# set up extrapolator
	#
	PixelPlanarClusterOnTrackTool = acc.popToolsAndMerge(PixelPlanarClusterOnTrackToolCfg())
	acc.addPublicTool(PixelPlanarClusterOnTrackTool)

	SCT_PlanarClusterOnTrackTool = acc.popToolsAndMerge(SCT_PlanarClusterOnTrackToolCfg())
	acc.addPublicTool(SCT_PlanarClusterOnTrackTool)

	kwargs.setdefault("ToolPixelCluster"       , PixelPlanarClusterOnTrackTool)
	kwargs.setdefault("ToolSCT_Cluster"       , SCT_PlanarClusterOnTrackTool)
	kwargs.setdefault("ToolMuonCluster"       , None)
	kwargs.setdefault("ToolMuonDriftCircle"       , None)
	kwargs.setdefault("ToolTRT_DriftCircle"       , None)
	kwargs.setdefault("Mode"       , 'indet')

	InDetRotCreator = CompFactory.Trk__RIO_OnTrackCreator(name = name, **kwargs)
	acc.setPrivateTools(InDetRotCreator)
	return acc

def Trk__KalmanFitterCfg(flags, name="InDetTrackFitter", **kwargs) :
	acc = ComponentAccumulator()

	InDetExtrapolator = acc.popToolsAndMerge(TC.InDetExtrapolatorCfg(flags))
	acc.addPublicTool(InDetExtrapolator)

	InDetRotCreator = acc.popToolsAndMerge(Trk__RIO_OnTrackCreatorCfg())
	acc.addPublicTool(InDetRotCreator)

	InDetUpdator = acc.popToolsAndMerge(Trk__KalmanUpdatorCfg())
	acc.addPublicTool(InDetUpdator)

	InDetFKF = acc.popToolsAndMerge(Trk__ForwardKalmanFitterCfg())
	acc.addPublicTool(InDetFKF)

	InDetBKS = acc.popToolsAndMerge(Trk__KalmanSmootherCfg())
	acc.addPublicTool(InDetBKS)

	InDetKOL = acc.popToolsAndMerge(Trk__KalmanOutlierLogicCfg())
	acc.addPublicTool(InDetKOL)

	kwargs.setdefault("ExtrapolatorHandle"       , InDetExtrapolator)
	kwargs.setdefault("RIO_OnTrackCreatorHandle"       , InDetRotCreator)
	kwargs.setdefault("MeasurementUpdatorHandle"       , InDetUpdator)
	kwargs.setdefault("ForwardKalmanFitterHandle"       , InDetFKF)
	kwargs.setdefault("KalmanSmootherHandle"       , InDetBKS)
	kwargs.setdefault("KalmanOutlierLogicHandle"       , InDetKOL)
	kwargs.setdefault("DynamicNoiseAdjustorHandle"       , None)
	kwargs.setdefault("BrempointAnalyserHandle"       , None)
	kwargs.setdefault("AlignableSurfaceProviderHandle"       , None)
	kwargs.setdefault("RecalibratorHandle"       , None)
	kwargs.setdefault("InternalDAFHandle"       , None)

	ConfiguredKalmanFitter = CompFactory.Trk__KalmanFitter
	InDetTrackFitter = ConfiguredKalmanFitter(name = name, **kwargs)
	acc.setPrivateTools(InDetTrackFitter)
	return acc
###########################################################################################################################################

def SiSpacePointsSeedMakerCfg(flags, name="InDetSpSeedsMaker", InputCollections = None, NewTrackingCuts = None, **kwargs) :
	acc = ComponentAccumulator()
	#
	# --- decide if use the association tool
	#
	if (len(InputCollections) > 0) and (NewTrackingCuts.mode() == "LowPt" or NewTrackingCuts.mode() == "VeryLowPt" or NewTrackingCuts.mode() == "LargeD0" or NewTrackingCuts.mode() == "LowPtLargeD0" or NewTrackingCuts.mode() == "BeamGas" or NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "ForwardSLHCTracks"  or NewTrackingCuts.mode() == "Disappearing" or NewTrackingCuts.mode() == "VeryForwardSLHCTracks" or NewTrackingCuts.mode() == "SLHCConversionFinding"):
		usePrdAssociationTool = True
	else:
		usePrdAssociationTool = False
	#
	# --- get list of already associated hits (always do this, even if no other tracking ran before)
	#
	if usePrdAssociationTool:
		prefix     = 'InDet'
		suffix     = NewTrackingCuts.extension()
		acc.merge(TC.InDetTrackPRD_AssociationCfg(flags,	namePrefix = prefix,
															nameSuffix = suffix,
															TracksName = list(InputCollections)))
	# ------------------------------------------------------------
	#
	# ----------- SiSPSeededTrackFinder
	#
	# ------------------------------------------------------------

	#
	# --- Space points seeds maker, use different ones for cosmics and collisions
	#
	if NewTrackingCuts.mode() == "DBM":
		SiSpacePointsSeedMaker = CompFactory.InDet__SiSpacePointsSeedMaker_ATLxk
	elif flags.InDet.doCosmics:
		SiSpacePointsSeedMaker = CompFactory.InDet__SiSpacePointsSeedMaker_Cosmic
	elif flags.InDet.doHeavyIon:
		SiSpacePointsSeedMaker = CompFactory.InDet__SiSpacePointsSeedMaker_HeavyIon
	elif NewTrackingCuts.mode() == "LowPt" or NewTrackingCuts.mode() == "VeryLowPt" or (NewTrackingCuts.mode() == "Pixel" and flags.InDet.doMinBias) :
		SiSpacePointsSeedMaker = CompFactory.InDet__SiSpacePointsSeedMaker_LowMomentum
	elif NewTrackingCuts.mode() == "BeamGas":
		SiSpacePointsSeedMaker = CompFactory.InDet__SiSpacePointsSeedMaker_BeamGas
	elif NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "VeryForwardSLHCTracks" :
		SiSpacePointsSeedMaker = CompFactory.InDet__SiSpacePointsSeedMaker_ITK
	else:
		SiSpacePointsSeedMaker = CompFactory.InDet__SiSpacePointsSeedMaker_ATLxk

	kwargs.setdefault("pTmin"       , NewTrackingCuts.minPT())
	kwargs.setdefault("maxdImpact"       , NewTrackingCuts.maxPrimaryImpact())
	kwargs.setdefault("maxZ"       , NewTrackingCuts.maxZImpact())
	kwargs.setdefault("minZ"       , -NewTrackingCuts.maxZImpact())
	kwargs.setdefault("usePixel"       , flags.DetFlags.haveRIO.pixel_on) #NewTrackingCuts.usePixel(),  ###FIX THIS!!
	kwargs.setdefault("SpacePointsPixelName"       , InDetKeys.PixelSpacePoints)
	kwargs.setdefault("useSCT"       , flags.DetFlags.haveRIO.SCT_on) ###(NewTrackingCuts.useSCT() and NewTrackingCuts.useSCTSeeding())
	kwargs.setdefault("SpacePointsSCTName"       , InDetKeys.SCT_SpacePoints)
	kwargs.setdefault("useOverlapSpCollection"       , (NewTrackingCuts.useSCT() and NewTrackingCuts.useSCTSeeding()))
	kwargs.setdefault("SpacePointsOverlapName"       , InDetKeys.OverlapSpacePoints)
	kwargs.setdefault("radMax"       , NewTrackingCuts.radMax())
	kwargs.setdefault("RapidityCut"       ,  NewTrackingCuts.maxEta())


	if NewTrackingCuts.mode() == "Offline" or flags.InDet.doHeavyIon or  NewTrackingCuts.mode() == "ForwardTracks":
		kwargs.setdefault("maxdImpactPPS"       , NewTrackingCuts.maxdImpactPPSSeeds())
		kwargs.setdefault("maxdImpactSSS"       , NewTrackingCuts.maxdImpactSSSSeeds())
	if usePrdAssociationTool:
		# not all classes have that property !!!
		kwargs.setdefault("PRDtoTrackMap"       , prefix+'PRDtoTrackMap'+ suffix \
															if usePrdAssociationTool else '')
	if not flags.InDet.doCosmics:
		kwargs.setdefault("maxRadius1"       , 0.75*NewTrackingCuts.radMax())
		kwargs.setdefault("maxRadius2"       , NewTrackingCuts.radMax())
		kwargs.setdefault("maxRadius3"       , NewTrackingCuts.radMax())
	if NewTrackingCuts.mode() == "LowPt" or NewTrackingCuts.mode() == "VeryLowPt" or (NewTrackingCuts.mode() == "Pixel" and flags.InDet.doMinBias):
		kwargs.setdefault("pTmax"       , NewTrackingCuts.maxPT())
		kwargs.setdefault("mindRadius"       , 4.0)
	if NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "SLHCConversionFinding":
		kwargs.setdefault("minRadius1"       ,0)
		kwargs.setdefault("minRadius2"       ,0)
		kwargs.setdefault("minRadius3"       ,0)
		kwargs.setdefault("maxRadius1"       ,1000.*Units.mm)
		kwargs.setdefault("maxRadius2"       ,1000.*Units.mm)
		kwargs.setdefault("maxRadius3"       ,1000.*Units.mm)
	if NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "VeryForwardSLHCTracks":
		kwargs.setdefault("checkEta"       ,True)
		kwargs.setdefault("etaMin"       ,NewTrackingCuts.minEta())
		kwargs.setdefault("etaMax"       ,NewTrackingCuts.maxEta())
		kwargs.setdefault("RapidityCut"       ,NewTrackingCuts.maxEta())
	if NewTrackingCuts.mode() == "DBM":
		kwargs.setdefault("etaMin"       ,NewTrackingCuts.minEta())
		kwargs.setdefault("etaMax"       ,NewTrackingCuts.maxEta())
		kwargs.setdefault("useDBM"       ,True)
	
	
	InDetSiSpacePointsSeedMaker = SiSpacePointsSeedMaker (	name = name, **kwargs)

	acc.setPrivateTools(InDetSiSpacePointsSeedMaker)
	return acc


def InDet__SiZvertexMaker_xkCfg(flags, name="InDetZvertexMaker", InputCollections = None, NewTrackingCuts = None, **kwargs) :
	acc = ComponentAccumulator()
	#
	# --- Z-coordinates primary vertices finder (only for collisions)
	#
	InDetSiSpacePointsSeedMaker = acc.popToolsAndMerge(SiSpacePointsSeedMakerCfg(flags, name = "InDetSpSeedsMaker" + NewTrackingCuts.extension(), 
																						InputCollections = InputCollections, 
																						NewTrackingCuts = NewTrackingCuts ))
	acc.addPublicTool(InDetSiSpacePointsSeedMaker)
	kwargs.setdefault("Zmax"       ,NewTrackingCuts.maxZImpact())
	kwargs.setdefault("Zmin"       ,-NewTrackingCuts.maxZImpact())
	kwargs.setdefault("minRatio"       ,0.17)
	kwargs.setdefault("SeedMakerTool"       ,InDetSiSpacePointsSeedMaker)
	if flags.InDet.doHeavyIon:
		kwargs.setdefault("HistSize"       ,2000)
		kwargs.setdefault("minContent"       ,30) #200

	InDetZvertexMaker = CompFactory.InDet__SiZvertexMaker_xk(name = name, **kwargs)
	acc.setPrivateTools(InDetZvertexMaker)
	return acc


def InDet__SiDetElementsRoadMaker_xkCfg(flags, name="InDetSiRoadMaker", NewTrackingCuts = None, **kwargs) :
	acc = ComponentAccumulator()
	#
	# --- SCT and Pixel detector elements road builder
	#
	InDetPatternPropagator = TC.InDetPatternPropagatorCfg(flags)
	acc.addPublicTool(InDetPatternPropagator)
	kwargs.setdefault("PropagatorTool"       ,InDetPatternPropagator)
	kwargs.setdefault("usePixel"       , flags.DetFlags.haveRIO.pixel_on) #NewTrackingCuts.usePixel()
	kwargs.setdefault("PixManagerLocation"       ,InDetKeys.PixelManager)
	kwargs.setdefault("useSCT"       ,flags.DetFlags.haveRIO.SCT_on) #NewTrackingCuts.useSCT()
	kwargs.setdefault("SCTManagerLocation"       ,InDetKeys.SCT_Manager)
	kwargs.setdefault("RoadWidth"       ,NewTrackingCuts.RoadWidth())

	InDetSiDetElementsRoadMaker = CompFactory.InDet__SiDetElementsRoadMaker_xk(name = name, **kwargs)
	acc.setPrivateTools(InDetSiDetElementsRoadMaker)
	return acc


def InDet__SiDetElementBoundaryLinksCondAlg_xkCfg(name="InDetSiDetElementBoundaryLinksPixelCondAlg", **kwargs) :
	acc = ComponentAccumulator()

	kwargs.setdefault("ReadKey"       ,"PixelDetectorElementCollection")
	kwargs.setdefault("WriteKey"       ,"PixelDetElementBoundaryLinks_xk")
	acc.addEventAlgo(CompFactory.InDet__SiDetElementBoundaryLinksCondAlg_xk(name = name))
	return acc


def InDet__SiCombinatorialTrackFinder_xkCfg(flags, name="InDetSiComTrackFinder", NewTrackingCuts = None, **kwargs) :
	acc = ComponentAccumulator()
	#
	# --- Local track finding using sdCaloSeededSSSpace point seed
	#
	# @TODO ensure that PRD association map is used if usePrdAssociationTool is set
	is_dbm = flags.InDet.doDBMstandalone or NewTrackingCuts.extension()=='DBM'
	if not is_dbm:
		acc.merge(TC.InDetRotCreatorDigitalCfg(flags))
		rot_creator_digital = acc.getPublicTool('InDetRotCreatorDigital')
	else:
		acc.merge(TC.InDetRotCreatorDBMCfg(flags))
		rot_creator_digital = acc.getPublicTool('InDetRotCreatorDBM')

	InDetPatternPropagator = TC.InDetPatternPropagatorCfg(flags)
	acc.addPublicTool(InDetPatternPropagator)

	InDetPatternUpdator = TC.InDetPatternUpdatorCfg(flags)
	acc.addPublicTool(InDetPatternUpdator)

	kwargs.setdefault("PropagatorTool"       ,InDetPatternPropagator)
	kwargs.setdefault("UpdatorTool"       ,InDetPatternUpdator)
	kwargs.setdefault("RIOonTrackTool"       ,rot_creator_digital)
	kwargs.setdefault("usePixel"       ,flags.DetFlags.haveRIO.pixel_on)   ###FIX THIS!!
	kwargs.setdefault("useSCT"       ,flags.DetFlags.haveRIO.SCT_on if not is_dbm else False)
	kwargs.setdefault("PixelClusterContainer"       ,InDetKeys.PixelClusters)
	kwargs.setdefault("SCT_ClusterContainer"       ,InDetKeys.SCT_Clusters)

	if is_dbm :
		kwargs.setdefault("MagneticFieldMode"       ,"NoField")
		kwargs.setdefault("TrackQualityCut"       ,9.3)

	if (flags.DetFlags.haveRIO.SCT_on):    ###FIX THIS!!
		InDetSCT_ConditionsSummaryTool = CompFactory.SCT_ConditionsSummaryTool(name = 'InDetSCT_ConditionsSummaryTool')
		acc.addPublicTool(InDetSCT_ConditionsSummaryTool)
		kwargs.setdefault("SctSummaryTool"       ,InDetSCT_ConditionsSummaryTool)
	else:
		kwargs.setdefault("SctSummaryTool"       ,None)

	track_finder = CompFactory.InDet__SiCombinatorialTrackFinder_xk(name = name, **kwargs)
	acc.setPrivateTools(track_finder)
	return acc


def InDet__SiTrackMaker_xkCfg(flags, name="InDetSiTrackMaker", InputCollections = None, NewTrackingCuts = None, **kwargs) :
	acc = ComponentAccumulator()
	useBremMode = NewTrackingCuts.mode() == "Offline" or NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "DBM"
	InDetSiDetElementsRoadMaker = acc.popToolsAndMerge(InDet__SiDetElementsRoadMaker_xkCfg(flags,	name = 'InDetSiRoadMaker'+ NewTrackingCuts.extension(), 
													NewTrackingCuts = NewTrackingCuts ))
	acc.addPublicTool(InDetSiDetElementsRoadMaker)
	track_finder = acc.popToolsAndMerge(InDet__SiCombinatorialTrackFinder_xkCfg(flags, name = 'InDetSiComTrackFinder'+ NewTrackingCuts.extension(), NewTrackingCuts = NewTrackingCuts ))
	acc.addPublicTool(track_finder)

	#
	# --- decide if use the association tool
	#
	if (len(InputCollections) > 0) and (NewTrackingCuts.mode() == "LowPt" or NewTrackingCuts.mode() == "VeryLowPt" or NewTrackingCuts.mode() == "LargeD0" or NewTrackingCuts.mode() == "LowPtLargeD0" or NewTrackingCuts.mode() == "BeamGas" or NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "ForwardSLHCTracks"  or NewTrackingCuts.mode() == "Disappearing" or NewTrackingCuts.mode() == "VeryForwardSLHCTracks" or NewTrackingCuts.mode() == "SLHCConversionFinding"):
		usePrdAssociationTool = True
	else:
		usePrdAssociationTool = False

	kwargs.setdefault("useSCT"       , flags.DetFlags.haveRIO.SCT_on)  #NewTrackingCuts.useSCT()
	kwargs.setdefault("usePixel"       , flags.DetFlags.haveRIO.pixel_on) #NewTrackingCuts.usePixel()
	kwargs.setdefault("RoadTool"       , InDetSiDetElementsRoadMaker)
	kwargs.setdefault("CombinatorialTrackFinder"       , track_finder)
	kwargs.setdefault("pTmin"       , NewTrackingCuts.minPT())
	kwargs.setdefault("pTminBrem"       , NewTrackingCuts.minPTBrem())
	kwargs.setdefault("pTminSSS"       , flags.InDet.pT_SSScut)
	kwargs.setdefault("nClustersMin"       , NewTrackingCuts.minClusters())
	kwargs.setdefault("nHolesMax"       , NewTrackingCuts.nHolesMax())
	kwargs.setdefault("nHolesGapMax"       , NewTrackingCuts.nHolesGapMax())
	kwargs.setdefault("SeedsFilterLevel"       , NewTrackingCuts.seedFilterLevel())
	kwargs.setdefault("Xi2max"       , NewTrackingCuts.Xi2max())
	kwargs.setdefault("Xi2maxNoAdd"       , NewTrackingCuts.Xi2maxNoAdd())
	kwargs.setdefault("nWeightedClustersMin"       , NewTrackingCuts.nWeightedClustersMin())
	kwargs.setdefault("CosmicTrack"       , flags.InDet.doCosmics)  #############################!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!fixme!!!!!!!!!!!!!!!!!!!!
	kwargs.setdefault("Xi2maxMultiTracks"       , NewTrackingCuts.Xi2max())
	kwargs.setdefault("useSSSseedsFilter"       , flags.InDet.doSSSfilter)
	kwargs.setdefault("doMultiTracksProd"       , True)
	kwargs.setdefault("useBremModel"       , flags.InDet.doBremRecovery and useBremMode) # only for NewTracking the brem is debugged !!!
	kwargs.setdefault("doCaloSeededBrem"       , flags.InDet.doCaloSeededBrem)
	kwargs.setdefault("doHadCaloSeedSSS"       , flags.InDet.doHadCaloSeededSSS)
	kwargs.setdefault("phiWidth"       ,  NewTrackingCuts.phiWidthBrem())
	kwargs.setdefault("etaWidth"       ,  NewTrackingCuts.etaWidthBrem())
	kwargs.setdefault("InputClusterContainerName"       ,  InDetKeys.CaloClusterROIContainer)
	kwargs.setdefault("InputHadClusterContainerName"       ,  InDetKeys.HadCaloClusterROIContainer)
	kwargs.setdefault("UseAssociationTool"       ,  usePrdAssociationTool)

	
	if NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "VeryForwardSLHCTracks" :
			kwargs.setdefault("ITKGeometry"       , True)

	if NewTrackingCuts.mode() == "DBM":
		kwargs.setdefault("MagneticFieldMode"       , "NoField")
		kwargs.setdefault("useBremModel"       , False)
		kwargs.setdefault("doMultiTracksProd"       , False)
		kwargs.setdefault("TrackPatternRecoInfo"       , 'SiSPSeededFinder')
		kwargs.setdefault("pTminSSS"       , -1)
		kwargs.setdefault("CosmicTrack"       , False)
		kwargs.setdefault("useSSSseedsFilter"       , False)
		kwargs.setdefault("doCaloSeededBrem"       , False)
		kwargs.setdefault("doHadCaloSeedSSS"       , False)


	elif flags.InDet.doCosmics:  #############fixme
		kwargs.setdefault("TrackPatternRecoInfo"       , 'SiSpacePointsSeedMaker_Cosmic')

	elif flags.InDet.doHeavyIon:
		kwargs.setdefault("TrackPatternRecoInfo"       , 'SiSpacePointsSeedMaker_HeavyIon')
	
	elif NewTrackingCuts.mode() == "LowPt":
		kwargs.setdefault("TrackPatternRecoInfo"       , 'SiSpacePointsSeedMaker_LowMomentum')

	elif NewTrackingCuts.mode() == "VeryLowPt" or (NewTrackingCuts.mode() == "Pixel" and flags.InDet.doMinBias):
		kwargs.setdefault("TrackPatternRecoInfo"       , 'SiSpacePointsSeedMaker_VeryLowMomentum')

	elif NewTrackingCuts.mode() == "BeamGas":
		kwargs.setdefault("TrackPatternRecoInfo"       , 'SiSpacePointsSeedMaker_BeamGas')

	elif NewTrackingCuts.mode() == "ForwardTracks":
		kwargs.setdefault("TrackPatternRecoInfo"       , 'SiSpacePointsSeedMaker_ForwardTracks')

	elif NewTrackingCuts.mode() == "ForwardSLHCTracks":
		kwargs.setdefault("TrackPatternRecoInfo"       , 'SiSpacePointsSeedMaker_ForwardSLHCTracks')

	elif NewTrackingCuts.mode() == "VeryForwardSLHCTracks":
		kwargs.setdefault("TrackPatternRecoInfo"       , 'SiSpacePointsSeedMaker_VeryForwardSLHCTracks')

	elif NewTrackingCuts.mode() == "SLHCConversionFinding":
		kwargs.setdefault("TrackPatternRecoInfo"       , 'SiSpacePointsSeedMaker_SLHCConversionTracks')

	elif NewTrackingCuts.mode() == "LargeD0" or NewTrackingCuts.mode() == "LowPtLargeD0":
		kwargs.setdefault("TrackPatternRecoInfo"       , 'SiSpacePointsSeedMaker_LargeD0')

	else:
		kwargs.setdefault("TrackPatternRecoInfo"       , 'SiSPSeededFinder')
			
	if flags.InDet.doStoreTrackSeeds:
		InDet_SeedToTrackConversion = CompFactory.InDet__SeedToTrackConversionTool(name = "InDet_SeedToTrackConversion",
																					OutputName = InDetKeys.SiSPSeedSegments + NewTrackingCuts.extension())
		acc.addPublicTool(InDet_SeedToTrackConversion)
		kwargs.setdefault("SeedToTrackConversion"       , InDet_SeedToTrackConversion)
		kwargs.setdefault("SeedSegmentsWrite"       , True)

	InDetSiTrackMaker = CompFactory.InDet__SiTrackMaker_xk(name = name, **kwargs)
	acc.setPrivateTools(InDetSiTrackMaker)
	return acc


def InDet__SiSPSeededTrackFinderCfg(flags, name="InDetSiSpTrackFinder", InputCollections = None, SiSPSeededTrackCollectionKey = None, NewTrackingCuts = None, **kwargs) :
	acc = ComponentAccumulator()
	# set output track collection name
	#
	SiTrackCollection = SiSPSeededTrackCollectionKey
	#
	# --- decide if use the association tool
	#
	if (len(InputCollections) > 0) and (NewTrackingCuts.mode() == "LowPt" or NewTrackingCuts.mode() == "VeryLowPt" or NewTrackingCuts.mode() == "LargeD0" or NewTrackingCuts.mode() == "LowPtLargeD0" or NewTrackingCuts.mode() == "BeamGas" or NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "ForwardSLHCTracks"  or NewTrackingCuts.mode() == "Disappearing" or NewTrackingCuts.mode() == "VeryForwardSLHCTracks" or NewTrackingCuts.mode() == "SLHCConversionFinding"):
		usePrdAssociationTool = True
	else:
		usePrdAssociationTool = False
	#
	# --- get list of already associated hits (always do this, even if no other tracking ran before)
	#
	if usePrdAssociationTool:
		prefix     = 'InDet'
		suffix     = NewTrackingCuts.extension()

	InDetSiTrackMaker = acc.popToolsAndMerge(InDet__SiTrackMaker_xkCfg(flags,	name = 'InDetSiTrackMaker'+ NewTrackingCuts.extension(), 
																				InputCollections = InputCollections, 
																				NewTrackingCuts = NewTrackingCuts ))
	acc.addPublicTool(InDetSiTrackMaker)

	InDetTrackSummaryToolNoHoleSearch = acc.popToolsAndMerge(TC.InDetTrackSummaryToolCfg(flags, name='InDetTrackSummaryToolNoHoleSearch', 
																								doHolesInDet = False))
	acc.addPublicTool(InDetTrackSummaryToolNoHoleSearch)

	InDetSiSpacePointsSeedMaker = acc.popToolsAndMerge(SiSpacePointsSeedMakerCfg(flags, name = "InDetSpSeedsMaker" + NewTrackingCuts.extension(), 
																						InputCollections = InputCollections, 
																						NewTrackingCuts = NewTrackingCuts ))
	acc.addPublicTool(InDetSiSpacePointsSeedMaker)
	
	#
	# --- Z-coordinates primary vertices finder (only for collisions)
	#
	if flags.InDet.useZvertexTool and NewTrackingCuts.mode() != "DBM":
		InDetZvertexMaker = acc.popToolsAndMerge(InDet__SiZvertexMaker_xkCfg(flags, name = 'InDetZvertexMaker'+ NewTrackingCuts.extension(), 
																					InputCollections = InputCollections,
																					NewTrackingCuts  = NewTrackingCuts ))
		acc.addPublicTool(InDetZvertexMaker)
	else:
		InDetZvertexMaker = None

	#
	# --- Setup Track finder using space points seeds
	#
	if NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "ForwardTracks":

		kwargs.setdefault("TrackTool"       ,InDetSiTrackMaker)
		kwargs.setdefault("PRDtoTrackMap"       ,prefix+'PRDtoTrackMap'+suffix \
													if usePrdAssociationTool else '')
		kwargs.setdefault("TrackSummaryTool"       ,InDetTrackSummaryToolNoHoleSearch)
		kwargs.setdefault("TracksLocation"       ,SiTrackCollection)
		kwargs.setdefault("SeedsTool"       ,InDetSiSpacePointsSeedMaker)
		kwargs.setdefault("useZvertexTool"       ,flags.InDet.useZvertexTool)
		kwargs.setdefault("ZvertexTool"       ,InDetZvertexMaker)
		kwargs.setdefault("useNewStrategy"       ,False)
		kwargs.setdefault("useMBTSTimeDiff"       ,flags.InDet.useMBTSTimeDiff)
		kwargs.setdefault("useZBoundFinding"       ,False)

		if flags.InDet.doHeavyIon :
			kwargs.setdefault("FreeClustersCut"       ,2) #Heavy Ion optimization from Igor
		InDetSiSPSeededTrackFinder = CompFactory.InDet__SiSPSeededTrackFinder(name = name, **kwargs)
	
	else:
		kwargs.setdefault("TrackTool"       ,InDetSiTrackMaker)
		kwargs.setdefault("PRDtoTrackMap"       ,prefix+'PRDtoTrackMap'+suffix \
													if usePrdAssociationTool else '')
		kwargs.setdefault("TrackSummaryTool"       ,InDetTrackSummaryToolNoHoleSearch)
		kwargs.setdefault("TracksLocation"       ,SiTrackCollection)
		kwargs.setdefault("SeedsTool"       ,InDetSiSpacePointsSeedMaker)
		kwargs.setdefault("useZvertexTool"       ,flags.InDet.useZvertexTool and NewTrackingCuts.mode() != "DBM")
		kwargs.setdefault("ZvertexTool"       ,InDetZvertexMaker)
		kwargs.setdefault("useNewStrategy"       ,flags.InDet.useNewSiSPSeededTF and NewTrackingCuts.mode() != "DBM")
		kwargs.setdefault("useMBTSTimeDiff"       ,flags.InDet.useMBTSTimeDiff)
		kwargs.setdefault("useZBoundFinding"       ,NewTrackingCuts.doZBoundary() and NewTrackingCuts.mode() != "DBM")

		if flags.InDet.doHeavyIon :   ##########################fix me
			kwargs.setdefault("FreeClustersCut"       ,2) #Heavy Ion optimization from Igor

		InDetSiSPSeededTrackFinder = CompFactory.InDet__SiSPSeededTrackFinder(name = name, **kwargs)
	acc.addEventAlgo(InDetSiSPSeededTrackFinder)
	return acc


def InDet__InDetAmbiTrackSelectionToolCfg(flags, name="InDetAmbiTrackSelectionTool", NewTrackingCuts = None, **kwargs) :
	acc = ComponentAccumulator()
	
	# ------------------------------------------------------------
	#
	# ---------- Ambiguity solving
	#
	# ------------------------------------------------------------

	#
	# --- load InnerDetector TrackSelectionTool
	#
	prob1 = flags.InDet.pixelClusterSplitProb1
	prob2 = flags.InDet.pixelClusterSplitProb2
	nhitsToAllowSplitting = 9
	
	from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags
	if CommonGeometryFlags.Run() == 1:
		prob1 = flags.InDet.pixelClusterSplitProb1_run1
		prob2 = flags.InDet.pixelClusterSplitProb2_run1
		nhitsToAllowSplitting = 8

	if flags.InDet.doTIDE_Ambi and not (NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "DBM"):
		AmbiTrackSelectionTool = CompFactory.InDet__InDetDenseEnvAmbiTrackSelectionTool
	else:
		AmbiTrackSelectionTool = CompFactory.InDet__InDetAmbiTrackSelectionTool
	InDetTRTDriftCircleCut = TC.InDetTRTDriftCircleCutForPatternRecoCfg(flags, TrackingCuts = NewTrackingCuts)
	acc.addPublicTool(InDetTRTDriftCircleCut)

	InDetPRDtoTrackMapToolGangedPixels = TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags)
	acc.addPublicTool(InDetPRDtoTrackMapToolGangedPixels)

	kwargs.setdefault("DriftCircleCutTool", InDetTRTDriftCircleCut)
	kwargs.setdefault("AssociationTool" , InDetPRDtoTrackMapToolGangedPixels)
	kwargs.setdefault("minHits"         , NewTrackingCuts.minClusters())
	kwargs.setdefault("minNotShared"    , NewTrackingCuts.minSiNotShared())
	kwargs.setdefault("maxShared"       , NewTrackingCuts.maxShared())
	kwargs.setdefault("minTRTHits"      , 0) # used for Si only tracking !!!
	kwargs.setdefault("sharedProbCut"   , 0.10)
	kwargs.setdefault("UseParameterization" , False)
	kwargs.setdefault("Cosmics"             , flags.InDet.doCosmics)
	kwargs.setdefault("doPixelSplitting"    , flags.InDet.doPixelClusterSplitting and NewTrackingCuts.mode != "DBM")
	
	if flags.InDet.doTIDE_Ambi and not (NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "DBM"):
		kwargs.setdefault("sharedProbCut"             , prob1)
		kwargs.setdefault("sharedProbCut2"            , prob2)
		kwargs.setdefault("minSiHitsToAllowSplitting" , nhitsToAllowSplitting)
		kwargs.setdefault("minUniqueSCTHits"          , 4)
		kwargs.setdefault("minTrackChi2ForSharedHits" , 3)
		kwargs.setdefault("InputHadClusterContainerName" , InDetKeys.HadCaloClusterROIContainer + "Bjet")
		kwargs.setdefault("doHadCaloSeed"             , flags.InDet.doCaloSeededAmbi)   #Do special cuts in region of interest
		kwargs.setdefault("minPtSplit"                , flags.InDet.pixelClusterSplitMinPt)       #Only allow split clusters on track withe pt greater than this MeV
		kwargs.setdefault("maxSharedModulesInROI"     , 3)     #Maximum number of shared modules for tracks in ROI
		kwargs.setdefault("minNotSharedInROI"         , 2)     #Minimum number of unique modules for tracks in ROI
		kwargs.setdefault("minSiHitsToAllowSplittingInROI" , 7)  #Minimum number of Si hits to allow splittings for tracks in ROI
		kwargs.setdefault("phiWidth"                  , 0.1)     #Split cluster ROI size
		kwargs.setdefault("etaWidth"                  , 0.1)     #Split cluster ROI size
		kwargs.setdefault("InputEmClusterContainerName" , InDetKeys.CaloClusterROIContainer)
		kwargs.setdefault("doEmCaloSeed"              , False)   #Only split in cluster in region of interest
		kwargs.setdefault("minPtConv"                 , 10000)   #Only allow split clusters on track withe pt greater than this MeV
		kwargs.setdefault("phiWidthEM"                , 0.05)     #Split cluster ROI size
		kwargs.setdefault("etaWidthEM"                , 0.05)     #Split cluster ROI size
	
	if NewTrackingCuts.mode() == "DBM":
		kwargs.setdefault("Cosmics", False)
		kwargs.setdefault("UseParameterization"   , False)
		kwargs.setdefault("doPixelSplitting"      , False)
		kwargs.setdefault("maxShared"             , 1000)
		kwargs.setdefault("maxTracksPerSharedPRD" , 2)
		kwargs.setdefault("minHits"               , 0)
		kwargs.setdefault("minNotShared"          , 0)
		kwargs.setdefault("minScoreShareTracks"   , 0.0)
		kwargs.setdefault("minTRTHits"            , 0)
		kwargs.setdefault("sharedProbCut"         , 0.1)

	InDetAmbiTrackSelectionTool = AmbiTrackSelectionTool(name = name, **kwargs)
	acc.setPrivateTools(InDetAmbiTrackSelectionTool)
	return acc


def Trk__DenseEnvironmentsAmbiguityProcessorToolCfg(flags, name = "InDetAmbiguityProcessor", NewTrackingCuts = None, **kwargs) :
	acc = ComponentAccumulator()
	#
	# --- set up different Scoring Tool for collisions and cosmics
	#
	if flags.InDet.doCosmics and NewTrackingCuts.mode() != "DBM":
		kwargs.setdefault("NewTrackingCuts", NewTrackingCuts)
		InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetCosmicsScoringToolBaseCfg(flags, name='InDetCosmicsScoringTool' + NewTrackingCuts.extension(), **kwargs))
																		##**setDefaults(kwargs, NewTrackingCuts=NewTrackingCuts)))
		acc.addPublicTool(InDetAmbiScoringTool)
	else:
		InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetAmbiScoringToolCfg(flags, NewTrackingCuts = NewTrackingCuts))
		acc.addPublicTool(InDetAmbiScoringTool)
	#
	# --- load Ambiguity Processor
	#
	useBremMode = NewTrackingCuts.mode() == "Offline" or NewTrackingCuts.mode() == "SLHC"
		
	use_low_pt_fitter =  True if NewTrackingCuts.mode() == "LowPt" or NewTrackingCuts.mode() == "VeryLowPt" or (NewTrackingCuts.mode() == "Pixel" and flags.InDet.doMinBias) else False
	fitter_list = []
	if len(NewTrackingCuts.extension()) > 0 :
		if not use_low_pt_fitter:
			InDetTrackFitter = acc.popToolsAndMerge(Trk__KalmanFitterCfg(flags, name = 'InDetTrackFitter'+ NewTrackingCuts.extension()))
			acc.addPublicTool(InDetTrackFitter)
			fitter_list.append(InDetTrackFitter)
	
	InDetPRDtoTrackMapToolGangedPixels = TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags)
	acc.addPublicTool(InDetPRDtoTrackMapToolGangedPixels)

	InDetTrackSummaryTool = acc.popToolsAndMerge(TC.InDetTrackSummaryToolCfg(flags))
	acc.addPublicTool(InDetTrackSummaryTool)

	InDetAmbiTrackSelectionTool = acc.popToolsAndMerge(InDet__InDetAmbiTrackSelectionToolCfg(flags, name = 'InDetAmbiTrackSelectionTool'+ NewTrackingCuts.extension(), NewTrackingCuts = NewTrackingCuts ))
	acc.addPublicTool(InDetAmbiTrackSelectionTool)

	kwargs.setdefault("Fitter"             , fitter_list)
	kwargs.setdefault("AssociationTool"    , InDetPRDtoTrackMapToolGangedPixels)
	kwargs.setdefault("AssociationMapName" , 'PRDToTrackMap'+ NewTrackingCuts.extension())
	kwargs.setdefault("TrackSummaryTool"   , InDetTrackSummaryTool)
	kwargs.setdefault("ScoringTool"        , InDetAmbiScoringTool)
	kwargs.setdefault("SelectionTool"      , InDetAmbiTrackSelectionTool)
	kwargs.setdefault("SuppressHoleSearch" , False)
	kwargs.setdefault("tryBremFit"         , flags.InDet.doBremRecovery and useBremMode and NewTrackingCuts.mode() != "DBM")
	kwargs.setdefault("caloSeededBrem"     , flags.InDet.doCaloSeededBrem and NewTrackingCuts.mode() != "DBM")
	kwargs.setdefault("pTminBrem"          , NewTrackingCuts.minPTBrem())
	kwargs.setdefault("RefitPrds"          , True)
	kwargs.setdefault("doHadCaloSeed"      , flags.InDet.doCaloSeededRefit)
	kwargs.setdefault("InputHadClusterContainerName" , InDetKeys.HadCaloClusterROIContainer + "Bjet")
	
	if flags.InDet.doTIDE_Ambi and not (NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "DBM")  and 'NnPixelClusterSplitProbTool' in globals():
		kwargs.setdefault("applydRcorrection", True)
	if NewTrackingCuts.mode() == "Pixel" or NewTrackingCuts.mode() == "DBM":
		kwargs.setdefault("SuppressHoleSearch", True)
	if flags.InDet.materialInteractions:
		kwargs.setdefault("MatEffects", flags.InDet.materialInteractionsType)
	else:
		kwargs.setdefault("MatEffects", 0)

	# DenseEnvironmentsAmbiguityProcessorTool
	ProcessorTool = CompFactory.Trk__DenseEnvironmentsAmbiguityProcessorTool
	InDetAmbiguityProcessor = ProcessorTool(name = name, **kwargs)
	acc.setPrivateTools(InDetAmbiguityProcessor)
	return acc


def Trk__DenseEnvironmentsAmbiguityScoreProcessorToolCfg(flags, name = 'InDetAmbiguityScoreProcessor', NewTrackingCuts = None, **kwargs) :
	acc = ComponentAccumulator()
	#
	# --- set up different Scoring Tool for collisions and cosmics
	#
	if flags.InDet.doCosmics and NewTrackingCuts.mode() != "DBM":
		kwargs.setdefault("NewTrackingCuts", NewTrackingCuts)
		InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetCosmicsScoringToolBaseCfg(flags, name='InDetCosmicsScoringTool' + NewTrackingCuts.extension(), **kwargs))
																		##**setDefaults(kwargs, NewTrackingCuts=NewTrackingCuts)))
		acc.addPublicTool(InDetAmbiScoringTool)
	else:
		InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetAmbiScoringToolCfg(flags, NewTrackingCuts = NewTrackingCuts))
		acc.addPublicTool(InDetAmbiScoringTool)

	if flags.InDet.doAmbiSolving:
		#
		# --- load InnerDetector TrackSelectionTool
		#
		prob1 = flags.InDet.pixelClusterSplitProb1
		prob2 = flags.InDet.pixelClusterSplitProb2
		
		from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags
		if CommonGeometryFlags.Run() == 1:
			prob1 = flags.InDet.pixelClusterSplitProb1_run1
			prob2 = flags.InDet.pixelClusterSplitProb2_run1
	
	if flags.InDet.doTIDE_Ambi and not (NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "DBM")  and 'NnPixelClusterSplitProbTool' in globals():
		#kwargs.setdefault("SplitProbTool", NnPixelClusterSplitProbTool)  ## was commented
		kwargs.setdefault("sharedProbCut", prob1)
		kwargs.setdefault("sharedProbCut2", prob2)
		if NewTrackingCuts.extension() == "":
			kwargs.setdefault("SplitClusterMap_old", "")
		elif NewTrackingCuts.extension() == "Disappearing":
			kwargs.setdefault("SplitClusterMap_old", InDetKeys.SplitClusterAmbiguityMap)
			kwargs.setdefault("SplitClusterMap_new", InDetKeys.SplitClusterAmbiguityMap + NewTrackingCuts.extension())
	
	

	InDetPRDtoTrackMapToolGangedPixels = TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags)
	acc.addPublicTool(InDetPRDtoTrackMapToolGangedPixels)

	PRDtoTrackMapTool = TC.PRDtoTrackMapToolCfg()
	acc.addPublicTool(PRDtoTrackMapTool)

	InDetAmbiTrackSelectionTool = acc.popToolsAndMerge(InDet__InDetAmbiTrackSelectionToolCfg(flags, name = 'InDetAmbiTrackSelectionTool'+ NewTrackingCuts.extension(), NewTrackingCuts = NewTrackingCuts ))
	acc.addPublicTool(InDetAmbiTrackSelectionTool)

	kwargs.setdefault("ScoringTool"        , InDetAmbiScoringTool)
	kwargs.setdefault("AssociationTool"    , InDetPRDtoTrackMapToolGangedPixels)
	kwargs.setdefault("AssociationToolNotGanged"  , PRDtoTrackMapTool)
	kwargs.setdefault("AssociationMapName" , 'PRDToTrackMap'+ NewTrackingCuts.extension())
	kwargs.setdefault("SelectionTool"      , InDetAmbiTrackSelectionTool)

	# DenseEnvironmentsAmbiguityScoreProcessorTool
	ScoreProcessorTool = CompFactory.Trk__DenseEnvironmentsAmbiguityScoreProcessorTool
	InDetAmbiguityScoreProcessor = ScoreProcessorTool(name = 'InDetAmbiguityScoreProcessor'+ NewTrackingCuts.extension(), **kwargs)
	acc.setPrivateTools(InDetAmbiguityScoreProcessor)
	return acc


def Trk__SimpleAmbiguityProcessorToolCfg(flags, name="InDetAmbiguityProcessor", NewTrackingCuts = None, **kwargs) :
	acc = ComponentAccumulator()
	#
	# --- set up different Scoring Tool for collisions and cosmics
	#
	if flags.InDet.doCosmics and NewTrackingCuts.mode() != "DBM":
		kwargs.setdefault("NewTrackingCuts", NewTrackingCuts)
		InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetCosmicsScoringToolBaseCfg(flags, name='InDetCosmicsScoringTool' + NewTrackingCuts.extension(), **kwargs))
																		##**setDefaults(kwargs, NewTrackingCuts=NewTrackingCuts)))
		acc.addPublicTool(InDetAmbiScoringTool)
	else:
		InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetAmbiScoringToolCfg(flags, NewTrackingCuts = NewTrackingCuts))
		acc.addPublicTool(InDetAmbiScoringTool)

	InDetPRDtoTrackMapToolGangedPixels = TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags)
	acc.addPublicTool(InDetPRDtoTrackMapToolGangedPixels)

	InDetTrackSummaryTool = acc.popToolsAndMerge(TC.InDetTrackSummaryToolCfg(flags))
	acc.addPublicTool(InDetTrackSummaryTool)

	InDetAmbiTrackSelectionTool = acc.popToolsAndMerge(InDet__InDetAmbiTrackSelectionToolCfg(flags, name = 'InDetAmbiTrackSelectionTool'+ NewTrackingCuts.extension(), NewTrackingCuts = NewTrackingCuts ))
	acc.addPublicTool(InDetAmbiTrackSelectionTool)

	useBremMode = NewTrackingCuts.mode() == "Offline" or NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "DBM"
	use_low_pt_fitter =  True if NewTrackingCuts.mode() == "LowPt" or NewTrackingCuts.mode() == "VeryLowPt" or (NewTrackingCuts.mode() == "Pixel" and flags.InDet.doMinBias) else False
	fitter_list = []
	if len(NewTrackingCuts.extension()) > 0 :
		if not use_low_pt_fitter:
			InDetTrackFitter = acc.popToolsAndMerge(Trk__KalmanFitterCfg(flags, name = 'InDetTrackFitter'+ NewTrackingCuts.extension()))
			acc.addPublicTool(InDetTrackFitter)
			fitter_list.append(InDetTrackFitter)

	kwargs.setdefault("Fitter"             , fitter_list)
	kwargs.setdefault("AssociationTool"    , InDetPRDtoTrackMapToolGangedPixels)
	kwargs.setdefault("TrackSummaryTool"   , InDetTrackSummaryTool)
	kwargs.setdefault("ScoringTool"        , InDetAmbiScoringTool)
	kwargs.setdefault("SelectionTool"      , InDetAmbiTrackSelectionTool)
	kwargs.setdefault("SuppressHoleSearch" , False)
	kwargs.setdefault("tryBremFit"         , flags.InDet.doBremRecovery and useBremMode and NewTrackingCuts.mode() != "DBM")
	kwargs.setdefault("caloSeededBrem"     , flags.InDet.doCaloSeededBrem and NewTrackingCuts.mode() != "DBM")
	kwargs.setdefault("pTminBrem"          , NewTrackingCuts.minPTBrem())
	kwargs.setdefault("RefitPrds"          , True)

	if flags.InDet.doTIDE_Ambi and not (NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "DBM")  and 'NnPixelClusterSplitProbTool' in globals():
		kwargs.setdefault("applydRcorrection", True)
	if NewTrackingCuts.mode() == "Pixel" or NewTrackingCuts.mode() == "DBM":
		kwargs.setdefault("SuppressHoleSearch", True)
	if flags.InDet.materialInteractions:
		kwargs.setdefault("MatEffects", flags.InDet.materialInteractionsType)
	else:
		kwargs.setdefault("MatEffects", 0)

	ProcessorTool =  CompFactory.Trk__SimpleAmbiguityProcessorTool
	InDetAmbiguityProcessor = ProcessorTool(name = name, **kwargs)
	acc.setPrivateTools(InDetAmbiguityProcessor)
	return acc


def Trk__TrkAmbiguityScoreCfg(flags, name="InDetAmbiguityScore", NewTrackingCuts = None, SiSPSeededTrackCollectionKey = None, **kwargs) :
	acc = ComponentAccumulator()
	#
	# --- set input and output collection
	#
	SiTrackCollection = SiSPSeededTrackCollectionKey
	InDetAmbiguityScoreProcessor = acc.popToolsAndMerge(Trk__DenseEnvironmentsAmbiguityScoreProcessorToolCfg(flags, name = 'InDetAmbiguityScoreProcessor'+ NewTrackingCuts.extension(), NewTrackingCuts = NewTrackingCuts))
	acc.addPublicTool(InDetAmbiguityScoreProcessor)

	#
	# --- configure Ambiguity (score) solver
	#
	kwargs.setdefault("TrackInput"         , [ SiTrackCollection ])
	kwargs.setdefault("TrackOutput"        , 'ScoredMap'+'InDetAmbiguityScore'+ NewTrackingCuts.extension())
	kwargs.setdefault("AmbiguityScoreProcessor" ,  InDetAmbiguityScoreProcessor ) ## TODO: check the case when it is None object
	
	InDetAmbiguityScore = CompFactory.Trk__TrkAmbiguityScore(name = name, **kwargs)
	
	acc.addEventAlgo(InDetAmbiguityScore)
	return acc


def Trk__TrkAmbiguitySolverCfg(flags, name="InDetAmbiguitySolver", NewTrackingCuts = None, SiSPSeededTrackCollectionKey = None, **kwargs):
	acc = ComponentAccumulator()
	SiTrackCollection = SiSPSeededTrackCollectionKey

	if flags.InDet.doTIDE_Ambi and not (NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "DBM"):
		InDetAmbiguityProcessor = acc.popToolsAndMerge(Trk__DenseEnvironmentsAmbiguityProcessorToolCfg(flags, name = 'InDetAmbiguityProcessor'+ NewTrackingCuts.extension(), NewTrackingCuts = NewTrackingCuts))
		acc.addPublicTool(InDetAmbiguityProcessor)

		InDetAmbiguityScoreProcessor = acc.popToolsAndMerge(Trk__DenseEnvironmentsAmbiguityScoreProcessorToolCfg(flags, name = 'InDetAmbiguityScoreProcessor'+ NewTrackingCuts.extension(), NewTrackingCuts = NewTrackingCuts))
		acc.addPublicTool(InDetAmbiguityScoreProcessor)
	else:
		InDetAmbiguityProcessor = acc.popToolsAndMerge(Trk__SimpleAmbiguityProcessorToolCfg(flags, name = 'InDetAmbiguityProcessor'+NewTrackingCuts.extension(), NewTrackingCuts = NewTrackingCuts))
		acc.addPublicTool(InDetAmbiguityProcessor)

	#
	# --- configure Ambiguity solver
	#
	kwargs.setdefault("TrackInput"   , 'ScoredMap'+'InDetAmbiguityScore'+ NewTrackingCuts.extension())
	kwargs.setdefault("TrackOutput"       , SiTrackCollection)
	kwargs.setdefault( "AmbiguityProcessor", InDetAmbiguityProcessor)

	InDetAmbiguitySolver = CompFactory.Trk__TrkAmbiguitySolver(name = name, **kwargs)
	acc.addEventAlgo(InDetAmbiguitySolver )
	return acc

###########################################################################################################################################################

# ------------------------------------------------------------
#
# ----------- Setup Si Pattern for New tracking
#
# ------------------------------------------------------------
def  ConfiguredNewTrackingSiPatternCfg(flags, InputCollections = None, ResolvedTrackCollectionKey = None, SiSPSeededTrackCollectionKey = None , NewTrackingCuts = None, TrackCollectionKeys=[] , TrackCollectionTruthKeys=[]):
	acc = ComponentAccumulator()

	# ------------------------------------------------------------
	#
	# ----------- SiSPSeededTrackFinder
	#
	# ------------------------------------------------------------
	if flags.InDet.doSiSPSeededTrackFinder:
		InDetSiSpacePointsSeedMaker = acc.popToolsAndMerge(SiSpacePointsSeedMakerCfg(flags, name = "InDetSpSeedsMaker" + NewTrackingCuts.extension(), 
																							InputCollections = InputCollections, 
																							NewTrackingCuts = NewTrackingCuts ))
		acc.addPublicTool(InDetSiSpacePointsSeedMaker)
		# Condition algorithm for InDet__SiDetElementsRoadMaker_xk
		if flags.DetFlags.pixel_on:   ###FIX THIS!!
			acc.merge(InDet__SiDetElementBoundaryLinksCondAlg_xkCfg())
		if NewTrackingCuts.useSCT():
			acc.addCondAlgo(CompFactory.InDet__SiDetElementsRoadCondAlg_xk(name = "InDet__SiDetElementsRoadCondAlg_xk"))
			acc.addCondAlgo(CompFactory.InDet__SiDetElementBoundaryLinksCondAlg_xk(	name = "InDetSiDetElementBoundaryLinksSCTCondAlg",
																					ReadKey = "SCT_DetectorElementCollection",
																					WriteKey = "SCT_DetElementBoundaryLinks_xk"))
		
		acc.merge(InDet__SiSPSeededTrackFinderCfg(flags,	name = 'InDetSiSpTrackFinder'+ NewTrackingCuts.extension(), 
															InputCollections = InputCollections, 
															SiSPSeededTrackCollectionKey = SiSPSeededTrackCollectionKey, 
															NewTrackingCuts = NewTrackingCuts))
		# ------------------------------------------------------------
		#
		# ---------- Ambiguity solving
		#
		# ------------------------------------------------------------

		if flags.InDet.doAmbiSolving:
			#
			# --- load InnerDetector TrackSelectionTool
			#
			InDetAmbiTrackSelectionTool = acc.popToolsAndMerge(InDet__InDetAmbiTrackSelectionToolCfg(flags, name = 'InDetAmbiTrackSelectionTool'+ NewTrackingCuts.extension(), NewTrackingCuts = NewTrackingCuts ))
			acc.addPublicTool(InDetAmbiTrackSelectionTool)

			acc.merge(Trk__TrkAmbiguitySolverCfg(flags, name = 'InDetAmbiguitySolver'+ NewTrackingCuts.extension(), NewTrackingCuts = NewTrackingCuts, SiSPSeededTrackCollectionKey = SiSPSeededTrackCollectionKey))
			acc.merge(Trk__TrkAmbiguityScoreCfg(flags, name = 'InDetAmbiguityScore'+ NewTrackingCuts.extension(), NewTrackingCuts = NewTrackingCuts, SiSPSeededTrackCollectionKey = SiSPSeededTrackCollectionKey))

	return acc



if __name__ == "__main__":
	from AthenaCommon.Configurable import Configurable
	Configurable.configurableRun3Behavior=1

	from AthenaConfiguration.AllConfigFlags import ConfigFlags

	numThreads=1
	ConfigFlags.Concurrency.NumThreads=numThreads
	ConfigFlags.Concurrency.NumConcurrentEvents=numThreads # Might change this later, but good enough for the moment.

	ConfigFlags.Detector.GeometryPixel   = True 
	ConfigFlags.Detector.GeometrySCT   = True
	ConfigFlags.InDet.doPixelClusterSplitting = True

	from AthenaConfiguration.TestDefaults import defaultTestFiles
	ConfigFlags.Input.Files = defaultTestFiles.RDO
	ConfigFlags.lock()
	ConfigFlags.dump()

	from AthenaConfiguration.MainServicesConfig import MainServicesCfg
	top_acc = MainServicesCfg(ConfigFlags)

	msgService = top_acc.getService('MessageSvc')
	msgService.Format = "S:%s E:%e % F%138W%S%7W%R%T  %0W%M"

	from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
	from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
	top_acc.merge(PixelGeometryCfg(ConfigFlags))
	top_acc.merge(SCT_GeometryCfg(ConfigFlags))

	from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
	top_acc.merge(MagneticFieldSvcCfg(ConfigFlags))

	# track collection keys for statistics
	if 'TrackCollectionKeys' not in dir():
		TrackCollectionKeys = []
	if 'TrackCollectionTruthKeys' not in dir():
		TrackCollectionTruthKeys   = []

	from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
	InDetNewTrackingCutsPixel = ConfiguredNewTrackingCuts("Pixel")

	top_acc.merge(ConfiguredNewTrackingSiPatternCfg(ConfigFlags, 
													[],
													InDetKeys.PixelTracks,
													InDetKeys.SiSpSeededPixelTracks,
													InDetNewTrackingCutsPixel,
													TrackCollectionKeys,
													TrackCollectionTruthKeys))
	iovsvc = top_acc.getService('IOVDbSvc')
	iovsvc.OutputLevel=5
	top_acc.run(25)
	top_acc.store(open("test_TrackingSiPattern.pkl", "w"))


'''
def main(flags):
	from AthenaCommon.Configurable import Configurable
	Configurable.configurableRun3Behavior = 1
	from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg
	top_acc = MainServicesThreadedCfg(flags)

	from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
	top_acc.merge(PoolReadCfg(flags))

	from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
	from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
	top_acc.merge(PixelGeometryCfg(flags))
	top_acc.merge(SCT_GeometryCfg(flags))

	from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
	top_acc.merge(MagneticFieldSvcCfg(flags))

	# track collection keys for statistics
	if 'TrackCollectionKeys' not in dir():
		TrackCollectionKeys = []
	if 'TrackCollectionTruthKeys' not in dir():
		TrackCollectionTruthKeys   = []

	from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
	InDetNewTrackingCutsPixel = ConfiguredNewTrackingCuts("Pixel")

	top_acc.merge(ConfiguredNewTrackingSiPatternCfg(flags, 
													[],
													InDetKeys.PixelTracks,
													InDetKeys.SiSpSeededPixelTracks,
													InDetNewTrackingCutsPixel,
													TrackCollectionKeys,
													TrackCollectionTruthKeys))

	return top_acc


if __name__ == "__main__":
	import InDetConfigFlags
	flags = InDetConfigFlags.createInDetConfigFlags()

	acc = main(flags)
	acc.run(25)
	#acc.store(open("test00.pkl", "w"))
	#acc.printConfig()
'''