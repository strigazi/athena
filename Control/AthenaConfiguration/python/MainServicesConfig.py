from __future__ import print_function

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.AlgSequence import AthSequencer

def MainServicesMiniCfg(LoopMgr='AthenaEventLoopMgr'):
    #Mininmal basic config, just good enough for HelloWorld and alike
    cfg=ComponentAccumulator()
    cfg.setAppProperty('TopAlg',['AthSequencer/AthAlgSeq']) #Just one sequence, no nesting
    cfg.setAppProperty('MessageSvcType', 'MessageSvc')
    cfg.setAppProperty('EventLoop', LoopMgr) 
    cfg.setAppProperty('ExtSvcCreates', 'False')
    cfg.setAppProperty('JobOptionsSvcType', 'JobOptionsSvc')
    cfg.setAppProperty('JobOptionsType', 'NONE')
    cfg.setAppProperty('JobOptionsPostAction', '')
    cfg.setAppProperty('JobOptionsPreAction', '')
    return cfg

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def MainServicesSerialCfg():
    serialflags=AthConfigFlags()
    serialflags.addFlag('Concurrency.NumProcs', 0)
    serialflags.addFlag('Concurrency.NumThreads', 0)
    serialflags.addFlag('Concurrency.NumConcurrentEvents', 0)
    return MainServicesThreadedCfg(serialflags)

