#!/bin/sh
#
# art-description: Run digitization of an MC16a ttbar sample with 2016 geometry and conditions, without pile-up using Athena and AthenaMT with one thread
# art-type: grid
# art-athena-mt: 8
# art-include: master/Athena
# art-output: mc16a_ttbar.ST.RDO.pool.root
# art-output: mc16a_ttbar.MT.RDO.pool.root
# art-output: log.*

export ATHENA_CORE_NUMBER=1

Digi_tf.py \
--multithreaded \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1 \
--conditionsTag default:OFLCOND-MC16-SDR-16 \
--digiSeedOffset1 170 \
--digiSeedOffset2 170 \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--DataRunNumber 284500 \
--outputRDOFile mc16a_ttbar.MT.RDO.pool.root \
--postExec 'default:condSeq.LArAutoCorrTotalCondAlg.deltaBunch=1' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preExec 'all:from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True)' \
--preInclude 'HITtoRDO:Digitization/ForceUseOfAlgorithms.py' \
--skipEvents 0 \
--maxEvents 10

rc=$?
echo  "art-result: $rc MTdigi"
mv log.HITtoRDO log.HITtoRDO_MT

rc2=-9999
if [ $rc -eq 0 ]
then
    Digi_tf.py \
    --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1 \
    --conditionsTag default:OFLCOND-MC16-SDR-16 \
    --digiSeedOffset1 170 \
    --digiSeedOffset2 170 \
    --geometryVersion default:ATLAS-R2-2016-01-00-01 \
    --DataRunNumber 284500 \
    --outputRDOFile mc16a_ttbar.ST.RDO.pool.root \
    --postExec 'default:condSeq.LArAutoCorrTotalCondAlg.deltaBunch=1' \
    --postInclude 'default:PyJobTransforms/UseFrontier.py' \
    --preExec 'all:from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True)' \
    --preInclude 'HITtoRDO:Digitization/ForceUseOfAlgorithms.py' \
    --skipEvents 0 \
    --maxEvents 10
    rc2=$?
fi
echo  "art-result: $rc2 STdigi"

rc3=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root mc16a_ttbar.ST.RDO.pool.root mc16a_ttbar.MT.RDO.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_HITStoRDO_timings index_ref
    rc3=$?
fi
echo  "art-result: $rc3 comparison"

rc4=-9999
if [ $rc2 -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --error-mode resilient --mode=semi-detailed --order-trees
    rc4=$?
fi
echo  "art-result: $rc4 regression"
