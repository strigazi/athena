/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTOOL_RPCDETECTORDBTOOL_H
#define MUONCONDTOOL_RPCDETECTORDBTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "MuonCondInterface/IRpcDetectorStatusDbTool.h" 
//#include "MuonCondData/Rpc_ConditionDataContainer.h"
//#include "MuonCondData/RpcConditionPar.h"
#include "GaudiKernel/MsgStream.h"
//#include "StoreGate/StoreGateSvc.h"

class Identifier; 
class StoreGateSvc; 

class RpcIdHelper;
class IIOVSvc;
class StatusCode;


class RpcDetectorStatusDbTool: public AlgTool,
	             virtual public IRpcDetectorStatusDbTool
{

public:    

  RpcDetectorStatusDbTool(const std::string& type, 
		const std::string& name, 
		const IInterface* parent); 

  /** required by the IAddressProvider interface */
  //  virtual StatusCode updateAddress(SG::TransientAddress* tad);
  virtual StatusCode updateAddress(StoreID::type storeID, SG::TransientAddress* tad);

private: 
  bool FirstCall ;
  StatusCode initialize();
  
  virtual StatusCode loadParameterStatus(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode loadRpcDqStatus(IOVSVC_CALLBACK_ARGS);
  virtual std::string FolderName() const {return m_FolderName;}
  StoreGateSvc* m_detStore;
  IIOVSvc* m_IOVSvc;

  virtual const std::vector<Identifier>&  EffPanelId(){ return  m_effPanelId;}
  virtual const std::vector<Identifier>&  EffStripId(){ return  m_effStripId;}

  virtual const std::map<Identifier,     std::vector<double> >& RPC_TimeMapforStrip(){ return RPC_StripTimeMap;}

  virtual const std::map<Identifier,     double>& RPC_EfficiencyMap      (){ return RPC_PanelEfficiencyMap      ;}
  virtual const std::map<Identifier,     double>& RPC_EfficiencyGapMap   (){ return RPC_GapEfficiencyMap        ;}
  virtual const std::map<Identifier,     double>& RPC_MeanClusterSizeMap (){ return RPC_PanelMeanClusterSizeMap ;}
  virtual const std::map<Identifier,     double>& RPC_FracClusterSize1Map(){ return RPC_PanelFracClusterSize1Map;}
  virtual const std::map<Identifier,     double>& RPC_FracClusterSize2Map(){ return RPC_PanelFracClusterSize2Map;}
  virtual const std::map<Identifier,     double>& RPC_FracClusterSize3Map(){ return RPC_PanelFracClusterSize3Map;}
  virtual const std::map<Identifier,std::string>& RPC_DeadStripListMap   (){ return RPC_PanelDeadStripListMap   ;}
  virtual const std::map<Identifier,      float>& RPC_FracDeadStripMap   (){ return RPC_PanelFracDeadStripMap   ;}
  virtual const std::map<Identifier,        int>& RPC_ProjectedTracksMap (){ return RPC_PanelProjectedTracksMap ;}
  virtual const std::map<Identifier,        int>& RPC_DeadStripList      (){ return RPC_PanelDeadStripList      ;}

  const RpcIdHelper* m_rpcIdHelper;
  
  //mutable Rpc_ConditionDataContainer * m_rpcDqStatusData;

  double  m_RPC_PanelEfficiency;
  std::string      m_FolderName;
  std::string      m_rpcDqFolder;
  std::string      m_rpcDqStatusDataLocation;
  std::vector<Identifier> m_effPanelId;
  std::vector<Identifier> m_effStripId;
  std::map<Identifier ,     double> RPC_PanelEfficiencyMap      ;
  std::map<Identifier ,     double> RPC_GapEfficiencyMap        ;
  std::map<Identifier ,     double> RPC_PanelMeanClusterSizeMap ;
  std::map<Identifier ,     double> RPC_PanelFracClusterSize1Map;
  std::map<Identifier ,     double> RPC_PanelFracClusterSize2Map;
  std::map<Identifier ,     double> RPC_PanelFracClusterSize3Map;
  std::map<Identifier ,std::string> RPC_PanelDeadStripListMap   ;
  std::map<Identifier ,      float> RPC_PanelFracDeadStripMap   ;
  std::map<Identifier ,        int> RPC_PanelProjectedTracksMap ;
  std::map<Identifier ,        int> RPC_PanelDeadStripList      ;
  std::map<Identifier ,     std::vector<double> > RPC_StripTimeMap;

  MsgStream m_log;
  bool      m_debug;
  bool      m_verbose;

};


#endif
