/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./FastReducer.h"
#include "./ITrigJetHypoInfoCollector.h"

#include <map>
#include <algorithm>
#include <sstream>

class DepthComparison{
public:
  DepthComparison(const Tree& t) : m_tree(t){}
  bool operator () (const std::size_t& lhs, const std::size_t rhs){
    return m_tree.depth(rhs) > m_tree.depth(lhs);
  }
private:
  Tree m_tree;
};


struct IndexVecComp{
  bool operator () (const std::vector<std::size_t>& lhs,
		    const std::vector<std::size_t>& rhs){
    if(lhs.size() < rhs.size()){return true;}
    if(rhs.size() < rhs.size()){return false;}
    
    for(std::size_t i = 0; i < lhs.size(); ++i){
      if(lhs[i] > rhs[i]){return false;}
    }

    return true;
  }

};


FastReducer::FastReducer(const HypoJetGroupCIter& groups_b,
			 const HypoJetGroupCIter& groups_e,
			 const ConditionsMT& conditions,
			 const Tree& tree,
			 const std::vector<std::vector<int>>& sharedConditions,
			 const Collector& collector):
  m_conditions(conditions),
  m_tree(tree),
  m_sharedConditions(std::move(sharedConditions))
{

  // create an empty list vector of indices of satisfying job groups
  // for each Condition.
  for(std::size_t i = 0; i < m_tree.size(); ++i){
    m_satisfiedBy.emplace(i, std::vector<std::size_t>{});
    m_testedBy.emplace(i, std::set<std::size_t>{});
  }


  for(const auto& leaves: m_sharedConditions){
    if(!findInitialJobGroups(leaves,
			     groups_b,
			     groups_e,
			     collector)){
      if(collector){
	collector->collect("FastReducer early return",
			   "from findInitialJobGroups");
	dumpDataStructures(collector);
      }
    }
  }

  
  if(!propagateJobGroups(collector)){
    // error propagating edges. e.g. unsatisfied condition
    if(collector){
      collector->collect("FastReducer early return",
			 "from propagateJobGroups");
      dumpDataStructures(collector);
    }
    return;  // early return, leave m_pass = false
  }

  // propagagattion to the top of the Conditions tree was successful.
  setPassingJets();
  if(collector){
    collector->collect("FastReducer early return",
		       "from propagateJobGroups");
    dumpDataStructures(collector);
  }
  m_pass = true;

}



void FastReducer::setPassingJets(){
  /*
   * return one of sets of jets that passes the hypo
   * This method will be modified once the interaction with the 
   * steering code is better understood.
   */
  

  std::size_t passingGroup = m_satisfiedBy[0][0];
  std::vector<std::size_t> passingElemGroups = m_jg2elemjgs[passingGroup];
  auto getJet = [indJetGroup = m_indJetGroup](std::size_t i){
    return indJetGroup.at(i)[0];};



  std::transform(passingElemGroups.begin(),
		 passingElemGroups.end(),
		 std::back_inserter(m_passingJets),
		 getJet);
}

bool FastReducer::findInitialJobGroups(const std::vector<int>& leaves,
				       const HypoJetGroupCIter& groups_b,
				       const HypoJetGroupCIter& groups_e,
				       const Collector& collector) {
  

  /*
    Will now test the incoming jet groups against the leaf conditions.
  */

  std::size_t ijg{0};
  for(auto iter = groups_b; iter != groups_e; ++iter){
    auto jg = *iter;
    
    if(jg.size() != 1){
      collector->collect("FastReducer", "No job groups");
      return false;
    }

    // if a jet group satisfies a condition, note the fact, and store it by index
    bool jg_used{false};
    
    auto cur_jg = m_jgIndAllocator(std::vector<std::size_t>{ijg});
    for(const auto& leaf: leaves){

      if (m_conditions[leaf]->isSatisfied(jg, collector)){
	  jg_used= true;
	  if(collector){recordJetGroup(cur_jg, jg, collector);}
	  // do the following for each satisfied condition ...
	  m_satisfiedBy[leaf].push_back(cur_jg);
	  m_testedBy[leaf].insert(cur_jg);
      }
    }
    if(jg_used){
      m_jg2elemjgs[cur_jg] =  std::vector<std::size_t>{cur_jg};
      m_indJetGroup.emplace(cur_jg, jg);
      ++ijg;
    }
  }
  
  if(collector){
    for(const auto& p : m_indJetGroup){
      recordJetGroup(p.first, p.second, collector);
    }
  }
  
  // check all leaf conditions are satisfied
  return
    static_cast<std::size_t>(std::count_if(m_satisfiedBy.begin(),
					   m_satisfiedBy.end(),
					   [](const auto& p){
					     return !p.second.empty();}))==
    leaves.size();
}



