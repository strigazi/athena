
# Declare the name of this package:
atlas_subdir( TopObjectSelectionTools )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          xAODBase
                          xAODPrimitives
                          xAODEventInfo
                          xAODEgamma
                          xAODMuon
                          xAODTau
                          xAODTracking
                          ElectronPhotonSelectorTools
                          MuonSelectorTools
                          TopEvent
                          TopSystematicObjectMaker
                          AssociationUtils
                          TrigConfInterfaces
                          TrigDecisionTool
                          TriggerMatchingTool
                          TrigTauMatching
                          GoodRunsLists
                          JetJvtEfficiency
                          xAODBTaggingEfficiency
                          JetSubStructureMomentTools
                          JetSubStructureUtils
                          JetReclustering )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TopObjectSelectionTools _cintDictSource
   ROOT_HEADERS TopObjectSelectionTools/RCJetMC15.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopObjectSelectionTools Root/*.cxx Root/*.h Root/*.icc
                   TopObjectSelectionTools/*.h TopObjectSelectionTools/*.icc TopObjectSelectionTools/*/*.h
                   TopObjectSelectionTools/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TopObjectSelectionTools
                   LINK_LIBRARIES xAODBase
                                  xAODPrimitives
                                  xAODEventInfo
                                  xAODEgamma
                                  xAODMuon
                                  xAODTau
                                  xAODTracking
                                  ElectronPhotonSelectorToolsLib
                                  MuonSelectorToolsLib
                                  TopEvent
                                  TopSystematicObjectMaker
                                  AssociationUtilsLib
                                  TrigConfInterfaces
                                  TrigDecisionToolLib
                                  TriggerMatchingToolLib
                                  TrigTauMatchingLib
                                  GoodRunsListsLib
                                  JetJvtEfficiencyLib
                                  xAODBTaggingEfficiencyLib
                                  JetSubStructureMomentToolsLib
                                  JetSubStructureUtils
                                  JetReclusteringLib
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install data files from the package:
atlas_install_data( share/* )

