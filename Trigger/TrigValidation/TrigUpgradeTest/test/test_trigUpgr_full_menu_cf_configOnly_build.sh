#!/bin/bash
# art-description: athenaMT trigger test checking only configuration of the full_menu_cf job options
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export SKIPEVENTS=10
export EVENTS=20
export THREADS=1
export SLOTS=1
export JOBOPTION="TrigUpgradeTest/full_menu_cf.py"
export ATHENAOPTS="--config-only=config.pkl"

# This test doesn't produce any output useful for RegTest, so do a dummy reference comparison
export REGTESTEXP="leaving with code"
export REGTESTREF=`find_data.py TrigUpgradeTest/dummy_regtest.ref`

# Skip dumping chain counts because this test doesn't produce the histogram including them
export SKIP_CHAIN_DUMP=1

source exec_TrigUpgradeTest_art_athenaMT.sh
source exec_TrigUpgradeTest_art_post.sh
