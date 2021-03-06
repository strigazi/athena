/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SubtractedCylinderLayer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_SUBTRACTEDCYLINDERLAYER_H
#define TRKGEOMETRY_SUBTRACTEDCYLINDERLAYER_H

class MsgStream;

#include "TrkGeometry/Layer.h"
#include "TrkGeometrySurfaces/SubtractedCylinderSurface.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkEventPrimitives/PropDirection.h"


namespace Trk {

class LayerMaterialProperties;
class OverlapDescriptor;

  /**
   @class SubtractedCylinderLayer
   
   Class to describe a cylindrical detector layer for tracking, with subtraction; it inhertis from both, 
   Layer base class and SubtractedCylinderSurface class
       
   @author Sarka.Todorova@cern.ch
  */

  class SubtractedCylinderLayer : virtual public SubtractedCylinderSurface, public Layer {
                   
      public:
        /**Default Constructor*/
        SubtractedCylinderLayer(){}
        
        /**Constructor with SubtractedCylinderSurface components and  MaterialProperties */
        SubtractedCylinderLayer(const SubtractedCylinderSurface* subCyl,
                      const LayerMaterialProperties& laymatprop,
                      double thickness = 0.,
                      OverlapDescriptor* od = nullptr,
                      int laytyp=int(Trk::active));
                              
        /**Copy constructor*/
        SubtractedCylinderLayer(const SubtractedCylinderLayer& cla);

        /**Copy constructor with shift*/
        SubtractedCylinderLayer(const SubtractedCylinderLayer& cla, const Amg::Transform3D& tr);
        
        /**Assignment operator */
        SubtractedCylinderLayer& operator=(const SubtractedCylinderLayer&);
                      
        /**Destructor*/
        virtual ~SubtractedCylinderLayer() override{}  
        
        /** Transforms the layer into a Surface representation for extrapolation */
        virtual const SubtractedCylinderSurface& surfaceRepresentation() const override;
        
        /** getting the MaterialProperties back - for pre-update*/ 
        virtual double preUpdateMaterialFactor(const Trk::TrackParameters& par,
                                       Trk::PropDirection dir) const override;

        /** getting the MaterialProperties back - for post-update*/ 
        virtual double  postUpdateMaterialFactor(const Trk::TrackParameters& par,
                                         Trk::PropDirection dir) const override;

        /** use the base class insideBounds (Vector2d, BoundaryCheck) */
        using CylinderSurface::insideBounds;

        /** move the Layer */
        virtual void moveLayer( Amg::Transform3D& shift ) override;

        /** move the Layer */
        virtual void moveLayer ATLAS_NOT_THREAD_SAFE ( Amg::Transform3D& shift ) const override{
         const_cast<SubtractedCylinderLayer*> (this)->moveLayer(shift);
        }


      private:
       /** Resize the layer to the tracking volume - not implemented*/  
       virtual void resizeLayer(const VolumeBounds&, double) override {}
      /** Resize the layer to the tracking volume - not implemented*/  
       virtual void resizeLayer ATLAS_NOT_THREAD_SAFE (const VolumeBounds&, double) const  override {}

       /** Resize the layer to the tracking volume - not implemented */ 
       virtual void resizeAndRepositionLayer(const VolumeBounds&, const Amg::Vector3D&, double)   override {}

       /** Resize the layer to the tracking volume - not implemented */
       virtual void resizeAndRepositionLayer ATLAS_NOT_THREAD_SAFE(const VolumeBounds&,
                                                                   const Amg::Vector3D&,
                                                                   double) const override
       {}
  };
 
} // end of namespace

#endif // TRKGEOMETRY_SUBTRACTEDCYLINDERLAYER_H

