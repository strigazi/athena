/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef __ELECTRONEFFICIENCYCORRECTION_CREATEDUMMYEL__
#define __ELECTRONEFFICIENCYCORRECTION_CREATEDUMMYEL__

#include <xAODEgamma/ElectronContainer.h>
#include <vector>
// Infrastructure include(s):
#ifdef ROOTCORE
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#endif // ROOTCORE

xAOD::TStore* getElectrons(const std::vector<std::pair<double,double> >& pt_eta, int runNumber);
#endif
