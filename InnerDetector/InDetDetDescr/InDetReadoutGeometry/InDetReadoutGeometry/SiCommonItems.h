/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SiCommonItems.h
 */

#ifndef INDETREADOUTGEOMETRY_SICOMMONITEMS_H
#define INDETREADOUTGEOMETRY_SICOMMONITEMS_H

class AtlasDetectorID;

/**
 * Message Stream Member
 */
#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "GeoPrimitives/GeoPrimitives.h"


#include "GaudiKernel/ServiceHandle.h"
#include "CLHEP/Geometry/Transform3D.h"

#include <mutex>

namespace InDetDD {

  /**
   * @class SiCommonItems
   *
   * Helper class to concentrate common items, such as the pointer to the IdHelper,
   * the lorentzAngle tool or the information about the solenoidal frame.
   *
   * To be used for Pixel and SCT.
   *
   * SiCommonItems is owned by SiDetectorManager.
   * 
   * @author: Grant Gorfine
   * mondified & maintained: Nick Styles, Andreas Salzburger
   */

  class SiCommonItems
  {
    
  public:
 
    /**
     * Constructor
     */
    SiCommonItems(const AtlasDetectorID* const idHelper);

    /**
     * @name Getter methods
     */
    //@{
    const AtlasDetectorID* getIdHelper() const;
    const HepGeom::Transform3D& solenoidFrame() const;
    const ISiLorentzAngleTool* lorentzAngleTool() const;
    //@}

    /**
     * @name Setter methods
     */
    //@{
    void setSolenoidFrame(const HepGeom::Transform3D& transform) const; 
    void setLorentzAngleTool(const ISiLorentzAngleTool* lorentzAngleTool);
    //@}

    /**
     * @name Message methods
     */
    //@{
    /**
     * The message method for further use
     */
    MsgStream& msg (MSG::Level lvl) const { return m_msg.get() << lvl; }
    /**
     * The method providing Verbosity Level
     */
    bool msgLvl (MSG::Level lvl) const { return m_msg.get().level() <= lvl; }
    //@}      
  
  private:

    /**
     * Message stream member
     */
    mutable Athena::MsgStreamMember m_msg ATLAS_THREAD_SAFE;
          
    /**
     * IdHelper
     */
    const AtlasDetectorID* m_idHelper; 

    /**
     * LorentzAngle tool
     */
    const ISiLorentzAngleTool* m_lorentzAngleTool;

    /**
     * Solenoidal frame. Guarded by m_mutex
     */
    mutable HepGeom::Transform3D m_solenoidFrame ATLAS_THREAD_SAFE;

    /**
     * To guard m_solenoidFrame in solenoidFrame()
     */
    mutable std::mutex m_mutex;
  };
    
    
  inline const AtlasDetectorID* SiCommonItems::getIdHelper() const
  {
    return m_idHelper;
  }
    
    
  inline const HepGeom::Transform3D & SiCommonItems::solenoidFrame() const
  {
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_solenoidFrame;
    // This reference might be changed by setSolenoidFrame.
    // However, it occurrs very rarely.
  }
    

} // End namespace InDetDD

#endif // INDETREADOUTGEOMETRY_SICOMMONITEMSS_H
