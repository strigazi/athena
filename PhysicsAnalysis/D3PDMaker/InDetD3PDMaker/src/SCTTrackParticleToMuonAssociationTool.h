/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetD3PDMaker/src/SCTTrackParticleToMuonAssociationTool.cxx
 * @author <laura.rehnisch@cern.ch>
 * @date Nov, 2012
 * @brief setting up an association of trackparticles
 *        and muons.
 */
#ifndef INDETD3PDMAKER_SCTTRACKPARTICLETOMUONASSOCIATIONTOOL_H
#define INDETD3PDMAKER_SCTTRACKPARTICLETOMUONASSOCIATIONTOOL_H
 

#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "muonEvent/Muon.h"

namespace Rec {
  class TrackParticle;
}

namespace D3PD {
 
  class SCTTrackParticleToMuonAssociationTool
    : public SingleAssociationTool<Rec::TrackParticle, Analysis::Muon>
    {
    public:
      typedef SingleAssociationTool<Rec::TrackParticle, Analysis::Muon> Base; 

      SCTTrackParticleToMuonAssociationTool (const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent);
 
      virtual StatusCode initialize();
      virtual StatusCode book();


      virtual const Analysis::Muon* get (const Rec::TrackParticle& trackparticle);

    private:
      // StoreGate keys
      std::string m_muonContainer;

    }; // class SCTTrackParticleToMuonAssociationTool
 
} // namespace D3PD
 
#endif // not INDETD3PDMAKER_SCTTRACKPARTICLETOMUONASSOCIATIONTOOL_H

