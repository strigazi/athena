#!/bin/sh

# art-description: DAOD building EXOT18 data17
# art-type: grid
# art-output: *.pool.root

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data17_13TeV.00327342.physics_Main.merge.AOD.f838_m1824._lb0300._0001.1 --outputDAODFile art.pool.root --reductionConf EXOT18 --maxEvents 8000 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-40" ' 

DAODMerge_tf.py --maxEvents 5 --inputDAOD_EXOT18File DAOD_EXOT18.art.pool.root --outputDAOD_EXOT18_MRGFile art_merged.pool.root
checkFile.py DAOD_EXOT18.art.pool.root > checkFile.txt