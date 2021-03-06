/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeometryManager.h"

using namespace std;

PixelGeometryManager::PixelGeometryManager(const PixelGeoModelAthenaComps * athenaComps) :
  m_athenaComps(athenaComps)
{}

PixelGeometryManager::~PixelGeometryManager()
{}

// Default implementation return 0.
PixelLegacyManager * PixelGeometryManager::legacyManager() const
{
  return 0;
}


