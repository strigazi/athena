/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/PFMomentCalculatorTool.h"
#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowRecCluster.h"

#include "xAODCaloEvent/CaloClusterKineHelper.h"

PFMomentCalculatorTool::PFMomentCalculatorTool(const std::string& type,const std::string& name,const IInterface* parent) :
  base_class( type, name, parent)
{
}

StatusCode PFMomentCalculatorTool::initialize(){

  /* Retrieve the cluster collection tool */
  ATH_CHECK(m_clusterCollectionTool.retrieve());

  /* Retrieve the cluster moments maker */
  if ( m_clusterMomentsMaker.retrieve().isFailure() ) {
    ATH_MSG_WARNING("Cannot find CaloClusterMomentsMaker Tool");
    return StatusCode::SUCCESS;
  }
 
  /* Retrieve the cluster calib hit moments maker */
  if (m_useCalibHitTruth){
    if ( m_clusterCalibHitMomentsMaker2.retrieve().isFailure() ) {
      ATH_MSG_WARNING("Cannot find CaloCalibClusterMomentsMaker2 Tool");
      return StatusCode::SUCCESS;
    }
  }
  else {
    m_clusterCalibHitMomentsMaker2.disable();
  }
 
  return StatusCode::SUCCESS;
}

void PFMomentCalculatorTool::execute(const eflowCaloObjectContainer& theEflowCaloObjectContainer) {

  /* Collect all the clusters in a temporary container (with VIEW_ELEMENTS!) */
  bool useNonModifiedClusters = true;
  if (true == m_LCMode) useNonModifiedClusters = false;
  std::unique_ptr<xAOD::CaloClusterContainer> tempClusterContainer = m_clusterCollectionTool->execute(theEflowCaloObjectContainer, useNonModifiedClusters);

  /* Set the layer energies */
  /* This must be set before the cluster moment calculations, which use the layer energies */
  for (auto cluster : *tempClusterContainer) CaloClusterKineHelper::calculateKine(cluster, true, true);

  /* Remake the cluster moments */
  if (m_clusterMomentsMaker->execute(tempClusterContainer.get()).isFailure()) ATH_MSG_WARNING("Could not execute ClusterMomentsMaker");

  if (m_useCalibHitTruth){
    if (m_clusterCalibHitMomentsMaker2->execute(tempClusterContainer.get()).isFailure()) ATH_MSG_WARNING("Could not execute CaloCalibClusterMomentsMaker2");
  }

  
}

StatusCode PFMomentCalculatorTool::finalize(){ return StatusCode::SUCCESS; }


