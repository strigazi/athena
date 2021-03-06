################################################################################
# Package: MuonEDM_AssociationObjects
################################################################################

# Declare the package name:
atlas_subdir( MuonEDM_AssociationObjects )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthLinks
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment )

# Component(s) in the package:
atlas_add_library( MuonEDM_AssociationObjects
                   src/*.cxx
                   PUBLIC_HEADERS MuonEDM_AssociationObjects
                   LINK_LIBRARIES AthLinks AthenaKernel MuonPattern MuonSegment )

atlas_add_dictionary( MuonEDM_AssociationObjectsDict
                      MuonEDM_AssociationObjects/MuonSegmentCombPatternCombAssociationMap.h
                      MuonEDM_AssociationObjects/selection.xml
                      LINK_LIBRARIES AthLinks MuonPattern MuonSegment)
