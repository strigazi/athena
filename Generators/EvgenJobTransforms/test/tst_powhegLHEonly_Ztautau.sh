#!/bin/bash
# art-description: Generation test Powheg LHE only Z->tautau
# art-type: build
# art-include: 21.6/AthGeneration
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
rm *;
Gen_tf_txt.py --ecmEnergy=13000 --jobConfig=421111 \
    --outputTXTFile=test_powheg_Ztautau.TXT \

echo "art-result: $? generate"


