#====================================================================
# Collection of tools to run the conversion finder
#====================================================================

class BPHYConversionFinderTools:

    def __init__(self, derivation = ""):

        if derivation == "":
            print '--------> FATAL: BPHYConversionFinderTools, "derivation" string not set!'
            import sys
            sys.exit()


        prefix = derivation+"ConversionFinderTools"

        from AthenaCommon.AppMgr import ToolSvc

        # set up extrapolator
        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        self.Extrapolator = AtlasExtrapolator(name = prefix+"_AtlasExtrapolator")
        ToolSvc += self.Extrapolator
        print      self.Extrapolator

        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        self.InDetSecVxFitterTool = Trk__TrkVKalVrtFitter(name                = prefix+"_Fitter",
                                                 Extrapolator        = self.Extrapolator,
                                                 MakeExtendedVertex  = True,
                                                 FirstMeasuredPoint  = True,
                                                 Robustness          = 6,
                                                 InputParticleMasses = [0.511,0.511],
                                                 VertexForConstraint = [0.,0.,0.],
                                                 CovVrtForConstraint = [0.015*0.015,0.,0.015*0.015,0.,0.,10000.*10000.],
                                                 FirstMeasuredPointLimit = True,
                                                 usePhiCnst          = True,
                                                 useThetaCnst        = True)
        ToolSvc += self.InDetSecVxFitterTool
        print      self.InDetSecVxFitterTool


        from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__SeedNewtonTrkDistanceFinder
        self.InDetSecVxTrkDistanceFinder = Trk__SeedNewtonTrkDistanceFinder(name = prefix+"_TrkDistanceFinder")
        ToolSvc += self.InDetSecVxTrkDistanceFinder
        print self.InDetSecVxTrkDistanceFinder

# The selection peformed here is now done in BPhysConversionFinder
#        from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__ConversionFinderUtils
#        self.InDetSecVxHelper = InDet__ConversionFinderUtils(name = prefix+"_FinderUtils")
#        ToolSvc += self.InDetSecVxHelper
#        print self.InDetSecVxHelper
#
#        from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__TrackPairsSelector
#        self.InDetSecVxTrackPairsSelector = InDet__TrackPairsSelector(name                       = prefix+"_TrackPairsSelector",
#                                                                 ConversionFinderHelperTool = self.InDetSecVxHelper,
#                                                                 DistanceTool               = self.InDetSecVxTrkDistanceFinder,
#                                                                 MaxFirstHitRadius          = 500.0,
#                                                                 MaxDistBetweenTracks       = [10.,50.,50.],
#                                                                 MaxEta                     = [0.3,0.5,0.5],
#                                                                 MaxInitDistance            = [10000.0, 10000.0, 10000.0],
#                                                                 MinTrackAngle              = 0.0)
#        ToolSvc += self.InDetSecVxTrackPairsSelector
#        print self.InDetSecVxTrackPairsSelector


        from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__VertexPointEstimator
        self.InDetSecVtxPointEstimator = InDet__VertexPointEstimator(name = prefix+"_PointEstimator",
                                                                MinDeltaR = [-5.,-25.,-50.], # D-R1-R2 min cut
                                                                MaxDeltaR = [5.,10.,10.] ,  # D-R1-R2 max cut
                                                                MaxPhi    = [0.05, 0.5, 0.5])  # dphi cut at vertex
        ToolSvc += self.InDetSecVtxPointEstimator
        print self.InDetSecVtxPointEstimator


        from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__ConversionPostSelector
        self.InDetSecVtxPostSelector = InDet__ConversionPostSelector(name = prefix+"_PostSelector",
                                                                MaxChi2Vtx       = [10.,10.,10.],
                                                                MaxInvariantMass = [10000.,10000.,10000.],
                                                                MinFitMomentum   = [0.,0.,0.],
                                                                MinRadius        = [10.0,10.0,10.0],
                                                                MinPt            = 0.0,
                                                                MaxdR            = -10000.0,
                                                                MaxPhiVtxTrk     = 10000.0)
        ToolSvc += self.InDetSecVtxPostSelector
        print self.InDetSecVtxPostSelector
