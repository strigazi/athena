/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @class  : TrigL2BjetContainer_p1
 *
 * @brief persistent partner for TrigL2BjetContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigL2BjetContainer_p1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGL2BJETCONTAINER_P1_H
#define TRIGPARTICLETPCNV_TRIGL2BJETCONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class TrigL2BjetContainer_p1 : public std::vector<TPObjRef>
{
 public:
    
  TrigL2BjetContainer_p1(){}
     
};// end of class definitions
 
 
#endif
