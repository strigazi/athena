# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

####################################################################################################################################

def setDefaults(kwargs, **def_kwargs) :
    def_kwargs.update(kwargs)
    return def_kwargs

####################################################################################################################################

def copyArgs(kwargs, copy_list) :
    dict_copy={}
    for elm in copy_list :
        if elm in kwargs :
            dict_copy[elm]=kwargs[elm]
    return dict_copy

####################################################################################################################################

def splitDefaultPrefix(name) :
    default_prefix=''
    for prefix in ['InDet','InDetTrig'] :
        if name[0:len(prefix)] == prefix :
            name=name[len(prefix):]
            default_prefix=prefix
            break
    return default_prefix,name

#####################################################################################################################################

def makeName(name, kwargs) :
    default_prefix,name=splitDefaultPrefix(name)
    namePrefix=kwargs.pop('namePrefix',default_prefix)
    nameSuffix=kwargs.pop('nameSuffix','')
    return namePrefix + name + nameSuffix

#####################################################################################################################################

def makeNameGetPreAndSuffix( name, kwargs) :
    default_prefix,name=splitDefaultPrefix(name)
    namePrefix=kwargs.pop('namePrefix',default_prefix)
    nameSuffix=kwargs.pop('nameSuffix','')
    return namePrefix + name + nameSuffix,namePrefix,nameSuffix

#####################################################################################################################################

def PixelClusterNnWithTrackCondAlgCfg(flags, **kwargs):
    kwargs.update(
        TrackNetwork =  True,
        name = 'PixelClusterNnWithTrackCondAlg')
    acc = PixelClusterNnCondAlgCfg(flags, **kwargs)
    return acc

#####################################################################################################################################
#@TODO maybe we need to write an analog of getNeuralNetworkToHistoTool   
#####################################################################################################################################

def PixelClusterNnCondAlgCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    track_nn = kwargs.pop('TrackNetwork',False)
    nn_names = [
          "NumberParticles_NoTrack/",
          "ImpactPoints1P_NoTrack/",
          "ImpactPoints2P_NoTrack/",
          "ImpactPoints3P_NoTrack/",
          "ImpactPointErrorsX1_NoTrack/",
          "ImpactPointErrorsX2_NoTrack/",
          "ImpactPointErrorsX3_NoTrack/",
          "ImpactPointErrorsY1_NoTrack/",
          "ImpactPointErrorsY2_NoTrack/",
          "ImpactPointErrorsY3_NoTrack/" ]

    if track_nn :
        nn_names = [ elm.replace('_NoTrack', '')  for elm in nn_names ]
    acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixelClustering/PixelClusNNCalib", "/PIXEL/PixelClustering/PixelClusNNCalib", className='CondAttrListCollection'))
#    acc.merge(addFolders(flags, "/PIXEL/PixelClustering/PixelClusNNCalib", "PIXEL", className='CondAttrListCollection'))
    kwargs.update(
        NetworkNames = nn_names,
        WriteKey     ='PixelClusterNN' if not track_nn else 'PixelClusterNNWithTrack')
    if 'NetworkToHistoTool' not in kwargs :
        Trk__NeuralNetworkToHistoTool = CompFactory.Trk__NeuralNetworkToHistoTool(name = "NeuralNetworkToHistoTool") ## here we don`t have kwargs as in source function
        kwargs.setdefault("NetworkToHistoTool", Trk__NeuralNetworkToHistoTool)
        acc.setPrivateTools([Trk__NeuralNetworkToHistoTool])
    acc.addCondAlgo(CompFactory.InDet__TTrainedNetworkCondAlg(kwargs.pop("name", 'PixelClusterNnCondAlg'), **kwargs))
    return acc    

#################################################################################################################################### 

def NnClusterizationFactoryToolCfg(flags, **kwargs):
    from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleToolCfg
    PixelLorentzAngleTool = PixelLorentzAngleToolCfg(flags, name="PixelLorentzAngleTool", **kwargs)
    do_runI = flags.geoFlags.Run not in ["RUN2", "RUN3"]
    if do_runI :
        NnClusterizationFactory = CompFactory.InDet__NnClusterizationFactory( name                               = "NnClusterizationFactory",
                                                                      **setDefaults(kwargs, 
                                                                      PixelLorentzAngleTool              = PixelLorentzAngleTool,
                                                                      doRunI                             = True,
                                                                      useToT                             = False,
                                                                      useRecenteringNNWithoutTracks      = True,
                                                                      useRecenteringNNWithTracks         = False,
                                                                      correctLorShiftBarrelWithoutTracks = 0,
                                                                      correctLorShiftBarrelWithTracks    = 0.030,
                                                                      NnCollectionReadKey                = 'PixelClusterNN',
                                                                      NnCollectionWithTrackReadKey       = 'PixelClusterNNWithTrack'))

    else:
        NnClusterizationFactory = CompFactory.InDet__NnClusterizationFactory( name                         = "NnClusterizationFactory",
                                                                      **setDefaults(kwargs, 
                                                                      PixelLorentzAngleTool        = PixelLorentzAngleTool,
                                                                      useToT                       = flags.InDetFlags.doNNToTCalibration,
                                                                      NnCollectionReadKey          = 'PixelClusterNN',
                                                                      NnCollectionWithTrackReadKey = 'PixelClusterNNWithTrack'))
    return NnClusterizationFactory

#################################################################################################################################### 

### return public tool
def NnClusterizationFactoryCfg(flags, **kwargs):
    acc = ComponentAccumulator()    
    privateTools = []
    privateTools += acc.popToolsAndMerge(PixelClusterNnCondAlgCfg(flags, **kwargs))
    privateTools += acc.popToolsAndMerge(PixelClusterNnWithTrackCondAlgCfg(flags, **kwargs))
    from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleToolCfg
    PixelLorentzAngleTool = PixelLorentzAngleToolCfg(flags, name="PixelLorentzAngleTool", **kwargs)
    acc.addPublicTool(NnClusterizationFactoryToolCfg(flags, **kwargs))
    privateTools += [PixelLorentzAngleTool]
    acc.setPrivateTools(privateTools)
    return acc

#####################################################################################################################################

# return tool to be public or public tool
def InDetPixelClusterOnTrackToolBaseCfg(flags, name, **kwargs):
    the_name = makeName(name, kwargs)
    split_cluster_map_extension = kwargs.pop('SplitClusterMapExtension','')
    ##from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__PixelClusterOnTrackTool
    if flags.InDetFlags.doCosmics or flags.InDetFlags.doDBMstandalone:
        kwargs = setDefaults(kwargs, ErrorStrategy    = 0,
                      PositionStrategy = 0)
    kwargs = setDefaults(kwargs, DisableDistortions       = (flags.InDetFlags.doFatras or flags.InDetFlags.doDBMstandalone),
                  applyNNcorrection        = ( flags.InDetFlags.doPixelClusterSplitting and
                                               flags.InDetFlags.pixelClusterSplittingType == 'NeuralNet' and not flags.InDetFlags.doSLHC),
                  NNIBLcorrection          = ( flags.InDetFlags.doPixelClusterSplitting and
                                               flags.InDetFlags.pixelClusterSplittingType == 'NeuralNet' and not flags.InDetFlags.doSLHC),
                  SplitClusterAmbiguityMap = flags.InDetKeys.SplitClusterAmbiguityMap + split_cluster_map_extension,
                  RunningTIDE_Ambi         = flags.InDetFlags.doTIDE_Ambi )
    return CompFactory.InDet__PixelClusterOnTrackTool(the_name, **kwargs)

####################################################################################################################################

#return configured InDetPixelClusterOnTrackToolBaseCfg
def InDetPixelClusterOnTrackToolDBMCfg(flags, name='InDetPixelClusterOnTrackToolDBM', **kwargs):
    kwargs = setDefaults(kwargs, DisableDistortions = True,
                  applyNNcorrection  = False,
                  NNIBLcorrection    = False,
                  RunningTIDE_Ambi   = False,
                  ErrorStrategy      = 0,
                  PositionStrategy   = 0)
    return InDetPixelClusterOnTrackToolBaseCfg(flags, name=name, **kwargs)

#################################################################################################################################### 

#return configured InDetPixelClusterOnTrackToolBaseCfg
def InDetPixelClusterOnTrackToolDigitalCfg(flags, name='InDetPixelClusterOnTrackToolDigital', **kwargs) :
    if 'LorentzAngleTool' not in kwargs :
        from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleToolCfg
        kwargs = setDefaults(kwargs, LorentzAngleTool = PixelLorentzAngleToolCfg(flags, name="PixelLorentzAngleTool"))

    if flags.InDetFlags.doDigitalROTCreation:
        kwargs = setDefaults(kwargs, applyNNcorrection = False,
                      NNIBLcorrection   = False,
                      ErrorStrategy     = 2,
                      PositionStrategy  = 1,
                      SplitClusterAmbiguityMap = "")
    else :
        kwargs = setDefaults(kwargs, SplitClusterAmbiguityMap = "")
    return InDetPixelClusterOnTrackToolBaseCfg(flags, name=name, **kwargs)

#################################################################################################################################### 

