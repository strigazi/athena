/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MagneticFieldMap_H
#define MagneticFieldMap_H

#include "G4MagneticField.hh"
#include <string>

namespace FADS {

class MagneticFieldMap: public G4MagneticField {
public:
	MagneticFieldMap();
	MagneticFieldMap(std::string n);
	void GetFieldValue( const double *xyzPos, double *xyzField) const;
	virtual void Initialize() {}
	virtual void Terminate() {}
	virtual MagneticFieldMap* Create() {return this;}
	virtual MagneticFieldMap* Create(std::string) {return this;}
	bool IsInitialized() {return initialized;}
	virtual void FieldValue( const double *xyzPos, double *xyzField) const = 0;
	std::string GetName() {return name;}
	void SetInitialValues();
protected:
	std::string name;
	bool initialized;
};

}	// end namespace

#endif
