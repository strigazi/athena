/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUMONITORING_TRIGTAUMONITORALGORITHM_H
#define TRIGTAUMONITORING_TRIGTAUMONITORALGORITHM_H


#include "xAODTau/TauJet.h"
#include "xAODTau/TauxAODHelpers.h"
#include "xAODTau/TauJetContainer.h"
#include "TrigTauInfo.h"

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "CxxUtils/phihelper.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"

class TrigTauMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TrigTauMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigTauMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

 private:

  /*! List of triggers from menu */
  Gaudi::Property<std::vector<std::string>> m_trigInputList{this, "TriggerList", {}};

  /*! navigation method called by executeNavigation */
  StatusCode executeNavigation(const EventContext& ctx, const std::string trigItem,float, const std::string,
                               std::vector<std::pair<const xAOD::TauJet*, const TrigCompositeUtils::Decision*>> &) const;

  void fillRNNInputVars(const std::string trigger, std::vector<const xAOD::TauJet*> tau_vec, bool online) const;

  void fillDistributions(std::vector< std::pair< const xAOD::TauJet*, const TrigCompositeUtils::Decision * >> pairObjs, const std::string trigger) const;
  void fillEfficiencies( std::vector< std::pair<const xAOD::TauJet*, const TrigCompositeUtils::Decision*> > pairObjs, const std::string trigger) const;
  void fillEfficiency( std::vector< std::pair<const xAOD::TauJet*, const TrigCompositeUtils::Decision*> > pairObjs, const std::string trigger) const;

  inline double dR(const double eta1, const double phi1, const double eta2, const double phi2) const
  {
    double deta = std::fabs(eta1 - eta2);
    double dphi = std::fabs(CxxUtils::wrapToPi(phi1-phi2));
    return sqrt(deta*deta + dphi*dphi);
  };


  SG::ReadHandleKey< xAOD::TauJetContainer> m_offlineTauJetKey { this, "offlineTauJetKey", "TauJets", "Offline taujet container key" };
  SG::ReadHandleKey< xAOD::EmTauRoIContainer > m_l1TauRoIKey    { this, "l1TauRoIKey","LVL1EmTauRoIs","Tau L1 RoI key"};
  SG::ReadHandleKey< xAOD::TauJetContainer> m_hltTauJetKey { this, "hltTauJetKey", "HLT_TrigTauRecMerged_MVA", "HLT taujet container key" };
  SG::ReadHandleKey< xAOD::TauJetContainer> m_hltTauJetPrecisionKey { this, "hltTauJetPrecisionKey", "HLT_TrigTauRecMerged_Precision", "HLT taujet container key" };
  SG::ReadHandleKey< xAOD::TauJetContainer> m_hltTauJetPreselKey { this, "hltTauJetPreselKey", "HLT_TrigTauRecMerged_Presel", "HLT taujet container key" };
  SG::ReadHandleKey< xAOD::TauJetContainer> m_hltTauJetCaloOnlyMVAKey { this, "hltTauJetCaloOnlyMVAKey", "HLT_TrigTauRecMerged_CaloOnlyMVA", "HLT taujet container key" };
  SG::ReadHandleKey< xAOD::TauJetContainer> m_hltTauJetCaloOnlyKey { this, "hltTauJetCaloOnlyKey", "HLT_TrigTauRecMerged_CaloOnly", "HLT taujet container key" };

  std::map<std::string,TrigInfo> m_trigInfo;
  static const std::vector<std::string> m_trigLevel;

protected:

  std::map<std::string,TrigInfo> getTrigInfoMap() { return m_trigInfo; }
  asg::AcceptData setAccept(const TrigCompositeUtils::Decision*, std::string trigger) const;

  TrigInfo getTrigInfo(const std::string) const;

  //void setTrigInfo(const std::string);
  
};
#endif
