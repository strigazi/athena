######################################################################################
#
# This file is automatically generated with TriggerTest/python/TrigvalJobOptionBuilder.py
# To generate TriggerTest/testIDAthenaModernRDO.py use  TriggerTest/python/trigValMakeJobOptions.py 
#
######################################################################################


### usually ATN tests runs with following RDO input:
#PoolRDOInput=["/afs/cern.ch/atlas/offline/data/testfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root"]

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

if not acf.EvtMax.is_locked():
  acf.EvtMax=10
if not ('OutputLevel' in dir()):
  rec.OutputLevel=INFO

###############################
rec.doCBNT=False

doTrigger=True
rec.doESD.set_Value_and_Lock(False)

TriggerModernConfig=True

rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False 
rec.doESD.set_Value_and_Lock(False) 
doTAG=False

#rec.doTruth=True
rec.doTruth.set_Value_and_Lock(False)


#-----------------------------------------------------------
include("RecExCond/RecExCommon_flags.py")
#-----------------------------------------------------------

TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False

TriggerFlags.enableMonitoring = [ 'Validation', 'Time', 'Log' ]
TriggerFlags.doHLT=True
#TriggerFlags.doL1Topo=True 

#Enable tau slice
# PJB I can't configure this such that it runs EF ID only, and not
# calo and/or tauRec, so I switch it off
#TriggerFlags.TauSlice.setAll()
#TriggerFlags.TauSlice.unsetCalo()
#TriggerFlags.TauSlice.unsetEF()
#TriggerFlags.TauSlice.unsetAll()
#TriggerFlags.TauSlice.setID()
#TriggerFlags.TauSlice.unsetCalo()
#TriggerFlags.TauSlice.signatures = ["tau10"]
#TriggerFlags.TauSlice.unsetEF()

#### switch OFF other detectors
#TriggerFlags.doMuon=False

#------------ This is for ATN/RTT tests only ---------
TriggerFlags.triggerMenuSetup = 'Physics_pp_v5'
#-------------end of flag for tests-------------------

#------------ This is a temporary fix ---------------
#from RecExConfig.RecConfFlags import recConfFlags
#recConfFlags.AllowIgnoreConfigError=False
#athenaCommonFlags.AllowIgnoreConfigError=False
#-------------end of temporary fix-------------------


#override menu
def ElectronOnly():
  TriggerFlags.Slices_all_setOff()
  
    # Enable electron slice TriggerFlags.EgammaSlice.setAll()
  TriggerFlags.EgammaSlice.unsetCalo()
  TriggerFlags.Lvl1.items = TriggerFlags.Lvl1.items() + [  ]
  
  TriggerFlags.EgammaSlice.signatures = [
    ['e24_medium_idperf', 'L1_EM18VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'],1],
    ]
    
try:
  from TriggerMenu import useNewTriggerMenu
  useNewTM = useNewTriggerMenu()
  log.info("Using new TriggerMenu: %r" % useNewTM)
except:
  useNewTM = False
  log.info("Using old TriggerMenuPython since TriggerMenu.useNewTriggerMenu can't be imported")
  
if useNewTM:
  from TriggerMenu.menu.GenerateMenu import GenerateMenu
else:
  from TriggerMenuPython.GenerateMenu import GenerateMenu

GenerateMenu.overwriteSignaturesWith(ElectronOnly)


#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

#-----------------------------------------------------------
include("TriggerTest/TriggerTestCommon.py")
#-----------------------------------------------------------



