/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "RpcRdoToPrepDataToolMT.h"

#include "MuonRPC_CnvTools/IRPC_RDO_Decoder.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"

//***
// Plan with this MT code is to allow the const-cast approach to occur in Core in a single-thread manner
// and once a container local only to this thread is filled, we will take its contents and cross-check 
// against the cache and insert any members not present. 
// Smarter behaviour can then be implemented to prevent too much duplicate computation but due to the 
// complexities of the Core decoding, this is the more reasonable approach to a first MT-safe implementation
//***

Muon::RpcRdoToPrepDataToolMT::RpcRdoToPrepDataToolMT( const std::string& type, const std::string& name,
						  const IInterface* parent ) 
  : AthAlgTool( type, name, parent ),
    RpcRdoToPrepDataToolCore( type, name, parent )
{
  declareProperty("RpcPrdContainerCacheKey", m_prdContainerCacheKey, 
    "Optional external cache for the RPC PRD container");
  declareProperty("RpcCoinDataContainerCacheKey", m_coindataContainerCacheKey, 
    "Optional external cache for the RPC coin data container");
}

Muon::RpcRdoToPrepDataToolMT::~RpcRdoToPrepDataToolMT()
{
}

StatusCode Muon::RpcRdoToPrepDataToolMT::initialize() 
{
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( RpcRdoToPrepDataToolCore::initialize() );
  ATH_CHECK( m_prdContainerCacheKey.initialize( SG::AllowEmpty ) );
  ATH_CHECK( m_coindataContainerCacheKey.initialize( SG::AllowEmpty ) );
  //m_rpcPrepDataContainerKeyLocal = m_rpcPrepDataContainerKey.key()+"_LOCAL";
  //m_rpcCoinDataContainerKeyLocal = m_rpcCoinDataContainerKey.key()+"_LOCAL";
  //ATH_CHECK(m_rpcPrepDataContainerKeyLocal.initialize());
  //ATH_CHECK(m_rpcCoinDataContainerKeyLocal.initialize());
  ATH_MSG_DEBUG("initialize() successful in " << name());
  
  return StatusCode::SUCCESS;
}

StatusCode Muon::RpcRdoToPrepDataToolMT::finalize()
{   
  ATH_MSG_DEBUG("Cleaning local-thread containers");
  ATH_MSG_DEBUG("Deleting PRD container");
  delete m_rpcPrepDataContainer;
  if (m_producePRDfromTriggerWords){
    ATH_MSG_DEBUG("Deleting Coin container");
    delete m_rpcCoinDataContainer;
  }
  return RpcRdoToPrepDataToolCore::finalize();
}

