/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ValAlgInterface_H
#define ValAlgInterface_H

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/DataHandle.h"

#include <iostream>

// base class for implementing basic functionality and provide 
// a common interface

class ValAlgInterface {
 public:
  inline ITHistSvc* tHistSvc();
  inline StoreGateSvc* storeGateSvc();
};

ITHistSvc* ValAlgInterface::tHistSvc()
{
  ITHistSvc* hSvc=0;
  if (!hSvc)
    {
      StatusCode status;
      ISvcLocator* svcLocator = Gaudi::svcLocator();
      status = svcLocator->service("THistSvc", hSvc);
      if (status.isFailure()) std::cout<<" could not get the THistSvc!!!"<<std::endl;
    }
  return hSvc;
}

StoreGateSvc* ValAlgInterface::storeGateSvc()
{
  StoreGateSvc* sGSvc=0;
  if (!sGSvc)
    {
      StatusCode status;
      ISvcLocator* svcLocator = Gaudi::svcLocator();
      status = svcLocator->service("StoreGateSvc", sGSvc);
      if (status.isFailure()) std::cout<<" could not get StoreGate!!!"<<std::endl;
    }
  return sGSvc;
}

#endif
