
#include <map>
#include <numeric>
#include <utility>         // Temporary

#include "TrigT1CaloUtils/ClusterProcessorModuleKey.h"
#include "TrigT1CaloUtils/CPAlgorithm.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloEvent/CMXCPHits.h"
#include "TrigT1CaloEvent/CMXCPTob.h"
#include "TrigT1CaloEvent/CPMTobRoI.h"
#include "TrigT1CaloEvent/EmTauROI.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/CPRoIDecoder.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
//#include "TrigConfigSvc/ITrigConfigSvc.h"    //@@vkousk follow the recommendations
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"
#include "TrigT1CaloTools/L1CPCMXTools.h"

namespace LVL1 {

/** Constructor */

L1CPCMXTools::L1CPCMXTools(const std::string& type,
                           const std::string& name,
                           const IInterface*  parent)
  :
  AthAlgTool(type, name, parent),
  m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
  m_crates(4), m_modules(14), m_maxTobs(5), m_sysCrate(3),
  m_debug(false)
{
  declareInterface<IL1CPCMXTools>(this);
  
  declareProperty( "TrigConfigSvc", m_configSvc, "Trigger Config Service");
}

/** Destructor */

L1CPCMXTools::~L1CPCMXTools()
{       
}


/** Initialisation */

StatusCode L1CPCMXTools::initialize()
{
  m_debug = msgLvl(MSG::DEBUG);
  
  // Connect to the TrigConfigSvc for the trigger configuration:
 
  StatusCode sc = m_configSvc.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Couldn't connect to " << m_configSvc.typeAndName()
                    << endreq;
    return sc;
  } else if (m_debug) {
    msg(MSG::DEBUG) << "Connected to " << m_configSvc.typeAndName()
                    << endreq;
  }
  
  msg(MSG::INFO) << "Initialization completed" << endreq;
  
  return sc;
}

/** Finalisation */

StatusCode L1CPCMXTools::finalize()
{
  return StatusCode::SUCCESS;
}

/** CPAlgorithm to CPMTobRoI conversion */

void L1CPCMXTools::formCPMTobRoI(const DataVector<CPAlgorithm>* cpAlgorithmVec,
                                       DataVector<CPMTobRoI>*   cpmRoiVec) const
{
  std::set<uint32_t> sorted;
  DataVector<CPAlgorithm>::const_iterator pos  = cpAlgorithmVec->begin();
  DataVector<CPAlgorithm>::const_iterator pose = cpAlgorithmVec->end();
  for (; pos != pose; ++pos) {
    //CPMTobRoI* roi = new CPMTobRoI((*pos)->RoIWord());
    //cpmRoiVec->push_back(roi);
    const EmTauROI* etRoi = (*pos)->produceExternal();
    const std::pair<uint32_t,uint32_t> words = roiWord(etRoi);
    if (words.first)  sorted.insert(words.first);
    if (words.second) sorted.insert(words.second);
  }
  std::set<uint32_t>::const_iterator siter  = sorted.begin();
  std::set<uint32_t>::const_iterator siterE = sorted.end();
  for (; siter != siterE; ++siter) {
    CPMTobRoI* roi = new CPMTobRoI(*siter);
    cpmRoiVec->push_back(roi);
  }
}

/** EmTauROI to CPMTobRoI conversion */

void L1CPCMXTools::formCPMTobRoI(const DataVector<EmTauROI>* emTauRoiVec,
                                    DataVector<CPMTobRoI>*   cpmRoiVec) const
{
  std::set<uint32_t> sorted;
  DataVector<EmTauROI>::const_iterator pos  = emTauRoiVec->begin();
  DataVector<EmTauROI>::const_iterator pose = emTauRoiVec->end();
  for (; pos != pose; ++pos) {
    //CPMTobRoI* roi = new CPMTobRoI((*pos)->roiWord());
    //cpmRoiVec->push_back(roi);
    const std::pair<uint32_t,uint32_t> words = roiWord(*pos);
    if (words.first)  sorted.insert(words.first);
    if (words.second) sorted.insert(words.second);
  }
  std::set<uint32_t>::const_iterator siter  = sorted.begin();
  std::set<uint32_t>::const_iterator siterE = sorted.end();
  for (; siter != siterE; ++siter) {
    CPMTobRoI* roi = new CPMTobRoI(*siter);
    cpmRoiVec->push_back(roi);
  }
}

