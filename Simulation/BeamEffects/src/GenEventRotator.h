// Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_HEPMC_GENEVENTROTATOR_H
#define ISF_HEPMC_GENEVENTROTATOR_H 1

// Base class headers
#include "AthenaBaseComps/AthAlgTool.h"
#include "HepMC_Interfaces/IGenEventManipulator.h"
// Gaudi headers
#include "GaudiKernel/ServiceHandle.h"
// CLHEP headers
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/LorentzRotation.h"

#include "AtlasHepMC/GenParticle.h"

namespace Simulation {

  /** @class GenEventRotator

      This tool takes a HepMC::GenEvent and applies rotations
      See https://twiki.cern.ch/twiki/bin/viewauth/Atlas/AtlasG4EventFilters for more details.
  */
  class GenEventRotator : public extends<AthAlgTool, IGenEventManipulator>
  {

  public:
    /** Constructor with parameters */
    GenEventRotator( const std::string& t, const std::string& n, const IInterface* p );

    /** Athena algtool's Hooks */
    StatusCode initialize() override final;
    StatusCode finalize() override final;

    StatusCode initializeAthenaEvent();// override final;

    /** modifies the given GenEvent */
    StatusCode manipulate(HepMC::GenEvent& ge) const override final;
  private:
    /** calculate the transformations that we want to apply to the particles in the current GenEvent */
    StatusCode initializeGenEvent(CLHEP::HepLorentzRotation& transform) const;
    /** apply rotations to individual GenParticles */
    void rotateParticle(HepMC::GenParticlePtr p, const CLHEP::HepLorentzRotation& transform) const;

    double m_xangle; /** rotation about x-axis (degrees) */
    double m_yangle; /** rotation about y-axis (degrees) */
    double m_zangle; /** rotation about z-axis (degrees) */
  };

}

#endif //> !ISF_HEPMC_GENEVENTROTATOR_H
