///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ElectronCnv_p2.cxx 
// Implementation file for class ElectronCnv_p2
/////////////////////////////////////////////////////////////////// 


// STL includes

// egammaEvent includes
#define private public
#define protected public
#include "egammaEvent/Electron.h"
#undef private
#undef protected

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaCnv_p2.h"
#include "egammaEventTPCnv/ElectronCnv_p2.h"

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

void ElectronCnv_p2::persToTrans( const Electron_p2* pers,
				  Analysis::Electron* trans, 
				  MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading Electron from persistent state..."
//       << endreq;

  // base class
  egammaCnv.persToTrans( &pers->m_egamma, trans, msg );

//   msg << MSG::DEBUG << "Loaded Electron from persistent state [OK]"
//       << endreq;

  return;
}

void ElectronCnv_p2::transToPers( const Analysis::Electron* trans, 
				  Electron_p2* pers, 
				  MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of Electron..."
//       << endreq;

  // base class
  egammaCnv.transToPers( trans, &pers->m_egamma, msg );

//   msg << MSG::DEBUG << "Created persistent state of Electron [OK]"
//       << endreq;
  return;
}