/** Temporary for testing until CPAlgorithm and EmTauROI are updated */

std::pair<uint32_t,uint32_t> L1CPCMXTools::roiWord(const EmTauROI* roi) const
{
  const uint32_t oldRoi = roi->roiWord();
  CPRoIDecoder decoder;
  const int crate = decoder.crate(oldRoi);
  const int module = decoder.module(oldRoi);
  const int chip = decoder.chip(oldRoi);
  const int coord = decoder.localcoord(oldRoi);
  bool em = false;
  bool tau = false;
  for (unsigned int thresh = 1; thresh <= TrigT1CaloDefs::numOfCPThresholds; ++thresh) {
    if (roi->thresholdPassed(thresh)) {
      if (roi->thresholdType(thresh) == TrigT1CaloDefs::EMAlg) em  = true;
      else                                                     tau = true;
    }
  }
  uint32_t emRoi = 0;
  uint32_t tauRoi = 0;
  if (em) {
    int isolation = isolationEm(roi->clusterEnergy(),
                                roi->emRingIsolationEnergy(),
				roi->hadRingIsolationEnergy(),
				roi->hadCoreEnergy());
    const CPMTobRoI newRoi(crate, module, chip, coord, 0, roi->clusterEnergy(),
                           isolation);
    emRoi = newRoi.roiWord();
  }
  if (tau) {
    int isolation = isolationTau(roi->tauClusterEnergy(),
                                 roi->emRingIsolationEnergy(),
				 roi->hadRingIsolationEnergy());
    const CPMTobRoI newRoi(crate, module, chip, coord, 1, roi->tauClusterEnergy(),
                           isolation);
    tauRoi = newRoi.roiWord();
  }
  return std::make_pair(emRoi, tauRoi);
}

int L1CPCMXTools::isolationEm(unsigned int /*clusterEnergy*/, unsigned int /*emIsol*/,
                              unsigned int /*hadIsol*/, unsigned int hadVeto) const
{
  int isol = 0;
  if (hadVeto > 1) isol = 1;
  return isol;
}

int L1CPCMXTools::isolationTau(unsigned int /*clusterEnergy*/, unsigned int emIsol,
                               unsigned int /*hadIsol*/) const
{
  int isol = 0;
  if (emIsol > 4) isol = 1;
  return isol;
}

void L1CPCMXTools::unpackEmIsol(int /*energy*/, int isol, unsigned int& emIsol,
                                unsigned int& hadIsol, unsigned int& hadVeto) const
{
  emIsol = 0;
  hadIsol = 0;
  hadVeto = (isol == 1) ? 2 : 0;
}

void L1CPCMXTools::unpackTauIsol(int /*energy*/, int isol, unsigned int& emIsol,
                                 unsigned int& hadIsol) const
{
  emIsol = (isol == 1) ? 5 : 0;
  hadIsol = 0;
}

/** form CMX-CP TOBs from RoIs - single slice */

void L1CPCMXTools::formCMXCPTob(const DataVector<CPMTobRoI>* cpmRoiVec,
                                      DataVector<CMXCPTob>*  cmxTobVec) const
{
  std::vector<const DataVector<CPMTobRoI>*> cpmRoiColls(1, cpmRoiVec);
  formCMXCPTob(cpmRoiColls, cmxTobVec, 0);
}

/** form CMX-CP TOBs from RoIs - multiple slices */

