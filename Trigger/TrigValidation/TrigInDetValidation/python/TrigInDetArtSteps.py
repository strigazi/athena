#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
Definitions of additional validation steps in Trigger ART tests relevant only for TrigInDetValidation
The main common check steps are defined in the TrigValSteering.CheckSteps module.
'''

import os

from TrigAnalysisTest.TrigAnalysisSteps import AthenaCheckerStep
from TrigValTools.TrigValSteering.Step import Step

##################################################
# Additional exec (athena) steps
##################################################

class TrigInDetAna(AthenaCheckerStep):
    def __init__(self, name='TrigInDetAna', in_file='AOD.pool.root'):
        AthenaCheckerStep.__init__(self, name, 'TrigInDetValidation/TrigInDetValidation_AODtoTrkNtuple.py')
        self.max_events=-1
        self.required = True
        self.input_file = in_file

##################################################
# Additional post-processing steps
##################################################

class TrigInDetdictStep(Step):
    '''
    Execute TIDArdict for TrkNtuple files.
    '''
    def __init__(self, name='TrigInDetdict'):
        super(TrigInDetdictStep, self).__init__(name)
        self.args=' '
        self.auto_report_result = True
        self.required = True
        self.executable = 'TIDArdict'

    def configure(self, test):
        cmd = 'get_files TIDAdata-run3.dat'
        os.system(cmd)
        cmd = 'get_files TIDAdata-chains-run3.dat'
        os.system(cmd)
        cmd = 'get_files TIDAbeam.dat'
        os.system(cmd)
        cmd = 'get_files Test_bin.dat'
        os.system(cmd)
        cmd = 'get_files TIDAdata_cuts.dat'
        os.system(cmd)
        super(TrigInDetdictStep, self).configure(test)


class TrigInDetCompStep(Step):
    '''
    Execute TIDAcomparitor for data.root files.
    '''
    def __init__(self, name='TrigInDetComp'):
        super(TrigInDetCompStep, self).__init__(name)
        self.input_file = 'data-hists.root'
        self.ref_file = 'data-hists.root'   #### need to add reference file here 
        self.output_dir = 'HLT-plots'
        self.chains = ' '
        self.args = ''
        self.auto_report_result = True
        self.required = True
        self.executable = 'TIDAcomparitor'
    
    def configure(self, test):
        self.args += self.input_file+' '+self.ref_file+' '+self.chains+' -d '+self.output_dir
        super(TrigInDetCompStep, self).configure(test)

