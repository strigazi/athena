#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''Functions useful for dealing with Trigger ART test scripts'''

import os
import re
from contextlib import contextmanager

def package_prefix(package):
    '''Returns a prefix included in names of all tests from the given package'''
    from TrigValTools.TrigValSteering.Common import package_prefix_dict
    if package == 'ALL':
        return '({})'.format('|'.join(package_prefix_dict.values()))
    elif package in package_prefix_dict.keys():
        return package_prefix_dict[package]
    else:
        return None


def duplicate_filename(list, filename):
    for path in list:
        if os.path.basename(path) == filename:
            return True
    return False


def find_scripts(patterns):
    scripts = []
    for path in os.environ['PATH'].split(os.pathsep):
        try:
            files = os.listdir(path)
        except OSError:
            continue
        for filename in files:
            matched = True
            for patt in patterns:
                if re.search(patt, filename) is None:
                    matched = False
                    break
            if matched and not duplicate_filename(scripts, filename):
                scripts.append(os.path.join(path, filename))
    scripts.sort()
    return scripts

@contextmanager
def remember_cwd():
    '''Simple pushd/popd replacement from https://stackoverflow.com/a/169112'''
    curdir = os.getcwd()
    try:
        yield
    finally:
        os.chdir(curdir)