

from AthenaCommon.Include import include  # to include old style job options

def setupRun3L1CaloSimulationSequence(skipCTPEmulation = False):

    ## print some information about the conditions the simulation
    ## is running in

    from AthenaCommon.Logging import logging
    log = logging.getLogger( 'TrigT1CaloFexSim.L1Sim' )
    log.setLevel(logging.INFO)

    from RecExConfig.RecFlags import rec
    from AthenaCommon.GlobalFlags import globalflags, jobproperties
    log.info("Begin setup of L1 Calo Run 3 simulation chain")
    log.info("rec.read.*     : RDO: %s, ESD: %s, AOD: %s" % (rec.readRDO(), rec.readESD(), rec.readAOD()) )
    log.info("rec.do*        : RDOTrigger: %s, ESD: %s, AOD: %s" % (rec.doRDOTrigger(), rec.doESD(), rec.doAOD()))
    log.info("rec.doWrite.*  : ESD: %s, AOD: %s, TAG: %s" % (rec.doWriteESD(), rec.doWriteAOD(), rec.doWriteTAG()) )

    from TrigT1CaloFexSim.L1SimulationControlFlags import L1Phase1SimFlags as simflags
    log.info(simflags._context_name)
    simflags.print_JobProperties('tree&value')

    ## Setup the histogramming, if it does not exist yet

    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if not hasattr(svcMgr,"THistSvc"):
        import AthenaCommon.CfgMgr as CfgMgr
        svcMgr += CfgMgr.THistSvc()
    (hStream , hFile) = simflags.OutputHistFile().split('#')  # e.g. EXPERT#l1Simulation.root
    if not hStream in [entry.split()[0] for entry in svcMgr.THistSvc.Output]:
        svcMgr.THistSvc.Output += ["%s DATAFILE='%s' OPT='RECREATE'" % tuple(simflags.OutputHistFile().split('#')) ]

    ## The L1 simulation algorithms will be added to a special sequence
    ## It will be merged later into the AthAlgSequence

    from AthenaCommon.AlgSequence import AthSequencer, AlgSequence
    l1simAlgSeq = AthSequencer("L1Run3SimulationSequence")
    topSequence = AlgSequence()


    ## scheduling eventinfo unless it exists already
    from RecExConfig.ObjKeyStore import objKeyStore
    if not objKeyStore.isInInput( "xAOD::EventInfo" ):
        if not hasattr( topSequence, "xAODMaker::EventInfoCnvAlg" ):
            from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
            topSequence += xAODMaker__EventInfoCnvAlg()
            pass
    else:
        if not hasattr( topSequence, "xAODMaker::EventInfoNonConstCnvAlg" ):
            topSequence += CfgMgr.xAODMaker__EventInfoNonConstCnvAlg()
            pass

    ## when running on data the ROBDataProviderSvc needs to be setup 
    if  jobproperties.Global.InputFormat() == 'bytestream':
        include('TrigT1CaloByteStream/ReadLVL1CaloBSRun2_jobOptions.py')

    ## CaloCells need to be created from LAr and Tile data when running on raw data
    ## (when running on ESD, CaloCells should be taken from the file)
    doCaloCellCreation = rec.readRDO()
    if doCaloCellCreation:
        # Setting up the CaloCell creation
        # turn off the digits->raw channel reconstruction
        from LArROD.LArRODFlags import larRODFlags
        from TileRecUtils.TileRecFlags import jobproperties as tileFlags
        from CaloRec.CaloCellFlags import jobproperties as caloCellFlags
        larRODFlags.readDigits = False
        tileFlags.TileRecFlags.readDigits = False
        caloCellFlags.CaloCellFlags.doLArCreateMissingCells = False

        log.info("Calling CaloCellGetter to setup creation of calo cells")
        # this is a bit ugly but it will make sure the CaloCellGetter gets into the L1Run3SimulationSequence
        topSequenceSnapshot = [c for c in topSequence]
        from CaloRec.CaloCellGetter import CaloCellGetter
        CaloCellGetter()
        for alg in topSequence:
            if alg not in topSequenceSnapshot:
                l1simAlgSeq += alg
        topSequence.removeAll()
        for alg in topSequenceSnapshot:
            topSequence += alg 



    ## Setup the provider of the SuperCells
    if simflags.Calo.SCellType() == "Pulse":
        # These are fully simulated supercells, from supercell pulse 
        # collection is CaloCellContainer#SCell
        SCIn="SCell"
    elif simflags.Calo.SCellType() == "BCID":
        # These are fully simulated supercells with applied BCID corrections
        # This is the only kind of supercells where BCID corrections are applied
        from TrigT1CaloFexSim.TrigT1CaloFexSimConfig import createSuperCellBCIDAlg
        l1simAlgSeq += createSuperCellBCIDAlg()
        SCIn="SCellBCID"
    elif simflags.Calo.SCellType() == "Emulated":
        # Supercells are reconstructed from the ET sum of the constituent calo cells 
        # This sets simflags.Calo.ApplySCQual to False
        from LArL1Sim.LArL1SimConf import LArSCSimpleMaker
        l1simAlgSeq += LArSCSimpleMaker( SCellContainer="SimpleSCell" )
        SCIn="SimpleSCell"
    else:
        SCIn="SCell" # default

    # Schedule towermaker
    from TrigT1CaloFexSim.TrigT1CaloFexSimConfig import createJGTowerReader, createJGTowerMaker
    l1simAlgSeq += createJGTowerMaker( useSCQuality = simflags.Calo.ApplySCQual(),
                                       useAllCalo = False,
                                       SuperCellType = SCIn,
                                       SuperCellQuality = simflags.Calo.QualBitMask() )


    # Schedule fex algorithms
    if simflags.Calo.RunFexAlgorithms():

        # eFEX
        from TrigL1CaloUpgrade.TrigL1CaloUpgradeConfig import enableEfexAlgorithms
        enableEfexAlgorithms ( l1simAlgSeq,
                               SuperCellContainer = SCIn, 
                               useTDR = False,
                               doLArFex = False,
                               ApplySCQual = simflags.Calo.ApplySCQual(), 
                               SCBitMask = simflags.Calo.QualBitMask() )
        # j/gFEX
        l1simAlgSeq += createJGTowerReader(SuperCellType=SCIn) # too much debug output

        #include L1Topo Simulation
        if simflags.Topo.RunTopoAlgorithms():

            if 'L1TopoSimulation' not in algseq:
                from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulation
                include ( "TrigT1CaloFexSim/EnableTopoAlgorithms.py" )
                enableL1TopoSimulation()

    # Schedule CTP Simulation
    if not skipCTPEmulation and simflags.CTP.RunCTPEmulation():
        from TrigT1CTP.TrigT1CTP_EnableCTPEmulation import enableCTPEmulation
        enableCTPEmulation(l1simAlgSeq)

    #algSequence = AthSequencer("AthAlgSeq")
    for alg in l1simAlgSeq:
        topSequence += alg
    l1simAlgSeq.removeAll()

    if simflags.EnableDebugOutput():
        log.debug("Algorithm sequence after L1 simulation setup")
        from AthenaCommon.AlgSequence import dumpSequence
        dumpSequence(topSequence)


    ## Configure the output content
    if rec.doWriteAOD():
        from TrigT1CaloFexSim.SetupOutput import fillAOD
        stream = fillAOD( SuperCellContainer=SCIn,
                          WriteAllCalo=False)

    if rec.doRDOTrigger():
        from TrigT1CaloFexSim.SetupOutput import fillRDO
        stream = fillRDO( SuperCellContainer=SCIn,
                          WriteAllCalo=False)

    if simflags.EnableDebugOutput():
        from TrigT1CaloFexSim.SetupOutput import printStreamingInfo
        printStreamingInfo()

    log.info("Finished setup of L1 Calo Run 3 simulation chain")
