/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************
// ----------------------------------------------------------------------------
// PrimaryVertexRefitter header file
//
// James Catmore <James.Catmore@cern.ch>

// ----------------------------------------------------------------------------
// ****************************************************************************
#ifndef PRIMARYVERTEXREFITTER_H
#define PRIMARYVERTEXREFITTER_H
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>
#include <cmath>
#include <string>
/////////////////////////////////////////////////////////////////////////////

namespace Trk { 
  class VxCandidate;
  class TrackParticleBase;
  class VxTrackAtVertex;
  class RecVertex;
  class V0Tools;
  class ExtendedVxCandidate;
  class IVertexLinearizedTrackFactory;
  class IVertexUpdator;               

}
namespace Rec { class TrackParticle; }

namespace Analysis {

static const InterfaceID IID_PrimaryVertexRefitter("PrimaryVertexRefitter", 1, 0);

class PrimaryVertexRefitter:  virtual public AthAlgTool {
public:
        PrimaryVertexRefitter(const std::string& t, const std::string& n, const IInterface*  p);
        ~PrimaryVertexRefitter();
        StatusCode initialize();
        StatusCode finalize();

	static const InterfaceID& interfaceID() { return IID_PrimaryVertexRefitter;};

  	Trk::VxCandidate* refitVertex(Trk::VxCandidate* vertex, Trk::VxCandidate* excludeVertex);
  	Trk::VxCandidate* refitVertex(Trk::VxCandidate* vertex, std::vector<const Rec::TrackParticle*> tps);

private:
	ToolHandle < Trk::V0Tools > m_V0Tools;
        ToolHandle<Trk::IVertexLinearizedTrackFactory> m_linFactory;   
	ToolHandle<Trk::IVertexUpdator> m_Updator;                     

};
} // end of namespace
#endif

