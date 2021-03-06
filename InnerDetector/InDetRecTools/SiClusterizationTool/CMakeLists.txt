################################################################################
# Package: SiClusterizationTool
################################################################################

# Declare the package name:
atlas_subdir( SiClusterizationTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   Control/AthenaKernel
   DetectorDescription/GeoPrimitives
   DetectorDescription/Identifier
   Event/EventPrimitives
   GaudiKernel
   InnerDetector/InDetRawEvent/InDetSimData
   InnerDetector/InDetConditions/BeamSpotConditionsData
   InnerDetector/InDetConditions/InDetConditionsSummaryService
   InnerDetector/InDetConditions/InDetCondTools
   InnerDetector/InDetConditions/PixelConditionsData
   InnerDetector/InDetDetDescr/PixelCabling
   InnerDetector/InDetDetDescr/InDetIdentifier
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   InnerDetector/InDetDetDescr/PixelReadoutGeometry
   InnerDetector/InDetDetDescr/SCT_ReadoutGeometry
   InnerDetector/InDetRawEvent/InDetRawData
   InnerDetector/InDetRecEvent/InDetPrepRawData
   InnerDetector/InDetRecTools/InDetRecToolInterfaces
   Tracking/TrkEvent/TrkParameters
   Tracking/TrkUtilityPackages/TrkNeuralNetworkUtils
   PRIVATE
   Database/AthenaPOOL/AthenaPoolUtilities
   Database/AthenaPOOL/PoolSvc
   Database/APR/FileCatalog
   DetectorDescription/AtlasDetDescr
   DetectorDescription/DetDescrCond/DetDescrCondTools
   InnerDetector/InDetDetDescr/PixelGeoModel
   Tracking/TrkDetDescr/TrkSurfaces
   Tracking/TrkEvent/TrkEventPrimitives
   Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core MathCore Hist )

# Component(s) in the package:
atlas_add_library( SiClusterizationToolLib
   SiClusterizationTool/*.h src/*.cxx
   PUBLIC_HEADERS SiClusterizationTool
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives
   Identifier EventPrimitives GaudiKernel InDetSimData InDetIdentifier
   InDetReadoutGeometry PixelReadoutGeometry SCT_ReadoutGeometry InDetRawData InDetPrepRawData InDetRecToolInterfaces InDetConditionsSummaryService
   TrkParameters TrkNeuralNetworkUtilsLib PixelConditionsData
   PixelGeoModelLib PixelCablingLib BeamSpotConditionsData
   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaPoolUtilities FileCatalog AtlasDetDescr
   TrkSurfaces TrkEventPrimitives VxVertex PixelGeoModelLib PoolSvcLib DetDescrCondToolsLib )

atlas_add_component( SiClusterizationTool
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel PixelConditionsData SiClusterizationToolLib PoolSvcLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
