/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_ANYTOPSEUDOJET_H
#define TRIGHLTJETREC_ANYTOPSEUDOJET_H

#include <string>
#include "JetEDM/IndexedConstituentUserInfo.h"
#include "JetEDM/LabelIndex.h"

/*Function object to convert input objects (clusters, jets) 
to fastjet::Pseudojets in order that they can be used as input to the fast
jet algorithms.
The pseudojets keep track of the clusters, to allow detector
specific caculation (eg jet cleaning) once the jets have been found */


namespace jet{
  class LabelIndex;
}

namespace fastjet{
  class PseudoJet;
}

template<class T>
class AnyToPseudoJet {
 public:

  /* No special assigment operator for copy constructor, or destructor
   as the LableIndex is deleted by the caller */
  
 AnyToPseudoJet(jet::LabelIndex* index_map):m_indexMap(index_map){}
  
  // T is decided by the caller.When using a DataVector<T> dv  and 
  // AnyToPSeudoJet  is used in an STL Algorithm, set T to dv::const_value_type

  fastjet::PseudoJet operator() (T cluster) const {
    fastjet::PseudoJet psj(cluster->p4());
    jet::IConstituentUserInfo* pcui = new jet::IndexedConstituentUserInfo(*cluster,
                                                                     1,
                                                                     m_indexMap);
    psj.set_user_info(pcui);
    return psj;
  }
  
 private:
  
  jet::LabelIndex* m_indexMap;
};
#endif
