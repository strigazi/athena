#!/usr/bin/env python
"""Run tests on SCT_DigitizationConfigNew.py

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
import sys
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from PixelDigitization.PixelDigitizationConfigNew import (
    ChargeCollProbSvcCfg, EnergyDepositionToolCfg, SensorSimPlanarToolCfg, SensorSim3DToolCfg,
    BarrelRD53SimToolCfg, EndcapRD53SimToolCfg, BarrelFEI4SimToolCfg, DBMFEI4SimToolCfg,
    BarrelFEI3SimToolCfg, EndcapFEI3SimToolCfg, PixelDigitizationBasicToolCfg, PixelDigitizationToolCfg,
    PixelGeantinoTruthDigitizationToolCfg, PixelDigitizationHSToolCfg, PixelDigitizationPUToolCfg,
    PixelDigitizationSplitNoMergePUToolCfg, PixelDigitizationOverlayToolCfg, PixelRangeCfg,
    PixelDigitizationHSCfg, PixelDigitizationPUCfg, PixelDigitizationOverlayCfg,
)

# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
# Configure
ConfigFlags.Input.Files = defaultTestFiles.HITS
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
ConfigFlags.GeoModel.Align.Dynamic = False
ConfigFlags.GeoModel.Type = "BrlIncl4.0_ref"
ConfigFlags.Beam.NumberOfCollisions = 0.
ConfigFlags.lock()
# Construct our accumulator to run
acc = MainServicesSerialCfg()
acc.merge(PoolReadCfg(ConfigFlags))
acc.merge(PixelDigitizationHSCfg(ConfigFlags))
# Dump config
acc.getService("StoreGateSvc").Dump = True
acc.getService("ConditionStore").Dump = True
acc.printConfig(withDetails=True)
ConfigFlags.dump()
# Execute and finish
sc = acc.run(maxEvents=3)
# Success should be 0
sys.exit(not sc.isSuccess())