#return configured InDetPixelClusterOnTrackToolBaseCfg
def InDetPixelClusterOnTrackToolNNSplittingCfg(flags, name='InDetPixelClusterOnTrackToolNNSplitting', **kwargs):
    if flags.InDetFlags.doPixelClusterSplitting and flags.InDetFlags.pixelClusterSplittingType == 'NeuralNet':
        if 'NnClusterizationFactory' not in kwargs :
            kwargs = setDefaults(kwargs, NnClusterizationFactory = NnClusterizationFactoryToolCfg(flags))

        if flags.InDetFlags.doTIDE_RescalePixelCovariances:
            kwargs = setDefaults(kwargs, applydRcorrection = True)
    return InDetPixelClusterOnTrackToolBaseCfg(flags, name=name, **kwargs)

#################################################################################################################################### 

#return configured InDetPixelClusterOnTrackToolBaseCfg
def InDetPixelClusterOnTrackToolCfg(flags, name='InDetPixelClusterOnTrackTool', **kwargs) :
    if 'LorentzAngleTool' not in kwargs :
        from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleToolCfg
        kwargs = setDefaults(kwargs, LorentzAngleTool = PixelLorentzAngleToolCfg(flags, name="PixelLorentzAngleTool"))
    if flags.InDetFlags.doDigitalROTCreation:
        return InDetPixelClusterOnTrackToolDigitalCfg(flags, name=name, **kwargs)
    else:
        return InDetPixelClusterOnTrackToolNNSplittingCfg(flags, name=name, **kwargs)

#################################################################################################################################### 

#return configured InDetPixelClusterOnTrackToolBaseCfg
def InDetPixelClusterOnTrackToolPatternCfg(flags, name='InDetPixelClusterOnTrackToolPattern', **kwargs) :
    return InDetPixelClusterOnTrackToolCfg(flags, name=name, **kwargs)

####################################################################################################################################

def InDetSCT_ClusterOnTrackToolCfg(flags, name='InDetSCT_ClusterOnTrackTool', **kwargs) :
    the_name = makeName(name, kwargs)
    if 'LorentzAngleTool' not in kwargs :
        from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleToolCfg
        kwargs = setDefaults(kwargs, LorentzAngleTool = PixelLorentzAngleToolCfg(flags, name="PixelLorentzAngleTool"))

    kwargs = setDefaults(kwargs, # CorrectionStrategy = -1,  # no position correction (test for bug #56477)
                  CorrectionStrategy = 0,  # do correct position bias
                  ErrorStrategy      = 2  # do use phi dependent errors
                  )
    return CompFactory.InDet__SCT_ClusterOnTrackTool(the_name, **kwargs)

#################################################################################################################################### 

def InDetBroadPixelClusterOnTrackToolCfg(flags, name='InDetBroadPixelClusterOnTrackTool', **kwargs) :
    kwargs = setDefaults(kwargs, ErrorStrategy = 0)
    return InDetPixelClusterOnTrackToolCfg(flags, name=name, **kwargs)

#################################################################################################################################### 

def InDetBroadSCT_ClusterOnTrackToolCfg(flags, name='InDetBroadSCT_ClusterOnTrackTool', **kwargs):
    kwargs = setDefaults(kwargs, ErrorStrategy = 0)
    return InDetSCT_ClusterOnTrackToolCfg(flags, name=name, **kwargs)

#################################################################################################################################### 

def RIO_OnTrackErrorScalingCondAlgCfg(flags, **kwargs) :
    the_name=kwargs.pop("name",None)
    acc = ComponentAccumulator()
    if flags.geoFlags.isIBL:
        error_scaling_type =   ["PixelRIO_OnTrackErrorScaling"]
        error_scaling_outkey = ["/Indet/TrkErrorScalingPixel"]
    else:
        error_scaling_type =   ["PixelRIO_OnTrackErrorScalingRun1"]
        error_scaling_outkey = ["/Indet/TrkErrorScalingPixel"]

    error_scaling_type +=      ["SCTRIO_OnTrackErrorScaling"]
    error_scaling_outkey +=    ["/Indet/TrkErrorScalingSCT"]

    error_scaling_type +=      ["TRTRIO_OnTrackErrorScaling"]
    error_scaling_outkey +=    ["/Indet/TrkErrorScalingTRT"]

    acc.merge(addFoldersSplitOnline(flags, 'INDET','/Indet/Onl/TrkErrorScaling','/Indet/TrkErrorScaling', className="CondAttrListCollection"))

    ##from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import RIO_OnTrackErrorScalingCondAlg
    kwargs = setDefaults(kwargs, ReadKey             = "/Indet/TrkErrorScaling",
                  ErrorScalingType    = error_scaling_type,
                  OutKeys             = error_scaling_outkey)
    if not (the_name is None):
        kwargs.setdefault("name", the_name)
    acc.addCondAlgo(CompFactory.RIO_OnTrackErrorScalingCondAlg(**kwargs))
    return acc

#################################################################################################################################### 

def InDetTRT_DriftCircleOnTrackToolCfg(flags, name='TRT_DriftCircleOnTrackTool', **kwargs) :
    the_name = makeName( name, kwargs)
    acc = ComponentAccumulator()
    acc.merge(RIO_OnTrackErrorScalingCondAlgCfg(flags, name = 'RIO_OnTrackErrorScalingCondAlg'))
    # @TODO create LuminosityCondAlg
    kwargs = setDefaults(kwargs, TRTErrorScalingKey = '/Indet/TrkErrorScalingTRT'
                  # LumiDataKey        = 'LuminosityCondData'        # @TODO undo out-commenting to re-enable mu-correction for TRT error scaling
                  )
    acc.addPublicTool(CompFactory.InDet__TRT_DriftCircleOnTrackTool(name = the_name, **kwargs), primary = True)
    return acc

#################################################################################################################################### 

def InDetBroadTRT_DriftCircleOnTrackToolCfg(flags, name='InDetBroadTRT_DriftCircleOnTrackTool', **kwargs) :
    the_name = makeName( name, kwargs)
    return CompFactory.InDet__TRT_DriftCircleOnTrackNoDriftTimeTool(the_name)

#################################################################################################################################### 

def InDetRotCreatorCfg(flags, name='InDetRotCreator', **kwargs) :
    acc = ComponentAccumulator()
    the_name = makeName(name, kwargs)
    split_cluster_map_extension = kwargs.pop('SplitClusterMapExtension','')
    use_broad_cluster_pix = flags.InDetFlags.useBroadPixClusterErrors and (not flags.InDetFlags.doDBMstandalone)
    use_broad_cluster_sct = flags.InDetFlags.useBroadSCTClusterErrors and (not flags.InDetFlags.doDBMstandalone)
#    privateTools = []
    if 'ToolPixelCluster' not in kwargs :
        if use_broad_cluster_pix :
            ToolPixelCluster= InDetBroadPixelClusterOnTrackToolCfg(flags, nameSuffix               = split_cluster_map_extension,
                                                                                                 SplitClusterMapExtension = split_cluster_map_extension) 
        else:
            ToolPixelCluster= InDetPixelClusterOnTrackToolCfg(flags, nameSuffix               = split_cluster_map_extension,
                                                                                            SplitClusterMapExtension = split_cluster_map_extension)
        acc.addPublicTool(ToolPixelCluster)
        kwargs.setdefault("ToolPixelCluster", ToolPixelCluster)  
  
    if 'ToolSCT_Cluster' not in kwargs :
        if use_broad_cluster_sct :
            ToolSCT_Cluster = InDetBroadSCT_ClusterOnTrackToolCfg(flags)
        else :
            ToolSCT_Cluster = InDetSCT_ClusterOnTrackToolCfg(flags)
        kwargs.setdefault("ToolSCT_Cluster", ToolSCT_Cluster)
        acc.addPublicTool(ToolSCT_Cluster)

    if 'ToolTRT_DriftCircle' not in kwargs :
        acc_trt = InDetTRT_DriftCircleOnTrackToolCfg(flags)
        TRT_DriftCircleOnTrackTool = acc_trt.getPrimary()
        acc.merge(acc_trt)
        kwargs.setdefault("ToolTRT_DriftCircle", TRT_DriftCircleOnTrackTool)


    kwargs = setDefaults(kwargs, Mode = 'indet')
    acc.addPublicTool(CompFactory.Trk__RIO_OnTrackCreator(name=the_name, **kwargs), primary = True)
    return acc

#################################################################################################################################### 

def InDetRotCreatorPatternCfg(flags, name='InDetRotCreatorPattern', **kwargs) :
    if 'ToolPixelCluster' not in kwargs :
        split_cluster_map_extension = kwargs.get('SplitClusterMapExtension','')
        ToolPixelCluster = InDetPixelClusterOnTrackToolPatternCfg(flags, nameSuffix               = split_cluster_map_extension,
                                                                                       SplitClusterMapExtension = split_cluster_map_extension)
        kwargs.setdefault('ToolPixelCluster', ToolPixelCluster)
        acc = ComponentAccumulator()
        acc.addPublicTool(ToolPixelCluster)
        acc.merge(InDetRotCreatorCfg(flags, name=name, **kwargs))
        return acc
    return InDetRotCreatorCfg(flags, name=name, **kwargs)

####################################################################################################################################

