// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id:  $
#ifndef XAODMUONATHENAPOOL_XAODMUONAUXCONTAINERCNV_V1_H
#define XAODMUONATHENAPOOL_XAODMUONAUXCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODMuon/versions/MuonAuxContainer_v1.h"
#include "xAODMuon/MuonAuxContainer.h"

/// Converter class used for reading xAOD::MuonAuxContainer_v1
///
/// This converter implements the conversion from xAOD::MuonAuxContainer_v1
/// to the latest version of the class. In a way that makes this converter
/// usable both from the POOL converter, and the BS converter.
///
/// @author Edward.Moyse@cern.ch (based on example from Attila)
///
/// $Revision: 619884 $
/// $Date: 2014-10-03 11:00:40 -0400 (Fri, 03 Oct 2014) $
///
class xAODMuonAuxContainerCnv_v1 :
   public T_AthenaPoolTPCnvConstBase< xAOD::MuonAuxContainer,
                                      xAOD::MuonAuxContainer_v1 > {

public:
   using base_class::transToPers;
   using base_class::persToTrans;

   /// Default constructor
   xAODMuonAuxContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::MuonAuxContainer_v1* oldObj,
                             xAOD::MuonAuxContainer* newObj,
                             MsgStream& log ) const override;
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::MuonAuxContainer*,
                             xAOD::MuonAuxContainer_v1*,
                             MsgStream& log ) const override;

}; // class xAODMuonAuxContainerCnv_v1

#endif // XAODMUONATHENAPOOL_XAODMUONAUXCONTAINERCNV_V1_H