void L1CPCMXTools::formCMXCPTob(
                    const std::vector<const DataVector<CPMTobRoI>*>& cpmRoiColls,
		    DataVector<CMXCPTob>* cmxTobVec, int peak) const
{
  std::map<uint32_t, const CPMTobRoI*> cpmRoiMap;
  std::map<int, CMXCPTob*>    cmxTobMap;
  DataVector<CPMTobRoI>::const_iterator it;
  int timeslices = cpmRoiColls.size();
  for (int slice = 0; slice < timeslices; ++slice) {
    const DataVector<CPMTobRoI>* cpmRoiVec = cpmRoiColls[slice];
    cpmRoiMap.clear();
    std::vector<unsigned int> presenceMaps(2*m_crates*m_modules);
    std::vector<int>          tobCount(2*m_crates*m_modules);
    DataVector<CPMTobRoI>::const_iterator it  = cpmRoiVec->begin();
    DataVector<CPMTobRoI>::const_iterator itE = cpmRoiVec->end();
    for (; it != itE; ++it) { // get sorted list
      const CPMTobRoI* roi = *it;
      const int type = roi->type();
      const int crate = roi->crate();
      const int cpm = roi->cpm();
      const int index = ((type*m_crates) + crate)*m_modules + cpm - 1;
      const int presenceBit = (roi->chip()<<1)|((roi->location()>>2)&0x1);      // <<== CHECK
      presenceMaps[index] |= (1<<presenceBit);
      tobCount[index]++;
      uint32_t key = roi->roiWord();
      cpmRoiMap.insert(std::make_pair(key, roi));
    }
    std::map<uint32_t, const CPMTobRoI*>::const_iterator mit  = cpmRoiMap.begin();
    std::map<uint32_t, const CPMTobRoI*>::const_iterator mitE = cpmRoiMap.end();
    for (; mit != mitE; ++mit) {
      const CPMTobRoI* roi = mit->second;
      const int type = roi->type();
      const int crate = roi->crate();
      const int cmx = 1 - type;                                                 // <<== CHECK
      const int cpm = roi->cpm();
      const int chip = (roi->chip()<<1)|((roi->location()>>2)&0x1);
      const int loc = roi->location()&0x3;
      const int index = ((type*m_crates) + crate)*m_modules + cpm - 1;
      const int energy = roi->energy();
      const int isolation = roi->isolation();
      const unsigned int presence = presenceMaps[index];
      int error = 0;
      if (tobCount[index]>m_maxTobs) { // overflow
	int count = 0;
        for (int bit = 0; bit <= chip; ++bit) count += (presence>>bit)&0x1;
	if (count > m_maxTobs) continue;
        LVL1::DataError err;
	err.set(LVL1::DataError::Overflow);
	error = err.error();
      }
      const int key = (((((((crate<<1)|cmx)<<4)|cpm)<<4)|chip)<<2)|loc;
      CMXCPTob* tob = 0;
      std::map<int, CMXCPTob*>::iterator xit = cmxTobMap.find(key);
      if (xit == cmxTobMap.end()) {
        tob = new CMXCPTob(crate, cmx, cpm, chip, loc);
	if (timeslices > 1) {
	  std::vector<int> vecI(timeslices);
	  std::vector<unsigned int> vecU(timeslices);
	  tob->addTob(vecI, vecI, vecI, vecU);
	  tob->setPeak(peak);
        }
	cmxTobMap.insert(std::make_pair(key, tob));
	cmxTobVec->push_back(tob);
      } else tob = xit->second;
      std::vector<int> energyVec(tob->energyVec());
      std::vector<int> isolationVec(tob->isolationVec());
      std::vector<int> errorVec(tob->errorVec());
      std::vector<unsigned int> presenceMapVec(tob->presenceMapVec());
      energyVec[slice] = energy;
      isolationVec[slice] = isolation;
      errorVec[slice] = error;
      presenceMapVec[slice] = presence;
      tob->addTob(energyVec, isolationVec, errorVec, presenceMapVec);
    }
  }
}

/** form complete CMX-CP hits from CMX-CP TOBs */

void L1CPCMXTools::formCMXCPHits(const DataVector<CMXCPTob>* cmxTobVec,
                                 DataVector<CMXCPHits>* cmxHitsVec) const
{
  DataVector<CMXCPHits>* cmxHitsCrate = new DataVector<CMXCPHits>;
  DataVector<CMXCPHits>* cmxHitsSys   = new DataVector<CMXCPHits>;
  DataVector<CMXCPHits>* cmxHitsTopo  = new DataVector<CMXCPHits>;
  formCMXCPHitsCrate(cmxTobVec, cmxHitsCrate);
  formCMXCPHitsSystem(cmxHitsCrate, cmxHitsSys);
  formCMXCPHitsTopo(cmxTobVec, cmxHitsTopo);
  mergeCMXCPHits(cmxHitsVec, cmxHitsCrate);
  mergeCMXCPHits(cmxHitsVec, cmxHitsSys);
  mergeCMXCPHits(cmxHitsVec, cmxHitsTopo);
  delete cmxHitsCrate;
  delete cmxHitsSys;
  delete cmxHitsTopo;
}

