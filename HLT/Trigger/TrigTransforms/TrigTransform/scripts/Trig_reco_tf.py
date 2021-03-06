#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## Trig_reco_tf.py
## - based on PyJobTransforms/Reco_tf.py
## Documentation on the twiki https://twiki.cern.ch/twiki/bin/viewauth/Atlas/TriggerTransform

import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor, DQMergeExecutor, reductionFrameworkExecutor, reductionFrameworkExecutorNTUP
from PyJobTransforms.trfArgs import addAthenaArguments, addD3PDArguments, addPrimaryDPDArguments, addExtraDPDTypes, addDetectorArguments, addReductionArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from RecJobTransforms.recTransformUtils import addCommonRecTrfArgs, addStandardRecoFiles

import PyJobTransforms.trfArgClasses as trfArgClasses

from TrigTransform.trigRecoExe import trigRecoExecutor
from TrigTransform.trigCostExe import trigCostExecutor

@stdTrfExceptionHandler
@sigUsrStackTrace
def main():
    
    msg.info('This is %s' % sys.argv[0])

    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    msg.info("%s stopped at %s, trf exit code %d" % (sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)

def getTransform():
    executorSet = set()
        
    #BSRDOtoRAW is new option for trigger transform
    #runs primarily using athenaHLT
    #literalRunargs used for when running with athena
    executorSet.add(trigRecoExecutor(name = 'BSRDOtoRAW', skeletonFile = 'TriggerJobOpts/runHLT_standalone.py',
                                     exe = 'setsid athenaHLT.py', 
                                     # setsid is needed to fix the process-group id of child processes to be the same as mother process; discussed in https://its.cern.ch/jira/browse/ATR-20513 
                                     substep = 'b2r', tryDropAndReload = False,
                                     inData = ['BS_RDO', 'RDO'], outData = ['BS', 'BS_TRIGCOST', 'HIST_HLTMON','HIST_DEBUGSTREAMMON'],
                                     perfMonFile = 'ntuple_BSRDOtoRAW.pmon.gz',
                                     literalRunargs = ['writeBS = runArgs.writeBS',
                                                       'from AthenaCommon.AthenaCommonFlags import jobproperties as jps',
                                                       'jps.AthenaCommonFlags.FilesInput.set_Value_and_Lock(runArgs.inputBS_RDOFile)',
                                                       'jps.AthenaCommonFlags.EvtMax.set_Value_and_Lock(runArgs.maxEvents)',
                                                       'if hasattr(runArgs,\'skipEvents\'):',
                                                       '   jps.AthenaCommonFlags.SkipEvents.set_Value_and_Lock(runArgs.skipEvents)',
                                                       'if hasattr(runArgs,\'outputBSFile\'):',
                                                       '   jps.AthenaCommonFlags.BSRDOOutput.set_Value_and_Lock( runArgs.outputBSFile )']))

    #RAWtoCOST is new option for trigger transform
    #runs in athena and will succeed if input BS file has costmon enabled
    executorSet.add(trigCostExecutor(name = 'RAWtoCOST', skeletonFile = 'TrigCostMonitor/readTrigCost.py',
                                     substep = 'r2c',
                                     inData = ['BS_TRIGCOST'], outData = ['NTUP_TRIGCOST','NTUP_TRIGRATE','NTUP_TRIGEBWGHT'],
                                     perfMonFile = 'ntuple_RAWtoCOST.pmon.gz',
                                     literalRunargs = ['BSRDOInput = runArgs.inputBSFile',
                                                       'EvtMax = runArgs.maxEvents']))

    #add default reconstruction steps
    # eventually to be replaced by:
    #from RecJobTransforms.recTransformUtils import addRecoSubsteps
    #addRecoSubsteps(executorSet)
       
    # one difference to reco_tf is that NTUP_TRIG has to be added - needs pyjobtf update before can use above    
        #NTUP_TRIG is added as is not available in ATLASP1HLT, but is available in the reco release
        #hence can be used later in a ATLASP1HLT job if switch releases

    executorSet.add(athenaExecutor(name = 'RAWtoESD', skeletonFile = 'RecJobTransforms/skeleton.RAWtoESD_tf.py',
                                   substep = 'r2e', inData = ['BS', 'RDO'], outData = ['ESD', 'HIST_ESD_INT'], 
                                   perfMonFile = 'ntuple_RAWtoESD.pmon.gz'))
    executorSet.add(athenaExecutor(name = 'ESDtoAOD', skeletonFile = 'RecJobTransforms/skeleton.ESDtoAOD_tf.py',
                                   substep = 'e2a', inData = ['ESD'], outData = ['AOD', 'HIST_AOD_INT'], 
                                   perfMonFile = 'ntuple_ESDtoAOD.pmon.gz'))
    executorSet.add(DQMergeExecutor(name = 'DQHistogramMerge', inData = [('HIST_ESD_INT', 'HIST_AOD_INT')], outData = ['HIST']))
    executorSet.add(athenaExecutor(name = 'ESDtoDPD', skeletonFile = 'PATJobTransforms/skeleton.ESDtoDPD_tf.py',
                                   substep = 'e2d', inData = ['ESD'], outData = ['NTUP_TRIG'],
                                   perfMonFile = 'ntuple_ESDtoDPD.pmon.gz'))
    executorSet.add(athenaExecutor(name = 'AODtoDPD', skeletonFile = 'PATJobTransforms/skeleton.AODtoDPD_tf.py',
                                   substep = 'a2d', inData = ['AOD', 'EVNT'], outData = ['NTUP_TRIG'],
                                   perfMonFile = 'ntuple_AODtoDPD.pmon.gz'))
    executorSet.add(athenaExecutor(name = 'AODtoTAG', skeletonFile = 'RecJobTransforms/skeleton.AODtoTAG_tf.py',
                                   inData = ['AOD'], outData = ['TAG'],))
    executorSet.add(reductionFrameworkExecutor(name = 'AODtoRED', skeletonFile = 'PATJobTransforms/skeleton.AODtoRED_tf.py',
                                   substep = 'a2r', inData = ['AOD'], outData = ['DAOD_RED']))
    executorSet.add(reductionFrameworkExecutorNTUP(name = 'NTUPtoRED', skeletonFile = 'PATJobTransforms/skeleton.NTUPtoRED_tf.py',
                                   substep = 'n2n', inData = ['NTUP_COMMON'], outData = ['NTUP_RED']))
    
    trf = transform(executor = executorSet, description = 'Trigger transform to run HLT_standalone, followed by'
                    ' general purpose ATLAS reconstruction transform. Input to HLT_Standalone is inputBS_RDOFile'
                    ' with outputs of RDO, ESD, AOD or DPDs. For more details see:'
                    ' https://twiki.cern.ch/twiki/bin/viewauth/Atlas/TriggerTransform or for reco_tf, see:'
                    ' https://twiki.cern.ch/twiki/bin/viewauth/Atlas/RecoTf')
    
    #add arguments as donw in reco_tf
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addCommonRecTrfArgs(trf.parser)
    addStandardRecoFiles(trf.parser)
    addPrimaryDPDArguments(trf.parser, transform = trf)
    addD3PDArguments(trf.parser, transform = trf)
    addExtraDPDTypes(trf.parser, transform = trf)
    addReductionArguments(trf.parser, transform = trf)

    #currently ignoring simulation arguments as can't find the package when have below in cmt/requirements:
    #use SimuJobTransforms SimuJobTransforms-* Simulation
    
    # For digi step - make sure we can add the digitisation/simulation arguments
    # before we add this substep
#    try:
#        from SimuJobTransforms.simTrfArgs import addForwardDetTrfArgs, addBasicDigiArgs, addPileUpTrfArgs, addCommonSimDigTrfArgs
#        addBasicDigiArgs(trf.parser)
#        addForwardDetTrfArgs(trf.parser)
#        addPileUpTrfArgs(trf.parser)
#        #addCommonSimDigTrfArgs(trf.parser)
#        executorSet.add(athenaExecutor(name = 'HITtoRDO', skeletonFile = 'SimuJobTransforms/skeleton.HITtoRDO.py',
#                                       substep = 'h2r', tryDropAndReload = False, perfMonFile = 'ntuple.pmon.gz',
#                                       inData = ['Hits'], outData = ['RDO'],
#                                       runtimeRunargs = {'LowPtMinbiasHitsFile' : 'runArgs.inputLowPtMinbiasHitsFile',
#                                                         'HighPtMinbiasHitsFile' : 'runArgs.inputHighPtMinbiasHitsFile',}))
#        
#    except ImportError:
#        msg.warning('Failed to import simulation/digitisation arguments. These substeps will not be available.')

    #now add specific trigger arguments
    #  putting this last makes them appear last in the help so easier to find
    addTriggerArgs(trf.parser)
    addTrigCostArgs(trf.parser)
    addTriggerDBArgs(trf.parser)
    addDebugArgs(trf.parser)

    return trf


def addTriggerArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('Trigger', 'Specific options related to the trigger configuration used for reprocessing')
    
    #arguments specific for trigger transform
    #writeBS used in literal arguments when running HLT step in athena (not athenaHLT)
    parser.add_argument('--writeBS', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                          help='Needed if running BSRDO to BS step in athena (default: True)', group='Trigger', default=trfArgClasses.argBool(True, runarg=True))
    #input BS file for the HLT step (name just to be unique identifier)
    parser.add_argument('--inputBS_RDOFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='input', runarg=True, type='bs'),
                        help='Input bytestream file', group='Trigger')
    #without an outputBSFile name specified then any further steps will know to use tmp.BS
    parser.add_argument('--outputBSFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='output', runarg=True, type='bs'),
                        help='Output bytestream file', group='Trigger')
    #select output stream in  BS file
    #athenaHLT writes All streams into one file, but this can't be proceesed by standard reco if it contains events in only PEB streams
    #by defualt selects the Main stream, as likely the most needed option, but can ber reverted to All or any other stream chosen
    parser.add_argument('--streamSelection', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='select output stream in  BS file (default: \"Main\"). Specify \"All\" to disable splitting (standard reco will fail on any events with only PEB data)', group='Trigger', default=trfArgClasses.argString("Main", runarg=True))
    #HLT out histogram file, if defined renames expert-monitoring file that is produced automatically
    parser.add_argument('--outputHIST_HLTMONFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='output', runarg=True, countable=False),
                        help='Output HLTMON file', group='Trigger')    
    #Trigger Configuration String as used in reco Steps
    parser.add_argument('--triggerConfig', nargs='+', metavar='substep=TRIGGERCONFIG',
                        type=trfArgClasses.argFactory(trfArgClasses.argSubstep, runarg=True, separator='='),
                        help='Trigger Configuration String. '
			'N.B. This argument uses EQUALS (=) to separate the substep name from the value.', group='Trigger')
    #precommand for athenaHLT aka -c
    parser.add_argument('--precommand', nargs='+', type=trfArgClasses.argFactory(trfArgClasses.argList, runarg=True),
                        help='precommand for athenaHLT aka -c', group='Trigger')
    #postcommand for athenaHLT aka -C
    parser.add_argument('--postcommand', nargs='+', type=trfArgClasses.argFactory(trfArgClasses.argList, runarg=True),
                        help='postcommand for athenaHLT aka -C', group='Trigger')
    #For prodsys to make sure uses inputBS_RDOFile rather than inputBSFile when running the b2r step
    parser.add_argument('--prodSysBSRDO', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='For prodsys to make sure uses inputBS_RDOFile rather than inputBSFile when running the b2r step', group='Trigger')


def addTrigCostArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('TrigCost', 'Specific options related to the trigger costmon steps in trigger reprocessings')

    #without a outputBS_TRIGCOSTFile name specified then it will not be possible to run any further COST mon if the BS is slimmed to a specific stream
    parser.add_argument('--outputBS_TRIGCOSTFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='output', runarg=True, type='bs'),
                        help='Output bytestream file of COST stream', group='TrigCost')
    #input BS file for the TRIGCOST step (name just to be unique identifier)
    parser.add_argument('--inputBS_TRIGCOSTFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='input', runarg=True, type='bs'),
                        help='Input bytestream file of COST stream', group='TrigCost')
    #For prodsys to make sure uses inputBS_TRIGCOSTFile rather than inputBSFile when running the r2c step
    parser.add_argument('--prodSysBSTRIGCOST', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='For prodsys to make sure uses inputBS_TRIGCOSTFile rather than inputBSFile when running the r2c step', group='TrigCost')
    #NTUP_TRIG is used for COST monitoring - used in the reco release
    parser.add_argument('--outputNTUP_TRIGFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='output', runarg=True, countable=False),
                        help='D3PD output NTUP_TRIG file (can be made in substeps e2d,a2d)', group='TrigCost')
    #NTUP_COST is used for COST monitoring - used in the reco release
    parser.add_argument('--outputNTUP_TRIGCOSTFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='output', runarg=True, countable=False),
                        help='D3PD output NTUP_TRIGCOST file', group='TrigCost')
    #NTUP_RATE is used for COST monitoring - used in the reco release
    parser.add_argument('--outputNTUP_TRIGRATEFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='output', runarg=True, countable=False),
                        help='D3PD output NTUP_TRIGRATE file', group='TrigCost')
    #NTUP_TRIGEBWGHT is used for COST monitoring - used in the reco release
    parser.add_argument('--outputNTUP_TRIGEBWGHTFile', nargs='+',
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='output', runarg=True, countable=False),
                        help='D3PD output NTUP_TRIGEBWGHT file', group='TrigCost')


def addTriggerDBArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('TriggerDB', 'Specific options related to the trigger DB')
    
    parser.add_argument('--useDB', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='read from DB for athenaHLT aka use-database', group='TriggerDB')
    parser.add_argument('--DBserver', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='DB name: aka athenaHLT db-server', group='TriggerDB')
    parser.add_argument('--DBsmkey', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='DB SMK', group='TriggerDB')
    parser.add_argument('--DBhltpskey', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='DB hltpskey', group='TriggerDB')
    parser.add_argument('--DBl1pskey', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='DB l1pskey', group='TriggerDB')


def addDebugArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('Debug', 'Specific options related to the trigger debug recovery')

    parser.add_argument('--debug_stream', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Runs debug_stream analysis', group='Debug')
    parser.add_argument('--outputHIST_DEBUGSTREAMMONFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='output', runarg=True, countable=False),
                        help='Output DEBUGSTREAMMON file', group='Debug')    
    

if __name__ == '__main__':
    main()
