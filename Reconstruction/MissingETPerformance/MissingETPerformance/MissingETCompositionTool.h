/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETCOMPOSITION_TOOL_H 
#define MISSINGETCOMPOSITION_TOOL_H 

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "MissingETPerformance/MissingETData.h"

#include <string>

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

static const InterfaceID IID_MissingETCompositionTool("MissingETCompositionTool", 1, 0);

class evt_display;

class MissingETCompositionTool : public AthAlgTool {

public:

  MissingETCompositionTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_MissingETCompositionTool; };

  StatusCode CBNT_initialize();

  /** Overriding initialize and finalize */
  virtual StatusCode initialize();
  /** finalization */
  virtual StatusCode finalize();
  virtual StatusCode execute(MissingETData *data);

   inline void SetFolderName(const std::string& folderName) { m_folderName=folderName; }

 private:
  StatusCode CBNT_initialize_jets();
  StatusCode CBNT_initialize_taus();
  StatusCode CBNT_initialize_electrons();
  StatusCode CBNT_initialize_gammas();
  StatusCode CBNT_initialize_muonboy();
  StatusCode CBNT_initialize_muid();
  StatusCode CBNT_initialize_refmuon();
  StatusCode CBNT_initialize_calocluster();
  StatusCode CBNT_initialize_eflow();
  StatusCode CBNT_initialize_tracks();

  StatusCode missingETCompositionPlots(MissingETData *data);

  /** a handle on the Hist/TTree registration service */
  ITHistSvc * m_thistSvc;

  std::string m_evtInfoKey;
  std::string m_folderName;
  bool m_make_event_displays;
  unsigned int m_n_event_displays;

  //histograms
  TH2F *m_h_New_V_Old_RefJet_metx;
  TH1F *m_h_New_minus_Old_RefJet_metx;
  TH2F *m_h_New_V_Old_RefJet_mety;
  TH1F *m_h_New_minus_Old_RefJet_mety;
  TH2F *m_h_New_V_Old_RefJet_met;
  TH1F *m_h_New_minus_Old_RefJet_met;

  TH2F *m_h_New_V_Old_SoftJets_metx;
  TH1F *m_h_New_minus_Old_SoftJets_metx;
  TH2F *m_h_New_V_Old_SoftJets_mety;
  TH1F *m_h_New_minus_Old_SoftJets_mety;
  TH2F *m_h_New_V_Old_SoftJets_met;
  TH1F *m_h_New_minus_Old_SoftJets_met;

  TH2F *m_h_jetweightx_V_jetweighty;
  TH2F *m_h_jetweightx_V_jetweightet;
  TH1F *m_h_jetweightx;
  TH2F *m_h_jetweightx_V_jetpt;
  TH2F *m_h_jetweightx_V_jetpx;
  TH2F *m_h_jetweightx_V_jeteta;
  TH2F *m_h_jetweightx_V_jetphi;
  TH3F *m_h_jetweightx_V_jeteta_V_jetphi;
  TH3F *m_h_jetweightx_V_jeteta_V_jetpt;
  TH1F *m_h_jetweighty;
  TH2F *m_h_jetweighty_V_jetpt;
  TH2F *m_h_jetweighty_V_jetpy;
  TH2F *m_h_jetweighty_V_jeteta;
  TH2F *m_h_jetweighty_V_jetphi;
  TH3F *m_h_jetweighty_V_jeteta_V_jetphi;
  TH3F *m_h_jetweighty_V_jeteta_V_jetpt;
  TH1F *m_h_jetweightet;
  TH2F *m_h_jetweightet_V_jetpt;
  TH2F *m_h_jetweightet_V_jeteta;
  TH2F *m_h_jetweightet_V_jetphi;
  TH3F *m_h_jetweightet_V_jeteta_V_jetphi;
  TH3F *m_h_jetweightet_V_jeteta_V_jetpt;

  TH2F *m_h_tauweightx_V_tauweighty;
  TH2F *m_h_tauweightx_V_tauweightet;
  TH2F *m_h_New_V_Old_RefTau_metx;
  TH1F *m_h_New_minus_Old_RefTau_metx;
  TH2F *m_h_New_V_Old_RefTau_mety;
  TH1F *m_h_New_minus_Old_RefTau_mety;
  TH2F *m_h_New_V_Old_RefTau_met;
  TH1F *m_h_New_minus_Old_RefTau_met;
  TH1F *m_h_tauweightx;
  TH2F *m_h_tauweightx_V_taupt;
  TH2F *m_h_tauweightx_V_taupx;
  TH2F *m_h_tauweightx_V_taueta;
  TH2F *m_h_tauweightx_V_tauphi;
  TH3F *m_h_tauweightx_V_taueta_V_tauphi;
  TH3F *m_h_tauweightx_V_taueta_V_taupt;
  TH1F *m_h_tauweighty;
  TH2F *m_h_tauweighty_V_taupt;
  TH2F *m_h_tauweighty_V_taupy;
  TH2F *m_h_tauweighty_V_taueta;
  TH2F *m_h_tauweighty_V_tauphi;
  TH3F *m_h_tauweighty_V_taueta_V_tauphi;
  TH3F *m_h_tauweighty_V_taueta_V_taupt;
  TH1F *m_h_tauweightet;
  TH2F *m_h_tauweightet_V_taupt;
  TH2F *m_h_tauweightet_V_taueta;
  TH2F *m_h_tauweightet_V_tauphi;
  TH3F *m_h_tauweightet_V_taueta_V_tauphi;
  TH3F *m_h_tauweightet_V_taueta_V_taupt;

  TH2F *m_h_eleweightx_V_eleweighty;
  TH2F *m_h_eleweightx_V_eleweightet;
  TH2F *m_h_New_V_Old_RefEle_metx;
  TH1F *m_h_New_minus_Old_RefEle_metx;
  TH2F *m_h_New_V_Old_RefEle_mety;
  TH1F *m_h_New_minus_Old_RefEle_mety;
  TH2F *m_h_New_V_Old_RefEle_met;
  TH1F *m_h_New_minus_Old_RefEle_met;
  TH1F *m_h_eleweightx;
  TH2F *m_h_eleweightx_V_elept;
  TH2F *m_h_eleweightx_V_elepx;
  TH2F *m_h_eleweightx_V_eleeta;
  TH2F *m_h_eleweightx_V_elephi;
  TH3F *m_h_eleweightx_V_eleeta_V_elephi;
  TH3F *m_h_eleweightx_V_eleeta_V_elept;
  TH1F *m_h_eleweighty;
  TH2F *m_h_eleweighty_V_elept;
  TH2F *m_h_eleweighty_V_elepy;
  TH2F *m_h_eleweighty_V_eleeta;
  TH2F *m_h_eleweighty_V_elephi;
  TH3F *m_h_eleweighty_V_eleeta_V_elephi;
  TH3F *m_h_eleweighty_V_eleeta_V_elept;
  TH1F *m_h_eleweightet;
  TH2F *m_h_eleweightet_V_elept;
  TH2F *m_h_eleweightet_V_eleeta;
  TH2F *m_h_eleweightet_V_elephi;
  TH3F *m_h_eleweightet_V_eleeta_V_elephi;
  TH3F *m_h_eleweightet_V_eleeta_V_elept;

  TH2F *m_h_gammaweightx_V_gammaweighty;
  TH2F *m_h_gammaweightx_V_gammaweightet;
  TH2F *m_h_New_V_Old_RefGamma_metx;
  TH1F *m_h_New_minus_Old_RefGamma_metx;
  TH2F *m_h_New_V_Old_RefGamma_mety;
  TH1F *m_h_New_minus_Old_RefGamma_mety;
  TH2F *m_h_New_V_Old_RefGamma_met;
  TH1F *m_h_New_minus_Old_RefGamma_met;
  TH1F *m_h_gammaweightx;
  TH2F *m_h_gammaweightx_V_gammapt;
  TH2F *m_h_gammaweightx_V_gammapx;
  TH2F *m_h_gammaweightx_V_gammaeta;
  TH2F *m_h_gammaweightx_V_gammaphi;
  TH3F *m_h_gammaweightx_V_gammaeta_V_gammaphi;
  TH3F *m_h_gammaweightx_V_gammaeta_V_gammapt;
  TH1F *m_h_gammaweighty;
  TH2F *m_h_gammaweighty_V_gammapt;
  TH2F *m_h_gammaweighty_V_gammapy;
  TH2F *m_h_gammaweighty_V_gammaeta;
  TH2F *m_h_gammaweighty_V_gammaphi;
  TH3F *m_h_gammaweighty_V_gammaeta_V_gammaphi;
  TH3F *m_h_gammaweighty_V_gammaeta_V_gammapt;
  TH1F *m_h_gammaweightet;
  TH2F *m_h_gammaweightet_V_gammapt;
  TH2F *m_h_gammaweightet_V_gammaeta;
  TH2F *m_h_gammaweightet_V_gammaphi;
  TH3F *m_h_gammaweightet_V_gammaeta_V_gammaphi;
  TH3F *m_h_gammaweightet_V_gammaeta_V_gammapt;

  TH2F *m_h_muonweightx_V_muonweighty;
  TH2F *m_h_muonweightx_V_muonweightet;
  TH2F *m_h_New_V_Old_MuonBoy_metx;
  TH1F *m_h_New_minus_Old_MuonBoy_metx;
  TH2F *m_h_New_V_Old_MuonBoy_mety;
  TH1F *m_h_New_minus_Old_MuonBoy_mety;
  TH2F *m_h_New_V_Old_MuonBoy_met;
  TH1F *m_h_New_minus_Old_MuonBoy_met;
  TH1F *m_h_muonweightx;
  TH2F *m_h_muonweightx_V_muonpt;
  TH2F *m_h_muonweightx_V_muonpx;
  TH2F *m_h_muonweightx_V_muoneta;
  TH2F *m_h_muonweightx_V_muonphi;
  TH3F *m_h_muonweightx_V_muoneta_V_muonphi;
  TH3F *m_h_muonweightx_V_muoneta_V_muonpt;
  TH1F *m_h_muonweighty;
  TH2F *m_h_muonweighty_V_muonpt;
  TH2F *m_h_muonweighty_V_muonpy;
  TH2F *m_h_muonweighty_V_muoneta;
  TH2F *m_h_muonweighty_V_muonphi;
  TH3F *m_h_muonweighty_V_muoneta_V_muonphi;
  TH3F *m_h_muonweighty_V_muoneta_V_muonpt;
  TH1F *m_h_muonweightet;
  TH2F *m_h_muonweightet_V_muonpt;
  TH2F *m_h_muonweightet_V_muoneta;
  TH2F *m_h_muonweightet_V_muonphi;
  TH3F *m_h_muonweightet_V_muoneta_V_muonphi;
  TH3F *m_h_muonweightet_V_muoneta_V_muonpt;

  TH2F *m_h_muidweightx_V_muidweighty;
  TH2F *m_h_muidweightx_V_muidweightet;
  TH2F *m_h_New_V_Old_MuID_metx;
  TH1F *m_h_New_minus_Old_MuID_metx;
  TH2F *m_h_New_V_Old_MuID_mety;
  TH1F *m_h_New_minus_Old_MuID_mety;
  TH2F *m_h_New_V_Old_MuID_met;
  TH1F *m_h_New_minus_Old_MuID_met;
  TH1F *m_h_muidweightx;
  TH2F *m_h_muidweightx_V_muonpt;
  TH2F *m_h_muidweightx_V_muonpx;
  TH2F *m_h_muidweightx_V_muoneta;
  TH2F *m_h_muidweightx_V_muonphi;
  TH3F *m_h_muidweightx_V_muoneta_V_muonphi;
  TH3F *m_h_muidweightx_V_muoneta_V_muonpt;
  TH1F *m_h_muidweighty;
  TH2F *m_h_muidweighty_V_muonpt;
  TH2F *m_h_muidweighty_V_muonpy;
  TH2F *m_h_muidweighty_V_muoneta;
  TH2F *m_h_muidweighty_V_muonphi;
  TH3F *m_h_muidweighty_V_muoneta_V_muonphi;
  TH3F *m_h_muidweighty_V_muoneta_V_muonpt;
  TH1F *m_h_muidweightet;
  TH2F *m_h_muidweightet_V_muonpt;
  TH2F *m_h_muidweightet_V_muoneta;
  TH2F *m_h_muidweightet_V_muonphi;
  TH3F *m_h_muidweightet_V_muoneta_V_muonphi;
  TH3F *m_h_muidweightet_V_muoneta_V_muonpt;

  TH2F *m_h_refmuonweightx_V_refmuonweighty;
  TH2F *m_h_refmuonweightx_V_refmuonweightet;
  TH2F *m_h_New_V_Old_refMuon_metx;
  TH1F *m_h_New_minus_Old_refMuon_metx;
  TH2F *m_h_New_V_Old_refMuon_mety;
  TH1F *m_h_New_minus_Old_refMuon_mety;
  TH2F *m_h_New_V_Old_refMuon_met;
  TH1F *m_h_New_minus_Old_refMuon_met;
  TH1F *m_h_refmuonweightx;
  TH2F *m_h_refmuonweightx_V_muonpt;
  TH2F *m_h_refmuonweightx_V_muonpx;
  TH2F *m_h_refmuonweightx_V_muoneta;
  TH2F *m_h_refmuonweightx_V_muonphi;
  TH3F *m_h_refmuonweightx_V_muoneta_V_muonphi;
  TH3F *m_h_refmuonweightx_V_muoneta_V_muonpt;
  TH1F *m_h_refmuonweighty;
  TH2F *m_h_refmuonweighty_V_muonpt;
  TH2F *m_h_refmuonweighty_V_muonpy;
  TH2F *m_h_refmuonweighty_V_muoneta;
  TH2F *m_h_refmuonweighty_V_muonphi;
  TH3F *m_h_refmuonweighty_V_muoneta_V_muonphi;
  TH3F *m_h_refmuonweighty_V_muoneta_V_muonpt;
  TH1F *m_h_refmuonweightet;
  TH2F *m_h_refmuonweightet_V_muonpt;
  TH2F *m_h_refmuonweightet_V_muoneta;
  TH2F *m_h_refmuonweightet_V_muonphi;

  TH2F *m_h_clusweightx_V_clusweighty;
  TH2F *m_h_clusweightx_V_clusweightet;
  TH2F *m_h_New_V_Old_CellOut_metx;
  TH1F *m_h_New_minus_Old_CellOut_metx;
  TH2F *m_h_New_V_Old_CellOut_mety;
  TH1F *m_h_New_minus_Old_CellOut_mety;
  TH2F *m_h_New_V_Old_CellOut_met;
  TH1F *m_h_New_minus_Old_CellOut_met;
  TH1F *m_h_clusweightx;
  TH2F *m_h_clusweightx_V_cluspt;
  TH2F *m_h_clusweightx_V_cluspx;
  TH2F *m_h_clusweightx_V_cluseta;
  TH2F *m_h_clusweightx_V_clusphi;
  TH3F *m_h_clusweightx_V_cluseta_V_clusphi;
  TH3F *m_h_clusweightx_V_cluseta_V_cluspt;
  TH1F *m_h_clusweighty;
  TH2F *m_h_clusweighty_V_cluspt;
  TH2F *m_h_clusweighty_V_cluspy;
  TH2F *m_h_clusweighty_V_cluseta;
  TH2F *m_h_clusweighty_V_clusphi;
  TH3F *m_h_clusweighty_V_cluseta_V_clusphi;
  TH3F *m_h_clusweighty_V_cluseta_V_cluspt;
  TH1F *m_h_clusweightet;
  TH2F *m_h_clusweightet_V_cluspt;
  TH2F *m_h_clusweightet_V_cluseta;
  TH2F *m_h_clusweightet_V_clusphi;
  TH3F *m_h_clusweightet_V_cluseta_V_clusphi;
  TH3F *m_h_clusweightet_V_cluseta_V_cluspt;

  TH2F *m_h_New_V_Old_CellOutEflow_metx;
  TH1F *m_h_New_minus_Old_CellOutEflow_metx;
  TH2F *m_h_New_V_Old_CellOutEflow_mety;
  TH1F *m_h_New_minus_Old_CellOutEflow_mety;
  TH2F *m_h_New_V_Old_CellOutEflow_met;
  TH1F *m_h_New_minus_Old_CellOutEflow_met;
  TH2F *m_h_eflow_clusweightx_V_clusweighty;
  TH2F *m_h_eflow_clusweightx_V_clusweightet;
  TH1F *m_h_eflow_clusweightx;
  TH2F *m_h_eflow_clusweightx_V_cluspt;
  TH2F *m_h_eflow_clusweightx_V_cluspx;
  TH2F *m_h_eflow_clusweightx_V_cluseta;
  TH2F *m_h_eflow_clusweightx_V_clusphi;
  TH3F *m_h_eflow_clusweightx_V_cluseta_V_clusphi;
  TH3F *m_h_eflow_clusweightx_V_cluseta_V_cluspt;
  TH1F *m_h_eflow_clusweighty;
  TH2F *m_h_eflow_clusweighty_V_cluspt;
  TH2F *m_h_eflow_clusweighty_V_cluspy;
  TH2F *m_h_eflow_clusweighty_V_cluseta;
  TH2F *m_h_eflow_clusweighty_V_clusphi;
  TH3F *m_h_eflow_clusweighty_V_cluseta_V_clusphi;
  TH3F *m_h_eflow_clusweighty_V_cluseta_V_cluspt;
  TH1F *m_h_eflow_clusweightet;
  TH2F *m_h_eflow_clusweightet_V_cluspt;
  TH2F *m_h_eflow_clusweightet_V_cluseta;
  TH2F *m_h_eflow_clusweightet_V_clusphi;
  TH3F *m_h_eflow_clusweightet_V_cluseta_V_clusphi;
  TH3F *m_h_eflow_clusweightet_V_cluseta_V_cluspt;

  TH2F *m_h_eflow_trkweightx_V_trkweighty;
  TH2F *m_h_eflow_trkweightx_V_trkweightet;
  TH1F *m_h_eflow_trkweightx;
  TH2F *m_h_eflow_trkweightx_V_trkpt;
  TH2F *m_h_eflow_trkweightx_V_trkpx;
  TH2F *m_h_eflow_trkweightx_V_trketa;
  TH2F *m_h_eflow_trkweightx_V_trkphi;
  TH3F *m_h_eflow_trkweightx_V_trketa_V_trkphi;
  TH3F *m_h_eflow_trkweightx_V_trketa_V_trkpt;
  TH1F *m_h_eflow_trkweighty;
  TH2F *m_h_eflow_trkweighty_V_trkpt;
  TH2F *m_h_eflow_trkweighty_V_trkpy;
  TH2F *m_h_eflow_trkweighty_V_trketa;
  TH2F *m_h_eflow_trkweighty_V_trkphi;
  TH3F *m_h_eflow_trkweighty_V_trketa_V_trkphi;
  TH3F *m_h_eflow_trkweighty_V_trketa_V_trkpt;
  TH1F *m_h_eflow_trkweightet;
  TH2F *m_h_eflow_trkweightet_V_trkpt;
  TH2F *m_h_eflow_trkweightet_V_trketa;
  TH2F *m_h_eflow_trkweightet_V_trkphi;
  TH3F *m_h_eflow_trkweightet_V_trketa_V_trkphi;
  TH3F *m_h_eflow_trkweightet_V_trketa_V_trkpt;

  std::vector<evt_display *> m_event_displays;

  TH1F *m_h_njets_compmap;
  TH1F *m_h_neles_compmap;
  TH1F *m_h_ngammas_compmap;
  TH1F *m_h_ntaus_compmap;
  TH1F *m_h_nmuboy_compmap;
  TH1F *m_h_nmuids_compmap;
  TH1F *m_h_nrefmuon_compmap;
  TH1F *m_h_nclus_compmap;
  TH1F *m_h_eflow_nclus_compmap;
  TH1F *m_h_eflow_ntrks_compmap;
};

class evt_display {
 public:
  evt_display(int run, int event, ITHistSvc *thistSvc, const std::string &folderName, MsgStream *mlog);
  evt_display() {}
  ~evt_display();
  TH2F *h_jeteta_V_phi_evt;
  TH2F *h_eleeta_V_phi_evt;
  TH2F *h_gammaeta_V_phi_evt;
  TH2F *h_taueta_V_phi_evt;
  TH2F *h_muoneta_V_phi_evt;
  TH2F *h_refmuoneta_V_phi_evt;
  TH2F *h_muideta_V_phi_evt;
  TH2F *h_cluseta_V_phi_evt;
};

#endif // MISSINGETCOMPOSITION_TOOL_H