StatusCode Muon::RpcRdoToPrepDataToolMT::manageOutputContainers(bool& firstTimeInTheEvent)
{
  // We will need to retrieve from cache even in different threads
  SG::WriteHandle< Muon::RpcPrepDataContainer >rpcPRDHandle(m_rpcPrepDataContainerKey);
  if (rpcPRDHandle.isPresent()){
    firstTimeInTheEvent = false;
    return StatusCode::SUCCESS;
  }
  // In MT, we always want to treat this as if its the first time in the event
  firstTimeInTheEvent = true;
  // Clear vectors which get filled
  m_decodedOfflineHashIds.clear();
  m_decodedRobIds.clear();

  // Caching of PRD container
  const bool externalCachePRD = !m_prdContainerCacheKey.key().empty();

  if (!externalCachePRD) {
    // without the cache we just record the container
    StatusCode status = rpcPRDHandle.record(std::make_unique<Muon::RpcPrepDataContainer>(m_muonIdHelperTool->rpcIdHelper().module_hash_max()));
    if (status.isFailure() || !rpcPRDHandle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of RPC PrepData Container at " << m_rpcPrepDataContainerKey.key()); 
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Created container " << m_rpcPrepDataContainerKey.key());
  } 
  else {
    // use the cache to get the container
    SG::UpdateHandle<RpcPrepDataCollection_Cache> update(m_prdContainerCacheKey);
    if (!update.isValid()){
      ATH_MSG_FATAL("Invalid UpdateHandle " << m_prdContainerCacheKey.key());
      return StatusCode::FAILURE;
    }
    StatusCode status = rpcPRDHandle.record(std::make_unique<Muon::RpcPrepDataContainer>(update.ptr()));
    if (status.isFailure() || !rpcPRDHandle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of RPC PrepData Container using cache " 
        << m_prdContainerCacheKey.key() << " - " <<m_rpcPrepDataContainerKey.key()); 
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Created container using cache for " << m_prdContainerCacheKey.key());
  }
  // Pass the container from the handle
  m_rpcPrepDataContainerFromCache = rpcPRDHandle.ptr();

  // Handle coin data if being used
  if (m_producePRDfromTriggerWords){
    SG::WriteHandle< Muon::RpcCoinDataContainer >rpcCoinHandle(m_rpcCoinDataContainerKey);
    const bool externalCacheCoinData = !m_coindataContainerCacheKey.key().empty();
    if(!externalCacheCoinData){
      // without the cache we just record the container
      StatusCode status = rpcCoinHandle.record(std::make_unique<Muon::RpcCoinDataContainer>(m_muonIdHelperTool->rpcIdHelper().module_hash_max()));
      if (status.isFailure() || !rpcCoinHandle.isValid() )   {
        ATH_MSG_FATAL("Could not record container of RPC Coin Data Container at " << m_rpcCoinDataContainerKey.key()); 
        return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG("Created container " << m_rpcCoinDataContainerKey.key());
    } 
    else {
      // use the cache to get the container
      SG::UpdateHandle<RpcCoinDataCollection_Cache> update(m_coindataContainerCacheKey);
      if (!update.isValid()){
        ATH_MSG_FATAL("Invalid UpdateHandle " << m_coindataContainerCacheKey.key());
        return StatusCode::FAILURE;
      }
      StatusCode status = rpcCoinHandle.record(std::make_unique<Muon::RpcCoinDataContainer>(update.ptr()));
      if (status.isFailure() || !rpcCoinHandle.isValid() )   {
        ATH_MSG_FATAL("Could not record container of RPC Coin Data Container using cache " 
          << m_coindataContainerCacheKey.key() << " - " <<m_rpcCoinDataContainerKey.key()); 
        return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG("Created container using cache for " << m_coindataContainerCacheKey.key());
    }
    // Pass the container from the handle
    m_rpcCoinDataContainerFromCache = rpcCoinHandle.ptr();
  }

  // We also need to make local thread ONLY versions of the containers which will be filled in a method which cannot
  // carry between threads. At the end of decoding we will move the contents to cache container in a thread-safe way
  /*
  SG::WriteHandle< Muon::RpcPrepDataContainer >rpcPRDHandleLocal(m_rpcPrepDataContainerKeyLocal);

  StatusCode status = rpcPRDHandleLocal.record(std::make_unique<Muon::RpcPrepDataContainer>(m_muonIdHelperTool->rpcIdHelper().module_hash_max()));
  if (status.isFailure() || !rpcPRDHandleLocal.isValid() )   {
    ATH_MSG_FATAL("Could not record (local-thread) container of RPC PrepData Container at " << m_rpcPrepDataContainerKeyLocal.key()); 
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Created (local-thread) container " << m_rpcPrepDataContainerKeyLocal.key());
  m_rpcPrepDataContainer = rpcPRDHandleLocal.ptr();

  if (m_producePRDfromTriggerWords){
    SG::WriteHandle< Muon::RpcCoinDataContainer >rpcCoinHandleLocal(m_rpcCoinDataContainerKeyLocal);
    status = rpcCoinHandleLocal.record(std::make_unique<Muon::RpcCoinDataContainer>(m_muonIdHelperTool->rpcIdHelper().module_hash_max()));
    if (status.isFailure() || !rpcCoinHandleLocal.isValid() )   {
      ATH_MSG_FATAL("Could not record (local-thread) container of RPC Coin Data Container at " << m_rpcCoinDataContainerKeyLocal.key()); 
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Created (local-thread) container " << m_rpcCoinDataContainerKeyLocal.key());
    m_rpcCoinDataContainer = rpcCoinHandleLocal.ptr();
  }
  */

  // To prevent a memory leak, delete the pointer if it exists
  if(m_rpcPrepDataContainer){
    delete m_rpcPrepDataContainer;
  }
  if (m_rpcCoinDataContainer){
    delete m_rpcCoinDataContainer;
  }

  m_rpcPrepDataContainer = new Muon::RpcPrepDataContainer(m_muonIdHelperTool->rpcIdHelper().module_hash_max());
  if (m_producePRDfromTriggerWords){
    m_rpcCoinDataContainer = new Muon::RpcCoinDataContainer(m_muonIdHelperTool->rpcIdHelper().module_hash_max());
  }

  return StatusCode::SUCCESS;
}

StatusCode Muon::RpcRdoToPrepDataToolMT::decode( std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& selectedIdVect ){
  ATH_MSG_DEBUG("Calling Core decode function from MT decode function (hash vector)");
  StatusCode status = Muon::RpcRdoToPrepDataToolCore::decode( idVect, selectedIdVect );
  if (status.isFailure()){
    ATH_MSG_FATAL("Error processing Core decode from MT (hash vector)");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Core decode processed in MT decode (hash vector)");
  status = transferOutputToCache();
  if (status.isFailure()){
    ATH_MSG_FATAL("Error processing container transfer from local to cache (hash vector)");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode Muon::RpcRdoToPrepDataToolMT::decode( const std::vector<uint32_t>& robIds ){
  ATH_MSG_DEBUG("Calling Core decode function from MT decode function (ROB vector)");
  StatusCode status = Muon::RpcRdoToPrepDataToolCore::decode( robIds );
  if (status.isFailure()){
    ATH_MSG_FATAL("Error processing Core decode from MT (ROB vector)");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Core decode processed in MT decode (ROB vector)");
  status = transferOutputToCache();
  if (status.isFailure()){
    ATH_MSG_FATAL("Error processing container transfer from local to cache (ROB vector)");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode Muon::RpcRdoToPrepDataToolMT::transferOutputToCache(){
  // This function should be called at the end of the Core decode function
  ATH_MSG_DEBUG("Transferring local decoding from Core to cache container inside MT");

  // Take m_rpcPrepDataContainer and transfer contents to m_rpcPrepDataContainerFromCache
  auto prd_hashes = m_rpcPrepDataContainer->GetAllCurrentHashes();
  for (auto hash : prd_hashes) {
    // Remove collection from local-thread container and place into unique_ptr to move to cache
    std::unique_ptr<Muon::RpcPrepDataCollection> coll ( m_rpcPrepDataContainer->removeCollection(hash) );
    // Check if it is present in the cache container
    bool isHashInCache = m_rpcPrepDataContainerFromCache->tryAddFromCache(hash);
    if (isHashInCache) {
      ATH_MSG_DEBUG("PRD hash " << hash << " exists inside cache container");
      continue;
    }
    // If not present, get a write lock for the hash and move collection
    RpcPrepDataContainer::IDC_WriteHandle lock = m_rpcPrepDataContainerFromCache->getWriteHandle( hash );
    StatusCode status_lock = lock.addOrDelete(std::move( coll ) );
    if (status_lock.isFailure()) {
      ATH_MSG_ERROR ( "Could not insert RpcPrepDataCollection into RpcPrepDataContainer..." );
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("PRD hash " << hash << " has been moved to cache container");
  }

  // Take m_rpcCoinDataContainer and transfer contents to m_rpcCoinDataContainerFromCache
  auto coin_hashes = m_rpcCoinDataContainer->GetAllCurrentHashes();
  for (auto hash : coin_hashes) {
    // Remove collection from local-thread container and place into unique_ptr to move to cache
    std::unique_ptr<Muon::RpcCoinDataCollection> coll ( m_rpcCoinDataContainer->removeCollection(hash) );
    // Check if it is present in the cache container
    bool isHashInCache = m_rpcCoinDataContainerFromCache->tryAddFromCache(hash);
    if (isHashInCache) {
      ATH_MSG_DEBUG("Coin hash " << hash << " exists inside cache container");
      continue;
    }
    // If not present, get a write lock for the hash and move collection
    RpcCoinDataContainer::IDC_WriteHandle lock = m_rpcCoinDataContainerFromCache->getWriteHandle( hash );
    StatusCode status_lock = lock.addOrDelete(std::move( coll ) );
    if (status_lock.isFailure()) {
      ATH_MSG_ERROR ( "Could not insert RpcCoinDataCollection into RpcCoinDataContainer..." );
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Coin hash " << hash << " has been moved to cache container");
  }

  // Debug - Check the contents
  auto prd_hashes_cache_check = m_rpcPrepDataContainerFromCache->GetAllCurrentHashes();
  for (auto hash : prd_hashes_cache_check){
    ATH_MSG_DEBUG("Contents of CACHE : " << hash);
  }
  auto prd_hashes_local_check = m_rpcPrepDataContainer->GetAllCurrentHashes();
 for (auto hash : prd_hashes_local_check){
    ATH_MSG_DEBUG("Contents of LOCAL : " << hash);
  }

  return StatusCode::SUCCESS;
}
