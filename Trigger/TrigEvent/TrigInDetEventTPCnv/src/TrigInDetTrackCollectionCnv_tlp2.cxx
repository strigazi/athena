/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEventTPCnv/TrigInDetTrackCollectionCnv_tlp2.h"


//constructor
TrigInDetTrackCollectionCnv_tlp2::TrigInDetTrackCollectionCnv_tlp2(){


  // add the "main" base class converter (ie. TrigInDetTrackCollectionCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverter( &m_trigInDetTrackFitParCnv );
  addTPConverter( &m_trigInDetTrackCnv );

}

void TrigInDetTrackCollectionCnv_tlp2::setPStorage( TrigInDetTrackCollection_tlp2* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_trigInDetTrackCollections );

   //for all other converters defined in the base class
   m_trigInDetTrackFitParCnv.     setPStorage( &storage->m_trigInDetTrackFitPars );
   m_trigInDetTrackCnv.           setPStorage( &storage->m_trigInDetTracks );

}

