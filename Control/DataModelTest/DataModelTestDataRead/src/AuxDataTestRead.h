// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file src/AuxDataTestRead.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Algorithm to test reading @c DataVector with auxiliary data.
 */


#ifndef DATAMODELTESTDATAREAD_AUXDATATESTREAD_H
#define DATAMODELTESTDATAREAD_AUXDATATESTREAD_H


#include "AthenaBaseComps/AthAlgorithm.h"


namespace DMTest {


/**
 * @brief Algorithm for reading test aux data.
 */
class AuxDataTestRead
  : public AthAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  AuxDataTestRead (const std::string &name, ISvcLocator *pSvcLocator);
  

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

  /// Parameter: Prefix for names written to SG.  Null for no write.
  std::string m_writePrefix;

  /// Event counter.
  int m_count;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAREAD_AUXDATATESTREAD_H
