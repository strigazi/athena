/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigEgammaMatchingToolMTTest.h"
#include "DecisionHandling/TrigCompositeUtils.h"




TrigEgammaMatchingToolMTTest::TrigEgammaMatchingToolMTTest(const std::string& name,  ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm( name, pSvcLocator ),
  m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_matchTool("Trig::TrigEgammaMatchingTool/TrigEgammaMatchingToolMT",this)

{
  declareProperty("TrigEgammaMatchingToolMT",m_matchTool);
  declareProperty("TriggerList",m_triggerList);
}



//**********************************************************************

StatusCode TrigEgammaMatchingToolMTTest::initialize() {
    
  ATH_MSG_INFO("Initializing " << name() << "...");
  
  ATH_CHECK( m_trigdec.retrieve() );
  ATH_CHECK( m_matchTool.retrieve());
  ATH_CHECK(m_electronKey.initialize()); 
 
  
  
  return StatusCode::SUCCESS;
}



//**********************************************************************

void TrigEgammaMatchingToolMTTest::inspect(const std::string trigger,const xAOD::Egamma *eg) const {

  if (eg) {
    
    ATH_MSG_INFO("Getting all associated objects for " << trigger);
    
    const TrigCompositeUtils::Decision *dec=nullptr;
    
    m_matchTool->match( eg, trigger , dec);

    if ( dec )
    {
      ATH_MSG_INFO( "Matched!");


      auto *l1 = m_matchTool->getFeature<xAOD::EmTauRoI>(dec);
      auto *emCluster = m_matchTool->getFeature<xAOD::TrigEMCluster>( dec );
      auto trig_el_cont = m_matchTool->getFeatures<xAOD::TrigElectron>( dec );
      auto cl_cont = m_matchTool->getFeatures<xAOD::CaloCluster>( dec );
      auto el_cont = m_matchTool->getFeatures<xAOD::Electron>( dec );

      ATH_MSG_INFO( "L1 = " << l1 );
      ATH_MSG_INFO( "emCluster = "<< emCluster );
      ATH_MSG_INFO( "TrigElectron container size  is " << trig_el_cont.size());
      ATH_MSG_INFO( "CaloCluster container size  is " << cl_cont.size());
      ATH_MSG_INFO( "Electron container size  is " << el_cont.size());

      bool passedHLT    =  m_matchTool->ancestorPassed<xAOD::ElectronContainer> (dec, trigger , "HLT_egamma_Electrons");
      bool passedEFCalo =  m_matchTool->ancestorPassed<xAOD::CaloClusterContainer> (dec, trigger , "HLT_CaloEMClusters");
      bool passedL2     =  m_matchTool->ancestorPassed<xAOD::TrigElectronContainer> (dec, trigger , "HLT_L2Electrons");
      bool passedL2Calo =  m_matchTool->ancestorPassed<xAOD::TrigEMClusterContainer> (dec, trigger , "HLT_L2CaloEMClusters");
      bool passedL1Calo =  m_matchTool->ancestorPassed<xAOD::EmTauRoIContainer> (dec, trigger , "LVL1EmTauRoIs");
      
      ATH_MSG_INFO( "L1Calo passed : "<< passedL1Calo   );
      ATH_MSG_INFO( "L2Calo passed : "<< passedL2Calo   );
      ATH_MSG_INFO( "L2     passed : "<< passedL2       );
      ATH_MSG_INFO( "EFCalo passed : "<< passedEFCalo   );
      ATH_MSG_INFO( "HLT    passed : "<< passedHLT      );

    }else{
      ATH_MSG_INFO( "Not Matched! There is no Dec object for this trigger " << trigger );
    }

  }
  else ATH_MSG_INFO("REGTEST: eg pointer null!");
}


//**********************************************************************

StatusCode TrigEgammaMatchingToolMTTest::fillHistograms( const EventContext& ctx ) const {   
    
    ATH_MSG_INFO ("Fill histograms " << name() << "...");


    SG::ReadHandle<xAOD::ElectronContainer> el_cont(m_electronKey, ctx);

    if( !el_cont.isValid() ){
        ATH_MSG_WARNING("Container "<< m_electronKey << " does not exist or is empty");
        return StatusCode::SUCCESS;
    }


    ATH_MSG_INFO("Offline Electron container size " << el_cont->size());
    
    for(const auto &trigger : m_triggerList){
        for(const auto& eg : *el_cont){
            ATH_MSG_INFO("REGTEST:: Electron offline (eta="<<eg->eta()<<",phi="<<eg->phi()<<")");
            inspect(trigger,eg);

        } //End loop of offline electrons
    } // End loop over trigger list
    


    return StatusCode::SUCCESS;
} // End execute






