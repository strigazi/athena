/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_ALGF_V
#define ATHEXHIVE_ALGF_V 1

#include "HiveAlgBase.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "AthExHive/HiveDataObj.h"
#include "rGen.h"

#include <string>

class HiveAlgV  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgV (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgV();

  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  SG::ReadHandleKeyArray<HiveDataObj> m_rhv;
   
};
#endif
