#!/usr/bin/env python

# art-description: art job for all_ttbar_pu80
# art-type: grid
# art-include: master/Athena
# art-input-nfiles: 3
# art-athena-mt: 4
# art-memory: 4096
# art-html: https://idtrigger-val.web.cern.ch/idtrigger-val/TIDAWeb/TIDAart/?jobdir=
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
# art-output: HLT*
# art-output: times*
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-output: *.dat 


from TrigValTools.TrigValSteering import Test, CheckSteps
from TrigInDetValidation.TrigInDetArtSteps import TrigInDetReco, TrigInDetAna, TrigInDetdictStep, TrigInDetCompStep, TrigInDetCpuCostStep


import sys,getopt

try:
    opts, args = getopt.getopt(sys.argv[1:],"lxp",["local"])
except getopt.GetoptError:
    print("Usage:  ")
    print("-l(--local)    run locally with input file from art eos grid-input")
    print("-x             don't run athena or post post-processing, only plotting")
    print("-p             run post-processing, even if -x is set")


local=False
exclude=False
postproc=False
for opt,arg in opts:
    if opt in ("-l", "--local"):
        local=True
    if opt=="-x":
        exclude=True
    if opt=="-p":
        postproc=True


rdo2aod = TrigInDetReco()
rdo2aod.slices = ['muon','electron','tau','bjet']
rdo2aod.max_events = 2000 
rdo2aod.threads = 1 # TODO: change to 4
rdo2aod.concurrent_events = 1 # TODO: change to 4
rdo2aod.perfmon = False
rdo2aod.timeout = 18*3600
rdo2aod.input = 'ttbar_pu80'   # defined in TrigValTools/share/TrigValInputs.json  


test = Test.Test()
test.art_type = 'grid'
if (not exclude):
    test.exec_steps = [rdo2aod]
    test.exec_steps.append(TrigInDetAna()) # Run analysis to produce TrkNtuple
    test.check_steps = CheckSteps.default_check_steps(test)

# Run Tidardict
if ((not exclude) or postproc ):
    rdict = TrigInDetdictStep()
    rdict.args='TIDAdata-run3.dat -r Offline -f data-hists.root -b Test_bin.dat '
    test.check_steps.append(rdict)

 
# Now the comparitor steps
comp=TrigInDetCompStep('Comp_L2muon','L2','muon')
comp.test='ttbar'
test.check_steps.append(comp)
 
 
comp2=TrigInDetCompStep('Comp_EFmuon','EF','muon')
comp2.test='ttbar'
test.check_steps.append(comp2)


comp3=TrigInDetCompStep('Comp_L2bjet','L2','bjet')
comp3.test='ttbar'
test.check_steps.append(comp3)

comp4=TrigInDetCompStep('Comp_EFbjet','EF','bjet')
comp4.test='ttbar'
test.check_steps.append(comp4)

comp5=TrigInDetCompStep('Comp_L2tau','L2','tau')
comp5.test='ttbar'
test.check_steps.append(comp5)

comp6=TrigInDetCompStep('Comp_EFtau','EF','tau')
comp6.test='ttbar'
test.check_steps.append(comp6)

comp7=TrigInDetCompStep('Comp_L2ele','L2','electron')
comp7.test='ttbar'
test.check_steps.append(comp7)

comp8=TrigInDetCompStep('Comp_EFele','EF','electron')
comp8.test='ttbar'
test.check_steps.append(comp8)

comp9=TrigInDetCompStep('Comp_L2FS','L2','FS')
comp9.test='ttbar'
test.check_steps.append(comp9)

# CPU cost steps
cpucost=TrigInDetCpuCostStep('CpuCostStep1', ftf_times=False)
test.check_steps.append(cpucost)

cpucost2=TrigInDetCpuCostStep('CpuCostStep2')
test.check_steps.append(cpucost2)


import sys
sys.exit(test.run())
