/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Base class
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "G4AtlasTools/PolyconicalEnvelope.h"

#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Polycone.hh"

// Geant4 includes used in functions

PolyconicalEnvelope::PolyconicalEnvelope(const std::string& type, const std::string& name, const IInterface* parent)
  : DetectorGeometryBase(type,name,parent)
{
	ATH_MSG_INFO( "PolyconicalEnvelope being created: " << name );
	declareProperty("NSurfaces",m_surfaceNr,"Nr. of surfaces in Z");
	declareProperty("InnerRadii",m_innerRadii,"Envelope inner radii");
	declareProperty("OuterRadii",m_outerRadii,"Envelope outer radii");
	declareProperty("ZSurfaces",m_ZSurfaces,"Envelope surface Zs");
}

StatusCode PolyconicalEnvelope::initialize()
{
	return StatusCode::SUCCESS;
}


void PolyconicalEnvelope::BuildGeometry() 
{
	ATH_MSG_INFO( "this is PolyconicalEnvelope::BuildGeometry: " );
	
// Get nist material manager
  	G4NistManager* nist = G4NistManager::Instance();

	G4Material* mat = nist->FindOrBuildMaterial("G4_AIR");
	
	if (!m_surfaceNr) m_surfaceNr=m_ZSurfaces.size();
	
	if (m_innerRadii.size() != m_surfaceNr)
		ATH_MSG_ERROR("Size of inner radii vector "<<m_innerRadii.size()<<
		             " differs from nr. of surfaces "<<m_surfaceNr<<"!!!");
	if (m_outerRadii.size() != m_surfaceNr)
		ATH_MSG_ERROR("Size of outer radii vector "<<m_outerRadii.size()<<
		             " differs from nr. of surfaces "<<m_surfaceNr<<"!!!");
	if (m_ZSurfaces.size() != m_surfaceNr)
		ATH_MSG_ERROR("Size of surface Z vector "<<m_ZSurfaces.size()<<
		             " differs from nr. of surfaces "<<m_surfaceNr<<"!!!");
					 
	double* zv=new double[m_surfaceNr];
	double* iv=new double[m_surfaceNr];
	double* ov=new double[m_surfaceNr];
	
	for (unsigned int i=0;i<m_surfaceNr;i++)
	{
		zv[i]=m_ZSurfaces[i];
		iv[i]=m_innerRadii[i];
		ov[i]=m_outerRadii[i];
	}
	
	G4VSolid* envSolid=new G4Polycone(m_detectorName,0.,360.*deg,m_surfaceNr,zv,iv,ov);
	
	G4LogicalVolume* logicWorld = new G4LogicalVolume(envSolid,mat,m_detectorName);

	envelope.theEnvelope=logicWorld;
	
	delete[] zv;
	delete[] iv;
	delete[] ov;
}

