################################################################################
# Package: BTagging
################################################################################

# Declare the package name:
atlas_subdir( BTagging )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Control/AthenaBaseComps
                          Event/xAOD/xAODBTagging
                          Event/xAOD/xAODJet
                          GaudiKernel
                          Reconstruction/Jet/JetInterface
                          Tracking/TrkEvent/VxVertex
                          PRIVATE
                          DetectorDescription/GeoPrimitives
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODJet
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          PhysicsAnalysis/AnalysisCommon/ParticleJetTools
                          PhysicsAnalysis/JetTagging/JetTagEvent
                          PhysicsAnalysis/JetTagging/JetTagInfo
                          PhysicsAnalysis/JetTagging/JetTagTools
                          PhysicsAnalysis/JetTagging/FlavorTagDiscriminants
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkLinks
                          Tracking/TrkEvent/VxJetVertex
                          Tracking/TrkEvent/VxSecVertex
                          MagneticField/MagFieldElements
                          MagneticField/MagFieldConditions
			  )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( BTaggingLib
                   src/BTagTool.cxx
                   src/BTagTrackAssociation.cxx
                   src/BTagSecVertexing.cxx
                   src/BTagLightSecVertexing.cxx
                   src/JetBTaggerTool.cxx
                   src/JetBTaggerAlg.cxx
                   src/JetBTaggingAlg.cxx
                   src/StandAloneJetBTaggerAlg.cxx
                   src/BTagJetPtScaling.cxx
                   src/JetParticleAssociationAlg.cxx
                   src/JetSecVertexingAlg.cxx
                   src/JetSecVtxFindingAlg.cxx
                   src/BTagTrackAugmenterAlg.cxx
                   src/BTagHighLevelAugmenterAlg.cxx
                   src/HighLevelBTagAlg.cxx
                   PUBLIC_HEADERS BTagging
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools AthenaBaseComps xAODBTagging xAODJet GaudiKernel JetInterface VxVertex ParticleJetToolsLib JetTagToolsLib FlavorTagDiscriminants
                   PRIVATE_LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives xAODBase xAODCore xAODMuon xAODTracking InDetRecToolInterfaces JetTagEvent JetTagInfo Particle TrkLinks VxJetVertex VxSecVertex MagFieldElements MagFieldConditions )

atlas_add_component( BTagging
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AsgTools AthenaBaseComps xAODBTagging xAODJet GaudiKernel JetInterface VxVertex GeoPrimitives xAODBase xAODCore xAODMuon xAODTracking InDetRecToolInterfaces ParticleJetToolsLib JetTagEvent JetTagInfo JetTagToolsLib FlavorTagDiscriminants Particle TrkLinks VxJetVertex VxSecVertex BTaggingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

