/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParticleHelper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class include
#include "ISF_Event/ParticleHelper.h"

// HepMC includes
#include "HepMC/GenParticle.h"
#include "HepMC/Flow.h"
#include "HepMC/SimpleVector.h" // HepMC::FourVector

// ISF includes
#include "ISF_Event/ISFParticle.h"

HepMC::GenParticle* ISF::ParticleHelper::convert( ISF::ISFParticle &particle) {

  const Amg::Vector3D &mom = particle.momentum();
  double mass = particle.mass();
  double energy = sqrt( mom.mag2() + mass*mass);
  HepMC::FourVector fourMomentum( mom.x(), mom.y(), mom.z(), energy);
  int status = 1; // stable particle not decayed by EventGenerator

  HepMC::Flow flow;
  flow.set_icode( particle.barcode(), particle.getExtraBC() );

  // return a newly created GenParticle
  return new HepMC::GenParticle( fourMomentum, particle.pdgCode(), status, flow );
}