def InDetRotCreatorDBMCfg(flags, name='InDetRotCreatorDBM', **kwargs) :
    split_cluster_map_extension = kwargs.pop('SplitClusterMapExtension','')
    if 'ToolPixelCluster' not in kwargs :
        if flags.InDetFlags.loadRotCreator and flags.DetFlags.pixel_on:
            ToolPixelCluster = InDetPixelClusterOnTrackToolDBMCfg(flags, nameSuffix               = split_cluster_map_extension,
                                                                                       SplitClusterMapExtension = split_cluster_map_extension)
            kwargs.setdefault('ToolPixelCluster', ToolPixelCluster)
        else :
            ToolPixelCluster = InDetPixelClusterOnTrackToolCfg(flags, nameSuffix               = split_cluster_map_extension,
                                                                                    SplitClusterMapExtension = split_cluster_map_extension)
            kwargs.setdefault('ToolPixelCluster', ToolPixelCluster)
        acc = ComponentAccumulator()
        acc.addPublicTool(ToolPixelCluster)
        acc.merge(InDetRotCreatorCfg(flags, name=name, **kwargs))
        return acc
    return InDetRotCreatorCfg(flags, name=name, **kwargs)

####################################################################################################################################

def InDetRotCreatorDigitalCfg(flags, name='InDetRotCreatorDigital', **kwargs) :
    if 'ToolPixelCluster' not in kwargs :
        split_cluster_map_extension = kwargs.get('SplitClusterMapExtension','')
        ToolPixelCluster = InDetPixelClusterOnTrackToolDigitalCfg(flags, nameSuffix               = split_cluster_map_extension,
                                                                                       SplitClusterMapExtension = split_cluster_map_extension)
        kwargs.setdefault('ToolPixelCluster', ToolPixelCluster)
        acc = ComponentAccumulator()
        acc.addPublicTool(ToolPixelCluster)
        acc.merge(InDetRotCreatorCfg(flags, name=name, **kwargs))
        return acc
    return InDetRotCreatorCfg(flags, name=name, **kwargs)

####################################################################################################################################

