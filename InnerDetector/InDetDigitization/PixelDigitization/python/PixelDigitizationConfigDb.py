# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool,addService,addAlgorithm

addTool("PixelDigitization.PixelDigitizationConfig.PixelDigitizationTool", "PixelDigitizationTool")
addTool("PixelDigitization.PixelDigitizationConfig.PixelDigitizationToolHS", "PixelDigitizationToolHS")
addTool("PixelDigitization.PixelDigitizationConfig.PixelDigitizationToolPU", "PixelDigitizationToolPU")
addTool("PixelDigitization.PixelDigitizationConfig.PixelDigitizationToolSplitNoMergePU", "PixelDigitizationToolSplitNoMergePU")

addAlgorithm("PixelDigitization.PixelDigitizationConfig.PixelDigitizationHS"  , "PixelDigitizationHS")
addAlgorithm("PixelDigitization.PixelDigitizationConfig.PixelDigitizationPU"  , "PixelDigitizationPU")
addTool("PixelDigitization.PixelDigitizationConfig.getPixelRange", "PixelRange")
addService("PixelDigitization.PixelDigitizationConfig.ChargeCollProbSvc","ChargeCollProbSvc")
addTool("PixelDigitization.PixelDigitizationConfig.SurfaceChargesTool", "SurfaceChargesTool")
addTool("PixelDigitization.PixelDigitizationConfig.PixelNoisyCellGenerator", "PixelNoisyCellGenerator")
addTool("PixelDigitization.PixelDigitizationConfig.PixelGangedMerger", "PixelGangedMerger")
addTool("PixelDigitization.PixelDigitizationConfig.SpecialPixelGenerator", "SpecialPixelGenerator")
addTool("PixelDigitization.PixelDigitizationConfig.PixelChargeSmearer", "PixelChargeSmearer")
addTool("PixelDigitization.PixelDigitizationConfig.PixelDiodeCrossTalkGenerator", "PixelDiodeCrossTalkGenerator")
addTool("PixelDigitization.PixelDigitizationConfig.PixelCellDiscriminator", "PixelCellDiscriminator")
addTool("PixelDigitization.PixelDigitizationConfig.PixelRandomDisabledCellGenerator", "PixelRandomDisabledCellGenerator")
addTool("PixelDigitization.PixelDigitizationConfig.DBMChargeTool","DBMChargeTool")
addTool("PixelDigitization.PixelDigitizationConfig.PixelBarrelChargeTool","PixelBarrelChargeTool")
addTool("PixelDigitization.PixelDigitizationConfig.PixelECChargeTool","PixelECChargeTool")
addTool("PixelDigitization.PixelDigitizationConfig.IblPlanarChargeTool","IblPlanarChargeTool")
addTool("PixelDigitization.PixelDigitizationConfig.Ibl3DChargeTool","Ibl3DChargeTool")
addTool("PixelDigitization.PixelDigitizationConfig.SubChargesTool","SubChargesTool")
addService("PixelDigitization.PixelDigitizationConfig.TimeSvc","TimeSvc")
addService("PixelDigitization.PixelDigitizationConfig.CalibSvc","CalibSvc")
addTool("PixelDigitization.PixelDigitizationConfig.PixelOverlayDigitizationTool", "PixelOverlayDigitizationTool")
addAlgorithm("PixelDigitization.PixelDigitizationConfig.PixelOverlayDigitization", "PixelOverlayDigitization")
