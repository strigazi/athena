#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# First go at making a test for DigiMReco using the python
# unit test framework
#

import unittest

from RecJobTransforms.DigiMReco_trf import DigiMultiTriggerRecoTrf

class TestSequenceFunctions(unittest.TestCase):
    def test_momAndApplePie(self):
        self.assertEqual(2, 2)
    
    def test_fullChain(self):
        '''DigiMReco from RDO to ADO/TAG'''
        # Define the inputDictionary
        inDic = {'inputHitsFile' : 'root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc11a/mc11_7TeV.105200.T1_McAtNlo_Jimmy.merge.HITS.e835_s1310_s1300/HITS.508808._000857.pool.root.1',
                 'tmpRDO' : 'tmp.RDO.pool.root',
                 'outputESDFile' : 'my.ESD.pool.root',
                 'outputAODFile' : 'my.ADO.pool.root',
                 'outputTAGFile' : 'my.TAG.root',
                 'maxEvents' : '10',
                 'triggerConfigByRun' : '{180164:"MCRECO:DB:TRIGGERDBMC:301,134,246",183003:"MCRECO:DB:TRIGGERDBMC:301,134,246",186169:"MCRECO:DB:TRIGGERDBMC:301,134,246",186275:"MCRECO:DB:TRIGGERDBMC:303,139,248"}',
                 'preInclude_h2r' : 'Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrains2011Config8_DigitConfig.py,RunDependentSimData/configLumi_mc11b_v1.py',
                 'preExec_h2r' : 'from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.overrideMetadata+=["SimLayout","PhysicsList"]',
                 'postExec_h2r' : 'ToolSvc.LArAutoCorrTotalToolDefault.NMinBias=0',
                 'preExec_r2e' : 'rec.Commissioning.set_Value_and_Lock(True);jobproperties.Beam.energy.set_Value_and_Lock(3500*Units.GeV);muonRecFlags.writeSDOs=True;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(8.0);jobproperties.Beam.bunchSpacing.set_Value_and_Lock(50);from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import L2IDTrackingCuts;L2IDTrackingCuts.setRegSelZmax(225*Units.mm)',
                 'postExec_r2e' : 'RegSelSvc=topSequence.allConfigurables.get("RegSelSvcDefault");RegSelSvc.DeltaZ=225*Units.mm',
                 'preExec_e2a' : 'TriggerFlags.AODEDMSet="AODSLIM";rec.Commissioning.set_Value_and_Lock(True);jobproperties.Beam.energy.set_Value_and_Lock(3500*Units.GeV);muonRecFlags.writeSDOs=True;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(8.0);jobproperties.Beam.bunchSpacing.set_Value_and_Lock(50)',
                 'RunNumber' : '105200',
                 'autoConfiguration' : 'everything',
                 'conditionsTag' : 'OFLCOND-SDR-BS7T-05-12',
                 'geometryVersion' : 'ATLAS-GEO-18-01-00',
                 'numberOfLowPtMinBias' : '29.966',
                 'numberOfHighPtMinBias' : '0.034',
                 'LowPtMinbiasHitsFile' : 'root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc11a/mc11_7TeV.108118.Pythia8_minbias_Inelastic_low.merge.HITS.e816_s1299_s1303/HITS.500617._000382.pool.root.1',
                 'HighPtMinbiasHitsFile' : 'root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc11a/mc11_7TeV.108119.Pythia8_minbias_Inelastic_high.merge.HITS.e848_s1299_s1303/HITS.500616._001496.pool.root.1',
                 'DataRunNumber' : '-1',
                 'jobNumber' : '41',
                 'digiSeedOffset1' : '41',
                 'digiSeedOffset2' : '41',
                 }
        print inDic
        trf = DigiMultiTriggerRecoTrf(inDic)
        trf._lastInChain=True
        self.assertEqual(trf.exeArgDict(inDic).exitCode(), 0)

if __name__ == '__main__':
    unittest.main()


        
