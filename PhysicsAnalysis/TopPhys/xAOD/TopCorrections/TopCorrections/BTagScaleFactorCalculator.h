/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTagScaleFactorCalculator.h 754162 2016-06-10 15:01:10Z tpelzer $
#ifndef ANALYSISTOP_TOPCORRECTIONS_BTAGSCALEFACTORCALCULATOR_H
#define ANALYSISTOP_TOPCORRECTIONS_BTAGSCALEFACTORCALCULATOR_H

/**
  * @author Andrea Knue <aknue@cern.ch>,  John Morris <john.morris@cern.ch>
  * 
  * @brief BTagScaleFactorCalculator
  *   Calculate all jet scale factors and decorate
  * 
  * $Revision: 
  * $Date: 2016-06-10 17:01:10 +0200 (Fri, 10 Jun 2016) $
  * 
  **/ 

// system include(s):
#include <memory>
#include <set>

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Systematic include(s):
#include "PATInterfaces/SystematicSet.h"

#include "xAODBTaggingEfficiency/IBTaggingEfficiencyTool.h"
#include "xAODBTaggingEfficiency/IBTaggingSelectionTool.h"

// Forward declaration(s):
namespace top{
  class TopConfig;
}

namespace top{

  class BTagScaleFactorCalculator final : public asg::AsgTool {
    public:
      explicit BTagScaleFactorCalculator( const std::string& name );
      virtual ~BTagScaleFactorCalculator(){}

      // Delete Standard constructors
      BTagScaleFactorCalculator(const BTagScaleFactorCalculator& rhs) = delete;
      BTagScaleFactorCalculator(BTagScaleFactorCalculator&& rhs) = delete;
      BTagScaleFactorCalculator& operator=(const BTagScaleFactorCalculator& rhs) = delete;

      StatusCode initialize();
      StatusCode execute();
      StatusCode apply( const std::shared_ptr<std::unordered_map<std::size_t,std::string>>& jet_syst_collections,
                        bool use_trackjets = false);

    private:
      
      std::shared_ptr<top::TopConfig> m_config;

      CP::SystematicSet m_nominal;
      std::unordered_map<std::string, CP::SystematicSet> m_systs;
      std::unordered_map<std::string, CP::SystematicSet> m_trkjet_systs;

      ///B-tagging efficiency tools
      std::unordered_map<std::string, ToolHandle<IBTaggingEfficiencyTool>> m_btagEffTools;
      std::unordered_map<std::string, ToolHandle<IBTaggingEfficiencyTool>> m_trkjet_btagEffTools;
      ///B-tagging selection tools
      std::unordered_map<std::string, ToolHandle<IBTaggingSelectionTool>> m_btagSelTools;
      std::unordered_map<std::string, ToolHandle<IBTaggingSelectionTool>> m_trkjet_btagSelTools;

      int m_release_series = 24;  // Default to 2.4

  };
} // namespace
#endif
