# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
def get_config(htag):
    import argparse,sys,os,shutil,ROOT,ast,sys
    import pyAMI,pyAMI.client
    import pyAMI.atlas.api as AtlasAPI
    client = pyAMI.client.Client('atlas')
    AtlasAPI.init()
    
    configs = ast.literal_eval(AtlasAPI.get_ami_tag(client,htag)[0]['phconfig'])['filepaths']['Collisions']
        
    from DataQualityUtils.DQWebDisplayConfig import DQWebDisplayConfig
    import os
    from ._resolve_data_path import resolve_data_path
    
    hcfg_dir = ast.literal_eval(AtlasAPI.get_ami_tag(client,htag)[0]['phconfig'])['filepaths']['basename']
    if hcfg_dir:
        print "Found DataQualityConfigurations data directory %s, using it" % hcfg_dir
    else:
        hcfg_dir = os.getcwd()
        print "DataQualityConfigurations data directory not found, attempting to use $PWD instead."
    print "Looking for collisions_*.hcfg files in %s" % (hcfg_dir)

    isprod = os.environ.get('DQPRODUCTION') == '1'

    dqconfig                = DQWebDisplayConfig()
    dqconfig.config         = "TEST"
    
    dqconfig.hcfg           = "%s%s" % (hcfg_dir, configs['run'])
    dqconfig.hcfg_min10     = "%s%s" % (hcfg_dir, configs['minutes10'])
    dqconfig.hcfg_min30     = "%s%s" % (hcfg_dir, configs['minutes30'])
    
    dqconfig.histogramCache = "/afs/cern.ch/user/a/atlasdqm/w1/histogram_web_display_cache"
    dqconfig.hanResultsDir  = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/han_results/test"
    dqconfig.htmlDir        = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/www/test"
    dqconfig.htmlWeb        = "http://atlasdqm.web.cern.ch/atlasdqm/test"
    dqconfig.runlist        = "runlist_TEST.xml"
    dqconfig.indexFile      = "results_TEST.html"
    dqconfig.lockFile       = "DQWebDisplay_TEST.lock"
    dqconfig.doHandi        = False
    
    return dqconfig
