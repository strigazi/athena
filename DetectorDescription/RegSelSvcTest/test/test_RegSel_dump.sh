#!/bin/sh
# art-type: build
# art-include: master/Athena
# art-ci: master

rm -rf test_RegSel_dump
mkdir test_RegSel_dump
cd test_RegSel_dump
athena --threads=1 --evtMax=10 --filesInput="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1" -c 'enableViews=True;TestType="dump"' RegSelSvcTest/testRegSelSvc.py
