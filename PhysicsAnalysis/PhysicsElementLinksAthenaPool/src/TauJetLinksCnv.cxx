/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauJetLinksCnv.h"

TauJetLinksCnv::TauJetLinksCnv(ISvcLocator* svcloc):ParticleLinksCnv<Analysis::TauJetContainer>(svcloc){
    MsgStream msg( this->msgSvc(), typeid(this).name() );
  msg << MSG::DEBUG << " constructor" << endmsg;
}

