/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class ITRT_SeededTrackFinder
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
//  Base class for track extension to Si ID
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 02/03/2006 T.Koffas
/////////////////////////////////////////////////////////////////////////////////

#ifndef ITRT_SeededTrackFinder_H
#define ITRT_SeededTrackFinder_H

#include <list>
#include "GaudiKernel/AlgTool.h"
#include "TrkTrack/Track.h"
#include "TrkSegment/TrackSegment.h"

class MsgStream;

namespace InDet {

  class SiCombinatorialTrackFinderData_xk;
 
  static const InterfaceID IID_ITRT_SeededTrackFinder
    ("InDet::ITRT_SeededTrackFinder",1,0);

  class ITRT_SeededTrackFinder : virtual public IAlgTool 
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:

      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      static const InterfaceID& interfaceID();
      virtual StatusCode initialize ()=0;
      virtual StatusCode finalize   ()=0;

      ///////////////////////////////////////////////////////////////////
      // Main methods for track extension to Si
      ///////////////////////////////////////////////////////////////////

      virtual std::list<Trk::Track*> getTrack
        (SiCombinatorialTrackFinderData_xk& combinatorialData,
         const Trk::TrackSegment&) = 0;
      virtual void newEvent(SiCombinatorialTrackFinderData_xk& combinatorialData)=0;
      virtual void newRegion(SiCombinatorialTrackFinderData_xk& combinatorialData,
                             const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&)=0;
      virtual void endEvent(SiCombinatorialTrackFinderData_xk& combinatorialData)=0;

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
     
      virtual MsgStream&    dump(MsgStream&    out) const=0;
      virtual std::ostream& dump(std::ostream& out) const=0;
     
    };
  
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator for MsgStream and  std::ostream
  ///////////////////////////////////////////////////////////////////
  
  MsgStream&    operator << (MsgStream&   ,const ITRT_SeededTrackFinder&);
  std::ostream& operator << (std::ostream&,const ITRT_SeededTrackFinder&);
  
  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const InterfaceID& ITRT_SeededTrackFinder::interfaceID()
    {
      return IID_ITRT_SeededTrackFinder;
    }

  ///////////////////////////////////////////////////////////////////
  // Overload of << operator MsgStream
  ///////////////////////////////////////////////////////////////////
   
  inline MsgStream& operator    << 
    (MsgStream& sl,const ITRT_SeededTrackFinder& se)
    { 
      return se.dump(sl); 
    }
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator std::ostream
  ///////////////////////////////////////////////////////////////////
  
  inline std::ostream& operator << 
    (std::ostream& sl,const ITRT_SeededTrackFinder& se)
    { 
      return se.dump(sl); 
    }   

} // end of name space


#endif // ITRT_SeededTrackFinder_H

