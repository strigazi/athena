/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFCLUSTERCOLLECTIONTOOL_H
#define PFCLUSTERCOLLECTIONTOOL_H

#include "eflowRec/IPFClusterCollectionTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

class PFClusterCollectionTool : virtual public IPFClusterCollectionTool, public AthAlgTool {

  public:
  
  PFClusterCollectionTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~PFClusterCollectionTool() {};

  static const InterfaceID& interfaceID();

  StatusCode initialize();
  std::unique_ptr<xAOD::CaloClusterContainer> execute(eflowCaloObjectContainer* theEflowCaloObjectContainer, bool useNonModifiedClusters, xAOD::CaloClusterContainer& theCaloClusterContainer);
  std::unique_ptr<eflowRecClusterContainer> retrieve(eflowCaloObjectContainer* theEflowCaloObjectContainer, bool useNonModifiedClusters);
  StatusCode finalize();

};
#endif

