/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TROOT.h"

#include "HistogramException.h"
#include "HistogramFactory.h"

using namespace Monitored;

// this mutex is used to prevent instantiating more than one new histogram at a time, to avoid
// potential name clashes in the gDirectory namespace
// alternative would be to set TH1::AddDirectory but that has potential side effects
namespace {
  static std::mutex s_histDirMutex;
}

HistogramFactory::HistogramFactory(const ServiceHandle<ITHistSvc>& histSvc,
                                   std::string histoPath)
: m_histSvc(histSvc)
{
  size_t split = histoPath.find('/');
  m_streamName = histoPath.substr(0,split);
  m_groupName = split!=std::string::npos ? histoPath.substr(split) : "";
}


TNamed* HistogramFactory::create(const HistogramDef& def) {
  std::scoped_lock lock(m_createLock);
  TNamed* rootObj(0);

  if (def.type == "TH1F") {
    rootObj = create1D<TH1F>(def);
  } else if (def.type == "TH1D") {
    rootObj = create1D<TH1D>(def);
  } else if (def.type == "TH1I") {
    rootObj = create1D<TH1I>(def);
  } else if (def.type == "TH2F") {
    rootObj = create2D<TH2F>(def);
  } else if (def.type == "TH2D") {
    rootObj = create2D<TH2D>(def);
  } else if (def.type == "TH2I") {
    rootObj = create2D<TH2I>(def);
  } else if (def.type == "TProfile") {
    rootObj = create1DProfile<TProfile>(def);
  } else if (def.type == "TProfile2D") {
    rootObj = create2DProfile<TProfile2D>(def);
  } else if (def.type == "TEfficiency") {
    rootObj = createEfficiency(def);
  }
  
  if (rootObj == 0) {
    throw HistogramException("Can not create yet histogram of type: >" + def.type + "<\n" +
                             "Try one of: TH1[F,D,I], TH2[F,D,I], TProfile, TProfile2D, " +
                             "TEfficiency.");
  }

  return rootObj;
}

template<class H> 
TH1* HistogramFactory::create1D(const HistogramDef& def) {
  if ( def.xarray.size()!=0 ) {
    return create<H,TH1>(def, def.xbins, &(def.xarray)[0]);
  } else {
    return create<H,TH1>(def, def.xbins, def.xmin, def.xmax);
  }
}

template<class H> 
TH1* HistogramFactory::create1DProfile(const HistogramDef& def) {
  if (def.xarray.size()!=0) {
    return create<H,TH1>(def, def.xbins, &(def.xarray)[0],
                              def.ymin, def.ymax);
  } else {
    return create<H,TH1>(def, def.xbins, def.xmin, def.xmax, 
                              def.ymin, def.ymax);
  }
}

template<class H> 
TH2* HistogramFactory::create2D(const HistogramDef& def) {
  if (def.xarray.size()!=0 && def.yarray.size()!=0) {
    return create<H,TH2>(def, def.xbins, &(def.xarray)[0],
                              def.ybins, &(def.yarray)[0]);
  } else if (def.yarray.size()!=0) {
    return create<H,TH2>(def, def.xbins, def.xmin, def.xmax,
                              def.ybins, &(def.yarray)[0]);
  } else if (def.xarray.size()!=0) {
    return create<H,TH2>(def, def.xbins, &(def.xarray)[0],
                              def.ybins, def.ymin, def.ymax);
  } else {
    return create<H,TH2>(def, def.xbins, def.xmin, def.xmax,
                              def.ybins, def.ymin, def.ymax);
  }
}

template<class H> 
TH2* HistogramFactory::create2DProfile(const HistogramDef& def) {
  if (def.xarray.size()!=0 && def.yarray.size()!=0) {
    return create<H,TH2>(def, def.xbins, &(def.xarray)[0],
                              def.ybins, &(def.yarray)[0]);
  } else if (def.yarray.size()!=0) {
    return create<H,TH2>(def, def.xbins, def.xmin, def.xmax, 
                              def.ybins, &(def.yarray)[0]);
  } else if (def.xarray.size()!=0) {
    return create<H,TH2>(def, def.xbins, &(def.xarray)[0],
                              def.ybins, def.ymin, def.ymax);
  } else {
    return create<H,TH2>(def, def.xbins, def.xmin, def.xmax, 
                              def.ybins, def.ymin, def.ymax, 
                              def.zmin, def.zmax);
  }
}

