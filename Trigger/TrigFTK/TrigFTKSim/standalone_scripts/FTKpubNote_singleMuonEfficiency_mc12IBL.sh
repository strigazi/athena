#!/bin/bash

#####################################################################
#
#
# Grid Job submission script for FTK simulation
#
#
#####################################################################

# SVN information that this script was used

# Path: .
# URL: svn+ssh://phchang@svn.cern.ch/reps/atlasoff/Trigger/TrigFTK/TrigFTKSim/branches/TrigFTKSim-00-09-09-branch
# Repository Root: svn+ssh://phchang@svn.cern.ch/reps/atlasoff
# Repository UUID: 4525493e-7705-40b1-a816-d608a930855b
# Revision: 569582
# Node Kind: directory
# Schedule: normal
# Last Changed Author: gvolpi
# Last Changed Rev: 569568
# Last Changed Date: 2013-11-10 04:08:03 -0600 (Sun, 10 Nov 2013)

# Basic environment setting.
source grid.sh # this script defines all the functions and the variable used but not the declared in this script
unset extra loops regions subregions inputs

# -------------------------------------------------------------------
#     runstring   - This current simulation run name.
#     INPUT       - Input data set that will run the simulation on.
#     inputs      - Sets how many files to run over in the INPUT
#     baseDS      - Base dataset contains constants and sectors which are 
#                   used in the trackfitting and many more.
#     bankDS      - Bank dataset contains the pattern banks, which will be 
#                   used to simulate "pattern matching" in the first stage.
#     site        - Grid site to submit to.
#     MAXPATTERNS - Sets the max number of patterns to use during the
#                   pattern matching stage.
#     regions     - Defines which regions Out of 64 regions the simulation 
#                   will run.
#     m           - For each regions, there are 'm' number of sub-regions.
#                   This variable defines the number of sub-regions.
#                   This value has to be compatiable with the data sets one
#                   is using.
#     extra       - This is the most complicated part where it defines 
#                   various options. If unsure, contact one of the FTK experts.
# -------------------------------------------------------------------

runstring=raw_8LcIbl123_15x16x36Ibl_singleMuons
INPUT="-n 100000 --nFilesPerJob 2 --inDS user.johnda.100kTestMuonsAfterFix_Aug_16_13 "'--match *.dat*'
INPUT="$INPUT --express "
inputs="0..120"
baseDS=user.annovi.tdaqtdr_base_8LcIbl123_12LIbl_20130807v2/ #TDAQ TDR
baseDS=user.phchang.baseDS_8LextFrom12L_TruthFilter_20131122/ #pub note
#bankDS=user.annovi.raw_8LcIbl123_15x16x36Ibl_mc12IBL_40Mx125loops_autotuneFix_4NSubs_AM30x32x72Ibl.2013_08_26_11_15_00/ #TDAQ TDR
bankDS=user.annovi.raw_8LcIbl123_15x16x36Ibl_50Mx100loops_AM30x32x72Ibl_effCache.ftk.2013_11_17_22_03_00/ #TDAQ TDR cache
bankDS=user.phchang.baseDS_8LextFrom12L_TruthFilter_20131122/ # for sectors as constants
site=ANALY_MWT2_SL6
MAXPATTERNS=4194304
regions="0" #"0..63"
m=1 #4

# global extra options
global_extra="${global_extra} -o N=64"
global_extra="${global_extra} -o RMAP_FILE=\${mapdir}/raw_12Libl.tmap"
global_extra="${global_extra} -o PMAP_FILE=\${mapdir}/raw_8LcIbl123.pmap"
global_extra="${global_extra} -o RFmode=roadfindertsp"

#1st stage
#global_extra="${global_extra} -o TFmode=trackfitter"
#global_extra="${global_extra} -o PMAP_FILE_COMPLETE=\${mapdir}/raw_8LcIbl123.pmap"
#global_extra="${global_extra} -o HWSDEV_FILE=\${hwdir}/raw_8LcIbl123.hw"
#extra="${extra} -o NCOORDS=11"

#2nd stage
global_extra="${global_extra} -o TFmode=trackfitter711"
global_extra="${global_extra} -o PMAP_FILE_COMPLETE=\${mapdir}/raw_12Libl.pmap"
global_extra="${global_extra} -o HWSDEV_FILE=\${hwdir}/raw_12L.hw"
global_extra="${global_extra} -o SS_FILE_UNUSED=\${mapdir}/../ss_file/raw_8LcIBL123_unusedmedium.ss"
global_extra="${global_extra} -o SS_FILE_COMPLETE=\${mapdir}/../ss_file/raw_8LcIBL123_unusedmedium.ss"
extra="${extra} -o NCOORDS=16"

# not used by Alberto for TDAQ TDR simulations:
extra="${extra} -o RESTRICT_SCT_PAIR_MODULE=1"                                            # RestrictSctPairModule 

# extra options
extra="${extra} -o amlabel=30x32x72Ibl"
extra="${extra} -o RAW_MODE=2"
extra="${extra} -o SAVE_INCOMPLETE=1"
extra="${extra} -o RESTRICT_SCT_PAIR_LAYER=1"                                             # RestrictSctPairLayer
extra="${extra} -o MAX_MISSING_SCT_PAIRS=1"                                               # MaxMissingSCTPairs
extra="${extra} -o USETSP_SIM=2"                                                          # TSPSimulationLevel
extra="${extra} -o NCONN=4"                                                               # SSFNConnections
extra="${extra} -o MULTI_CONN_MODE=1"                                                     # SSFMultiConnection
extra="${extra} -o TRACKFITTER_MODE=3"                                                    # TRACKFITTER_MODE
extra="${extra} -o USETSP_SETAMSIZE=2"                                                    # SetAMSize
extra="${extra} -o IBL_MODE=1"                                                            # IBLMode=1
extra="${extra} -o PIXEL_CLUSTERING_MODE=1"                                               # PixelClusteringMode=1
extra="${extra} -o SCT_CLUSTERING=0"                                                      # SctClustering=0
extra="${extra} -o ALLOW_EXTRA_MISS_IN_SUPEREXP=1"                                        # SSFAllowExtraMiss=1
#extra="${extra} -o USETSP_MINCOVERAGE=0"                                                  # TSPMinCoverage 
#extra="${extra} -o USETSP_DBLEVEL=1"                                                      # DBBankLevel
#extra="${extra} -o USETSP_BANK=1"                                                         # UseTSPBank
#extra="${extra} -o MAX_TRANSITION_ETA=1.4"                                                # SSFTRMaxEta
#extra="${extra} -o MIN_TRANSITION_ETA=1.0"                                                # SSFTRMinEta
#extra="${extra} -o CHECK_TRANSITION_BY=1"                                                 # SSFTRDefn
#extra="${extra} -o CACHEDBANK=1,_PATTDB=pcache.root"
extra="${extra} -o SECTORSASPATTERNS=1,UNSPLIT_SECTORS=1"

# Run simulation
runTSP

# Merge
unset regions
ftkDS=user.phchang.raw_8LcIbl123_15x16x36Ibl_singleMuons.ftk.2013_11_12_16_42_41/
ftkDS=user.phchang.raw_8LcIbl123_15x16x36Ibl_singleMuons.ftk.2013_11_13_10_07_04/

ftkDS=user.annovi.raw_8LcIbl123_15x16x36Ibl_singleMuons.ftk.2013_11_19_14_53_52/
#mergeTSP

#ftkDS=
#exploreFTK
