# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#from AthenaCommon.Logging import logging 

#Note: This tool is used as CaloCellCorrection tool and by the CaloNoiseTool
def LArCellHVCorrDefault(name="LArCellHVCorrDefault"):
     # check if tool already exists
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if hasattr(svcMgr.ToolSvc, name):
        # re-use previously configured (public) tool
        return getattr(svcMgr.ToolSvc, name)
    
    
    from LArConditionsCommon import LArHVDB

    from LArCellRec.LArCellRecConf import LArCellHVCorr
    theLArCellHVCorr = LArCellHVCorr(name)

    svcMgr.ToolSvc += theLArCellHVCorr
    return theLArCellHVCorr
