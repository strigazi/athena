/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERICMETADATATOOL_H
#define GENERICMETADATATOOL_H

/** @file GenericMetadataTool.h
 *  @brief This file contains the class definition for the GenericMetadataTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: GenericMetadataTool.h 663679 2015-04-29 08:31:54Z krasznaa $
 **/

#include "AthenaBaseComps/AthAlgTool.h"
//#include "AsgTools/AsgMetadataTool.h"
#include "AthenaKernel/IMetaDataTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
//#include "AthenaKernel/ICutFlowSvc.h"

//#include "xAODCutFlow/CutBookkeeper.h"
//#include "xAODCutFlow/CutBookkeeperContainer.h"
//#include "xAODCutFlow/CutBookkeeperAuxContainer.h"

#include <string>

/** @class GenericMetadataTool
 *  @brief This class provides an example for reading with a ISelectorTool to veto events on AttributeList.
 **/

template <typename T, typename U>
class GenericMetadataTool : public AthAlgTool, public virtual ::IMetaDataTool
{
public: // Constructor and Destructor
   /// Standard Service Constructor
   GenericMetadataTool(const std::string& type, 
                  const std::string& name,
                  const IInterface*  parent);
   /// Destructor
   virtual ~GenericMetadataTool();

public:
   virtual StatusCode metaDataStop(const SG::SourceID&);
   virtual StatusCode beginInputFile(const SG::SourceID& sid = "Serial");
   virtual StatusCode endInputFile(const SG::SourceID& sid = "Serial");
   virtual StatusCode initialize();
   virtual StatusCode finalize();
protected:
   ServiceHandle<StoreGateSvc> inputMetaStore() const;
   ServiceHandle<StoreGateSvc> outputMetaStore() const;

private:
  
  /// Helper class to update a container with information from another one
  //StatusCode updateContainer( T* contToUpdate,
  //                            const T* otherCont );

  StatusCode initOutputContainer(const std::string& sgkey);

  StatusCode buildAthenaInterface(const std::string& inputName,
                                  const std::string& outputName,
                                  const SG::SourceID& sid);

  /// Fill Cutflow information
  StatusCode addProcessMetadata();
 
  /// Pointer to cut flow svc 
  ServiceHandle<StoreGateSvc> m_inputMetaStore;
  ServiceHandle<StoreGateSvc> m_outputMetaStore;

  /// The name of the output Container
  std::string m_outputCollName;
  
  /// The name of the input Container
  std::string  m_inputCollName;

  /// The name of the process Container
  std::string m_procMetaName;

  bool m_processMetadataTaken;
  bool m_markIncomplete;

  /// List of source ids which have reached end file
  std::set<SG::SourceID> m_fullreads;
  std::set<SG::SourceID> m_read;
  std::set<SG::SourceID> m_written;

};

template <typename T, typename U>
inline ServiceHandle<StoreGateSvc> GenericMetadataTool<T,U>::inputMetaStore() const
{
  return m_inputMetaStore;
}

template <typename T, typename U>
inline ServiceHandle<StoreGateSvc> GenericMetadataTool<T,U>::outputMetaStore() const
{
  return m_outputMetaStore;
}

#include "GenericMetadataTool.icc"
#endif

