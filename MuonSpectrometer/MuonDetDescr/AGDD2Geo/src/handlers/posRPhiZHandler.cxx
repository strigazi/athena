/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/posRPhiZHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/AGDDPositioner.h"
#include <iostream>

#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Transform3D.h"

posRPhiZHandler::posRPhiZHandler(std::string s):XMLHandler(s)
{
}

void posRPhiZHandler::ElementHandle()
{
	bool res;
	std::string volume=getAttributeAsString("volume",res);
	std::vector<double> vvv=getAttributeAsVector("R_Phi_Z",res);
	
	double radius,phi,zpos;

	radius=vvv[0];
	phi=vvv[1];
	zpos=vvv[2];

	const double pi=2.*asin(1.);
	const double degrad=pi/180.;
	
	CLHEP::Hep3Vector cvec;
	CLHEP::HepRotation crot;

        vvv=getAttributeAsVector("rot",res);
        if (res) 
        {
                crot=CLHEP::HepRotation();
                crot.rotateX(vvv[0]*degrad);
                crot.rotateY(vvv[1]*degrad);
                crot.rotateZ(vvv[2]*degrad);
        }

	crot.rotateZ(phi*degrad);
	double x=radius*cos(phi*degrad);
	double y=radius*sin(phi*degrad);
	cvec=CLHEP::Hep3Vector(x,y,zpos);
	AGDDPositioner *p __attribute__((__unused__));
	p=new AGDDPositioner(volume,crot,cvec);
}
