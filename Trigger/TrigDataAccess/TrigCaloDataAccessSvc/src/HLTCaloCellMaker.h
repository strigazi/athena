/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */

/**
 *
 *  NAME:     HLTCaloCellMaker.h
 *   PACKAGE:  Trigger/TrigDataAccess/TrigCaloDataAccessSvc
 *
 *    AUTHOR:   Denis Oliveira Damazio
 *
 *     PURPOSE:  New Algorithm to produce CaloCellContainer
 *      **/

#ifndef TRIGCALODATAACCESSSVC_HLTCALOCELLMAKER_H
#define TRIGCALODATAACCESSSVC_HLTCALOCELLMAKER_H

#include <string>
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"


class ITrigCaloDataAccessSvc;

/**  Test Algorithm to unpack sequence
 *     produces the CaloCellContainer output. */
class HLTCaloCellMaker: public AthReentrantAlgorithm {
     public:
	HLTCaloCellMaker(const std::string & name, ISvcLocator* pSvcLocator);
	~HLTCaloCellMaker();

	StatusCode execute_r(const EventContext& context ) const ;
	StatusCode initialize() ;
     private:

};

#endif