def InDetBroadRotCreatorCfg(flags, name='InDetBroadInDetRotCreator', **kwargs) :
    acc = ComponentAccumulator()
    if 'ToolPixelCluster' not in kwargs :
        split_cluster_map_extension = kwargs.get('SplitClusterMapExtension','')
        ToolPixelCluster = InDetBroadPixelClusterOnTrackToolCfg(flags, nameSuffix               = split_cluster_map_extension,
                                                                                        SplitClusterMapExtension = split_cluster_map_extension)
        kwargs.setdefault("ToolPixelCluster", ToolPixelCluster)
        acc.addPublicTool(ToolPixelCluster)
    if 'ToolSCT_Cluster' not in kwargs :
        ToolSCT_Cluster = InDetBroadSCT_ClusterOnTrackToolCfg(flags)
        kwargs.setdefault("ToolSCT_Cluster", ToolSCT_Cluster)

    if flags.DetFlags.TRT_on:
        if 'ToolTRT_DriftCircle' not in kwargs :
            ToolTRT_DriftCircle = InDetBroadTRT_DriftCircleOnTrackToolCfg(flags)
            kwargs.setdefault("ToolTRT_DriftCircle", ToolTRT_DriftCircle)
            acc.addPublicTool(ToolTRT_DriftCircle)
    
    acc.merge(InDetRotCreatorCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(ToolSCT_Cluster)
    return acc

####################################################################################################################################

default_ScaleHitUncertainty = 2.5
#all tools should be public
def InDetTRT_DriftCircleOnTrackUniversalToolCfg(flags, name='InDetTRT_RefitRotCreator',**kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    if 'RIOonTrackToolDrift' not in kwargs :
        RIOonTrackToolDrift = InDetBroadTRT_DriftCircleOnTrackToolCfg(flags)
        kwargs.setdefault("RIOonTrackToolDrift", RIOonTrackToolDrift)
        acc.addPublicTool(RIOonTrackToolDrift)
    if 'RIOonTrackToolTube' not in kwargs :
        RIOonTrackToolTube = InDetBroadTRT_DriftCircleOnTrackToolCfg(flags)
        kwargs.setdefault("RIOonTrackToolTube", RIOonTrackToolTube)
        acc.addPublicTool(RIOonTrackToolTube)
    kwargs = setDefaults(kwargs, ScaleHitUncertainty = default_ScaleHitUncertainty) 
    acc.setPrivateTools(CompFactory.InDet__TRT_DriftCircleOnTrackUniversalTool(name = the_name, **kwargs))
    return acc

####################################################################################################################################

def InDetRefitRotCreatorCfg(flags, name='InDetRefitRotCreator', **kwargs) :
    ScaleHitUncertainty = kwargs.pop('ScaleHitUncertainty',default_ScaleHitUncertainty)
    if flags.InDetFlags.redoTRT_LR:
        if flags.DetFlags.TRT_on:
            if 'ToolTRT_DriftCircle' not in kwargs :
                acc = ComponentAccumulator()
                ToolTRT_DriftCircle = acc.popToolsAndMerge(InDetTRT_DriftCircleOnTrackUniversalToolCfg(flags, ScaleHitUncertainty = ScaleHitUncertainty))
                kwargs.setdefault("ToolTRT_DriftCircle", ToolTRT_DriftCircle)
                acc.addPublicTool(ToolTRT_DriftCircle)
                acc.merge(InDetRotCreatorCfg(flags, name = name, **kwargs))
                return acc
    return InDetRotCreatorCfg(flags, name = name, **kwargs)

####################################################################################################################################

#used in ConfiguredTRTSegmentFindingCfg
# that function returns tool
def InDetUpdatorCfg(flags, name = 'InDetUpdator', **kwargs) :
    the_name = makeName( name, kwargs )
    if flags.InDetFlags.kalmanUpdator == "fast" :
        return CompFactory.Trk__KalmanUpdator_xk(name = the_name, **kwargs)
    elif flags.InDetFlags.kalmanUpdator == "weight" :
        return CompFactory.Trk__KalmanWeightUpdator(name = the_name, **kwargs)
    elif flags.InDetFlags.kalmanUpdator == "smatrix" :
        return CompFactory.Trk__KalmanUpdatorSMatrix(name = the_name, **kwargs)
    elif flags.InDetFlags.kalmanUpdator == "amg" :
        return CompFactory.Trk__KalmanUpdatorAmg(name = the_name, **kwargs)
    else :
        return CompFactory.Trk__KalmanUpdator(name = the_name, **kwargs)

####################################################################################################################################

#used in ConfiguredTRTSegmentFindingCfg
# that function returns tool
def InDetPropagatorCfg(flags, name='InDetPropagator',**kwargs) :
    the_name = makeName( name, kwargs)
    if flags.InDetFlags.propagatorType == "STEP":
        Propagator = CompFactory.Trk__STEP_Propagator
    else:
        Propagator = CompFactory.Trk__RungeKuttaPropagator

    if flags.InDetFlags.propagatorType == "RungeKutta":
        kwargs = setDefaults(kwargs, AccuracyParameter = 0.0001,
                      MaxStraightLineStep = .004 ) # Fixes a failed fit
    return Propagator(name = the_name, **kwargs)

####################################################################################################################################

# that function returns CA with tool and Svc
def InDetNavigatorCfg(flags, name='InDetNavigator', **kwargs) :
    the_name = makeName( name, kwargs)
    acc = ComponentAccumulator()
    if 'TrackingGeometrySvc' not in kwargs :
        from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
        acc.addService(AtlasTrackingGeometrySvc, primary = True) # add service from old style config
        kwargs.setdefault('TrackingGeometrySvc', acc.getPrimary())

    acc.setPrivateTools(CompFactory.Trk__Navigator(name=the_name,**kwargs ))
    return acc

####################################################################################################################################

#used in ConfiguredTRTSegmentFindingCfg
# that function returns tool
def InDetMaterialEffectsUpdatorCfg(flags, name = "InDetMaterialEffectsUpdator", **kwargs) :
    the_name = makeName( name, kwargs)
    if not flags.InDetFlags.solenoidOn:
        import AthenaCommon.SystemOfUnits as Units
        kwargs = setDefaults(kwargs, EnergyLoss          = False,
                      ForceMomentum       = True,
                      ForcedMomentumValue = 1000*Units.MeV )
    return CompFactory.Trk__MaterialEffectsUpdator(name = the_name, **kwargs)

####################################################################################################################################

#used in ConfiguredTRTSegmentFindingCfg
# that function returns CA with private tool and public tools
def InDetExtrapolatorCfg(flags, name='InDetExtrapolator', **kwargs) :
    the_name = makeName( name, kwargs)
    acc = ComponentAccumulator()
    if 'Propagators' not in kwargs :
        acc.addPublicTool(InDetPropagatorCfg(flags), primary = True)
        kwargs.setdefault('Propagators', [ acc.getPrimary() ] ) # [ InDetPropagator, InDetStepPropagator ],
    propagator= kwargs.get('Propagators')[0].name() if  kwargs.get('Propagators',None) is not None and len(kwargs.get('Propagators',None))>0 else None

    if 'MaterialEffectsUpdators' not in kwargs :
        acc.addPublicTool(InDetMaterialEffectsUpdatorCfg(flags), primary = True)
        kwargs.setdefault('MaterialEffectsUpdators', [ acc.getPrimary() ] )
    material_updator= kwargs.get('MaterialEffectsUpdators')[0].name() if  kwargs.get('MaterialEffectsUpdators',None) is not None and len(kwargs.get('MaterialEffectsUpdators',None))>0  else None

    if 'Navigator' not in kwargs :
        tool = acc.popToolsAndMerge(InDetNavigatorCfg(flags))
        acc.addPublicTool(tool, primary = True)
        kwargs.setdefault('Navigator', acc.getPrimary())

    sub_propagators = []
    sub_updators    = []

    # -------------------- set it depending on the geometry ----------------------------------------------------
    # default for ID is (Rk,Mat)
    sub_propagators += [ propagator ]
    sub_updators    += [ material_updator ]

    # default for Calo is (Rk,MatLandau)
    sub_propagators += [ propagator ]
    sub_updators    += [ material_updator ]

    # default for MS is (STEP,Mat)
    #  sub_propagators += [ InDetStepPropagator.name() ]
    sub_updators    += [ material_updator ]
    # @TODO should check that all sub_propagators and sub_updators are actually defined.

    kwargs = setDefaults(kwargs, SubPropagators          = sub_propagators,
                  SubMEUpdators           = sub_updators)

    acc.setPrivateTools(CompFactory.Trk__Extrapolator(name = the_name, **kwargs))
    return acc

####################################################################################################################################
##### Fitter part begins ###########################################################################################################
####################################################################################################################################

#def InDetKalmanTrackFitterBase(name='InDetKalmanTrackFitterBase',**kwargs) :
#    from InDetRecExample import TrackingCommon as TrackingCommon
#    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanFitter
#    from AthenaCommon.AppMgr import ToolSvc
#    split_cluster_map_extension = kwargs.get('SplitClusterMapExtension','')#
#
#    kwargs.setdefault('ExtrapolatorHandle', TrackingCommon.getInDetExtrapolator())
#    if 'RIO_OnTrackCreatorHandle' not in kwargs :
#        from InDetRecExample import TrackingCommon as TrackingCommon
#        kwargs=setDefaults(kwargs,
#                           RIO_OnTrackCreatorHandle = TrackingCommon.getInDetRefitRotCreator(
#                               SplitClusterMapExtension = split_cluster_map_extension))
#
#    kwargs.setdefault('MeasurementUpdatorHandle', ToolSvc.InDetUpdator)
#    setTool('KalmanSmootherHandle', 'InDetBKS', kwargs )
#    setTool('KalmanOutlierLogicHandle', 'InDetKOL',kwargs )
#    kwargs.setdefault('DynamicNoiseAdjustorHandle', None)
#    kwargs.setdefault('BrempointAnalyserHandle', None)
#    kwargs.setdefault('AlignableSurfaceProviderHandle',None)
#    if len(split_cluster_map_extension)>0 :
#        if 'RecalibratorHandle' not in kwargs :
#            the_tool_name = 'InDetMeasRecalibST'
#            kwargs.setdefault('RecalibratorHandle', CfgGetter.getPublicToolClone(the_tool_name+split_cluster_map_extension,the_tool_name, SplitClusterMapExtension = split_cluster_map_extension) )
#    else :
#        setTool('RecalibratorHandle', 'InDetMeasRecalibST', kwargs )
#    # setTool('InternalDAFHandle','KalmanInternalDAF',kwargs )
#    # from InDetRecExample.InDetJobProperties import InDetFlags
#    # kwargs.setdefault('DoDNAForElectronsOnly', True if InDetFlags.doBremRecovery() and InDetFlags.trackFitterType() is 'KalmanFitter' else False)
#    return Trk__KalmanFitter(name,**kwargs)


####################################################################################################################################
##### Fitter part ends #############################################################################################################
####################################################################################################################################

#used in ConfiguredTRTSegmentFindingCfg
def InDetPRDtoTrackMapToolGangedPixelsCfg(flags, name='PRDtoTrackMapToolGangedPixels',**kwargs) :
    the_name = makeName( name, kwargs)
    kwargs = setDefaults( kwargs,
                          PixelClusterAmbiguitiesMapName = flags.InDetKeys.GangedPixelMap,
                          addTRToutliers                 = True)
    return CompFactory.InDet__InDetPRDtoTrackMapToolGangedPixels( name=the_name, **kwargs)

####################################################################################################################################

#used in ConfiguredTRTSegmentFindingCfg
def InDetTrackPRD_AssociationCfg(flags, name='InDetTrackPRD_Association', **kwargs) :
    the_name,prefix,suffix=makeNameGetPreAndSuffix(name,kwargs)

    if kwargs.get('TracksName',None) is None :
        raise Exception('Not TracksName argument provided')

    kwargs = setDefaults( kwargs,
                          AssociationTool    = InDetPRDtoTrackMapToolGangedPixelsCfg(flags) \
                                                    if 'AssociationTool' not in kwargs else None,
                          AssociationMapName = prefix+'PRDtoTrackMap'+suffix)
    acc.addEventAlgo(CompFactory.InDet__InDetTrackPRD_Association(name = the_name, **kwargs))
    return acc

####################################################################################################################################

#used in ConfiguredTRTSegmentFindingCfg
def InDetTRT_DriftCircleOnTrackUniversalToolCosmicsCfg(flags, name='TRT_DriftCircleOnTrackUniversalTool',**kwargs) :
    return InDetTRT_DriftCircleOnTrackUniversalToolCfg(flags, name=name,ScaleHitUncertainty=2.)

####################################################################################################################################

#used in ConfiguredTRTSegmentFindingCfg
#very strange functions (looks useless)
def InDetTRT_DriftCircleOnTrackNoDriftTimeToolCfg(flags, **kwargs) :
    return InDetBroadTRT_DriftCircleOnTrackToolCfg(flags, **kwargs)

####################################################################################################################################

#used in ConfiguredTRTSegmentFindingCfg
#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetTRT_ExtensionToolCosmicsCfg(flags, name='InDetTRT_ExtensionToolCosmics',**kwargs) :
    the_name = makeName( name, kwargs)
    acc = ComponentAccumulator()

    if 'Propagator' not in kwargs :
        InDetPropagator = InDetPropagatorCfg(flags)
        acc.addPublicTool(InDetPropagator)
        kwargs=setDefaults(kwargs, Propagator = InDetPropagator)

    if 'Extrapolator' not in kwargs :
        InDetExtrapolator = acc.popToolsAndMerge(InDetExtrapolatorCfg(flags))
        acc.addPublicTool(InDetExtrapolator)
        kwargs=setDefaults(kwargs, Extrapolator        = InDetExtrapolator)

    if 'RIOonTrackToolYesDr' not in kwargs :
        InDetTRT_DriftCircleOnTrackUniversalToolCosmics = acc.popToolsAndMerge(InDetTRT_DriftCircleOnTrackUniversalToolCosmicsCfg(flags))
        acc.addPublicTool(InDetTRT_DriftCircleOnTrackUniversalToolCosmics)
        kwargs=setDefaults(kwargs, RIOonTrackToolYesDr = InDetTRT_DriftCircleOnTrackUniversalToolCosmics)

    if 'RIOonTrackToolNoDr' not in kwargs :
        InDetBroadTRT_DriftCircleOnTrackTool = InDetTRT_DriftCircleOnTrackNoDriftTimeToolCfg(flags)
        acc.addPublicTool(InDetBroadTRT_DriftCircleOnTrackTool)
        kwargs=setDefaults(kwargs, RIOonTrackToolNoDr  = InDetBroadTRT_DriftCircleOnTrackTool)

    kwargs = setDefaults(	kwargs,
                            TRT_ClustersContainer = flags.InDetKeys.TRT_DriftCircles,
                            SearchNeighbour       = False,  # needs debugging!!!
                            RoadWidth             = 10.)
    acc.setPrivateTools(CompFactory.InDet__TRT_TrackExtensionToolCosmics(name = the_name, **kwargs))
    return acc
####################################################################################################################################
#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetTRT_ExtensionToolPhaseCfg(flags, name='InDetTRT_ExtensionToolPhase', **kwargs) :
    acc = ComponentAccumulator()
    InDetTRT_DriftCircleOnTrackUniversalTool = acc.popToolsAndMerge(InDetTRT_DriftCircleOnTrackUniversalToolCfg(flags))
    acc.addPublicTool(InDetTRT_DriftCircleOnTrackUniversalTool)
    if 'RIOonTrackToolYesDr' not in kwargs :
        kwargs=setDefaults(kwargs, RIOonTrackToolYesDr   = InDetTRT_DriftCircleOnTrackUniversalTool)

    acc.setPrivateTools(acc.popToolsAndMerge(InDetTRT_ExtensionToolCosmicsCfg(flags, name = name, **setDefaults(kwargs,
                                                                                                    TRT_ClustersContainer = flags.InDetKeys.TRT_DriftCirclesUncalibrated,
                                                                                                    RoadWidth             = 20.))))
    return acc

####################################################################################################################################

## that is preliminary version (should be fixed or taken from https://gitlab.cern.ch/atlas/athena/blob/master/InnerDetector/InDetExample/InDetRecExample/python/ConfiguredNewTrackingCuts.py) 
#used in ConfiguredTRTSegmentFindingCfg
def InDetNewTrackingCutsCfg(flags) :
    from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
    from InDetRecExample.InDetJobProperties import InDetFlags
    if InDetFlags.doDBMstandalone():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("DBM")
    elif InDetFlags.doVtxLumi():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("VtxLumi")
    elif InDetFlags.doVtxBeamSpot():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("VtxBeamSpot")
    elif InDetFlags.doCosmics():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("Cosmics")
    elif InDetFlags.doHeavyIon():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("HeavyIon")
    elif InDetFlags.doSLHC():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("SLHC")
    elif InDetFlags.doIBL():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("IBL")
    elif InDetFlags.doHighPileup():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("HighPileup")
    elif InDetFlags.doMinBias():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("MinBias")
    elif InDetFlags.doDVRetracking():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("LargeD0")
    else:
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("Offline")
    return InDetNewTrackingCuts
    # --- backward compatible

####################################################################################################################################

#used in ConfiguredTRTSegmentFindingCfg
#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetPatternPropagatorCfg(flags, name='InDetPatternPropagator',**kwargs) :
    the_name = makeName( name, kwargs)
    return CompFactory.Trk__RungeKuttaPropagator(name = the_name, **kwargs)

####################################################################################################################################

#used in ConfiguredTRTSegmentFindingCfg
def InDetPatternUpdatorCfg(flags, name='InDetPatternUpdator',**kwargs) :
    the_name = makeName(name, kwargs)
    return CompFactory.Trk__KalmanUpdator_xk(name = the_name, **kwargs)

####################################################################################################################################

#used in ConfiguredTRTSegmentFindingCfg
#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetTRTDriftCircleCutForPatternRecoCfg(flags, name='InDetTRTDriftCircleCutForPatternReco', TrackingCuts=None, **kwargs) :
    the_name = makeName( name, kwargs)

    InDetNewTrackingCuts = TrackingCuts if TrackingCuts is not None  else InDetNewTrackingCutsCfg(flags)
    kwargs=setDefaults(kwargs,
                       MinOffsetDCs           = 5,
                       UseNewParameterization = InDetNewTrackingCuts.useNewParameterizationTRT(),  # Use new parameterization only for high lumi
                       UseActiveFractionSvc   = flags.DetFlags.TRT_on)

    return CompFactory.InDet__InDetTrtDriftCircleCutTool(the_name, **kwargs)

####################################################################################################################################

#used in ConfiguredTRTSegmentFindingCfg
#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetTRT_RoadMakerCfg(flags, name='InDetTRT_RoadMaker',**kwargs) :
    the_name = makeName( name, kwargs)
    acc = ComponentAccumulator()
    InDetPatternPropagator = InDetPatternPropagatorCfg(flags)
    acc.addPublicTool(InDetPatternPropagator)
    kwargs=setDefaults(kwargs,
                        RoadWidth      = 20.,
                        PropagatorTool = InDetPatternPropagator)
    acc.setPrivateTools(CompFactory.InDet__TRT_DetElementsRoadMaker_xk(the_name, **kwargs))
    return acc

####################################################################################################################################

#used in ConfiguredTRTSegmentFindingCfg
#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetTRT_TrackExtensionTool_xkCfg(flags, name='InDetTRT_ExtensionTool', TrackingCuts=None, **kwargs) :
    the_name = makeName( name, kwargs)
    acc = ComponentAccumulator()
    InDetNewTrackingCuts = TrackingCuts if TrackingCuts is not None else InDetNewTrackingCutsCfg(flags)
    if 'PropagatorTool' not in kwargs :
        InDetPatternPropagator = InDetPatternPropagatorCfg(flags)
        acc.addPublicTool(InDetPatternPropagator)
        kwargs=setDefaults(kwargs, PropagatorTool = InDetPatternPropagator)

    if 'UpdatorTool' not in kwargs :
        InDetPatternUpdator = InDetPatternUpdatorCfg(flags)
        acc.addPublicTool(InDetPatternUpdator)
        kwargs=setDefaults(kwargs, UpdatorTool = InDetPatternUpdator)

    if 'DriftCircleCutTool' not in kwargs :
        InDetTRTDriftCircleCutForPatternReco = InDetTRTDriftCircleCutForPatternRecoCfg(flags, TrackingCuts=InDetNewTrackingCutsCfg(flags))
        acc.addPublicTool(InDetTRTDriftCircleCutForPatternReco)
        kwargs=setDefaults(kwargs,
                            DriftCircleCutTool = InDetTRTDriftCircleCutForPatternReco)

    if 'RIOonTrackToolYesDr' not in kwargs :
        from TrackingCommonConfig import InDetTRT_DriftCircleOnTrackToolCfg
        acc_tmp = InDetTRT_DriftCircleOnTrackToolCfg(flags)
        InDetTRT_DriftCircleOnTrackTool = acc_tmp.getPrimary()
        acc.merge(acc_tmp)
        kwargs=setDefaults(kwargs, RIOonTrackToolYesDr = InDetTRT_DriftCircleOnTrackTool)

    if 'RoadTool' not in kwargs :
        InDetTRT_RoadMaker = acc.popToolsAndMerge(InDetTRT_RoadMakerCfg(flags))
        acc.addPublicTool(InDetTRT_RoadMaker)
        kwargs=setDefaults(kwargs, RoadTool = InDetTRT_RoadMaker)

    kwargs=setDefaults(kwargs,
                        TRT_ClustersContainer = flags.InDetKeys.TRT_DriftCircles,
                        TrtManagerLocation    = flags.InDetKeys.TRT_Manager,
                        UseDriftRadius        = not flags.InDetFlags.noTRTTiming,
                        MinNumberDriftCircles = InDetNewTrackingCuts.minTRTonTrk(),
                        ScaleHitUncertainty   = 2.,
                        RoadWidth             = 20.,
                        UseParameterization   = InDetNewTrackingCuts.useParameterizedTRTCuts(),
                        maxImpactParameter    = 500 if flags.InDetFlags.doBeamHalo or flags.InDetFlags.doBeamGas else 50) # single beam running, open cuts

    acc.setPrivateTools(CompFactory.InDet__TRT_TrackExtensionTool_xk(the_name, **kwargs))
    return acc

####################################################################################################################################

#used in ConfiguredTRTSegmentFindingCfg
#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetWeightCalculatorCfg(flags, name='InDetWeightCalculator',**kwargs) :
    the_name = makeName( name, kwargs)
    return CompFactory.Trk__DAF_SimpleWeightCalculator(name = the_name, **kwargs)

####################################################################################################################################

#used in ConfiguredTRTSegmentFindingCfg
#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetCompetingTRT_DC_ToolCfg(flags, name='InDetCompetingTRT_DC_Tool',**kwargs) :
    the_name = makeName( name, kwargs)
    acc = ComponentAccumulator()
    if 'Extrapolator' not in kwargs :
        InDetExtrapolator = acc.popToolsAndMerge(InDetExtrapolatorCfg(flags))
        acc.addPublicTool(InDetExtrapolator)
        kwargs=setDefaults(kwargs, Extrapolator = InDetExtrapolator)

    if 'ToolForWeightCalculation' not in kwargs :
        InDetWeightCalculator = InDetWeightCalculatorCfg(flags)
        acc.addPublicTool(InDetWeightCalculator)
        kwargs=setDefaults(kwargs, ToolForWeightCalculation = InDetWeightCalculator)

    if 'ToolForTRT_DriftCircleOnTrackCreation' not in kwargs :
        acc_tmp = InDetTRT_DriftCircleOnTrackToolCfg(flags)
        InDetTRT_DriftCircleOnTrackTool = acc_tmp.getPrimary()
        acc.merge(acc_tmp)
        kwargs=setDefaults(kwargs, ToolForTRT_DriftCircleOnTrackCreation = InDetTRT_DriftCircleOnTrackTool)

    acc.setPrivateTools(CompFactory.InDet__CompetingTRT_DriftCirclesOnTrackTool( the_name, **kwargs))
    return acc

####################################################################################################################################

#used in ConfiguredTRTSegmentFindingCfg
#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetTRT_TrackExtensionTool_DAFCfg(flags, name='TRT_TrackExtensionTool_DAF',**kwargs) :
    the_name = makeName( name, kwargs)
    acc = ComponentAccumulator()
    if 'CompetingDriftCircleTool' not in kwargs :
        InDetCompetingTRT_DC_Tool = acc.popToolsAndMerge(InDetCompetingTRT_DC_ToolCfg(flags))
        acc.addPublicTool(InDetCompetingTRT_DC_Tool)
        kwargs=setDefaults(kwargs, CompetingDriftCircleTool    = InDetCompetingTRT_DC_Tool)

    if 'PropagatorTool' not in kwargs :
        InDetPatternPropagator = InDetPatternPropagatorCfg(flags)
        acc.addPublicTool(InDetPatternPropagator)
        kwargs=setDefaults(kwargs, PropagatorTool              = InDetPatternPropagator)

    if 'RoadTool' not in kwargs :
        InDetTRT_RoadMaker = acc.popToolsAndMerge(InDetTRT_RoadMakerCfg(flags)) 
        acc.addPublicTool(InDetTRT_RoadMaker)
        kwargs=setDefaults(kwargs, RoadTool                    = InDetTRT_RoadMaker)

    kwargs = setDefaults(kwargs, TRT_DriftCircleContainer = flags.InDetKeys.TRT_DriftCircles)

    acc.setPrivateTools(CompFactory.InDet__TRT_TrackExtensionTool_DAF(the_name,**kwargs))
    return acc

####################################################################################################################################

#used in ConfiguredTRTSegmentFindingCfg
def InDetTRT_ExtensionToolCfg(flags, TrackingCuts=None, **kwargs) :
    # @TODO set all names to InDetTRT_ExtensionTool ?
    if (flags.InDetFlags.trtExtensionType == 'xk') or (not flags.InDetFlags.doNewTracking) :
        if flags.InDetFlags.doCosmics:
            return InDetTRT_ExtensionToolCosmicsCfg(flags, **kwargs)
        else:
            return InDetTRT_TrackExtensionTool_xkCfg(flags, TrackingCuts=TrackingCuts, **kwargs)
    elif flags.InDetFlags.trtExtensionType == 'DAF' :
        return InDetTRT_TrackExtensionTool_DAFCfg(flags, 'InDetTRT_ExtensionTool',**kwargs)

####################################################################################################################################

#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetPrdAssociationToolCfg(flags, name='InDetPrdAssociationTool',**kwargs) :
    acc = ComponentAccumulator()
    '''
    Provide an instance for all clients in which the tool is only set in c++
    '''
    the_name = makeName( name, kwargs)
    kwargs = setDefaults( 	kwargs,
                            PixelClusterAmbiguitiesMapName = flags.InDetKeys.GangedPixelMap,
                            addTRToutliers                 = True)
    acc.setPrivateTools(CompFactory.InDet__InDetPRD_AssociationToolGangedPixels(name = the_name,**kwargs))
    return acc

####################################################################################################################################

#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetPrdAssociationTool_setupCfg(flags, name='InDetPrdAssociationTool_setup',**kwargs) :
    acc = ComponentAccumulator()
    '''
    Provide an instance for all clients which set the tool explicitely
    '''
    acc.setPrivateTools(acc.popToolsAndMerge(InDetPrdAssociationToolCfg(flags, name = name, **setDefaults(kwargs, SetupCorrect = True))))
    return acc

####################################################################################################################################

#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetTrigPrdAssociationToolCfg(flags, name='InDetTrigPrdAssociationTool_setup', **kwargs) :
    acc = ComponentAccumulator()
    acc.setPrivateTools(acc.popToolsAndMerge(InDetPrdAssociationToolCfg(flags, name = name, 
                                                                        **setDefaults(kwargs,
                                                                        PixelClusterAmbiguitiesMapName = "TrigPixelClusterAmbiguitiesMap",
                                                                        addTRToutliers                 = False))))
    return acc

####################################################################################################################################

#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetPixelConditionsSummaryToolCfg(flags, name='PixelConditionsSummaryTool', **kwargs) :
    if flags.InDetFlags.usePixelDCS:
        kwargs = setDefaults(	kwargs,
                                IsActiveStates = [ 'READY', 'ON', 'UNKNOWN', 'TRANSITION', 'UNDEFINED' ],
                                IsActiveStatus = [ 'OK', 'WARNING', 'ERROR', 'FATAL' ])
    kwargs = setDefaults(	kwargs,
                            UseByteStream = (flags.globalflags.DataSource=='data'))
    return CompFactory.PixelConditionsSummaryTool(name = name,**kwargs)
####################################################################################################################################

#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetTestPixelLayerToolCfg(flags, name = "InDetTestPixelLayerTool", **kwargs) :
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    if 'PixelSummaryTool' not in kwargs :
        InDetPixelConditionsSummaryTool = InDetPixelConditionsSummaryToolCfg(flags)
        acc.addPublicTool(InDetPixelConditionsSummaryTool)
        kwargs = setDefaults( kwargs, PixelSummaryTool = InDetPixelConditionsSummaryTool)

    kwargs = setDefaults( 	kwargs,
                            CheckActiveAreas = flags.InDetFlags.checkDeadElementsOnTrack,
                            CheckDeadRegions = flags.InDetFlags.checkDeadElementsOnTrack,
                            CheckDisabledFEs = flags.InDetFlags.checkDeadElementsOnTrack)

    acc.setPrivateTools(CompFactory.InDet__InDetTestPixelLayerTool(name = the_name,**kwargs))
    return acc

####################################################################################################################################

#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetHoleSearchToolCfg(flags, name = 'InDetHoleSearchTool', **kwargs) :
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    
    if 'Extrapolator' not in kwargs :
        from TrackingCommonConfig import InDetExtrapolatorCfg
        InDetExtrapolator = acc.popToolsAndMerge(InDetExtrapolatorCfg(flags))
        acc.addPublicTool(InDetExtrapolator)
        kwargs = setDefaults( kwargs, Extrapolator     = InDetExtrapolator)

    if 'PixelSummaryTool' not in kwargs :
        InDetPixelConditionsSummaryTool = InDetPixelConditionsSummaryToolCfg(flags)
        acc.addPublicTool(InDetPixelConditionsSummaryTool)
        kwargs = setDefaults( kwargs, PixelSummaryTool = InDetPixelConditionsSummaryTool if flags.DetFlags.pixel_on else None)

    if 'SctSummaryTool' not in kwargs :
        kwargs = setDefaults( kwargs, SctSummaryTool   = None  if flags.DetFlags.SCT_on   else None)

    if 'PixelLayerTool' not in kwargs :
        InDetTestPixelLayerTool = acc.popToolsAndMerge(InDetTestPixelLayerToolCfg(flags))
        acc.addPublicTool(InDetTestPixelLayerTool)
        kwargs = setDefaults( kwargs, PixelLayerTool   = InDetTestPixelLayerTool)

    if flags.InDetFlags.doCosmics :
        kwargs = setDefaults( kwargs, Cosmics = True)

    kwargs = setDefaults( 	kwargs,
                            usePixel                     = flags.DetFlags.pixel_on,
                            useSCT                       = flags.DetFlags.SCT_on,
                            CountDeadModulesAfterLastHit = True)

    acc.setPrivateTools(CompFactory.InDet__InDetTrackHoleSearchTool(name = the_name,**kwargs))
    return acc

####################################################################################################################################

#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetSummaryHelperCfg(flags, name='InDetSummaryHelper',**kwargs) :
    acc = ComponentAccumulator()
    isHLT=kwargs.pop("isHLT",False)
    # Configrable version of loading the InDetTrackSummaryHelperTool
    #
    if 'AssoTool' not in kwargs :
        InDetPrdAssociationTool_setup = acc.popToolsAndMerge(InDetPrdAssociationTool_setupCfg(flags))
        acc.addPublicTool(InDetPrdAssociationTool_setup)
        InDetTrigPrdAssociationTool = acc.popToolsAndMerge(InDetTrigPrdAssociationToolCfg(flags))
        acc.addPublicTool(InDetTrigPrdAssociationTool)
        kwargs = setDefaults( kwargs, AssoTool = InDetPrdAssociationTool_setup   if not isHLT else   InDetTrigPrdAssociationTool)
    if 'HoleSearch' not in kwargs :
        InDetHoleSearchTool = acc.popToolsAndMerge(InDetHoleSearchToolCfg(flags))
        acc.addPublicTool(InDetHoleSearchTool)
        kwargs = setDefaults( kwargs, HoleSearch = InDetHoleSearchTool) # @TODO trigger specific hole search tool ?

    if not flags.DetFlags.TRT_on :
        kwargs = setDefaults( kwargs, TRTStrawSummarySvc = "")

    kwargs = setDefaults(	kwargs,
                            PixelToTPIDTool = None,         # we don't want to use those tools during pattern
                            TestBLayerTool  = None,         # we don't want to use those tools during pattern
                            # PixelToTPIDTool = InDetPixelToTPIDTool,
                            # TestBLayerTool  = InDetRecTestBLayerTool,
                            RunningTIDE_Ambi = flags.InDetFlags.doTIDE_Ambi,
                            DoSharedHits    = False,
                            usePixel        = flags.DetFlags.pixel_on,
                            useSCT          = flags.DetFlags.SCT_on,
                            useTRT          = flags.DetFlags.TRT_on)
    acc.setPrivateTools(CompFactory.InDet__InDetTrackSummaryHelperTool(name = name,**kwargs))
    return acc

####################################################################################################################################

#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetSummaryHelperNoHoleSearchCfg(flags, name='InDetSummaryHelperNoHoleSearch',**kwargs) :
    acc = ComponentAccumulator()
    if 'HoleSearch' not in kwargs :
        kwargs = setDefaults( kwargs, HoleSearch = None)
    acc.setPrivateTools(acc.popToolsAndMerge(InDetSummaryHelperCfg(flags, name = name,**kwargs)))
    return acc

####################################################################################################################################

#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetTrackSummaryToolCfg(flags, name='InDetTrackSummaryTool',**kwargs) :
    acc = ComponentAccumulator()
    # makeName will remove the namePrefix in suffix from kwargs, so copyArgs has to be first
    from TrackingCommonConfig import copyArgs
    hlt_args = copyArgs(kwargs,['isHLT','namePrefix'])
    kwargs.pop('isHLT',None)
    do_holes=kwargs.get("doHolesInDet",True)
    if 'InDetSummaryHelperTool' not in kwargs :
        InDetSummaryHelper = acc.popToolsAndMerge(InDetSummaryHelperCfg(flags, **hlt_args))
        acc.addPublicTool(InDetSummaryHelper)
        InDetSummaryHelperNoHoleSearch = acc.popToolsAndMerge(InDetSummaryHelperNoHoleSearchCfg(flags))
        acc.addPublicTool(InDetSummaryHelperNoHoleSearch)
        InDetSummaryHelperNoHoleSearch = acc.popToolsAndMerge(InDetSummaryHelperNoHoleSearchCfg(flags))
        acc.addPublicTool(InDetSummaryHelperNoHoleSearch)
        kwargs = setDefaults( kwargs, InDetSummaryHelperTool = InDetSummaryHelper if do_holes else InDetSummaryHelperNoHoleSearch)

    #
    # Configurable version of TrkTrackSummaryTool: no TRT_PID tool needed here (no shared hits)
    #
    kwargs = setDefaults(kwargs,
                            doSharedHits           = False,
                            doHolesInDet           = do_holes,
                            TRT_ElectronPidTool    = None,         # we don't want to use those tools during pattern
                            TRT_ToT_dEdxTool       = None,         # dito
                            PixelToTPIDTool        = None)         # we don't want to use those tools during pattern
    acc.setPrivateTools(CompFactory.Trk__TrackSummaryTool(name = name, **kwargs))
    return acc

####################################################################################################################################

#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetCosmicsScoringToolBaseCfg(flags, name='InDetCosmicsScoringTool', **kwargs) :
    acc = ComponentAccumulator()
    NewTrackingCuts = kwargs.pop("NewTrackingCuts")
    InDetTrackSummaryTool = acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags))
    acc.addPublicTool(InDetTrackSummaryTool)
    kwargs=setDefaults(	kwargs,
                        nWeightedClustersMin = NewTrackingCuts.nWeightedClustersMin(),
                        minTRTHits           = 0,
                        SummaryTool          = InDetTrackSummaryTool)
    acc.setPrivateTools(CompFactory.InDet__InDetCosmicScoringTool(name = name, **kwargs))
    return acc

