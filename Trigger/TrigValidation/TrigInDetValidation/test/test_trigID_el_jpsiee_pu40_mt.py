#!/usr/bin/env python

# art-description: art job for el_Jpsiee_pu40_mt
# art-type: grid
# art-include: master/Athena
# art-input: mc15_13TeV.129190.Pythia8_AU2CTEQ6L1_ppToJpsie3e3.recon.RDO.e3802_s2608_s2183_r7042
# art-input-nfiles: 16
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
rdo2aod.slices = ['electron']
rdo2aod.max_events = 8000 
rdo2aod.threads = 4
rdo2aod.concurrent_events = 4
rdo2aod.perfmon = False
rdo2aod.timeout = 18*3600
if local:
    rdo2aod.input = 'Jpsiee_pu40'     # defined in TrigValTools/share/TrigValInputs.json  
else:
    rdo2aod.input = ''
    rdo2aod.args += '--inputRDOFile=$ArtInFile '


test = Test.Test()
test.art_type = 'grid'
if (not exclude):
    test.exec_steps = [rdo2aod]
    test.exec_steps.append(TrigInDetAna()) # Run analysis to produce TrkNtuple
    test.check_steps = CheckSteps.default_check_steps(test)

 
# Run Tidardict
if ((not exclude) or postproc ):
    rdict = TrigInDetdictStep()
    rdict.args='TIDAdata-run3.dat -f data-hists.root -p 11 -b Test_bin.dat '
    test.check_steps.append(rdict)
    rdict2 = TrigInDetdictStep('TrigInDetDict2')
    rdict2.args='TIDAdata-run3.dat -r Offline  -f data-hists-offline.root -b Test_bin.dat '
    test.check_steps.append(rdict2)

 
# Now the comparitor steps
comp=TrigInDetCompStep('Comp_L2ele','L2','electron')
test.check_steps.append(comp)
  
comp2=TrigInDetCompStep('Comp_EFele','EF','electron')
test.check_steps.append(comp2)

comp3=TrigInDetCompStep('Comp_L2eleLowpt','L2','electron',lowpt=True)
test.check_steps.append(comp3)

comp4=TrigInDetCompStep('Comp_EFeleLowpt','EF','electron',lowpt=True)
test.check_steps.append(comp4)

comp5=TrigInDetCompStep('Comp_L2ele_off','L2','electron')
comp5.type='offl'
test.check_steps.append(comp5)
  
comp6=TrigInDetCompStep('Comp_EFele_off','EF','electron')
comp6.type='offl'
test.check_steps.append(comp6)

comp7=TrigInDetCompStep('Comp_L2eleLowpt_off','L2','electron',lowpt=True)
comp7.type='offl'
test.check_steps.append(comp7)

comp8=TrigInDetCompStep('Comp_EFeleLowpt_off','EF','electron',lowpt=True)
comp8.type='offl'
test.check_steps.append(comp8)

# CPU cost steps
cpucost=TrigInDetCpuCostStep('CpuCostStep1', ftf_times=False)
test.check_steps.append(cpucost)

cpucost2=TrigInDetCpuCostStep('CpuCostStep2')
test.check_steps.append(cpucost2)

import sys
sys.exit(test.run())
