/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration 
*/

#ifndef ITrigBtagEmulationTool_H_
#define ITrigBtagEmulationTool_H_

#include "AsgTools/IAsgTool.h"
#include "xAODJet/JetContainer.h"

namespace Trig {

  class ITrigBtagEmulationTool : public virtual asg::IAsgTool {
    ASG_TOOL_INTERFACE(Trig::ITrigBtagEmulationTool)      

    public:
      virtual bool isPassed(const std::string&)=0;
      virtual const xAOD::JetContainer* retaggedJets(const std::string&) const=0;
      virtual std::vector< std::vector< std::string > > definition(const std::string&) const=0;
  };

}
#endif