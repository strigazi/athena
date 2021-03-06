################################################################################
# Package: SCT_ModuleDistortions
################################################################################

# Declare the package name:
atlas_subdir( SCT_ModuleDistortions )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_library( SCT_ModuleDistortionsLib
                   SCT_ModuleDistortions/*.h
                   INTERFACE
                   PUBLIC_HEADERS SCT_ModuleDistortions
                   LINK_LIBRARIES GaudiKernel GeoPrimitives TrkParameters Identifier AthenaKernel )


# Component(s) in the package:
atlas_add_component( SCT_ModuleDistortions
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} SCT_ModuleDistortionsLib InDetReadoutGeometry TrkEventPrimitives AthenaBaseComps StoreGateLib SGtests InDetIdentifier PathResolver )

atlas_add_test( TestSCT_DistortionsTool
                SCRIPT athena.py --threads=5 SCT_ModuleDistortions/TestSCT_DistortionsTool.py
                PROPERTIES TIMEOUT 600
                ENVIRONMENT THREADS=5 )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.txt )
