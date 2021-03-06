import os
print 'get_files checkFile.py'
os.system('get_files checkFile.py')

#from AthenaCommon.AthenaCommonFlags import jobproperties
#jobproperties.AthenaCommonFlags.PoolESDInput.set_Value_and_Lock(['/afs/cern.ch/work/t/tothj/public/CaloAnaEx_RTT_infiles/mc11/mc11_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e835_s1272_s1274_d580_tid541908_00/ESD.541908._000015_200ev.pool.root.1'])

readESD=True
noESDTrigger=True

doWriteESD=False
doWriteAOD=True 
doWriteTAG=False

doAOD=True
doAODLVL1=False
doTrigger=False
doMissingET=False

# number of event to process
#EvtMax=-1
EvtMax=5

include ("RecExCommon/RecExCommon_flags.py")

from AthenaCommon.GlobalFlags import globalflags

globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-18-01-01')
globalflags.ConditionsTag = "OFLCOND-SDR-BS7T-05-14"

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

import os
com="echo \"BEGIN_RTTINFO = ESDtoAOD_mc11_top\">&MYRTTINFOS.txt"
os.system(com)

