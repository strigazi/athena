/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//@file TagFunctions.h
//@brief Helper functions for tag resolution
//@author Shaun Roe
#ifndef IOVDbSvc_TagFunctions_h
#define IOVDbSvc_TagFunctions_h

#include "CoolKernel/IFolder.h"
#include "EventInfo/TagInfo.h"
#include <string>
#include <optional>
class StoreGateSvc;

namespace IOVDbNamespace{
  ///Retrieve the TagInfo
  std::string getTagInfo(const std::string &tag, const StoreGateSvc* detStore);
  
  ///Get the GeoAtlas version directly from GeoModelSvc
  std::string getGeoAtlasVersion();
  
  ///Check whether a tag is locked on a folder, if possible
  std::optional<bool>
  checkTagLock(const cool::IFolderPtr fptr, const std::string & tag);
  
  std::string
  resolveUsingTagInfo(const std::string & tag, StoreGateSvc * pDetStore, const std::optional<TagInfo> & inputTag = std::nullopt);
}
#endif
