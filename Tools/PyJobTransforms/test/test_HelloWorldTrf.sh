#!/bin/bash

set -e

# ST test
HelloWorld_tf.py --maxEvents=5 --CA 

grep 'runArgs.threads = 0' runargs.athena.py
grep 'runArgs.concurrentEvents = 0' runargs.athena.py

# MT test 1
HelloWorld_tf.py --maxEvents=5 --CA --athenaopts="--threads=2 --concurrent-events=1"

grep 'runArgs.threads = 2' runargs.athena.py
grep 'runArgs.concurrentEvents = 1' runargs.athena.py

# MT test 2
ATHENA_CORE_NUMBER=2 HelloWorld_tf.py --maxEvents=5 --CA --multithreaded

grep 'runArgs.threads = 2' runargs.athena.py
grep 'runArgs.concurrentEvents = 2' runargs.athena.py
