#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
from __future__ import print_function

import ast
import collections
import json
import pickle
import pprint
import re
import sys


def main(args):
    if args.printComps:
        for fileName in args.file:
            conf = _loadSingleFile(fileName, args)
            _printComps(conf)

    if args.printConf:
        for fileName in args.file:
            conf = _loadSingleFile(fileName, args)
            _print(conf)

    if args.toJSON:
        if len(args.file) != 1:
            sys.exit("ERROR, can convert single file at a time, got: %s" % args.file)
        conf = _loadSingleFile(args.file[0], args)
        with open(args.toJSON, "w") as oFile:
            json.dump(conf, oFile, indent=2, ensure_ascii=True)

    if args.toPickle:
        if len(args.file) != 1:
            sys.exit("ERROR, can convert single file at a time, got: %s" % args.file)
        conf = _loadSingleFile(args.file[0], args)
        with open(args.toPickle, "wb") as oFile:
            for item in conf:
                pickle.dump(item, oFile)

    if args.diff:
        if len(args.file) != 2:
            sys.exit("ERROR, can diff exactly two files at a time, got: %s" % args.file)
        configRef = _loadSingleFile(args.file[0], args)
        configChk = _loadSingleFile(args.file[1], args)
        for ref, chk in zip(configRef, configChk):
            if isinstance(ref, dict) and isinstance(chk, dict):
                _compareConfig(ref, chk, args)
            else:
                print("Given list of size %d. Not looking for differences." % len(ref))


def _loadSingleFile(fname, args):
    conf = []
    if fname.endswith(".pkl"):
        with open(fname, "rb") as input_file:
            conf_dict = collections.defaultdict()
            while True:
                try:
                    cfg = pickle.load(input_file)
                    if type(cfg) == collections.defaultdict:
                        conf_dict.update(cfg)
                    else:
                        conf.append(cfg)
                except EOFError:
                    break
            conf.append(conf_dict)
        print("... Read", len(conf), "items from python pickle file: ", fname)
    elif fname.endswith(".json"):

        def __keepPlainStrings(element):
            if isinstance(element, str):
                return str(element)
            if isinstance(element, list):
                return [__keepPlainStrings(x) for x in element]
            if isinstance(element, dict):
                return {
                    __keepPlainStrings(key): __keepPlainStrings(value)
                    for key, value in element.items()
                }
            return element

        with open(fname, "r") as input_file:
            conf = json.load(input_file, object_hook=__keepPlainStrings)

            print("... Read", len(conf), "items from JSON file: ", fname)

    else:
        sys.exit("File format not supported.")

    if conf is None:
        sys.exit("Unable to load %s file" % fname)

    if args.comps:  # returning only wanted components

        def is_component_from_list(component):
            return True in [s in component for s in compsToReport]

        compsToReport = [
            item for elem in args.comps for item in elem
        ]  # creates flat list of wanted components
        conf = [
            {key: value for (key, value) in dic.items() if is_component_from_list(key)}
            for dic in conf
            if isinstance(dic, dict)
        ]
    return conf


def _print(conf):
    for item in conf:
        pprint.pprint(dict(item))


def _printComps(conf):
    for item in conf:
        if isinstance(item, dict):
            for compName in item.keys():
                print(compName)


def _compareConfig(configRef, configChk, args):
    # Find superset of all components:
    allComps = list(set(configRef.keys()) | set(configChk.keys()))
    allComps.sort()

    print("Step 1: reference file #components:", len(configRef))
    print("Step 2: file to check  #components:", len(configChk))

    for component in allComps:

        if component not in configRef:
            if not args.ignoreMissing:
                print(
                    "\n\033[91m Component ",
                    component,
                    " \033[94m exists only in Chk \033[0m \033[0m \n",
                )
            continue

        if component not in configChk:
            if not args.ignoreMissing:
                print(
                    "\n\033[91m Component",
                    component,
                    " \033[92m exists only in Ref \033[0m  \033[0m \n",
                )
            continue

        refValue = configRef[component]
        chkValue = configChk[component]

        if chkValue == refValue:
            if args.printIdenticalComponents:
                print("Component", component, "identical")
        else:
            print("\033[91m Component", component, "differ \033[0m")
            if not args.allComponentPrint:
                _compareComponent(refValue, chkValue, "\t", args, component)
            else:
                print(
                    "\t\033[92mRef\033[0m\t",
                    sorted(configRef[component].items(), key=lambda kv: kv[0]),
                )
                print(
                    "\t\033[94mChk\033[0m\t",
                    sorted(configChk[component].items(), key=lambda kv: kv[0]),
                )


