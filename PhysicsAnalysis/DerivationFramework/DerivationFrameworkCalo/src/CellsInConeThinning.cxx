/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloEvent/CaloCellLinkContainer.h"

#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"

#include "DerivationFrameworkCalo/CellsInConeThinning.h"
#include "CellsInCone.h"

DerivationFramework::CellsInConeThinning::CellsInConeThinning(const std::string& type, 
							      const std::string& name, 
							      const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_selectionString(""),
  m_dr(0.5),
  m_parser(0)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("deltaR",m_dr=0.5);		
}

StatusCode DerivationFramework::CellsInConeThinning::initialize(){

  ATH_CHECK(m_SGKey.initialize());
  ATH_CHECK(m_InputCellsSGKey.initialize());
  ATH_CHECK(m_OutputClusterSGKey.initialize());
  ATH_CHECK(m_OutputCellLinkSGKey.initialize());

  if (m_selectionString!="") {
    ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
    proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
    proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
    if (m_selectionString!="") {
      m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
      m_parser->loadExpression(m_selectionString);
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::CellsInConeThinning::finalize(){
  if (m_selectionString!="") {
    delete m_parser;
    m_parser = 0;
  }
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::CellsInConeThinning::addBranches() const{

  ///Make new container
  SG::WriteHandle<xAOD::CaloClusterContainer> dclHdl(m_OutputClusterSGKey);
  ATH_CHECK(CaloClusterStoreHelper::AddContainerWriteHandle(&(*evtStore()), dclHdl, msg()));
  /// Input objects
  SG::ReadHandle<xAOD::EgammaContainer> egHdl(m_SGKey);
  const xAOD::EgammaContainer *egammas = egHdl.cptr();
  if(!egammas ) {
    ATH_MSG_ERROR( "Couldn't retrieve egamma container with key: " <<m_SGKey);
    return StatusCode::FAILURE;
  }

  unsigned int nEgammas(egammas->size());
  if (nEgammas==0) {
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<CaloCellContainer> cellHdl(m_InputCellsSGKey);
  const CaloCellContainer* cells = cellHdl.cptr();
  if(!cells) {
      ATH_MSG_ERROR( "Couldn't retrieve cell container with key: " <<m_InputCellsSGKey);
      return StatusCode::FAILURE;
  }
  //We have a selection string 
  if (m_selectionString!="") {
    std::vector<int> entries =  m_parser->evaluateAsVector();
    unsigned int nEntries = entries.size();
    // check the sizes are compatible
    if (nEgammas!= nEntries ) {
      ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used e-gamma objects");
      return StatusCode::FAILURE;
    }
    //Loop over the egammas, pick the selected ones and fill the cluster
    int index(0);
    for (const xAOD::Egamma* eg : *egammas){
      if(entries.at(index)==true){
	xAOD::CaloCluster *dummy = CaloClusterStoreHelper::makeCluster(cells);
	DerivationFramework::CellsInCone::egammaSelect(dummy,cells,eg,m_dr);
	dclHdl->push_back(dummy);
      }
      ++index;
    }
  }
  /// Fill clusters without selection string
  else{
    for (const xAOD::Egamma* eg : *egammas){
      xAOD::CaloCluster *dummy = CaloClusterStoreHelper::makeCluster(cells);
      DerivationFramework::CellsInCone::egammaSelect(dummy,cells,eg,m_dr);
      dclHdl->push_back(dummy);
    }
  }
  ///Finalize clusters
  SG::WriteHandle<CaloClusterCellLinkContainer> cellLinks(m_OutputCellLinkSGKey);
  ATH_CHECK( CaloClusterStoreHelper::finalizeClusters(cellLinks, dclHdl.ptr()));
  ///Return
  return StatusCode::SUCCESS;
}
////

