/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HIClusterSubtraction.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "HIEventUtils/HIEventShapeMap.h"
#include "HIJetRec/HIJetRecDefs.h"
#include "xAODCore/ShallowAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

//**********************************************************************

HIClusterSubtraction::HIClusterSubtraction(std::string name) : asg::AsgTool(name)//,
{
}

//**********************************************************************

StatusCode HIClusterSubtraction::initialize()
{
	//New key for the shallow copy automatically built from the cluster key
	m_outClusterKey = m_inClusterKey.key() + ".shallowCopy";
	//Keys initialization
	ATH_CHECK( m_eventShapeKey.initialize() );
	ATH_CHECK( m_inClusterKey.initialize() );
	ATH_CHECK( m_outClusterKey.initialize() );
	//Vertex container needs to be initialized only if origin correction will take place
	ATH_CHECK( m_vertexContainer.initialize( m_originCorrection ) );

  for (auto tool :  m_clusterCorrectionTools)
  {
    StatusCode sc = tool.retrieve();
    if (sc.isFailure()) ATH_MSG_ERROR("Failed to retrieve correction tool " << tool);
    else ATH_MSG_DEBUG("Successfully retrieved correction tool " << tool);
  }
  return StatusCode::SUCCESS;
}

bool HIClusterSubtraction::doOriginCorrection( xAOD::CaloCluster* cl, const xAOD::Vertex* origin, xAOD::IParticle::FourMom_t& p4_cl ) const{
	//made boolean to return what was "missingMoment" in HIJetConstituentSubtractionTool
	bool missingMoment = false;
	float mag = 0;
	if(cl->isAvailable<float>("HIMag")) mag=cl->auxdataConst<float>("HIMag");
	else
	{
		double cm_mag=0;
		if(cl->retrieveMoment (xAOD::CaloCluster::CENTER_MAG, cm_mag)) mag=cm_mag;
	}
	if(mag!=0.)
	{
		float eta0=cl->eta0();
		float phi0=cl->phi0();
		float radius=mag/std::cosh(eta0);
		xAOD::IParticle::FourMom_t p4_pos;
		p4_pos.SetX(radius*std::cos(phi0)-origin->x());
		p4_pos.SetY(radius*std::sin(phi0)-origin->y());
		p4_pos.SetZ(radius*std::sinh(eta0)-origin->z());

		double deta=p4_pos.Eta()-eta0;
		double dphi=p4_pos.Phi()-phi0;
		//adjust in case eta/phi are flipped in case of neg E clusters
		//this method is agnostic wrt convention
		if(p4_cl.Eta()*eta0 <0.) deta*=-1;

		double eta_prime=p4_cl.Eta()+deta;
		double phi_prime=p4_cl.Phi()+dphi;
		double e_subtr=p4_cl.E();
		p4_cl.SetPtEtaPhiE(e_subtr/std::cosh(eta_prime),eta_prime,phi_prime,e_subtr);
	}
	else missingMoment=true;

	return missingMoment;
}