#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetCosmicExtenScoringToolCfg(flags, NewTrackingCuts, name='InDetCosmicExtenScoringTool',**kwargs) :
    acc = ComponentAccumulator()
    acc.setPrivateTools(acc.popToolsAndMerge(InDetCosmicsScoringToolBaseCfg(flags, name = 'InDetCosmicExtenScoringTool',
                                                                            **setDefaults(	kwargs,
                                                                            NewTrackingCuts      = NewTrackingCuts,
                                                                            nWeightedClustersMin = 0,
                                                                            minTRTHits           = NewTrackingCuts.minTRTonTrk()))))
    return acc

####################################################################################################################################

#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetROIInfoVecCondAlgCfg(flags, name='InDetROIInfoVecCondAlg',**kwargs) :
    kwargs=setDefaults(kwargs,
                        InputEmClusterContainerName = flags.InDetKeys.CaloClusterROIContainer,
                        WriteKey                    = kwargs.get("namePrefix","") +"ROIInfoVec"+ kwargs.get("nameSuffix",""),
                        minPtEM                     = 5000.  # in MeV
                        )
    return CompFactory.ROIInfoVecAlg(name = name,**kwargs)

####################################################################################################################################

#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetAmbiScoringToolBaseCfg(flags, name='InDetAmbiScoringTool', **kwargs) :
    acc = ComponentAccumulator()
    NewTrackingCuts = kwargs.pop("NewTrackingCuts")

    from TrackingCommonConfig import InDetExtrapolatorCfg
    InDetExtrapolator = acc.popToolsAndMerge(InDetExtrapolatorCfg(flags))
    acc.addPublicTool(InDetExtrapolator)

    InDetTrackSummaryTool = acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags))
    acc.addPublicTool(InDetTrackSummaryTool)

    InDetTRTDriftCircleCutForPatternReco = InDetTRTDriftCircleCutForPatternRecoCfg(flags)
    acc.addPublicTool(InDetTRTDriftCircleCutForPatternReco)

    have_calo_rois = flags.InDetFlags.doBremRecovery and flags.InDetFlags.doCaloSeededBrem and flags.DetFlags.detdescr.Calo_allOn
    if have_calo_rois :
        alg = InDetROIInfoVecCondAlgCfg(flags, name = 'InDetROIInfoVecCondAlg')
        kwargs=setDefaults(kwargs, CaloROIInfoName = alg.WriteKey )
    kwargs=setDefaults(	kwargs, 
                        Extrapolator            = InDetExtrapolator,
                        SummaryTool             = InDetTrackSummaryTool,
                        DriftCircleCutTool      = InDetTRTDriftCircleCutForPatternReco,
                        useAmbigFcn             = True,  # this is NewTracking
                        useTRT_AmbigFcn         = False,
                        maxZImp                 = NewTrackingCuts.maxZImpact(),
                        maxEta                  = NewTrackingCuts.maxEta(),
                        usePixel                = NewTrackingCuts.usePixel(),
                        useSCT                  = NewTrackingCuts.useSCT(),
                        doEmCaloSeed            = have_calo_rois)

    acc.setPrivateTools(CompFactory.InDet__InDetAmbiScoringTool(name = name, **kwargs))
    return acc

