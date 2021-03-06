/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARSHAPE32MC_H
#define LARRAWCONDITIONS_LARSHAPE32MC_H

#include "LArElecCalib/ILArShape.h" 
#include "LArRawConditions/LArShapeP1.h"
#include "LArRawConditions/LArConditionsContainer.h"
#include "AthenaKernel/BaseInfo.h"

#include <vector>

/** This class implements the ILArShape interface
 *
 * @author S. Laplace
 * @version  0-0-1 , 29/01/04
 *
 * History:
 *  - 08/02/2004, S. Laplace: new online ID
 *
 */

class LArShape32MC: public ILArShape,
	public LArConditionsContainer<LArShapeP1>
 {
  
 public: 
  
  typedef ILArShape::ShapeRef_t ShapeRef_t;
  typedef LArConditionsContainer<LArShapeP1> CONTAINER ;

  LArShape32MC();
  
  virtual ~LArShape32MC( );
  virtual StatusCode initialize( );
  
  // retrieving Shape using online ID
  virtual  ShapeRef_t Shape   (const HWIdentifier&  CellID, int gain, int tbin = 0, int mode = 0 ) const ;
  virtual  ShapeRef_t ShapeDer(const HWIdentifier&  CellID, int gain, int tbin = 0, int mode = 0) const ;
  
  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, int gain, 
	   const std::vector<float>& vShape,
           const std::vector<float>& vShapeDer);

};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArShape32MC,107446859,1)
CONDCONT_DEF(LArShape32MC, 27292873, ILArShape);


#endif 
