/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDD2Geo/AGDDMicromegas.h"
#include "AGDD2Geo/AGDDController.h"
#include "AGDD2Geo/AGDDBuilder.h"
#include "AGDD2Geo/AGDDParameterStore.h"

#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"

#include "MuonGeoModel/MicromegasComponent.h"
#include "MuonGeoModel/Micromegas.h"

void AGDDMicromegas::CreateSolid() 
{
//	std::cout<<"this is AGDDMicromegas::CreateSolid()"<<std::endl;
//	void *p=GetSolid();
//	if (!p)
//	{
//		std::cout<<" creating solid with dimensions "<<
//		_small_x<<" "<<_large_x<<" "<<_y<<" "<<_z<<std::endl;
//		GeoShape* solid=new GeoTrd(_small_x/2.,_large_x/2.,_y/2.,_y/2.,_z/2.);
//		SetSolid(solid);
//	}

}

void AGDDMicromegas::CreateVolume() 
{
//    std::cout<<"this is AGDDMicromegas::CreateVolume()"<<std::endl;
	
	MuonGM::MicromegasComponent *mm_comp=new MuonGM::MicromegasComponent;
	mm_comp->name=tech;
	mm_comp->dx1=_small_x;
	mm_comp->dx2=_large_x;
	mm_comp->dy=_y;
	mm_comp->subType=sType;
	
	MuonGM::Micromegas *cham=new MuonGM::Micromegas(mm_comp);
	GeoPhysVol *vvv=(GeoPhysVol*)cham->build(1);

	CreateSolid();

	if (!GetVolume())
	{
		SetVolume(vvv);
	}

	if(AGDDParameterStore::GetParameterStore()->Exist((*this).GetName())) {
		std::cout << " parameters for volume " << (*this).GetName() << " already registered" << std::endl;
	}
	else {
		AGDDParameterBagMM* paraBag = new AGDDParameterBagMM();
		paraBag->largeX = _large_x;
		paraBag->smallX = _small_x;
		paraBag->lengthY = _y;
		AGDDParameterBagMMTech* techparaBag = dynamic_cast<AGDDParameterBagMMTech*> (AGDDParameterStore::GetParameterStore()->GetParameterBag(tech));
		if(!paraBag) std::cout << " not possible to retrieve technology parameters for <" << tech << ">" << std::endl;
		paraBag->TechParameters = techparaBag; 
		AGDDParameterStore::GetParameterStore()->RegisterParameterBag((*this).GetName(), paraBag);
	}
}

GeoMaterial* AGDDMicromegas::GetMMMaterial(std::string name)
{
	StoreGateSvc* pDetStore=0;
	ISvcLocator* svcLocator = Gaudi::svcLocator();
	StatusCode sc=svcLocator->service("DetectorStore",pDetStore);
	if(sc.isSuccess())
	{
		DataHandle<StoredMaterialManager> theMaterialManager;
		sc = pDetStore->retrieve(theMaterialManager, "MATERIALS");
		if(sc.isSuccess())
        {
			return theMaterialManager->getMaterial(name);
        }
	}
	return 0;
}
