/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetTagger.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro AT cern.ch
// 
// ************************************************

#include "TrigBjetHypo/TrigBjetTagger.h"
#include "TrigBjetHypo/TrigBjetDataHelper.h"
#include "TaggerHelper.h"

#ifndef VALIDATION_TOOL
#include "TrigBjetHypo/TrigBjetFex.h"
#else
#include "Root/TrigBjetFexStandalone.h"
#include "Root/MsgStream.h"
#endif


//** ----------------------------------------------------------------------------------------------------------------- **//

#ifndef VALIDATION_TOOL

TrigBjetTagger::TrigBjetTagger(TrigBjetFex* trigBjetFex, MsgStream& log, unsigned int logLvl) :
  m_trigBjetFex(trigBjetFex),
  m_log(log),
  m_logLvl(logLvl)
{
  
  m_taggerHelper = new TaggerHelper(log, logLvl);

}

#else

TrigBjetTagger::TrigBjetTagger(TrigBjetFexStandalone* trigBjetFex, MsgStream& log, unsigned int logLvl, bool negativeWeights) :
  m_trigBjetFex(trigBjetFex),
  m_log(log),
  m_logLvl(logLvl)
{
  
  m_taggerHelper = new TaggerHelper(log, logLvl, negativeWeights);

}

#endif

//** ----------------------------------------------------------------------------------------------------------------- **//


