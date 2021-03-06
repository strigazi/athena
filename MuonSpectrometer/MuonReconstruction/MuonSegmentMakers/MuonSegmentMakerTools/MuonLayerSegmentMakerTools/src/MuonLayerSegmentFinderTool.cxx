/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerSegmentFinderTool.h"

#include "MuonPrepRawDataProviderTools/MuonPrepRawDataCollectionProviderTool.h"
#include "MuonLayerHough/MuonLayerHough.h"
#include "TrkSegment/SegmentCollection.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

namespace Muon {

 MuonLayerSegmentFinderTool::MuonLayerSegmentFinderTool(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_muonPRDSelectionTool("Muon::MuonPRDSelectionTool/MuonPRDSelectionTool"),
    m_segmentMaker("Muon::DCMathSegmentMaker/DCMathSegmentMaker"),
    m_csc2dSegmentFinder("Csc2dSegmentMaker/Csc2dSegmentMaker"),
    m_csc4dSegmentFinder("Csc4dSegmentMaker/Csc4dSegmentMaker"),
    m_clusterSegmentFinder("Muon::MuonClusterSegmentFinder/MuonClusterSegmentFinder"),
    m_clusterSegMakerNSW("Muon::MuonClusterSegmentFinderTool/MuonClusterSegmentFinderTool"),
    m_recoValidationTool("") // ("Muon::MuonRecoValidationTool/MuonRecoValidationTool")

 {
    declareInterface<IMuonLayerSegmentFinderTool>(this);

    declareProperty("MuonEDMPrinterTool",m_printer );
    declareProperty("MuonPRDSelectionTool", m_muonPRDSelectionTool );
    declareProperty("SegmentMaker",m_segmentMaker);
    declareProperty("Csc2DSegmentMaker",m_csc2dSegmentFinder);
    declareProperty("Csc4DSegmentMaker",m_csc4dSegmentFinder);
    declareProperty("MuonClusterSegmentFinder",m_clusterSegmentFinder);
    declareProperty("NSWMuonClusterSegmentFinderTool",m_clusterSegMakerNSW);
    declareProperty("MuonRecoValidationTool",m_recoValidationTool);
  }

  StatusCode MuonLayerSegmentFinderTool::initialize() {
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_muonPRDSelectionTool.retrieve());
    ATH_CHECK(m_segmentMaker.retrieve());
    if (m_idHelperSvc->hasCSC() && !m_csc2dSegmentFinder.empty()) ATH_CHECK(m_csc2dSegmentFinder.retrieve());
    if (m_idHelperSvc->hasCSC() && !m_csc4dSegmentFinder.empty()) ATH_CHECK(m_csc4dSegmentFinder.retrieve());
    ATH_CHECK(m_clusterSegmentFinder.retrieve());
    ATH_CHECK(m_clusterSegMakerNSW.retrieve());
    if( !m_recoValidationTool.empty() ) ATH_CHECK(m_recoValidationTool.retrieve());
    ATH_CHECK(m_houghDataPerSectorVecKey.initialize());
    return StatusCode::SUCCESS;
  }

  void MuonLayerSegmentFinderTool::find( const MuonSystemExtension::Intersection& intersection, std::vector< std::shared_ptr<const Muon::MuonSegment> >& segments, 
					 MuonLayerPrepRawData& layerPrepRawData ) const {

    ATH_MSG_VERBOSE(" Running segment finding in sector " << intersection.layerSurface.sector
                    << " region " << MuonStationIndex::regionName(intersection.layerSurface.regionIndex)
                    << " layer " << MuonStationIndex::layerName(intersection.layerSurface.layerIndex)
                    << " intersection position: r " << intersection.trackParameters->position().perp() << " z " << intersection.trackParameters->position().z()
                    << " locX " << intersection.trackParameters->parameters()[Trk::locX] << " locY " << intersection.trackParameters->parameters()[Trk::locY]
                    << " phi " << intersection.trackParameters->position().phi() );

    // run cluster hit based segment finding on PRDs
    findClusterSegments(intersection,layerPrepRawData,segments);
    ATH_MSG_VERBOSE(" findClusterSegments " << segments.size() );

    // run standard MDT/Trigger hit segment finding either from Hough or hits
    findMdtSegments(intersection,layerPrepRawData,segments);
  }

