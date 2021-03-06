/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _PCDio_h_
#define _PCDio_h_

#include "CxxUtils/checker_macros.h"
#include "RelationalAccess/AccessMode.h"

#include <string>

namespace coral {
  class ISessionProxy;
}

class ATLAS_NOT_THREAD_SAFE PCDio // global static variable is used.
{
 public:
  PCDio(std::string connString, std::string tableName, int verbose);
  ~PCDio();

  void init(coral::AccessMode access_mode);
  void load(std::string tag, int revision);
  void save(std::string tag, int revision, std::string sources);

 private:
  std::string m_connString;
  std::string m_pixeltable;
  int m_verbose;
  coral::ISessionProxy *m_session;

  void createTable();
  void createAuxTables();
  void createHeaderTable();
  void createDataTable();
  long long updateKey();

  void transactionStartReadOnly();
  void transactionStartUpdate();
  void transactionCommit();
};

#endif
