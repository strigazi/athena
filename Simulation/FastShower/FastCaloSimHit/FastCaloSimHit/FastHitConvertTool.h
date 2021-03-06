/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FastHitConvertTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef FASTCALOSIMHIT_FASTHITCONVERTTOOL_H
#define FASTCALOSIMHIT_FASTHITCONVERTTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "CaloInterface/ICaloCellMakerTool.h"
#include "LArElecCalib/ILArfSampl.h"
#include "LArSimEvent/LArHitContainer.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/WriteHandle.h"
#include "TileSimEvent/TileHitVector.h"

#include "GaudiKernel/ServiceHandle.h"

class LArHitFloat;
class LArHitFloatContainer;
class LArHit;
class LArHitContainer;
class PileUpMergeSvc;
class TileInfo;
class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class TileID;

// No NameSpace selected
//{

/** @class FastHitConvertTool

    This is for the Doxygen-Documentation.
    Please delete these lines and fill in information about
    the Algorithm!
    Please precede every member function declaration with a
    short Doxygen comment stating the purpose of this function.

    @author  Bo Liu <boliu@cern.ch>
*/

class FastHitConvertTool : public extends<AthAlgTool, ICaloCellMakerTool>
{
public:
  FastHitConvertTool(const std::string& type,const std::string& name,const IInterface* parent);

  /** default destructor */
  virtual ~FastHitConvertTool(){};

  /** standard Athena-Algorithm method */
  virtual StatusCode initialize() override final;
  /** standard Athena-Algorithm method */
  virtual StatusCode process (CaloCellContainer* theCellContainer,
                              const EventContext& ctx) const override;

private:
  StatusCode process_nc (CaloCellContainer *theCellContainer);

  StatusCode initEvent();
  StatusCode finaliseEvent();
  StatusCode hitConstruction(CaloCellContainer *theCellCont);

  //for writing out of Hit, naming as G4 for default
  SG::WriteHandle<LArHitContainer> m_embHitContainer;
  SG::WriteHandle<LArHitContainer> m_emecHitContainer;
  SG::WriteHandle<LArHitContainer> m_fcalHitContainer;
  SG::WriteHandle<LArHitContainer> m_hecHitContainer;
  SG::WriteHandle<TileHitVector>   m_tileHitVector;

  ServiceHandle<StoreGateSvc> m_storeGateFastCalo;
  PileUpMergeSvc *m_pMergeSvc;

  SG::ReadCondHandleKey<ILArfSampl> m_fSamplKey{this,"fSamplKey","LArfSamplSym","SG Key of LArfSampl object"};
  const TileInfo *m_tileInfo;
  const LArEM_ID *m_larEmID;
  const LArFCAL_ID *m_larFcalID;
  const LArHEC_ID *m_larHecID;
  const TileID* m_tileID;

  bool m_pileup;
  /** member variables for algorithm properties: */
  // int/double/bool  m_propertyName;

};
#endif          //FASTCALOSIMHIT_FASTHITCONVERTTOOL_H
