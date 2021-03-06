// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SUSYToolsAlg.h

#ifndef SUSYToolsAlg_H
#define SUSYToolsAlg_H

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "SUSYTools/ISUSYObjDef_xAODTool.h"
#include "PATInterfaces/SystematicSet.h"
#include "TStopwatch.h"


class SUSYToolsAlg : public AthAnalysisAlgorithm { 

  public: 

    /// Constructor with parameters:
    SUSYToolsAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /// Destructor:
    ~SUSYToolsAlg();

    /// Athena algorithm's Hooks
    StatusCode  initialize();
    StatusCode  execute();
    StatusCode  finalize();

  private: 

    /// Default constructor:
    SUSYToolsAlg();

    int  m_dataSource;

    bool m_doSyst;
    std::vector<ST::SystInfo> sysInfoList;

    std::vector<std::string> syst_el_weights;
    std::vector<std::string> syst_mu_weights;
    std::vector<std::string> syst_ph_weights;
    std::vector<std::string> syst_tau_weights;
    std::vector<std::string> syst_jet_weights;
    std::vector<std::string> syst_btag_weights;
    std::vector<std::string> syst_event_weights;

    ToolHandle<ST::ISUSYObjDef_xAODTool> m_SUSYTools;

    unsigned int m_Nevts;

    TStopwatch m_clock0;
    TStopwatch m_clock1;
    TStopwatch m_clock2;

    std::string m_rateMonitoringPath;

    std::vector<std::string> el_triggers;
    std::vector<std::string> ph_triggers;
    std::vector<std::string> mu_triggers;

    int count_el_signal;
    int count_ph_signal;
    int count_mu_signal;
}; 



#endif
