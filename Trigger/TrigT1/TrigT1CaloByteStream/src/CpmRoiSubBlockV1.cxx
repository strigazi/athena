/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1CaloEvent/CPMRoI.h"

#include "CpmRoiSubBlockV1.h"

namespace LVL1BS {

// Constant definitions

const int CpmRoiSubBlockV1::s_wordIdVal;

const int CpmRoiSubBlockV1::s_glinkPins;
const int CpmRoiSubBlockV1::s_hitsLen;
const int CpmRoiSubBlockV1::s_errorLen;
const int CpmRoiSubBlockV1::s_locationLen;
const int CpmRoiSubBlockV1::s_bunchCrossingBits;


CpmRoiSubBlockV1::CpmRoiSubBlockV1()
{
}

CpmRoiSubBlockV1::~CpmRoiSubBlockV1()
{
}

// Clear all data

void CpmRoiSubBlockV1::clear()
{
  L1CaloSubBlock::clear();
  m_roiData.clear();
}

// Store header

void CpmRoiSubBlockV1::setRoiHeader(const int version, const int crate,
                                                       const int module)
{
  setHeader(s_wordIdVal, version, NEUTRAL, 0, crate, module, 0, 1);
}

// Store RoI

void CpmRoiSubBlockV1::fillRoi(const LVL1::CPMRoI roi)
{
  const LVL1::CPMRoI roiTemp(crate(), module(), 0, 0, 0, 0);
  if (roi.crate() == roiTemp.crate() && roi.cpm() == roiTemp.cpm()) {
    m_roiData.resize(s_glinkPins);
    const int pin = (roi.chip() << 1) |
                    ((roi.location() >> s_locationLen) & 0x1);
    m_roiData[pin] = roi;
  }
}

// Return RoI for given chip and location (left/right) 

LVL1::CPMRoI CpmRoiSubBlockV1::roi(const int chip, const int loc) const
{
  const int pin = (chip << 1) | (loc & 0x1);
  if (pin < s_glinkPins && !m_roiData.empty()) return m_roiData[pin];
  else return LVL1::CPMRoI(0);
}

// Packing/Unpacking routines

bool CpmRoiSubBlockV1::pack()
{
  bool rc = false;
  switch (version()) {
    case 1:
      switch (format()) {
        case NEUTRAL:
	  rc = packNeutral();
	  break;
        default:
	  break;
      }
      break;
    default:
      break;
  }
  return rc;
}

bool CpmRoiSubBlockV1::unpack()
{
  bool rc = false;
  switch (version()) {
    case 1:
      switch (format()) {
        case NEUTRAL:
	  rc = unpackNeutral();
	  break;
        default:
	  setUnpackErrorCode(UNPACK_FORMAT);
	  break;
      }
      break;
    default:
      setUnpackErrorCode(UNPACK_VERSION);
      break;
  }
  return rc;
}

// Pack neutral data

bool CpmRoiSubBlockV1::packNeutral()
{
  m_roiData.resize(s_glinkPins);
  for (int pin = 0; pin < s_glinkPins; ++pin) {
    // RoI data
    const LVL1::CPMRoI& roi(m_roiData[pin]);
    packerNeutral(pin, roi.hits(), s_hitsLen);
    packerNeutral(pin, roi.error(), s_errorLen);
    packerNeutral(pin, roi.location(), s_locationLen);
    // Bunch Crossing number
    if (pin < s_bunchCrossingBits) {
      packerNeutral(pin, bunchCrossing() >> pin, 1);
    } else packerNeutral(pin, 0, 1);
    // G-Link parity
    packerNeutralParity(pin);
  }
  return true;
}

// Unpack neutral data

bool CpmRoiSubBlockV1::unpackNeutral()
{
  m_roiData.resize(s_glinkPins);
  int bunchCrossing = 0;
  for (int pin = 0; pin < s_glinkPins; ++pin) {
    // RoI data
    const int hits  = unpackerNeutral(pin, s_hitsLen);
    const int error = unpackerNeutral(pin, s_errorLen);
    const int loc   = unpackerNeutral(pin, s_locationLen) |
                                           ((pin & 0x1) << s_locationLen);
    const int chip = pin >> 1;
    m_roiData[pin] = LVL1::CPMRoI(crate(), module(), chip, loc, hits, error);
    // Bunch Crossing number
    if (pin < s_bunchCrossingBits) {
      bunchCrossing |= unpackerNeutral(pin, 1) << pin;
    } else unpackerNeutral(pin, 1);
    // G-Link parity error
    unpackerNeutralParityError(pin);
  }
  setBunchCrossing(bunchCrossing);
  const bool rc = unpackerSuccess();
  if (!rc) setUnpackErrorCode(UNPACK_DATA_TRUNCATED);
  return rc;
}

} // end namespace
