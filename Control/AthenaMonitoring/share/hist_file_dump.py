#!/usr/bin/env python
from __future__ import print_function

import ROOT
import sys, os, operator
import argparse
import zlib

parser=argparse.ArgumentParser()
parser.add_argument('filename',
                    help='Input HIST file name')
parser.add_argument('-r', '--rankorder', default='onfile',
                    choices=['onfile', 'uncompressed', 'name'],
                    help='rankorder is "onfile" (default), "uncompressed" or "name"')
parser.add_argument('-p', '--path',
                    help='Only look under this directory')
parser.add_argument('--hash', action='store_true',
                    help='Print hashes of objects')
parser.add_argument('--metadata', action='store_true',
                    help='Include metadata trees')
parser.add_argument('--no_onfile', action='store_true',
                    help="Don't show on file size")
parser.add_argument('--no_inmem', action='store_true',
                    help="Don't show in memory size")
args=parser.parse_args()

ordering = args.rankorder

accounting = {}; hashes = {}; types = {}

ROOT.gInterpreter.LoadText("UInt_t bufferhash(TKey* key) { key->SetBuffer(); key->ReadFile(); UInt_t rv = TString::Hash(key->GetBuffer()+key->GetKeylen(), key->GetNbytes()-key->GetKeylen()); key->DeleteBuffer(); return rv; }")
ROOT.gInterpreter.LoadText("void* getbuffer(TKey* key) { key->SetBuffer(); key->ReadFile(); return (void*) (key->GetBuffer()+key->GetKeylen()); }")
ROOT.gInterpreter.LoadText("UInt_t bufferhash2(TKey* key) { TObject* obj = key->ReadObj(); TMessage msg(kMESS_OBJECT); msg.WriteObject(obj); UInt_t rv = TString::Hash(msg.Buffer(), msg.Length()); delete obj; return rv; }")
ROOT.gInterpreter.LoadText("UInt_t bufferhash3(TKey* key) { TObject* obj = key->ReadObj(); UInt_t rv = obj->Hash(); delete obj; return rv; }")
ROOT.gSystem.Load('libDataQualityUtils')

def dumpdir(d):
    thispath = d.GetPath()
    if ':' in thispath:
        thispath = thispath.split(':', 1)[1]
    #print thispath
    subdirs = []
    for k in d.GetListOfKeys():
        if not args.metadata and k.GetName() == 'metadata' and k.GetClassName() == 'TTree':
            continue
        if k.GetClassName().startswith('TDirectory'):
            subdirs.append(k)
        else:
            if args.hash:
                lhash = ROOT.bufferhash2(k)
            else:
                lhash = 0
            idxname = os.path.join(thispath, k.GetName())
            accounting[idxname] = (k.GetObjlen(), k.GetNbytes()-k.GetKeylen())
            hashes[idxname] = lhash
            types[idxname] = k.GetClassName()
    for k in subdirs:
        dumpdir(k.ReadObj())

f = ROOT.TFile.Open(args.filename)
if args.path:
    d = f.Get(args.path.rstrip('/'))
    if not d:
        print("Can't access path", args.path, "- exiting")
        sys.exit(1)
else:
    d = f
dumpdir(d)

if ordering == 'onfile':
    key=lambda x: (x[1][1], x[1][0], x[0])
elif ordering == 'uncompressed':
    key=lambda x: (x[1][0], x[1][1], x[0])
else:
    key=lambda x: (x[0], x[1][1], x[1][0])
sortedl = sorted(accounting.items(), key=key, reverse=True)
if args.hash:
    print('\n'.join(('%s %s: '
                     + ('%d uncompressed' % b if not args.no_inmem else '')
                     + (', %d on file ' % c if not args.no_onfile else '')
                     + '(hash %s)')
                    % (types[a], a, hashes[a]) for a, (b, c) in  sortedl)
          )
else:
    print('\n'.join(('%s %s: '
                     + ('%d uncompressed' % b if not args.no_inmem else '')
                     + (', %d on file' % c if not args.no_onfile else ''))
                    % (types[a], a) for a, (b, c) in  sortedl)
          )
