/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT
 * @Package: TrigDecisionEventTPCnv
 * @class  : TrigDecision_p5
 *
 * @brief  persistent representation of all three trigger level information - for now
 *         this is exactly what is in TrigDecisionData.
 *
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 * @author Nicolas Berger   <Nicolas.Berger@cern.ch>   - CERN
 * @author Till Eifert      <Till.Eifert@cern.ch>      - U. of Geneva, Switzerland
 * @author Ricardo Goncalo  <Jose.Goncalo@cern.ch>     - Royal Holloway, U. of London
 * @author Tomasz Bold      <Tomasz.Bold@cern.ch>      - AGH-UST Krakow
 *
 *
 * File and Version Information:
 * $Id: TrigDecision_p5.h,v 1.2 2009-04-01 22:04:16 salvator Exp $
 **********************************************************************************/

#ifndef TrigDecisionEventTPCnv_TrigDecision_p5_H
#define TrigDecisionEventTPCnv_TrigDecision_p5_H

#include "TrigSteeringEventTPCnv/HLTResult_p1.h"
#include "TrigSteeringEventTPCnv/Lvl1Result_p2.h"
#include "DataModelAthenaPool/DataLink_p2.h"
#include "iostream"
#include <vector>

// needed to fix a Gaudi problem in SLC3 (should be removed eventually):
#include <stdint.h>

namespace TrigDec {

  class TrigDecision_p5
  {
    friend class TrigDecisionCnv; //!< TrigDecisionCnv will set all member variables !?

  public:

    // default constructor
    TrigDecision_p5() {
    }

    TrigDecision_p5(const TrigDecision_p5& other) :
      m_l1_result(other.getL1Result()),
      m_l2_result(other.m_l2_result),
      m_ef_result(other.m_ef_result) {
    }

    virtual ~TrigDecision_p5() { }

    uint32_t configMasterKey() const { return m_configMasterKey; }

    const LVL1CTP::Lvl1Result_p2&  getL1Result() const { return m_l1_result; }
    //    const HLT::HLTResult_p1&       getL2Result() const { return *m_l2_result; }
    //    const HLT::HLTResult_p1&       getEFResult() const { return *m_ef_result; }

    //private:

    uint32_t m_configMasterKey = 0;
    char     m_bgCode = 0;

    LVL1CTP::Lvl1Result_p2  m_l1_result; //!< Light result of trigger level 1
    DataLink_p2 m_l2_result;             //!< HLTResult of trigger level 2
    DataLink_p2 m_ef_result;             //!< HLTResult of trigger level EF
  };

} // end of namespace


#endif