/** form partial CMX-CP hits (crate) from CMX-CP TOBs */

void L1CPCMXTools::formCMXCPHitsCrate(const DataVector<CMXCPTob>* cmxTobVec,
                                      DataVector<CMXCPHits>* cmxHitsCrate) const
{
  int peakm = 0;
  std::vector<HitsVector> hitVec(4*m_crates);
  std::vector<ErrorVector> errVec(4*m_crates); // Need overflow for neutral format
  HitsVector hit0;
  HitsVector hit1;
  DataVector<CMXCPTob>::const_iterator pos  = cmxTobVec->begin();
  DataVector<CMXCPTob>::const_iterator pose = cmxTobVec->end();
  for (; pos != pose; ++pos) {
    const CMXCPTob* tob = *pos;
    const int crate = tob->crate();
    const int cmx = tob->cmx();
    const int index = (crate*2 + cmx)*2;
    const std::vector<int>& error(tob->errorVec());
    hit0.clear();
    hit1.clear();
    getHits(tob, hit0, hit1);
    addCMXCPHits(hitVec[index],   hit0);
    addCMXCPHits(hitVec[index+1], hit1);
    addOverflow(errVec[index],   error);
    addOverflow(errVec[index+1], error);
    const int peak = tob->peak();
    if (peak > peakm) peakm = peak;
  }
  // Save non-zero crate totals
  for (int crate = 0; crate < m_crates; ++crate) {
    for (int cmx = 0; cmx < 2; ++cmx) {
      const int index = (crate*2 + cmx)*2;
      saveCMXCPHits(cmxHitsCrate, hitVec[index], hitVec[index+1],
                    errVec[index], errVec[index+1],
                    crate, cmx, CMXCPHits::LOCAL, peakm);
      if (crate != m_sysCrate) { // REMOTE totals
        const int source = crate;
        saveCMXCPHits(cmxHitsCrate, hitVec[index], hitVec[index+1],
                      errVec[index], errVec[index+1],
	              m_sysCrate, cmx, source, peakm);
      }
    }
  }
}

/** Temporary for testing, mostly lifted from CPAlgorithm */

