/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CALOTRIGGERTOOL_GTOWERSCMAP_H
#define CALOTRIGGERTOOL_GTOWERSCMAP_H

#include "Identifier/Identifier.h" 
#include "CaloTriggerTool/LArTTCell.h" 
#include "CaloTriggerTool/LArTTCell_P.h" 
#include "CaloIdentifier/LArID_Exception.h" 

#include <vector>
#include <map>

class MsgStream ; 

/**
   @brief This is a StoreGate object holding the offline TT-cell map

   Its persistifiable version is the GTowerSCMap_P class

   @author Hong Ma
   @author maintained by F. Ledroit
   @author slighted modified for j/gTowers by W. Hopkins
 */

class GTowerSCMap
{
public:

  typedef LArTTCell PDATA; 

  /* Constructor
  **/ 
  GTowerSCMap( )  ; 

  /* Destructor
  **/ 
  ~GTowerSCMap()  ;

  /**  initialize from Nova */
  void set(  const PDATA& t ) ; 

  /**
     Return a vector of offline Identifiers (corresponding helpers =
     LArEM_ID, LArHEC_ID, LArFCAL_ID) for a TT-layer offline id (helper=CaloLVL1_ID)
  */
  const std::vector<Identifier>& createCellIDvec(const int& id) const ; 

  /**
     return the offline id (CaloLVL1_ID) of the TT to which a cell (CaloCell_ID) belongs
  */
  Identifier whichTTID(const int & id) const; 

  /** return the persistified map */
  LArTTCell_P* getP() ; 
 
  /**  initialize from POOL */
  void set(  const LArTTCell_P& t ) ; 

  /* Offline TT Identifier to Offline cell Identifier vector */
  /* Offline TT Identifier to Offline cell Identifier vector*/
  /*public, or another vector shall be contructed when buidling information into database*/
  std::vector<Identifier> m_cell2ttIdVec;
  std::vector<std::vector<Identifier>> m_tt2cellIdVec;


//private: 

  void convert_to_P(const LArTTCell &t) ; // from NOVA to POOL
  void convert_to_D(const LArTTCell_P &t, LArTTCell& d) ; // POOL to NOVA

  //  std::map<HWIdentifier,Identifier>  m_on2offIdMap;

  LArTTCell_P m_persData; 

  MsgStream* m_msg ;

private:
 
};

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF( GTowerSCMap,178971817,1)

#endif 