  void MuonLayerSegmentFinderTool::findMdtSegmentsFromHough( const MuonSystemExtension::Intersection& intersection,
                                                             const MuonLayerPrepRawData& /*layerPrepRawData*/,
                                                             std::vector< std::shared_ptr<const Muon::MuonSegment> >& segments ) const {

    unsigned int nprevSegments = segments.size(); // keep track of what is already there
    int sector = intersection.layerSurface.sector;
    MuonStationIndex::DetectorRegionIndex regionIndex = intersection.layerSurface.regionIndex;
    MuonStationIndex::LayerIndex  layerIndex  = intersection.layerSurface.layerIndex;

    // get hough data
    SG::ReadHandle<MuonLayerHoughTool::HoughDataPerSectorVec> houghDataPerSectorVec {m_houghDataPerSectorVecKey};
    if (!houghDataPerSectorVec.isValid()) {
      ATH_MSG_ERROR("Hough data per sector vector not found");
      return;
    }

    // sanity check
    if( static_cast<int>(houghDataPerSectorVec->vec.size()) <= sector-1 ){
      ATH_MSG_WARNING(" MuonLayerHoughTool::HoughDataPerSectorVec smaller than sector " << houghDataPerSectorVec->vec.size()
                      << " sector " << sector );
      return;
    }

    // get hough maxima in the layer
    unsigned int sectorLayerHash = MuonStationIndex::sectorLayerHash( regionIndex,layerIndex );
    const MuonLayerHoughTool::HoughDataPerSector& houghDataPerSector = houghDataPerSectorVec->vec[sector-1];
    ATH_MSG_DEBUG(" findMdtSegmentsFromHough: sector " << sector << " " << MuonStationIndex::regionName(regionIndex)
                  << " " << MuonStationIndex::layerName(layerIndex) << " sector hash " << sectorLayerHash
                  << " houghData " << houghDataPerSectorVec->vec.size() << " " << houghDataPerSector.maxVec.size());

    // sanity check
    if( houghDataPerSector.maxVec.size() <= sectorLayerHash ){
      ATH_MSG_WARNING(" houghDataPerSector.maxVec.size() smaller than hash " << houghDataPerSector.maxVec.size() << " hash " << sectorLayerHash );
      return;
    }
    const MuonLayerHoughTool::MaximumVec& maxVec = houghDataPerSector.maxVec[sectorLayerHash];

    // get local coordinates in the layer frame
    bool barrelLike = intersection.layerSurface.regionIndex == MuonStationIndex::Barrel;

    float phi = intersection.trackParameters->position().phi();

    // in the endcaps take the r in the sector frame from the local position of the extrapolation
    float r = barrelLike ?
        m_muonSectorMapping.transformRToSector(intersection.trackParameters->position().perp(),phi, intersection.layerSurface.sector,true) :
        intersection.trackParameters->parameters()[Trk::locX];

    float z = intersection.trackParameters->position().z();
    float errx = Amg::error(*intersection.trackParameters->covariance(),Trk::locX);
    float x = barrelLike ? r : z;
    float y = barrelLike ? z : r;
    float theta = atan2(x,y);

    ATH_MSG_DEBUG("  Got Hough maxima " <<  maxVec.size() << " extrapolated position in Hough space (" << x << "," << y
                  << ") error " << errx << " " << " angle " << theta );

    // lambda to handle calibration and selection of MDTs
    auto handleMdt = [this,intersection](const MdtPrepData& prd, std::vector<const MdtDriftCircleOnTrack*>& mdts){
      const MdtDriftCircleOnTrack* mdt = m_muonPRDSelectionTool->calibrateAndSelect( intersection, prd );
      if( mdt ) mdts.push_back(mdt);
    };


    // lambda to handle calibration and selection of clusters
    auto handleCluster = [this,intersection](const MuonCluster& prd, std::vector<const MuonClusterOnTrack*>& clusters){
      const MuonClusterOnTrack* cluster = m_muonPRDSelectionTool->calibrateAndSelect( intersection, prd );
      if( cluster ) clusters.push_back(cluster);
    };



    // loop over maxima and associate them to the extrapolation
    MuonLayerHoughTool::MaximumVec::const_iterator mit = maxVec.begin();
    MuonLayerHoughTool::MaximumVec::const_iterator mit_end = maxVec.end();
    for( ;mit!=mit_end;++mit){
      const MuonHough::MuonLayerHough::Maximum& maximum = **mit;
      float residual = maximum.pos - y;
      float residualTheta = maximum.theta - theta;
      float refPos = (maximum.hough != nullptr) ? maximum.hough->m_descriptor.referencePosition : 0;
      float maxwidth = (maximum.binposmax-maximum.binposmin);
      if( maximum.hough ) maxwidth *= maximum.hough->m_binsize;
      float pull = residual/sqrt(errx*errx+maxwidth*maxwidth/12.);
      
      // fill validation content
      if( !m_recoValidationTool.empty() ) m_recoValidationTool->add( intersection, maximum );

      ATH_MSG_DEBUG("   Hough maximum " << maximum.max << " position (" << refPos
                    << "," << maximum.pos << ") residual " << residual << " pull " << pull
                    << " angle " << maximum.theta << " residual " << residualTheta );

      // select maximum
      if( std::abs(pull) > 5 ) continue;

      // loop over hits in maximum and add them to the hit list
      std::vector<const MdtDriftCircleOnTrack*> mdts;
      std::vector<const MuonClusterOnTrack*>    clusters;
      std::vector<MuonHough::Hit*>::const_iterator hit = maximum.hits.begin();
      std::vector<MuonHough::Hit*>::const_iterator hit_end = maximum.hits.end();
      for( ;hit!=hit_end;++hit ) {

        // treat the case that the hit is a composite TGC hit
        if( (*hit)->tgc ){
          for( const auto& prd : (*hit)->tgc->etaCluster.hitList ) handleCluster(*prd,clusters);
        }else if( (*hit)->prd ){
          Identifier id = (*hit)->prd->identify();
          if( m_idHelperSvc->isMdt(id) ) handleMdt( static_cast<const MdtPrepData&>(*(*hit)->prd),mdts);
          else                        handleCluster( static_cast<const MuonCluster&>(*(*hit)->prd),clusters);
        }
      }

      // get phi hits
      const MuonLayerHoughTool::PhiMaximumVec& phiMaxVec = houghDataPerSector.phiMaxVec[intersection.layerSurface.regionIndex];
      ATH_MSG_DEBUG("   Got Phi Hough maxima " <<  phiMaxVec.size() << " phi " << phi );

      // loop over maxima and associate them to the extrapolation
      MuonLayerHoughTool::PhiMaximumVec::const_iterator pit = phiMaxVec.begin();
      MuonLayerHoughTool::PhiMaximumVec::const_iterator pit_end = phiMaxVec.end();
      for( ;pit!=pit_end;++pit){
        const MuonHough::MuonPhiLayerHough::Maximum& maximum = **pit;
        float residual = maximum.pos - phi;
        if( residual > 2*M_PI )  residual -= 2*M_PI;
        if( residual < -2*M_PI ) residual += 2*M_PI;

        ATH_MSG_DEBUG("     Phi Hough maximum " << maximum.max << " phi " << maximum.pos << ") angle " << maximum.pos
                      << " residual " << residual );

        std::vector<MuonHough::PhiHit*>::const_iterator hit = maximum.hits.begin();
        std::vector<MuonHough::PhiHit*>::const_iterator hit_end = maximum.hits.end();
        for( ;hit!=hit_end;++hit ) {
          // treat the case that the hit is a composite TGC hit
          if( (*hit)->tgc && !(*hit)->tgc->phiCluster.hitList.empty() ){
            Identifier id = (*hit)->tgc->phiCluster.hitList.front()->identify();
            if( m_idHelperSvc->layerIndex(id) != intersection.layerSurface.layerIndex ) continue;
            for( const auto& prd : (*hit)->tgc->phiCluster.hitList ) handleCluster(*prd,clusters);
          }else if( (*hit)->prd ){
            Identifier id = (*hit)->prd->identify();
            if( m_idHelperSvc->layerIndex(id) != intersection.layerSurface.layerIndex ) continue;
            handleCluster( static_cast<const MuonCluster&>(*(*hit)->prd),clusters);
          }
        }
      }

      // call segment finder
      ATH_MSG_DEBUG("    Got hits: mdts " <<  mdts.size() << " clusters " << clusters.size() );
      findMdtSegments(intersection,mdts,clusters,segments);

      // clean-up memory
      for( auto hit : mdts )     delete hit;
      for( auto hit : clusters ) delete hit;
      ATH_MSG_DEBUG("  Done maximum: new segments " <<  segments.size()-nprevSegments );

    }
    ATH_MSG_DEBUG("  Done with layer: new segments " <<  segments.size()-nprevSegments );

    return;
  }


