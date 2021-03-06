# $Id: CMakeLists.txt 740701 2016-04-15 18:22:55Z ssnyder $
################################################################################
# Package: CaloTools
################################################################################

# Declare the package name:
atlas_subdir( CaloTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Calorimeter/CaloConditions
   Calorimeter/CaloDetDescr
   Calorimeter/CaloEvent
   Calorimeter/CaloGeoHelpers
   Calorimeter/CaloIdentifier
   Calorimeter/CaloInterface
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/AthAllocators
   Control/StoreGate
   Database/AthenaPOOL/AthenaPoolUtilities
   DetectorDescription/AtlasDetDescr
   DetectorDescription/Identifier
   GaudiKernel
   LArCalorimeter/LArElecCalib
   LArCalorimeter/LArIdentifier
   LArCalorimeter/LArRecEvent
   LArCalorimeter/LArCabling
   LArCalorimeter/LArRawConditions
   LumiBlock/LumiBlockComps
   TileCalorimeter/TileConditions
   TileCalorimeter/TileEvent
   Control/CxxUtils
   PRIVATE
   Calorimeter/CaloCondBlobObjs
   Control/AthAllocators
   DetectorDescription/IdDictParser
   Event/xAOD/xAODBase
   Event/xAOD/xAODEventInfo
   TileCalorimeter/TileIdentifier )

# External dependencies:
find_package( Boost )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT COMPONENTS Core MathCore )

# Component(s) in the package:
atlas_add_component( CaloTools
   CaloTools/*.h CaloTools/*.icc src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
   ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CORAL_LIBRARIES}
   ${CLHEP_LIBRARIES} CaloConditions CaloDetDescrLib CaloEvent CaloGeoHelpers
   CaloIdentifier AthenaBaseComps AthenaKernel StoreGateLib
   AthenaPoolUtilities AtlasDetDescr Identifier GaudiKernel LArIdentifier
   LArRecEvent LArToolsLib LumiBlockCompsLib TileConditionsLib TileEvent
   CxxUtils CaloCondBlobObjs AthAllocators IdDictParser xAODBase xAODEventInfo
   TileIdentifier LArRawConditions CaloInterfaceLib )

# Install files from the package:
atlas_install_headers( CaloTools )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )


atlas_add_test( CaloEstimatedGainToolConfig_test
                SCRIPT python -m CaloTools.CaloEstimatedGainToolConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---|^IOVDbSvc" )

atlas_add_test( CaloEstimatedGainTool_test
                SCRIPT python -m CaloTools.CaloEstimatedGainTool_test
                PROPERTIES TIMEOUT 300
                LOG_SELECT_PATTERN "ERROR|error|WARNING [^U]|FATAL|processing|TestAlg" )

