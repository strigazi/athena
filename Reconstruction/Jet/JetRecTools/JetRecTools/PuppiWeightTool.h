/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef PUPPIWeightTool_h
#define PUPPIWeightTool_h

#include "JetRecTools/JetConstituentModifierBase.h"
#include "xAODBase/IParticleContainer.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "JetRecTools/Puppi.h"
#include <string>

class PuppiWeightTool: public JetConstituentModifierBase {
  ASG_TOOL_CLASS(PuppiWeightTool, IJetConstituentModifier)

 public:

  PuppiWeightTool(const std::string& name);

  StatusCode initialize() override final;
  StatusCode finalize() override final;

  StatusCode process_impl(xAOD::IParticleContainer* cont) const override final;
  StatusCode applyPuppiWeights(xAOD::PFOContainer* cont) const; 

 private:

  // puppi parameters
  double m_R0;
  double m_Rmin;
  double m_beta;
  double m_centralPTCutOffset;
  double m_centralPTCutSlope;
  double m_forwardPTCutOffset;
  double m_forwardPTCutSlope;
  double m_etaBoundary;

  Puppi* m_puppi;

  bool m_includeCentralNeutralsInAlpha;
  bool m_applyWeight;

  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainer_key{"PrimaryVertices"};

};

#endif
