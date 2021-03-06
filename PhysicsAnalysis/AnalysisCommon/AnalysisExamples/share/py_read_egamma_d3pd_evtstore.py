_d3pd_files = [
    'root://castoratlas//castor/cern.ch/user/b/binet/utests/utests/filter-d3pd/ntuple.0.root',
    'root://castoratlas//castor/cern.ch/user/b/binet/utests/utests/filter-d3pd/ntuple.1.root',
    #'ntuple.0.root',
    #'ntuple.1.root',
    ]
import AthenaRootComps.ReadAthenaRoot
svcMgr.EventSelector.InputCollections = _d3pd_files
svcMgr.EventSelector.TupleName = "egamma"

# disable all branches (will be activated on a need-to-use basis)
svcMgr.EventSelector.ActiveBranches = []

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

import AnalysisExamples.Lib as AEL
job += AEL.PyReadD3pdEvtStore(
    OutputLevel = Lvl.DEBUG
    )

if not 'EVTMAX' in dir():
    EVTMAX=10
theApp.EvtMax = EVTMAX

