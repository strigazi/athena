/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderVolumeCreator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_CYLINDERVOLUMECREATOR_H
#define TRKDETDESCRTOOLS_CYLINDERVOLUMECREATOR_H

// Trk
#include "TrkDetDescrInterfaces/ITrackingVolumeCreator.h"
#include "TrkVolumes/BoundarySurfaceFace.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// STL
#include <string>
#include <vector>


namespace Trk {

    class ILayerArrayCreator;
    class ITrackingVolumeArrayCreator;
    class ITrackingVolumeHelper;

    class Layer;
    class CylinderLayer;
    class DiscLayer;
    class TrackingVolume;
    class VolumeBounds;
    class CylinderVolumeBounds;
    class Material;

    /** @class CylinderVolumeCreator

      The concrete implementation for cylindrical TrackingVolume
      objects of the ITrackingVolumeCreator interface

      @author Andreas.Salzburger@cern.ch
     */

    class CylinderVolumeCreator : public AthAlgTool, virtual public ITrackingVolumeCreator {

      public:
        /** Constructor */
        CylinderVolumeCreator(const std::string&,const std::string&,const IInterface*);
        /** Destructor */
        ~CylinderVolumeCreator();

        /** AlgTool initialize method */
        StatusCode initialize();
        /** AlgTool finalize method */
        StatusCode finalize();

        /** @copydoc ITrackingVolumeCreator::createTrackingVolume(const std::vector<const Layer*>&, Material&, VolumeBounds*, Amg::Transform3D*,bool, const std::string&) const;
         */
        const TrackingVolume* createTrackingVolume(const std::vector<const Layer*>& layers,
                                                   Material& matprop,
                                                   VolumeBounds* volBounds = 0,
                                                   Amg::Transform3D* transform = 0,
                                                   int entryLayers = 0,
                                                   const std::string& volumeName = "UndefinedVolume",
                                                   BinningType btype = arbitrary) const;

        /** @copydoc ITrackingVolumeCreator::createTrackingVolume(const std::vector<const Layer*>& , Material&, ,double,double,double,double,bool,const std::string&) const;
         */
        const TrackingVolume* createTrackingVolume(const std::vector<const Layer*>& layers,
                                                   Material& matprop,
                                                   double loc1Min, double loc1Max,
                                                   double loc2Min, double loc2Max,
                                                   int entryLayers = 0,
                                                   const std::string& volumeName = "UndefinedVolume",
                                                   BinningType btype = arbitrary) const;

       /** @copydoc ITrackingVolumeCreator::createGapTrackingVolume(Material&,,double,double,double,double,int,bool,const std::string&) const;
        */
        const TrackingVolume* createGapTrackingVolume(
                                              Material& matprop,
                                              double rMin, double rMax,
                                              double zMin, double zMax,
                                              unsigned int materialLayers,
                                              bool cylinder = true,
                                              int entryLayers = 0,
                                              const std::string& volumeName = "UndefinedVolume") const;

        /** @copydoc ITrackingVolumeCreator::createGaoTrackingVolume(Material&,,std::vector<double>&,int,bool,const std::string&) const;
         */
        const TrackingVolume* createGapTrackingVolume(
                                              Material& matprop,
                                              double rMin, double rMax,
                                              double zMin, double zMax,
                                              const std::vector<double>& layerPositions,
                                              bool cylinder = true,
                                              int entryLayers = 0,
                                              const std::string& volumeName = "UndefinedVolume",
                                              BinningType btype = arbitrary) const;

        /** @copydoc ITrackingVolumeCreator::createContainerTrackingVolume(
                const std::vector<const TrackingVolume*>, Material&, ,
                const std::string&) const;
         */
        const TrackingVolume* createContainerTrackingVolume(
                                              const std::vector<const TrackingVolume*>& volumes,
                                              Material& matprop,
                                              const std::string& volumeName ="UndefinedVolume") const;


      private:
        /** Private method - it estimates the CylinderBounds and Translation of layers,
                    if given, these are checked against the layer positions/dimensions. */

        StatusCode estimateAndCheckDimension(const std::vector<const Layer*>& layers,
                                             Trk::CylinderVolumeBounds*& cylBounds,
                                             Amg::Transform3D*& translation,
                                             std::vector<const CylinderLayer*>& cylLayers,
                                             std::vector<const DiscLayer*>& discLayers,
                                             bool estimateSBounds,
                                             Trk::CylinderVolumeBounds*& sensitiveBounds,
                                             double& rMinClean, double& rMaxClean,
                                             double& zMinClean, double& zMaxClean) const;

        /** Private method - interglue all volumes contained by a TrackingVolume
                    and set the outside glue volumes in the descriptor */
        StatusCode interGlueTrackingVolume(TrackingVolume& tVolume,
                                           bool rBinned) const;

        /** Private method - helper method not to duplicate code */
        void addFaceVolumes(const TrackingVolume& tvol,
                            Trk::BoundarySurfaceFace bsf,
                            std::vector<const Trk::TrackingVolume*>& vols) const;

        /** Private method - glue volume to the other -- use trackingVolume helper */
        void glueTrackingVolumes(const TrackingVolume& volumeOne,
                                 BoundarySurfaceFace faceOne,
                                 const TrackingVolume& volumeTwo,
                                 BoundarySurfaceFace faceTwo) const;

        /** Private method - helper method to save some code */
        CylinderLayer* createCylinderLayer(double z,
                                           double r,
                                           double halflength,
                                           double thickness,
                                           int binsPhi,
                                           int binsZ) const;

        /** Private method - helper method to save some code */
        DiscLayer* createDiscLayer(double z,
                                   double rMin, double rMax,
                                   double thickness,
                                   int binsPhi,
                                   int binsR) const;

        // helper tools
        ToolHandle<ILayerArrayCreator>          m_layerArrayCreator;           //!< A Tool for coherent LayerArray creation
        ToolHandle<ITrackingVolumeArrayCreator> m_trackingVolumeArrayCreator;  //!< Helper Tool to create TrackingVolume Arrays
        ToolHandle<ITrackingVolumeHelper>       m_trackingVolumeHelper;        //!< TrackingVolume helper
        // properties for the tracking volume
        double                                  m_innerRenvelopeCover;    //!< tolerance to be added at rMin
        double                                  m_outerRenvelopeCover;    //!< tolerance to be added at rMax
        double                                  m_zMinEnvelopeCover;      //!< tolerance to be added at zMin
        double                                  m_zMaxEnvelopeCover;      //!< tolerance to be added at zMax
        // properties for passive layers
        double                                  m_passiveLayerThickness;  //!< thickness of passive layers
        int                                     m_passiveLayerPhiBins;    //!< bins in phi for the passive layer
        int                                     m_passiveLayerRzBins;     //!< bins in r/z for the passive layer
        // properties for the entry layers
        double                                  m_entryLayerThickness;    //!< thickness of entry layers
        int                                     m_entryLayerPhiBins;      //!< bins in phi for the entry layer
        int                                     m_entryLayerRzBins;       //!< bins in r/z for the entry layer

    };
}

#endif




