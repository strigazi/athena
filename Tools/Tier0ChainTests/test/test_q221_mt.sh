#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena

Reco_tf.py --AMI=q221 --athenaopts='--threads=1' --outputRDOFile=myRDO.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --imf False
echo "art-result: $? Reco"

Reco_tf.py --validationFlags 'doExample,doMET,doPFlow,doTau,doEgamma,doBtag,doZee,doJet,doTopoCluster,doMuon,doTrigMinBias,doTrigIDtrk,doTrigBphys,doTrigMET,doTrigJet,doTrigTau, doTrigEgamma,doTrigMuon,doTrigBjet,doTrigHLTResult' --inputAODFile=myAOD.pool.root  --outputNTUP_PHYSVALFile=myNTUP_PHYSVAL.root --preExec="TriggerFlags.EDMDecodingVersion.set_Value_and_Lock(3)"
echo "art-result: $? PhysVal"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo "art-result: $? Diff"
