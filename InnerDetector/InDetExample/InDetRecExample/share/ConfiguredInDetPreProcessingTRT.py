# Blocking the include for after first inclusion
include.block ('InDetRecExample/ConfiguredInDetPreProcessingTRT.py')

# ------------------------------------------------------------
#
# ----------- TRT Data-Preparation stage
#
# ------------------------------------------------------------
#
class ConfiguredInDetPreProcessingTRT:

   def __init__(self, useTimeInfo = True, usePhase = False):

      from InDetRecExample.InDetJobProperties import InDetFlags
      from AthenaCommon.DetFlags              import DetFlags
      from AthenaCommon.GlobalFlags           import globalflags
      from AthenaCommon.BeamFlags             import jobproperties
      #
      # get ToolSvc and topSequence
      #
      from AthenaCommon.AppMgr      import ToolSvc
      from AthenaCommon.AlgSequence import AlgSequence
      topSequence = AlgSequence()
      
      if InDetFlags.doPRDFormation() and DetFlags.makeRIO.TRT_on() and InDetFlags.doTRT_PRDFormation():

         #
         # --- setup naming of tools and algs
         #
         if useTimeInfo:
            prefix     = "InDetTRT_"
            collection = InDetKeys.TRT_DriftCircles()
         else:
            prefix     = "InDetTRT_noTime_"
            collection = InDetKeys.TRT_DriftCirclesUncalibrated()
         #
         # --- TRT_DriftFunctionTool
         #
         from TRT_DriftFunctionTool.TRT_DriftFunctionToolConf import TRT_DriftFunctionTool
         InDetTRT_DriftFunctionTool = TRT_DriftFunctionTool(name                = prefix+"DriftFunctionTool",
                                                            TRTCalDbTool        = InDetTRTCalDbSvc)
         # --- overwrite for uncalibrated DC production
         if (not useTimeInfo) or InDetFlags.noTRTTiming():
            InDetTRT_DriftFunctionTool.DummyMode      = True
            InDetTRT_DriftFunctionTool.UniversalError = 1.15
         # --- overwrite for calibration of MC
         if usePhase and jobproperties.Beam.beamType()=='cosmics' and globalflags.DataSource == "geant4":
            InDetTRT_DriftFunctionTool.AllowDigiVersionOverride = True
            InDetTRT_DriftFunctionTool.ForcedDigiVersion        = 9

         ToolSvc += InDetTRT_DriftFunctionTool
         if (InDetFlags.doPrintConfigurables()):
            print InDetTRT_DriftFunctionTool
         #
         # --- TRT_DriftCircleTool
         #
         if usePhase:
            from TRT_DriftCircleTool.TRT_DriftCircleToolConf import InDet__TRT_DriftCircleToolCosmics as InDet__TRT_DriftCircleTool
         else:
            from TRT_DriftCircleTool.TRT_DriftCircleToolConf import InDet__TRT_DriftCircleTool

         #
         # set gating values for MC/DATA
         # for now, no values for argon. This is OK (?) for 50 ns. What about 25ns?
         MinTrailingEdge = 17.0*ns
         MaxDriftTime    = 66.0*ns
         LowGate         = 19.0*ns
         HighGate        = 44.0*ns
         if globalflags.DataSource == 'data':
            MinTrailingEdge = 11.0*ns
            MaxDriftTime    = 60.0*ns
            LowGate         = 19.0*ns
            HighGate        = 44.0*ns

         InDetTRT_DriftCircleTool = InDet__TRT_DriftCircleTool(name                            = prefix+"DriftCircleTool",
                                                               TRTDriftFunctionTool            = InDetTRT_DriftFunctionTool,
                                                               TrtDescrManageLocation          = InDetKeys.TRT_Manager(),
                                                               ConditionsSummaryTool           = InDetTRTStrawStatusSummarySvc,
                                                               #used to be InDetTRTConditionsSummaryService,
                                                               UseConditionsStatus             = True,
                                                               UseConditionsHTStatus           = True,
                                                               SimpleOutOfTimePileupSupression = True,
                                                               RejectIfFirstBit                = False, # fixes 50 nsec issue 
                                                               MinTrailingEdge                 = MinTrailingEdge,
                                                               MaxDriftTime                    = MaxDriftTime,
                                                               ValidityGateSuppression         = False,
                                                               LowGate                         = LowGate,
                                                               HighGate                        = HighGate,
                                                               MaskFirstHTBit                  = False,
                                                               MaskMiddleHTBit                 = False,
                                                               MaskLastHTBit                   = False,
                                                               SimpleOutOfTimePileupSupressionArgon = False,# no OOT rejection for argon
                                                               RejectIfFirstBitArgon                = False, # no OOT rejection for argon
                                                               MinTrailingEdgeArgon                 = 0, # no OOT rejection for argon
                                                               MaxDriftTimeArgon                    = 99*ns,# no OOT rejection for argon
                                                               ValidityGateSuppressionArgon         = False,# no OOT rejection for argon
                                                               LowGateArgon                         = 0,# no OOT rejection for argon
                                                               HighGateArgon                        = 75*ns,# no OOT rejection for argon
                                                               MaskFirstHTBitArgon                  = False,
                                                               MaskMiddleHTBitArgon                 = False,
                                                               MaskLastHTBitArgon                   = False,
                                                               useDriftTimeHTCorrection        = True,
                                                               useDriftTimeToTCorrection       = True) # reenable ToT

         from AthenaCommon.BeamFlags import jobproperties 
         if InDetFlags.InDet25nsec() and jobproperties.Beam.beamType()=="collisions": 
            InDetTRT_DriftCircleTool.ValidityGateSuppression=True  
            InDetTRT_DriftCircleTool.SimpleOutOfTimePileupSupression=False  
         if jobproperties.Beam.beamType()=="cosmics": 
            InDetTRT_DriftCircleTool.SimpleOutOfTimePileupSupression=False 