TEfficiency* HistogramFactory::createEfficiency(const HistogramDef& def) {    
  std::string fullName = getFullName(def);

  // Check if efficiency exists already
  TEfficiency* e = nullptr;
  if ( m_histSvc->existsEfficiency(fullName) ) {
    if ( !m_histSvc->getEfficiency(fullName,e) ) {
      throw HistogramException("Histogram >"+ fullName + "< seems to exist but can not be obtained from THistSvc");
    }
    return e;
  }

  // Otherwise, create the efficiency and register it
  // Hold global lock until we have detached object from gDirectory
  {
    std::scoped_lock<std::mutex> dirLock(s_histDirMutex);
    e = new TEfficiency(def.alias.c_str(),def.title.c_str(),def.xbins,def.xmin,def.xmax);
    e->SetDirectory(0);
  }
  if ( !m_histSvc->regEfficiency(fullName,e) ) {
    delete e;
    throw HistogramException("Histogram >"+ fullName + "< can not be registered in THistSvc");
  }
  return e;
}

template<class H, class HBASE, typename... Types> 
HBASE* HistogramFactory::create(const HistogramDef& def, Types&&... hargs) {    
  std::string fullName = getFullName(def);
   
  // Check if histogram exists already
  HBASE* histo = nullptr;
  if ( m_histSvc->exists( fullName ) ) {
    if ( !m_histSvc->getHist( fullName, histo ) ) {
      throw HistogramException("Histogram >"+ fullName + "< seems to exist but can not be obtained from THistSvc");
    }    
    return histo;
  }

  // Create the histogram and register it
  // Hold global lock until we have detached object from gDirectory
  H* h = nullptr;
  { 
    std::scoped_lock<std::mutex> dirLock(s_histDirMutex);
    h = new H(def.alias.c_str(), def.title.c_str(), std::forward<Types>(hargs)...);
    h->SetDirectory(0);
  }
  if ( !m_histSvc->regHist( fullName, static_cast<TH1*>(h) ) ) {
    delete h;
    throw HistogramException("Histogram >"+ fullName + "< can not be registered in THistSvc");

  }
  h->GetYaxis()->SetTitleOffset(1.25); // magic shift to make histograms readable even if no post-procesing is done

  setLabels(h, def);
  setOpts(h, def.opt);

  return h;
}

void HistogramFactory::setOpts(TH1* hist, const std::string& opt) {
  // try to apply an option
  const unsigned canExtendPolicy = opt.find("kCanRebin") != std::string::npos ? TH1::kAllAxes : TH1::kNoAxis;
  hist->SetCanExtend(canExtendPolicy);

  // try to apply option to make Sumw2 in histogram
  const bool shouldActivateSumw2 = opt.find("Sumw2") != std::string::npos;
  hist->Sumw2(shouldActivateSumw2);
}

void HistogramFactory::setLabels(TH1* hist, const HistogramDef& def) {
  if ( !def.xlabels.empty() ) {
    int nBinX = hist->GetNbinsX();
    for ( int xbin=0; xbin<nBinX; xbin++ ) {
      hist->GetXaxis()->SetBinLabel(xbin+1, def.xlabels[xbin].c_str());
    }
  }

  if ( !def.ylabels.empty() ) {
    int nBinY = hist->GetNbinsY();
    for ( int ybin=0; ybin<nBinY; ybin++ ) {
      hist->GetYaxis()->SetBinLabel(ybin+1, def.ylabels[ybin].c_str());
    }
  }

  if ( !def.zlabels.empty() ) {
    int nBinZ = hist->GetNbinsZ();
    for ( int zbin=0; zbin<nBinZ; zbin++ ) {
      hist->GetZaxis()->SetBinLabel(zbin+1, def.zlabels[zbin].c_str());
    }
  }  
}

std::string HistogramFactory::getFullName(const HistogramDef& def) const {
  const static std::set<std::string> online( { "EXPERT", "SHIFT", "DEBUG", "RUNSTAT", "EXPRESS" } );
  
  std::string path;
  if ( online.count( def.path)!=0 ) {
    path =  "/" + def.path + "/" + m_streamName + "/" + m_groupName;
  } else if ( def.path=="DEFAULT" ) {
    path = "/" + m_streamName + "/" + m_groupName;
  } else {
    path = "/" + m_streamName + "/" + def.tld + "/" + m_groupName + "/" + def.path;
  }

  // remove duplicate slashes
  std::string fullName = path + "/" + def.alias;
  fullName.erase( std::unique( fullName.begin(), fullName.end(), 
    [](const char a, const char b) { 
      return a == b and a == '/';
    } ), fullName.end() );

  return fullName;
}
