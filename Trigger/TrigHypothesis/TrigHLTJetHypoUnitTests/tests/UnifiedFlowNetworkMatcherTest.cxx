/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/../src/UnifiedFlowNetworkMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/../src/ConditionsDefsMT.h"
#include "TrigHLTJetHypo/../src/conditionsFactoryMT.h"
#include "TrigHLTJetHypo/../src/DebugInfoCollector.h"
#include "TrigHLTJetHypo/../src/xAODJetCollector.h"
#include "TrigHLTJetHypo/../src/UnifiedFlowNetworkBuilder.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGrouper.h"

#include "../src/TLorentzVectorFactory.h"
#include "../src/makeHypoJets.h"
#include "gtest/gtest.h"

#include <TLorentzVector.h>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>

#include <iostream>
/*
 * UnifiedFlowNetworkMatcher functionality tested:
 * 0 fail if no jet vector indices
 * 1 fail if no if there are fewer passing jets than conditions
 * 2 pass if there are at least as many passing jets as conditions
 * 3 conditions are ordered by threshold
 * 4 jets are ordered by Et
 *
 */

class UnifiedFlowNetworkMatcherTest: public ::testing::Test {
public:
  UnifiedFlowNetworkMatcherTest() {
  }

  ConditionsMT m_conditions;
  int m_nconditions;
  bool m_debug{false};
  
  HypoJetGroupVector makeJetGroupsMT(HypoJetIter b,
				     HypoJetIter e,
				     std::size_t n){
    CombinationsGrouper g(n);  // 
    return g.group(b, e)[0];
  }

  void makeConditions_1(const std::vector<double>& etaMins,
			const std::vector<double>& etaMaxs){


    auto netas = etaMins.size();
    
    std::vector<double> thresholds(netas, 1.0);
    std::vector<int> asymmetricEtas(netas, 0);

    m_conditions = conditionsFactoryEtaEtMT(etaMins, etaMaxs,
                                            thresholds,
                                            asymmetricEtas);
    m_nconditions = m_conditions.size();
  }
  
  
  void makeConditions_2(const std::vector<double>& detaMins,
			const std::vector<double>& detaMaxs){
    std::vector<double> massMins{0., 0.};
    std::vector<double> massMaxs{
      std::numeric_limits<double>::max(),
        std::numeric_limits<double>::max(),
        };


    std::vector<double> dphiMins{0., 0.};
    std::vector<double> dphiMaxs{
      std::numeric_limits<double>::max(),
        std::numeric_limits<double>::max(),
        };


    m_conditions = conditionsFactoryDijetMT(massMins, massMaxs,
                                            detaMins, detaMaxs,
                                            dphiMins, dphiMaxs);
    m_nconditions = m_conditions.size();
  }
};


TEST_F(UnifiedFlowNetworkMatcherTest, mj_flowNetworkBuilder_0){
   /* j0 -> c0  
      j0  -> c1                        

       Fails - j0 is shared.  

       treeVec is {0,0,0} (two condition nodes connected to the source node

 */

  auto out = std::make_unique<std::ofstream>(nullptr);
  if (m_debug){out.reset(new std::ofstream("mj_flowNetworkBuilder_0.log"));}

  std::vector<double> etaMins{0.0, 0.5};
  std::vector<double> etaMaxs{1.0, 1.5};

  makeConditions_1(etaMins, etaMaxs);


  if(out){
    for(const auto& c : m_conditions){*out << c->toString();}
  }


  std::vector<double> etas{0.7, 2.0, 3.0, 4.0};
  EXPECT_TRUE(etas.size() == 4);

    
  auto jets = makeHypoJets(etas);

  EXPECT_TRUE(jets.size() == 4);
  if(m_debug){
    for(const auto & j: jets){*out<< j << " " << j->toString() <<'\n';}
  }
  EXPECT_TRUE(m_conditions.size() == 2);

  std::vector<std::size_t> treeVec{0,0,0};
  std::map<int, pHypoJet> nodeToJet;

  auto groups = makeJetGroupsMT(jets.begin(), jets.end(), 1u);
  EXPECT_TRUE(groups.size() == 4);

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>();

  collector.reset(new DebugInfoCollector("mj_flowNetworkBuilder_0"));   

  xAODJetCollector j_collector;
  std::unique_ptr<ITrigJetHypoInfoCollector> d_collector(nullptr);
  
  std::unique_ptr<IGroupsMatcherMT> matcher(nullptr);
  matcher.reset(new UnifiedFlowNetworkMatcher(std::move(m_conditions),
					      treeVec));
  std::cerr<< "UnifiedFlowNetworkMatcherTest, mj_flowNetworkBuilder_0 800\n";
  bool pass = *(matcher->match(groups.cbegin(), groups.cend(),
  j_collector, d_collector));
  
  
  std::cerr<< "UnifiedFlowNetworkMatcherTest, mj_flowNetworkBuilder_0 900\n";
  // bool pass {false};
  EXPECT_FALSE(pass);

  for(auto j : jets){delete j;}
}

