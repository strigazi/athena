#!/usr/bin/env bash
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
#/** @file post.sh
# @brief sh script that checks the return code of an executable and compares
# its output with a reference (if available).
#
# @author Scott Snyder <snyder@fnal.gov> - ATLAS Collaboration.
# @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration.
# @author Frank Winklmeier - ATLAS Collaboration.
# **/

usage() {
    cat <<EOF
Syntax: post.sh TESTNAME [-s REGEX] [-i REGEX]
    TESTNAME       name of unit test
    -s             lines matching REGEX will be selected for the diff
    -i             lines matching REGEX will be ignored for the diff
    -h             help

Post-processing script that checks the return code of an executable (expected
in \$testStatus) and compares its output with a reference.

The select pattern is always extended to include common ERROR patterns.
If both select and ignore are specified, the lines are first selected and
then filtered by the ignore pattern. In all cases, a default ignore list is applied.
EOF
}

if [ "$#" -lt 1 -o "$1" == "-h" ]; then
    usage
    exit 1
fi
test=$1
shift

while getopts ":s:i:h" opt; do
    case $opt in
        s)
            selectpatterns=$OPTARG
            ;;
        i)
            ignorepatterns=$OPTARG
            ;;
        h)
            usage
            exit 0
            ;;
    esac
done

if [ "$POST_SH_NOCOLOR" = "" ]; then
 YELLOW="[93;1m"
 RED="[97;101;1m"
 RESET="[m"
else
 YELLOW=""
 RED=""
 RESET=""
fi

########################################## START ####################################################
## Definition of default patterns

