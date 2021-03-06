################################################################################
# Package: TRT_ConditionsAlgs
################################################################################

# Declare the package name:
atlas_subdir( TRT_ConditionsAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetConditions/TRT_ConditionsData
                          PRIVATE
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
		          Database/AthenaPOOL/AthenaPoolUtilities
			  DetectorDescription/DetDescrCond/DetDescrConditions 
			  DetectorDescription/GeoModel/GeoModelUtilities )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( TRT_ConditionsAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TRT_ConditionsServicesLib TRT_ConditionsData InDetIdentifier
                                               InDetReadoutGeometry TRT_ReadoutGeometry AthenaPoolUtilities DetDescrConditions GeoModelUtilities )


# Install files from the package:
atlas_install_headers( TRT_ConditionsAlgs )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.txt share/*.py )

