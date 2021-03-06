###############################################################################
# Package: SCT_ConditionsTools
################################################################################

# Declare the package name:
atlas_subdir( SCT_ConditionsTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/SCT_ConditionsData
			  InnerDetector/InDetConditions/InDetByteStreamErrors
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Event/xAOD/xAODEventInfo
                          Event/EventContainers
                          Database/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          InnerDetector/InDetConditions/InDetByteStreamErrors
                          InnerDetector/InDetConditions/SiPropertiesTool
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/SCT_Cabling
                          AtlasTest/TestTools )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( GTest )
find_package( GMock )

# Component(s) in the package:
atlas_add_component ( SCT_ConditionsTools
                      src/components/*.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel Identifier GeoModelUtilities GaudiKernel AthenaBaseComps StoreGateLib SGtests xAODEventInfo SCT_ConditionsData InDetByteStreamErrors InDetIdentifier InDetReadoutGeometry SCT_CablingLib SiPropertiesToolLib SCT_ConditionsToolsLib )


atlas_add_library( SCT_ConditionsToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS SCT_ConditionsTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel Identifier GeoModelUtilities GaudiKernel AthenaBaseComps StoreGateLib SGtests xAODEventInfo SCT_ConditionsData SiPropertiesToolLib InDetByteStreamErrors InDetIdentifier InDetReadoutGeometry SCT_CablingLib EventContainers InDetConditionsSummaryService RDBAccessSvcLib )

# Add unit tests
atlas_add_test( SCT_RODVetoTool_test
                SOURCES test/SCT_RODVetoTool_test.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}  ${GMOCK_INCLUDE_DIRS}
                LINK_LIBRARIES TestTools SCT_ConditionsToolsLib ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES}
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( SCT_ConditionsConfig_test
                SCRIPT test/SCT_ConditionsConfig_test.py
                PROPERTIES TIMEOUT 300 )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/*.py )

