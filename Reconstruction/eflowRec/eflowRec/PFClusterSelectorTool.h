/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef PFCLUSTERSELECTORTOOL_H
#define PFCLUSTERSELECTORTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "eflowRec/IPFClusterSelectorTool.h"
#include "eflowRec/eflowRecCluster.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include <map>

class PFClusterSelectorTool : public extends<AthAlgTool, IPFClusterSelectorTool> {

public:
  /** Default constructor */
  PFClusterSelectorTool(const std::string& type,const std::string& name,const IInterface* parent);
  /** Default destructor */
  ~PFClusterSelectorTool() {};

  /** Gaudi AthAlgorithm hooks */
  StatusCode initialize();
  StatusCode execute(eflowRecClusterContainer& theEFlowRecClusterContainer,xAOD::CaloClusterContainer& theCaloClusterContainer);
  StatusCode finalize();

private:
  /** for EM mode, LC weight for cells are retrieved before doing any subtraction; they will be used after subtraction */
  void retrieveLCCalCellWeight(const double& energy, const unsigned& index, std::map<IdentifierHash,double>& cellsWeight,const xAOD::CaloClusterContainer& caloCalClustersContainer);
  
  /** ReadHandleKey for the CaloClusterContainer to be used as input */
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_caloClustersReadHandleKey{this,"clustersName","CaloTopoCluster","ReadHandleKey for the CaloClusterContainer to be used as input"};

  /** ReadHandleKey for the CaloClusterContainer, at LC scale, to be used as input */
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_caloCalClustersReadHandleKey{this,"calClustersName","CaloCalTopoClusters","ReadHandleKey for the CaloClusterContainer, at LC scale, to be used as input"};  
  
};
#endif