def MainServicesThreadedCfg(cfgFlags):

    # Run a serial job for threads=0
    LoopMgr = 'AthenaEventLoopMgr'
    if cfgFlags.Concurrency.NumThreads>0:
        if cfgFlags.Concurrency.NumConcurrentEvents==0:
            # In a threaded job this will mess you up because no events will be processed
            raise Exception("Requested Concurrency.NumThreads>0 and Concurrency.NumConcurrentEvents==0, which will not process events!")

        LoopMgr = "AthenaHiveEventLoopMgr"

    ########################################################################
    # Core components needed for serial and threaded jobs
    cfg=ComponentAccumulator("AthMasterSeq")
    cfg.merge( MainServicesMiniCfg(LoopMgr) )
    cfg.setAppProperty('TopAlg',['AthSequencer/AthMasterSeq'],overwrite=True)
    cfg.setAppProperty('OutStreamType', 'AthenaOutputStream')

    #Build standard sequences:
    cfg.addSequence(AthSequencer('AthAlgEvtSeq',Sequential=True, StopOverride=True),parentName="AthMasterSeq") 
    cfg.addSequence(AthSequencer('AthOutSeq',StopOverride=True),parentName="AthMasterSeq")
    cfg.addSequence(AthSequencer('AthRegSeq',StopOverride=True),parentName="AthMasterSeq")

    cfg.addSequence(AthSequencer('AthBeginSeq',Sequential=True),parentName='AthAlgEvtSeq')
    cfg.addSequence(AthSequencer('AthAllAlgSeq',StopOverride=True),parentName='AthAlgEvtSeq') 

    if cfgFlags.Concurrency.NumThreads==0:
        # For serial execution, we need the CondAlgs to execute first.
        cfg.addSequence(AthSequencer('AthCondSeq',StopOverride=True),parentName='AthAllAlgSeq')
        cfg.addSequence(AthSequencer('AthAlgSeq',IgnoreFilterPassed=True,StopOverride=True),parentName='AthAllAlgSeq')
    else:
        # In MT, the order of execution is irrelevant (determined by data deps).
        # We add the conditions sequence later such that the CondInputLoader gets
        # initialized after all other user Algorithms for MT, so the base classes
        # of data deps can be correctly determined. 
        cfg.addSequence(AthSequencer('AthAlgSeq',IgnoreFilterPassed=True,StopOverride=True),parentName='AthAllAlgSeq')
        cfg.addSequence(AthSequencer('AthCondSeq',StopOverride=True),parentName='AthAllAlgSeq')

    cfg.addSequence(AthSequencer('AthEndSeq',Sequential=True),parentName='AthAlgEvtSeq') 

    #Set up incident firing:
    from AthenaServices.AthenaServicesConf import AthIncFirerAlg
    from GaudiCoreSvc.GaudiCoreSvcConf import IncidentProcAlg

    cfg.addEventAlgo(AthIncFirerAlg("BeginIncFiringAlg",FireSerial=False,Incidents=['BeginEvent']),sequenceName='AthBeginSeq')
    cfg.addEventAlgo(IncidentProcAlg('IncidentProcAlg1'),sequenceName='AthBeginSeq')

    cfg.addEventAlgo(AthIncFirerAlg('EndIncFiringAlg',FireSerial=False,Incidents=['EndEvent']), sequenceName="AthEndSeq")
    cfg.addEventAlgo(IncidentProcAlg('IncidentProcAlg2'),sequenceName="AthEndSeq")

    #Basic services:
    from CLIDComps.CLIDCompsConf import ClassIDSvc
    cfg.addService(ClassIDSvc(CLIDDBFiles= ['clid.db',"Gaudi_clid.db" ]))

    from StoreGate.StoreGateConf import StoreGateSvc
    cfg.addService(StoreGateSvc())
    cfg.addService(StoreGateSvc("DetectorStore"))
    cfg.addService(StoreGateSvc("HistoryStore"))
    cfg.addService( StoreGateSvc("ConditionStore") )
    
    cfg.setAppProperty('InitializationLoopCheck',False)

    ########################################################################
    # Additional components needed for threaded jobs only
    if cfgFlags.Concurrency.NumThreads>0:

        # Migrated code from AtlasThreadedJob.py
        from GaudiCoreSvc.GaudiCoreSvcConf import MessageSvc
        from GaudiSvc.GaudiSvcConf import StatusCodeSvc, AuditorSvc

        msgsvc = MessageSvc()
        msgsvc.defaultLimit = 0 
        #msgFmt = "% F%40W%S%4W%e%s%7W%R%T %0W%M"
        msgFmt = "% F%18W%S%7W%R%T %0W%M"
        msgsvc.Format = msgFmt
        cfg.addService(msgsvc)

        scsvc = StatusCodeSvc()
        scsvc.AbortOnError = False
        cfg.addService(scsvc)
        cfg.setAppProperty('StatusCodeCheck',False)

        from StoreGate.StoreGateConf import SG__HiveMgrSvc
        hivesvc = SG__HiveMgrSvc("EventDataSvc")
        hivesvc.NSlots = cfgFlags.Concurrency.NumConcurrentEvents
        cfg.addService( hivesvc )

        from GaudiHive.GaudiHiveConf import AlgResourcePool
        from AthenaCommon.Constants import INFO
        arp=AlgResourcePool( OutputLevel = INFO )
        arp.TopAlg=["AthMasterSeq"] #this should enable control flow
        cfg.addService( arp )

        from GaudiHive.GaudiHiveConf import AvalancheSchedulerSvc
        scheduler = AvalancheSchedulerSvc()
        scheduler.CheckDependencies    = cfgFlags.Scheduler.CheckDependencies
        scheduler.ShowDataDependencies = cfgFlags.Scheduler.ShowDataDeps
        scheduler.ShowDataFlow         = cfgFlags.Scheduler.ShowDataFlow
        scheduler.ShowControlFlow      = cfgFlags.Scheduler.ShowControlFlow
        scheduler.ThreadPoolSize       = cfgFlags.Concurrency.NumThreads
        cfg.addService(scheduler)

        from SGComps.SGCompsConf import SGInputLoader
        # FailIfNoProxy=False makes it a warning, not an error, if unmet data
        # dependencies are not found in the store.  It should probably be changed
        # to True eventually.
        inputloader = SGInputLoader (FailIfNoProxy = False)
        cfg.addEventAlgo( inputloader, "AthAlgSeq" )
        scheduler.DataLoaderAlg = inputloader.getName()

        from AthenaServices.AthenaServicesConf import AthenaHiveEventLoopMgr

        elmgr = AthenaHiveEventLoopMgr()
        elmgr.WhiteboardSvc = "EventDataSvc"
        elmgr.SchedulerSvc = scheduler.getName()
        cfg.addService( elmgr )

        # enable timeline recording
        from GaudiHive.GaudiHiveConf import TimelineSvc
        cfg.addService( TimelineSvc( RecordTimeline = True, Partial = False ) )

        #
        ## Setup SGCommitAuditor to sweep new DataObjects at end of Alg execute
        #

        auditorsvc = AuditorSvc()
        from SGComps.SGCompsConf import SGCommitAuditor
        auditorsvc += SGCommitAuditor()
        cfg.addService( auditorsvc )
        cfg.setAppProperty("AuditAlgorithms", True)

    return cfg
    