####################################################################################################################################

#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetAmbiScoringToolCfg(flags, NewTrackingCuts, name='InDetAmbiScoringTool', **kwargs) :
    acc = ComponentAccumulator()
    kwargs=setDefaults(	kwargs, 
                        NewTrackingCuts         = NewTrackingCuts,
                        useAmbigFcn             = True,  # this is NewTracking
                        useTRT_AmbigFcn         = False,
                        minTRTonTrk             = 0,
                        minTRTPrecisionFraction = 0,
                        minPt                   = NewTrackingCuts.minPT(),
                        maxRPhiImp              = NewTrackingCuts.maxPrimaryImpact(),
                        minSiClusters           = NewTrackingCuts.minClusters(),
                        minPixel                = NewTrackingCuts.minPixel(),
                        maxSiHoles              = NewTrackingCuts.maxHoles(),
                        maxPixelHoles           = NewTrackingCuts.maxPixelHoles(),
                        maxSCTHoles             = NewTrackingCuts.maxSCTHoles(),
                        maxDoubleHoles          = NewTrackingCuts.maxDoubleHoles())

    acc.setPrivateTools(acc.popToolsAndMerge(InDetAmbiScoringToolBaseCfg(flags, name = name + NewTrackingCuts.extension(), **kwargs )))
    return acc

