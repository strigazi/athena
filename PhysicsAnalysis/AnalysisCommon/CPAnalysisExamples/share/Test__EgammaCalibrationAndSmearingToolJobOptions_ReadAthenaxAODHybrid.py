#Skeleton joboption for a simple analysis job

theApp.EvtMax=-1                                         #says how many events to run over. Set to -1 for all events

import AthenaRootComps.ReadAthenaxAODHybrid              #read xAOD with TEvent but the metadata with pool
svcMgr.EventSelector.AccessMode = 2 #0 = branch, 1 = class, 2 = athena, -1 = default ... we have to use 2 with the default test file in the releases
svcMgr.EventSelector.InputCollections=[os.environ['ASG_TEST_FILE_MC']]   #insert your list of input files here

algseq = CfgMgr.AthSequencer("AthAlgSeq")                #gets the main AthSequencer
algseq += CfgMgr.Test__EgammaCalibrationAndSmearingTool()                                 #adds an instance of your alg to it

svcMgr.ChronoStatSvc.PrintEllapsedTime=True #show the wall clock time
svcMgr.AthenaEventLoopMgr.UseDetailChronoStat=True #show event loop execution time statistics


##--------------------------------------------------------------------
## This section shows up to set up a HistSvc output stream for outputing histograms and trees
## See https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_output_trees_and_histogra for more details and examples

#if not hasattr(svcMgr, 'THistSvc'): svcMgr += CfgMgr.THistSvc() #only add the histogram service if not already present (will be the case in this jobo)
#svcMgr.THistSvc.Output += ["MYSTREAM DATAFILE='myfile.root' OPT='RECREATE'"] #add an output root file stream

##--------------------------------------------------------------------


##--------------------------------------------------------------------
## The lines below are an example of how to create an output xAOD
## See https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_create_an_output_xAOD for more details and examples

#from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
#xaodStream = MSMgr.NewPoolRootStream( "StreamXAOD", "xAOD.out.root" )

##EXAMPLE OF BASIC ADDITION OF EVENT AND METADATA ITEMS
##AddItem and AddMetaDataItem methods accept either string or list of strings
#xaodStream.AddItem( ["xAOD::JetContainer#*","xAOD::JetAuxContainer#*"] ) #Keeps all JetContainers (and their aux stores)
#xaodStream.AddMetaDataItem( ["xAOD::TriggerMenuContainer#*","xAOD::TriggerMenuAuxContainer#*"] )
#ToolSvc += CfgMgr.xAODMaker__TriggerMenuMetaDataTool("TriggerMenuMetaDataTool") #MetaDataItems needs their corresponding MetaDataTool
#svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.TriggerMenuMetaDataTool ] #Add the tool to the MetaDataSvc to ensure it is loaded

##EXAMPLE OF SLIMMING (keeping parts of the aux store)
#xaodStream.AddItem( ["xAOD::ElectronContainer#Electrons","xAOD::ElectronAuxContainer#ElectronsAux.pt.eta.phi"] ) #example of slimming: only keep pt,eta,phi auxdata of electrons

##EXAMPLE OF SKIMMING (keeping specific events)
#xaodStream.AddAcceptAlgs( "Test__EgammaCalibrationAndSmearingTool" ) #will only keep events where 'setFilterPassed(false)' has NOT been called in the given algorithm

##--------------------------------------------------------------------


#include("AthAnalysisBaseComps/SuppressLogging.py")              #Optional include to suppress as much athena output as possible. Keep at bottom of joboptions so that it doesn't suppress the logging of the things you have configured above

