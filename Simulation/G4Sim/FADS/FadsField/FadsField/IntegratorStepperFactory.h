/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IntegratorStepperFactory_H
#define IntegratorStepperFactory_H

#include "FadsField/IntegratorStepperBase.h"
#include "FadsField/FieldManager.h"
#include "G4MagIntegratorStepper.hh"

namespace FADS {

template <class T> 
class IntegratorStepperFactory : public IntegratorStepperBase {
private:
	T* theStepper;
public:
	IntegratorStepperFactory(std::string n): IntegratorStepperBase(n) , theStepper(0)
	{
		RegisterToStore();
	}
	void Create() 
	{
		if (!theStepper) 
		{
			theStepper=
			new T(FieldManager::GetFieldManager()->GetEquationOfMotion());
		}
	}
	void Delete()
	{
		if (theStepper) 
		{
			delete theStepper;
			theStepper=0;
		}
	}
	virtual G4MagIntegratorStepper* Build() 
	{
		Create(); return theStepper;
	}
	void RegisterToStore() {FieldManager::GetFieldManager()->RegisterStepper(this);}
};

}	// end namespace

#endif
