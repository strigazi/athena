# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_FlaggedConditionSvcSetup:
    "Class to simplify setup of SCT_FlaggedConditionSvc and required conditions algorithms"

    def __init__(self):
        self.svcName = "InDetSCT_FlaggedConditionSvc"
        self.svc = None

    def setSvc(self):
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr(ServiceMgr, self.svcName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_FlaggedConditionSvc
            ServiceMgr += SCT_FlaggedConditionSvc(name = self.svcName)
        self.svc = getattr(ServiceMgr, self.svcName)

    def getSvc(self):
        return self.svc

    def getSvcName(self):
        return self.svcName

    def setSvcName(self, svcName):
        self.svcName = svcName

    def setup(self):
        self.setSvc()

sct_FlaggedConditionSvcSetup = SCT_FlaggedConditionSvcSetup()
