// Hi Emacs ! this is  -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:      TrigCaloClusterMaker
 * PACKAGE:   Trigger/TrigAlgorithms/TrigCaloRec
 *
 * AUTHOR:    P.A. Delsart
 * CREATED:   August 2006
 *
 *          Builds Clusters from CaloCell in a ROI using offline calo tools.
 *          Works currently only for CaloTopoCluster and SWClusters. (hard coded hack...)
 *********************************************************************/
#ifndef TRIGCALOREC_TRIGCALOCLUSTERMAKER_H
#define TRIGCALOREC_TRIGCALOCLUSTERMAKER_H

#include "TrigInterfaces/FexAlgo.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"


class IAlgToolEFCalo;
class CaloClusterCollectionProcessor;
class CaloClusterProcessor;
class CaloClusterContainer;
class CaloCellLinkContainer;
class TrigCaloQuality;

class TrigCaloClusterMaker : public HLT::FexAlgo {

 public:

  TrigCaloClusterMaker(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigCaloClusterMaker();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute( const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE );

 private:

  std::string retrieveCellContName ( HLT::TriggerElement* outputTE );
  std::string retrieveTowerContName( HLT::TriggerElement* outputTE );

  std::vector<std::string> m_clusterMakerNames;      // Cluster maker tools name
  std::vector<std::string> m_clusterCorrectionNames; // Cluster correction tools name


  // Following used for testing only :
  bool        m_useMeaningfullNames;      
  std::string m_clustersOutputName;
 
  // Timers
  std::vector<TrigTimer*> m_timer;

  //Other members  
  std::vector<CaloClusterCollectionProcessor*> m_clusterMakerPointers;      //pointers to tools
  std::vector<CaloClusterProcessor*>           m_clusterCorrectionPointers; //pointers to tools
  xAOD::CaloClusterContainer* m_pCaloClusterContainer;
  TrigCaloQuality*      pTrigCaloQuality;

  double m_ClusterContainerSize;

//   double m_Eta;
//   double m_Phi;
//   double m_Et;
//   double m_rCore;
//   double m_EtaEFvsL2;
//   double m_PhiEFvsL2;
//   double m_IsoFrac;
//   double m_StripWidth;
//   double m_EMFraction;
//   double m_EdRFraction;

  float m_L2Eta;
  float m_L2Phi;
  
  float m_eneFracMax;
  float m_clusterTime;
  long m_containBadCells;

  std::vector<float> m_Eta;
  std::vector<float> m_Phi;
  std::vector<float> m_Et;
  std::vector<float> m_EtaEFvsL2;
  std::vector<float> m_PhiEFvsL2;

public:

  inline xAOD::CaloClusterContainer* GetClusterContainer() const {return m_pCaloClusterContainer;}

};
#endif