####################################################################################################################################

#used in ConfiguredNewTrackingTRTExtensionConfig
def InDetExtenScoringToolCfg(flags, NewTrackingCuts, name='InDetExtenScoringTool', **kwargs) :
    acc = ComponentAccumulator()
    if flags.InDetFlags.trackFitterType in ['KalmanFitter', 'KalmanDNAFitter', 'ReferenceKalmanFitter']:
        kwargs=setDefaults(kwargs, minTRTPrecisionFraction = 0.2)
    kwargs=setDefaults(	kwargs,
                        minTRTonTrk             = NewTrackingCuts.minTRTonTrk(),
                        minTRTPrecisionFraction = NewTrackingCuts.minTRTPrecFrac())

    acc.setPrivateTools(acc.popToolsAndMerge(InDetAmbiScoringToolCfg(flags, NewTrackingCuts, name = name, **kwargs)))
    return acc

####################################################################################################################################

def PRDtoTrackMapToolCfg(name='PRDtoTrackMapTool',**kwargs) :
    the_name = makeName( name, kwargs)
    return CompFactory.Trk__PRDtoTrackMapTool( name=the_name, **kwargs)

####################################################################################################################################

def InDetPixelToTPIDToolCfg(name = "InDetPixelToTPIDTool", **kwargs) :
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    InDetPixelToTPIDTool = CompFactory.InDet__PixelToTPIDTool(name = the_name, **kwargs)
    acc.setPrivateTools(InDetPixelToTPIDTool)
    return acc

def InDetRecTestBLayerToolCfg(flags, name='InDetRecTestBLayerTool', **kwargs) :
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)

    if not flags.DetFlags.pixel_on:
        return None

    if 'Extrapolator' not in kwargs :
        InDetExtrapolator = acc.popToolsAndMerge(InDetExtrapolatorCfg(flags))
        acc.addPublicTool(InDetExtrapolator)
        kwargs = setDefaults( kwargs, Extrapolator     = InDetExtrapolator)

    if 'PixelSummaryTool' not in kwargs :
        InDetPixelConditionsSummaryTool = InDetPixelConditionsSummaryToolCfg(flags)
        acc.addPublicTool(InDetPixelConditionsSummaryTool)
        kwargs = setDefaults( kwargs, PixelSummaryTool = InDetPixelConditionsSummaryTool)

    InDetRecTestBLayerTool = CompFactory.InDet__InDetTestBLayerTool(name=the_name, **kwargs)
    acc.setPrivateTools(InDetRecTestBLayerTool)
    return acc

