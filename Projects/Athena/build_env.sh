# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# This script sets up the build enironment for an Athena
# build, on top of a built set of externals (including Gaudi)
#
# This script is kept separate from the build.sh
# wrapper so it can be sourced separately from it when
# clients want to manage their own build and just want
# to setup the build environment

env_usage() {
    echo "Usage: build_env.sh [-b build dir]"
}

# This function actually sets up the environment for us
# (factorise it here in case it needs skipped)
env_setup() {
    startdir=$(pwd)
    # As this script can be sourced we need to support zsh and
    # possibly other Bourne shells
    if [ "x${BASH_SOURCE[0]}" = "x" ]; then
        # This trick should do the right thing under ZSH:
        thisdir=$(dirname `print -P %x`)
        if [ $? != 0 ]; then
            echo "ERROR: This script must be sourced from BASH or ZSH"
            return 1
        fi
    else
        # The BASH solution is a bit more straight forward:
        thisdir=$(dirname ${BASH_SOURCE[0]})
    fi
    AthenaSrcDir=$(cd ${thisdir};pwd)

    # The directory holding the helper scripts:
    scriptsdir=${AthenaSrcDir}/../../Build/AtlasBuildScripts

    # Go to the main directory of the repository:
    cd ${AthenaSrcDir}/../..

    # Check if the user specified any source/build directories:
    if [ "$BUILDDIR" = "" ]; then
	    BUILDDIR=${AthenaSrcDir}/../../../build
    fi

    # Get the version of Athena for the build.
    version=`cat ${AthenaSrcDir}/version.txt`

    # Set up the environment for the build:
    export NICOS_PROJECT_HOME=$(cd ${BUILDDIR}/install;pwd)/Athena

    # Set up the environment variables for finding LCG and the TDAQ externals:
    source ${scriptsdir}/LCG_RELEASE_BASE.sh
    source ${scriptsdir}/TDAQ_RELEASE_BASE.sh

    # Set up the AthenaExternals project:
    extDir=${BUILDDIR}/install/AthenaExternals/${version}/InstallArea
    if [ ! -d ${extDir} ]; then
	    echo "Didn't find the AthenaExternals project under ${extDir}"
        echo "(Hopefully this is intentional and you have done e.g. asetup AthenaExternals,master,latest)"
    else
        echo "Setting up AthenaExternals from: ${extDir}"
        source ${extDir}/*/setup.sh
    fi

    # Point to Gaudi:
    # Get platform from the GAUDI build - we're assuming here that there's only one platform installed.
    if [ -z "${GAUDI_ROOT+1}" ]; then
        platform=$(cd ${BUILDDIR}/install/GAUDI/${version}/InstallArea/;ls)
        export GAUDI_ROOT=${BUILDDIR}/install/GAUDI/${version}/InstallArea/${platform}
    fi
    echo "Taking Gaudi from: ${GAUDI_ROOT}"

    cd $startdir
}

# we need to reset the option index as we are sourcing this script
# http://stackoverflow.com/questions/23581368/bug-in-parsing-args-with-getopts-in-bash
OPTIND=1

# Parse the command line arguments:
BUILDDIR=""
while getopts "b:h" opt; do
    case $opt in
        b)
            BUILDDIR=$OPTARG
            ;;
	h)
	    env_usage
	    ABORT=1
	    ;;
        :)
            echo "Argument -$OPTARG requires a parameter!"
            env_usage
	    ABORT=1
            ;;
        ?)
            echo "Unknown argument: -$OPTARG"
            env_usage
	    ABORT=1
            ;;
    esac
done

# Put a big wrapper around bad argument case, because
# a sourced script should not call "exit". This is quite
# annoying...
if [ -z "$ABORT" ]; then
    env_setup
fi
