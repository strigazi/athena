################################################################################
# Package: SiSpacePointFormation
################################################################################

# Declare the package name:
atlas_subdir( SiSpacePointFormation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaMonitoringKernel
                          Control/AthContainers
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecTools/SiSpacePointTool
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/VxVertex
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          Event/xAOD/xAODEventInfo
                          InnerDetector/InDetDetDescr/InDetIdentifier )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( SiSpacePointFormationLib
                   src/*.cxx
                   PUBLIC_HEADERS SiSpacePointFormation
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthContainers GeoPrimitives Identifier GaudiKernel InDetReadoutGeometry InDetPrepRawData TrkSpacePoint VxVertex SiSpacePointToolLib BeamSpotConditionsData AthenaMonitoringKernelLib
                   PRIVATE_LINK_LIBRARIES AtlasDetDescr xAODEventInfo InDetIdentifier )

atlas_add_component( SiSpacePointFormation
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthContainers GeoPrimitives Identifier GaudiKernel InDetReadoutGeometry InDetPrepRawData SiSpacePointToolLib TrkSpacePoint VxVertex AtlasDetDescr xAODEventInfo InDetIdentifier SiSpacePointFormationLib BeamSpotConditionsData )

atlas_install_python_modules( python/*.py )
