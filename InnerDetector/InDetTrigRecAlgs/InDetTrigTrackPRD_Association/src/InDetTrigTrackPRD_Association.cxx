/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrigTrackPRD_Association/InDetTrigTrackPRD_Association.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"
#include <ostream>

#include "TrigNavigation/NavigationCore.icc"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::InDetTrigTrackPRD_Association::InDetTrigTrackPRD_Association(const std::string& name,ISvcLocator* pSvcLocator) : 
  HLT::FexAlgo(name, pSvcLocator)
{
  // InDetTrigTrackPRD_Association steering parameters
  //
  declareProperty("TracksName"     , m_tracksName);
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

HLT::ErrorCode InDet::InDetTrigTrackPRD_Association::hltInitialize(){

   // Get association tool
  //
  if( m_assoTool.retrieve().isFailure()) {

    msg() << MSG::FATAL<< "Failed to retrieve tool " << m_assoTool << endmsg;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  } 
  else{
    msg() << MSG::INFO << "Retrieved tool " << m_assoTool << endmsg;
  }

  if (m_assoMapName.empty()) {
    msg() << MSG::FATAL<< "Association map name must not be empty." << endmsg;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }
  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  
  if(m_outputlevel<=0) {
    m_nprint=0; msg()<<MSG::DEBUG<<(*this)<<endmsg;
  }
  return HLT::OK;
}

///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////

HLT::ErrorCode InDet::InDetTrigTrackPRD_Association::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE) {
 
   int outputLevel = msgLvl();

  // Assosiate tracks with PRDs
  //
  std::unique_ptr<Trk::PRDtoTrackMap> prd_to_track_map(m_assoTool->createPRDtoTrackMap());

  std::vector<std::string>::const_iterator col=m_tracksName.begin(),cole=m_tracksName.end();
  
  int n = 0;
  for(; col!=cole; ++col) {

    m_istracks  [n]               = false;
    m_tracksPRD [n]               = 0    ;
    m_tracksPRDn[n]               = 0    ;
    const TrackCollection* tracks        ;

    if ( HLT::OK != getFeature(outputTE,tracks,(*col)) ) {
      msg() << MSG::ERROR << " Input track collection could not be found " << endmsg;
      return HLT::NAV_ERROR;
    }

    if(!tracks){
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << " Input "<< (*col) << " track collection was not attached. Trying another collection." << endmsg;
      //return HLT::OK;
      continue;
    } 
    else {
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << " Input "<< (*col) << " track collection has size " << tracks->size() << endmsg;
      if ( tracks->size() == 0 ) {
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << " Input "<< (*col) << " track collection has 0 size. Trying another collection." << endmsg;
	//return HLT::OK;
	continue;
      }
    }

    m_istracks  [n] = true;
    
    TrackCollection::const_iterator t  = tracks->begin();
    TrackCollection::const_iterator te = tracks->end  ();
    
    for ( ; t!=te; ++t) {
      if((m_assoTool->addPRDs(*prd_to_track_map, **t)).isFailure()) ++m_tracksPRDn[n]; else ++m_tracksPRD[n];
    }
    if(n<9) ++n;
  }
  if (attachFeature(outputTE, prd_to_track_map.release(), m_assoMapName.value() ) != HLT::OK) {
    msg() << MSG::ERROR << " Failed to store PRD-to-track map: " << m_assoMapName << endmsg;
  }
// Print common event information
//
if(m_outputlevel<=0) {
  m_nprint=1; msg()<<MSG::DEBUG<<(*this)<<endmsg;
 }

return HLT::OK;

}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

HLT::ErrorCode InDet::InDetTrigTrackPRD_Association::hltFinalize() {return HLT::OK;}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::InDetTrigTrackPRD_Association& se)
{ 
  return se.dump(sl);
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::InDetTrigTrackPRD_Association& se)
{
  return se.dump(sl);
}   

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::InDetTrigTrackPRD_Association::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)  return dumpevent(out);
  return dumptools(out);
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::InDetTrigTrackPRD_Association::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::InDetTrigTrackPRD_Association::dumptools( MsgStream& out ) const
{
  out<<"|----------------------------------------------------------------"
     <<"----------------------------------------------------|"
     <<std::endl;
  int n = 65-m_assoTool.type().size();

  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");
  out<<"| Tool for track-prd association                  | "<<m_assoTool.type()<<s1
     <<std::endl;
  out<<"|----------------------------------------------------------------"
     <<"----------------------------------------------------|"
     <<std::endl;

  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the ostream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::InDetTrigTrackPRD_Association::dumpevent( MsgStream& out ) const
{
  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;

  std::vector<std::string>::const_iterator col=m_tracksName.begin(),cole=m_tracksName.end();

  int N = 0;

  for(; col!=cole; ++col) {

    int n  = 30-(*col).size();
    std::string s; for(int i=0; i<n; ++i) s.append(" "); s.append("|");
    out<<"| "
       <<(*col)<<s
       <<" is collection "
       <<std::setw(2)<<m_istracks  [N]
       <<"  tracks with PRD "
       <<std::setw(5)<<m_tracksPRD [N]
       <<"  without PRD "
       <<std::setw(5)<<m_tracksPRDn[N]
       <<"     |"
       <<std::endl;
    if(N<9) ++N;
  }

  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;

  return out;
}





