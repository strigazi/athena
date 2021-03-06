/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelDetectorFactory_h
#define PixelDetectorFactory_h 


#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h" 
// readout includes:
#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "CxxUtils/checker_macros.h"

class PixelSwitches;
class PixelGeometryManager;
class PixelGeoModelAthenaComps;

class PixelDetectorFactory : public InDetDD::DetectorFactoryBase {

 public:
  
  // Constructor:
  PixelDetectorFactory(const PixelGeoModelAthenaComps * athenaComps,
		       const PixelSwitches & switches) ATLAS_CTORDTOR_NOT_THREAD_SAFE; // Thread unsafe GeoVPixelFactory class is used.
  
  // Destructor:
  ~PixelDetectorFactory();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world);
  
  // Access to the results:
  virtual const InDetDD::PixelDetectorManager * getDetectorManager() const;


 private:  
  
  // Illegal operations:
  const PixelDetectorFactory & operator=(const PixelDetectorFactory &right);
  PixelDetectorFactory(const PixelDetectorFactory &right);
  // private data
  InDetDD::PixelDetectorManager     *m_detectorManager;
  PixelGeometryManager * m_geometryManager;

  void doChecks();

private:
  bool m_useDynamicAlignFolders;
  
};

// Class PixelDetectorFactory 
#endif


