################################################################################
# Package: JetEvent
################################################################################

# Declare the package name:
atlas_subdir( JetEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthContainers
                          Control/AthLinks
                          Control/Navigation
                          Control/StoreGate
                          Event/EventKernel
                          Event/FourMom
                          Event/NavFourMom
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( JetEvent
                   src/*.cxx
                   PUBLIC_HEADERS JetEvent
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers AthLinks Navigation AthenaKernel EventKernel FourMom NavFourMom GaudiKernel ParticleEvent StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( JetEventDict
                      JetEvent/JetEventDict.h
                      JetEvent/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers AthLinks Navigation AthenaKernel StoreGateLib SGtests EventKernel FourMom NavFourMom GaudiKernel ParticleEvent JetEvent
                      NAVIGABLES JetCollection )

