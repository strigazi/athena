# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from collections import defaultdict
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponentsNaming import CFNaming
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import buildFilter, makeSummary
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFDot import stepCF_DataFlow_to_dot, \
    stepCF_ControlFlow_to_dot, all_DataFlow_to_dot
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CFSequence
from AthenaCommon.CFElements import parOR, seqAND
from AthenaCommon.Logging import logging

log = logging.getLogger('HLTCFConfig_newJO')

def printStepsMatrix(matrix):
    print('----- Steps matrix ------')
    for nstep in matrix:
        print('step {}:'.format(nstep))
        for chainName in matrix[nstep]:
            namesInCell = map(lambda el: el.name, matrix[nstep][chainName])
            print('---- {}: {}'.format(chainName, namesInCell))
    print('-------------------------')


def generateDecisionTree(chains):
    acc = ComponentAccumulator()
    mainSequenceName = 'HLTAllSteps'
    acc.addSequence( seqAND(mainSequenceName) )

    log.debug('Generating decision tree with main sequence: {}'.format(mainSequenceName))

    chainStepsMatrix = defaultdict(lambda: defaultdict(list))

    ## Fill chain steps matrix
    for index, chain in enumerate(chains):
        for stepNumber, chainStep in enumerate(chain.steps):
            chainStepsMatrix[stepNumber][chainStep.name].append(chain)

    printStepsMatrix(chainStepsMatrix)

    allCFSequences = []

    ## Matrix with steps lists generated. Creating filters for each cell
    for nstep in sorted(chainStepsMatrix.keys()):
        stepDecisions = []

        stepName = CFNaming.stepName(nstep)

        stepFilterNodeName = '{}{}'.format(stepName, CFNaming.FILTER_POSTFIX)
        filterAcc = ComponentAccumulator()
        filterAcc.addSequence( parOR(stepFilterNodeName) )

        stepRecoNodeName = CFNaming.stepRecoNodeName(mainSequenceName, stepName)
        stepRecoNode = parOR(stepRecoNodeName)
        recoAcc = ComponentAccumulator()
        recoAcc.addSequence(stepRecoNode)

        CFSequences = []

        for chainName in chainStepsMatrix[nstep]:
            chainsInCell = chainStepsMatrix[nstep][chainName]

            if not chainsInCell:
                # If cell is empty, there is nothing to do
                continue

            firstChain = chainsInCell[0]
            if nstep == 0:
                filter_input = firstChain.group_seed
            else:
                filter_input = [output for sequence in firstChain.steps[nstep - 1].sequences for output in sequence.outputs]

            chainStep = firstChain.steps[nstep]

            # One aggregated filter per chain (one per column in matrix)
            filterName = CFNaming.filterName(chainStep.name)
            sfilter = buildFilter(filterName, filter_input)
            filterAcc.addEventAlgo(sfilter.Alg, sequenceName = stepFilterNodeName)

            stepReco = parOR('{}{}'.format(chainStep.name, CFNaming.RECO_POSTFIX))
            stepView = seqAND('{}{}'.format(chainStep.name, CFNaming.VIEW_POSTFIX), [stepReco])
            viewWithFilter = seqAND(chainStep.name, [sfilter.Alg, stepView])
            recoAcc.addSequence(viewWithFilter, parentName = stepRecoNodeName)

            stepsAcc = ComponentAccumulator()

            CFSequenceAdded = False

            for chain in chainsInCell:
                step = chain.steps[nstep]
                CFSeq = CFSequence(step, sfilter)
                if not CFSequenceAdded:
                    CFSequences.append(CFSeq)
                    CFSequenceAdded = True
                for seq in step.sequences:
                    if seq.ca is None:
                        raise ValueError('ComponentAccumulator missing in sequence {} in chain {}'.format(seq.name, chain.name))
                    stepsAcc.merge( seq.ca )
                    recoAcc.addEventAlgo(seq.hypo.Alg, sequenceName = stepView.getName())
                if step.isCombo:
                    recoAcc.addEventAlgo(step.combo.Alg, sequenceName = stepView.getName())
                sfilter.setChains(chain.name)

            recoAcc.merge(stepsAcc, sequenceName = stepReco.getName())

            for sequence in chainStep.sequences:
                stepDecisions += sequence.outputs

        acc.merge(filterAcc, sequenceName = mainSequenceName)
        acc.merge(recoAcc, sequenceName = mainSequenceName)

        summary = makeSummary('TriggerSummary{}'.format(stepName), stepDecisions)
        acc.addSequence(summary, parentName = mainSequenceName)

        allCFSequences.append(CFSequences)

        stepCF_DataFlow_to_dot(stepRecoNodeName, CFSequences)
        stepCF_ControlFlow_to_dot(stepRecoNode)

    acc.printConfig()

    all_DataFlow_to_dot(mainSequenceName, allCFSequences)

    return acc
