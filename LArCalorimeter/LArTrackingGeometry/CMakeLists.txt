################################################################################
# Package: LArTrackingGeometry
################################################################################

# Declare the package name:
atlas_subdir( LArTrackingGeometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloTrackingGeometry
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelUtilities
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          Tracking/TrkDetDescr/TrkDetDescrGeoModelCnv
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkGeometrySurfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkDetDescr/TrkVolumes )

# External dependencies:
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( LArTrackingGeometry
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaBaseComps GaudiKernel TrkDetDescrInterfaces CaloDetDescrLib StoreGateLib SGtests GeoModelUtilities LArReadoutGeometry TrkDetDescrGeoModelCnv TrkDetDescrUtils TrkGeometry TrkGeometrySurfaces TrkSurfaces TrkVolumes CaloTrackingGeometryLib )

# Install files from the package:
atlas_install_headers( LArTrackingGeometry )
atlas_install_python_modules( python/*.py )