TrigBjetTagger::~TrigBjetTagger() {

  if (m_taggerHelper) delete m_taggerHelper;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigBjetTagger::getIP1DErr(float param, float errParam) {

  float sd0=errParam;
  
  if (m_trigBjetFex->m_instance == "L2" && m_trigBjetFex->m_algo == 1) {
    float eta = fabs(param);
    float sd0 = 0.173 + 8.43e-3*std::pow(eta,4);

    return sd0;
  }

  return sd0;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigBjetTagger::getIP2DErr(float param, float errParam ) {

  float sd0=errParam;

  if (m_trigBjetFex->m_instance == "L2" && m_trigBjetFex->m_algo == 1) {
    float pt = fabs(param);
    float p0 = 0.023, p1=27, p2=1.48;
    float sd014 = sqrt(p0*p0 + std::pow((float)(p1/14000.),p2));   
      
    sd0 = sqrt(p0*p0 + std::pow((float)(p1/pt),p2));
	
    if (pt>14000)
      sd0 = sd014;
  
    return sd0*1.5;
  }
  
  return sd0;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigBjetTagger::getWeights() {

  m_taggersXMap["IP1D"] = -50; m_taggersXMap["IP2D"] = -50; m_taggersXMap["IP3D"] = -50; m_taggersXMap["CHI2"] = -50;
  m_taggersXMap["MVTX"] = -50; m_taggersXMap["EVTX"] = -50; m_taggersXMap["NVTX"] = -50, m_taggersXMap["SVTX"] = -50;
  m_taggersXMap["COMB"] = -50;
  return;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigBjetTagger::getWeights(std::vector<TrigBjetTrackInfo>*& m_trigBjetTrackInfoVector,
				TrigBjetPrmVtxInfo*&  m_trigBjetPrmVtxInfo,
				TrigBjetSecVtxInfo*&  m_trigBjetSecVtxInfo, 
				TrigBjetJetInfo*&     m_trigBjetJetInfo) {

  if (m_logLvl <= MSG::DEBUG)
    m_log << MSG::DEBUG << "Executing TrigBjetTagger::getWeights" << endreq;

  m_taggersWMap["IP1D"]= 1; m_taggersWMap["IP2D"]= 1; m_taggersWMap["IP3D"]= 1; m_taggersWMap["CHI2"]= 1;
  m_taggersXMap["IP1D"]=-48; m_taggersXMap["IP2D"]=-48; m_taggersXMap["IP3D"]=-48; m_taggersXMap["CHI2"]=-48;

#ifdef VALIDATION_TOOL
  m_vectorIP1D.clear();
  m_vectorIP2D.clear();
  m_vectorIP1D_lowSiHits.clear();
  m_vectorIP2D_lowSiHits.clear();
#endif

  float m_sigmaBeamSpot = (m_trigBjetPrmVtxInfo->xBeamSpotWidth()+m_trigBjetPrmVtxInfo->yBeamSpotWidth())/2;

  ///////////////////////////////////////
  // likelihood-based track IP taggers //
  ///////////////////////////////////////

  std::vector<TrigBjetTrackInfo>::const_iterator pTrack    = m_trigBjetTrackInfoVector->begin();
  std::vector<TrigBjetTrackInfo>::const_iterator lastTrack = m_trigBjetTrackInfoVector->end();

  std::vector<std::string>::iterator pTagger = m_trigBjetFex->m_taggers.begin();
  std::vector<std::string>::iterator lastTagger = m_trigBjetFex->m_taggers.end();

  for (unsigned int j=0; pTrack != lastTrack; j++, pTrack++) {

    m_trigBjetFex->m_mon_trk_a0_sel.push_back((*pTrack).d0());
    m_trigBjetFex->m_mon_trk_z0_sel.push_back((*pTrack).z0());
    m_trigBjetFex->m_mon_trk_z0_sel_PV.push_back((*pTrack).z0()-m_trigBjetPrmVtxInfo->zPrmVtx());

    float w=1;

    float m_IP1D=0, errIP1D=0; 
    float m_IP2D=0, errIP2D=0;
    float z0=0, z0Sign=0, d0Sign=0;
    
    if (m_trigBjetFex->m_useErrIPParam) {
      errIP1D = getIP1DErr((*pTrack).eta(), (*pTrack).ez0());
      errIP2D = getIP2DErr((*pTrack).pT(),  (*pTrack).ed0());
    } else {
      errIP1D = (*pTrack).ez0();
      errIP2D = (*pTrack).ed0();
    }

    z0 = (*pTrack).z0Corr() - m_trigBjetPrmVtxInfo->zPrmVtx();

    if (m_trigBjetFex->m_useJetDirection == 1) {
      d0Sign = m_taggerHelper->signedD0((*pTrack).d0Corr(), (*pTrack).phi(), m_trigBjetJetInfo->phiJet());
      z0Sign = m_taggerHelper->signedZ0(z0, (*pTrack).eta(), m_trigBjetJetInfo->etaJet());
    } else if (m_trigBjetFex->m_useJetDirection == 2) {
      d0Sign = m_taggerHelper->signedD0((*pTrack).d0Corr(), (*pTrack).phi(), m_trigBjetJetInfo->phiTrkJet());
      z0Sign = m_taggerHelper->signedZ0(z0, (*pTrack).eta(), m_trigBjetJetInfo->etaTrkJet());
    } else if (m_trigBjetFex->m_useJetDirection == 3) {
      d0Sign = m_taggerHelper->signedD0((*pTrack).d0Corr(), (*pTrack).phi(), m_trigBjetJetInfo->phiRoI());
      z0Sign = m_taggerHelper->signedZ0(z0, (*pTrack).eta(), m_trigBjetJetInfo->etaRoI());
    }

    pTagger = m_trigBjetFex->m_taggers.begin();

    for (unsigned int j=0; pTagger != lastTagger; j++, pTagger++) {

      if ((*pTagger).find("IP") == std::string::npos) continue;

      if (m_logLvl <= MSG::DEBUG) 
         m_log << MSG::DEBUG << "Calculating IP likelihood weights for tagger " << (*pTagger) << endreq;

      if (m_logLvl <= MSG::VERBOSE)
	m_log << MSG::VERBOSE << (*pTrack) << endreq;

      if ((*pTagger) == "IP1D") {

	if (errIP1D) m_IP1D = z0Sign/sqrt(errIP1D*errIP1D);
        if (m_trigBjetFex->m_useLowSiHits && ((*pTrack).siHits() < 7) ) {
#ifdef VALIDATION_TOOL
          m_vectorIP1D_lowSiHits.push_back(m_IP1D);
#endif
	  w = getW("IP1D_lowSiHits", m_IP1D);
        } else {
#ifdef VALIDATION_TOOL
          m_vectorIP1D.push_back(m_IP1D);
#endif
	  w = getW("IP1D", m_IP1D);
        }

      } else if ((*pTagger) == "IP2D") {

	if (errIP2D && m_sigmaBeamSpot) m_IP2D = d0Sign/sqrt(errIP2D*errIP2D + m_sigmaBeamSpot*m_sigmaBeamSpot);
        if (m_trigBjetFex->m_useLowSiHits && ((*pTrack).siHits() < 7) ) {
#ifdef VALIDATION_TOOL
          m_vectorIP2D_lowSiHits.push_back(m_IP2D);
#endif
	  w = getW("IP2D_lowSiHits", m_IP2D);
        } else {
#ifdef VALIDATION_TOOL
          m_vectorIP2D.push_back(m_IP2D);
#endif
	  w = getW("IP2D", m_IP2D);
        }

      } else if ((*pTagger) == "IP3D") {

	if (errIP1D) m_IP1D = z0Sign/sqrt(errIP1D*errIP1D);
	if (errIP2D && m_sigmaBeamSpot) m_IP2D = d0Sign/sqrt(errIP2D*errIP2D + m_sigmaBeamSpot*m_sigmaBeamSpot);

        if (m_trigBjetFex->m_useLowSiHits && ((*pTrack).siHits() < 7) ) {
	  w = getW("IP3D_lowSiHits", m_IP2D, m_IP1D);
        } else {
	  w = getW("IP3D", m_IP2D, m_IP1D);
        }

	m_trigBjetFex->m_mon_trk_Sz0_sel.push_back(m_IP1D);
	m_trigBjetFex->m_mon_trk_Sa0_sel.push_back(m_IP2D);

      }
      
      if(m_taggersWMap[(*pTagger)]<1e17) m_taggersWMap[(*pTagger)]*= w;
      
      if (m_logLvl <= MSG::DEBUG) {
	m_log << MSG::DEBUG << (*pTrack) << endreq;
	m_log << MSG::DEBUG << "---> w(" << (*pTagger) << ") = " << w
	      << ";   W(" << (*pTagger) << ") = " << m_taggersWMap[(*pTagger)] << endreq;
      }
    }
  }

  /////////////////////////////////
  // likelihood-based SV taggers //
  /////////////////////////////////

  m_taggersWMap["MVTX"]= 1; m_taggersWMap["EVTX"]= 1; m_taggersWMap["NVTX"]= 1; m_taggersWMap["SVTX"]= 1;  m_taggersWMap["COMB"]= 1;
  m_taggersXMap["MVTX"]=-48; m_taggersXMap["EVTX"]=-48; m_taggersXMap["NVTX"]=-48; m_taggersXMap["SVTX"]=-48;  m_taggersXMap["COMB"]=-48;

  pTagger = m_trigBjetFex->m_taggers.begin();

  if(m_trigBjetSecVtxInfo->isValid()) {
     
     for ( ; pTagger != lastTagger; pTagger++) {
        
        if ((*pTagger).find("VTX") == std::string::npos) continue;
        
        if (m_logLvl <= MSG::DEBUG) 
           m_log << MSG::DEBUG << "Calculating " << (*pTagger) << " likelihood weight" << endreq;
        
        if ((*pTagger) == "MVTX") {
           
           m_taggersWMap[(*pTagger)] = getW("MVTX", m_trigBjetSecVtxInfo->vtxMass());
           
           if (m_logLvl <= MSG::DEBUG) {
              m_log << MSG::DEBUG << "Invariant mass " << m_trigBjetSecVtxInfo->vtxMass() << endreq;
              m_log << MSG::DEBUG << "---> W(" << (*pTagger) << ") = " << m_taggersWMap[(*pTagger)] << endreq;
           }
        }
        
        else if ((*pTagger) == "EVTX") { 
           
           m_taggersWMap[(*pTagger)] = getW("EVTX", m_trigBjetSecVtxInfo->energyFraction());
           
           if (m_logLvl <= MSG::DEBUG) {
              m_log << MSG::DEBUG << "Fraction of energy " << m_trigBjetSecVtxInfo->energyFraction() << endreq;
              m_log << MSG::DEBUG << "---> W(" << (*pTagger) << ") = " << m_taggersWMap[(*pTagger)] << endreq;
           }
        }
        
        else if ((*pTagger) == "NVTX") { 
           
           m_taggersWMap[(*pTagger)] = getW("NVTX", m_trigBjetSecVtxInfo->nTrksInVtx());
           
           if (m_logLvl <= MSG::DEBUG) {
              m_log << MSG::DEBUG << "Number of tracks " << m_trigBjetSecVtxInfo->nTrksInVtx() << endreq;
              m_log << MSG::DEBUG << "---> W(" << (*pTagger) << ") = " << m_taggersWMap[(*pTagger)] << endreq;
           }
        } 
        
        else if ((*pTagger) == "SVTX") {
           
           m_taggersWMap[(*pTagger)] = getW("SVTX", m_trigBjetSecVtxInfo->vtxMass(), m_trigBjetSecVtxInfo->energyFraction(), m_trigBjetSecVtxInfo->nTrksInVtx());
           
           if (m_logLvl <= MSG::DEBUG) {
              m_log << MSG::DEBUG << "Invariant mass " << m_trigBjetSecVtxInfo->vtxMass() << " Fraction of energy " 
                    << m_trigBjetSecVtxInfo->energyFraction() << " Number of tracks " << m_trigBjetSecVtxInfo->nTrksInVtx() << endreq;
              m_log << MSG::DEBUG << "---> W(" << (*pTagger) << ") = " << m_taggersWMap[(*pTagger)] << endreq;
           }
        }
     }
  }

  pTagger = m_trigBjetFex->m_taggers.begin();

  if (!m_trigBjetTrackInfoVector->size() && !m_trigBjetSecVtxInfo->isValid()) {

     if (m_logLvl <= MSG::DEBUG) {
        m_log << MSG::DEBUG << " No track info or sec vtx info available, set likelihood taggers to default" << endreq;
     }        
     for ( ; pTagger != lastTagger; pTagger++)
       m_taggersXMap[(*pTagger)] = -46;

  } else if (!m_trigBjetTrackInfoVector->size()) {
    
    if (m_logLvl <= MSG::DEBUG)
      m_log << MSG::DEBUG << " No track info but the sec vtx exists" << endreq;

    for ( ; pTagger != lastTagger; pTagger++) {
      if ((*pTagger).find("COMB") != std::string::npos) continue; //skip the combined here, set separately below
      if ((*pTagger).find("IP") == std::string::npos) {
	m_taggersXMap[(*pTagger)] = getX(m_taggersWMap[(*pTagger)]); 
      } else {
	m_taggersXMap[(*pTagger)] = -44;
      }
    }

    m_taggersWMap["COMB"] = (m_taggersWMap["EVTX"]*m_taggersWMap["NVTX"]*m_taggersWMap["MVTX"]);
    m_taggersXMap["COMB"] = getXComb(m_taggersWMap["COMB"]);

    //DEBUG
    if(m_taggersXMap["MVTX"]<0) {
      m_log << MSG::DEBUG << "There is a valid sec vtx but MVTX weight is " << m_taggersXMap["MVTX"] << ". Should not happen!" << endreq;
    }
    
  } else if (!m_trigBjetSecVtxInfo->isValid()) {
    
    if (m_logLvl <= MSG::DEBUG)
      m_log << MSG::DEBUG << " No valid sec vtx but track info exists" << endreq;
    
    for ( ; pTagger != lastTagger; pTagger++) {
      if ((*pTagger).find("COMB") != std::string::npos) continue; //skip combined here, set separately below
      if ((*pTagger).find("VTX") == std::string::npos) {
	m_taggersXMap[(*pTagger)] = getX(m_taggersWMap[(*pTagger)]);
      } else {
	m_taggersXMap[(*pTagger)] = -42;
      }
    }
    
    m_taggersWMap["COMB"] = m_taggersWMap["IP3D"];
    m_taggersXMap["COMB"] = getXComb(m_taggersWMap["COMB"]);

  } else {
    
    if (m_logLvl <= MSG::DEBUG)
      m_log << MSG::DEBUG << " Both track info and sec vtx info exists" << endreq;

    for ( ; pTagger != lastTagger; pTagger++) {
      if ((*pTagger).find("COMB") != std::string::npos) continue; //skip combined here, set separately below
      m_taggersXMap[(*pTagger)] = getX(m_taggersWMap[(*pTagger)]);
    }

    m_taggersWMap["COMB"] = m_taggersWMap["EVTX"]*m_taggersWMap["NVTX"]*m_taggersWMap["MVTX"]*m_taggersWMap["IP3D"];
    m_taggersXMap["COMB"] = getXComb(m_taggersWMap["COMB"]);

  }

  ///////////////////////////////////////
  // probability-based track IP tagger //
  ///////////////////////////////////////

  pTrack = m_trigBjetTrackInfoVector->begin();
  int m_n_pos=0;

  for (unsigned int j=0; pTrack != lastTrack; j++, pTrack++) {

    float p = -1.0;    
    float d0Sign = 0;

    if (m_trigBjetFex->m_useJetDirection == 1)
      d0Sign = m_taggerHelper->signedD0((*pTrack).d0Corr(), (*pTrack).phi(), m_trigBjetJetInfo->phiJet());
    else if (m_trigBjetFex->m_useJetDirection == 2)
      d0Sign = m_taggerHelper->signedD0((*pTrack).d0Corr(), (*pTrack).phi(), m_trigBjetJetInfo->phiTrkJet());
    else if (m_trigBjetFex->m_useJetDirection == 3)
      d0Sign = m_taggerHelper->signedD0((*pTrack).d0Corr(), (*pTrack).phi(), m_trigBjetJetInfo->phiRoI());

    float m_IP2D = d0Sign/sqrt((*pTrack).ed0()*(*pTrack).ed0() + m_sigmaBeamSpot*m_sigmaBeamSpot);

    if(fabs(m_IP2D <= 15.0)) {
      if((*pTrack).siHits() < 7)  p = f_ip(-m_IP2D,0);
      if((*pTrack).siHits() >= 7) p = f_ip(-m_IP2D,1);
    } else p = 0.0001;

    m_trigBjetFex->m_mon_trk_prob.push_back(p);

    if(d0Sign>=0 && p>=0) {

      m_taggersWMap["CHI2"]*= p; 
      m_n_pos++;

      if (m_logLvl <= MSG::DEBUG && j == 0) 
	m_log << MSG::DEBUG << "Calculating CHI2 probability weight" << endreq;
      else if (m_logLvl <= MSG::VERBOSE)
	m_log << MSG::VERBOSE << "Calculating CHI2 probability weight" << endreq;

      if (m_logLvl <= MSG::DEBUG) {
	m_log << MSG::DEBUG << (*pTrack) << endreq;
	m_log << MSG::DEBUG << "---> w(CHI2) = " << p << ";   W(CHI2) = " << m_taggersWMap["CHI2"] << endreq;
      }
    }
  }

  float prob;

  if(!m_trigBjetTrackInfoVector->size())
    prob = 1.0;
  else {
    prob = -1;
    float m_sum = 0.0; 
    
    if(m_n_pos>=1 && m_taggersWMap["CHI2"] > 0) {
      for(int i=0; i<m_n_pos; i++)
	m_sum += TMath::Power(-TMath::Log(m_taggersWMap["CHI2"]),i)/TMath::Gamma(i+1);
      prob = m_taggersWMap["CHI2"]*m_sum;
    }
    if (m_n_pos==0) prob=1.0;
  }

  float val = 1.-prob;
  if (val>=1) val = 0.999;
  if (val<=0) val = 0.001;
  
  if (!m_trigBjetTrackInfoVector->size())
    m_taggersXMap["CHI2"] = -46;
  else
    m_taggersXMap["CHI2"] = val;

  return;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigBjetTagger::getBestWeight() {
  
  if (m_taggersWMap["IP3D"] != 1 && m_taggersWMap["SVTX"] != 1)
    m_taggersWMap["COMB"] = m_taggersWMap["IP3D"]*m_taggersWMap["SVTX"];
  else if (m_taggersWMap["IP3D"] != 0)
    m_taggersWMap["COMB"] = m_taggersWMap["IP3D"];
  else
    m_taggersWMap["COMB"] = 1;

  m_taggersXMap["COMB"] = getX(1 - m_taggersWMap["COMB"]/(1.0 + m_taggersWMap["COMB"]));

  return; 
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigBjetTagger::getW(const std::string tagger, float val) {

  float min, max, num;
  float b, u, w = 1;

  int index;

  min = *m_likelihoodMap[tagger]->getMinXLikelihood();
  max = *m_likelihoodMap[tagger]->getMaxXLikelihood();
  num = *m_likelihoodMap[tagger]->getNumXLikelihood();

  index = (int)floor(((val - min)/(max - min))*num);

  if (index > (num - 1)) index = (int)num - 1;
  else if (index < 0)    index = 0;

  b = m_likelihoodMap[tagger]->getBLikelihoodValue(index);
  u = m_likelihoodMap[tagger]->getULikelihoodValue(index);

  if (b != 0 && u != 0) 
    w = b/u; 
  else if (b != 0)
    w=100.;

  return w;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigBjetTagger::getW(const std::string tagger, float valX, float valY) {
    
  float minX, maxX, numX;
  float minY, maxY, numY;
  float b, u, w = 1;

  int indexX, indexY, index;

  minX = *m_likelihoodMap[tagger]->getMinXLikelihood();
  maxX = *m_likelihoodMap[tagger]->getMaxXLikelihood();
  numX = *m_likelihoodMap[tagger]->getNumXLikelihood();

  minY = *m_likelihoodMap[tagger]->getMinYLikelihood();
  maxY = *m_likelihoodMap[tagger]->getMaxYLikelihood();
  numY = *m_likelihoodMap[tagger]->getNumYLikelihood();

  indexX = (int)floor(((valX - minX)/(maxX - minX))*numX);
  indexY = (int)floor(((valY - minY)/(maxY - minY))*numY);

  if (indexX > (numX - 1)) indexX = (int)numX - 1;
  else if (indexX < 0)     indexX = 0;

  if (indexY > (numY - 1)) indexY = (int)numY - 1;
  else if (indexY < 0)     indexY = 0;

  index = indexX*(int)numY + indexY;

  b = m_likelihoodMap[tagger]->getBLikelihoodValue(index);
  u = m_likelihoodMap[tagger]->getULikelihoodValue(index);

  if (b != 0 && u != 0) 
    w = b/u; 
  else if (b != 0)
    w=100.;
 
  return w;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigBjetTagger::getW(const std::string tagger, float valX, float valY, float valZ) {
    
  float minX, maxX, numX;
  float minY, maxY, numY;
  float minZ, maxZ, numZ;
  float b, u, w = 1;

  int indexX, indexY, indexZ, index;

  minX = *m_likelihoodMap[tagger]->getMinXLikelihood();
  maxX = *m_likelihoodMap[tagger]->getMaxXLikelihood();
  numX = *m_likelihoodMap[tagger]->getNumXLikelihood();

  minY = *m_likelihoodMap[tagger]->getMinYLikelihood();
  maxY = *m_likelihoodMap[tagger]->getMaxYLikelihood();
  numY = *m_likelihoodMap[tagger]->getNumYLikelihood();

  minZ = *m_likelihoodMap[tagger]->getMinZLikelihood();
  maxZ = *m_likelihoodMap[tagger]->getMaxZLikelihood();
  numZ = *m_likelihoodMap[tagger]->getNumZLikelihood();

  indexX = (int)floor(((valX - minX)/(maxX - minX))*numX);
  indexY = (int)floor(((valY - minY)/(maxY - minY))*numY);
  indexZ = (int)floor(((valZ - minZ)/(maxZ - minZ))*numZ);

  if (indexX > (numX - 1)) indexX = (int)numX - 1;
  else if (indexX < 0)     indexX = 0;

  if (indexY > (numY - 1)) indexY = (int)numY - 1;
  else if (indexY < 0)     indexY = 0;

  if (indexZ > (numZ - 1)) indexZ = (int)numZ - 1;
  else if (indexZ < 0)     indexZ = 0;

  index = indexX*(int)(numZ*numY) + indexY*(int)numZ + indexZ;

  b = m_likelihoodMap[tagger]->getBLikelihoodValue(index);
  u = m_likelihoodMap[tagger]->getULikelihoodValue(index);

  if (b != 0 && u != 0) 
    w = b/u; 
  else if (b != 0)
    w=100.;

  return w;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigBjetTagger::getX(double w) {

  if (w <= 0)
    return -40;

  return TMath::Log10(w);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigBjetTagger::getOldX(double w) {

  if (w == 1) 
    return 0;

  return w/(1+w);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigBjetTagger::getXComb(double w) {

  if (w/(1+w) >= 1) 
    return 50;

  return -1.0*TMath::Log10(1-(w/(1+w)));
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigBjetTagger::f_ip(float x, int iclass) {

  float* p = 0;

  if (m_trigBjetFex->m_useParamFromData) {

    if (iclass==0)
      p = &m_trigBjetFex->m_par_0_DT[0];
    else if (iclass==1)
      p = &m_trigBjetFex->m_par_1_DT[0];

  } else {

    if (iclass==0)
        p = &m_trigBjetFex->m_par_0_MC[0];
    else if (iclass==1)
        p = &m_trigBjetFex->m_par_1_MC[0];

  }

  float res=0;
  float y1  = p[2]*p[3]*erfc(fabs(x)/(sqrt(2)*p[3]));
  float y2  = p[0]*p[1]*erfc(fabs(x)/(sqrt(2)*p[1]));
  float y3  = p[4]*exp(-fabs(x)*p[5]);
  float y4  = p[6]*exp(-fabs(x)*p[7]);
  float y01 = p[2]*p[3];
  float y02 = p[0]*p[1];
  float y03 = p[4];
  float y04 = p[6];
  res=(y1+y2+y3+y4)/(y01+y02+y03+y04); 

  return res;
}