void L1CPCMXTools::getHits(const CMXCPTob* tob,
			   HitsVector& hit0, HitsVector& hit1) const
{
  using namespace TrigConf;
  const std::vector<int>& energy(tob->energyVec());
  const std::vector<int>& isolation(tob->isolationVec());
  const int type = 1 - tob->cmx();
  const int timeslices = energy.size();
  hit0.assign(timeslices, 0);
  hit1.assign(timeslices, 0);
  // Get thresholds from menu
  L1DataDef def;
  const std::string thrType = (type == 0) ? def.emType() : def.tauType();
  //const std::vector<TriggerThreshold*>& thresholds(m_configSvc->ctpConfig()->menu()->thresholdVector()); // vkousk@@
  const std::vector<TriggerThreshold*>& thresholds(m_configSvc->ctpConfig()->menu().thresholdVector());
  // Get eta/phi using old decoder
  CPRoIDecoder decoder;
  uint32_t oldRoiWord = ((((((tob->crate()<<4)|tob->cpm())<<4)|tob->chip())<<2)|tob->location())<<18;
  CoordinateRange coord = decoder.coordinate(oldRoiWord);
  const double eta = coord.eta() - 0.05;
  const double phi = coord.phi() - M_PI/64.;
  //msg() << MSG::INFO << "roiWord: " << MSG::hex << oldRoiWord << MSG::dec << " ";
  //msg() << MSG::INFO << "eta/phi: " << eta << "/" << phi << endreq;
  /** eta bins run from -49 -> 49
     "reference tower" coordinate should ensure correct RoI->module association.
      Firmware upgrade will allow variation of threshold values at full eta granularity,
      but phi granularity is still that of the module*/
  int ieta = int(eta/0.1) + ((eta>0) ? 0 : -1);
  int iphi = int(phi*32/M_PI);
  // quantise by module
  //if (ieta > 0) ieta = 4*(ieta/4) + 2;
  //else          ieta = 4*((ieta-3)/4) + 2;
  iphi = 16*(iphi/16) + 8;
  //msg() << MSG::INFO << "ieta/iphi: " << ieta << "/" << iphi << endreq;
  std::vector<TriggerThreshold*>::const_iterator it;
  std::vector<TriggerThreshold*>::const_iterator itE = thresholds.end();
  for (int slice = 0; slice < timeslices; ++slice) {
    if (energy[slice] == 0) continue;
    unsigned int emIsol = 0;
    unsigned int hadIsol = 0;
    unsigned int hadVeto = 0;
    if (type == 0) {  // Em
      unpackEmIsol(energy[slice], isolation[slice], emIsol, hadIsol, hadVeto);
    } else {          // Tau
      unpackTauIsol(energy[slice], isolation[slice], emIsol, hadIsol);
    }
    for (it = thresholds.begin(); it != itE; ++it) {

      if ( (*it)->type() == thrType ) {
        /** get threshold values from TriggerMenu */
	int clusterThreshold = 999;      // Set impossible default in case no threshold found
	unsigned int emRingIsolationThreshold = 0;
	unsigned int hadRingIsolationThreshold = 0;
	unsigned int hadCoreIsolationThreshold = 0;

	TriggerThresholdValue* tv = (*it)->triggerThresholdValue(ieta,iphi);

	// cluster/isolation ET in counts, not GeV, so get values in same units
	if (tv != 0) {
	  ClusterThresholdValue* ctv;
	  ctv = dynamic_cast<ClusterThresholdValue*> (tv);
	  if (ctv) {
	    clusterThreshold = ctv->thresholdValueCount();
	    emRingIsolationThreshold = ctv->emIsolationCount();
	    hadRingIsolationThreshold = ctv->hadIsolationCount();
	    hadCoreIsolationThreshold = ctv->hadVetoCount();
	  }
	}

        /** Get threshold number and check validity */
        int numThresh = (*it)->thresholdNumber();
        if (numThresh >= 0 && numThresh < (int)TrigT1CaloDefs::numOfCPThresholds) {

          bool passes = false;
          if (type == 0) {
	    if ( (energy[slice]  > clusterThreshold)
	      && (emIsol  <= emRingIsolationThreshold)
	      && (hadIsol <= hadRingIsolationThreshold)
	      && (hadVeto <= hadCoreIsolationThreshold) )
	      passes=true;
          } else {
	    if ( (energy[slice]  > clusterThreshold)
	      && (emIsol  <= emRingIsolationThreshold)
	      && (hadIsol <= hadRingIsolationThreshold) )
	      passes=true;
          }
 
          /** Set bit if passed */
          if (passes) {
	    if (numThresh < 8) hit0[slice] |= (1<<(numThresh*3));
	    else               hit1[slice] |= (1<<((numThresh-8)*3));
          }
        }
      }
    }
  }
}

void L1CPCMXTools::addOverflow(ErrorVector& hitErr, const ErrorVector& tobErr) const
{
  const int timeslices = tobErr.size();
  hitErr.resize(timeslices);
  for (int slice = 0; slice < timeslices; ++slice) {
    int error = tobErr[slice];
    if (error) {
      LVL1::DataError err(error);
      int overflow = err.get(LVL1::DataError::Overflow);
      LVL1::DataError err2;
      err2.set(LVL1::DataError::Overflow, overflow);
      hitErr[slice] |= err2.error();
    }
  }
}

/** form partial CMX-CP hits (system) from crate CMX-CP hits */

