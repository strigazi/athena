/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */

// Source file for LongLivedTruthJetKinematics.h
// Jennifer Roloff, Harvard University

#include "DerivationFrameworkSUSY/LongLivedTruthJetKinematics.h"
#include "AthContainers/ConstDataVector.h"
#include <iostream>
#include <string>

namespace DerivationFramework {

LongLivedTruthJetKinematics::LongLivedTruthJetKinematics(const std::string& t, const std::string& n, const IInterface* p):
    AthAlgTool(t,n,p){
  declareInterface<DerivationFramework::IAugmentationTool>(this);

  declareProperty("InputTruthJetContainer", m_inputTruthJetContainer, "The input container for the sequence.");
  declareProperty("InputTruthParticleContainer", m_inputParticleContainer, "The input container for the sequence.");
  declareProperty("OutputContainer", m_outputTruthJetContainer, "The output container for the sequence.");
  declareProperty("CalorimeterRadius", m_caloRad, "The radius of the calorimeter");
  declareProperty("DeltaRMatching", m_dR_matching, "The maximum deltaR between particle and jet which is considered a match");
  declareProperty("LLP_PDGID", m_llp_pdgid, "The pdgid of the long-lived particle");
}

StatusCode LongLivedTruthJetKinematics::initialize() {
  ATH_MSG_INFO("Initializing tool " << name() << "...");
  ATH_MSG_DEBUG("initializing version with data handles");
  return StatusCode::SUCCESS;
}
  
StatusCode LongLivedTruthJetKinematics::addBranches() const{
  // Create the shallow copy according to the input type

  const xAOD::JetContainer* injets = nullptr;
  evtStore()->retrieve(injets, m_inputTruthJetContainer);
  std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > newjets = xAOD::shallowCopyContainer(*injets); 

  for(auto truthJet: *(newjets.first)){
    TLorentzVector newJet = matchJets( truthJet );
    xAOD::JetFourMom_t v(newJet.Pt(), newJet.Eta(), newJet.Phi(), newJet.M());
    truthJet->setJetP4( v );
  }

  ATH_CHECK( evtStore()->record( newjets.first, m_outputTruthJetContainer ) );
  ATH_CHECK( evtStore()->record( newjets.second, m_outputTruthJetContainer+"Aux." ) );

  return StatusCode::SUCCESS;
}

TLorentzVector LongLivedTruthJetKinematics::matchJets(const xAOD::Jet* mytruthJet) const{
  // Looping over all truth jets to see if they match to a LLP decay
  TLorentzVector truthJetTLV(0,0,0,0);
  truthJetTLV.SetPtEtaPhiE(mytruthJet->pt(), mytruthJet->eta(), mytruthJet->phi(), mytruthJet->e());
  double dRMinTruth = m_dR_matching;
  const xAOD::TruthParticle* matchedSparticle = nullptr;
  const xAOD::TruthParticleContainer* inparts = nullptr;

  evtStore()->retrieve(inparts, m_inputParticleContainer);
  std::pair< xAOD::TruthParticleContainer*, xAOD::ShallowAuxContainer* > inSparticles = xAOD::shallowCopyContainer(*inparts); 

  // Need to find which particle to match to
  for(auto sparticle: *(inSparticles.first)){
    // Want to make sure this is a long-lived particle, and it's not decaying to itself
    bool isDecay = isDecayParticle(sparticle);
    if(!isDecay) continue;
    const xAOD::TruthVertex* vert = sparticle->decayVtx();

		//for(auto quark: vert->OutgoingParticles()){
    for(unsigned int i=0; i<vert->nOutgoingParticles(); i++){
      if(!vert->outgoingParticle(i)) continue;
      const xAOD::TruthParticle* quark = vert->outgoingParticle(i);
      TLorentzVector outgoing(0,0,0,0);
      outgoing.SetPtEtaPhiE( quark->pt(), quark->eta(), quark->phi(), quark->e());

      // Find truth quark nearest to truth jet (within dR < 0.4)
      if( truthJetTLV.DeltaR( outgoing) < dRMinTruth ){
        dRMinTruth = truthJetTLV.DeltaR( outgoing);
        matchedSparticle = sparticle;
      }
    }
  }

  // Only care about jets matched to truth particles from LLP
  if( !matchedSparticle ) return truthJetTLV;
  if( matchedSparticle == nullptr ) return truthJetTLV;

  TLorentzVector sparticleTLV(0,0,0,0);
  sparticleTLV.SetPtEtaPhiM( matchedSparticle->pt(), matchedSparticle->eta(), matchedSparticle->phi(), matchedSparticle->m());
  const xAOD::TruthVertex* vert = matchedSparticle->decayVtx();
  double radius = sqrt(vert->x() * vert->x() + vert->y() * vert->y() + vert->z() * vert->z());

  TLorentzVector newJet = getDVKine( sparticleTLV, truthJetTLV, radius, m_caloRad);

  return newJet;
}

// Want to check if the particle has a decay, and that it does not decay to itself
bool LongLivedTruthJetKinematics::isDecayParticle(const xAOD::TruthParticle* sparticle) const{
  if(sparticle->pdgId() != m_llp_pdgid) return false;
  if(sparticle->pdgId() < 1e6) return false;
      
  if(!sparticle->hasDecayVtx()) return false;

  bool isRealDecay = true;
  for(unsigned int j=0; j<sparticle->decayVtx()->nOutgoingParticles(); j++){
    if(!sparticle->decayVtx()->outgoingParticle(j)) continue;
    if( sparticle->pdgId() == sparticle->decayVtx()->outgoingParticle(j)->pdgId()) isRealDecay = false;
  }
  const xAOD::TruthVertex* vert = sparticle->decayVtx();
  double radius = sqrt(vert->x() * vert->x() + vert->y() * vert->y() + vert->z() * vert->z());
  if(radius < 5) isRealDecay = false;

  return isRealDecay;

}

TLorentzVector LongLivedTruthJetKinematics::getDVKine(const TLorentzVector longLivedParticle, const TLorentzVector decayProduct, double r_dv, double R_cal) const{
  double ph = longLivedParticle.Phi();
  double eta = longLivedParticle.Eta();
  double th = getThetaFromEta(eta);

  double ph2 = decayProduct.Phi();
  double eta2 = decayProduct.Eta();
  double th2 = getThetaFromEta(eta2);

  if(r_dv*sin(th) > R_cal) {
    TLorentzVector dvDecay(0,0,0,0);
    dvDecay.SetPtEtaPhiM(decayProduct.Pt(), longLivedParticle.Eta(), longLivedParticle.Phi(), decayProduct.M());
    return dvDecay;
  }

  double c = (r_dv*sin(th))*(r_dv*sin(th)) - R_cal*R_cal;
  double b = 2*r_dv*sin(th)*sin(th2) * ( cos(ph)*cos(ph2) + sin(ph)*sin(ph2) );
  double a = sin(th2)*sin(th2);

  double r_tr = (-b + sqrt(b*b - 4*a*c) )/ (2*a);

  double x_cal = r_dv*sin(th)*cos(ph) + r_tr*sin(th2)*cos(ph2);
  double y_cal = r_dv*sin(th)*sin(ph) + r_tr*sin(th2)*sin(ph2);
  double z_cal = r_dv*cos(th)         + r_tr*cos(th2);

  double radius = sqrt(x_cal*x_cal + y_cal*y_cal + z_cal*z_cal);
  double theta = acos(z_cal / radius);
  double etaNew = getEtaFromTheta(theta);
  double phiNew = atan2( y_cal  , x_cal);

  TLorentzVector dvDecay(0,0,0,0);
  dvDecay.SetPtEtaPhiM(decayProduct.Pt(), etaNew, phiNew, decayProduct.M());
  return dvDecay;
}


double LongLivedTruthJetKinematics::getEtaFromTheta(double theta) const{
  double eta = - log( fabs(tan(theta / 2.)));
  return eta;
}

double LongLivedTruthJetKinematics::getThetaFromEta(double eta) const{
  double theta = 2*atan( exp( -eta ));
  return theta;
}


}
