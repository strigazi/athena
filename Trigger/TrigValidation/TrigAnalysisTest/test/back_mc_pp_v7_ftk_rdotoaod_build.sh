#!/bin/bash

# art-description: Trigger MC pp v7 RDO to AOD test FTK
# art-type: build
# art-include: 21.1/AthenaP1
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export NAME="mc_pp_v7_ftk_rdotoaod_build"
export INPUT="ftk"
export COST_MONITORING="False"
export TEST="TrigAnalysisTest"
export MENU="MC_pp_v7"
export EVENTS="3"
export JOBOPTION="TrigAnalysisTest/testAthenaTrigRDOtoAOD.py"
export EXTRA="from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.doFTK=True;from RecExConfig.RecFlags import rec;rec.doFTK.set_Value_and_Lock(True);"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh