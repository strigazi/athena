#!/bin/sh
#
# art-description: Run simulation outside ISF, reading ttbar events, writing HITS, using 2015 geometry and conditions
# art-type: grid

#FIXME need to copy input file to CVMFS
AtlasG4_tf.py --inputEVNTFile '/afs/cern.ch/atlas/offline/ProdData/16.6.X/16.6.7.Y/ttbar_muplusjets-pythia6-7000.evgen.pool.root' --outputHITSFile 'test.HITS.pool.root' --maxEvents '4' --skipEvents '0' --randomSeed '10' --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' --conditionsTag 'OFLCOND-RUN12-SDR-19' --DataRunNumber '222525' --physicsList 'FTFP_BERT' --postInclude 'PyJobTransforms/UseFrontier.py' 'AtlasG4Tf:G4AtlasTests/postInclude.DCubeTest.py' --preExec 'AtlasG4Tf:simFlags.ReleaseGeoModel=False;'

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
