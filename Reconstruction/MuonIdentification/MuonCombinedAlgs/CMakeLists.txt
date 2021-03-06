################################################################################
# Package: MuonCombinedAlgs
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaMonitoringKernel
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
            			  Event/xAOD/xAODTruth
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
			              MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
			              MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonLayerEvent
                          Reconstruction/MuonIdentification/MuonCombinedEvent
                          Reconstruction/MuonIdentification/MuonCombinedToolInterfaces
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_component( MuonCombinedAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaMonitoringKernelLib xAODCaloEvent xAODMuon xAODTruth xAODTracking GaudiKernel MuonSegment MuonRecToolInterfaces MuonLayerEvent MuonCombinedEvent MuonCombinedToolInterfaces TrkSegment TrkTrack TrkToolInterfaces)

atlas_install_python_modules( python/MuonCombinedAlgsMonitoring.py)
