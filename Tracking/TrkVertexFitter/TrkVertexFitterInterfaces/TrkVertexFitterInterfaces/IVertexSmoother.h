/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IVertexSmoother, (c) ATLAS Detector software 2005
///////////////////////////////////////////////////////////////////

#ifndef TRKVERTEXFITTERINTERFACE_IVERTEXSMOOTHER_H
#define TRKVERTEXFITTERINTERFACE_IVERTEXSMOOTHER_H

#include "GaudiKernel/IAlgTool.h"

/**
 * @class Trk::IVertexSmoother
 * An abstract base class for the vertex smoothers
 * Update the tracks of type Trk::VxTrackAtVertex belonging to 
 * the Trk::VertexCandidate with the knowledge of the vertex 
 * position. The resulting set of tracks is stored in the 
 * VxCandidate. The actual update is performed as a loop
 * using IVertexTrackUpdator to update one track at the time.
 * 
 * @author Kirill.Prokofiev@cern.ch, Giacinto.Piacquadio@physik.uni-freiburg.de
 */

namespace Trk
{

  class VxCandidate;

  static const InterfaceID IID_IVertexSmoother("IVertexSmoother", 1, 0);

  class IVertexSmoother : virtual public IAlgTool 
  {

     public:
       /** 
        * Virtual destructor 
        */
       virtual ~IVertexSmoother(){};

       /**
        * AlgTool interface methods 
        */
       static const InterfaceID& interfaceID() { return IID_IVertexSmoother; };
  
       /**
        * Actual smooth method
        */
       virtual void smooth(const VxCandidate & vtx) const = 0;

  };
}//end of namespace definition

#endif
