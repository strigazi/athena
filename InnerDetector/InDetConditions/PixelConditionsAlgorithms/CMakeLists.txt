################################################################################
# Package: PixelConditionsAlgorithms
################################################################################

# Declare the package name:
atlas_subdir( PixelConditionsAlgorithms )

# External dependencies:
find_package( CLHEP )
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT COMPONENTS Core Hist RIO )

# Component(s) in the package:
atlas_add_library( PixelConditionsAlgorithmsLib
   PixelConditionsAlgorithms/*.h
   INTERFACE
   NO_PUBLIC_HEADERS
   LINK_LIBRARIES GaudiKernel AthenaKernel AthenaBaseComps PixelConditionsData DetDescrConditions
   AthenaPoolUtilities PathResolver TrkGeometry)

atlas_add_component( PixelConditionsAlgorithms
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
   ${COOL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES}
   ${CLHEP_LIBRARIES} CommissionEvent AthenaBaseComps AthenaKernel AthenaPoolUtilities
   GaudiKernel PixelConditionsData SGTools StoreGateLib CoralDB
   GeoModelUtilities Identifier InDetIdentifier InDetReadoutGeometry PixelReadoutGeometry
   PixelCablingLib PixelConditionsAlgorithmsLib PathResolver )

# Install files from the package:
atlas_install_python_modules( python/*.py )