def _compareComponent(compRef, compChk, prefix, args, component):

    if isinstance(compRef, dict):

        allProps = list(set(compRef.keys()) | set(compChk.keys()))
        allProps.sort()

        ignoreList = ["StoreGateSvc", "OutputLevel", "MuonEDMHelperSvc"]
        for prop in allProps:
            if prop not in compRef.keys():
                print(
                    "%s%s = %s: \033[94m exists only in Chk \033[0m \033[91m<< !!!\033[0m"
                    % (prefix, prop, compChk[prop])
                )
                continue

            if prop not in compChk.keys():
                print(
                    "%s%s = %s: \033[92m exists only in Ref \033[0m \033[91m<< !!!\033[0m"
                    % (prefix, prop, compRef[prop])
                )
                continue

            refVal = compRef[prop]
            chkVal = compChk[prop]

            if args.ignoreIrrelevant and chkVal in ignoreList:
                continue

            if str(chkVal) == str(refVal):
                if not args.printIdenticalPerParameter:
                    continue
                diffmarker = ""
            else:
                diffmarker = " \033[91m<< !!!\033[0m"

            if not (component == "IOVDbSvc" and prop == "Folders"):
                print(
                    "%s%s : \033[92m %s \033[0m vs \033[94m %s \033[0m %s"
                    % (prefix, prop, str(refVal), str(chkVal), diffmarker)
                )

            try:
                refVal = ast.literal_eval(str(refVal)) if refVal else ""
                chkVal = ast.literal_eval(str(chkVal)) if chkVal else ""
            except SyntaxError:
                pass
            except ValueError:
                pass  # literal_eval exception when parsing particular strings

            if refVal and (isinstance(refVal, list) or isinstance(refVal, dict)):
                if component == "IOVDbSvc" and prop == "Folders":
                    _compareIOVDbFolders(refVal, chkVal, "\t", args)
                else:
                    _compareComponent(
                        refVal, chkVal, "\t" + prefix + ">> ", args, component
                    )

    elif isinstance(compRef, (list, tuple)) and len(compRef) > 1:
        if isinstance(compRef[0], list): # to achieve hashability
            compChk = [tuple(el) for el in compRef]
        if len(compChk) > 0 and isinstance(compChk[0], list):
            compChk = [tuple(el) for el in compChk]

        diffRef = list(set(compRef) - set(compChk))
        diffChk = list(set(compChk) - set(compRef))

        if diffRef:
            print(
                "%s exists only in Ref : \033[92m %s \033[0m \033[91m<< !!!\033[0m"
                % (prefix, str(diffRef))
            )
        if diffChk:
            print(
                "%s exists only in Chk : \033[94m %s \033[0m \033[91m<< !!!\033[0m"
                % (prefix, str(diffChk))
            )

        if len(compRef) == len(compChk):
            if sorted(compRef) == sorted(compChk):
                print("%s : \033[91m ^^ Different order ^^ !!!\033[0m" % (prefix))
            else:
                for i, (refVal, chkVal) in enumerate(zip(compRef, compChk)):
                    if refVal != chkVal:
                        print(
                            "%s : \033[92m %s \033[0m vs \033[94m %s \033[0m \033[91m<< at index %s !!!\033[0m"
                            % (prefix, str(refVal), str(chkVal), str(i))
                        )
                        _compareComponent(
                            refVal, chkVal, "\t" + prefix + ">> ", args, ""
                        )


def _parseIOVDbFolder(definition):
    result = {}
    # db
    db_match = re.search(r"<db>(.*)</db>", definition)
    if db_match:
        result["db"] = db_match.group(1)
        definition = definition.replace(db_match.group(0), "")
    # key
    key_match = re.search(r"<key>(.*)</key>", definition)
    if key_match:
        result["key"] = key_match.group(1)
        definition = definition.replace(key_match.group(0), "")
    # tag
    tag_match = re.search(r"<tag>(.*)</tag>", definition)
    if tag_match:
        result["tag"] = tag_match.group(1)
        definition = definition.replace(tag_match.group(0), "")
    # cache -- ignore for now
    cache_match = re.search(r"<cache>(.*)</cache>", definition)
    if cache_match:
        definition = definition.replace(cache_match.group(0), "")
    # noover
    noover_match = re.search(r"<noover/>", definition)
    if noover_match:
        result["noover"] = True
        definition = definition.replace(noover_match.group(0), "")
    # name
    result["name"] = definition.strip()

    return json.dumps(result)


def _compareIOVDbFolders(compRef, compChk, prefix, args):
    refParsed = []
    chkParsed = []
    for item in compRef:
        refParsed.append(_parseIOVDbFolder(item))
    for item in compChk:
        chkParsed.append(_parseIOVDbFolder(item))
    _compareComponent(refParsed, chkParsed, prefix, args, "")
