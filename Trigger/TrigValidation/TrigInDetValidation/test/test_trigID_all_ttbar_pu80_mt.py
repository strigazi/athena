#!/usr/bin/env python

# art-description: art job for all_ttbar_pu80_mt
# art-type: grid
# art-include: master/Athena
# art-input-nfiles: 3
# art-athena-mt: 4
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.out
# art-output: *.err
# art-output: *.log.tar.gz
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.check*
# art-output: HLTEF-plots
# art-output: HLTL2-plots
# art-output: times
# art-output: times-FTF
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-output: *.dat 


from TrigValTools.TrigValSteering import Test, CheckSteps
from TrigInDetValidation.TrigInDetArtSteps import TrigInDetReco, TrigInDetAna, TrigInDetdictStep, TrigInDetCompStep, TrigInDetCpuCostStep


import sys,getopt

try:
    opts, args = getopt.getopt(sys.argv[1:],"l",["local"])
except getopt.GetoptError:
    print("Usage: -l(--local)")


local=False
for opt,arg in opts:
    if opt in ("-l", "--local"):
        local=True




rdo2aod = TrigInDetReco()
rdo2aod.slices = ['muon','electron','tau','bjet']
rdo2aod.max_events = 2000 
rdo2aod.threads = 1 # TODO: change to 4
rdo2aod.concurrent_events = 4 
rdo2aod.perfmon = False
rdo2aod.timeout = 18*3600
rdo2aod.input = 'ttbar_pu80'   # defined in TrigValTools/share/TrigValInputs.json  


test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [rdo2aod]
test.check_steps = CheckSteps.default_check_steps(test)

# Run analysis to produce TrkNtuple
test.exec_steps.append(TrigInDetAna())
 
# Run Tidardict
test.check_steps.append(TrigInDetdictStep())
 
# Now the comparitor steps
comp=TrigInDetCompStep('Comp_L2muon')
comp.flag='L2muon'
comp.test='ttbar'
test.check_steps.append(comp)
 
 
comp2=TrigInDetCompStep('Comp_EFmuon')
comp2.flag='EFmuon'
comp2.test='ttbar'
test.check_steps.append(comp2)


comp3=TrigInDetCompStep('Comp_L2bjet')
comp3.flag='L2bjet'
comp3.test='ttbar'
test.check_steps.append(comp3)

comp4=TrigInDetCompStep('Comp_EFbjet')
comp4.flag='EFbjet'
comp4.test='ttbar'
test.check_steps.append(comp4)

comp5=TrigInDetCompStep('Comp_L2tau')
comp5.flag='L2tau'
comp5.test='ttbar'
test.check_steps.append(comp5)

comp6=TrigInDetCompStep('Comp_EFtau')
comp6.flag='EFtau'
comp6.test='ttbar'
test.check_steps.append(comp6)

comp7=TrigInDetCompStep('Comp_L2ele')
comp7.flag='L2ele'
comp7.test='ttbar'
test.check_steps.append(comp7)

comp8=TrigInDetCompStep('Comp_EFele')
comp8.flag='EFele'
comp8.test='ttbar'
test.check_steps.append(comp8)


# CPU cost steps
cpucost=TrigInDetCpuCostStep('CpuCostStep1')
test.check_steps.append(cpucost)

cpucost2=TrigInDetCpuCostStep('CpuCostStep2')
cpucost2.args += '  -p FastTrack'
cpucost2.output_dir = 'times-FTF' 
test.check_steps.append(cpucost2)


import sys
sys.exit(test.run())
