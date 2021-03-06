#!/usr/bin/env python

from __future__ import print_function

import os
import sys
from AthenaCommon import ChapPy

from future import standard_library
standard_library.install_aliases()
import subprocess

###-----------------------------------------------------
## For compatibility with ATN tests
from TestTools.iobench import workDir

###-----------------------------------------------------
## Little helper to validate output of jobs
from TestTools.iobench import ScOutput
from TestTools.iobench import BenchSequence

print ("#"*80)
print ("## StoreGate test... [producer/consumer-DataPool-bench]")
print ("#"*80)
print (":::   No DataPool ", end='')
athena = ChapPy.Athena(
    jobOptions = [
    ChapPy.JobOptionsCmd( "SGDATAPOOL=False" ),
    ChapPy.JobOptions("StoreGateTests/test_sgProducerConsumer_jobOptions.py"),
    ChapPy.JobOptionsCmd("jp.PerfMonFlags.OutputFile = 'nodatapool.pmon.gz'"),
    ]
    )
athena.EvtMax = 100
sc = athena.run()
if sc != 0:
    print ("ERROR")
    sys.exit(sc)

print ("::: With DataPool ", end='')
athena = ChapPy.Athena(
    jobOptions = [
    ChapPy.JobOptionsCmd( "SGDATAPOOL=True" ),
    ChapPy.JobOptions("StoreGateTests/test_sgProducerConsumer_jobOptions.py"),
    ChapPy.JobOptionsCmd("jp.PerfMonFlags.OutputFile = 'withdatapool.pmon.gz'"),
    ]
    )
athena.EvtMax = 100
sc = athena.run()
if sc != 0:
    print ("ERROR")
    sys.exit(sc)

sc,out = subprocess.getstatusoutput(
    "perfmon.py %s %s -o %s -l dp,nodp -s \"m.name!=''\"" % (
             "withdatapool.pmon.gz",
             "nodatapool.pmon.gz",
             "datapool.root"
             )
    )
print ("All tests SUCCESSFULLY completed")
    
print ("")
print ("#"*80)
print ("## Bye.")
print ("#"*80)
