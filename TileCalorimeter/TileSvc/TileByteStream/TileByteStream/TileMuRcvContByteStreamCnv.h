/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILEMURCVCONTBYTESTREAMCNV_H
#define TILEBYTESTREAM_TILEMURCVCONTBYTESTREAMCNV_H

#include "TileEvent/TileContainer.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

class DataObject;
class StatusCode;
class IAddressCreator;
class IByteStreamEventAccess;
class StoreGateSvc; 
class IROBDataProviderSvc; 
class TileMuRcvContByteStreamTool ; 
class ByteStreamCnvSvc;
class TileROD_Decoder;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


/**
 * @class TileMuRcvContByteStreamCnv
 * @brief This Converter class provides conversion between ByteStream and TileMuRcvCont
 * @author Joao Gentil Saraiva
 *
 * This class provides methods to convert the bytestream data into TileMuRcv objects and vice versa.
 */

class TileMuRcvContByteStreamCnv
  : public Converter
  , public ::AthMessaging
{
 public:
  TileMuRcvContByteStreamCnv(ISvcLocator* svcloc);

  typedef TileMuRcvContByteStreamTool  BYTESTREAMTOOL ; 

  virtual StatusCode initialize() override;
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj) override;
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr) override;
  virtual StatusCode finalize() override;

  /// Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static long storageType();
  static const CLID& classID();

 private: 

  std::string m_name;

  /** Pointer to TileMuRcvContByteStreamTool */
  ToolHandle<BYTESTREAMTOOL> m_tool;

  ServiceHandle<IByteStreamEventAccess> m_byteStreamEventAccess;
  ByteStreamCnvSvc* m_byteStreamCnvSvc;

  /** Pointer to StoreGateSvc */
  ServiceHandle<StoreGateSvc> m_storeGate;

  /** Pointer to IROBDataProviderSvc */
  ServiceHandle<IROBDataProviderSvc> m_robSvc;

  /** Pointer to TileROD_Decoder */
  ToolHandle<TileROD_Decoder> m_decoder;
};
#endif