void L1CPCMXTools::formCMXCPHitsSystem(
                                const DataVector<CMXCPHits>* cmxHitsCrate,
				DataVector<CMXCPHits>* cmxHitsSys) const
{
  int peakm = 0;
  std::vector<HitsVector> systemHit0(2);
  std::vector<HitsVector> systemHit1(2);
  std::vector<ErrorVector> systemErr0(2);
  std::vector<ErrorVector> systemErr1(2);
  DataVector<CMXCPHits>::const_iterator pos  = cmxHitsCrate->begin();
  DataVector<CMXCPHits>::const_iterator pose = cmxHitsCrate->end();
  for (; pos != pose; ++pos) {
    const CMXCPHits* hits = *pos;
    if (hits->crate() != m_sysCrate) continue;
    const int source = hits->source();
    if (source != CMXCPHits::LOCAL    &&
        source != CMXCPHits::REMOTE_0 &&
        source != CMXCPHits::REMOTE_1 &&
	source != CMXCPHits::REMOTE_2) continue;
    const int peak   = hits->peak();
    if (peak > peakm) peakm = peak;
    HitsVector hits0(hits->hitsVec0());
    HitsVector hits1(hits->hitsVec1());
    ErrorVector err0(hits->errorVec0());
    ErrorVector err1(hits->errorVec1());
    const int cmx = hits->cmx();
    addCMXCPHits(systemHit0[cmx], hits0);
    addCMXCPHits(systemHit1[cmx], hits1);
    addOverflow(systemErr0[cmx], err0);
    addOverflow(systemErr1[cmx], err1);
  }
  // Save non-zero system totals
  for (int cmx = 0; cmx < 2; ++cmx) {
    saveCMXCPHits(cmxHitsSys, systemHit0[cmx], systemHit1[cmx],
                              systemErr0[cmx], systemErr1[cmx],
			      m_sysCrate, cmx, CMXCPHits::TOTAL, peakm);
  }
}

/** form partial CMX-CP hits (topo) from CMX-CP TOBs */                   // Temporary for testing

void L1CPCMXTools::formCMXCPHitsTopo(const DataVector<CMXCPTob>* cmxTobVec,
                                     DataVector<CMXCPHits>* cmxHitsTopo) const
{
  int peakm = 0;
  int timeslices = 0;
  std::vector<HitsVector> hitVec(8*m_crates);
  DataVector<CMXCPTob>::const_iterator pos  = cmxTobVec->begin();
  DataVector<CMXCPTob>::const_iterator pose = cmxTobVec->end();
  for (; pos != pose; ++pos) {
    const CMXCPTob* tob = *pos;
    const int crate = tob->crate();
    const int cmx = tob->cmx();
    const int cpm = tob->cpm();
    const int chip = tob->chip()>>1;
    const int loc = ((tob->chip()&0x1)<<2)|tob->location();
    const int index = (crate*2 + cmx)*4;
    const std::vector<int>& energy(tob->energyVec());
    const std::vector<int>& isolation(tob->isolationVec());
    const std::vector<int>& error(tob->errorVec());
    timeslices = energy.size();
    HitsVector& checksum(hitVec[index]);
    HitsVector& map(hitVec[index+1]);
    HitsVector& countsLow(hitVec[index+2]);
    HitsVector& countsHigh(hitVec[index+3]);
    checksum.resize(timeslices);
    map.resize(timeslices);
    countsLow.resize(timeslices);
    countsHigh.resize(timeslices);
    for (int slice = 0; slice < timeslices; ++slice) {
      if (energy[slice] == 0) continue;
      // checksum
      LVL1::DataError err(error[slice]);
      int overflow = err.get(LVL1::DataError::Overflow);
      checksum[slice] += cpm + chip + loc + energy[slice]
                             + isolation[slice] + overflow;
      checksum[slice] &= 0xffff;
      // occupancy map
      map[slice] |= (1<<(cpm-1));
      // occupancy counts
      if (cpm <= 7) {
        countsLow[slice] += (1<<(3*(cpm-1))); // can't saturate
      } else {
        countsHigh[slice] += (1<<(3*(cpm-8)));
      }
    }
    const int peak = tob->peak();
    if (peak > peakm) peakm = peak;
  }
  // Save non-zero crate totals
  HitsVector dummy(timeslices);
  std::vector<int> dummyE(timeslices);
  for (int crate = 0; crate < m_crates; ++crate) {
    for (int cmx = 0; cmx < 2; ++cmx) {
      const int index = (crate*2 + cmx)*4;
      saveCMXCPHits(cmxHitsTopo, hitVec[index], dummy,
		    dummyE, dummyE,
                    crate, cmx, CMXCPHits::TOPO_CHECKSUM, peakm);
      saveCMXCPHits(cmxHitsTopo, hitVec[index+1], dummy,
		    dummyE, dummyE,
                    crate, cmx, CMXCPHits::TOPO_OCCUPANCY_MAP, peakm);
      saveCMXCPHits(cmxHitsTopo, hitVec[index+2], hitVec[index+3],
		    dummyE, dummyE,
                    crate, cmx, CMXCPHits::TOPO_OCCUPANCY_COUNTS, peakm);
    }
  }
}