  void MuonLayerSegmentFinderTool::findMdtSegments( const MuonSystemExtension::Intersection& intersection,
                                                    const MuonLayerPrepRawData& layerPrepRawData,
                                                    std::vector< std::shared_ptr<const Muon::MuonSegment> >& segments ) const {
    // calibrate what is already there
    MuonLayerROTs layerROTs;
    if( !m_muonPRDSelectionTool->calibrateAndSelect( intersection, layerPrepRawData, layerROTs ) ){
      ATH_MSG_WARNING("Failed to calibrate and select layer data");
      return;
    }

    // get hits
    MuonStationIndex::TechnologyIndex clusterTech = intersection.layerSurface.regionIndex == MuonStationIndex::Barrel ? MuonStationIndex::RPC : MuonStationIndex::TGC;
    const std::vector<const MdtDriftCircleOnTrack*>& mdts = layerROTs.getMdts();
    const std::vector<const MuonClusterOnTrack*>&    clusters = layerROTs.getClusters(clusterTech);

    findMdtSegments(intersection,mdts,clusters,segments);
  }

  void MuonLayerSegmentFinderTool::findMdtSegments( const MuonSystemExtension::Intersection& intersection,
                                                    const std::vector<const MdtDriftCircleOnTrack*>& mdts,
                                                    const std::vector<const MuonClusterOnTrack*>&    clusters,
                                                    std::vector< std::shared_ptr<const Muon::MuonSegment> >& segments ) const {
    // require at least 2 MDT hits
    if( mdts.size() > 2 ){

      // run segment finder
      std::unique_ptr<Trk::SegmentCollection> segColl(new Trk::SegmentCollection(SG::VIEW_ELEMENTS));
      m_segmentMaker->find( intersection.trackParameters->position(),intersection.trackParameters->momentum(),
			    mdts, clusters,
			    !clusters.empty(), segColl.get(), intersection.trackParameters->momentum().mag() );

      if( segColl ){
	Trk::SegmentCollection::iterator sit = segColl->begin();
	Trk::SegmentCollection::iterator sit_end = segColl->end();
        for( ; sit!=sit_end;++sit){
	  Trk::Segment* tseg=*sit;
	  MuonSegment* mseg=dynamic_cast<MuonSegment*>(tseg);
          ATH_MSG_DEBUG( " " << m_printer->print(*mseg));
          segments.push_back( std::shared_ptr<const MuonSegment>(mseg) );
        }
      }
    }
  }

