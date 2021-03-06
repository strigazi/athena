# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addAlgorithm

addAlgorithm("OverlayCopyAlgs.OverlayCopyAlgsConfigLegacy.getCopyCaloCalibrationHitContainer", "CopyCaloCalibrationHitContainer")
addAlgorithm("OverlayCopyAlgs.OverlayCopyAlgsConfigLegacy.getCopyInTimeJetTruthInfo", "CopyInTimeJetTruthInfo")
addAlgorithm("OverlayCopyAlgs.OverlayCopyAlgsConfigLegacy.getCopyOutOfTimeJetTruthInfo", "CopyOutOfTimeJetTruthInfo")
addAlgorithm("OverlayCopyAlgs.OverlayCopyAlgsConfigLegacy.getCopyMcEventCollection", "CopyMcEventCollection")
addAlgorithm("OverlayCopyAlgs.OverlayCopyAlgsConfigLegacy.getCopyTimings", "CopyTimings")
addAlgorithm("OverlayCopyAlgs.OverlayCopyAlgsConfigLegacy.getCopyTrackRecordCollection", "CopyTrackRecordCollection")
