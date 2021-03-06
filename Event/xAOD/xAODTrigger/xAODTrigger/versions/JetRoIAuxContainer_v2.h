// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetRoIAuxContainer_v2.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_JETROIAUXCONTAINER_V2_H
#define XAODTRIGGER_VERSIONS_JETROIAUXCONTAINER_V2_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Auxiliary store for the LVL1 jet RoI container
   ///
   /// This auxiliary container can describe the properties of a container
   /// of LVL1 jet RoIs.
   ///
   /// @author Lukas Heinrich <Lukas.Heinrich@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 631149 $
   /// $Date: 2014-11-26 13:26:18 +0100 (Wed, 26 Nov 2014) $
   ///
   class JetRoIAuxContainer_v2 : public AuxContainerBase{

   public:
      /// Default constuctor
      JetRoIAuxContainer_v2();

   private:
      std::vector< float > eta;
      std::vector< float > phi;
      std::vector< uint32_t > roiWord;

      std::vector< float > etScale;

      std::vector< uint32_t > thrPattern;
      std::vector< std::vector< float > > thrValues;
      std::vector< std::vector< std::string > > thrNames;

      std::vector< float > et4x4;
      std::vector< float > et6x6;
      std::vector< float > et8x8;

   }; // class JetRoIAuxContainer_v2

} // namespace xAOD

// Declare the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::JetRoIAuxContainer_v2, xAOD::AuxContainerBase );

#endif // XAODTRIGGER_VERSIONS_JETROIAUXCONTAINER_V2_H
