/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IOODatabase.cpp 506608 2012-06-20 19:20:10Z gemmeren $
//====================================================================
//  Package    : StorageSvc (The POOL project)
//
//  @author      M.Frank
//====================================================================
#include "PersistentDataModel/Guid.h"
#include "StorageSvc/IOODatabase.h"

// Declaration of the interface ID needs to be unique within ONE process
const Guid& pool::IOODatabase::interfaceID()  {
  static const Guid id(true);
  return id;
}
