/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DBREPLICASVC_DBREPLICASVC_H
#define DBREPLICASVC_DBREPLICASVC_H
// DBReplicaSvc.h - concrete implementation of service implementating
// CORAL IReplicaSortingAlgorithm
// Richard Hawkings, started 24/4/07

#include<string>
#include "GaudiKernel/Service.h"
#include "DBReplicaSvc/IDBReplicaSvc.h"
#include "RelationalAccess/IDatabaseServiceDescription.h"

class DBReplicaSvc : public virtual IDBReplicaSvc, public virtual Service
{
  template <class TYPE> class SvcFactory;
 public:
  DBReplicaSvc(const std::string& name, ISvcLocator* svc);
  virtual ~DBReplicaSvc();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  virtual const InterfaceID& type() const;

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  void sort(std::vector<const coral::IDatabaseServiceDescription*>& 
	    replicaSet);

 private:
  StatusCode readConfig();
  std::string par_configfile;
  std::string par_testhost;
  std::string par_coolsqlitepattern;
  bool par_usecoolsqlite;
  bool par_usecoolfrontier;
  bool par_usegeomsqlite;
  bool par_nofailover;

  bool m_frontiergen;
  std::string m_hostname;
  typedef std::pair<std::string,int> ServerPair;
  typedef std::vector< ServerPair > ServerMap;
  ServerMap m_servermap;
  MsgStream* m_log;
};

#endif // DBREPLICASVC_DBREPLICASVC_H
