/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKHit.h"
#include "TrigFTKSim/MultiTruth.h"
#include "TrigFTKSim/ftkdefs.h"
#include <iostream>

FTKCoord::FTKCoord() :
  m_dim(0), m_coord(0x0), m_includesGangedHits(false)
{
   // nothing else to do
}


FTKCoord::FTKCoord(int dim) :
   m_dim(dim), m_includesGangedHits(false)
{
  if (m_dim) {
    m_coord = new float[m_dim];
    for (int i=0;i<m_dim;++i) {
      m_coord[i] = 0;
    }
  }
  else m_coord = 0x0;
}


FTKCoord::FTKCoord(const FTKCoord &copy) :
  m_dim(copy.m_dim)
{
  m_includesGangedHits = copy.m_includesGangedHits;
  if (m_dim!=0) {
    m_coord = new float[m_dim];
    for (int i=0;i<m_dim;++i) {
      m_coord[i] = copy.m_coord[i];
    }
  }
  else m_coord = 0x0;
}

FTKCoord& FTKCoord::operator=(const FTKCoord &copy)
{
#ifdef PROTECT_SA // speedup
  if (this != &copy)
#endif
    {
      if (m_dim!=copy.m_dim) {
          if (m_coord) delete [] m_coord;
          m_dim = copy.m_dim;
          if (m_dim>0) m_coord = new float[m_dim];
          else m_coord = 0x0;
      }

      m_includesGangedHits = copy.m_includesGangedHits;

      for (int i=0;i<m_dim;++i) {
        m_coord[i] = copy.m_coord[i];
      }
    }
  return *this;
}

FTKCoord::~FTKCoord()
{
  if (m_coord) delete [] m_coord;
}


void FTKCoord::setDim(int v)
{
  if (m_coord) delete [] m_coord;
  m_dim = v;
  m_coord = new float[m_dim];
  for (int i=0;i<m_dim;++i) {
    m_coord[i] = 0;
  }
}

bool operator==(const FTKCoord &left, const FTKCoord &right) {
  if(left.getDim()!=right.getDim()) return false;
  for(int i=0;i<right.getDim();i++) {
    if(left[i]!=right[i]) return false;
  }
  return true;
}

std::ostream &operator<<(std::ostream& out,const FTKCoord& coord)
{
  out << '(';
  for (int i=0;i<coord.m_dim-1;++i)
    out << coord.m_coord[i] << ',';
  out << coord.m_coord[coord.m_dim-1];
  out << ')';
  
  return out;
}

ClassImp(FTKHit)

FTKHit::FTKHit() :
  m_IdentifierHash(0),
  m_sector(-1), m_plane(-1),
  m_etaWidth(0), m_phiWidth(0), m_n_strips(0),
  m_bankID(-1),
  m_coord(0), m_truth(MultiTruth()),
  m_channels()
{
   // nothing to do
}

FTKHit::FTKHit(int dim) :
  m_IdentifierHash(0),
  m_sector(-1), m_plane(-1),
  m_etaWidth(0), m_phiWidth(0), m_n_strips(0),
  m_bankID(-1),
  m_coord(dim), m_truth(MultiTruth()),
  m_channels(0)
{
   // nothing to do
}

FTKHit::FTKHit(const FTKHit &hit) :
  m_IdentifierHash(hit.m_IdentifierHash),
  m_sector(hit.m_sector), m_plane(hit.m_plane),
  m_etaWidth(hit.m_etaWidth), m_phiWidth(hit.m_phiWidth), 
  m_n_strips(hit.m_n_strips), m_bankID(hit.m_bankID),
  m_coord(hit.m_coord), m_truth(hit.m_truth),
  m_channels(hit.m_channels)
{
  // nothing to do
}

FTKHit& FTKHit::operator=(const FTKHit &hit)
{
#ifdef PROTECT_SA // speedup
  if (this != &hit) 
#endif
    {
      m_IdentifierHash = hit.m_IdentifierHash;
      m_sector = hit.m_sector;
      m_plane = hit.m_plane;
      m_etaWidth = hit.m_etaWidth;
      m_phiWidth = hit.m_phiWidth;
      m_n_strips = hit.m_n_strips;
      m_bankID = hit.m_bankID;
      m_coord = hit.m_coord;
      m_truth = hit.m_truth;
      m_channels = hit.m_channels;
    }
  return *this;
}

bool operator==(const FTKHit &left, const FTKHit &right) {
  return left.getCoord()==right.getCoord();
}

std::ostream &operator<<(std::ostream& out,const FTKHit& hit)
{
  out << "Hit plane " << hit.getPlane() << " sector " << hit.getSector()
      << " barrel/EC " << (hit.getEtaCode() < 20 ? 0 : ( hit.getASide() ? 2 : -2 ))
      << " coordinate(s) " << hit.getCoord() << " truth: "
      << std::endl;
  hit.getTruth().display();
  return out;
}

FTKHit::~FTKHit()
{
  // nothing to do
}

const MultiTruth& FTKHit::getTruth() const
{
  //  static MultiTruth zero;
  //  if(m_truth) { return zero; }
  //  return(*m_truth);
  return m_truth;
}

void FTKHit::setTruth(const MultiTruth& v)
{
  m_truth = v;
}

float FTKHit::getLocalCoord(unsigned int i) const {
  // Returns the i-th module local coordinate in millimiter units.

  unsigned int hitNCoords = this->getDim();
  if (i>=hitNCoords) return -9999;
  if (hitNCoords==2) { // Pixel case
    bool isIBL = m_plane==0; // Annovi: this is compatible only with IBL prod that uses IBL for FTK
                             // It works correctly for 2nd stage tracks with IBL
                             // It WON'T work for 1st stage tracks without IBL in the AM
    if (isIBL) {
      if (i==0) return (m_coord[i]-0.5*ftk::numberOfPhiPixelsInIblModule + 0.5 - 0.14 ) // + half a pixel  - 7um  
  		* ftk::phiPitch/ftk::millimiter;
      if (i==1) return (m_coord[i]-0.5*ftk::numberOfEtaPixelsInIblModule)
  		* ftk::etaPitchIbl/ftk::millimiter
  		* ftk::lengthOfIblModuleIn250umPixels / ftk::numberOfEtaPixelsInIblModule; // planar sensors 
    } else {
      if (i==0) {
        float localCoordIn400umUnits = m_coord[i]-0.5*ftk::numberOfPhiPixelsInPixelModule;
        if (this->getIsBarrel()) localCoordIn400umUnits += 0.5; // + half a pixel for barrel layers  
        return localCoordIn400umUnits*ftk::phiPitch/ftk::millimiter;
      }
      if (i==1) return (m_coord[i]-0.5*ftk::numberOfEtaPixelsInPixelModule)*ftk::etaPitchPixel/ftk::millimiter
                  * ftk::lengthOfPixelModuleIn400umPixels / ftk::numberOfEtaPixelsInPixelModule; // planar sensors
    }
  } 
  if (hitNCoords==1) { // SCT case
  }

  // SCT case left to be implemented since it is not needed now (and tricky for endcaps)
  // createSCT_Cluster uses strip number and Nstrips directly.

  return -999;
}
