/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ThreadLocalContext.h"
#include "TrigConfHLTData/HLTUtils.h"

#include "TrigCostAnalysis.h"
#include "CostData.h"

#include "monitors/MonitorAlgorithm.h"

TrigCostAnalysis::TrigCostAnalysis( const std::string& name, ISvcLocator* pSvcLocator ) :
  AthHistogramAlgorithm(name, pSvcLocator),
  m_fullEventDumps(0) {
}


StatusCode  TrigCostAnalysis::initialize() {
  ATH_MSG_VERBOSE("In initialize()");

  ATH_MSG_DEBUG("Reading from " << m_costDataKey.key());
  ATH_CHECK( m_costDataKey.initialize() );

  if (!m_enhancedBiasTool.name().empty()) {
    ATH_CHECK( m_enhancedBiasTool.retrieve() );
  } else {
    ATH_MSG_DEBUG("No EnhancedBiasTool. Not doing additional weighting.");
    if (m_useEBWeights) {
      ATH_MSG_FATAL("Configured to use Enhanced Bias weights. Must supply weighting tool.");
      return StatusCode::FAILURE;
    }
  }

  ATH_CHECK( m_TimeRangeLengthLB > 0 );
  if (m_hashDictionaryFromFile) {
    TrigConf::HLTUtils::file2hashes();
  }
  
  return StatusCode::SUCCESS;
}


float TrigCostAnalysis::getWeight(const EventContext& context) {
  // TODO Prescale of CostMon chain for P1
  double ebWeight = 1.0;
  if (m_useEBWeights) {
    ebWeight = m_enhancedBiasTool->getEBWeight(context);
    ATH_MSG_DEBUG("EB Weight is " << ebWeight);
  }
  return m_baseEventWeight * ebWeight;
}


TH1* TrigCostAnalysis::bookGetPointer_fwd(TH1* hist, const std::string& tDir) {
  return bookGetPointer(hist, tDir);
}


StatusCode TrigCostAnalysis::execute() {
  const EventContext& context = Gaudi::Hive::currentContext();

  MonitoredRange* range = nullptr;
  ATH_CHECK(getRange(context, range));

  if (!range) {

    ATH_MSG_DEBUG("Not monitoring event");
    return StatusCode::SUCCESS;

  } else {

    ATH_MSG_DEBUG("Monitoring event " << context.eventID().event_number() << " in range " << range->getName());
    
    SG::ReadHandle<xAOD::TrigCompositeContainer> costDataHandle(m_costDataKey, context);
    ATH_CHECK( costDataHandle.isValid() );

    CostData costData;
    ATH_CHECK( costData.set(costDataHandle.get()) );
    if (!m_enhancedBiasTool.name().empty()) {
      costData.setEventLivetime( m_enhancedBiasTool->getEBLiveTime(context) );
    }

    ATH_CHECK( range->newEvent( costData, getWeight(context) ) );

  }

  if (checkDoFullEventDump(context)) {
    ATH_CHECK( dumpEvent(context) );
  }

  return StatusCode::SUCCESS;
}


bool TrigCostAnalysis::checkDoFullEventDump(const EventContext& context) {
  if (m_fullEventDumps < m_maxFullEventDumps) {
    const uint64_t start = context.eventID().event_number() - m_fullEventDumpExtraTimeSlices;
    const uint64_t stop  = context.eventID().event_number() + m_fullEventDumpExtraTimeSlices;
    for (uint64_t i = start; i <= stop; ++i ) {
      if (i % m_fullEventDumpProbability == 0) {
        if (i == context.eventID().event_number()) {
          ++m_fullEventDumps;
        }
        return true;
      }
    }
  }
  return false;
}


StatusCode TrigCostAnalysis::registerMonitors(MonitoredRange* range) {
  ATH_CHECK(range != nullptr);
  if (m_doMonitorAlgorithm) {
    std::unique_ptr<MonitorAlgorithm> monitor = std::make_unique<MonitorAlgorithm>("Algorithm_HLT", range);
    ATH_CHECK( range->addMonitor(std::move(monitor)) );
    ATH_MSG_DEBUG("Registering Algorithm_HLT Monitor for range " << range->getName() << ". Size:" << range->getMonitors().size());
  }
  // if (m_do...) {}
  return StatusCode::SUCCESS;
}