  void MuonLayerSegmentFinderTool::findClusterSegments( const MuonSystemExtension::Intersection& intersection,
                                                        const MuonLayerPrepRawData& layerPrepRawData,
                                                        std::vector< std::shared_ptr<const Muon::MuonSegment> >& segments ) const {

    // if there are CSC hits run CSC segment finding
    if( !layerPrepRawData.cscs.empty() ) findCscSegments(layerPrepRawData,segments);

    // find TGC segments
    //if( !layerPrepRawData.tgcs.empty() && intersection.layerSurface.layerIndex == MuonStationIndex::Middle ) {
    //  m_clusterSegmentFinder->findSegments(layerPrepRawData.tgcs,segments);
    //}

    if( layerPrepRawData.stgcs.empty() && layerPrepRawData.mms.empty() ) return;

    // NSW segment finding
    MuonLayerROTs layerROTs;
    if( !m_muonPRDSelectionTool->calibrateAndSelect( intersection, layerPrepRawData, layerROTs ) ){
        ATH_MSG_WARNING("Failed to calibrate and select layer data");
       return;
    }

    ATH_MSG_DEBUG( " MM prds " << layerPrepRawData.mms.size() << " STGC prds " << layerPrepRawData.stgcs.size());

     // get STGC and MM clusters
    const std::vector<const MuonClusterOnTrack*>&    clustersSTGC = layerROTs.getClusters(MuonStationIndex::STGC);
    const std::vector<const MuonClusterOnTrack*>&    clustersMM   = layerROTs.getClusters(MuonStationIndex::MM);

    std::vector<const MuonClusterOnTrack*> clusters;
    if( clustersSTGC.size()>0 )  {
       ATH_MSG_DEBUG( " STGC clusters " << clustersSTGC.size());
       for( auto cl : clustersSTGC ){
         clusters.push_back(cl);
       }
    }
    if( clustersMM.size()>0 )  {
       ATH_MSG_DEBUG( " MM clusters " << clustersMM.size());
       for( auto cl : clustersMM ){
         clusters.push_back(cl);
       }
    }

    std::vector<MuonSegment*> foundSegments;
    m_clusterSegMakerNSW->find(clusters,foundSegments);
    if( foundSegments.size()>0 )  {
         for( auto seg : foundSegments ){
            ATH_MSG_DEBUG( " NSW segment " << m_printer->print(*seg) );
            segments.push_back( std::shared_ptr<const MuonSegment>(seg) );
            ATH_MSG_DEBUG( " total segments " << segments.size() );
         }
     }
  }

