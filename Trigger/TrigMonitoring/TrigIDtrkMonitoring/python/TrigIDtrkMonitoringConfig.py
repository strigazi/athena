# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def TrigIDtrkMonitoringTool():
	from AthenaCommon.Constants import INFO,ERROR,FALSE,TRUE,DEBUG,VERBOSE

	# dataTypes: userDefined = 0, monteCarlo, collisions, cosmics
	if not 'DQMonFlags' in dir():
		from AthenaMonitoring.DQMonFlags import DQMonFlags
	dataType = DQMonFlags.monManDataType()

	from AthenaCommon.AppMgr import ToolSvc

	list = []

	if not 'rec' in dir():
		from RecExConfig.RecFlags  import rec


#	if rec.doInDet:
	if True:
		# the old DumpTool has been removed, the old TIDAMonTool code has
		# been moved to TrigInDetAnalysisExample/TrigTestBase
		# from TrigIDtrkMonitoring.TrigIDtrkMonitoringConf import TIDAMonTool
		

		# from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConfig import TrigTestCosmic
		#               how do we set the base class variables in this way??? What do we need to do in the 
		#               derived class code?
		# tidacos = TrigTestCosmic(name = "TIDACosmicMonTool",
		#                          histoPathBase = "/Trigger/HLT")
		# tidacos = TrigTestCosmic(name = "TIDACosmicMonTool" )
		# tidacos.AnalysisConfig = "Tier0"
		# tidacos.SliceTag = "HLT/IDCosmic"
		# tidacos.ntupleChainNames += [
		#		"Offline",
		#		"HLT_id_cosmic.*:InDetTrigTrackingxAODCnv_CosmicsN_EFID",
		#		"HLT_id_cosmic.*:InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID"
		#		]
		#	ToolSvc += tidacos;
		#	list += [ "TrigTestCosmic/TIDACosmicMonTool" ]


		from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigTestBase

		# Cosmic instance
		tidacos = TrigTestBase(name = "IDCosmicMonTool",
					histoPathBase = "/Trigger/HLT")
		tidacos.AnalysisConfig = "Tier0"
		tidacos.SliceTag = "HLT/TRIDCosmic"
		# tidacos.OutputLevel = DEBUG
		tidacos.ntupleChainNames += [
			"Offline",
			"HLT_id_cosmic.*:InDetTrigTrackingxAODCnv_CosmicsN_EFID",
			"HLT_id_cosmic.*:InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID"
			]
		ToolSvc += tidacos;
		list += [ "TrigTestBase/IDCosmicMonTool" ]




		# test instances 
		tidabase = TrigTestBase(name = "IDMonTool",
					histoPathBase = "/Trigger/HLT")
		tidabase.AnalysisConfig = "Tier0"
		tidabase.SliceTag = "HLT/TRIDMon"
		# tidabase.OutputLevel = DEBUG
		tidabase.ntupleChainNames += [
			"Offline",
			"HLT_e.*idperf.*:InDetTrigTrackingxAODCnv_Electron_EFID",
			"HLT_mu.*idperf.*:InDetTrigTrackingxAODCnv_Muon_EFID",
			"HLT_tau.*idperf.*:InDetTrigTrackingxAODCnv_TauN_EFID",
			"HLT_id_cosmic.*:InDetTrigTrackingxAODCnv_CosmicsN_EFID",
			"HLT_id_cosmic.*:InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID"
			]
		ToolSvc += tidabase;
		list += [ "TrigTestBase/IDMonTool" ]




		# test instances
		tidatool = TrigTestBase(name = "TIDATool",
					histoPathBase = "/Trigger/HLT")
		tidatool.AnalysisConfig = "Tier0"
		tidatool.SliceTag = "HLT/TRIDMonTest"
		# tidatool.OutputLevel = DEBUG
		tidatool.ntupleChainNames += [
			"Offline",
			#egamma
			"EF_e24vh_medium1:InDetTrigParticleCreation_Electron_EFID",
			"L2_e24vh_medium1_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:0",
			"L2_e24vh_medium1_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:1",
			"L2_e24vh_medium1_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:2",
			#mu
			"EF_mu18_IDTrkNoCut_tight:InDetTrigParticleCreation_Muon_EFID",
			"L2_mu18_IDTrkNoCut_tight:TrigL2SiTrackFinder_Muon:0",
			"L2_mu18_IDTrkNoCut_tight:TrigL2SiTrackFinder_Muon:1",
			"L2_mu18_IDTrkNoCut_tight:TrigL2SiTrackFinder_Muon:2",
			"EF_mu22_IDTrkNoCut_tight:InDetTrigParticleCreation_Muon_EFID",
			"L2_mu22_IDTrkNoCut_tight:TrigL2SiTrackFinder_Muon:0",
			"L2_mu22_IDTrkNoCut_tight:TrigL2SiTrackFinder_Muon:1",
			"L2_mu22_IDTrkNoCut_tight:TrigL2SiTrackFinder_Muon:2",
			#tau
			"L2_tau29_IDTrkNoCut:TrigL2SiTrackFinder_Tau:0",
			"L2_tau29_IDTrkNoCut:TrigL2SiTrackFinder_Tau:1",
			"L2_tau29_IDTrkNoCut:TrigL2SiTrackFinder_Tau:2",
			"L2_tau125_IDTrkNoCut:TrigL2SiTrackFinder_Tau:0",
			"L2_tau125_IDTrkNoCut:TrigL2SiTrackFinder_Tau:1",
			"L2_tau125_IDTrkNoCut:TrigL2SiTrackFinder_Tau:2",
			"EF_tau29_IDTrkNoCut:InDetTrigParticleCreation_Tau_EFID",
			"EF_tau125_IDTrkNoCut:InDetTrigParticleCreation_Tau_EFID",
			#bjet
			"EF_b55_NoCut_j55_a4tchad:InDetTrigParticleCreation_Bjet_EFID",
			"L2_b50_NoCut_j50_c4cchad:TrigL2SiTrackFinder_Jet",
			#Full Scan
			"L2_InDetMon_FS:TrigIDSCAN_FullScan",
			"L2_InDetMon_FS:TrigSiTrack_FullScan",
			"L2_InDetMon_FS:TrigL2SiTrackFinder_FullScan:0",
			"L2_InDetMon_FS:TrigL2SiTrackFinder_FullScan:1",
			"L2_InDetMon_FS:TrigL2SiTrackFinder_FullScan:2",
			"EF_InDetMon_FS:InDetTrigParticleCreation_FullScan_EFID",
                        #Cosmics
			"HLT_id_cosmic.*:InDetTrigTrackingxAODCnv_CosmicsN_EFID",
			"HLT_id_cosmic.*:InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID",
		        #HI running
			"L2_mu4_IDTrkNoCut:TrigL2SiTrackFinder_Muon:0",
			"L2_mu4_IDTrkNoCut:TrigL2SiTrackFinder_Muon:1",
			"L2_mu4_IDTrkNoCut:TrigL2SiTrackFinder_Muon:2",
			"EF_mu4_IDTrkNoCut:InDetTrigParticleCreation_Muon_EFID",

			"L2_mu8_IDTrkNoCut:TrigL2SiTrackFinder_Muon:0",
			"L2_mu8_IDTrkNoCut:TrigL2SiTrackFinder_Muon:1",
			"L2_mu8_IDTrkNoCut:TrigL2SiTrackFinder_Muon:2",
			"EF_mu8_IDTrkNoCut:InDetTrigParticleCreation_Muon_EFID",

			"L2_mu10_IDTrkNoCut:TrigL2SiTrackFinder_Muon:0",
			"L2_mu10_IDTrkNoCut:TrigL2SiTrackFinder_Muon:1",
			"L2_mu10_IDTrkNoCut:TrigL2SiTrackFinder_Muon:2",
			"EF_mu10_IDTrkNoCut:InDetTrigParticleCreation_Muon_EFID",

			"L2_e15_loose1_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:0",
			"L2_e15_loose1_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:1",
			"L2_e15_loose1_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:2",
			"EF_e15_loose1_IDTrkNoCut:InDetTrigParticleCreation_Electron_EFID",

			"L2_e5_medium1_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:0",
			"L2_e5_medium1_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:1",
			"L2_e5_medium1_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:2",
			"EF_e5_medium1_IDTrkNoCut:InDetTrigParticleCreation_Electron_EFID",


			]
		ToolSvc += tidatool;
		list += [ "TrigTestBase/TIDATool" ]

        ### Tag and Probe ###

	if not 'rec' in dir():
		from RecExConfig.RecFlags  import rec

	if not 'DQMonFlags' in dir():
		from AthenaMonitoring.DQMonFlags import DQMonFlags
		dataType = DQMonFlags.monManDataType()

	if dataType != 'cosmics' and rec.doCalo:
		# Tag and Probe
		from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
		theAtlasExtrapolator=AtlasExtrapolator(name =  "MyAtlasExtrapolator")
		theAtlasExtrapolator.DoCaloDynamic = False # this turns off dynamic
		ToolSvc += AtlasExtrapolator('MyAtlasExtrapolator')

		# from TrackToCalo.TrackToCaloConf import ExtrapolateToCaloTool
		# theExtrapolateToCaloTool=ExtrapolateToCaloTool(name =	 "MyCaloExtrapolatorTool", Extrapolator = theAtlasExtrapolator)
		# ToolSvc += ExtrapolateToCaloTool('MyCaloExtrapolatorTool')

		# from TrigIDtrkMonitoring.TrigIDtrkMonitoringConf import HLTIDZeeTagProbe
		# from AthenaCommon.AppMgr import ToolSvc
		# HLTIDZeeTag = HLTIDZeeTagProbe(name               =    'HLTIDZeeTag', histoPathBase      =  "/Trigger/HLT", ExtrapolateToCaloTool = theExtrapolateToCaloTool);


		#from TrigIDtrkMonitoring.TrigIDtrkMonitoringConf import HLTIDZeeTagProbe
		#from AthenaCommon.AppMgr import ToolSvc
		#HLTIDZeeTag = HLTIDZeeTagProbe(name               = 'HLTIDZeeTag',
		#			       histoPathBase      = "/Trigger/HLT");

#		HLTIDZeeTag.TrigChainName = "e20_tight_e15_NoCut_Zee"

		# ToolSvc += HLTIDZeeTag;
		# list += ["HLTIDZeeTagProbe/HLTIDZeeTag"];

	return list
