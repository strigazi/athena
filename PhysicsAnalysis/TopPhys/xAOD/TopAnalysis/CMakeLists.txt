
# Declare the name of this package:
atlas_subdir( TopAnalysis )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          xAODCore
                          xAODRootAccess
                          xAODEgamma
                          xAODMuon
                          xAODJet
                          xAODMissingET
                          xAODCutFlow
                          AsgTools
                          AthContainers
                          PATInterfaces
                          TopCPTools
                          TopEventSelectionTools
                          TopConfiguration
                          TopCorrections
                          TopEvent
                          TopParticleLevel
                          TopPartons
                          TopObjectSelectionTools
                          TopSystematicObjectMaker
                          TopFakes
			  TopDataPreparation
			  TopHLUpgrade )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TopAnalysis _cintDictSource
   ROOT_HEADERS TopAnalysis/ObjectLoaderStandardCuts.h
   TopAnalysis/EventSaverFlatNtuple.h TopAnalysis/EventSaverxAOD.h
   TopAnalysis/EventSaverxAODNext.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopAnalysis Root/*.cxx Root/*.h Root/*.icc
                   TopAnalysis/*.h TopAnalysis/*.icc TopAnalysis/*/*.h
                   TopAnalysis/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TopAnalysis
                   LINK_LIBRARIES xAODCore
                                  xAODRootAccess
                                  xAODEgamma
                                  xAODMuon
                                  xAODJet
                                  xAODMissingET
                                  xAODCutFlow
                                  AsgTools
                                  AthContainers
                                  PATInterfaces
                                  TopCPTools
                                  TopEventSelectionTools
                                  TopConfiguration
                                  TopCorrections
                                  TopEvent
                                  TopParticleLevel
                                  TopPartons
                                  TopObjectSelectionTools
                                  TopSystematicObjectMaker
                                  TopFakes
				  TopDataPreparation
				  TopHLUpgrade
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install data files from the package:
atlas_install_data( share/* )

# Install user scripts
atlas_install_scripts( scripts/* )

# Build the executables of the package:
atlas_add_executable( top-xaod
                      util/top-xaod.cxx
                      LINK_LIBRARIES xAODCore
                                     xAODRootAccess
                                     xAODEgamma
                                     xAODMuon
                                     xAODJet
                                     xAODMissingET
                                     xAODCutFlow
                                     AsgTools
                                     AthContainers
                                     PATInterfaces
                                     TopCPTools
                                     TopEventSelectionTools
                                     TopConfiguration
                                     TopCorrections
                                     TopEvent
                                     TopParticleLevel
                                     TopPartons
                                     TopObjectSelectionTools
                                     TopSystematicObjectMaker
                                     TopFakes
				     TopDataPreparation
                                     TopHLUpgrade
                                     ${ROOT_LIBRARIES}
                                     TopAnalysis )

