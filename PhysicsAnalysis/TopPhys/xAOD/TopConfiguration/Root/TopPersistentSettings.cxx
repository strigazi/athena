/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopConfiguration/TopPersistentSettings.h"

namespace top{
  TopPersistentSettings::TopPersistentSettings() :
    m_isMC(false),
    m_isAFII(false),
    m_applyElectronInJetSubtraction(false),
    m_doOverlapRemovalOnLooseLeptonDef(false),
    m_doKLFitter(false),
    m_doPseudoTop(false),
    m_sgKeyTopSystematicEvents("SetMe"),
    m_sgKeyTopSystematicEventsLoose("SetMe"),

    m_sgKeyEventInfo("SetMe"),
    m_sgKeyPrimaryVertices("SetMe"),
    m_sgKeyPhotons("SetMe"),
    m_sgKeyElectrons("SetMe"),
    m_sgKeyMuons("SetMe"),
    m_sgKeyTaus("SetMe"),
    m_sgKeyJets("SetMe"),
    m_sgKeyLargeRJets("SetMe"),
    m_sgKeyTrackJets("SetMe"),
    m_sgKeyMissingEt("SetMe"),
    m_sgKeyMissingEtLoose("SetMe"),

    m_electronID("SetMe"),
    m_electronIDLoose("SetMe"),
    m_electronIsolation("SetMe"),
    m_electronIsolationLoose("SetMe"),

    m_muon_trigger_SF("SetMe"),
    m_muonQuality("SetMe"),
    m_muonQualityLoose("SetMe"),
    m_muonIsolation("SetMe"),
    m_muonIsolationLoose("SetMe"),

    m_chosen_btaggingWP(),

    m_systSgKeyMapPhotons(),
    m_systSgKeyMapElectrons(),
    m_systSgKeyMapMuons(),
    m_systSgKeyMapTaus(),
    m_systSgKeyMapJets(),
    m_systSgKeyMapJets_electronInJetSubtraction(),
    m_systSgKeyMapJetsLoose_electronInJetSubtraction(),
    m_systSgKeyMapLargeRJets(),
    m_systSgKeyMapTrackJets(),
    m_systSgKeyMapMissingET(),
    m_systSgKeyMapMissingETLoose(),
    m_systSgKeyMapKLFitter(),
    m_systSgKeyMapKLFitterLoose(),
    m_systSgKeyMapPseudoTop(),
    m_systPersistantAllTTreeNames(),
    m_systDecoKeyMapJetGhostTrack(),

    m_list_systHashAll(),
    m_list_systHash_electronInJetSubtraction(),

    m_allSelectionNames(),
    m_allTriggers(),
    m_electronTriggers(),
    m_muonTriggers(),
    m_tauTriggers()
  {}
}
