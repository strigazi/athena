/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TrigAFPReco/Trig_AFPSiTrkReco.h"
#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPSiHitContainer.h"

Trig_AFPSiTrkReco::Trig_AFPSiTrkReco(const std::string& name,
                                     ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator),
  m_robDataProvider("ROBDataProviderSvc", name),
  m_rawDataTool("AFP_RawDataProviderTool"),
  m_digiTool("AFP_Raw2DigiTool"),
  m_trackRecoTool("AFP_SIDLocRecoTool") {
  declareProperty("AFP_RawDataCollectionKey", m_rawDataCollectionKey = "AFP_RawData", "Name of the raw data container");
  declareProperty("AFP_SiHitContainerName", m_siHitContainerName = "AFPSiHitContainer", "Name of the storegate container with Silicon pixel hits");
  declareProperty("AFP_TrackContainerName", m_trackContainerName = "AFPTrackContainer", "Name of the AFP tracks container to be called in the hypothesis algorithm with the same name");
  declareProperty("RawDataProviderTool", m_rawDataTool, "AFP_RawDataProviderTool used to reconstruct raw data from ROB data");
  declareProperty("DigiTool", m_digiTool, "AFP_Raw2DigiTool used to reconstruct Silicon pixel hits from raw data");
  declareProperty("SiDTool", m_trackRecoTool, "AFPSiDLocRecoTool used to reconstruct AFP tracks from Si hits");
}

Trig_AFPSiTrkReco::~Trig_AFPSiTrkReco() {}

HLT::ErrorCode Trig_AFPSiTrkReco::hltInitialize() {
  ATH_MSG_DEBUG("Trig_AFPSiTrkReco::initialize");

  if (m_robDataProvider.retrieve().isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve service " << m_robDataProvider);
    return HLT::ERROR;
  } else ATH_MSG_DEBUG("Retrieved service " << m_robDataProvider);

  if (m_rawDataTool.retrieve().isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve service " << m_rawDataTool);
    return HLT::ERROR;
  } else {
    ATH_MSG_DEBUG("Retrieved service " << m_rawDataTool);
  }

  if (m_digiTool.retrieve().isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve service " << m_digiTool);
    return HLT::ERROR;
  } else {
    ATH_MSG_DEBUG("Retrieved service " << m_digiTool);
  }

  if (m_trackRecoTool.retrieve().isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve service " << m_trackRecoTool);
    return HLT::ERROR;
  } else {
    ATH_MSG_DEBUG("Retrieved service " << m_trackRecoTool);
  }

  ATH_MSG_DEBUG("Before the end of Trig_AFPSiTrkReco::initialize");
  return HLT::OK;
}

HLT::ErrorCode Trig_AFPSiTrkReco::hltExecute(const HLT::TriggerElement* /*inputTE*/,
                                             HLT::TriggerElement* /*outputTE*/) {
  ATH_MSG_DEBUG("Trig_AFPSiTrkReco::EXECUTE");

  //Reconstructing Si hits in case it is not already in evtStore, in which case there would be a conflict
  if (!evtStore()->contains<xAOD::AFPSiHitContainer>(m_siHitContainerName)) {
	  //Recording raw data container to be used by m_rawDataTool
    AFP_RawDataContainer* container = new AFP_RawDataContainer();
    ATH_MSG_DEBUG("Created AFP RDO Container");
    StatusCode recordSC =
      evtStore()->record(container, m_rawDataCollectionKey);
    if (recordSC.isFailure()) {
      ATH_MSG_WARNING("Unable to record AFP RDO Container");
      return HLT::ERROR;
    } else {
      ATH_MSG_DEBUG("AFP RDO Container recorded");
    }

    std::vector<const ROBFragment*> listOfRobf;
    std::vector<unsigned int> ROBIDs;
	//Adding ROB numbers with AFP information
    ROBIDs.push_back(0x00850001);
    ROBIDs.push_back(0x00850002);

	//Retrieving ROB data
    m_robDataProvider->getROBData(ROBIDs, listOfRobf);
    ATH_MSG_DEBUG("  ROB ID " << std::hex << ROBIDs << std::dec);
    ATH_MSG_DEBUG(" Number of ROB fragments is " << listOfRobf.size());

    //Reconstructing Raw Data from ROBs
    if (m_rawDataTool->convert(listOfRobf, container).isFailure()) {
      ATH_MSG_WARNING("BS conversion into RDOs failed");
      return HLT::ERROR;
    } else {
      ATH_MSG_DEBUG(" Number of collections in container is "
                    << container->size());
    }

	// Reconstructing Si Hits from raw data
    if (m_digiTool->recoSiHits().isFailure()) {
      ATH_MSG_WARNING("Could not reconstruct SiHits");
      return HLT::ERROR;
    } else {
      ATH_MSG_DEBUG(" Number of collections in Si Hit container is ");
    }
  }

  //Reconstructing AFP tracks in case a container is not present in evtStore, in which case there would be a conflict
  if (!evtStore()->contains<xAOD::AFPTrackContainer>(m_trackContainerName)) {
    if (m_trackRecoTool->reconstructTracks().isFailure()) {
      ATH_MSG_WARNING("Tracks reconstruction failed");
      return HLT::ERROR;
    }
  }

  return HLT::OK;
}

HLT::ErrorCode Trig_AFPSiTrkReco::hltFinalize() {
  ATH_MSG_DEBUG("Trig_AFPSiTrkReco::FINALIZE");
  return HLT::OK;
}
