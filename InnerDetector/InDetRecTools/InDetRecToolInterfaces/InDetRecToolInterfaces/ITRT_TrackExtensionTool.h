/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class ITRT_TrackExtensionTool
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
//  Base class for track extension to TRT
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 22/08/2005 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef ITRT_TrackExtensionTool_H
#define ITRT_TrackExtensionTool_H

#include <list>
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "TrkTrack/Track.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkSegment/TrackSegment.h"

class MsgStream;

namespace InDet {

 
  static const InterfaceID IID_ITRT_TrackExtensionTool
    ("InDet::ITRT_TrackExtensionTool",1,0);

  class ITRT_TrackExtensionTool : virtual public IAlgTool 
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      class IEventData {
      public:
         virtual ~IEventData() {}
      };

      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      static const InterfaceID& interfaceID();
      virtual StatusCode initialize ()=0;
      virtual StatusCode finalize   ()=0;

      ///////////////////////////////////////////////////////////////////
      // Main methods for track extension to TRT
      ///////////////////////////////////////////////////////////////////

      ///////////////////////////////////////////////////////////////////
      // Pixles and sct track extension to TRT
      ///////////////////////////////////////////////////////////////////

      virtual std::vector<const Trk::MeasurementBase*>& extendTrack
        (const EventContext& ctx,
         const Trk::Track&,
         InDet::ITRT_TrackExtensionTool::IEventData &virt_event_data) const = 0;

      virtual std::vector<const Trk::MeasurementBase*>& extendTrack
        (const EventContext& ctx,
         const Trk::TrackParameters&,
         InDet::ITRT_TrackExtensionTool::IEventData &virt_event_data) const = 0;

      virtual Trk::Track* newTrack
        (const EventContext& ctx,
         const Trk::Track&,
         InDet::ITRT_TrackExtensionTool::IEventData &virt_event_data) const = 0;

      ///////////////////////////////////////////////////////////////////
      // TRT seed extension to TRT
      ///////////////////////////////////////////////////////////////////

      virtual Trk::TrackSegment* findSegment
        (const EventContext& ctx,
         const Trk::TrackParameters&,
         InDet::ITRT_TrackExtensionTool::IEventData &virt_event_data) const = 0;

      ///////////////////////////////////////////////////////////////////
      //  Tool initialisation  for new eevent
      ///////////////////////////////////////////////////////////////////

      virtual std::unique_ptr<InDet::ITRT_TrackExtensionTool::IEventData> newEvent(const EventContext& ctx) const = 0;

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
     
      virtual MsgStream&    dump(MsgStream&    out) const=0;
      virtual std::ostream& dump(std::ostream& out) const=0;
     
    };
  
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator for MsgStream and  std::ostream
  ///////////////////////////////////////////////////////////////////
  
  MsgStream&    operator << (MsgStream&   ,const ITRT_TrackExtensionTool&);
  std::ostream& operator << (std::ostream&,const ITRT_TrackExtensionTool&);
  
  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const InterfaceID& ITRT_TrackExtensionTool::interfaceID()
    {
      return IID_ITRT_TrackExtensionTool;
    }

  ///////////////////////////////////////////////////////////////////
  // Overload of << operator MsgStream
  ///////////////////////////////////////////////////////////////////
   
  inline MsgStream& operator    << 
    (MsgStream& sl,const ITRT_TrackExtensionTool& se)
    { 
      return se.dump(sl); 
    }

  ///////////////////////////////////////////////////////////////////
  // Overload of << operator std::ostream
  ///////////////////////////////////////////////////////////////////
  
  inline std::ostream& operator << 
    (std::ostream& sl,const ITRT_TrackExtensionTool& se)
    { 
      return se.dump(sl); 
    }   

} // end of name space


#endif // ITRT_TrackExtensionTool_H

