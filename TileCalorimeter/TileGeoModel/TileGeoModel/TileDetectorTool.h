/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileDetectorTool_H
#define TileDetectorTool_H

#include "GeoModelUtilities/GeoModelTool.h"

#include <string>

class MsgStream;
class StoreGateSvc;
class TileDetDescrManager;

class TileDetectorTool : public GeoModelTool 
{
 public:

  TileDetectorTool(const std::string& type, 
		   const std::string& name, 
		   const IInterface* parent);

  virtual ~TileDetectorTool() override final;
  
  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;

  bool isTestBeam() const { return m_testBeam; }
  bool isAddPlates() const { return m_addPlates; }
  int Ushape() const { return m_Ushape; }
  int setUshape(int Ushape) { if (m_not_locked) {m_Ushape = Ushape;} return m_Ushape; }

 private:

  StatusCode initIds(StoreGateSvc* detStore, TileDetDescrManager *manager, MsgStream * log);
  StatusCode createElements(TileDetDescrManager *manager,MsgStream *log);


  bool m_testBeam;
  bool m_addPlates;
  int  m_Ushape;

  /** 0: glue layer is removed and replaced by air, 
      1: simulation with glue, 
      2: glue is replaced by air + width of iron is modified in order to get the same sampling fraction      */
  int m_Glue;

  bool m_not_locked;
  bool m_useNewFactory;
  std::string m_geometryConfig; // FULL, SIMU, RECO
  TileDetDescrManager* m_manager;
};

#endif 
