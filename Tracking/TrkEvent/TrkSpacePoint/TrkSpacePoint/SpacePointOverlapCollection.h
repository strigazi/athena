/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SpacePointCollection.h
//   Header file for class SpacePointCollection
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to contain all the overlap SPs, the other ones are in 
// the SpacePointCollection
///////////////////////////////////////////////////////////////////
// Version 1.0 14/10/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef TRKSPACEPOINT_SPACEPOINTOVERLAPCOLLECTION_H
#define TRKSPACEPOINT_SPACEPOINTOVERLAPCOLLECTION_H

// Base classes
#include "DataModel/DataVector.h"
#include "GaudiKernel/DataObject.h"
//class SpacePoint;
#include "TrkSpacePoint/SpacePoint.h"

class SpacePointOverlapCollection : public DataVector< Trk::SpacePoint >{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor 
  SpacePointOverlapCollection();

  // Destructor:
  virtual ~SpacePointOverlapCollection();


  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////


  
  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:

  //  SpacePointOverlapCollection();
  SpacePointOverlapCollection(const SpacePointOverlapCollection&);
  SpacePointOverlapCollection &operator=(const SpacePointOverlapCollection&);

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:
  

};
///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(SpacePointOverlapCollection,1164030866, 1)


#endif // TRKSPACEPOINT_SPACEPOINTOVERLAPCOLLECTION_H
