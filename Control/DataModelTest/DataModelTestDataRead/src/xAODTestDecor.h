// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file src/xAODTestDecor.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Algorithm to test adding decorations to xAOD types.
 */


#ifndef DATAMODELTESTDATAREAD_XAODTESTDECOR_H
#define DATAMODELTESTDATAREAD_XAODTESTDECOR_H


#include "AthenaBaseComps/AthAlgorithm.h"


namespace DMTest {


/**
 * @brief Algorithm to test adding decorations to xAOD data.
 */
class xAODTestDecor
  : public AthAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  xAODTestDecor (const std::string &name, ISvcLocator *pSvcLocator);
  

  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize();


  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute(); 


  /**
   * @brief Algorithm finalization; called at the end of the job.
   */
  virtual StatusCode finalize();


private:
  /// Parameter: Prefix for names read from SG.
  std::string m_readPrefix;

  /// Parameter: Name of the decoration.
  std::string m_decorName;

  /// Parameter: Offset to be applied to decoration.
  int m_offset;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAREAD_XAODTESTDECOR_H
