/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// STL includes
#include <pthread.h>
#include <time.h>
#include <ctime>

// Framework includes
#include "GaudiKernel/ThreadLocalContext.h"

// PerfMonComps includes
#include "PerfMonComps/PerfMonMTSvc.h"
#include "PerfMonUtils.h" // borrow from existing code

/*
 * Constructor
 */
PerfMonMTSvc::PerfMonMTSvc( const std::string& name,
                            ISvcLocator* pSvcLocator )
  : AthService( name, pSvcLocator ) {
}

/*
 * Query Interface
 */
StatusCode PerfMonMTSvc::queryInterface( const InterfaceID& riid,
                                         void** ppvInterface ) {

  if( !ppvInterface ) {
    return StatusCode::FAILURE;
  }

  if ( riid == IPerfMonMTSvc::interfaceID() ) {
    *ppvInterface = static_cast< IPerfMonMTSvc* >( this );
    return StatusCode::SUCCESS;
  }

  return AthService::queryInterface( riid, ppvInterface );
}

/*
 * Initialize the Service
 */
StatusCode PerfMonMTSvc::initialize() {

  ATH_MSG_INFO("Initialize");

  /// Configure the auditor
  if( !PerfMon::makeAuditor("PerfMonMTAuditor", auditorSvc(), msg()).isSuccess()) {
    ATH_MSG_ERROR("Could not register auditor [PerfMonMTAuditor]!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

/*
 * Finalize the Service
 */
StatusCode PerfMonMTSvc::finalize() {

  ATH_MSG_INFO("Finalize");

  return StatusCode::SUCCESS;

}

/*
 * Start Auditing
 */
void PerfMonMTSvc::startAud( const std::string& stepName,
                             const std::string& compName ) {
  ATH_MSG_INFO("Starting Auditing " << stepName << " " << compName);
  const EventContext ctx = Gaudi::Hive::currentContext();
  if(ctx.valid()) {
    ATH_MSG_INFO("Current event number is " << ctx.evt() <<
                 " slot number is " << ctx.slot());
  }

  // Get thread's clock id
  clockid_t thread_cid;
  pthread_getcpuclockid(pthread_self(),&thread_cid);
  ATH_MSG_INFO("START :: Current thread's CPU clock id is " << thread_cid);

  // Read the CPU time for the clock id
  struct timespec ctime;
  clock_gettime(thread_cid, &ctime);
  ATH_MSG_INFO("START :: Current thread's CPU clock time is " << ctime.tv_sec*1.e3 + ctime.tv_nsec*1.e-6 << " [ms]");
  ATH_MSG_INFO("START :: Current std::clock time is " << std::clock()/CLOCKS_PER_SEC*1.e3 << " [ms]");
}

/*
 * Stop Auditing
 */
void PerfMonMTSvc::stopAud( const std::string& stepName,
                            const std::string& compName ) {
  ATH_MSG_INFO("Stopping Auditing " << stepName << " " << compName);

  // Get thread's clock id
  clockid_t thread_cid;
  pthread_getcpuclockid(pthread_self(),&thread_cid);
  ATH_MSG_INFO("STOP :: Current thread's CPU clock id is " << thread_cid);

  // Read the CPU time for the clock id
  struct timespec ctime;
  clock_gettime(thread_cid, &ctime);
  ATH_MSG_INFO("STOP :: Current thread's CPU clock time is " << ctime.tv_sec*1.e3 + ctime.tv_nsec*1.e-6 << " [ms]");
  ATH_MSG_INFO("STOP :: Current std::clock time is " << std::clock()/CLOCKS_PER_SEC*1.e3 << " [ms]");
}