/** Add hits from second vector to first */

void L1CPCMXTools::addCMXCPHits(HitsVector& vec1, const HitsVector& vec2) const
{
  int size1 = vec1.size();
  int size2 = vec2.size();
  if (size1 < size2) vec1.resize(size2);
  HitsVector::iterator pos1  = vec1.begin();
  HitsVector::iterator pose1 = vec1.end();
  HitsVector::const_iterator pos2  = vec2.begin();
  HitsVector::const_iterator pose2 = vec2.end();
  for (; pos1 != pose1 && pos2 != pose2; ++pos1, ++pos2) {
    *pos1 = addHits(*pos1, *pos2, 24, 24);
  }
}

/** Increment CPM/CMX hit word */

unsigned int L1CPCMXTools::addHits(unsigned int hitMult,
                                   unsigned int hitVec,
			           int multBits, int vecBits) const
{
  if (m_debug) 
    msg() << MSG::DEBUG <<"addHits: Original hitMult = " << std::hex << hitMult
          << ". Add hitWord = " << hitVec << std::dec<<endreq;
  
  // Results transmitted in 2 words, each reporting half of the CP thresholds
  int nthresh = TrigT1CaloDefs::numOfCPThresholds/2;
  
  int nbitsOut = multBits/nthresh;
  int nbitsIn  = vecBits/nthresh;
  
  if (m_debug) msg() << MSG::DEBUG <<" Bits per threshold = " << nbitsOut
                     <<endreq;
  
  int max = (1<<nbitsOut) - 1;
  unsigned int multMask = max;
  unsigned int hitMask = (1<<nbitsIn) - 1;
  unsigned int shift = 0;
  
  unsigned int hits = 0;
    
  for (int i = 0; i < nthresh; i++) {
    int mult = (hitMult&multMask) + (hitVec&hitMask);
    mult = ( (mult<=max) ? mult : max);
    hits += (mult<<shift);
    
    hitMult >>= nbitsOut;
    hitVec  >>= nbitsIn;
    shift += nbitsOut;
  }
  
  if (m_debug) msg() << MSG::DEBUG <<"addHits returning hitMult = "
                     << std::hex << hits << std::dec << endreq;
  
  return hits;
}

/** Merge CMX-CP hits vectors */

void L1CPCMXTools::mergeCMXCPHits(DataVector<CMXCPHits>* cmxHitsVec1,
                                  DataVector<CMXCPHits>* cmxHitsVec2) const
{
  int size = cmxHitsVec2->size();
  for (int index = 0; index < size; ++index) {
    CMXCPHits* hitsIn  = 0;
    CMXCPHits* hitsOut = 0;
    cmxHitsVec2->swapElement(index, hitsIn, hitsOut);
    cmxHitsVec1->push_back(hitsOut);
  }
  cmxHitsVec2->clear();
}

/** Save non-zero CMX-CP hits */

void L1CPCMXTools::saveCMXCPHits(DataVector<CMXCPHits>* cmxHitsVec,
                                 const HitsVector& hits0,
				 const HitsVector& hits1,
				 const ErrorVector& err0,
				 const ErrorVector& err1,
				 int crate, int cmx, int source, int peak) const
{
  if (std::accumulate(hits0.begin(), hits0.end(), 0) ||
      std::accumulate(hits1.begin(), hits1.end(), 0)) {
    cmxHitsVec->push_back(new CMXCPHits(crate, cmx, source, hits0, hits1,
                                                       err0, err1, peak));
  }
}

} // end of namespace
