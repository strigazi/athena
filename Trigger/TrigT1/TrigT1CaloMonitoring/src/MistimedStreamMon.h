/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:        MistimedStreamMon.h
// PACKAGE:     TrigT1CaloMonitoring
//
// Author:      Julia I. Djuvsland (julia.djuvsland@kip.uni-heidelberg.de)
//              Sebastian M. Weber (sebastian.weber@kip.uni-heidelberg.de)
//              Universitaet Heidelberg
//
// ********************************************************************

#ifndef TRIGT1CALOMONITORING_MISTIMEDSTREAMMON_H
#define TRIGT1CALOMONITORING_MISTIMEDSTREAMMON_H

#include <string>
#include <vector>

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

class TH1F_LW;
class TH2F_LW;
class TH2I_LW;

class StatusCode;
class EventInfo;

// ============================================================================
namespace LVL1 {
// ============================================================================
// Forward declarations:
// ============================================================================
class IL1TriggerTowerTool;
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramTool;

/** This class monitors events that fired the trigger HLT_mistimemonj400
 *  to spot potential late or mistimed trigger towers, but will also pick up noise
 *
 *  <b>ROOT Histogram Directories (Tier0):</b>
 *
 *  <table>
 *  <tr><th> Directory                                    </th><th> Contents               </th></tr>
 *  <tr><td> @c L1Calo/MistimedStream/EventsPerLumiBlock  </td><td> Selected events per lumiblock <br>
 *  </table>
 *
 *
 *  @authors Julia I. Djuvsland, Sebastian M. Weber
 *
 * */

class MistimedStreamMon: public ManagedMonitorToolBase
{

 public:
  
  MistimedStreamMon(const std::string & type, const std::string & name,const IInterface* parent);
  virtual ~MistimedStreamMon();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  
private:
  /// Tool to retrieve bytestream errors
  ToolHandle<ITrigT1CaloMonErrorTool>     m_errorTool;
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramTool>   m_histTool;
  /// TT simulation tool for Identifiers
  ToolHandle<LVL1::IL1TriggerTowerTool>   m_ttTool;

  /// Histograms booked flag
  bool m_histBooked;
  
  // eta-phi Map of EM TT classification
  TH2F_LW* m_h_em_2d_etaPhi_tt_classification_mistimedStreamAna;
  // Selected events per lumi block
  TH1F_LW* m_h_1d_selectedEvents_mistimedStreamAna;
  TH1F_LW* m_h_1d_cutFlow_mistimedStreamAna;

  //Variables for the properties
  /// Root directory
  std::string m_PathInRootFile;
  /// xAODTriggerTower Container key
  std::string m_xAODTriggerTowerContainerName;
    
};

// ============================================================================
}  // end namespace
// ============================================================================

#endif
