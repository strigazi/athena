/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     FlowNetworkMatcherBase.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include "./FlowNetworkMatcherBase.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowNetwork.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FordFulkerson.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "./xAODJetCollector.h"
#include "./MultijetFlowNetworkBuilder.h"

#include <cmath>
#include <sstream>
#include <algorithm>
#include <map>

#include <iostream>

FlowNetworkMatcherBase::FlowNetworkMatcherBase(std::size_t nConditions) :
  m_nConditions(nConditions){
  // remainder of members initialised by derived classes.
}

void
FlowNetworkMatcherBase::setFlowNetworkBuilder(std::unique_ptr<IFlowNetworkBuilder> b){
  m_flowNetworkBuilder = std::move(b);
}

std::optional<bool>
FlowNetworkMatcherBase::match(const HypoJetGroupCIter& groups_b,
                                       const HypoJetGroupCIter& groups_e,
				       xAODJetCollector& jetCollector,
                                       const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
                                       bool) const {
  /* setup a FlowNetwork. This consists of a sink, a source, and alternating
     layers of nodes representing conditions and job groups. Nodes in the same
     layer are at the same distance from the source.
     leaf nodes are the last nodes before the sink. The job group layer
     above them consists of "external products" of the job groups
     satisfying the conditions layer below them. The exxternal product is
     sound by asking the parent node (above the currenrt job group layer)
     which children it is connected to. The job groups from the children
     are collected. All combinations of job groups, taking one per child,
     are merged into a single job group, and then passed to the parent,
     which decides whether or not it ia a satisfying job group. If there
     is satisfaction edges of the approoriate capacity  are established 
     between the parent and its children. The job group satisfiying the parent
     is noted. The process is iterated until the "first generation" 
     conditions are reached. If these are satisfied, they are connected to 
     the sink.
  */

  std::cerr<<"FlowNetworkMatcherBase::match 0\n";

  if(!m_nConditions){
    if(collector){
      collector->collect("FlowNetworkMatcherBase", "No conditions configured");
    }
    return std::make_optional<bool>(false);
  }
  std::cerr<<"FlowNetworkMatcherBase::match 100\n";
  
  // Determine jet group - condition satisfaction.

  auto iter_diff = groups_e - groups_b;  // number of jet groups
  if (iter_diff < 0){return std::optional<bool>();}  // must be postive

  /* Temp remove - 
  if(static_cast<std::size_t>(iter_diff) < m_nConditions) {
    if(collector){
      collector->collect("FlowNetworkMatcherBase",
			 "Too few jet groups " +
			 std::to_string(m_nConditions) + " " +
			 std::to_string(iter_diff));
    }
    return std::make_optional<bool>(false);
  }
  */
  std::cerr<<"FlowNetworkMatcherBase::match 200\n";

  std::map<int, pHypoJet> nodeToJet; 

  // auto edges = make_flowEdges(groups_b, groups_e, collector, V, nodeToJet);
  auto G = m_flowNetworkBuilder -> create(groups_b,
                                          groups_e,
                                          collector,
                                          nodeToJet);
  std::cerr<<"FlowNetworkMatcherBase::match 300\n";
  if(!G.has_value()){
    if(collector){collector -> collect("MaximumBipartiteGroupsMatcher",
				       "Network construction early return");}
    return std::make_optional<bool> (false);
  }
  std::cerr<<"FlowNetworkMatcherBase::match 400\n";

  
  if(collector){
    std::stringstream ss;
    ss << **G;
    collector->collect("MaximumBipartiteGroupsMatcher - before", ss.str());
  }
  
  std::cerr<<"FlowNetworkMatcherBase::match 500\n";
  std::cerr<<**G<<'\n';
  auto V = (*G)->V();
  FordFulkerson ff (**G, 0, V-1);
  if(ff.error()){
    if(collector){
      std::string msg("FordFulkerson error: ");
      msg += ff.errMsg() + "\n";
      collector -> collect("MaximumBipartiteGroupsMatcher", msg);
    }
    return std::optional<bool> ();
  }
  std::cerr<<"FlowNetworkMatcherBase::match 600\n";

  if(collector){
    std::stringstream ss;
    ss << **G;
    collector->collect("MaximumBipartiteGroupsMatcher - after", ss.str());
  }
  std::cerr<<"FlowNetworkMatcherBase::match 700\n";
  
  auto edges = (*G)->edges();
 
  bool pass = std::round(ff.value()) == m_totalCapacity;
  if(collector){
    std::string msg("FordFulkerson pass status: ");
    msg += pass ? "true" : "false";
    msg += " total capacity: " + std::to_string(m_totalCapacity); 
    collector -> collect("MaximumBipartiteGroupsMatcher", msg);
  }
  // loop over edges, figure out if it is a condition - jet edge
  // figure out if it the jet is used (flow == 1)
  // add the jet to passing_jets. As this is a set, duplicates
  // will be removed (each edge appears twice in G).
  
  std::cerr<<"FlowNetworkMatcherBase::match 800\n";
  HypoJetVector passing_jets;
  auto iter = std::partition(edges.begin(),
                             edges.end(),
                             [V](const auto& edge){return edge->to() == V-1 and
                                                   std::round(edge->flow()) == 1;});

  std::cerr<<"FlowNetworkMatcherBase::match 900\n";
  std::transform(edges.begin(),
                 iter,
                 std::back_inserter(passing_jets),
                 [&nodeToJet](const auto& edge){return nodeToJet[edge->from()];});


  std::cerr<<"FlowNetworkMatcherBase::match 1000\n";
  jetCollector.addJets(passing_jets.cbegin(), passing_jets.cend());
	       
  std::cerr<<"FlowNetworkMatcherBase::match 1100\n";
  
  return std::make_optional<bool>(pass);
  
}




