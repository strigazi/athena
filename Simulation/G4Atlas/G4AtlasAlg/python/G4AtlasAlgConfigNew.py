# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
#from AthenaCommon import CfgMgr
from G4AtlasServices.G4AtlasServicesConfigNew import DetectorGeometrySvcCfg
from ISF_Services.ISF_ServicesConfigNew import MC15aPlusTruthServiceCfg, GeoIDSvcCfg, InputConverterCfg


#todo - think about the flow, do we need if statements?!

from  G4AtlasAlg.G4AtlasAlgConf import G4AtlasAlg

#to still migrate: getAthenaStackingActionTool, getAthenaTrackingActionTool

def G4AtlasAlgCfg(ConfigFlags, name='G4AtlasAlg', **kwargs):
    #add Services to G4AtlasAlg
    result = DetectorGeometrySvcCfg(ConfigFlags)
    kwargs.setdefault('DetGeoSvc', result.getService("DetectorGeometrySvc"))

    
    kwargs.setdefault("InputTruthCollection", "BeamTruthEvent") #tocheck -are these string inputs?
    kwargs.setdefault("OutputTruthCollection", "TruthEvent")
    ## Killing neutrinos
    if ConfigFlags.Sim.ReleaseGeoModel:
        ## Don't drop the GeoModel
        kwargs.setdefault('ReleaseGeoModel' ,ConfigFlags.Sim.ReleaseGeoModel)

    if ConfigFlags.Sim.RecordFlux:
        ## Record the particle flux during the simulation
        kwargs.setdefault('RecordFlux' , ConfigFlags.Sim.RecordFlux)

    if ConfigFlags.Sim.FlagAbortedEvents:
        ## default false
        kwargs.setdefault('FlagAbortedEvents' ,ConfigFlags.Sim.FlagAbortedEvents)
        if ConfigFlags.Sim.FlagAbortedEvents and ConfigFlags.Sim.KillAbortedEvents:
            print('WARNING When G4AtlasAlg.FlagAbortedEvents is True G4AtlasAlg.KillAbortedEvents should be False!!! Setting G4AtlasAlg.KillAbortedEvents = False now!')
            kwargs.setdefault('KillAbortedEvents' ,False)
    if  ConfigFlags.Sim.KillAbortedEvents:
        ## default true
        kwargs.setdefault('KillAbortedEvents' ,ConfigFlags.Sim.KillAbortedEvents)

    from RngComps.RandomServices import AthEngines, RNG
    if ConfigFlags.Random.Engine in AthEngines.keys():
        result.merge(RNG(ConfigFlags.Random.Engine, name="AthRNGSvc"))
        kwargs.setdefault("AtRndmGenSvc",result.getService("AthRNGSvc"))

    kwargs.setdefault("RandomGenerator", "athena")

    # Multi-threading settinggs
    #is_hive = (concurrencyProps.ConcurrencyFlags.NumThreads() > 0)
    is_hive = ConfigFlags.Concurrency.NumThreads > 0
    kwargs.setdefault('MultiThreading', is_hive)


    accMCTruth = MC15aPlusTruthServiceCfg(ConfigFlags)
    result.merge(accMCTruth)
    kwargs.setdefault('TruthRecordService', result.getService("ISF_MC15aPlusTruthService"))
    #kwargs.setdefault('TruthRecordService', ConfigFlags.Sim.TruthStrategy) # TODO need to have manual override (simFlags.TruthStrategy.TruthServiceName())

    accGeoID = GeoIDSvcCfg(ConfigFlags)
    result.merge(accGeoID)
    kwargs.setdefault('GeoIDSvc', result.getService('ISF_GeoIDSvc'))

    #input converter
    accInputConverter = InputConverterCfg(ConfigFlags)
    result.merge(accInputConverter)
    kwargs.setdefault('InputConverter', result.getService("ISF_InputConverter"))



    ## G4AtlasAlg verbosities (available domains = Navigator, Propagator, Tracking, Stepping, Stacking, Event)
    ## Set stepper verbose = 1 if the Athena logging level is <= DEBUG
    # TODO: Why does it complain that G4AtlasAlgConf.G4AtlasAlg has no "Verbosities" object? Fix.
    verbosities=dict()
    #from AthenaCommon.AppMgr import ServiceMgr
    #if ServiceMgr.MessageSvc.OutputLevel <= 2:
    #    verbosities["Tracking"]='1'
    #    print verbosities
    kwargs.setdefault('Verbosities', verbosities)

    # Set commands for the G4AtlasAlg
    kwargs.setdefault("G4Commands", ConfigFlags.Sim.G4Commands)

    result.addEventAlgo(G4AtlasAlg(name, **kwargs))
    return result






if __name__ == '__main__':
  from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
  import os

  # Set up logging and config behaviour
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import DEBUG
  from AthenaCommon.Configurable import Configurable
  log.setLevel(DEBUG)
  Configurable.configurableRun3Behavior = 1


  #import config flags
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  
  from AthenaConfiguration.TestDefaults import defaultTestFiles
  inputDir = defaultTestFiles.d
  ConfigFlags.Input.Files = defaultTestFiles.EVNT

  ConfigFlags.Sim.WorldRRange = 15000
  ConfigFlags.Sim.WorldZRange = 27000 #change defaults?
  ConfigFlags.Detector.SimulateForward = False
  # Finalize 
  ConfigFlags.lock()

  ## Initialize a new component accumulator
  cfg = MainServicesSerialCfg()

  #add the algorithm
  cfg.merge(G4AtlasAlgCfg(ConfigFlags))

  # Dump config
  cfg.getService("StoreGateSvc").Dump = True
  cfg.getService("ConditionStore").Dump = True
  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()


  # Execute and finish
  sc = cfg.run(maxEvents=3)
  # Success should be 0
  os.sys.exit(not sc.isSuccess())

  #f=open("test.pkl","w")
  #cfg.store(f) 
  #f.close()



  print(cfg._publicTools)
  print("-----------------finished----------------------")
