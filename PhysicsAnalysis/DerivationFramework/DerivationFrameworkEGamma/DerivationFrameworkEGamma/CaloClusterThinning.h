/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CaloClusterThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EGAMMACALOCLUSTERPARTICLETHINNING_H
#define DERIVATIONFRAMEWORK_EGAMMACALOCLUSTERPARTICLETHINNING_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
//#include "GaudiKernel/ToolHandle.h"
//#include "DerivationFrameworkInDet/TracksInCone.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODCaloEvent/CaloCluster.h"

namespace ExpressionParsing {
  class ExpressionParser;
}

class IThinningSvc;

namespace DerivationFramework {

  class CaloClusterThinning : public AthAlgTool, public IThinningTool {
    public:
      CaloClusterThinning(const std::string& t, const std::string& n, const IInterface* p);
      ~CaloClusterThinning();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode doThinning() const;

    private:
      ServiceHandle<IThinningSvc> m_thinningSvc;
      mutable unsigned int m_ntot, m_ntotTopo, m_npass, m_npassTopo; //, m_ntotFrwd, m_npassFrwd;
      mutable bool m_is_muons, m_is_egamma, m_run_calo, m_run_topo;
      std::string m_sgKey;
      std::string m_CaloClSGKey;
      std::string m_TopoClSGKey;
      //std::string m_FrwdClSGKey;
      std::string m_selectionString;
      float m_coneSize;
      bool m_and;
      bool m_keep;
      ExpressionParsing::ExpressionParser *m_parser;

      //template <class T> void setClustersMask(std::vector<bool>& mask, const T*& particles, const xAOD::CaloClusterContainer*& cps) const;

      StatusCode setClustersMask(std::vector<bool>& mask, const xAOD::IParticleContainer* particles, const xAOD::CaloClusterContainer*& cps) const;
      StatusCode setClustersMask(std::vector<bool>& mask, std::vector<const xAOD::IParticle*>& particles, const xAOD::CaloClusterContainer*& cps) const;

      template <class T> StatusCode particleCluster(std::vector<bool>& mask, T*& particle, const xAOD::CaloClusterContainer*& cps) const;
/*
      void setPhItsClustersMask(std::vector<bool>&, const xAOD::EgammaContainer*&, const xAOD::CaloClusterContainer*&) const;
      void setElItsClustersMask(std::vector<bool>&, const xAOD::EgammaContainer*&, const xAOD::CaloClusterContainer*&) const;

      void setPhClustersMask(std::vector<bool>&, const xAOD::EgammaContainer*&, const xAOD::CaloClusterContainer*&) const;
      void setPhClustersMask(std::vector<bool>&, std::vector<const xAOD::Egamma*>&, const xAOD::CaloClusterContainer*&) const;

      void setElClustersMask(std::vector<bool>&, const xAOD::EgammaContainer*&, const xAOD::CaloClusterContainer*&) const;
      void setElClustersMask(std::vector<bool>&, std::vector<const xAOD::Egamma*>&, const xAOD::CaloClusterContainer*&) const;
*/
  };
}

#endif // DERIVATIONFRAMEWORK_EGAMMACALOCLUSTERPARTIGLETHINNING_H
