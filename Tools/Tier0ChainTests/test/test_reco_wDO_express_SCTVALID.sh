#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-mc16a/Athena
# art-include: 21.0-mc16d/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena

Reco_tf.py --conditionsTag all:CONDBR2-BLKPA-2017-05 --ignoreErrors 'False' --autoConfiguration='everything' --maxEvents '550' --geometryVersion all:ATLAS-R2-2015-04-00-00 --steering='doRAWtoALL' --inputBSFile='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data15_13TeV.00276689.express_express.merge.RAW._lb0220._SFO-ALL._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data15_13TeV.00276689.express_express.merge.RAW._lb0221._SFO-ALL._0001.1' --outputDAOD_SCTVALIDFile 'myDAOD_SCTVALID.pool.root' --imf False
echo "art-result: $? Reco"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo "art-result: $? Diff"
