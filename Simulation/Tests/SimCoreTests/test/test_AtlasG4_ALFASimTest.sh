#!/bin/sh
#
# art-description: Tests ATLAS + ALFA simulation, generating events on-the-fly
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-type: grid
# art-output: test.HITS.pool.root
# art-output: truth.root

AtlasG4_tf.py \
--preInclude 'G4AtlasTests/preInclude.ALFA_SingleParticle.py' \
--outputHITSFile 'test.HITS.pool.root' \
--maxEvents '3' \
--randomSeed '10' \
--geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--DataRunNumber '222525' \
--physicsList 'FTFP_BERT' \
--ALFAOn 'True' \
--postInclude 'PyJobTransforms/UseFrontier.py' 'AtlasG4Tf:G4AtlasTests/postInclude.ALFA_dcube.py' \
--imf False

echo  "art-result: $? simulation"

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references and add DCube tests
art.py compare grid --entries 3 ${ArtPackage} ${ArtJobName} --mode=semi-detailed

echo  "art-result: $? regression"
