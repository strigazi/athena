/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$

// Gaudi/Athena include(s):
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s):
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/versions/L2StandAloneMuonContainer_v1.h"
#include "xAODTrigMuon/L2StandAloneMuonAuxContainer.h"
#include "xAODTrigMuon/versions/L2StandAloneMuonAuxContainer_v1.h"

// Local include(s):
#include "xAODL2StandAloneMuonContainerCnv_v1.h"
#include "xAODL2StandAloneMuonAuxContainerCnv_v1.h"

// Declare the T/P converter(s):
DECLARE_TPCNV_FACTORY( xAODL2StandAloneMuonContainerCnv_v1,
                       xAOD::L2StandAloneMuonContainer,
                       xAOD::L2StandAloneMuonContainer_v1,
                       Athena::TPCnvVers::Old )
DECLARE_TPCNV_FACTORY( xAODL2StandAloneMuonAuxContainerCnv_v1,
                       xAOD::L2StandAloneMuonAuxContainer,
                       xAOD::L2StandAloneMuonAuxContainer_v1,
                       Athena::TPCnvVers::Old )
