################################################################################
# Package: CaloClusterCorrection
################################################################################

# Declare the package name:
atlas_subdir( CaloClusterCorrection )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloConditions
			  TileCalorimeter/TileConditions
			  LArCalorimeter/LArRecConditions
                          Calorimeter/CaloEvent
                          Calorimeter/CaloInterface
                          Calorimeter/CaloRec
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/StoreGate
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaBaseComps
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloUtils
                          Calorimeter/CaloCondBlobObjs
                          Calorimeter/CaloIdentifier
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArCabling
                          Control/AthContainers
                          DetectorDescription/Identifier
                          Event/EventKernel
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( CaloClusterCorrectionLib
                   src/*.cxx
                   PUBLIC_HEADERS CaloClusterCorrection
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES CaloCondBlobObjs CaloConditions CaloEvent CaloIdentifier AthenaBaseComps AthenaKernel CxxUtils AthenaPoolUtilities xAODCaloEvent GaudiKernel LArIdentifier LArRecConditions TileConditionsLib CaloRecLib StoreGateLib SGtests LArToolsLib CaloDetDescrLib CaloUtilsLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} TestTools CaloGeoHelpers Identifier EventKernel PathResolver )

atlas_add_component( CaloClusterCorrection
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES CaloClusterCorrectionLib )

atlas_add_test( interpolate_test
                SOURCES
                test/interpolate_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloCondBlobObjs CaloConditions CaloEvent CaloIdentifier CaloRecLib AthenaBaseComps AthenaKernel CxxUtils StoreGateLib SGtests AthenaPoolUtilities xAODCaloEvent GaudiKernel LArIdentifier LArToolsLib TestTools CaloDetDescrLib CaloGeoHelpers CaloUtilsLib Identifier EventKernel PathResolver CaloClusterCorrectionLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

