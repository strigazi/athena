// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AthDataAccess.h 637174 2014-12-20 00:26:47Z ssnyder $
#ifndef TRIGROOTANALYSIS_ATHDATAACCESS_H
#define TRIGROOTANALYSIS_ATHDATAACCESS_H

// ROOT include(s):
#include <Rtypes.h>

// Gaudi/Athena include(s):
#include "StoreGate/ReadHandle.h"

// Local include(s):
#include "../TrigRootAnalysis/IDataAccess.h"

namespace D3PD {

   /**
    *  @short Athena implementation of the IDataAccess interface
    *
    *         This implementation of the IDataAccess interface is used as
    *         a component of the Athena version of ITrigDecisionToolD3PD.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 637174 $
    * $Date: 2014-12-20 01:26:47 +0100 (Sat, 20 Dec 2014) $
    */
   class AthDataAccess : public virtual D3PD::Trig::IDataAccess {

   public:
      /// Default constructor
      AthDataAccess();

      /// Get the detail level that the D3PD was produced with
      virtual ::Int_t GetDetailLevel() const;

      /// Get the Super Master Key of the current event
      virtual ::Int_t GetSMK() const;
      /// Get the LVL1 prescale key of the current event
      virtual ::Int_t GetL1PSK() const;
      /// Get the HLT prescale key of the current event
      virtual ::Int_t GetHLTPSK() const;

      /// Function for retrieving the encoded LVL1 result
      virtual const std::vector< unsigned int >*
      GetL1Result( L1ResultType type ) const;
      /// Function for retrieving the encoded LVL2 result
      virtual const std::vector< short >*
      GetL2Result( HLTResultType type ) const;
      /// Function for retrieving the encoded EF result
      virtual const std::vector< short >*
      GetEFResult( HLTResultType type ) const;

   private:
      /// Set the prefix of the variable names
      void setPrefix( const std::string& prefix );

      //
      // Variables coming from TrigDBKeysD3PDObject:
      //
      SG::ReadHandle< ::UInt_t > m_smk;
      SG::ReadHandle< ::UInt_t > m_l1psk;
      SG::ReadHandle< ::UInt_t > m_hltpsk;

      //
      // Variables coming from TrigDecisionD3PDObject:
      //
      SG::ReadHandle< std::vector< unsigned int > > m_l1_tav;
      SG::ReadHandle< std::vector< short > >        m_l2_physics;
      SG::ReadHandle< std::vector< short > >        m_ef_physics;

      SG::ReadHandle< std::vector< unsigned int > > m_l1_tbp;
      SG::ReadHandle< std::vector< unsigned int > > m_l1_tap;
      SG::ReadHandle< std::vector< short > >        m_l2_raw;
      SG::ReadHandle< std::vector< short > >        m_ef_raw;

      SG::ReadHandle< std::vector< short > > m_l2_resurrected;
      SG::ReadHandle< std::vector< short > > m_ef_resurrected;

      SG::ReadHandle< std::vector< short > > m_l2_passedThrough;
      SG::ReadHandle< std::vector< short > > m_ef_passedThrough;

   }; // class AthDataAccess

} // namespace D3PD

#endif // TRIGROOTANALYSIS_ATHDATAACCESS_H
