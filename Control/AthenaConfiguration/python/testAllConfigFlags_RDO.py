#!/usr/bin/env python
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
ConfigFlags.Input.Files = defaultTestFiles.RDO
ConfigFlags._loadDynaFlags("Detector")

# Don't fail just because G4AtlasApps isn't present in this build.
havesim = True
try:
    import G4AtlasApps # noqa: F401
except ImportError:
    havesim = False
if havesim:
    ConfigFlags._loadDynaFlags("Sim")

# Don't fail just because Digitization isn't present in this build.
haveDigi = True
try:
    import Digitization # noqa: F401
except ImportError:
    haveDigi = False
if haveDigi:
    ConfigFlags._loadDynaFlags("Digitization")

# Don't fail just because OverlayConfiguration isn't present in this build.
haveOverlay = True
try:
    import OverlayConfiguration # noqa: F401
except ImportError:
    haveOverlay = False
if haveOverlay:
    ConfigFlags._loadDynaFlags("Overlay")

ConfigFlags.initAll()
ConfigFlags.dump()

