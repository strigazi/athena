/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**   @file SCT_Clusterization
 *   Header file for the SCT_Clusterization class (an Algorithm).
 *   @author Paul Bell, Tommaso Lari, Shaun Roe, Carl Gwilliam
 *   @date 08 July 2008
 */


#ifndef INDETPREPRAWDATAFORMATION_SCT_CLUSTERIZATION_H
#define INDETPREPRAWDATAFORMATION_SCT_CLUSTERIZATION_H
//STL
#include <string>
#include <map>
#include <set>

//Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
// Base class
#include "AthenaBaseComps/AthAlgorithm.h"

//InDet includes
//Stored by value, cannot be fwd declared
#include "Identifier/IdentifierHash.h"
//template parameter, so should not be possible to just fwd declare
#include "Identifier/Identifier.h"
//Next contains a typedef so cannot be fwd declared
#include "InDetPrepRawData/SCT_ClusterContainer.h"
//tool/service handle template parameters


class SCT_ID;
class SCT_ChannelStatusAlg;
class SiDetectorManager;
class ISvcLocator;
class StatusCode;
class Incident;
class ISCT_FlaggedConditionSvc;

class IInDetConditionsSvc;
namespace InDetDD{
  class SiDetectorManager;
}


namespace InDet {
  class ISCT_ClusteringTool;
/**
 *    @class SCT_Clusterization
 *    @brief Form clusters from SCT Raw Data Objects
 *    The class loops over an RDO grouping strips and creating collections of clusters, subsequently recorded in StoreGate
 *    Uses SCT_ConditionsServices to determine which strips to include.
 */
class SCT_Clusterization : public AthAlgorithm,
                           public IIncidentListener {
public:
  /// Constructor with parameters:
  SCT_Clusterization(const std::string &name,ISvcLocator *pSvcLocator);
  
  /**    @name Usual algorithm methods */
  //@{
  ///Retrieve the tools used and initialize variables
  virtual StatusCode initialize();
  ///Form clusters and record them in StoreGate (detector store)
  virtual StatusCode execute();
  ///Clean up and release the collection containers
  virtual StatusCode finalize();
  //@}

  /// Incident listener method re-declared
  virtual void handle( const Incident& incident );

private:
  /**    @name Disallow default instantiation, copy, assignment */
  //@{
  SCT_Clusterization();
  SCT_Clusterization(const SCT_Clusterization&);
  SCT_Clusterization &operator=(const SCT_Clusterization&);
  //@}

  ToolHandle< ISCT_ClusteringTool >        m_clusteringTool;       //!< Clustering algorithm
  std::string                              m_dataObjectName;       //!< RDO container name in StoreGate
  std::string                              m_managerName;          //!< Detector manager name in StoreGate
  std::string                              m_clustersName; 	          
  int                                      m_page; 	           //!< Page number for hash function
  const SCT_ID*                            m_idHelper;
  typedef std::map<Identifier, int>        IdMap_t;
  IdMap_t                                  m_status;
  IdentifierHash                           m_maxKey;
  SCT_ClusterContainer*                    m_clusterContainer;
  const InDetDD::SiDetectorManager*        m_manager;
  unsigned int                             m_maxRDOs;
  ServiceHandle<IInDetConditionsSvc>       m_pSummarySvc;
  ServiceHandle<ISCT_FlaggedConditionSvc>   m_flaggedConditionSvc;
  bool                                     m_checkBadModules;
  std::set<IdentifierHash>                 m_flaggedModules;
  unsigned int                             m_maxTotalOccupancyPercent;
};

}

#endif // INDETRIOMAKER_SCT_CLUSTERIZATION_H