# consider these name pairs identical in the diff
read -d '' II <<EOF
s/^StoreGateSvc_Impl VERBOSE/StoreGateSvc      VERBOSE/
s/^StoreGateSvc_Impl   DEBUG/StoreGateSvc        DEBUG/
s/StoreGateSvc_Impl/StoreGateSvc/
s/SGImplSvc/StoreGateSvc/
s/SG::DataProxyHolder::sgkey_t/sgkey_t/
s!(ERROR|INFO|WARNING|FATAL) [^ ]*/!\\\\1 ../!
s/(\.cxx|\.cpp|\.h|\.icc|LINE):[0-9]+/\\\\1/
s/.[[][?]1034h//
s/([0-9][0-9]* ms)/(xx ms)/
s/([0-9][0-9]* ms total)/(xx ms total)/
s/[[][0-9;]*m//g
s/INFO set[(][)]/INFO set([])/  #py2 vs py3
EOF

# Patterns that cannot be ignored
ERRORS="^ERROR | ERROR | FATAL "

# ignore diff annotations
PP='^---|^[[:digit:]]+[acd,][[:digit:]]+'

# ignore hex addresses
PP="$PP"'|0x\w{4,}'

# ignore package names e.g. Package-00-00-00
PP="$PP"'|\w+-[[:digit:]]{2}-[[:digit:]]{2}-[[:digit:]]{2}'
# ignore trunk package names e.g. Package-r123456
PP="$PP"'|\w+-r[[:digit:]]+'
# ignore cpu usage printouts
PP="$PP"'|ChronoStatSvc +INFO Time'
PP="$PP"'|Time left.+ Seconds'
PP="$PP"'|Timeleft.+ sec'
PP="$PP"'|INFO Time User'
# ignore clid db file name
PP="$PP"'|from CLIDDB file'
# ignore slug machine printout
PP="$PP"'| Machine: .* System and Processor Info'
PP="$PP"'| Jobname = .* Machine ='
# ignore slug pid printout
PP="$PP"'|Atlas Detector Simulation, Reconstruction and Analysis Running on'
PP="$PP"'|Program:  Slug-Dice-Arecon .+ pid +[[:digit:]]+'
#ignore DllClassManager DEBUG messages
PP="$PP"'|DllClassManager     DEBUG'
# ignore slug Library printout
PP="$PP"'|Library of +[[:digit:]]+ at +[[:digit:]]+'
PP="$PP"'|Library compiled on +[[:digit:]]'
# ignore ClassIDSvc "in memory db" printouts
PP="$PP"'|CLID: .* - type name:'
# ignore ClassIDSvc "already set" printouts
PP="$PP"'|ClassIDSvc .* setTypeNameForID: .* already set for'
# ignore ClassIDSvc finalize output
PP="$PP"'|ClassIDSvc .* finalize: wrote .*'
# PoolSvc
PP="$PP"'|^PoolSvc.*INFO'
# ignore any finalize output
PP="$PP"'|^.*INFO [Ff]inali[sz]'
# ignore rcs version comments
PP="$PP"'|Id: .+ Exp \$'
# ignore plugin count
PP="$PP"'|PluginMgr +INFO loaded plugin info for'
# ignore HistorySvc registered count
PP="$PP"'|HistorySvc +INFO Registered'
# ignore clid registry entries count
PP="$PP"'|ClassIDSvc[ [:digit:]]+INFO  getRegistryEntries: read'
# ignore existsDir path WARNINGS
PP="$PP"'|DirSearchPath::existsDir: WARNING not a directory'
# ignore warnings about duplicate services/converters.
PP="$PP"'|Service factory for type [^ ]+ already declared'
PP="$PP"'|Converter for class:[^ ]+ already exists'
# Number of configurables read can vary from build to build.
PP="$PP"'|INFO Read module info for|confDb modules in'
PP="$PP"'|INFO Read module info for|configurables from'
# ignore ApplicationMgr header.
PP="$PP"'|^ApplicationMgr *SUCCESS *$'
PP="$PP"'|^=+$'
PP="$PP"'|^ *Welcome to ApplicationMgr'
PP="$PP"'|^ *running on .* on '
PP="$PP"'|//GP: '
#ignore which malloc we are using
PP="$PP"'|^Preloading tcmalloc'
PP="$PP"'|^WARNING: TCMALLOCDIR not defined'
PP="$PP"'|^Py:AthFile .*shutting down athfile-server'
PP="$PP"'|^HistogramPersis...   INFO *.CnvServices.:'
PP="$PP"'|^HistogramPersis.*Histograms saving not required.'
PP="$PP"'|^StatusCodeSvc'
PP="$PP"'|^ApplicationMgr +INFO Successfully loaded'
PP="$PP"'|^IncidentSvc +DEBUG Service base class'
PP="$PP"'|^ClassIDSvc +WARNING Could not resolve clid DB path notthere.db'
PP="$PP"'|^IncidentSvc         DEBUG Adding .* listener '.*' with priority .*'
PP="$PP"'|MessageSvc not found, will use std::cerr'
PP="$PP"'|^AtRndmGenSvc         INFO Initializing AtRndmGenSvc'
PP="$PP"'|^AtRanluxGenSvc2      INFO Initializing AtRanluxGenSvc2'
PP="$PP"'|^AtRanluxGenSvc       INFO Initializing AtRanluxGenSvc'
#ignore personal .athenarc files
PP="$PP"'|including file \"\$HOME/.athenarc'
#ignore known gaudi python warning
PP="$PP"'|Bindings.py:660: DeprecationWarning'
#ignore the ignored
PP="$PP"'|Warning in <TEnvRec::ChangeValue>: duplicate entry <Root.ErrorIgnoreLevel=Print> for level 1; ignored'
PP="$PP"'|^JobOptionsSvc +INFO'
# Gaudi 31
PP="$PP"'|PluginService::SetDebug|setting LC_ALL'
# Ignore root6 duplicate dictionary warnings
PP="$PP"'|^Warning in .* (header|class) .* is already in'
# Ignore GaudiHive timeline printouts
PP="$PP"'|^TimelineSvc +INFO'
# StoreGate v3 migration
PP="$PP"'|VERBOSE ServiceLocatorHelper::service: found service IncidentSvc'
PP="$PP"'|VERBOSE ServiceLocatorHelper::service: found service ProxyProviderSvc'
# Pathnames / versions / times / hosts
PP="$PP"'|^IOVDb(Svc|Folder).*INFO (Folder|Connection|Total payload|.*bytes in)'
PP="$PP"'|^DBReplicaSvc.*INFO Read replica configuration'
PP="$PP"'|^EventInfoMgtInit: Got release version'
PP="$PP"'|^Py:Athena +INFO using release'
PP="$PP"'|servers found for host'
PP="$PP"'|^(Sun|Mon|Tue|Wed|Thu|Fri|Sat) '
PP="$PP"'|INFO Database being retired|^Domain.*INFO'
PP="$PP"'|SZ='
PP="$PP"'|using job opts'

# Hive ordering.
PP="$PP"'|Terminating thread-pool resources|Joining Scheduler thread|Disconnecting from sqlite|Opening COOL connection|Initializing CondInputLoader|preLoadAddresses: Removing|IOVRanges will be checked|User session with|ConnectionService I[nN][fF][oO]|Disconnect from the database|RalSessionMgr I[nN][fF][oO]|Connect to the database'

# Outputs dependent on whether or not a file catalog already exists.
PP="$PP"'|XMLFileCatalog|File is not in Catalog|Failed to open container to check POOL collection|Open     DbSession|Access   DbDomain|Access   DbDatabase|^RootDatabase.open|Deaccess DbDatabase'

PP="$PP"'|^Py:ConfigurableDb'
PP="$PP"'|^DBReplicaSvc.*INFO'
PP="$PP"'|INFO ... COOL  exception caught: The database does not exist|Create a new conditions database'
PP="$PP"'|SetGeometryVersion.py obtained'
PP="$PP"'|^ConditionStore +INFO Start ConditionStore'
PP="$PP"'|^ConditionStore +INFO Stop ConditionStore'
PP="$PP"'|INFO Found XML file:|INFO copying from'

# Differences between Gaudi versions.
PP="$PP"'|DEBUG input handles:|DEBUG output handles:|DEBUG Data Deps for|DEBUG Property update for OutputLevel :|-ExtraInputs |-ExtraOutputs |-Cardinality |-IsClonable |-NeededResources |-Timeline |Service base class initialized successfully'

# StoreGate INFO messages changed to VERBOSE
PP="$PP"'|^(StoreGateSvc|[^ ]+Store) +(INFO|VERBOSE) (Stop|stop|Start)'

# Transient frontier warnings.
PP="$PP"'|^warn  .fn-|^warn  .frontier.c'

# ubsan
PP="$PP"'|bits/regex.h:11'

# More StoreGate changes.
PP="$PP"'|DEBUG trying to create store'

PP="$PP"'|^IncidentProcAlg.* INFO|^Ath.*Seq +INFO|Loop Finished .seconds'
PP="$PP"'|INFO massageEventInfo:'
PP="$PP"'|Loop Finished'
PP="$PP"'|Terminating thread-pool resources|Joining Scheduler thread'
PP="$PP"'|DEBUG Calling destructor'
PP="$PP"'|INFO Found MetaDataTools'

PP="$PP"'|INFO TopAlg list empty.'

PP="$PP"'|^ChronoStatSvc +INFO +Number of skipped events for MemStat'
PP="$PP"'|^Py:Athena +INFO +including file'
PP="$PP"'|^Athena +INFO +including file'
PP="$PP"'|statistics are unreliable'

# Ignore annoying error from root 6.10.06
PP="$PP"'|no interpreter information for class TSelectorCint'

# Ignore warning stemming from apparent bug in ReadRootmapFile; gives messages
# like
#  Warning in <TInterpreter::ReadRootmapFile>: enum  xAOD::Type::ObjectType found in libxAODBaseDict.so  libEventKernelDict.so  libxAODBaseDict.so  libEventKernelDict.so  is already in libxAODBaseDict.so  libEventKernelDict.so 
PP="$PP"'|Warning in <TInterpreter::ReadRootmapFile>: enum'

# Ignore sourceID message from EventSelector.
PP="$PP"'|Disconnecting input sourceID'

# Printouts from new-style job configuration.
PP="$PP"'|Py:ComponentAccumulator +INFO '
PP="$PP"'|^[a-zA-Z0-9.]+ +: [^ ]'

# xAODMaker::EventInfoCnvAlg
PP="$PP"'|^xAODMaker::Even.*(WARNING|INFO)'

# gtest
PP="$PP"'|Ran [0-9]+ tests in'

# Verbosity cleanup in TileInfoLoader.
PP="$PP"'|^TileInfoLoader +INFO'

# Ignore new output stream helper tools.
PP="$PP"'|Found HelperTools'

# Ignore ID helper verbosity.
PP="$PP"'|filling address for'

# MetaInputLoader addresses and SIDs
PP="$PP"'|MetaInputLoader *INFO ( address|.*is still valid for|.*and sid)'

# Message useless for judging test success
PP="$PP"'|^FileMgr +DEBUG Successfully registered handler for tech'

########################################### END #####################################################

# Always use default ignore list
if [ -n "$ignorepatterns" ]; then
    ignorepatterns="$PP|$ignorepatterns"
else
    ignorepatterns=$PP
fi

# Make sure errors are not filtered
if [ -n "$selectpatterns" ]; then
    selectpatterns="$selectpatterns|$ERRORS"
fi

if [ -z "$testStatus" ]; then
   echo "$YELLOW post.sh> Warning: athena exit status is not available (\$testStatus is not set). $RESET"
else
   # check exit status
   joblog=${test}.log
   if [ "$testStatus" = 0 ]; then
       reflog=../share/${test}.ref

       # If we can't find the reference file, maybe it's located outside
       # the repo.  With the switch to git, we have to fall back
       # to handling the versioning manually.
       # ATLAS_REFERENCE_TAG should be a string of the form PACKAGE/VERSION.
       # We first look for it in DATAPATH.  If we don't find it,
       # we then look under ATLAS_REFERENCE_DATA.
       if [ \( ! -r $reflog \) -a "$ATLAS_REFERENCE_TAG" != "" ]; then
           # Look for the file in DATAPATH.
           # We have to look for the directory, not the file itself,
           # since get_files is hardcoded not to look more than two
           # levels down.
           get_files -data -symlink $ATLAS_REFERENCE_TAG > /dev/null
           reflog=`basename $ATLAS_REFERENCE_TAG`/${test}.ref
           if [ ! -r $reflog ]; then
               reflog=${ATLAS_REFERENCE_DATA}/${ATLAS_REFERENCE_TAG}/${test}.ref
           fi
       fi

       if [ -r $reflog ]; then
           jobdiff=${joblog}-todiff
           refdiff=`basename ${reflog}`-todiff

           # Process/filter log and reference file
           process() {
               sed -r "$II" $1 |
                   ( [[ "$selectpatterns" ]] && egrep -a "$selectpatterns" || tee ) |
                   ( [[ "$ignorepatterns" ]] && egrep -av "$ignorepatterns" || tee ) > $2
           }
           process $joblog $jobdiff
           process $reflog $refdiff

           diff -a -b -E -B -u $refdiff $jobdiff
           diffStatus=$?
           if [ $diffStatus != 0 ] ; then
               echo "$RED post.sh> ERROR: $reflog and $joblog differ $RESET"
               # Return with failure in this case:
               exit 1
           fi
       else
           # Don't warn for gtest tests.
           tail -1 $joblog | grep 'PASSED .* tests' > /dev/null
           refstat=$?
           if [ $refstat != 0 ]; then
             tail $joblog
             echo "$YELLOW post.sh> WARNING: reference output $reflog not available $RESET"
             echo  " post.sh> Please check ${PWD}/$joblog"
           fi
       fi
   else
       tail $joblog
       echo  "$RED post.sh> ERROR: Athena exited abnormally! Exit code: $testStatus $RESET"
       echo  " post.sh> Please check ${PWD}/$joblog"
   fi
fi
exit $testStatus
