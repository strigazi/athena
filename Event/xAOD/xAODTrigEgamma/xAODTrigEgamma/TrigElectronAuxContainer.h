// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigElectronAuxContainer.h 631414 2014-11-26 22:03:16Z gwatts $
#ifndef XAODTRIGEGAMMA_TRIGELECTRONAUXCONTAINER_H
#define XAODTRIGEGAMMA_TRIGELECTRONAUXCONTAINER_H

// Local include(s):
#include "xAODTrigEgamma/versions/TrigElectronAuxContainer_v1.h"

namespace xAOD {
   /// Declare the latest version of the auxiliary container
   typedef TrigElectronAuxContainer_v1 TrigElectronAuxContainer;
}

#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigElectronAuxContainer, 1180743572, 1 )

#endif // not XAODTRIGEGAMMA_TRIGELECTRONAUXCONTAINER_H
