/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */

 /*
 *  NAME:     HLTCaloCellSumMaker.cxx
 *  PACKAGE:  Trigger/TrigDataAccess/TrigCaloDataAccessSvc
 *
 *  AUTHOR:   Denis Oliveira Damazio
 *
 *  PURPOSE:  New Algorithm to produce CaloCellContainer Vector
 *
 **/

#include "HLTCaloCellSumMaker.h"

HLTCaloCellSumMaker::HLTCaloCellSumMaker(const std::string & name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator),
    m_cellContainerV("CaloClusters"),
    m_roiMode(true)
{
    declareProperty("CellsVName", m_cellContainerVKey = std::string("CellsVClusters"), "Calo cluster container");
    declareProperty("CellsName", m_cellContainerKey = std::string("CellsClusters"), "Calo cluster container");
    declareProperty("roiMode",m_roiMode,"RoiMode roi->CaloCellCollection");
}

HLTCaloCellSumMaker::~HLTCaloCellSumMaker()
{
}

StatusCode HLTCaloCellSumMaker::initialize() {
	if ( m_roiMode )
        ATH_CHECK( m_cellContainerKey.initialize() );
	else
        ATH_CHECK( m_cellContainerVKey.initialize() );
        return StatusCode::SUCCESS;
}

StatusCode HLTCaloCellSumMaker::execute_r( const EventContext& /*context*/ ) const {


  if ( m_roiMode ) {
  int cc=0;
  // datahandle
  auto roisHandle = SG::makeHandle( m_cellContainerKey );
  const CaloCellContainer* roiCollection = roisHandle.cptr();
  if ( !roiCollection ){
  	ATH_MSG_INFO( "empty container"); 
	return StatusCode::SUCCESS;
  }
  float sum=0.;
  for(auto c : *roiCollection ) {sum+=c->et(); cc++;}
  ATH_MSG_INFO ( "Executing " << name() << "..." << "RoIToSum Single : " << cc << " " << sum );

  } else {
  auto roisHandle = SG::makeHandle( m_cellContainerVKey );
  const ConstDataVector<CaloCellContainerVector>* roiCollection = roisHandle.cptr();

  int cc=0;
  // datahandle
  for(const CaloCellContainer* roiDescriptor : *roiCollection ) {
	if ( roiDescriptor->size() > 0 ) {
	float sum=0.;
	for(auto c : *roiDescriptor ) sum+=c->et();
	ATH_MSG_INFO ( "Executing " << name() << "..." << "RoIToSum : " << cc++ << " " << roiDescriptor->size() << " " << roiDescriptor->at(0)->eta() << " " << sum );
	} else { ATH_MSG_INFO( "empty container"); }

  }
  }
  return StatusCode::SUCCESS;
}
