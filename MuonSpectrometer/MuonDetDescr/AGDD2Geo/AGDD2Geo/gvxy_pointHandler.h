/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef gvxy_pointHandler_H
#define gvxy_pointHandler_H

#include "AGDD2Geo/XMLHandler.h"
#include "AGDD2Geo/TwoPoint.h"
#include <string>

class gvxy_pointHandler:public XMLHandler {
public:
	gvxy_pointHandler(std::string);
	void ElementHandle();
	static TwoPoint CurrentTwoPoint() {return point;}
private:
	static TwoPoint point;
};

#endif
