/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_TANGENTTOCIRCLES_H
#define DCMATH_TANGENTTOCIRCLES_H

#include "TrkDriftCircleMath/Line.h"
#include "TrkDriftCircleMath/DriftCircle.h"

#include <cmath>
#include <vector>

namespace TrkDriftCircleMath {

  class TangentToCircles{
  public:
    typedef std::vector<Line> LineVec;
  public:
    TangentToCircles() : m_debug(false) { }

    LineVec tangentLines( const DriftCircle& dc1, const DriftCircle& dc2 ) const;

    void debug(bool debug) { m_debug = debug; }
  private:
    bool m_debug;
  };

}

#endif
