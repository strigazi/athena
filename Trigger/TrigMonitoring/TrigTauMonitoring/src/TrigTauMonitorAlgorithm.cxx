/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauMonitorAlgorithm.h"

TrigTauMonitorAlgorithm::TrigTauMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{}


TrigTauMonitorAlgorithm::~TrigTauMonitorAlgorithm() {}


StatusCode TrigTauMonitorAlgorithm::initialize() {

  ATH_CHECK( m_offlineTauJetKey.initialize() );
  ATH_CHECK( m_l1TauRoIKey.initialize() );
  ATH_CHECK( m_hltTauJetKey.initialize() );
  ATH_CHECK( m_hltTauJetPrecisionKey.initialize() );
  ATH_CHECK( m_hltTauJetPreselKey.initialize() );
  ATH_CHECK( m_hltTauJetCaloOnlyMVAKey.initialize() );
  ATH_CHECK( m_hltTauJetCaloOnlyKey.initialize() );
  ATH_CHECK( m_trigDecTool.retrieve() );

  return AthMonitorAlgorithm::initialize();
}


StatusCode TrigTauMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
  //using namespace Monitored;

  ATH_MSG_DEBUG("Executing TrigTauMonitorAlgorithm");

  if(m_trigDecTool->ExperimentalAndExpertMethods()->isHLTTruncated()){
    ATH_MSG_WARNING("HLTResult truncated, skip trigger analysis");
    return StatusCode::SUCCESS; 
  }
  
  ATH_MSG_DEBUG("Chains for Analysis " << m_trigInputList);

  std::vector< std::pair<const xAOD::TauJet*, const TrigCompositeUtils::Decision*>> pairObjs;

  for(const auto& trigger : m_trigInputList){

    if ( executeNavigation( ctx, trigger,25,"mediumRNN", pairObjs).isFailure() )                                                            
    {                                                                                                                                                       
       ATH_MSG_WARNING("executeNavigation failed");                                                                                                       
       return StatusCode::SUCCESS;                                                                                                                         
    }  

    fillDistributions( pairObjs, trigger );                                                                                                           
    fillDistributions( pairObjs, trigger );
  }

    return StatusCode::SUCCESS;
}


StatusCode TrigTauMonitorAlgorithm::executeNavigation( const EventContext& ctx, 
                                                                  const std::string trigItem,
                                                                  float etthr,
                                                                  const std::string pidname,
                                                                  std::vector<std::pair<const xAOD::TauJet*, const TrigCompositeUtils::Decision* >> &pairObjs) const
{

  ATH_MSG_DEBUG("Apply navigation selection "); 

  SG::ReadHandle<xAOD::TauJetContainer> offTaus(m_offlineTauJetKey, ctx);

  const double thresholdOffset{10.0};

  if(!offTaus.isValid())
  {
     ATH_MSG_WARNING("Failed to retrieve offline Taus ");
     return StatusCode::FAILURE;
  }

  for(const auto& Tau : *offTaus ){

    const TrigCompositeUtils::Decision *dec=nullptr; 

    if( !( Tau->p4().Pt()  > (etthr-thresholdOffset)*1.e3) ) continue;

    if((pidname.find("mediumRNN") != std::string::npos) && !Tau->isTau(xAOD::TauJetParameters::JetRNNSigMedium)) continue;

    auto vec = m_trigDecTool->features<xAOD::TauJetContainer>(trigItem,TrigDefs::includeFailedDecisions ,"HLT_TrigTauRecMerged_MVA");      
    for( auto &featLinkInfo : vec ){                                             
      if(! featLinkInfo.isValid() ) continue;
      const auto *feat = *(featLinkInfo.link);                   
      if(!feat) continue;
      float deltaR = dR(Tau->eta(),Tau->phi(), feat->eta(),feat->phi());                                
      if(deltaR < 0.4){            
         dec = featLinkInfo.source;                                                
      }                     
    }


    std::pair< const xAOD::TauJet*, const TrigCompositeUtils::Decision * > pair(Tau,dec);
    pairObjs.push_back(pair);

  }

  ATH_MSG_DEBUG("BaseToolMT::Tau TEs " << pairObjs.size() << " found.");
  return StatusCode::SUCCESS;
}

void TrigTauMonitorAlgorithm::fillDistributions(std::vector< std::pair< const xAOD::TauJet*, const TrigCompositeUtils::Decision * >> pairObjs, std::string trigger) const
{

  // Offline
  std::vector<const xAOD::TauJet*> tau_vec;
  for( auto pairObj: pairObjs )
  {
    tau_vec.push_back(pairObj.first);
  }
    
  // Offline
  fillRNNInputVars( trigger, tau_vec, false );

}


