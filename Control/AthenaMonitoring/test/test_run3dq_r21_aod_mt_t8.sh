#!/bin/bash
# art-description: AOD->HIST, R21 data AOD, Threads=8
# art-type: grid
# art-include: master/Athena
# art-output: ExampleMonitorOutput.root
# art-output: log*
# art-athena-mt: 8

Run3DQTestingDriver.py 'Input.Files=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ASG/data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1"]' DQ.Steering.doHLTMon=False Concurrency.NumThreads=8 Concurrency.NumConcurrentEvents=8 DQ.Environment=AOD  > log.HIST_Creation 2>&1

echo "art-result: $? HIST_Creation"

ArtPackage=$1
ArtJobName=$2
art.py download ${ArtPackage} ${ArtJobName}
REFFILE=(./ref-*/ExampleMonitorOutput.root)
hist_diff.sh ExampleMonitorOutput.root $REFFILE -x TIME_execute -i -t > log.HIST_Diff 2>&1
echo "art-result: $? HIST_Diff"

art.py download AthenaMonitoring test_run3dq_r21_aod.sh
hist_diff.sh ExampleMonitorOutput.root $REFFILE -x TIME_execute -i -t > log.HIST_Diff_Serial 2>&1
echo "art-result: $? HIST_Diff_Serial"
