#!/bin/sh
#
# art-description: Run simulation outside ISF, using 2011 geometry and conditions, reading single muon events, writing HITS
# art-type: grid

#FIXME need to copy input file to CVMFS
AtlasG4_tf.py --inputEVNTFile '/afs/cern.ch/atlas/offline/ProdData/16.6.X/16.6.7.Y/mu_E200_eta0-25.evgen.pool.root' --outputHITSFile 'test.HITS.pool.root' --maxEvents '10' --skipEvents '0' --randomSeed '10' --geometryVersion 'ATLAS-R1-2011-02-00-00' --conditionsTag 'OFLCOND-RUN12-SDR-19' --DataRunNumber '180164' --physicsList 'FTFP_BERT' --postInclude 'PyJobTransforms/UseFrontier.py'

SCRIPT_DIRECTORY=$1
PACKAGE=$2
TYPE=$3
TEST_NAME=$4
NIGHTLY_RELEASE=$5
PROJECT=$6
PLATFORM=$7
NIGHTLY_TAG=$8

# TODO This is a regression test I think. We would also need to compare these files to fixed references
art.py compare grid $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME test.HITS.pool.root
