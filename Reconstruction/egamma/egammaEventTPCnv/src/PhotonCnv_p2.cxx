///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhotonCnv_p2.cxx 
// Implementation file for class PhotonCnv_p2
/////////////////////////////////////////////////////////////////// 


// STL includes

// egammaEvent includes
#define private public
#define protected public
#include "egammaEvent/Photon.h"
#undef private
#undef protected

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaCnv_p2.h"
#include "egammaEventTPCnv/PhotonCnv_p2.h"

// pre-allocate converters
static egammaCnv_p2   egammaCnv;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

void PhotonCnv_p2::persToTrans( const Photon_p2* pers,
				Analysis::Photon* trans, 
				MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading Photon from persistent state..."
//       << endreq;

  // base class
  egammaCnv.persToTrans( &pers->m_egamma, trans, msg );

//   msg << MSG::DEBUG << "Loaded Photon from persistent state [OK]"
//       << endreq;

  return;
}

void PhotonCnv_p2::transToPers( const Analysis::Photon* trans, 
				Photon_p2* pers, 
				MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of Photon..."
//       << endreq;

  // base class
  egammaCnv.transToPers( trans, &pers->m_egamma, msg );

//   msg << MSG::DEBUG << "Created persistent state of Photon [OK]"
//       << endreq;
  return;
}