// TEST_F(UnifiedFlowNetworkMatcherTest, mj_flowNetworkBuilder_1){
//   /* (j0, j1) -> c0
//     (j0, j1) -> c1
//     (j3, j4) -> c1K
//      Passes.  
//   */
// 
//   auto out = std::make_unique<std::ofstream>(nullptr);
//   if (m_debug){out.reset(new std::ofstream("mj_flowNetworkBuilder_1.log"));}
// 
//   std::vector<double> detaMins{0., 0.};
// 
//   std::vector<double> detaMaxs{1.0, 2.0};
//   makeConditions(detaMins, detaMaxs);
// 
// 
//   if(out){
//     for(const auto& c : m_conditions){*out << c->toString();}
//   }
// 
//   std::vector<double> etas{-5.0, -4.9, 3.9, 5.0};
//   EXPECT_TRUE(etas.size() == 4);
// 
//     
//   auto jets = makeHypoJets(etas);
// 
//   EXPECT_TRUE(jets.size() == 4);
//   if(m_debug){
//     for(const auto & j: jets){*out<<j<< " " << j->toString() <<'\n';}
//   }
//   EXPECT_TRUE(m_conditions.size() == 2);
// 
//   std::unique_ptr<IFlowNetworkBuilder> builder = 
//     std::make_unique<UnifiedFlowNetworkBuilder>(std::move(m_conditions));
// 
//   std::map<int, pHypoJet> nodeToJet;
// 
//   auto groups = makeJetGroupsMT(jets.begin(), jets.end());
//   EXPECT_TRUE(groups.size() == 6);
//   std::unique_ptr<ITrigJetHypoInfoCollector> collector =
//     std::make_unique<DebugInfoCollector>("mj_flowNetworkBuilder_1_collector");
// 
//   auto G = builder->create(groups.begin(), groups.end(), collector, nodeToJet);
//   EXPECT_TRUE(G.has_value());
// 
//   if(m_debug){
//     std::stringstream ss;
//     ss << **G << '\n'; 
//     collector->collect("FlowNetwork", ss.str());
//     collector->write();
//   }
// 
//   EXPECT_TRUE((*G)->V() == 14);
//   EXPECT_TRUE(((*G)->edges()).size() == 13);
// 
//   for(auto j : jets){delete j;}
// }
// 
// TEST_F(UnifiedFlowNetworkMatcherTest, mj_flowNetworkBuilder_2){
//   /* (j0, j1) -> c0    
//     (j0, j1) -> c1
//        (j3, j4) -> c1
//        Passes.
//      Test with no collector                      
//   */
// 
//   auto out = std::make_unique<std::ofstream>(nullptr);
//   if (m_debug){out.reset(new std::ofstream("mj_flowNetworkBuilder_2.log"));}
// 
//   std::vector<double> detaMins{0., 0.};
// 
//   std::vector<double> detaMaxs{1.0, 2.0};
//   makeConditions(detaMins, detaMaxs);
// 
// 
//   if(out){
//     for(const auto& c : m_conditions){*out << c->toString();}
//   }
// 
//   std::vector<double> etas{-5.0, -4.9, 3.9, 5.0};
//   EXPECT_TRUE(etas.size() == 4);
//   
//   auto jets = makeHypoJets(etas);
// 
//   EXPECT_TRUE(jets.size() == 4);
//   if(m_debug){
//     for(const auto & j: jets){*out<<j<< " " << j->toString() <<'\n';}
//   }
//   EXPECT_TRUE(m_conditions.size() == 2);
// 
//   std::unique_ptr<IFlowNetworkBuilder> builder =
//     std::make_unique<UnifiedFlowNetworkBuilder>(std::move(m_conditions));
//   std::map<int, pHypoJet> nodeToJet;
// 
//   auto groups = makeJetGroupsMT(jets.begin(), jets.end());
//   EXPECT_TRUE(groups.size() == 6);
//   auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>();
//   // collector.reset(new DebugInfoCollector("mj_flowNetworkBuilder_2_collector);
// 
//   auto G = builder->create(groups.begin(), groups.end(), collector, nodeToJet);
//   EXPECT_TRUE(G.has_value());
// 
//   EXPECT_TRUE((*G)->V() == 14);
//   EXPECT_TRUE(((*G)->edges()).size() == 13);
// 
//   for(auto j : jets){delete j;}
// }
// 
// 
// TEST_F(UnifiedFlowNetworkMatcherTest, mj0){
//   /* (j0, j1) -> c0
//      (j0, j2) -> c0
//      (j0, j1) -> c1
//      Fails - j0 is shared.
// 
//      treeVec is {0,0,0} (two condition nodes connected to the source node
//   */
// 
//   auto out = std::make_unique<std::ofstream>(nullptr);
//   if (m_debug){out.reset(new std::ofstream("Unified_mj0.log"));}
// 
//   std::vector<double> detaMins{3.6, 5.5};
//   
//   std::vector<double> detaMaxs{
//     std::numeric_limits<double>::max(),
//       std::numeric_limits<double>::max(),
//       };
//   makeConditions(detaMins, detaMaxs);
// 
// 
//   if(out){
//     for(const auto& c : m_conditions){*out << c->toString();}
//   }
//   std::vector<double> etas{-5.0, 1.0, -1.0, -2.5};
//   EXPECT_TRUE(etas.size() == 4);
//   
//   auto jets = makeHypoJets(etas);
// 
//   if(m_debug){
//     for(const auto & j: jets){*out<< j << " " << j->toString() <<'\n';}
//   }
//    EXPECT_TRUE(jets.size() == 4);
// 
//   
//   EXPECT_TRUE(m_conditions.size() == 2);
//   
//   auto matcher = std::unique_ptr<IGroupsMatcherMT>(nullptr);
//   auto treeVec = std::vector<std::size_t>{0,0,0}; 
//   matcher.reset(new UnifiedFlowNetworkMatcher(std::move(m_conditions),
// 					      treeVec));
// 
//   /*
//   auto groups = makeJetGroupsMT(jets.begin(), jets.end());
//   EXPECT_TRUE(groups.size() == 6);
//   auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>();
//   collector.reset(new DebugInfoCollector("mj0"));
// 
//   xAODJetCollector jetCollector;
//   
//   auto pass = matcher->match(groups.begin(),
// 			     groups.end(),
// 			     jetCollector,
// 			     collector);
//   
//   if (m_debug){collector->write();}
// 
//   for(auto j : jets){delete j;}
// 
//   EXPECT_TRUE(jetCollector.empty());
//   */
//   bool xpass{false};
//   bool* pass = &xpass;
//   EXPECT_FALSE(*pass);
// }
// 
// 
// TEST_F(UnifiedFlowNetworkMatcherTest, mj1){
//   /* (j0, j3) -> c0
//      (j0, j1) -> c1
//      Fails - shared j0.
//      treeVec is {0,0,0} (two condition nodes connected to the source node
// 
//   */
// 
//   auto out = std::make_unique<std::ofstream>(nullptr);
//   if (m_debug){out.reset(new std::ofstream("Unified_mj1.log"));}
// 
//   std::vector<double> detaMins{3.6, 5.5};
//   
//   std::vector<double> detaMaxs{4.5, 6.5};
//   makeConditions(detaMins, detaMaxs);
// 
// 
//   if(out){
//     for(const auto& c : m_conditions){*out << c->toString();}
//   }
// 
//   std::vector<double> etas{-5.0, 1.0, -1.0, -2.5};
//   EXPECT_TRUE(etas.size() == 4);
//   
//   auto jets = makeHypoJets(etas);
// 
//   EXPECT_TRUE(jets.size() == 4);
//   if(m_debug){
//     for(const auto & j: jets){*out<<j<< " " << j->toString() <<'\n';}
//   }
//   EXPECT_TRUE(m_conditions.size() == 2);
// 
//   auto treeVec = std::vector<std::size_t>{0,0,0}; 
//   auto matcher = std::unique_ptr<IGroupsMatcherMT>(nullptr);
//   matcher.reset(new UnifiedFlowNetworkMatcher(std::move(m_conditions),
// 					      treeVec));
// 
//   auto groups = makeJetGroupsMT(jets.begin(), jets.end());
//   EXPECT_TRUE(groups.size() == 6);
//   auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>();
// 
//   if(m_debug){
//     collector.reset(new DebugInfoCollector("mj1"));
//   }
// 
//   xAODJetCollector jetCollector;
// 
//   auto pass = matcher->match(groups.begin(),
// 			     groups.end(),
// 			     jetCollector,
// 			     collector,
// 			     m_debug);
// 
//   if(m_debug){collector->write();}
//   
//   for(auto j : jets){delete j;}
// 
//   EXPECT_TRUE(jetCollector.empty());
//   EXPECT_FALSE(*pass);
// }
// 
// 
// TEST_F(UnifiedFlowNetworkMatcherTest, mj2){
//   /* (j0, j1) -> c0
//      (j3, j4) -> c1
//      Passes 
// 
//      treeVec is {0,0,0} (two condition nodes connected to the source node
//   */
// 
//   auto out = std::make_unique<std::ofstream>(nullptr);
//   if (m_debug){out.reset(new std::ofstream("Unified_mj2.log"));}
// 
//   std::vector<double> detaMins{0.0, 1.0};
//   
//   std::vector<double> detaMaxs{1.5, 2.0};
//   makeConditions(detaMins, detaMaxs);
// 
// 
//   if(out){
//     for(const auto& c : m_conditions){*out << c->toString();}
//   }
// 
//   std::vector<double> etas{-5.0, -4.9, 5.0, 3.25};
//   EXPECT_TRUE(etas.size() == 4);
//    
//   auto jets = makeHypoJets(etas);
//   
//   EXPECT_TRUE(jets.size() == 4);
//   if(m_debug){
//     for(const auto & j: jets){*out<<j<< " " << j->toString() <<'\n';}
//   }
//   EXPECT_TRUE(m_conditions.size() == 2);
// 
//   auto treeVec = std::vector<std::size_t>{0,0,0}; 
//   auto matcher = std::unique_ptr<IGroupsMatcherMT>(nullptr);
//   matcher.reset(new UnifiedFlowNetworkMatcher(std::move(m_conditions),
// 					      treeVec));
// 
//     auto groups = makeJetGroupsMT(jets.begin(), jets.end());
//   EXPECT_TRUE(groups.size() == 6);
//   auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);
// 
//   if(m_debug){
//     collector.reset(new DebugInfoCollector("mj2"));
//   }
// 
//   xAODJetCollector jetCollector;
// 
//   auto pass = matcher->match(groups.begin(),
// 			     groups.end(),
// 			     jetCollector,
// 			     collector,
// 			     m_debug);
// 
//   if(m_debug){collector->write();}
//   
//   for(auto j : jets){delete j;}
// 
//   EXPECT_TRUE(jetCollector.empty()); // not xAOD jets
//   
//   EXPECT_TRUE(*pass);
// }
// 
// 
// TEST_F(UnifiedFlowNetworkMatcherTest, mj3){
//   /* (j0, j1) -> c0
//      (j0, j1) -> c1
//      (j3, j4) -> c1
//      Passes.
// 
//      treeVec is {0,0,0,} (two condition nodes connected to the source node
//   */
// 
//   auto out = std::make_unique<std::ofstream>(nullptr);
//   if (m_debug){out.reset(new std::ofstream("Unified_mj3.log"));}
// 
//   std::vector<double> detaMins{0., 0.};
//   
//   std::vector<double> detaMaxs{1.0, 2.0};
//   makeConditions(detaMins, detaMaxs);
// 
// 
//   if(out){
//     for(const auto& c : m_conditions){*out << c->toString();}
//   }
// 
//   std::vector<double> etas{-5.0, -4.9, 4.0, 5.0};
//   EXPECT_TRUE(etas.size() == 4);
// 
//   auto jets = makeHypoJets(etas);
// 
//   EXPECT_TRUE(jets.size() == 4);
// 
//   if(m_debug){
//     for(const auto & j: jets){*out<<j<< " " << j->toString() <<'\n';}
//   }
//   EXPECT_TRUE(m_conditions.size() == 2);
// 
//   auto treeVec = std::vector<std::size_t>{0,0,0}; 
// 
//   auto matcher = std::unique_ptr<IGroupsMatcherMT>(nullptr);
//   matcher.reset(new UnifiedFlowNetworkMatcher(std::move(m_conditions),
// 					      treeVec));
//   
//     auto groups = makeJetGroupsMT(jets.begin(), jets.end());
//   EXPECT_TRUE(groups.size() == 6);
//   auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>();
//   collector.reset(new DebugInfoCollector("mj3"));
// 
//   xAODJetCollector jetCollector;
// 
//   auto pass = matcher->match(groups.begin(),
// 			     groups.end(),
// 			     jetCollector,
// 			     collector);
// 
//   if(m_debug){collector->write();}
//   
//   for(auto j : jets){delete j;}
//   EXPECT_TRUE(jetCollector.empty()); //not xAOD jets
//   EXPECT_TRUE(*pass);
// }
// 
// 
// TEST_F(UnifiedFlowNetworkMatcherTest, mj4){
//   /* (j0, j1) -> c0
//      (j0, j1) -> c1
//      (j3, j4) -> c0
//      (j3, j4) -> c1
// 
//      Passes.
// 
//      treeVec is {0,0,0} (two condition nodes connected to the source node
// 
//   */
// 
//   auto out = std::make_unique<std::ofstream>(nullptr);
//   if (m_debug){out.reset(new std::ofstream("Unified_mj4.log"));}
// 
//   std::vector<double> detaMins{0., 0.};
//   
//   std::vector<double> detaMaxs{1.0, 2.0};
//   makeConditions(detaMins, detaMaxs);
// 
// 
//   if(out){
//     for(const auto& c : m_conditions){*out << c->toString();}
//   }
// 
//   std::vector<double> etas{-5., -4.9, 4.9, 5.};
//   EXPECT_TRUE(etas.size() == 4);
// 
//   auto jets = makeHypoJets(etas);
//   
//   EXPECT_TRUE(jets.size() == 4);
//   if(m_debug){
//     for(const auto & j: jets){*out<<j<< " " << j->toString() <<'\n';}
//   }
// 
//   
//   EXPECT_TRUE(m_conditions.size() == 2);
// 
//   auto treeVec = std::vector<std::size_t>{0,0,0}; 
//   auto matcher = std::unique_ptr<IGroupsMatcherMT>(nullptr);
//   matcher.reset(new UnifiedFlowNetworkMatcher(std::move(m_conditions),
// 					      treeVec));
// 
//   auto groups = makeJetGroupsMT(jets.begin(), jets.end());
//   EXPECT_TRUE(groups.size() == 6);
//   auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>();
//   
//   if(m_debug){
//     collector.reset(new DebugInfoCollector("mj4"));
//   }
// 
//   xAODJetCollector jetCollector;
//   auto pass = matcher->match(groups.begin(),
// 			     groups.end(),
// 			     jetCollector,
// 			     collector,
// 			     m_debug);
// 
//   if(m_debug){collector->write();}
//   
//   for(auto j : jets){delete j;}
// 
//   EXPECT_TRUE(jetCollector.empty()); // not xAOD jets
//   EXPECT_TRUE(*pass);
// }
// 
