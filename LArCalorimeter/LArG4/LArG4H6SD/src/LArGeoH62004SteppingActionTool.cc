/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoH62004SteppingActionTool.h"

#include "LArG4RunControl/LArGeoTB2004Options.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  LArGeoH62004SteppingActionTool::LArGeoH62004SteppingActionTool(const std::string& type,
                                                                 const std::string& name,
                                                                 const IInterface* parent)
    : UserActionToolBase<LArGeoH62004SteppingAction>(type, name, parent)
  {
    declareProperty("yTable", m_config.yTable);
    declareProperty("CheckPrim", m_config.checkprim);
    declareProperty("PrintStep", m_config.printstep);
  }


  StatusCode LArGeoH62004SteppingActionTool::initialize()
  {
    //FIXME Old-style configuration - to be removed
    const LArGeoTB2004Options *largeoTB2004Options = nullptr;
    if(detStore()->retrieve(largeoTB2004Options, "LArGeoTB2004Options").isFailure()) {
      ATH_MSG_WARNING ( "Can't access LArGeoTB2004Options, using default values" );
    }
    m_config.yTable = largeoTB2004Options->TableYPosition();
    m_config.checkprim = largeoTB2004Options->isCheckprim();
    m_config.printstep = largeoTB2004Options->isPrintstep();
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the action on request
  //---------------------------------------------------------------------------
  std::unique_ptr<LArGeoH62004SteppingAction>
  LArGeoH62004SteppingActionTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Construction a LArGeoH62004SteppingAction");
    auto action = std::make_unique<LArGeoH62004SteppingAction>(m_config);
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

}
