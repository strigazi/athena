/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LLPStrategy_H
#define LLPStrategy_H

#include "MCTruth/TruthStrategy.h"

class LLPStrategy: public TruthStrategy {
  public:
   LLPStrategy(const std::string);
   bool AnalyzeVertex(const G4Step*);
  private:
   bool isSUSYParticle(const int) const;
};

#endif
