################################################################################
# Package: TRT_StandaloneTrackFinder
################################################################################

# Declare the package name:
atlas_subdir( TRT_StandaloneTrackFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkEventUtils
                          PRIVATE
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters )

# Component(s) in the package:
atlas_add_component( TRT_StandaloneTrackFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkSegment TrkTrack InDetRecToolInterfaces TrkEventPrimitives TrkParameters TrkEventUtils )

# Install files from the package:
atlas_install_headers( TRT_StandaloneTrackFinder )

