#ifndef PFNEUTRALFLOWELEMENTCREATORALGORITHM_H
#define PFNEUTRALFLOWELEMENTCREATORALGORITHM_H

#include "eflowRec/eflowCaloObject.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODPFlow/FlowElement.h"
#include "xAODPFlow/FlowElementContainer.h"
#include "xAODPFlow/PFODefs.h"

class PFNeutralFlowElementCreatorAlgorithm : public AthReentrantAlgorithm {

public:
  
  PFNeutralFlowElementCreatorAlgorithm(const std::string& name,ISvcLocator* pSvcLocator);

  ~PFNeutralFlowElementCreatorAlgorithm() {}

  static const InterfaceID& interfaceID();

  StatusCode initialize();
  StatusCode execute(const EventContext& ctx) const;
  StatusCode finalize();

private:

  /** Create the chargedneutral PFO */ 
  StatusCode createNeutralFlowElement(const eflowCaloObject& energyFlowCaloObject, xAOD::FlowElementContainer* neutralFEContainer) const;

  /** Function to add cluster moments onto PFO */
  void addMoment(const xAOD::CaloCluster::MomentType& momentType, const xAOD::PFODetails::PFOAttributes& pfoAttribute, const xAOD::CaloCluster& theCluster, xAOD::FlowElement& theFE) const;
 
  /** Toggle EOverP algorithm mode, whereby no charged shower subtraction is performed */
  Gaudi::Property<bool> m_eOverPMode{this,"EOverPMode",false,"Toggle EOverP algorithm mode, whereby no charged shower subtraction is performed"};

  /** Bool to toggle which jetetmiss configuration we are in - EM cluster input or LC cluster input */
  Gaudi::Property<bool> m_doClusterMoments{this,"DoClusterMoments",true,"Bool to toggle whether cluster moments are added to the PFOs"};

  /** Toggle usage of calibration hit truth - false by default */
  Gaudi::Property<bool> m_useCalibHitTruth{this,"UseCalibHitTruth",false,"Toggle usage of calibration hit truth - false by default"};

  /** Toggle addition of charged shower subtracted CaloCluster links to neutral PFO - false by default */
  Gaudi::Property<bool> m_addShowerSubtractedClusters{this,"AddShowerSubtractedClusters",false,"Toggle addition of charged shower subtracted CaloCluster links to neutral PFO - false by default"};

  /** ReadHandleKey for eflowCaloObjectContainer */
  SG::ReadHandleKey<eflowCaloObjectContainer> m_eflowCaloObjectContainerReadHandleKey{this,"eflowCaloObjectContainerName","eflowCaloObjects","ReadHandleKey for eflowCaloObjectContainer"};
  
  /** WriteHandleKey for neutral PFO */
  SG::WriteHandleKey<xAOD::FlowElementContainer> m_neutralFEContainerWriteHandleKey{this,"FEOutputName","JetETMissNeutralFlowElements","WriteHandleKey for neutral FlowElements"};

};
#endif
