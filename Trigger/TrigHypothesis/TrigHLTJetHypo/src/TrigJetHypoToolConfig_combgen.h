/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPOTOOLConfig_COMBGEN_H
#define TRIGJETHYPOTOOLConfig_COMBGEN_H
/********************************************************************
 *
 * NAME:     TrigJetHypoToolConfig_combgen.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 *
 *********************************************************************/


#include "ITrigJetHypoToolConfig.h"
#include "./ConditionsDefsMT.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "TrigHLTJetHypo/ITrigJetHypoToolHelperMT.h"


#include "./ConditionsDefsMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerBridge.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigJetHypoToolConfig_combgen:
public extends<AthAlgTool, ITrigJetHypoToolConfig> {

 public:
  
  TrigJetHypoToolConfig_combgen(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);
  virtual ~TrigJetHypoToolConfig_combgen();

  virtual StatusCode initialize() override;
  virtual std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  virtual std::unique_ptr<IJetGrouper> getJetGrouper() const override;
  virtual std::unique_ptr<IGroupsMatcherMT> getMatcher() const override;

  virtual std::optional<ConditionsMT> getConditions() const override;

  virtual std::size_t requiresNJets() const override;
  
 private:
  
  Gaudi::Property<std::vector<std::string>>
    m_EtThresholds{this, "EtThresholds", {}, "Etthresholds by eta region"};
  
  Gaudi::Property<std::vector<std::string>>
    m_etaMins{this, "eta_mins", {}, "Eta min for eta regions"};
  
  Gaudi::Property<std::vector<std::string>>
    m_etaMaxs{this, "eta_maxs", {}, "Eta max for eta regions"};

  Gaudi::Property<std::vector<int>>
    m_asymmetricEtas{this, "asymmetricEtas", {}, "Apply asym. eta cuts"};

  std::size_t m_size{0};  // size of jet groups to pass to children

  ToolHandleArray<ITrigJetHypoToolHelperMT> m_children {
    this, "children", {}, "list of child jet hypo helpers"};

  virtual StatusCode checkVals()  const override;
 
};
#endif
