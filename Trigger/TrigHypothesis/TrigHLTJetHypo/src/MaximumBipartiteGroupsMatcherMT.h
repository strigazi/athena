/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_MAXIMUMBIPARTITEGROUPSMATCHERMT_H
#define TRIGHLTJETHYPO_MAXIMUMBIPARTITEGROUPSMATCHERMT_H

// ********************************************************************
//
// NAME:     MaximumBipartiteGroupsMatcherMT.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include "./FlowNetworkBase.h"
#include "./ConditionsDefsMT.h"

class MaximumBipartiteGroupsMatcherMT:
virtual public FlowNetworkBase {

  /* An initialiser for FlowNetwork base */

public:
  MaximumBipartiteGroupsMatcherMT(ConditionsMT&& cs);
  ~MaximumBipartiteGroupsMatcherMT(){}
};

#endif
