/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_CalibHistoTool.h
 * Header file to interface class for histograms in the SCT_CalibAlgs package
 * @author Shaun Roe
**/

#ifndef ISCT_CalibHistoTool_h
#define ISCT_CalibHistoTool_h

//Gaudi includes
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>
#include <string>
#include <utility>
class TH1F;
class TH2F;
class TH1I;
class ITHistSvc;
class SCT_ID;

class ISCT_CalibHistoTool: virtual public IAlgTool {
   public:
      ISCT_CalibHistoTool();
      /// no-op destructor
      virtual ~ISCT_CalibHistoTool() = default;
      /// interfaceID re-implemented from IInterface
      static const InterfaceID & interfaceID();
      /// book histograms
      virtual bool book()=0;
      /// read histograms
      virtual bool read(const std::string & fileName)=0;
      /// fill histograms from simulation
      virtual bool fill(const bool fromData=false)=0;
      /// fill histograms from data
      virtual bool fillFromData()=0;
      /// get number of events in a specific bin
      virtual int getNumberOfEventsInBin(const int nbin);
      /// get total number of entries in the number of events histo
      int size();
      /// get bin content for bin in specific histogram
      double getBinForHistogramIndex(const int bin, const int histogramIndex);
      /// get the number of entries in a given histogram
      int size(const int histogramIndex);
      void binHistograms(const int nLbMerged);
      /// set number of lumiblocks
      static void setNumberOfLb(const int nLb);
      /// get number of lumiblocks
      static int numberOfLb();
      /// set number of lumiblocks
      static void setLbToMerge(const int nLbMerge);
      /// get number of lumiblocks
      static int LbToMerge();
   protected:
      std::vector<TH1F *> m_phistoVector;
      std::vector<TH2F *> m_phistoVector2D;
      TH1I * m_numberOfEventsHisto;
      ITHistSvc * m_thistSvc;
      //need to implement retrieval for these
      //ServiceHandle<StoreGateSvc> m_detStore;
      //ServiceHandle<StoreGateSvc> m_evtStore;
      const SCT_ID* m_pSCTHelper;
      static int m_nLb;
      static int m_nLbMerge;
      //
      bool init();
      template<class T>
      std::pair<std::string, bool> retrievedTool(T & tool) {
         if (tool.retrieve().isFailure() ) return std::make_pair(std::string("Unable to retrieve ")+tool.name(), false);
         return std::make_pair("",true);
      }
};


inline const InterfaceID & ISCT_CalibHistoTool::interfaceID() {
   static const InterfaceID IID("ISCT_CalibHistoTool",1,0);
   return IID;
}

#endif
