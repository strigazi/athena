# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function


#########################################################################
# TrackingCutsConfig 
#########################################################################
# Additional flags and set of dynamically loaded flags for Tracking configuration
#
# Author: Ivan Yeletskikh
#
#########################################################################

import AthenaCommon.SystemOfUnits as Units

# --------------------------------------
# --- NEW TRACKING cuts
# --------------------------------------
def addTrackingFlags():
	from InDetConfigFlags import createInDetConfigFlags
	icf = createInDetConfigFlags()
	## constants
	max_holes = 3 ##was 5   

	icf.addFlag("InDet.NewTracking.extension, "")
	icf.addFlag("InDet.NewTracking.minPT", 0.1 * Units.GeV)
	icf.addFlag("InDet.NewTracking.minSecondaryPt", 0.4 * Units.GeV) #Pt cut for back tracking + segment finding for these
	icf.addFlag("InDet.NewTracking.minTRTonlyPt", 0.4 * Units.GeV) #Pt cut for TRT only

	# --- first set kinematic defaults
	icf.addFlag("InDet.NewTracking.maxPT", None) # off!
	icf.addFlag("InDet.NewTracking.minEta", -1) # off!
	icf.addFlag("InDet.NewTracking.maxEta", 2.7)


	icf.addFlag("InDet.NewTracking.minClusters", 7) # Igor 6, was 7
	icf.addFlag("InDet.NewTracking.minSiNotShared", 6)
	icf.addFlag("InDet.NewTracking.maxShared", 1) # cut is now on number of shared modules
	icf.addFlag("InDet.NewTracking.minPixel", 0)
	icf.addFlag("InDet.NewTracking.maxHoles", max_holes) #was 5
	icf.addFlag("InDet.NewTracking.maxPixelHoles", 2) #was 5
	icf.addFlag("InDet.NewTracking.maxSctHoles", 2) #was 5
	icf.addFlag("InDet.NewTracking.maxDoubleHoles", 1) #was 2
	icf.addFlag("InDet.NewTracking.maxPrimaryImpact", 10.0 * Units.mm) #low lumi
	icf.addFlag("InDet.NewTracking.maxZImpact", 320.0 * Units.mm) #was 250mm

	# --- this is for the TRT-extension
	icf.addFlag("InDet.NewTracking.minTRTonTrk", 9)
	icf.addFlag("InDet.NewTracking.minTRTPrecFrac", 0.4) #old: 0.3

	# --- general pattern cuts for NewTracking
	icf.addFlag("InDet.NewTracking.radMax", 600.0 * Units.mm) # default R cut for SP in SiSpacePointsSeedMaker
	icf.addFlag("InDet.NewTracking.roadWidth", 20.0)
	icf.addFlag("InDet.NewTracking.nHolesMax", max_holes )  #####################!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	icf.addFlag("InDet.NewTracking.nHolesGapMax", max_holes ) # not as tight as 2*maxDoubleHoles
	icf.addFlag("InDet.NewTracking.Xi2max", 15.0)
	icf.addFlag("InDet.NewTracking.Xi2maxNoAdd", 35.0)
	icf.addFlag("InDet.NewTracking.nWeightedClustersMin", 6)

	# --- seeding
	icf.addFlag("InDet.NewTracking.seedFilterLevel", 1)
	icf.addFlag("InDet.NewTracking.maxdImpactPPSSeeds", 1.7)
	icf.addFlag("InDet.NewTracking.maxdImpactSSSSeeds", 1000.0)

	# --- min pt cut for brem
	icf.addFlag("InDet.NewTracking.minPTBrem", 1. * Units.GeV) # off
	icf.addFlag("InDet.NewTracking.phiWidthBrem", 0.3 ) # default is 0.3
	icf.addFlag("InDet.NewTracking.etaWidthBrem", 0.2 ) # default is 0.3

	# --- Z Boundary Seeding
	icf.addFlag("InDet.NewTracking.doZBoundary", False)
	
	# --------------------------------------
	# --- BACK TRACKING cuts
	# --------------------------------------

	# --- settings for segment finder
	icf.addFlag("InDet.NewTracking.TRTSegFinderPtBins", 70)
	icf.addFlag("InDet.NewTracking.maxSegTRTShared", 0.7)
	icf.addFlag("InDet.NewTracking.excludeUsedTRToutliers", False)

	# --- triggers SegmentFinder and BackTracking
	icf.addFlag("InDet.NewTracking.useParameterizedTRTCuts", False)
	icf.addFlag("InDet.NewTracking.useNewParameterizationTRT", False)
	icf.addFlag("InDet.NewTracking.maxSecondaryTRTShared", 0.7)

	# --- defaults for secondary tracking
	icf.addFlag("InDet.NewTracking.maxSecondaryImpact", 100.0 * Units.mm) # low lumi
	
	icf.addFlag("InDet.NewTracking.minSecondaryClusters      , 4)
	icf.addFlag("InDet.NewTracking.minSecondarySiNotShared   , 4)
	icf.addFlag("InDet.NewTracking.maxSecondaryShared        , 1)  # cut is now on number of shared modules
	icf.addFlag("InDet.NewTracking.minSecondaryTRTonTrk      , 10)
	icf.addFlag("InDet.NewTracking.minSecondaryTRTPrecFrac   , 0.0)
	icf.addFlag("InDet.NewTracking.maxSecondaryHoles         , 2)
	icf.addFlag("InDet.NewTracking.maxSecondaryPixelHoles    , 2)
	icf.addFlag("InDet.NewTracking.maxSecondarySCTHoles      , 2)
	icf.addFlag("InDet.NewTracking.maxSecondaryDoubleHoles   , 1)
	icf.addFlag("InDet.NewTracking.SecondarynHolesMax        , InDet.NewTracking.maxSecondaryHoles)
	icf.addFlag("InDet.NewTracking.SecondarynHolesGapMax     , InDet.NewTracking.maxSecondaryHoles)

	icf.addFlag("InDet.NewTracking.rejectShortExtensions     , False) # extension finder in back tracking
	icf.addFlag("InDet.NewTracking.SiExtensionCuts           , False) # cut in Si Extensions before fit

	# --- pattern cuts for back tracking
	icf.addFlag("InDet.NewTracking.SecondaryXi2max           , 15.0)
	icf.addFlag("InDet.NewTracking.SecondaryXi2maxNoAdd      , 50.0)

	# --- run back tracking and TRT only in RoI seed regions
	icf.addFlag("InDet.NewTracking.RoISeededBackTracking     , False)

	# --------------------------------------
	# --- TRT Only TRACKING cuts
	# --------------------------------------

	# --- TRT only
	icf.addFlag("InDet.NewTracking.minTRTonly                , 15)
	icf.addFlag("InDet.NewTracking.maxTRTonlyShared          , 0.7)
	icf.addFlag("InDet.NewTracking.useTRTonlyParamCuts       , False)
	icf.addFlag("InDet.NewTracking.useTRTonlyOldLogic        , True)

	#
	# --------------------------------------
	# --- now start tighening cuts level by level
	# --------------------------------------
	#
	if icf.InDet.cutLevel >= 2:
		# --- cutLevel() == 3, defaults for 2011 first processing
		icf.InDet.NewTracking.minPT = 0.4 * Units.GeV
		icf.InDet.NewTracking.minSecondaryPt = 1.0 * Units.GeV 
		icf.InDet.NewTracking.minTRTonlyPt              = 1.0 * Units.GeV  # Pt cut for TRT only
		icf.InDet.NewTracking.TRTSegFinderPtBins        = 50

	if icf.InDet.cutLevel >= 3:
		# --- cutLevel() == 3, defaults for 2011 reprocessing
		icf.InDet.NewTracking.useParameterizedTRTCuts   = True # toggles BackTracking and TRT only
		icf.InDet.NewTracking.useNewParameterizationTRT = True

	if icf.InDet.cutLevel >= 4:
		# --- PUTF cuts
		icf.InDet.NewTracking.maxdImpactPPSSeeds        = 2.0     # loosen cut on PPS seeds
		icf.InDet.NewTracking.maxdImpactSSSSeeds        = 20.0    # apply cut on SSS seeds

	if icf.InDet.cutLevel >= 5:
		# --- PUTF cuts
		icf.InDet.NewTracking.seedFilterLevel           = 2       # increased seed filter level

	if icf.InDet.cutLevel >= 6:
		# --- stop using TRT outlies from failed extension fits to create BackTracks or TRT Only
		icf.InDet.NewTracking.excludeUsedTRToutliers    = True             # TRT outliers are added to the exclusion list
		# --- TRT only cuts
		icf.InDet.NewTracking.minTRTonlyPt              = 2.0 * Units.GeV  # increase Pt cut for TRT only to the value of egamma for 1 track conversions
		icf.InDet.NewTracking.useTRTonlyOldLogic        = False            # turn off ole overlap logic to reduce number of hits
		# icf.InDet.NewTracking.maxTRTonlyShared          = 0.2              # reduce number of shared hits

	if icf.InDet.cutLevel >= 7:
		# --- more BackTracking cuts
		icf.InDet.NewTracking.minSecondaryTRTonTrk      = 15               # let's not allow for short overlap tracks
		icf.InDet.NewTracking.maxSecondaryHoles         = 1                # tighten hole cuts
		icf.InDet.NewTracking.maxSecondaryPixelHoles    = 1                # tighten hole cuts
		icf.InDet.NewTracking.maxSecondarySCTHoles      = 1                # tighten hole cuts
		icf.InDet.NewTracking.maxSecondaryDoubleHoles   = 0                # tighten hole cuts
		icf.InDet.NewTracking.minSecondaryTRTPrecFrac   = 0.3              # default for all tracking now, as well for BackTracking
		icf.InDet.NewTracking.rejectShortExtensions     = True             # fall back onto segment if TRT extension is short
		icf.InDet.NewTracking.SiExtensionCuts           = True             # use cuts from ambi scoring already early
		# icf.InDet.NewTracking.maxSecondaryTRTShared     = 0.2              # tighen shared hit cut for segment maker ?

	if icf.InDet.cutLevel >= 8:
		# --- slightly tighen NewTracking cuts
		icf.InDet.NewTracking.maxHoles                  = 2                # was 3
		icf.InDet.NewTracking.maxPixelHoles             = 1                # was 2


	if icf.InDet.cutLevel >= 9:
		# --- tighten maxZ for the IP parameter
		icf.InDet.NewTracking.maxZImpact              = 250 * Units.mm

	if icf.InDet.cutLevel >= 10:
		# --- turn on Z Boundary seeding
		icf.InDet.NewTracking.doZBoundary              = True

	if icf.InDet.cutLevel >= 11:
		# --- turn on eta dependent cuts
		icf.InDet.NewTracking.useTRTonlyParamCuts      = True

	if icf.InDet.cutLevel >= 12:
		# --- Tighten track reconstruction criteria
		icf.InDet.NewTracking.Xi2max                  = 9.0  # was 15
		icf.InDet.NewTracking.Xi2maxNoAdd             = 25.0 # was 35
		icf.InDet.NewTracking.nHolesMax               = 2 # was 3
		icf.InDet.NewTracking.nHolesGapMax            = 2 # was 3

	if icf.InDet.cutLevel >= 13 and DetFlags.detdescr.Calo_allOn():
		# --- turn on RoI seeded for Back Tracking and TRT only
		icf.InDet.NewTracking.RoISeededBackTracking   = True

	if icf.InDet.cutLevel >= 14 :
		icf.InDet.NewTracking.minPT                   = 0.5 * Units.GeV

	if icf.InDet.cutLevel >= 15 :
		icf.InDet.NewTracking.minClusters             = 8 #based on studies by R.Jansky

	if icf.InDet.cutLevel >= 16 :
		icf.InDet.NewTracking.maxPrimaryImpact        = 5.0 * Units.mm #based on studies by T.Strebler

	if icf.InDet.cutLevel >= 17:
		print('--------> FATAL ERROR, cut level undefined, abort !')
		import sys
		sys.exit()

	# --------------------------------------
	# --- now the overwrites for special setups
	# --------------------------------------

	# --- do robust reconstruction

	if icf.InDet.doRobustReco:
		# ---- new tracking
		icf.InDet.NewTracking.minClusters             = 7                # Igor 6, was 7
		icf.InDet.NewTracking.maxHoles                = 5                # was 5
		 icf.InDet.NewTracking.maxPixelHoles           = 2                # was 5
		icf.InDet.NewTracking.maxSctHoles             = 5                # was 5
		icf.InDet.NewTracking.maxDoubleHoles          = 4                # was 2
		icf.InDet.NewTracking.maxZImpact              = 500.0 * Units.mm
		# ---- back tracking
		icf.InDet.NewTracking.maxSecondaryHoles       = 5
		icf.InDet.NewTracking.maxSecondaryPixelHoles  = 5
		icf.InDet.NewTracking.maxSecondarySCTHoles    = 5
		icf.InDet.NewTracking.maxSecondaryDoubleHoles = 2

	if icf.InDet.doInnerDetectorCommissioning:
		icf.InDet.NewTracking.minClusters             = 6
		icf.InDet.NewTracking.nWeightedClustersMin    = 6
		icf.InDet.NewTracking.minSiNotShared   = 5
		icf.InDet.NewTracking.rejectShortExtensions = False

def setTrackingCuts(icf, mode):
	# --- SLHC setup
	if mode == "SLHC":
		icf.InDet.NewTracking.extension               = "SLHC"
		# --- higher pt cut and impact parameter cut
		icf.InDet.NewTracking.minPT                   = 0.9 * Units.GeV
		icf.InDet.NewTracking.maxPrimaryImpact        = 2.0 * Units.mm # highlumi
		icf.InDet.NewTracking.maxZImpact              = 250.0 * Units.mm

		# --- cluster cuts
		icf.InDet.NewTracking.minClusters             = 9
		icf.InDet.NewTracking.minSiNotShared          = 8
		#icf.InDet.NewTracking.maxShared               = 3 # cut is now on number of shared modules
		#icf.InDet.NewTracking.maxHoles                = 3
		#icf.InDet.NewTracking.maxPixelHoles           = D2
		#icf.InDet.NewTracking.maxSctHoles             = 2
		#icf.InDet.NewTracking.maxDoubleHoles          = 2
		# --- also tighten pattern cuts
		icf.InDet.NewTracking.radMax                  = 1000. * Units.mm
		#icf.InDet.NewTracking.seedFilterLevel         = 1
		#icf.InDet.NewTracking.nHolesMax               = icf.InDet.NewTracking.maxHoles
		#icf.InDet.NewTracking.nHolesGapMax            = icf.InDet.NewTracking.maxHoles
		#icf.InDet.NewTracking.Xi2max                  = 15.0
		#icf.InDet.NewTracking.Xi2maxNoAdd             = 35.0
		#icf.InDet.NewTracking.nWeightedClustersMin    = icf.InDet.NewTracking.minClusters-1

	# --- IBL setup
	if mode == "IBL" :
		icf.InDet.NewTracking.extension               = "IBL"
		icf.InDet.NewTracking.seedFilterLevel         = 1
		icf.InDet.NewTracking.minPT                   = 0.900 * Units.GeV
		icf.InDet.NewTracking.minClusters             = 10
		icf.InDet.NewTracking.maxPixelHoles           = 1

	# --- High pile-up setup
	if mode == "HighPileup" :
		icf.InDet.NewTracking.extension               = "HighPileup"
		icf.InDet.NewTracking.seedFilterLevel         = 1
		icf.InDet.NewTracking.minPT                   = 0.900 * Units.GeV
		icf.InDet.NewTracking.minClusters             = 9
		icf.InDet.NewTracking.maxPixelHoles           = 0

	# --- mode for min bias, commissioning or doRobustReco
	if mode == 'MinBias' or self.__indetflags.doRobustReco():
		if icf.InDet.doHIP300:
		icf.InDet.NewTracking.minPT                     = 0.300 * Units.GeV
		else:
		icf.InDet.NewTracking.minPT                     = 0.100 * Units.GeV
		icf.InDet.NewTracking.minClusters               = 5
		icf.InDet.NewTracking.minSecondaryPt            = 0.4 * Units.GeV  # Pt cut for back tracking + segment finding for these
		icf.InDet.NewTracking.minTRTonlyPt              = 0.4 * Units.GeV  # Pt cut for TRT only
		icf.InDet.NewTracking.TRTSegFinderPtBins        = 50
		icf.InDet.NewTracking.maxdImpactSSSSeeds        = 20.0    # apply cut on SSS seeds
		icf.InDet.NewTracking.excludeUsedTRToutliers    = False   # TRT outliers are added to the exclusion list
		icf.InDet.NewTracking.useTRTonlyOldLogic        = True    # turn off ole overlap logic to reduce number of hits
		icf.InDet.NewTracking.maxSecondaryImpact        = 100.0 * Units.mm # low lumi

	# --- mode for high-d0 tracks
	if mode == "LargeD0":
		icf.InDet.NewTracking.extension          = "LargeD0" # this runs parallel to NewTracking
		icf.InDet.NewTracking.maxPT              = 1.0 * Units.TeV
		icf.InDet.NewTracking.minPT              = 900 * Units.MeV
		icf.InDet.NewTracking.maxEta             = 5
		icf.InDet.NewTracking.maxPrimaryImpact   = 300.0 * Units.mm
		icf.InDet.NewTracking.maxZImpact         = 1500.0 * Units.mm
		icf.InDet.NewTracking.maxSecondaryImpact = 300.0 * Units.mm
		icf.InDet.NewTracking.minSecondaryPt     = 500.0 * Units.MeV
		icf.InDet.NewTracking.minClusters        = 7
		icf.InDet.NewTracking.minSiNotShared     = 5
		icf.InDet.NewTracking.maxShared          = 2   # cut is now on number of shared modules
		icf.InDet.NewTracking.minPixel           = 0
		icf.InDet.NewTracking.maxHoles           = 2
		icf.InDet.NewTracking.maxPixelHoles      = 1
		icf.InDet.NewTracking.maxSctHoles        = 2
		icf.InDet.NewTracking.maxDoubleHoles     = 1
		icf.InDet.NewTracking.radMax             = 600. * Units.mm
		icf.InDet.NewTracking.nHolesMax          = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.nHolesGapMax       = InDet.NewTracking.maxHoles # not as tight as 2*maxDoubleHoles
		icf.InDet.NewTracking.seedFilterLevel   = 1
		icf.InDet.NewTracking.maxTracksPerSharedPRD = 2

	# --- mode for high-d0 tracks (re-optimisation for Run 2 by M.Danninger)
	if mode == "R3LargeD0":
		icf.InDet.NewTracking.extension          = "R3LargeD0" # this runs parallel to NewTracking                             
		icf.InDet.NewTracking.maxPT              = 1.0 * Units.TeV
		icf.InDet.NewTracking.minPT              = 1.0 * Units.GeV                                                                                    
		icf.InDet.NewTracking.maxEta             = 3                                                                                                        
		icf.InDet.NewTracking.maxPrimaryImpact   = 300.0 * Units.mm
		icf.InDet.NewTracking.maxZImpact         = 750 * Units.mm    
		icf.InDet.NewTracking.maxSecondaryImpact = 300.0 * Units.mm  
		icf.InDet.NewTracking.minSecondaryPt     = 1000.0 * Units.MeV 
		icf.InDet.NewTracking.minClusters        = 8                  
		icf.InDet.NewTracking.minSiNotShared     = 6                 
		icf.InDet.NewTracking.maxShared          = 2   # cut is now on number of shared modules                                                                                  
		icf.InDet.NewTracking.minPixel           = 0
		icf.InDet.NewTracking.maxHoles           = 2
		icf.InDet.NewTracking.maxPixelHoles      = 1
		icf.InDet.NewTracking.maxSctHoles        = 1  
		icf.InDet.NewTracking.maxDoubleHoles     = 0  
		icf.InDet.NewTracking.radMax             = 600. * Units.mm
		icf.InDet.NewTracking.nHolesMax          = icf.InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.nHolesGapMax       = 1 
		icf.InDet.NewTracking.seedFilterLevel    = 1  
		icf.InDet.NewTracking.maxTracksPerSharedPRD   = 2
		icf.InDet.NewTracking.Xi2max                  = 9.0  
		icf.InDet.NewTracking.Xi2maxNoAdd             = 25.0 
		icf.InDet.NewTracking.roadWidth               = 10. 
		icf.InDet.NewTracking.nWeightedClustersMin    = 8   
		icf.InDet.NewTracking.maxdImpactSSSSeeds      = 300.0
		icf.InDet.NewTracking.doZBoundary             = True

	# --- mode for high-d0 tracks down to 100 MeV (minPT, minClusters, minSecondaryPt cuts loosened to MinBias level)
	if mode == "LowPtLargeD0":
		icf.InDet.NewTracking.extension          = "LowPtLargeD0" # this runs parallel to NewTracking
		icf.InDet.NewTracking.maxPT              = 1.0 * Units.TeV
		icf.InDet.NewTracking.minPT              = 100 * Units.MeV
		icf.InDet.NewTracking.maxEta             = 5
		icf.InDet.NewTracking.maxPrimaryImpact   = 300.0 * Units.mm
		icf.InDet.NewTracking.maxZImpact         = 1500.0 * Units.mm
		icf.InDet.NewTracking.maxSecondaryImpact = 300.0 * Units.mm
		icf.InDet.NewTracking.minSecondaryPt     = 400.0 * Units.MeV
		icf.InDet.NewTracking.minClusters        = 5
		icf.InDet.NewTracking.minSiNotShared     = 5
		icf.InDet.NewTracking.maxShared          = 2   # cut is now on number of shared modules
		icf.InDet.NewTracking.minPixel           = 0
		icf.InDet.NewTracking.maxHoles           = 2
		icf.InDet.NewTracking.maxPixelHoles      = 1
		icf.InDet.NewTracking.maxSctHoles        = 2
		icf.InDet.NewTracking.maxDoubleHoles     = 1
		icf.InDet.NewTracking.radMax             = 600. * Units.mm
		icf.InDet.NewTracking.nHolesMax          = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.nHolesGapMax       = InDet.NewTracking.maxHoles # not as tight as 2*maxDoubleHoles
		icf.InDet.NewTracking.seedFilterLevel   = 1
		icf.InDet.NewTracking.maxTracksPerSharedPRD = 2

	# --- change defaults for low pt tracking
	if mode == "LowPt":
		icf.InDet.NewTracking.extension        = "LowPt" # this runs parallel to NewTracking
		icf.InDet.NewTracking.maxPT            = InDet.NewTracking.minPT + 0.3 * Units.GeV # some overlap
		icf.InDet.NewTracking.minPT            = 0.050 * Units.GeV
		icf.InDet.NewTracking.minClusters      = 5
		icf.InDet.NewTracking.minSiNotShared   = 4
		icf.InDet.NewTracking.maxShared        = 1   # cut is now on number of shared modules
		icf.InDet.NewTracking.minPixel         = 2   # At least one pixel hit for low-pt (assoc. seeded on pixels!)
		icf.InDet.NewTracking.maxHoles         = 2
		icf.InDet.NewTracking.maxPixelHoles    = 1
		icf.InDet.NewTracking.maxSctHoles      = 2
		icf.InDet.NewTracking.maxDoubleHoles   = 1
		icf.InDet.NewTracking.radMax           = 600. * Units.mm
		icf.InDet.NewTracking.nHolesMax        = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.nHolesGapMax     = InDet.NewTracking.maxHoles # not as tight as 2*maxDoubleHoles

		if icf.InDet.doMinBias:
			icf.InDet.NewTracking.maxPT            = 1000000 * Units.GeV # Won't accept None *NEEDS FIXING*
			icf.InDet.NewTracking.maxPrimaryImpact = 100.0 * Units.mm

	if mode == "SLHCConversionFinding":
		icf.InDet.NewTracking.extension        = "SLHCConversionFinding" # this runs parallel to NewTracking
		icf.InDet.NewTracking.minPT                   = 0.9 * Units.GeV
		icf.InDet.NewTracking.maxPrimaryImpact        = 10.0 * Units.mm
		icf.InDet.NewTracking.maxZImpact              = 150.0 * Units.mm
		icf.InDet.NewTracking.minClusters             = 6
		icf.InDet.NewTracking.minSiNotShared          = 4
		#icf.InDet.NewTracking.maxShared               = 3
		icf.InDet.NewTracking.maxHoles                = 0
		#icf.InDet.NewTracking.maxPixelHoles           = D2
		#icf.InDet.NewTracking.maxSctHoles             = 2
		#icf.InDet.NewTracking.maxDoubleHoles          = 2
		# --- also tighten pattern cuts
		icf.InDet.NewTracking.radMax                  = 1000. * Units.mm
		icf.InDet.NewTracking.radMin                  = 0. * Units.mm # not turn on this cut for now
		#icf.InDet.NewTracking.seedFilterLevel         = 1
		#icf.InDet.NewTracking.nHolesMax               = InDet.NewTracking.maxHoles
		#icf.InDet.NewTracking.nHolesGapMax            = InDet.NewTracking.maxHoles
		#icf.InDet.NewTracking.Xi2max                  = 15.0
		#icf.InDet.NewTracking.Xi2maxNoAdd             = 35.0
		#icf.InDet.NewTracking.nWeightedClustersMin    = InDet.NewTracking.minClusters-1
		# --- turn on Z Boundary seeding
		icf.InDet.NewTracking.doZBoundary              = False #


	# --- change defaults for very low pt tracking
	if mode == "VeryLowPt":
		icf.InDet.NewTracking.extension        = "VeryLowPt" # this runs parallel to NewTracking
		icf.InDet.NewTracking.maxPT            = InDet.NewTracking.minPT + 0.3 * Units.GeV # some overlap
		icf.InDet.NewTracking.minPT            = 0.050 * Units.GeV
		icf.InDet.NewTracking.minClusters      = 3
		icf.InDet.NewTracking.minSiNotShared   = 3
		icf.InDet.NewTracking.maxShared        = 1   # cut is now on number of shared modules
		icf.InDet.NewTracking.minPixel         = 3   # At least one pixel hit for low-pt (assoc. seeded on pixels!)
		icf.InDet.NewTracking.maxHoles         = 1
		icf.InDet.NewTracking.maxPixelHoles    = 1
		icf.InDet.NewTracking.maxSctHoles      = 1
		icf.InDet.NewTracking.maxDoubleHoles   = 0
		icf.InDet.NewTracking.nHolesMax        = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.nHolesGapMax     = InDet.NewTracking.maxHoles # not as tight as 2*maxDoubleHoles
		icf.InDet.NewTracking.radMax           = 600. * Units.mm # restrivt to pixels

		if icf.InDet.doMinBias:
			icf.InDet.NewTracking.maxPT            = 100000 * Units.GeV # Won't accept None *NEEDS FIXING*

	# --- change defaults for forward muon tracking
	if mode == "ForwardTracks":
		icf.InDet.NewTracking.extension        = "ForwardTracks" # this runs parallel to NewTracking
		icf.InDet.NewTracking.minEta           = 2.4 # restrict to minimal eta
		icf.InDet.NewTracking.maxEta           = 2.7
		icf.InDet.NewTracking.minPT            = 2 * Units.GeV
		icf.InDet.NewTracking.minClusters      = 3
		icf.InDet.NewTracking.minSiNotShared   = 3
		icf.InDet.NewTracking.maxShared        = 1
		icf.InDet.NewTracking.minPixel         = 3
		icf.InDet.NewTracking.maxHoles         = 1
		icf.InDet.NewTracking.maxPixelHoles    = 1
		icf.InDet.NewTracking.maxSctHoles      = 1
		icf.InDet.NewTracking.maxDoubleHoles   = 0
		icf.InDet.NewTracking.nHolesMax        = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.nHolesGapMax     = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.radMax           = 600. * Units.mm
		icf.InDet.NewTracking.useTRT           = False # no TRT for forward tracks


	if mode == "ForwardSLHCTracks":
		icf.InDet.NewTracking.extension        = "ForwardSLHCTracks" # this runs parallel to NewTracking
		icf.InDet.NewTracking.minEta           = 2.4 # restrict to minimal eta
		icf.InDet.NewTracking.maxEta           = 3.0
		icf.InDet.NewTracking.minPT            = 0.9 * Units.GeV
		icf.InDet.NewTracking.minClusters      = 5
		icf.InDet.NewTracking.minSiNotShared   = 3
		icf.InDet.NewTracking.maxShared        = 1
		icf.InDet.NewTracking.minPixel         = 3
		icf.InDet.NewTracking.maxHoles         = 1
		icf.InDet.NewTracking.maxPixelHoles    = 1
		icf.InDet.NewTracking.maxSctHoles      = 1
		icf.InDet.NewTracking.maxDoubleHoles   = 0
		icf.InDet.NewTracking.nHolesMax        = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.nHolesGapMax     = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.radMax           = 600. * Units.mm
		icf.InDet.NewTracking.useTRT           = False # no TRT for forward tracks

	if mode == "VeryForwardSLHCTracks":
		icf.InDet.NewTracking.extension        = "VeryForwardSLHCTracks" # this runs parallel to NewTracking
		icf.InDet.NewTracking.minEta           = 2.4 # restrict to minimal eta
		icf.InDet.NewTracking.maxEta           = 4.0
		icf.InDet.NewTracking.minPT            = 0.9 * Units.GeV
		icf.InDet.NewTracking.minClusters      = 5
		icf.InDet.NewTracking.minSiNotShared   = 3
		icf.InDet.NewTracking.maxShared        = 1
		icf.InDet.NewTracking.minPixel         = 3
		icf.InDet.NewTracking.maxHoles         = 1
		icf.InDet.NewTracking.maxPixelHoles    = 1
		icf.InDet.NewTracking.maxSctHoles      = 0
		icf.InDet.NewTracking.maxDoubleHoles   = 0
		icf.InDet.NewTracking.nHolesMax        = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.nHolesGapMax     = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.radMax           = 600. * Units.mm
		icf.InDet.NewTracking.useTRT           = False # no TRT for forward tracks

	# --- change defauls for beam gas tracking
	if mode == "BeamGas":
		icf.InDet.NewTracking.extension        = "BeamGas" # this runs parallel to NewTracking
		icf.InDet.NewTracking.minPT            = 0.500 * Units.GeV
		icf.InDet.NewTracking.maxPrimaryImpact = 300. * Units.mm
		icf.InDet.NewTracking.maxZImpact       = 2000. * Units.mm
		icf.InDet.NewTracking.minClusters      = 6
		icf.InDet.NewTracking.maxHoles         = 3
		icf.InDet.NewTracking.maxPixelHoles    = 3
		icf.InDet.NewTracking.maxSctHoles      = 3
		icf.InDet.NewTracking.maxDoubleHoles   = 1
		icf.InDet.NewTracking.nHolesMax        = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.nHolesGapMax     = InDet.NewTracking.maxHoles # not as tight as 2*maxDoubleHoles

	# --- setup for lumi determination based on vertices
	if mode == "VtxLumi" :
		icf.InDet.NewTracking.extension               = "VtxLumi"
		icf.InDet.NewTracking.seedFilterLevel         = 1
		icf.InDet.NewTracking.minPT                   = 0.900 * Units.GeV
		icf.InDet.NewTracking.minClusters             = 7
		icf.InDet.NewTracking.maxPixelHoles           = 1
		icf.InDet.NewTracking.radMax                  = 600. * Units.mm
		icf.InDet.NewTracking.nHolesMax               = 2
		icf.InDet.NewTracking.nHolesGapMax            = 1
		icf.InDet.NewTracking.useTRT                  = False

	# --- setup for beamspot determination based on vertices
	if mode == "VtxBeamSpot" :
		icf.InDet.NewTracking.extension               = "VtxBeamSpot"
		icf.InDet.NewTracking.seedFilterLevel         = 1
		icf.InDet.NewTracking.minPT                   = 0.900 * Units.GeV
		icf.InDet.NewTracking.minClusters             = 9
		icf.InDet.NewTracking.maxPixelHoles           = 0
		icf.InDet.NewTracking.radMax                  = 320. * Units.mm
		icf.InDet.NewTracking.nHolesMax               = 2
		icf.InDet.NewTracking.nHolesGapMax            = 1
		icf.InDet.NewTracking.useTRT                  = False

	# --- changes for cosmics
	if mode == "Cosmics":
		icf.InDet.NewTracking.minPT            = 0.500 * Units.GeV
		icf.InDet.NewTracking.maxPrimaryImpact = 1000. * Units.mm
		icf.InDet.NewTracking.maxZImpact       = 10000. * Units.mm
		icf.InDet.NewTracking.minClusters      = 4
		icf.InDet.NewTracking.minSiNotShared   = 4
		icf.InDet.NewTracking.maxHoles         = 3
		icf.InDet.NewTracking.maxPixelHoles    = 3
		icf.InDet.NewTracking.maxSctHoles      = 3
		icf.InDet.NewTracking.maxDoubleHoles   = 1
		icf.InDet.NewTracking.minTRTonTrk      = 15
		icf.InDet.NewTracking.minTRTOnly       = 15
		icf.InDet.NewTracking.roadWidth        = 60.
		icf.InDet.NewTracking.seedFilterLevel  = 3
		icf.InDet.NewTracking.Xi2max           = 60.0
		icf.InDet.NewTracking.Xi2maxNoAdd      = 100.0
		icf.InDet.NewTracking.nWeightedClustersMin = 8
		icf.InDet.NewTracking.nHolesMax        = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.nHolesGapMax     = InDet.NewTracking.maxHoles # not as tight as 2*maxDoubleHoles

	# --- changes for heavy ion
	if mode == "HeavyIon":
		icf.InDet.NewTracking.maxZImpact       = 200. * Units.mm
		icf.InDet.NewTracking.minPT            = 0.500 * Units.GeV
		icf.InDet.NewTracking.minClusters      = 9
		icf.InDet.NewTracking.minSiNotShared   = 7
		icf.InDet.NewTracking.maxShared        = 2 # was 1, cut is now on number of shared modules
		icf.InDet.NewTracking.maxHoles         = 0
		icf.InDet.NewTracking.maxPixelHoles    = 0
		icf.InDet.NewTracking.maxSctHoles      = 0
		icf.InDet.NewTracking.maxDoubleHoles   = 0
		icf.InDet.NewTracking.nHolesMax        = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.nHolesGapMax     = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.Xi2max           = 6.
		icf.InDet.NewTracking.Xi2maxNoAdd      = 10.
		if self.__indetflags.cutLevel() == 1:
			icf.InDet.NewTracking.seedFilterLevel  = 1
		elif self.__indetflags.cutLevel() == 2:
			icf.InDet.NewTracking.seedFilterLevel  = 2
			icf.InDet.NewTracking.maxdImpactSSSSeeds        = 20.0 # apply cut on SSS seeds
		elif self.__indetflags.cutLevel() == 3: # This is for MB data
			icf.InDet.NewTracking.minPT            = 0.300 * Units.GeV
			icf.InDet.NewTracking.seedFilterLevel  = 2
			icf.InDet.NewTracking.maxdImpactSSSSeeds        = 20.0 # apply cut on SSS seeds
			icf.InDet.NewTracking.useParameterizedTRTCuts   = False #Make these false on all HI cut levels >=3, since standard cut levels set it true from levels >=3
			icf.InDet.NewTracking.useNewParameterizationTRT = False
		elif self.__indetflags.cutLevel() == 4: # ==CutLevel 2 with loosened hole cuts and chi^2 cuts
			icf.InDet.NewTracking.seedFilterLevel  = 2
			icf.InDet.NewTracking.maxdImpactSSSSeeds        = 20.0 # apply cut on SSS seeds
			icf.InDet.NewTracking.maxdImpactPPSSeeds      = 1.7 #set this to 1.7 for all HI cut levels >=4, since standard cut levels set it to 2.0 from levels >=4. Not sure it has any effect, since we don't usually run mixed seeds (also true for HI?)
			icf.InDet.NewTracking.useParameterizedTRTCuts   = False
			icf.InDet.NewTracking.useNewParameterizationTRT = False
			icf.InDet.NewTracking.maxHoles               = 2
			icf.InDet.NewTracking.maxPixelHoles       = 1
			icf.InDet.NewTracking.maxSctHoles         = 1
			icf.InDet.NewTracking.maxDoubleHoles   = 0
			icf.InDet.NewTracking.Xi2max                   = 9.
			icf.InDet.NewTracking.Xi2maxNoAdd        = 25.
		elif self.__indetflags.cutLevel() == 5: # ==CutLevel 3 with loosened hole cuts and chi^2 cuts
			icf.InDet.NewTracking.minPT            = 0.300 * Units.GeV
			icf.InDet.NewTracking.seedFilterLevel  = 2
			icf.InDet.NewTracking.maxdImpactSSSSeeds        = 20.0 # apply cut on SSS seeds
			icf.InDet.NewTracking.maxdImpactPPSSeeds      = 1.7
			icf.InDet.NewTracking.useParameterizedTRTCuts   = False
			icf.InDet.NewTracking.useNewParameterizationTRT = False
			icf.InDet.NewTracking.maxHoles               = 2
			icf.InDet.NewTracking.maxPixelHoles       = 1
			icf.InDet.NewTracking.maxSctHoles         = 1
			icf.InDet.NewTracking.maxDoubleHoles   = 0
			icf.InDet.NewTracking.Xi2max                   = 9.
			icf.InDet.NewTracking.Xi2maxNoAdd        = 25.

		icf.InDet.NewTracking.radMax           = 600. * Units.mm # restrict to pixels + first SCT layer
		icf.InDet.NewTracking.useTRT           = False

	# --- changes for Pixel/SCT segments
	from AthenaCommon.DetFlags    import DetFlags
	if ( DetFlags.haveRIO.pixel_on() and not DetFlags.haveRIO.SCT_on() ):
		icf.InDet.NewTracking.minClusters = 3
	elif ( DetFlags.haveRIO.SCT_on() and not DetFlags.haveRIO.pixel_on() ):
		icf.InDet.NewTracking.minClusters = 6

	# --- changes for Pixel segments


	if mode == "Pixel":
		icf.InDet.NewTracking.extension        = "Pixel" # this runs parallel to NewTracking
		icf.InDet.NewTracking.minPT            = 0.1 * Units.GeV
		icf.InDet.NewTracking.minClusters      = 3
		icf.InDet.NewTracking.maxHoles         = 1
		icf.InDet.NewTracking.maxPixelHoles    = 1
		icf.InDet.NewTracking.maxSctHoles      = 0
		icf.InDet.NewTracking.maxDoubleHoles   = 0
		icf.InDet.NewTracking.minSiNotShared   = 3
		icf.InDet.NewTracking.maxShared        = 0
		icf.InDet.NewTracking.seedFilterLevel  = 2
		icf.InDet.NewTracking.nHolesMax        = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.nHolesGapMax     = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.useSCT           = False
		icf.InDet.NewTracking.useTRT           = False

		if icf.InDet.doMinBias:
			if icf.InDet.doHIP300():
				icf.InDet.NewTracking.minPT            = 0.300 * Units.GeV
			else:
				icf.InDet.NewTracking.minPT            = 0.05 * Units.GeV
			icf.InDet.NewTracking.maxPT            = 100000 * Units.GeV # Won't accept None *NEEDS FIXING*

		if icf.InDet.doHeavyIon:
			icf.InDet.NewTracking.minPT            = 0.1 * Units.GeV
			icf.InDet.NewTracking.maxHoles         = 0
			icf.InDet.NewTracking.maxPixelHoles    = 0
			icf.InDet.NewTracking.minSiNotShared   = 3
			icf.InDet.NewTracking.maxShared        = 0
			icf.InDet.NewTracking.seedFilterLevel  = 2
			icf.InDet.NewTracking.nHolesMax        = InDet.NewTracking.maxHoles
			icf.InDet.NewTracking.nHolesGapMax     = InDet.NewTracking.maxHoles
			icf.InDet.NewTracking.useSCT           = False
			icf.InDet.NewTracking.useTRT           = False

		if icf.InDet.doCosmics: ##fix to beamtype
			icf.InDet.NewTracking.minPT            = 0.500 * Units.GeV
			icf.InDet.NewTracking.maxPrimaryImpact = 1000. * Units.mm
			icf.InDet.NewTracking.maxZImpact       = 10000. * Units.mm
			icf.InDet.NewTracking.maxHoles         = 3
			icf.InDet.NewTracking.maxPixelHoles    = 3
			icf.InDet.NewTracking.maxShared        = 0    # no shared hits in cosmics
			icf.InDet.NewTracking.roadWidth        = 60.
			icf.InDet.NewTracking.seedFilterLevel  = 3 # 2 ?
			icf.InDet.NewTracking.nHolesMax        = InDet.NewTracking.maxHoles
			icf.InDet.NewTracking.nHolesGapMax     = InDet.NewTracking.maxHoles
			icf.InDet.NewTracking.Xi2max           = 60.0
			icf.InDet.NewTracking.Xi2maxNoAdd      = 100.0
			icf.InDet.NewTracking.nWeightedClustersMin = 6


	if mode == "Disappearing":
		icf.InDet.NewTracking.extension        = "Disappearing" # this runs after NewTracking
		icf.InDet.NewTracking.minPT            = 5.0 * Units.GeV
		icf.InDet.NewTracking.minClusters      = 4
		icf.InDet.NewTracking.maxHoles         = 0
		icf.InDet.NewTracking.maxPixelHoles    = 0
		icf.InDet.NewTracking.maxSctHoles      = 0
		icf.InDet.NewTracking.maxDoubleHoles   = 0
		icf.InDet.NewTracking.minSiNotShared   = 3
		icf.InDet.NewTracking.maxShared        = 0
		icf.InDet.NewTracking.seedFilterLevel  = 2
		icf.InDet.NewTracking.nHolesMax        = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.nHolesGapMax     = InDet.NewTracking.maxHoles
		# icf.InDet.NewTracking.useSCT           = False
		icf.InDet.NewTracking.useSCT           = True
		# icf.InDet.NewTracking.useTRT           = False
		icf.InDet.NewTracking.useTRT           = True
		icf.InDet.NewTracking.useSCTSeeding    = False
		icf.InDet.NewTracking.maxEta           = 2.2


	# --- changes for SCT segments
	if mode == "SCT":
		icf.InDet.NewTracking.extension        = "SCT" # this runs parallel to NewTracking
		icf.InDet.NewTracking.minPT            = 0.1 * Units.GeV
		icf.InDet.NewTracking.minClusters      = 7
		icf.InDet.NewTracking.maxHoles         = 2
		icf.InDet.NewTracking.maxPixelHoles    = 0
		icf.InDet.NewTracking.maxSctHoles      = 2
		icf.InDet.NewTracking.maxDoubleHoles   = 1
		icf.InDet.NewTracking.minSiNotShared   = 5
		icf.InDet.NewTracking.maxShared        = 0
		icf.InDet.NewTracking.seedFilterLevel  = 2
		icf.InDet.NewTracking.nHolesMax        = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.nHolesGapMax     = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.usePixel         = False
		icf.InDet.NewTracking.useTRT           = False

		if icf.InDet.doMinBias:
			if icf.InDet.doHIP300:
				icf.InDet.NewTracking.minPT            = 0.3 * Units.GeV
			else:
				icf.InDet.NewTracking.minPT            = 0.1 * Units.GeV

		if icf.InDet.doCosmics: ##fix to beamtype
			icf.InDet.NewTracking.minPT            = 0.500 * Units.GeV
			icf.InDet.NewTracking.maxPrimaryImpact = 1000. * Units.mm
			icf.InDet.NewTracking.maxZImpact       = 10000. * Units.mm
			icf.InDet.NewTracking.maxHoles         = 3
			icf.InDet.NewTracking.maxPixelHoles    = 0
			icf.InDet.NewTracking.maxSctHoles      = 3
			icf.InDet.NewTracking.maxShared        = 0   # no shared hits in cosmics
			icf.InDet.NewTracking.roadWidth        = 60.
			icf.InDet.NewTracking.seedFilterLevel  = 3 # 2 ?
			icf.InDet.NewTracking.nHolesMax        = InDet.NewTracking.maxHoles
			icf.InDet.NewTracking.nHolesGapMax     = InDet.NewTracking.maxHoles
			icf.InDet.NewTracking.Xi2max           = 60.0
			icf.InDet.NewTracking.Xi2maxNoAdd      = 100.0
			icf.InDet.NewTracking.nWeightedClustersMin = 6

			if icf.InDet.doInnerDetectorCommissioning:
				icf.InDet.NewTracking.minClusters      = 4
				icf.InDet.NewTracking.minSiNotShared   = 4
				icf.InDet.NewTracking.nWeightedClustersMin = 4

	# --- TRT subdetector tracklet cuts
	if mode == "TRT":
		icf.InDet.NewTracking.minPT                   = 0.4 * Units.GeV
		icf.InDet.NewTracking.minTRTonly              = 15
		icf.InDet.NewTracking.maxTRTonlyShared        = 0.7

	# --- mode for SCT and TRT
	if mode == "SCTandTRT":
		icf.InDet.NewTracking.extension        = "SCTandTRT" # this runs parallel to NewTracking
		icf.InDet.NewTracking.minPT            = 0.4 * Units.GeV
		icf.InDet.NewTracking.minClusters      = 7
		icf.InDet.NewTracking.maxHoles         = 2
		icf.InDet.NewTracking.maxPixelHoles    = 0
		icf.InDet.NewTracking.maxSctHoles      = 2
		icf.InDet.NewTracking.maxDoubleHoles   = 0
		icf.InDet.NewTracking.minSiNotShared   = 5
		icf.InDet.NewTracking.maxShared        = 0
		icf.InDet.NewTracking.seedFilterLevel  = 2
		icf.InDet.NewTracking.nHolesMax        = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.nHolesGapMax     = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.usePixel         = False
		icf.InDet.NewTracking.useTRT           = True

		if icf.InDet.doCosmics: ##fix to beamtype
			icf.InDet.NewTracking.minPT            = 0.500 * Units.GeV
			icf.InDet.NewTracking.maxPrimaryImpact = 1000. * Units.mm
			icf.InDet.NewTracking.maxZImpact       = 10000. * Units.mm
			icf.InDet.NewTracking.maxHoles         = 3
			icf.InDet.NewTracking.maxPixelHoles    = 0
			icf.InDet.NewTracking.maxSctHoles      = 3
			icf.InDet.NewTracking.maxShared        = 0   # no shared hits in cosmics
			icf.InDet.NewTracking.roadWidth        = 60.
			icf.InDet.NewTracking.seedFilterLevel  = 3 # 2 ?
			icf.InDet.NewTracking.nHolesMax        = InDet.NewTracking.maxHoles
			icf.InDet.NewTracking.nHolesGapMax     = InDet.NewTracking.maxHoles
			icf.InDet.NewTracking.Xi2max           = 60.0
			icf.InDet.NewTracking.Xi2maxNoAdd      = 100.0
			icf.InDet.NewTracking.nWeightedClustersMin = 6

			if icf.InDet.doInnerDetectorCommissioning:
				icf.InDet.NewTracking.minClusters      = 4
				icf.InDet.NewTracking.nWeightedClustersMin = 4
				icf.InDet.NewTracking.minSiNotShared   = 4
				icf.InDet.NewTracking.rejectShortExtensions     = False

	if mode == "DBM":
		icf.InDet.NewTracking.extension               = "DBM"
		icf.InDet.NewTracking.minEta                  = 3.05
		icf.InDet.NewTracking.maxEta                  = 3.45
		icf.InDet.NewTracking.Xi2maxNoAdd             = 10000
		icf.InDet.NewTracking.Xi2max                  = 10000
		icf.InDet.NewTracking.nWeightedClustersMin    = 0
		icf.InDet.NewTracking.seedFilterLevel         = 1
		icf.InDet.NewTracking.maxdImpactPPSSeeds      = 100000.0 * Units.mm
		icf.InDet.NewTracking.maxdImpactSSSSeeds      = 100000.0 * Units.mm
		icf.InDet.NewTracking.maxPrimaryImpact        = 100000.0 * Units.mm  # low lumi
		icf.InDet.NewTracking.maxZImpact              = 320000.0 * Units.mm  # Was 250 mm
		icf.InDet.NewTracking.maxPT            = 100000.0 * Units.GeV # some overlap
		icf.InDet.NewTracking.minPT            = 0.0 * Units.GeV
		icf.InDet.NewTracking.minClusters      = 0
		icf.InDet.NewTracking.minSiNotShared   = 0
		icf.InDet.NewTracking.maxShared        = 1000   # cut is now on number of shared modules
		icf.InDet.NewTracking.minPixel         = 0
		icf.InDet.NewTracking.maxHoles         = 0
		icf.InDet.NewTracking.maxPixelHoles    = 0
		icf.InDet.NewTracking.maxSctHoles      = 0
		icf.InDet.NewTracking.maxDoubleHoles   = 0
		icf.InDet.NewTracking.radMax           = 600000. * Units.mm
		icf.InDet.NewTracking.nHolesMax        = InDet.NewTracking.maxHoles
		icf.InDet.NewTracking.nHolesGapMax     = InDet.NewTracking.maxHoles # not as tight as 2*maxDoubleHoles
		icf.InDet.NewTracking.useTRT           = False
		icf.InDet.NewTracking.useSCT           = False
		icf.InDet.NewTracking.usePixel         = True


#        elif rec.Commissioning():
#        icf.InDet.NewTracking.minClusters             = 7               # Igor 6, was 7
#        icf.InDet.NewTracking.maxHoles                = 5               # was 5
#        icf.InDet.NewTracking.maxSctHoles             = 5               # was 5
#        icf.InDet.NewTracking.maxDoubleHoles          = 4               # was 2
#        icf.InDet.NewTracking.maxPrimaryImpact        = 50.0 * Units.mm # low lumi
#        icf.InDet.NewTracking.maxZImpact              = 500.0 * Units.mm

# ----------------------------------------------------------------------------
# --- private method
  def __set_indetflags(self):
    from InDetRecExample.InDetJobProperties import InDetFlags
    self.__indetflags = InDetFlags
