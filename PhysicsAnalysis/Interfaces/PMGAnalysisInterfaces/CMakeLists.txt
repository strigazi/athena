# The name of the package:
atlas_subdir( PMGAnalysisInterfaces )

# The dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   PhysicsAnalysis/AnalysisCommon/PATInterfaces )

# Component(s) in the package:
atlas_add_library( PMGAnalysisInterfacesLib
   PMGAnalysisInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS PMGAnalysisInterfaces
   LINK_LIBRARIES AsgTools PATInterfaces )

atlas_add_dictionary( PMGAnalysisInterfacesDict
   PMGAnalysisInterfaces/PMGAnalysisInterfacesDict.h
   PMGAnalysisInterfaces/selection.xml
   LINK_LIBRARIES PMGAnalysisInterfacesLib )
