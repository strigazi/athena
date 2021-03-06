# --- for athena online running ---
if 'EventBlockSize' not in dir():
    EventBlockSize=0


from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
CaloNoiseCondAlg(noisetype="electronicNoise")

###### LAr Coverage Tool Configuration ###############
from LArMonTools.LArMonToolsConf import LArCoverage
theLArCoverage = LArCoverage(name="LArCoverage",
                             ProcessNEvents             = EventBlockSize,
                             LArBadChannelMask          = theLArBadChannelsMasker,
                             Nevents                    = 40
                         )
LArMon.AthenaMonTools+=[ theLArCoverage ]




