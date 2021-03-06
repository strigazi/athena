#!/usr/bin/env python
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
ConfigFlags.Input.Files = defaultTestFiles.HITS
ConfigFlags._loadDynaFlags("Detector")

# Don't fail just because G4AtlasApps isn't present in this build.
havesim = True
try:
    import G4AtlasApps # noqa: F401
except ImportError:
    havesim = False
if havesim:
    ConfigFlags._loadDynaFlags("Sim")

ConfigFlags.initAll()
ConfigFlags.dump()

