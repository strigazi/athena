################################################################################
# Package: CaloTriggerTool
################################################################################

# Declare the package name:
atlas_subdir( CaloTriggerTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/Identifier
                          GaudiKernel
                          Trigger/TrigT1/TrigT1CaloCalibConditions
                          PRIVATE
                          Control/AthenaBaseComps
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArCabling )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( CaloTriggerToolLib
                   src/*.cxx
                   PUBLIC_HEADERS CaloTriggerTool
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} CaloIdentifier AthenaKernel Identifier GaudiKernel TrigT1CaloCalibConditions StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps LArIdentifier LArCablingLib )

atlas_add_component( CaloTriggerTool
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES CaloTriggerToolLib AthenaBaseComps )

atlas_add_dictionary( CaloTriggerToolDict
                      CaloTriggerTool/CaloTriggerToolDict.h
                      CaloTriggerTool/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} CaloIdentifier AthenaKernel StoreGateLib SGtests Identifier GaudiKernel TrigT1CaloCalibConditions AthenaBaseComps LArIdentifier CaloTriggerToolLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

