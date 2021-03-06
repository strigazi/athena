# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaPoolTest )

# Component(s) in the package:
atlas_add_component( AthenaPoolTest
                     src/AthenaPoolTestDataReader.cxx
                     src/AthenaPoolTestDataWriter.cxx
                     src/AthenaPoolTestAttrWriter.cxx
                     src/AthenaPoolTestAttrReader.cxx
                     src/InDetRawDataFakeReader.cxx
                     src/InDetRawDataFakeWriter.cxx
                     src/LArCellContFakeWriter.cxx
                     src/LArCellContFakeReader.cxx
                     src/EventInfoWriter.cxx
                     src/EventInfoReader.cxx
                     src/PileUpEventInfoWriter.cxx
                     src/PileUpEventInfoReader.cxx
                     src/TrkTrackFakeReader.cxx
                     src/TrkTrackFakeWriter.cxx
                     src/FakeTrackBuilder.cxx
                     src/RDOReaderDoubleSelector.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaPoolTestData CaloDetDescrLib CaloEvent CaloIdentifier AthenaBaseComps AthenaKernel StoreGateLib SGtests AthenaPoolUtilities GeoPrimitives Identifier EventInfo GaudiKernel InDetIdentifier InDetReadoutGeometry InDetRawData InDetCompetingRIOsOnTrack InDetPrepRawData InDetRIO_OnTrack LArRecEvent TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkMaterialOnTrack TrkParameters TrkTrack TrkVertexOnTrack EventInfoMgtLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )


function (athenapooltest_run_test testName jo pattern)
  cmake_parse_arguments( ARG "" "DEPENDS" "" ${ARGN} )

  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/test/athenapooltest_test.sh.in
                  ${CMAKE_CURRENT_BINARY_DIR}/athenapooltest_${testName}.sh
                  @ONLY )
  atlas_add_test( ${testName}
                  SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/athenapooltest_${testName}.sh
                  ENVIRONMENT "ATLAS_REFERENCE_TAG=AthenaPoolTest/AthenaPoolTest-01-00-09"
                  POST_EXEC_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test/post_check_with_select.sh ${testName} ${pattern}"
                  PROPERTIES TIMEOUT 900
                   )
  if( ARG_DEPENDS )
    set_tests_properties( AthenaPoolTest_${testName}_ctest
                          PROPERTIES DEPENDS AthenaPoolTest_${ARG_DEPENDS}_ctest )
  endif()
endfunction (athenapooltest_run_test)


# Write/read out some simple objects:
athenapooltest_run_test( AthenaPoolTestWrite AthenaPoolTestWrite
                         "^AthenaPoolTestD" )
athenapooltest_run_test( AthenaPoolTestRead AthenaPoolTestRead
                         "AthenaPoolTestD"
                         DEPENDS AthenaPoolTestWrite )

# Write with DoubleEventSelector
athenapooltest_run_test( AthenaPoolTestWriteDoubleSelector AthenaPoolTestWriteDoubleSelector
                         "'^..DoubleEvent|^..SecondaryEvent|^..AthenaPoolTest'"
                         DEPENDS AthenaPoolTestRead)
athenapooltest_run_test( AthenaPoolTestReadDoubleSelector AthenaPoolTestReadDoubleSelector
                         "'^..DoubleEvent|^..SecondaryEvent|^..AthenaPoolTest'"
                         DEPENDS AthenaPoolTestWriteDoubleSelector )

# Add on test of navigation still using simple objects:
# AthenaPoolTestWrite - Step2 - rewrite part in another file
# DoubleEventSelector
athenapooltest_run_test( AthenaPoolTestStep2WriteDoubleSelector AthenaPoolTestStep2WriteDoubleSelector
                         "'^..DoubleEvent|^..SecondaryEvent|^..AthenaPoolTest'"
                         DEPENDS AthenaPoolTestReadDoubleSelector )
# normal EventSelector
athenapooltest_run_test( AthenaPoolTestStep2Write AthenaPoolTestStep2Write
                         "^..AthenaPoolTestD"
                         DEPENDS AthenaPoolTestStep2WriteDoubleSelector )

# Copy the Step2 file
#athenapooltest_run_test( AthenaPoolTestCopy AthenaPoolTestCopy
#                         "^..Stream1" )
# 
# Remove the Step2 file and read the copy
#athenapooltest_run_test( AthenaPoolTestReadAfterCopy AthenaPoolTestReadAfterCopy
#                         "AthenaPoolTestD" )
# 
# Write/read out PileUpEventInfo:
#athenapooltest_run_test( PileUpEventInfoWrite PileUpEventInfoWrite
#                         "^..PileUpEventInfo" )
#athenapooltest_run_test( PileUpEventInfoRead PileUpEventInfoRead
#                         "^..PileUpEventInfo" )
# 
# 
# 
# Atlfast NOT YET WORKING
#athenapooltest_run_test( AtlfastPoolWrite AtlfastPoolWriteOptions
#                         "AtlfastPoolWrite" )
#athenapooltest_run_test( AtlfastPoolRead AtlfastPoolReadOptions
#                         "AtlfastPoolRead" )

# Test write/read of indet rdos:
athenapooltest_run_test( InDetRawDataWriter InDetRawDataWriter_jobOptions
                         "InDetRawDataFak"
                         DEPENDS AthenaPoolTestStep2Write )
athenapooltest_run_test( InDetRawDataRead InDetRawDataReader_jobOptions
                         "InDetRawDataFak"
                         DEPENDS InDetRawDataWriter )

# Test write/read of LAr cell
athenapooltest_run_test( LArCellContWriter LArCellContWriter_jobOptions
                         "LArCellContFake"
                         DEPENDS InDetRawDataRead )