####################################################################################################################################

def InDetSummaryHelperSharedHitsCfg(flags, name='InDetSummaryHelperSharedHits', **kwargs) :
    acc = ComponentAccumulator()
    if 'PixelToTPIDTool' not in kwargs :
        InDetPixelToTPIDTool = acc.popToolsAndMerge(InDetPixelToTPIDToolCfg())
        acc.addPublicTool(InDetPixelToTPIDTool)
        kwargs = setDefaults( kwargs,  PixelToTPIDTool = InDetPixelToTPIDTool)

    if 'TestBLayerTool' not in kwargs :
        InDetRecTestBLayerTool = acc.popToolsAndMerge(InDetRecTestBLayerToolCfg(flags))
        acc.addPublicTool(InDetRecTestBLayerTool)
        kwargs = setDefaults( kwargs,  TestBLayerTool  = InDetRecTestBLayerTool)

    kwargs = setDefaults( kwargs,      DoSharedHits    = flags.InDetFlags.doSharedHits)

    if flags.DetFlags.TRT_on:
        kwargs = setDefaults( kwargs,  DoSharedHitsTRT = flags.InDetFlags.doSharedHits)

    acc.setPrivateTools(acc.popToolsAndMerge(InDetSummaryHelperCfg(flags, name = name, **kwargs)))
    return acc

####################################################################################################################################

def InDetTRTStrawStatusSummaryToolCfg(flags, name = "InDetTRT_StrawStatusSummaryTool", **kwargs) :
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    kwargs = setDefaults( kwargs, isGEANT4 = (flags.globalflags.DataSource == 'geant4'))
    InDetTRTStrawStatusSummaryTool = CompFactory.TRT_StrawStatusSummaryTool(name = the_name, **kwargs )
    acc.setPrivateTools(InDetTRTStrawStatusSummaryTool)
    return acc

####################################################################################################################################

def InDetTRTCalDbToolCfg(name = "InDetTRT_CalDbTool", **kwargs) :
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    InDetTRTCalDbTool =  CompFactory.TRT_CalDbTool(name = the_name, **kwargs)
    acc.setPrivateTools(InDetTRTCalDbTool)
    return acc

####################################################################################################################################

def InDetTRT_LocalOccupancyCfg(flags, name ="InDet_TRT_LocalOccupancy", **kwargs) :
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    if 'TRTCalDbTool' not in kwargs :
        InDetTRTCalDbTool = acc.popToolsAndMerge(InDetTRTCalDbToolCfg())
        acc.addPublicTool(InDetTRTCalDbTool)
        kwargs = setDefaults( kwargs, TRTCalDbTool = InDetTRTCalDbTool )

    if 'TRTStrawStatusSummaryTool' not in kwargs :
        InDetTRTStrawStatusSummaryTool = acc.popToolsAndMerge(InDetTRTStrawStatusSummaryToolCfg(flags))
        acc.addPublicTool(InDetTRTStrawStatusSummaryTool)
        kwargs = setDefaults( kwargs, TRTStrawStatusSummaryTool = InDetTRTStrawStatusSummaryTool )

    InDetTRT_LocalOccupancy = CompFactory.InDet__TRT_LocalOccupancy(name=the_name, **setDefaults( kwargs, isTrigger = False) )
    acc.setPrivateTools(InDetTRT_LocalOccupancy)
    return acc

####################################################################################################################################

def InDetTRT_dEdxToolCfg(flags, name = "InDetTRT_dEdxTool", **kwargs) :
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)

    kwargs = setDefaults( kwargs, TRT_dEdx_isData = (flags.globalflags.DataSource == 'data'))

    if 'TRT_LocalOccupancyTool' not in kwargs :
        InDetTRT_LocalOccupancy = acc.popToolsAndMerge(InDetTRT_LocalOccupancyCfg(flags))
        acc.addPublicTool(InDetTRT_LocalOccupancy)
        kwargs = setDefaults( kwargs, TRT_LocalOccupancyTool = InDetTRT_LocalOccupancy)

    InDetTRT_dEdxTool = CompFactory.TRT_ToT_dEdx(name = the_name, **kwargs)
    acc.setPrivateTools(InDetTRT_dEdxTool)
    return acc

####################################################################################################################################

def InDetTRT_ElectronPidToolCfg(flags, name = "InDetTRT_ElectronPidTool", **kwargs) :
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)

    if 'TRTStrawSummaryTool' not in kwargs :
        InDetTRTStrawStatusSummaryTool = acc.popToolsAndMerge(InDetTRTStrawStatusSummaryToolCfg(flags))
        acc.addPublicTool(InDetTRTStrawStatusSummaryTool)
        kwargs = setDefaults( kwargs, TRTStrawSummaryTool = InDetTRTStrawStatusSummaryTool)

    if 'TRT_LocalOccupancyTool' not in kwargs :
        InDetTRT_LocalOccupancy = acc.popToolsAndMerge(InDetTRT_LocalOccupancyCfg(flags))
        acc.addPublicTool(InDetTRT_LocalOccupancy)
        kwargs = setDefaults( kwargs, TRT_LocalOccupancyTool = InDetTRT_LocalOccupancy)

    if 'TRT_ToT_dEdx_Tool' not in kwargs :
        if not flags.DetFlags.TRT_on or flags.InDetFlags.doSLHC or flags.InDetFlags.doHighPileup or  flags.InDetFlags.useExistingTracksAsInput:
            InDetTRT_dEdxTool = None
        else:
            InDetTRT_dEdxTool = acc.popToolsAndMerge(InDetTRT_dEdxToolCfg(flags))
            acc.addPublicTool(InDetTRT_dEdxTool)
        kwargs = setDefaults( kwargs, TRT_ToT_dEdx_Tool = InDetTRT_dEdxTool)

    kwargs = setDefaults( kwargs, isData = (flags.globalflags.DataSource == 'data'))

    InDetTRT_ElectronPidTool = CompFactory.InDet__TRT_ElectronPidToolRun2(name = the_name, **kwargs)
    acc.setPrivateTools(InDetTRT_ElectronPidTool)
    return acc

####################################################################################################################################

def InDetTrackSummaryToolSharedHitsCfg(flags, name='InDetTrackSummaryToolSharedHits',**kwargs) :
    acc = ComponentAccumulator()
    if 'InDetSummaryHelperTool' not in kwargs :
        InDetSummaryHelperSharedHits = acc.popToolsAndMerge(InDetSummaryHelperSharedHitsCfg(flags))
        acc.addPublicTool(InDetSummaryHelperSharedHits)
        kwargs = setDefaults( kwargs, InDetSummaryHelperTool = InDetSummaryHelperSharedHits)

    if 'TRT_ElectronPidTool' not in kwargs :
        if not flags.DetFlags.TRT_on or  flags.InDetFlags.doSLHC or  flags.InDetFlags.doHighPileup or  flags.InDetFlags.useExistingTracksAsInput:
            InDetTRT_ElectronPidTool = None
        else:
            InDetTRT_ElectronPidTool = acc.popToolsAndMerge(InDetTRT_ElectronPidToolCfg(flags))
            acc.addPublicTool(InDetTRT_ElectronPidTool)
        kwargs = setDefaults( kwargs, TRT_ElectronPidTool    = InDetTRT_ElectronPidTool)

    if 'TRT_ToT_dEdxTool' not in kwargs :
        if not flags.DetFlags.TRT_on or flags.InDetFlags.doSLHC or flags.InDetFlags.doHighPileup or  flags.InDetFlags.useExistingTracksAsInput:
            InDetTRT_dEdxTool = None
        else:
            InDetTRT_dEdxTool = acc.popToolsAndMerge(InDetTRT_dEdxToolCfg(flags))
            acc.addPublicTool(InDetTRT_dEdxTool)
        kwargs = setDefaults( kwargs, TRT_ToT_dEdxTool       = InDetTRT_dEdxTool)

    if 'PixelToTPIDTool' not in kwargs :
        InDetPixelToTPIDTool = acc.popToolsAndMerge(InDetPixelToTPIDToolCfg())
        acc.addPublicTool(InDetPixelToTPIDTool)
        kwargs = setDefaults( kwargs, PixelToTPIDTool        = InDetPixelToTPIDTool)

    kwargs = setDefaults(kwargs,
                         doSharedHits           = flags.InDetFlags.doSharedHits,
                         TRTdEdx_DivideByL      = True, # default is True
                         TRTdEdx_useHThits      = True, # default is True
                         TRTdEdx_corrected      = True, # default is True
                         minTRThitsForTRTdEdx   = 1)    # default is 1
    InDetTrackSummaryTool = acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(InDetTrackSummaryTool)
    return acc

####################################################################################################################################

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    import IDTestFlags
    flags = IDTestFlags.IDTestFlags_q221()
    flags.Input.Files=["myRDO.pool.root"]
    flags.lock()
    from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg
    acc = MainServicesThreadedCfg(flags)

    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
    acc.merge(PixelGeometryCfg(flags))
    acc.merge(SCT_GeometryCfg(flags))

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(flags))
    from PixelConditionsConfig import PixelConditionsSummaryCfg
    acc.popToolsAndMerge( PixelConditionsSummaryCfg(flags))

    InDetRotCreatorCfg(flags)