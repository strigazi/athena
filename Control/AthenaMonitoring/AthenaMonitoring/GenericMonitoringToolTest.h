/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoring_GenericMonitoringToolTest_h
#define AthenaMonitoring_GenericMonitoringToolTest_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"

class GenericMonitoringToolTest : public AthAlgorithm {
public:
	GenericMonitoringToolTest(const std::string& name, ISvcLocator* pSvcLocator);
	StatusCode initialize();
	StatusCode beginRun();
	StatusCode execute();
	StatusCode finalize();

private:
    ToolHandle<GenericMonitoringTool> m_monitoringTool;

    void scalarTest();
    void valuesCollectionTest();
    void objectsCollectionTest();
    void inLoopTest();
};

#endif
