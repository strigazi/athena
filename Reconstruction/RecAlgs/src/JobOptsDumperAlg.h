// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECO_ALGS_JOBOPTIONSDUMPERALG_H
#define RECO_ALGS_JOBOPTIONSDUMPERALG_H

#include "GaudiKernel/Property.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>

class JobOptsDumperAlg : public AthAlgorithm {
public:

  using AthAlgorithm::AthAlgorithm;
  StatusCode initialize() override;
  StatusCode execute() {return StatusCode::SUCCESS;};
    
private:
  // Properties
  Gaudi::Property<std::string> m_fileName {this, "FileName","jobOptions.txt", "Name of the output file"};  
};

#endif 