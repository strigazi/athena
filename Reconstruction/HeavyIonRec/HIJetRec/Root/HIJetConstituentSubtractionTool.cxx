/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetRec/HIJetConstituentSubtractionTool.h"
#include "xAODJet/JetConstituentVector.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "HIEventUtils/HIEventShapeMap.h"


//**********************************************************************

HIJetConstituentSubtractionTool::HIJetConstituentSubtractionTool(const std::string& n)
  :  JetModifierBase(n)
{
  declareProperty("EventShapeKey",m_event_shape_key);
  declareProperty("MomentName",m_moment_name="JetSubtractedScaleMomentum");
  declareProperty("SetMomentOnly",m_moment_only=true);
  declareProperty("Subtractor",m_subtractor_tool);
  declareProperty("Modulator",m_modulator_tool);
}


//**********************************************************************


int HIJetConstituentSubtractionTool::modify(xAOD::JetContainer& jets) const
{
  ATH_MSG_DEBUG("In HI tool");

  //const jet::cellset_t & badcells = badCellMap.cells() ;

  //retrieve UE

  const xAOD::HIEventShapeContainer* shape=0;
  if(EventShapeKey().compare("")==0) ATH_MSG_INFO("No HIEventShape specified, only resetting jet kinematics");

  if(evtStore()->retrieve(shape,EventShapeKey()).isFailure())
  {
    ATH_MSG_ERROR("Could not retrieve input HIEventShape " << EventShapeKey() );
    return StatusCode::FAILURE;
  }
  const HIEventShapeIndex* es_index=HIEventShapeMap::getIndex(EventShapeKey());
  if(es_index==nullptr)
  {
    ATH_MSG_ERROR("No HIEventShapeIndex w/ name " << EventShapeKey());
    return StatusCode::FAILURE;
  }

  CHECK(m_modulator_tool->retrieveShape());

  //check to see if unsubtracted moment has been stored
  for ( xAOD::JetContainer::iterator ijet=jets.begin(); ijet!=jets.end(); ++ijet)
  {

    xAOD::IParticle::FourMom_t p4((*ijet)->p4());
    xAOD::IParticle::FourMom_t p4_cl;
    xAOD::IParticle::FourMom_t p4_subtr;

    const xAOD::JetConstituentVector constituents = (*ijet)->getConstituents();
    for (xAOD::JetConstituentVector::iterator itr = constituents.begin(); itr != constituents.end(); ++itr) 
    {
      if(shape)	m_subtractor_tool->Subtract(p4_cl,itr->rawConstituent(),shape,es_index,m_modulator_tool); //modifies p4_cl to be constituent 4-vector AFTER subtraction
      p4_subtr+=p4_cl;
      
    }
    
    ATH_MSG(DEBUG) << std::setw(12) << "Subtracting" 
		   << std::setw(12) << "Before:"
		   << std::setw(7) << std::setprecision(3) << p4.E()*1e-3
		   << std::setw(7) << std::setprecision(3) << p4.Pt()*1e-3
		   << std::setw(7) << std::setprecision(3) << p4.Eta()
		   << std::setw(7) << std::setprecision(3) << p4.Phi()
		   << std::setw(7) << std::setprecision(3) << p4.M()*1e-3
		   << std::setw(10) << "After:"
		   << std::setw(7) << std::setprecision(3) << p4_subtr.E()*1e-3
		   << std::setw(7) << std::setprecision(3) << p4_subtr.Pt()*1e-3
		   << std::setw(7) << std::setprecision(3) << p4_subtr.Eta()
		   << std::setw(7) << std::setprecision(3) << p4_subtr.Phi()
		   << std::setw(7) << std::setprecision(3) << p4_subtr.M()*1e-3
		   << endreq;



    xAOD::JetFourMom_t jet4vec;
    if(p4_subtr.E() < 0.) p4_subtr*=1e-5;
    jet4vec.SetCoordinates(p4_subtr.Pt(),p4_subtr.Eta(),p4_subtr.Phi(),p4_subtr.M());
    (*ijet)->setJetP4(MomentName(),jet4vec);

    if(!MomentOnly()) 
    {
      (*ijet)->setJetP4("JetUnsubtractedScaleMomentum", (*ijet)->jetP4());
      //hack for now to allow use of pp calib tool skipping pileup subtraction
      //can be skipped in future if custom HI calibration configuration file exists
      (*ijet)->setJetP4("JetPileupScaleMomentum", jet4vec );
      (*ijet)->setJetP4(xAOD::JetEMScaleMomentum, jet4vec);

      (*ijet)->setJetP4(jet4vec);
      (*ijet)->setConstituentsSignalState(xAOD::UncalibratedJetConstituent);
    }
  }

  return 1;
}