# --- overwrite for phase usage
         if usePhase:
            InDetTRT_DriftCircleTool.ComTimeName = "TRT_Phase"

         ToolSvc += InDetTRT_DriftCircleTool
         if (InDetFlags.doPrintConfigurables()):
            print InDetTRT_DriftCircleTool
         
         #
         # --- TRT_RIO_Maker Algorithm
         #
         from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__TRT_RIO_Maker
         InDetTRT_RIO_Maker = InDet__TRT_RIO_Maker(name                   = prefix+"RIO_Maker",
                                                   TRT_DriftCircleTool    = InDetTRT_DriftCircleTool,
                                                   TrtDescrManageLocation = InDetKeys.TRT_Manager(),
                                                   TRTRDOLocation         = InDetKeys.TRT_RDOs(),
                                                   TRTRIOLocation         = collection)
         topSequence += InDetTRT_RIO_Maker
         if (InDetFlags.doPrintConfigurables()):
            print InDetTRT_RIO_Maker
       
         #
         # --- we need to do truth association if requested (not for uncalibrated hits in cosmics)
         #
         if InDetFlags.doTruth() and useTimeInfo:
            from InDetTruthAlgs.InDetTruthAlgsConf import InDet__PRD_MultiTruthMaker
            InDetPRD_MultiTruthMakerTRT = InDet__PRD_MultiTruthMaker (name                        = prefix+"PRD_MultiTruthMaker",
                                                                      PixelClusterContainerName   = "",
                                                                      SCTClusterContainerName     = "",
                                                                      TRTDriftCircleContainerName = InDetKeys.TRT_DriftCircles(),
                                                                      SimDataMapNamePixel         = "",
                                                                      SimDataMapNameSCT           = "",
                                                                      SimDataMapNameTRT           = InDetKeys.TRT_SDOs(),
                                                                      TruthNamePixel              = "",
                                                                      TruthNameSCT                = "",
                                                                      TruthNameTRT                = InDetKeys.TRT_DriftCirclesTruth())
            topSequence += InDetPRD_MultiTruthMakerTRT
            if (InDetFlags.doPrintConfigurables()):
               print InDetPRD_MultiTruthMakerTRT

