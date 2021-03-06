/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloClusterContainerCnv_p2.h" 
#include "CaloUtils/CaloClusterSignalState.h"

#include "AthAllocators/DataPool.h"
#include "AthenaKernel/errorcheck.h"


void CaloClusterContainerCnv_p2::persToTrans (const CaloClusterContainer_p2* pers, 
                                              CaloClusterContainer* trans,
                                              MsgStream &log) const
{
  // reset element link converters, and provide container name lookup table
  ShowerLinkState showerLinkState (pers->m_linkNames);
  CellLinkState cellLinkState (pers->m_linkNames);

  // Use data pool for clusters to avoid calling constructor for each event
  DataPool<CaloCluster> clusters;

  trans->clear (SG::VIEW_ELEMENTS);
  trans->reserve(pers->m_vec.size());
  CaloClusterMomentContainer_p1::const_iterator momentStoreIterator;
  CaloSamplingDataContainerCnv_p1::State samplingState;
  if ( ! m_momentContainerCnv.setIterator(&pers->m_momentContainer,
                                          pers->m_vec.size(),
                                          momentStoreIterator) ||
       ! m_samplingDataContainerCnv.setState(&pers->m_samplingDataContainer,
                                             pers->m_vec.size(),
                                             samplingState))
  {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, "CaloClusterContainerCnv_p2")
      << "Not converting CaloClusterContainer.";
    return;
  }

  CaloClusterContainer_p2::const_iterator itp=pers->m_vec.begin();
  CaloClusterContainer_p2::const_iterator itp_e=pers->m_vec.end();
  for(;itp!=itp_e;++itp) {

    // Get next ptr for next cluster
    CaloCluster* transCluster = clusters.nextElementPtr();

    //The var-type pattern is stored in CaloSamplingDataContainer_p1 (once for the entire cluster container)
    //transCluster->m_dataStore.m_varTypePattern=pers->m_samplingDataContainer.m_varTypePattern;
    //... not any more... 

    //Convert Cluster-quantities
    persToTrans(&(*itp),transCluster,
                showerLinkState,
                cellLinkState,
                log);
    //Convert sampling data store
    m_samplingDataContainerCnv.persToTrans(&(pers->m_samplingDataContainer),&(transCluster->m_dataStore),
                                           samplingState);
    //Convert moment store
    m_momentContainerCnv.persToTrans(&(pers->m_momentContainer),&(transCluster->m_momentStore),
                                     momentStoreIterator);
    trans->push_back(transCluster);
  }
  //Convert TowerSegment
  CaloTowerSeg seg;
  m_caloTowerSegCnv.persToTrans(&(pers->m_towerSeg),&seg);
  trans->setTowerSeg (seg);
}


void CaloClusterContainerCnv_p2::transToPers (const CaloClusterContainer* trans, 
                                              CaloClusterContainer_p2* pers,
                                              MsgStream &log) const
{
  // reset element link converters, and provide container name lookup table
  ShowerLinkState showerLinkState (pers->m_linkNames);
  CellLinkState cellLinkState (pers->m_linkNames);

  pers->m_vec.resize(trans->size());
  CaloClusterContainer_p2::iterator itp=pers->m_vec.begin();
  CaloClusterContainer::const_iterator it=trans->begin();
  CaloClusterContainer::const_iterator it_e=trans->end();
  if (it!=it_e) {
    //The var-type pattern is stored in CaloSamplingDataContainer_p1 (once for the entire container)
    //pers->m_samplingDataContainer.m_varTypePattern=(*it)->m_dataStore.m_varTypePattern;
    //... not any more...
    
    //The number of moments is also the same for the entire container
    pers->m_momentContainer.m_nMoments=(*it)->m_momentStore.size();
  }
  for(;it!=it_e;++it,++itp) {
    transToPers(*it,&(*itp),
                showerLinkState,
                cellLinkState,
                log);
    m_samplingDataContainerCnv.transToPers(&((*it)->m_dataStore),&(pers->m_samplingDataContainer),log);
    m_momentContainerCnv.transToPers(&((*it)->m_momentStore),&(pers->m_momentContainer));
  }
  m_caloTowerSegCnv.transToPers(&trans->getTowerSeg(),&(pers->m_towerSeg));
}



void CaloClusterContainerCnv_p2::persToTrans(const CaloClusterContainer_p2::CaloCluster_p* pers, 
					     CaloCluster* trans,
                                             ShowerLinkState& showerLinkState,
                                             CellLinkState& cellLinkState,
                                             MsgStream& log) const
{
  trans->setDefaultSignalState (P4SignalState::CALIBRATED);
  trans->setBasicEnergy (pers->m_basicSignal);
  trans->setTime (pers->m_time);
  trans->m_samplingPattern=pers->m_samplingPattern; 
  trans->m_barrel=pers->m_barrel;  
  trans->m_endcap=pers->m_endcap;
  trans->m_eta0=pers->m_eta0;
  trans->m_phi0=pers->m_phi0;   
  trans->m_status = CaloRecoStatus(pers->m_caloRecoStatus);
  trans->setClusterSize (pers->m_clusterSize);

  //trans->m_barrel=pers->m_samplingPattern & 0x1ff00f; //That's the OR of all barrel-bits
  //trans->m_endcap=pers->m_samplingPattern & 0xe00ff0; //That's the OR of all endcap-bits

  //Convert base class and element links
  m_P4EEtaPhiMCnv.persToTrans(&pers->m_P4EEtaPhiM,(P4EEtaPhiM*)trans,log);
  m_showerElementLinkCnv.persToTrans(showerLinkState, pers->m_dataLink, trans->m_dataLink,log);
  m_cellElementLinkCnv.persToTrans(cellLinkState, pers->m_cellLink, trans->m_cellLink,log);
  trans->setAthenaBarCode (IAthenaBarCode::UNDEFINEDBARCODE);
 
}




void CaloClusterContainerCnv_p2::transToPers(const CaloCluster* trans, 
					     CaloClusterContainer_p2::CaloCluster_p* pers,
                                             ShowerLinkState& showerLinkState,
                                             CellLinkState& cellLinkState,
                                             MsgStream& log) const
{
  pers->m_basicSignal=trans->getBasicEnergy();
  pers->m_time=trans->getTime();
  pers->m_samplingPattern=trans->m_samplingPattern; 
  pers->m_barrel=trans->m_barrel;  
  pers->m_endcap=trans->m_endcap;
  pers->m_eta0=trans->eta0();
  pers->m_phi0=trans->phi0(); 
  pers->m_caloRecoStatus=trans->m_status.getStatusWord();
  pers->m_clusterSize=trans->getClusterSize();
 
  //Convert base class and element links
  P4EEtaPhiM tmp = *trans;
  m_P4EEtaPhiMCnv.transToPers(&tmp,&pers->m_P4EEtaPhiM,log);
  m_showerElementLinkCnv.transToPers(showerLinkState, trans->m_dataLink, pers->m_dataLink,log);
  m_cellElementLinkCnv.transToPers(cellLinkState, trans->m_cellLink, pers->m_cellLink,log);
}
