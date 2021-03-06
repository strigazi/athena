################################################################################
# Package: MuonSegmentMakerToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentMakerToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonLayerEvent
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces 
			  MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonEDM_AssociationObjects)

# External dependencies:
find_package( Eigen )

atlas_add_library( MuonSegmentMakerToolInterfaces
                   MuonSegmentMakerToolInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonSegmentMakerToolInterfaces
                   LINK_LIBRARIES  GaudiKernel MuonPrepRawData TrkTruthData MuonRIO_OnTrack TrkSegment MuonPattern MuonSegment MuonLayerEvent MuonRecToolInterfaces Identifier MuonEDM_AssociationObjects GeoPrimitives EventPrimitives )