StatusCode TrigCostAnalysis::getRange(const EventContext& context, MonitoredRange*& range) {
  std::string rangeName;
  range = nullptr;
  bool includeEndOfLB = false;

  if (m_singleTimeRange) {
    rangeName = m_singleTimeRangeName;
  } else {
    const EventIDBase::number_type lumiBlock = context.eventID().lumi_block();
    const size_t lumiBlockRangeStart = lumiBlock - (lumiBlock % m_TimeRangeLengthLB);
    const size_t lumiBlockRangeStop  = lumiBlockRangeStart + m_TimeRangeLengthLB - 1;
    std::stringstream ss;
    ss << "LumiBlock_" << std::setfill('0') << std::setw(5) << lumiBlockRangeStart;
    if (includeEndOfLB && lumiBlockRangeStop != lumiBlockRangeStart) {
      ss << "_" << lumiBlockRangeStop;
    }
    rangeName = ss.str();
  }

  std::unordered_map<std::string, std::unique_ptr<MonitoredRange>>::iterator it;
  it = m_monitoredRanges.find(rangeName);

  ATH_MSG_DEBUG("Event's range string is:" << rangeName << ". Already exists? " << (it != m_monitoredRanges.end() ? "Y" : "N"));

  // If we don't have a MonitoredRange with this name, try and make one.
  if (it == m_monitoredRanges.end()) {
    if (m_monitoredRanges.size() < m_maxTimeRange) {
      auto result = m_monitoredRanges.insert( 
        std::make_pair(rangeName, std::make_unique<MonitoredRange>(rangeName, this))
      );
      it = result.first; // Returns pair. First: map iterator. Second: insertion boolean
      ATH_CHECK(registerMonitors( it->second.get() ));
    } else {
      range = nullptr; // Not monitoring any more ranges
      return StatusCode::SUCCESS;
    }
  }

  range = it->second.get(); // Pointer to MonitoredRange
  return StatusCode::SUCCESS;
}


StatusCode TrigCostAnalysis::dumpEvent(const EventContext& context) const {
  ++m_fullEventDumps;
  SG::ReadHandle<xAOD::TrigCompositeContainer> costDataHandle(m_costDataKey, context);

  std::stringstream ss;
  size_t algID = 0, threadCounter = 0;
  std::unordered_map<uint32_t, size_t> threadToCounterMap;

  for ( const xAOD::TrigComposite* tc : *costDataHandle ) {
    const uint64_t start = tc->getDetail<uint64_t>("start"); // in mus
    const uint64_t stop  = tc->getDetail<uint64_t>("stop"); // in mus
    const uint32_t slot  = tc->getDetail<uint32_t>("slot");
    const uint64_t start_ms_round = std::llround( start * 1e-3 ); // in ms
    const uint64_t stop_ms_round  = std::llround( stop  * 1e-3 ); // in ms

    const uint32_t threadID = tc->getDetail<uint32_t>("thread");
    if (threadToCounterMap.count(threadID) == 0) {
      threadToCounterMap[threadID] = threadCounter++;
    }

    ss << "{id:" << algID++;
    ss << ", group:" << threadToCounterMap[threadID];
    ss << ", className:'slot" << slot << "'";
    ss << ", content:'" << TrigConf::HLTUtils::hash2string( tc->getDetail<TrigConf::HLTHash>("alg"), "ALG");
    ss << "<br>" << TrigConf::HLTUtils::hash2string( tc->getDetail<TrigConf::HLTHash>("store"), "STORE") << "'";
    ss << ", duration:" << (stop - start); // For tooltip display: in mus
    ss << ", start:" << start_ms_round; // in ms
    if (stop_ms_round > start_ms_round) {
      ss << ", end:" << stop_ms_round;
    } else {
      ss << ", type:'point'";
    }
    ss  << "}," << std::endl;
  }

  ATH_MSG_DEBUG("Full Event Summary for event " << context.eventID().event_number());
  ATH_MSG_DEBUG(ss.str());
  // TODO Persist this

  return StatusCode::SUCCESS;
}


StatusCode TrigCostAnalysis::finalize() {
  ATH_MSG_VERBOSE("In finalize()");
  return StatusCode::SUCCESS;
}