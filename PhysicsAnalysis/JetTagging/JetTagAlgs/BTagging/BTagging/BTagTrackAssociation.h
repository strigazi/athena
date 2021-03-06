/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_BTAGTRACKASSOCIATION_H
#define BTAGGING_BTAGTRACKASSOCIATION_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "BTagging/IBTagTrackAssociation.h"
#include "xAODMuon/MuonContainer.h"

#include <string>

/** The namespace of all packages in PhysicsAnalysis/JetTagging */
namespace Analysis
{
  class IBTagTrackAssociation;
  class ParticleToJetAssociator;

  class BTagTrackAssociation : public AthAlgTool, virtual public IBTagTrackAssociation
  {
      public:
          /** Constructors and destructors */
          BTagTrackAssociation(const std::string&,const std::string&,const IInterface*); //NameType& name);
          virtual ~BTagTrackAssociation();

          virtual StatusCode initialize() override;
          virtual StatusCode finalize() override;
	  virtual StatusCode BTagTrackAssociation_exec(jetcollection_t* theJets, const xAOD::TrackParticleContainer* tracks = 0) const override; //Kept for TrigBtagFex.cxx
	  virtual StatusCode BTagTrackAssociation_exec(const xAOD::JetContainer * jetContainer, xAOD::BTaggingContainer* btaggingContainer) const override;

      private:
          bool m_BTagAssociation;

          /** Jet Track Merger, Muon Merger*/
          /** There are 2 lists of associator tools: to associate tracks and muons
          */

          ToolHandleArray< Analysis::ParticleToJetAssociator > m_TrackToJetAssociatorList;
          ToolHandleArray< Analysis::ParticleToJetAssociator > m_MuonToJetAssociatorList;

          std::vector<std::string> m_TrackToJetAssocNameList;
          std::vector<std::string> m_MuonToJetAssocNameList;

          std::vector<std::string> m_TrackContainerNameList;
          //only one track container for test
          SG::ReadHandleKey<xAOD::TrackParticleContainer > m_TrackContainerName {this, "TrackContainerName", "InDetTrackParticles", "Input track particle container to build track-to-jet association"};
          std::vector<std::string> m_MuonContainerNameList;
          //only one muon container for test
          SG::ReadHandleKey<xAOD::MuonContainer > m_MuonContainerName {this, "MuonContainerName", "Muons", "Input muon container to build muon-to-jet associations"};

          std::vector<std::string> m_TracksToTagList;

          std::string m_collectionAppendix ;

  }; // End class

} // End namespace

#endif // BTAGTRACKASSOCIATION_H