bool FastReducer::propagateJobGroups(const Collector& collector){
  
  
  // construct jet groups according from jet groups that pass child
  // conditions.
  // This method controls which nodes to process.
  // It checks whether all sibling nodes are processed.
  // if so, processing of the parent is delegated to propagate_()
  
  //The parent of the next condition to be processed
  // is found, and from the parent the condition's siblings are found,
  
  typedef std::priority_queue<std::size_t,
			      std::vector<std::size_t>,
			      DepthComparison> DepthQueue;

  auto comparator = DepthComparison(m_tree);
  DepthQueue to_process(comparator); // conditions to be processed.
  
  // keep track if a condition's sibling has been processed.
  std::vector<bool> checked(m_conditions.size(), false);

  // initialise the queue with satisfied leaf conditions indices.
  for(const auto& item : m_satisfiedBy){
    if(!(item.second.empty())){
      to_process.push(item.first);
    }
  }
  
  while(!to_process.empty()){
    auto k = to_process.top();
    to_process.pop();

    if(checked[k]){
      continue;
    }
    
    if(k == 0){
      // have propagated to the root node
      if(m_satisfiedBy.at(0).empty()){
	if(collector){
	  collector->collect("FastReducer",
			     "Condition node 0 fails");
	}
	return false;
      } else {
	return true; // event passes
      }
    }

    auto siblings = m_tree.siblings(k);
    for(const auto& s : siblings){
      checked[s] = true;
    }
     
    // check if combinations of groups satisfying children satisfy their parent
    if(!propagate_(k,
		   siblings,
		   collector)){
      return false;
    }
    
    std::size_t par =  m_tree.parent(k);
    to_process.push(par);
  }
  return true;
}


bool FastReducer::propagate_(std::size_t child,
			     const std::vector<std::size_t>& siblings,
			     const Collector& collector){

  // all combinations of the jet groups passing the sibings are
  // constructed. One by one these combinations are tested for
  // parent satisfaction. Edges are contructed between satisfying job groups and the parent.
  // if any such edge is constructed, the calling rroutine is notified so it
  // can scheduling processing the parent as a child.


  std::size_t par =  m_tree.parent(child);

  // par == true not an error, but not processed
  if(child == 0){return true;}

  bool par_satisfied{false};  
      
  // calculate the external product of the jet groups
  // eg if condition c1 is satisfied by jg11 and jg12, while its only
  // sibling c2 is satisfied by jg21, the external jet groups are
  // jg11jg21, jg12jg21. Each of these  are flattened.
  
  auto jg_product = JetGroupProduct(siblings, m_satisfiedBy);
  // Note: while not siblings have been processed, next() will fail due to empty group vectors
  auto next = jg_product.next();

  // step through the job groups found by combining ghe child groups
  // check ecach combination to see if it satisfies the parent. If so
  // add an edge from the contributing children, and from the new job group to the parent.

  while (next.has_value()){
    
    auto jg_indices = *next;
    std::vector<std::size_t> elem_jgs;
    
    for(auto i : jg_indices){
      elem_jgs.insert(elem_jgs.end(),
			m_jg2elemjgs[i].begin(),
		      m_jg2elemjgs[i].end());
    }
    
    std::set<std::size_t> unique_indices(elem_jgs.begin(),
					 elem_jgs.end());
    if(unique_indices.size() != elem_jgs.size()){
      next = jg_product.next();
      continue;
    }

    HypoJetVector jg;

    auto cur_jg = m_jgIndAllocator(elem_jgs);
    if(m_testedBy[par].find(cur_jg) != m_testedBy[par].end()){
      next = jg_product.next();
      continue;
    }
    m_testedBy[par].insert(cur_jg);
	
    for(const auto& i : elem_jgs){
      jg.push_back(m_indJetGroup.at(i)[0]);  // why [0]? assume elemental jg has size 1
    }

    if (m_conditions[par]->isSatisfied(jg, collector)){// par is a tree ind.

      // get an index for this set of elementarys jhob groups indices
      par_satisfied = true;
      m_satisfiedBy[par].push_back(cur_jg);

      m_jg2elemjgs[cur_jg] = elem_jgs;
      if(collector){recordJetGroup(cur_jg, jg, collector);}
    }
    
    next = jg_product.next();
  }

  if(collector and !par_satisfied){
    collector->collect("FastReducer",
		       "Condition node " + std::to_string(par) +
		       " unsatisfied");
  }
  
  return par_satisfied;
}



