################################################################################
# Package: MuonMomentumCorrections
################################################################################

# Declare the package name:
atlas_subdir( MuonMomentumCorrections )

# Extra dependencies, based on the environment:
set( extra_deps )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess )
else()
   set( extra_deps Control/AthenaBaseComps GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Control/AthToolSupport/AsgMessaging
   Control/AthToolSupport/AsgDataHandles
   Event/xAOD/xAODMuon
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/Interfaces/MuonAnalysisInterfaces
   PhysicsAnalysis/Interfaces/AsgAnalysisInterfaces
   PRIVATE
   PhysicsAnalysis/MuonID/MuonSelectorTools
   Event/xAOD/xAODBase
   Event/xAOD/xAODCore
   Event/xAOD/xAODEventInfo
   Tools/PathResolver
   ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree RIO Physics )

# Libraries in the package:
atlas_add_library( MuonMomentumCorrectionsLib
   MuonMomentumCorrections/*.h Root/*.cxx
   PUBLIC_HEADERS MuonMomentumCorrections
   INCLUDE_DIRS  ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES  ${ROOT_LIBRARIES} AsgTools xAODMuon MuonAnalysisInterfacesLib AsgAnalysisInterfaces
   PATInterfaces AsgDataHandlesLib AsgMessagingLib
   PRIVATE_LINK_LIBRARIES xAODEventInfo PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( MuonMomentumCorrections
      src/*.h src/*.cxx src/components/*.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODMuon xAODCore MuonAnalysisInterfacesLib AsgAnalysisInterfaces
      xAODBase GaudiKernel MuonSelectorToolsLib MuonMomentumCorrectionsLib StoreGateLib AsgMessagingLib )
endif()

atlas_add_dictionary( MuonMomentumCorrectionsDict
   MuonMomentumCorrections/MuonMomentumCorrectionsDict.h
   MuonMomentumCorrections/selection.xml
   LINK_LIBRARIES MuonMomentumCorrectionsLib )

if (NOT XAOD_STANDALONE)
# Executable(s) in the package:
atlas_add_executable( MCAST_Tester
   util/MCAST_Tester.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess
   xAODEventInfo xAODMuon xAODCore PATInterfaces xAODCore AsgTools AsgAnalysisInterfaces
   MuonSelectorToolsLib MuonMomentumCorrectionsLib  MuonAnalysisInterfacesLib AsgMessagingLib )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
