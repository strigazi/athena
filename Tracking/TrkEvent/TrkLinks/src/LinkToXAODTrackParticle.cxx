/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkLinks/LinkToXAODTrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"


namespace Trk
{
	LinkToXAODTrackParticle::LinkToXAODTrackParticle() : ElementLink<xAOD::TrackParticleContainer>()
	{}

	LinkToXAODTrackParticle::LinkToXAODTrackParticle ( ElementLink<xAOD::TrackParticleContainer>& link ) : ElementLink<xAOD::TrackParticleContainer> ( link )
	{}

        const TrackParameters* LinkToXAODTrackParticle::parameters() const
	{
		if ( isValid() )
		{
			const xAOD::TrackParticle * trk = this->cachedElement();
			if ( nullptr != trk ) return dynamic_cast<const Trk::TrackParameters* > ( &trk->perigeeParameters() );
			return nullptr;
		}
		return nullptr;
	}//end of parameters method

        Trk::LinkToXAODTrackParticle* Trk::LinkToXAODTrackParticle::clone() const
	{
		return new LinkToXAODTrackParticle ( *this );
	}

}//end of namespace definitions
