/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
// BeamSpot_ConditionsAlgs includes
#include "BeamSpotCondAlg.h"
#include "CoralBase/AttributeListException.h"

const EventIDRange BeamSpotCondAlg::alwaysValid { EventIDBase { 1,
                                                                EventIDBase::UNDEFEVT,
                                                                EventIDBase::UNDEFNUM,
                                                                EventIDBase::UNDEFNUM,
                                                                0 },
                                                  EventIDBase { EventIDBase::UNDEFNUM - 1,
	                                                        EventIDBase::UNDEFEVT,
	                                                        EventIDBase::UNDEFNUM,
	                                                        EventIDBase::UNDEFNUM,
	                                                        EventIDBase::UNDEFNUM - 1 }
                                                };

BeamSpotCondAlg::BeamSpotCondAlg( const std::string& name, ISvcLocator* pSvcLocator ) 
  : AthAlgorithm( name, pSvcLocator )
{ }


BeamSpotCondAlg::~BeamSpotCondAlg() {}


StatusCode BeamSpotCondAlg::initialize() 
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  ATH_CHECK( m_condSvc.retrieve() );

  ATH_CHECK( m_readKey.initialize() );
  ATH_CHECK( m_writeKey.initialize() );

  if ( m_condSvc->regHandle( this, m_writeKey ).isFailure() )
  {
    ATH_MSG_ERROR("Unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode BeamSpotCondAlg::finalize() 
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode BeamSpotCondAlg::execute() 
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  SG::WriteCondHandle<InDet::BeamSpotData> writeHandle { m_writeKey };
  if ( writeHandle.isValid() )
  {
    ATH_MSG_DEBUG( "CondHandle " << writeHandle.fullKey() << " is already valid " <<
		   "Forcing update of Store contents" );
    return StatusCode::SUCCESS;
  }

  EventIDRange rangeW;
  InDet::BeamSpotData* writeCdo = nullptr;

  if ( !m_useDB )
  {
    rangeW = alwaysValid;
    writeCdo = new InDet::BeamSpotData( m_status, m_posX, m_posY, m_posZ,
				        m_sigmaX, m_sigmaY, m_sigmaZ,
				        m_tiltX, m_tiltY, m_sigmaXY );
  }
  else
  {

    SG::ReadCondHandle<AthenaAttributeList> readHandle { m_readKey };
  
    const AthenaAttributeList* raw { *readHandle };
    if ( raw == nullptr )
    {
      ATH_MSG_ERROR("Beam Spot data for key " << m_readKey.fullKey() << " not found");
      return StatusCode::FAILURE;
    }
  
    int status { (*raw)["status"].data<int>() };
    float posX { (*raw)["posX"].data<float>() };
    float posY { (*raw)["posY"].data<float>() };
    float posZ { (*raw)["posZ"].data<float>() };
    float sigmaX { (*raw)["sigmaX"].data<float>() };
    float sigmaY { (*raw)["sigmaY"].data<float>() };
    float sigmaZ { (*raw)["sigmaZ"].data<float>() };
    float tiltX  { (*raw)["tiltX"].data<float>() };
    float tiltY  { (*raw)["tiltY"].data<float>() };
    float sigmaXY { m_sigmaXY };
    try
    {
      sigmaXY = (*raw)["sigmaXY"].data<float>();
    }
    catch (coral::AttributeListException& e)
    {
      ATH_MSG_DEBUG( "No sigmaXY value present in database, using default value of " << sigmaXY );
    }

    ATH_MSG_INFO( "Read from condDB"
               << " status " << status
	       << " pos (" << posX << "," << posY << "," << posZ << ")"
	       << " sigma (" << sigmaX << "," << sigmaY << "," << sigmaZ << ")"
	       << " tilt (" << tiltX << "," << tiltY << ")"
	       << " sigmaXY " << sigmaXY );

    if ( !readHandle.range(rangeW) )
    {
      ATH_MSG_ERROR( "Failed to retrieve validity range for " << readHandle.key() );
      return StatusCode::FAILURE;
    }

    writeCdo = new InDet::BeamSpotData( status, posX, posY, posZ,
				        sigmaX, sigmaY, sigmaZ,
					tiltX, tiltY, sigmaXY );
  }

  if ( writeHandle.record( rangeW, writeCdo ).isFailure() )
  {
    ATH_MSG_ERROR( "Could not record InDet::BeamSpotData " << writeHandle.key() <<
                   " with EventRange " << rangeW << " into conditions store." );
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_INFO( "Recorded new InDet::BeamSpotData to " << writeHandle.key() << " with range " << rangeW <<
		" into conditions store." );

  return StatusCode::SUCCESS;
}

