#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
#
# File: TrkVertexFitters/share/AdaptiveMultiVertexFitter_test.py
# Author: scott snyder <snyder@bnl.gov>
# Data: Jul, 2019
# Brief: Unit test for AdaptiveMultiVertexFitter.  Incomplete!
#


from __future__ import print_function


from AthenaCommon.DetFlags      import DetFlags
DetFlags.detdescr.ID_setOn()
DetFlags.detdescr.TRT_setOff()

RunNumber = 284500

import sys
import string
import ROOT
import math
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
ServiceMgr += GeoModelSvc()
theApp.CreateSvc += [ "GeoModelSvc"]
from AtlasGeoModel import InDetGM

from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
IOVDbSvc().GlobalTag = 'OFLCOND-RUN12-SDR-35'

import MagFieldServices.SetupField

from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

theApp.EvtMax=2

from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
eialg = xAODMaker__EventInfoCnvAlg ()
topSequence += eialg



# Suppress useless GeoModelSvc messages.
from AthenaCommon import Constants
GeoModelSvc().OutputLevel=Constants.WARNING

from TrkExTools.AtlasExtrapolator import AtlasExtrapolator


from TrkVertexFitters.TrkVertexFittersConf import \
    Trk__AdaptiveMultiVertexFitterTestAlg, Trk__AdaptiveMultiVertexFitter
fitter = Trk__AdaptiveMultiVertexFitter ('AdaptiveMultiVertexFitter',
                                         OutputLevel = INFO)
testalg1 = Trk__AdaptiveMultiVertexFitterTestAlg ('testalg1',
                                                  OutputLevel = VERBOSE,
                                                  Tool = fitter)
topSequence += testalg1