int HIClusterSubtraction::execute() const
{
  //const jet::cellset_t & badcells = badCellMap.cells() ;
  //retrieve UE
	ATH_MSG_WARNING("HIClusterSubtraction being rebuilt to work in MT - upgrade not yet over! ");
	const xAOD::HIEventShapeContainer* shape = 0;
	SG::ReadHandle<xAOD::HIEventShapeContainer>  readHandleEvtShape ( m_eventShapeKey );
  shape = readHandleEvtShape.cptr();
  const HIEventShapeIndex* es_index = HIEventShapeMap::getIndex( m_eventShapeKey.key() );

  const xAOD::HIEventShape* eshape = nullptr;
  CHECK(m_modulatorTool->getShape(eshape), 1);
	ATH_MSG_DEBUG("HIClusterSubtraction creating key for ShallowCopy! ");

  //New implementation: make a shallow copy of original HIClusters and apply subtraction to clusters in the new container
	SG::ReadHandle<xAOD::CaloClusterContainer>  readHandleClusters ( m_inClusterKey );
  std::pair<xAOD::CaloClusterContainer*,xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(*readHandleClusters);
  /// Set whether only the overriden parameters should be written out - default is true
  //shallowcopy.second->setShallowIO(m_shallowIO);
  // Now a handle to write the shallow Copy
  SG::WriteHandle<xAOD::CaloClusterContainer> writeHandleShallowClusters ( m_outClusterKey );

  // Preparing keys and container to perfrom the origin correction
	const xAOD::Vertex* primVertex=nullptr;
	const xAOD::VertexContainer* vertices=nullptr;
	// ReadHandle to retrieve the vertex container
	SG::ReadHandle<xAOD::VertexContainer>  readHandleVertexContainer ( m_vertexContainer );
  // Boolean to flag that at least a vertex is present in the vertex container
	bool isOriginPossible = true;
	// Finding the primary vertex in case origin correction has to be performed
	if(m_originCorrection)
  {
    vertices = readHandleVertexContainer.get();
    for ( size_t iVertex = 0; iVertex < vertices->size(); ++iVertex )
    {
      if(vertices->at(iVertex)->vertexType() == xAOD::VxType::PriVtx)
      {
        	primVertex=vertices->at(iVertex);
        	break;
      }
    }
    if(!primVertex && vertices->size() > 0)
    {
      ATH_MSG_WARNING("No primary vertices found, using first in container");
      primVertex=vertices->at(0);
    }
		if(!primVertex && vertices->size() == 0)
    {
      ATH_MSG_WARNING("No primary vertices found, and vertex container empty. Abortin Origin correction for this event.");
      isOriginPossible = false;
		}
  }
	bool missingMoment=false;

  if(m_updateMode)
  {
    std::unique_ptr<std::vector<float> > subtractedE(new std::vector<float>());
    subtractedE->reserve(shallowcopy.first->size());
		//Decoration via SG::AuxElement::Decorator should still work in MT code (that seems from browsing the code)
    SG::AuxElement::Decorator< float > decorator("HISubtractedE");

    for(xAOD::CaloClusterContainer::const_iterator itr=shallowcopy.first->begin(); itr!=shallowcopy.first->end(); itr++)
    {
      const xAOD::CaloCluster* cl=*itr;
      xAOD::IParticle::FourMom_t p4;
      m_subtractorTool->subtract(p4,cl,shape,es_index,m_modulatorTool,eshape);
      subtractedE->push_back(p4.E());
      decorator(*cl)=p4.E();
    }
  }
  else
  {
    for(xAOD::CaloClusterContainer::iterator itr=shallowcopy.first->begin(); itr!=shallowcopy.first->end(); itr++)
    {
      xAOD::CaloCluster* cl=*itr;
			xAOD::IParticle::FourMom_t p4;

			//Unsubtracted state record done by the subtractor tool functions.
      if(m_setMoments) {
				  //This flag is set to false for HIJetClustersSubtractorTool and true for HIJetCellSubtractorTool,
					// but for the second we don't do origin correction. In principle the code is structured to do the same as the
					//else for m_setMoments=true and HIJetClustersSubtractorTool, therefore we add the code for origin correction also here
				  m_subtractorTool->subtractWithMoments(cl, shape, es_index, m_modulatorTool, eshape);
					if(isOriginPossible && m_originCorrection){
						missingMoment = HIClusterSubtraction::doOriginCorrection( cl, primVertex, p4 );
						HIJetRec::setClusterP4(p4,cl,HIJetRec::subtractedPVCorrectedClusterState());
					}
			}
      else
      {
					m_subtractorTool->subtract(p4,cl,shape,es_index,m_modulatorTool,eshape);
					HIJetRec::setClusterP4(p4,cl,HIJetRec::subtractedClusterState());
					ATH_MSG_INFO("Applying origin correction"
						<< std::setw(12) << "Before:"
						<< std::setw(10) << std::setprecision(3) << p4.Pt()*1e-3
						<< std::setw(10) << std::setprecision(3) << p4.Eta()
						<< std::setw(10) << std::setprecision(3) << p4.Phi()
						<< std::setw(10) << std::setprecision(3) << p4.E()*1e-3
						<< std::setw(10) << std::setprecision(3) << p4.M()*1e-3);
					if(isOriginPossible){
						missingMoment = HIClusterSubtraction::doOriginCorrection( cl, primVertex, p4 );
					  HIJetRec::setClusterP4(p4,cl,HIJetRec::subtractedPVCorrectedClusterState());
					}
					ATH_MSG_INFO("Applying origin correction"
						<< std::setw(12) << "After:"
						<< std::setw(10) << std::setprecision(3) << p4.Pt()*1e-3
						<< std::setw(10) << std::setprecision(3) << p4.Eta()
						<< std::setw(10) << std::setprecision(3) << p4.Phi()
						<< std::setw(10) << std::setprecision(3) << p4.E()*1e-3
						<< std::setw(10) << std::setprecision(3) << p4.M()*1e-3);
					}
    }//End of iterator over CaloClusterContainer

    for(ToolHandleArray<CaloClusterCollectionProcessor>::const_iterator toolIt=m_clusterCorrectionTools.begin();
	      toolIt != m_clusterCorrectionTools.end(); toolIt++)
    {
      ATH_MSG_INFO(" Applying correction = " << (*toolIt)->name() );
			CHECK((*toolIt)->execute(Gaudi::Hive::currentContext(), shallowcopy.first), 1);
    }//End loop over correction tools
  }
	if(missingMoment) ATH_MSG_WARNING("No origin correction applied, CENTERMAG missing");

// Make sure that memory is managed safely
  std::unique_ptr<xAOD::CaloClusterContainer> outClusters(shallowcopy.first);
  std::unique_ptr<xAOD::ShallowAuxContainer> shallowAux(shallowcopy.second);

  // Connect the copied jets to their originals
  xAOD::setOriginalObjectLink(*readHandleClusters, *outClusters);

	if(writeHandleShallowClusters.record ( std::move(outClusters), std::move(shallowAux)).isFailure() ){
			ATH_MSG_ERROR("Unable to write Shallow Copy containers for event shape with key: " << m_outClusterKey.key());
			return 1;
	}
  return 0;
}
