/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELIBLFWDSVC_CADMODEL_H
#define GEOPIXELIBLFWDSVC_CADMODEL_H

#include "GeoVPixelFactory.h"

#include "GeoPixelStaveSupport.h"
#include "GeoPixelLadder.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoPhysVol.h"

class GeoTransform;

class GeoPixelIBLFwdSvcCADModel :  public GeoVPixelFactory {

public:  

  GeoPixelIBLFwdSvcCADModel(int);
  virtual GeoVPhysVol* Build();

  GeoPhysVol* getSupportA() const { return m_supportPhysA; }
  GeoPhysVol* getSupportC() const { return m_supportPhysC; }
  GeoTransform* getSupportTrfA() const { return m_xformSupportA; }
  GeoTransform* getSupportTrfC() const { return m_xformSupportC; }

 private:

  GeoPhysVol *m_supportPhysA;
  GeoPhysVol *m_supportPhysC;
  GeoTransform *m_xformSupportA;
  GeoTransform *m_xformSupportC;

  const GeoShape* addShape(const GeoShape * lastShape, const GeoShape * nextShape, const GeoTrf::Transform3D & trans);
};

#endif
