/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODJet/versions/JetTrigAuxContainer_v1.h"

namespace xAOD {

   JetTrigAuxContainer_v1::JetTrigAuxContainer_v1()
      : ByteStreamAuxContainer_v1() {

      AUX_VARIABLE( constituentLinks );
      AUX_VARIABLE( constituentWeights );

      AUX_VARIABLE( pt );
      AUX_VARIABLE( eta );
      AUX_VARIABLE( phi );
      AUX_VARIABLE( m );
   }

} // namespace xAOD
