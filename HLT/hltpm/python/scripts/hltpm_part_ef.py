#!/usr/bin/env tdaq_python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Created by Andre Anjos <andre.dos.anjos@cern.ch>
# Tue 21 Aug 2007 12:14:52 PM CEST 

# to bypass variable lenght check in OKS (do it before OKS is loaded)
import os
os.environ['OKS_SKIP_MAX_LENGTH_CHECK'] = 'true'

import pm.project
from pm.partition.utils import parse_cmdline, post_process
from pm.partition.ef import option, gendb

# we modify the default templates file to include hlt specifics
option['template-file']['default'] = 'daq/sw/hlt-templates.data.xml'

import hltpm.common

from hltpm.option import option as hltopt
del hltopt['l2-hlt']
option.update(hltopt)

import os, sys
option['partition-name']['default'] = \
  os.path.basename(sys.argv[0]).split('.', 1)[0].split('_', 1)[1]

args = parse_cmdline(gendb.__doc__, option)

ef_hlt = args['ef-hlt']
args['ef-hlt'] = None 
d = gendb(**args)

# now we change the stuff we need.
hltpm.common.make_hlt_partition(d, l2=None, ef=ef_hlt, 
    extra_area=args['hlt-extra-path'], proxy=args['proxy'],
    templates=pm.project.Project(args['template-file']))
d['partition'] = post_process(args, d['partition'])

save_db = pm.project.Project(d['partition'].id + '.data.xml', d['includes'])
save_db.addObjects([d['partition']])