  void MuonLayerSegmentFinderTool::findCscSegments( const MuonLayerPrepRawData& layerPrepRawData, std::vector< std::shared_ptr<const Muon::MuonSegment> >& segments ) const {

    // run 2d segment finder
    std::unique_ptr<MuonSegmentCombinationCollection> combi2D = m_csc2dSegmentFinder->find(layerPrepRawData.cscs);
    if( combi2D ){

      // run 4d segment finder
      std::unique_ptr<MuonSegmentCombinationCollection> combi4D = m_csc4dSegmentFinder->find( *combi2D );
      if( combi4D ){

        // extract segments and clean-up memory
        for( auto com : *combi4D ){
          const Muon::MuonSegmentCombination& combi = *com;
          unsigned int nstations = combi.numberOfStations();

          // loop over chambers in combi and extract segments
          for(unsigned int i=0; i<nstations; ++i){

            // loop over segments in station
            const Muon::MuonSegmentCombination::SegmentVec* segs = combi.stationSegments( i ) ;

            // check if not empty
            if( !segs || segs->empty() ) continue;
            // loop over new segments, copy them into collection
            Muon::MuonSegmentCombination::SegmentVec::const_iterator sit = segs->begin();
            Muon::MuonSegmentCombination::SegmentVec::const_iterator sit_end = segs->end();
            for( ; sit!=sit_end;++sit){
              ATH_MSG_DEBUG( " " << m_printer->print(**sit) );
              segments.push_back( std::shared_ptr<const MuonSegment>( (*sit)->clone() ) );
            }
          }
        }
      }
    }
  }

}