std::string FastReducer::toString() const {
  std::stringstream ss;
  ss << "FastReducer:\n";
  ss << "  treeVector: " << m_tree << '\n';;
  ss << "  shared node sets [" << m_sharedConditions.size() << "]:\n";
  for(const auto& snodelist : m_sharedConditions){
    for(const auto el : snodelist){
      ss << el << " ";
    }
    ss << '\n';
  }

  ss << "FastReducer Conditions ["
     << m_conditions.size() << "]: \n";

  std::size_t count{0u};
  for(const auto& c : m_conditions){
    auto sc = std::to_string(count++);
    sc.insert(sc.begin(), 3-sc.length(), ' ');
    ss << sc <<": "<< c->toString() + '\n';
  }

  return ss.str();
}


std::string FastReducer::dataStructuresToStr() const {

   std::stringstream ss;
  ss << "FastReducer data structure dumps\nindJetGroup:\n";
  for(const auto& pair : m_indJetGroup){
    ss << pair.first << " [";
    for(const auto& j : pair.second){
      ss << static_cast<const void*>(j) << " ";
    }
    ss << "]\n";
  }

  ss << "satisfiedBy: \n";
  for(const auto& pair : m_satisfiedBy){
    ss << pair.first << " [";
    for(const auto& i : pair.second){
      ss << i << " ";
    }
    ss << "]\n";
  }

  
  ss << "testedBy: \n";
  for(const auto& pair : m_testedBy){
    ss << pair.first << " [";
    for(const auto& i : pair.second){
      ss << i << " ";
    }
    ss << "]\n";
  }
  
  ss << "jg to elemental jgs: \n";
  
  for(const auto& pair : m_jg2elemjgs){
    ss << pair.first << " [";
    for(const auto& i : pair.second){
      ss << i << " ";
    }
    ss << "]\n";
  }
  ss <<'\n';

  return ss.str();
 }

 void  FastReducer::dumpDataStructures(const Collector& collector) const {
   
   if(!collector){return;}
   
   collector->collect("FastReducer",
		      dataStructuresToStr());
 }


void FastReducer::recordJetGroup(std::size_t ind,
				 const HypoJetVector& jg,
				 const Collector& collector) const {
  
  std::stringstream ss0;
  ss0  << "FastReducer jet group "
       << ind << " [" << jg.size() <<"]:";
  
  std::stringstream ss1;
  for(auto ip : jg){
    const void* address = static_cast<const void*>(ip);
    ss1  << "\n "  << address << " eta " << ip->eta()
	 << " e " << ip->e()
	 << " et " << ip->et();
  }
  ss1 << '\n';
  collector -> collect(ss0.str(), ss1.str());
}

std::optional<HypoJetVector>
FastReducer::passingJets() const {
  if(m_pass){
    return std::make_optional<HypoJetVector>(m_passingJets);
  }
  return std::optional<HypoJetVector>();
}