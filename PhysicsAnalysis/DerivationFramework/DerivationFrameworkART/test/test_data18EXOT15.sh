#!/bin/sh

# art-include
# art-description: DAOD building EXOT15 data18
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/DAOD_RPVLL.13725673._000089.pool.root.1 --outputDAODFile art.pool.root --reductionConf EXOT15 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-40"; from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( "InDetTrackParticlesFixer", Containers = [ "InDetTrackParticlesAux." ] )' 

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_EXOT15File DAOD_EXOT15.art.pool.root --outputDAOD_EXOT15_MRGFile art_merged.pool.root

echo "art-result: $? merge"

checkFile.py DAOD_EXOT15.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_EXOT15.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
