/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUtils/JetOriginHelpers.h"

#include "CaloUtils/CaloVertexedTopoCluster.h"
#include "xAODTracking/Vertex.h"

namespace jet {

  xAOD::JetFourMom_t clusterOriginCorrection(const xAOD::Jet& jet, const xAOD::Vertex & vx){
    

    xAOD::CaloCluster::State  constitScale = (xAOD::CaloCluster::State) jet.getConstituentsSignalState();
    int numC = jet.numConstituents();
    const Amg::Vector3D& vxpos= vx.position();

    xAOD::IParticle::FourMom_t corrP4(0,0,0,0);

    for(int i=0; i<numC;i++){
      const xAOD::CaloCluster* cl = dynamic_cast<const xAOD::CaloCluster*>(jet.rawConstituent(i) );
      if( ! cl ) continue;

      xAOD::CaloVertexedTopoCluster corrCl(*cl, constitScale, vxpos );
      corrP4 += corrCl.p4();      
    }
    
    xAOD::JetFourMom_t corrJet;
    corrJet.SetPxPyPzE(corrP4.Px(), corrP4.Py(), corrP4.Pz(), corrP4.E() );
    return corrJet;
    
  }

}
