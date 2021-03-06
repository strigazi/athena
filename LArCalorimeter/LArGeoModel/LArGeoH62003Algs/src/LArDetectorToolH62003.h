/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARGEOMODEL_LARDETECTORTOOLH62003_H
#define LARGEOMODEL_LARDETECTORTOOLH62003_H

#include "GeoModelUtilities/GeoModelTool.h"

class LArDetectorToolH62003 final : public GeoModelTool {

public:

    // Standard Constructor
    LArDetectorToolH62003( const std::string& type, const std::string& name, const IInterface* parent );

    // Standard Destructor
    virtual ~LArDetectorToolH62003() override final;

    // Create Method:
    virtual StatusCode create() override final;
	
 private:
    
    int  m_fcalVisLimit;
    bool m_axisVisState;
};

#endif 
