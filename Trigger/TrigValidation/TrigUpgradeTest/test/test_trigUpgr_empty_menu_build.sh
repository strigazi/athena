#!/bin/bash
# art-description: athenaMT trigger test on data running no HLT menu
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export EVENTS=20
export THREADS=2
export SLOTS=2
export JOBOPTION="TrigUpgradeTest/full_menu.py"
export EXTRA="isOnline=True;doEmptyMenu=True;doWriteBS=False;doWriteRDOTrigger=True;"

# This test doesn't produce any output useful for RegTest, so do a dummy reference comparison
export REGTESTEXP="leaving with code"
export REGTESTREF=`find_data.py TrigUpgradeTest/dummy_regtest.ref`

# Skip dumping chain counts because this test doesn't produce the histogram including them
export SKIP_CHAIN_DUMP=1

source exec_TrigUpgradeTest_art_athenaMT.sh
source exec_TrigUpgradeTest_art_post.sh
