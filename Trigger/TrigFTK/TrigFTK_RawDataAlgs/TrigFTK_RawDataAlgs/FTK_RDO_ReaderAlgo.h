/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_RDO_ReaderAlgo_h
#define FTK_RDO_ReaderAlgo_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include "FTK_DataProviderInterfaces/IFTK_DataProviderSvc.h"

#include "TrigFTK_RawData/FTK_RawTrack.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"

#include <vector>
#include <string>
#include <map>

#include "TH1D.h"

/////////////////////////////////////////////////////////////////////////////
class FTK_RDO_ReaderAlgo: public AthAlgorithm {
public:
  FTK_RDO_ReaderAlgo (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FTK_RDO_ReaderAlgo ();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  /// Tools and services ///
  ITHistSvc*    rootHistSvc;
  StoreGateSvc* m_StoreGate;


  /// Track collections ///
  std::string m_ftk_raw_trackcollection_Name;
  FTK_RawTrackContainer *m_ftk_raw_trackcollection;

  //IFTK_DataProviderSvc* m_DataProviderSvc;
  ServiceHandle<IFTK_DataProviderSvc> m_DataProviderSvc;

  bool m_fillHists;

  /// Histograms ///
  TH1D* h_FTK_RawTrack_n;
  TH1D* h_FTK_RawTrack_phi;
  TH1D* h_FTK_RawTrack_d0;
  TH1D* h_FTK_RawTrack_z0;
  TH1D* h_FTK_RawTrack_curv;
  TH1D* h_FTK_RawTrack_cot;
  TH1D* h_FTK_RawTrack_eta;
  TH1D* h_FTK_RawTrack_nPix;
  TH1D* h_FTK_RawTrack_nSCT;

  TH1D* h_Track_n;
  TH1D* h_Track_phi;
  TH1D* h_Track_d0;
  TH1D* h_Track_z0;
  TH1D* h_Track_curv;
  TH1D* h_Track_cot;
  TH1D* h_Track_eta;
  TH1D* h_Track_nPix;
  TH1D* h_Track_nSCT;

  TH1D* h_refitTrack_n;
  TH1D* h_refitTrack_phi;
  TH1D* h_refitTrack_d0;
  TH1D* h_refitTrack_z0;
  TH1D* h_refitTrack_curv;
  TH1D* h_refitTrack_cot;
  TH1D* h_refitTrack_eta;
  TH1D* h_refitTrack_nPix;
  TH1D* h_refitTrack_nSCT;

};

#endif // FTK_RDO_ReaderAlgo_h