void TrigTauMonitorAlgorithm::fillRNNInputVars(const std::string trigger, std::vector<const xAOD::TauJet*> tau_vec, bool online) const
{
  ATH_MSG_DEBUG("Fill RNN input variables: " << trigger);

  auto monGroup = getGroup(trigger+( online ? "_RNNInputVars_HLT" : "_RNNInputVars_Offline"));  

  auto centFrac           = Monitored::Collection("centFrac", tau_vec,  [] (const xAOD::TauJet* tau){
                                                    float detail = -999;
                                                    if (tau->detail(xAOD::TauJetParameters::centFrac, detail)){
                                                      detail = std::min(detail, 1.0f);    
                                                    } return detail;});
  auto etOverPtLeadTrk    = Monitored::Collection("etOverPtLeadTrk", tau_vec,  [] (const xAOD::TauJet* tau){
                                                    float detail = -999;
                                                    if (tau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, detail)){
                                                      detail = TMath::Log10(std::max(detail, 0.1f));
                                                    } return detail;});
  auto dRmax              = Monitored::Collection("dRmax", tau_vec,  [] (const xAOD::TauJet* tau){
                                                    float detail = -999;
                                                    if (tau->detail(xAOD::TauJetParameters::dRmax, detail)){
                                                    } return detail;});
  auto absipSigLeadTrk    = Monitored::Collection("absipSigLeadTrk", tau_vec,  [] (const xAOD::TauJet* tau){
                                                    float detail = -999;
                                                    if (tau->detail(xAOD::TauJetParameters::ipSigLeadTrk, detail)){
                                                      detail = std::min(TMath::Abs(detail), 30.0f);
                                                    } return detail;});
  auto sumPtTrkFrac       = Monitored::Collection("sumPtTrkFrac", tau_vec,  [] (const xAOD::TauJet* tau){
                                                    float detail = -999;
                                                    if (tau->detail(xAOD::TauJetParameters::SumPtTrkFrac, detail)){
                                                    } return detail;});
  auto emPOverTrkSysP     = Monitored::Collection("emPOverTrkSysP", tau_vec,  [] (const xAOD::TauJet* tau){
                                                    float detail = -999;
                                                    if (tau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, detail)){
                                                      detail = TMath::Log10(std::max(detail, 1e-3f));
                                                    } return detail;});
  auto ptRatioEflowApprox = Monitored::Collection("ptRatioEflowApprox", tau_vec,  [] (const xAOD::TauJet* tau){
                                                    float detail = -999;
                                                    if (tau->detail(xAOD::TauJetParameters::ptRatioEflowApprox, detail)){
                                                      detail = std::min(detail, 4.0f);
                                                    } return detail;});
  auto mEflowApprox       = Monitored::Collection("mEflowApprox", tau_vec,  [] (const xAOD::TauJet* tau){
                                                    float detail = -999;
                                                    if (tau->detail(xAOD::TauJetParameters::mEflowApprox, detail)){
                                                      detail = TMath::Log10(std::max(detail, 140.0f));
                                                    }return detail;});
  auto ptDetectorAxis     = Monitored::Collection("ptDetectorAxis", tau_vec,  [] (const xAOD::TauJet* tau){
                                                    return TMath::Log10(std::min(tau->ptDetectorAxis() / 1000.0, 100.0));});

  fill(monGroup, centFrac,etOverPtLeadTrk,dRmax,absipSigLeadTrk,sumPtTrkFrac,emPOverTrkSysP,ptRatioEflowApprox,mEflowApprox,ptDetectorAxis);
  
}

void TrigTauMonitorAlgorithm::fillEfficiencies(std::vector< std::pair< const xAOD::TauJet*, const TrigCompositeUtils::Decision * >> pairObjs, std::string trigger) const
{
   // here can apply additional selection on top of pair selection in TrigTauMonitorAlgorithm::executeNavigation  
   fillEfficiency( pairObjs, trigger );
}


void TrigTauMonitorAlgorithm::fillEfficiency( std::vector< std::pair< const xAOD::TauJet*, const TrigCompositeUtils::Decision * >> pairObjs, std::string trigger) const
{
   // can pass to this function also additional information we could get in TrigTauMonitorAlgorithm::fillEfficiencies
    auto monGroup = getGroup( trigger + "_Efficiency" );  

    std::vector<float> pt_vec, eta_vec, avgmu_vec;
    std::vector<float> match_pt_vec, match_eta_vec, match_avgmu_vec;
    std::vector<bool>  pt_passed_vec, eta_passed_vec, avgmu_passed_vec;

    auto pt_col     = Monitored::Collection( "pt"     , pt_vec );
    auto eta_col    = Monitored::Collection( "eta"    , eta_vec ); 

    auto match_pt_col     = Monitored::Collection( "match_pt"     , match_pt_vec );
    auto match_eta_col    = Monitored::Collection( "match_eta"    , match_eta_vec );

    auto pt_passed_col     = Monitored::Collection( "pt_passed"     , pt_passed_vec );
    auto eta_passed_col    = Monitored::Collection( "eta_passed"    , eta_passed_vec );

    for( auto pairObj : pairObjs ){
       const auto *tau = pairObj.first;
       float pt = tau->pt()/Gaudi::Units::GeV;

       pt_vec.push_back( pt );

      

    }

}

const std::vector<std::string> TrigTauMonitorAlgorithm::m_trigLevel = {"L1Calo","L2Calo","L2","EFCalo","EFTrack","HLT"};

asg::AcceptData TrigTauMonitorAlgorithm::setAccept( const TrigCompositeUtils::Decision *dec, std::string trigger) const {

    ATH_MSG_DEBUG("setAccept");

    asg::AcceptInfo accept;
    for(const auto& cut:m_trigLevel) accept.addCut(cut,cut);
    asg::AcceptData acceptData (&accept);    

    bool passedL1Calo=false;
    bool passedL2Calo=false;
    bool passedEFCalo=false;
    bool passedL2=false;
    bool passedEFTrk=false;
    bool passedEF=false;    

    //passedL1Calo = m_trigDecTool->ancestorPassed<TrigRoiDescriptorCollection>( dec , trigger , "initialRois");

    return acceptData;    

}