athenapooltest_run_test( LArCellContRead LArCellContReader_jobOptions
                         "LArCellContFake"
                         DEPENDS LArCellContWriter )

# Read two RDOs with the DoubleEventSelector
athenapooltest_run_test( ReadRDODoubleSelector ReadRDODoubleSelector
                         "'^..DoubleEvent|^..SecondaryEvent|^..RDOReader'"
                         DEPENDS LArCellContRead )
athenapooltest_run_test( ReadCombinedRDO ReadCombinedRDO
                         "'^..DoubleEvent|^..SecondaryEvent|^..LArCellContFake|^..InDetRawDataFak'"
                         DEPENDS ReadRDODoubleSelector )

# Test basic overlay algorithms with DoubleEventSelector
athenapooltest_run_test( DoubleEventSelectorOverlayTest DoubleEventSelectorOverlayTest
                         "'^..DoubleEvent|^..SecondaryEvent|^..AthenaPoolAddr|^..Stream1|Found HelperTools'"
                         DEPENDS ReadCombinedRDO )

# AthenaMT test for DoubleEventSelector
atlas_add_test( DoubleEventSelectorOverlayTestMT
                SCRIPT athena.py AthenaPoolTest/DoubleEventSelectorOverlayTest.py --threads=4
                PROPERTIES TIMEOUT 900
                POST_EXEC_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test/post_check_with_select.sh \
                                  DoubleEventSelectorOverlayTestMT '^..DoubleEvent.*WARNING|ERROR'")

# Test of explicit collection write/read
#athenapooltest_run_test( LArCellContWriteReg LArCellContWriter_jobOptionsReg
#                         "LArCellContFake" )
#athenapooltest_run_test( LArCellContReadReg LArCellContReader_jobOptionsReg
#                         "LArCellContFake" )
# 
# LFN test
#athenapooltest_run_test( LArCellContWriteStep1 LArCellContWriter_jobOptionsStep1
#                         "LArCellContFake" )
#athenapooltest_run_test( LArCellContWriteStep2 LArCellContWriter_jobOptionsStep2
#                         "LArCellContFake" )
#athenapooltest_run_test( LArCellContReadByLFN LArCellContReader_jobOptionsByLFN
#                         "LArCellContFake" )
# 
# 
# write/read test of calo clusters
#athenapooltest_run_test( CaloClusterWriter CaloClusterWriter_jobOptions
#                         "^..CaloClusterFakeWriter"
#                         DEPENDS LArCellContRead  )
#athenapooltest_run_test( CaloClusterReader CaloClusterReader_jobOptions
#                         "^..CaloClusterFakeWriter"
#                         DEPENDS CaloClusterWriter  )
# 
# Test ElementLinks across files
#athenapooltest_run_test( CaloClusterStep1Writer CaloClusterStep1Writer_jobOptions
#                         "Fake" )
#athenapooltest_run_test( CaloClusterStep2Writer CaloClusterStep2Writer_jobOptions
#                         "Fake" )
#athenapooltest_run_test( CaloCluster2Step2Reader CaloCluster2StepReader_jobOptions
#                         "Fake" )
# 
# 
# The following four tests are for writing explicit root collections with an extra ref 
#   First write an implicit collection, then a second file and an explicit root collection
#   Then read back with standard ref - Step1, or extra ref - Step2
# 
# AthenaPoolTestWrite - write out implicit collection with athena pool test data
#athenapooltest_run_test( AthenaPoolTestWrite2 AthenaPoolTestWrite
#                         "^..AthenaPoolTestD" )
# LArCellContWriteRegStep2 - write out explicit collection with LArCellCont
#athenapooltest_run_test( LArCellContWriteRegStep2 LArCellContWriter_jobOptionsRegStep2
#                         "LArCellContFake" )
# LArCellContWriteRegStep3 - write out explicit collection with LArCellCont 
# in separate step
#athenapooltest_run_test( LArCellContWriteRegStep3 LArCellContWriter_jobOptionsRegStep3
#                         "LArCellContFake" )
# LArCellContReadRegStep1 - read in LArCellCont
#athenapooltest_run_test( LArCellContReadRegStep1 LArCellContReader_jobOptionsRegStep1
#                         "LArCellContFake" )
# LArCellContReadRegStep2 - read in athena pool test data
#athenapooltest_run_test( LArCellContReadRegStep2 LArCellContReader_jobOptionsRegStep2
#                         "LArCellContFake" )

# Check for adding DetDescr tags to event info object:
athenapooltest_run_test( EventInfoWrite EventInfoWrite
                         "'^..[TE][av][ge][In][nt][fI][on][Mf][go][rW]'"
                         DEPENDS DoubleEventSelectorOverlayTest )
athenapooltest_run_test( EventInfoWriteStep2 EventInfoWriteStep2
                         "'^..[TE][av][ge][In][nt][fI][on][Mf][go][rW]'"
                         DEPENDS EventInfoWrite )
athenapooltest_run_test( EventInfoWriteStep3 EventInfoWriteStep3
                         "'^..[TE][av][ge][In][nt][fI][on][Mf][go][rW]'"
                         DEPENDS EventInfoWriteStep2 )
athenapooltest_run_test( EventInfoWriteStep4 EventInfoWriteStep4
                         "'^..[TE][av][ge][In][nt][fI][on][Mf][go][rW]'"
                         DEPENDS EventInfoWriteStep3 )
athenapooltest_run_test( EventInfoWriteStep5 EventInfoWriteStep5
                         "'^..[TE][av][ge][In][nt][fI][on][Mf][go][rW]'"
                         DEPENDS EventInfoWriteStep4 )
athenapooltest_run_test( EventInfoRead EventInfoRead
                         "'^..[TE][av][ge][In][nt][fI][on][Mf][go][rW]'"
                         DEPENDS EventInfoWriteStep5 )
