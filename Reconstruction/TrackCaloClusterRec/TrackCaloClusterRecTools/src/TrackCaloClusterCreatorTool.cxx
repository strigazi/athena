#include "TrackCaloClusterRecTools/TrackCaloClusterCreatorTool.h"

TrackCaloClusterCreatorTool::TrackCaloClusterCreatorTool(const std::string& t, const std::string& n, const IInterface*  p )
  : AthAlgTool(t,n,p)
{
}

TrackCaloClusterCreatorTool::~TrackCaloClusterCreatorTool() {}

StatusCode TrackCaloClusterCreatorTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode TrackCaloClusterCreatorTool::finalize() {
  return StatusCode::SUCCESS;
}

void TrackCaloClusterCreatorTool::createChargedTCCs(xAOD::TrackCaloClusterContainer* tccContainer, 
						    const xAOD::TrackParticleClusterAssociationContainer* assocContainer, 
						    std::map <const xAOD::TrackParticle*, FourMom_t>* TrackTotalClusterPt, 
						    std::map <const xAOD::CaloCluster*, FourMom_t>* clusterToTracksWeightMap ) {

    for ( const auto* assocClusters : *assocContainer ) {
        ATH_MSG_VERBOSE ("InDetTrackParticlesClusterAssociations index = " << assocClusters->index());
        // flollow the link to the track particle
        const xAOD::TrackParticle* trk = 0;
        xAOD::TrackCaloCluster* tcc = new xAOD::TrackCaloCluster;
        FourMom_t tcc_4p(0.,0.,0.,0.);
        if (assocClusters->trackParticleLink().isValid()) {
            trk = *(assocClusters->trackParticleLink());
        }
        else if ( !assocClusters->trackParticleLink().isValid() ){
            ATH_MSG_ERROR ("trackParticleLink is not valid! " );
        }
        // follow the link to the calorimeter clusters
        ATH_MSG_VERBOSE ("#(CaloCluster) = " << assocClusters->caloClusterLinks().size());
        if (assocClusters->caloClusterLinks().size()) {
            for (size_t c = 0; c < assocClusters->caloClusterLinks().size(); ++c) {
                    const xAOD::CaloCluster* cluster = *(assocClusters->caloClusterLinks().at(c));
                    tcc_4p += cluster->p4()*(( trk->pt() * cluster->pt() / (TrackTotalClusterPt->at(trk).Pt())) / ((clusterToTracksWeightMap->at(cluster)).Pt()));
                    ATH_MSG_VERBOSE ("cluster->pt() " << cluster->pt() << " cluster->eta() " << cluster->eta() << " cluster->phi() " 
		    << cluster->phi() << " track pt " << trk->pt() << " (clusterToTracksWeightMap.at(cluster)).Pt() " << (clusterToTracksWeightMap->at(cluster)).Pt());
            } // for caloClusterLinks
        } // if caloClusterLinks().size
        
        tcc->setP4(tcc_4p.Pt(),trk->eta(),trk->phi(),tcc_4p.M());
        tcc->setTaste(xAOD::TrackCaloCluster::Taste::Charged);
        tccContainer->push_back(tcc);
        ATH_MSG_VERBOSE ("Created TCC with pt " << tcc->pt() << " eta " << tcc->eta() << " phi " << tcc->phi() << " mass " << tcc->m() << " taste " << tcc->getTaste());
    } // for assoc clusters
    
}

void TrackCaloClusterCreatorTool::createNeutralTCCs(xAOD::TrackCaloClusterContainer* tccContainer, 
						    const xAOD::CaloClusterContainer* assocContainer, 
						    std::map <const xAOD::CaloCluster*, FourMom_t>* clusterToTracksWeightMap  ) {

    for ( const auto* cluster : *assocContainer ) {
        if(clusterToTracksWeightMap->find(cluster)==clusterToTracksWeightMap->end()){
            xAOD::TrackCaloCluster* tcc = new xAOD::TrackCaloCluster;
            tccContainer->push_back(tcc);
            tcc->setP4(cluster->pt(),cluster->eta(),cluster->phi(),cluster->m());
            tcc->setTaste(xAOD::TrackCaloCluster::Taste::Neutral);
            ATH_MSG_VERBOSE ("Created TCC with pt " << tcc->pt() << " eta " << tcc->eta() << " phi " << tcc->phi() << " mass " << tcc->m() << " taste " << tcc->getTaste());
        }
    } // for all clusters
    
}

void TrackCaloClusterCreatorTool::createTrackOnlyTCCs(xAOD::TrackCaloClusterContainer* tccContainer, 
						      const xAOD::TrackParticleContainer* assocContainer, 
						      std::map <const xAOD::TrackParticle*, FourMom_t>* TrackTotalClusterPt  ) {

    for ( const auto* track : *assocContainer ) {
        if(TrackTotalClusterPt->find(track)==TrackTotalClusterPt->end()){
            xAOD::TrackCaloCluster* tcc = new xAOD::TrackCaloCluster;
            tccContainer->push_back(tcc);
            tcc->setP4(track->pt(),track->eta(),track->phi(),track->m());
            tcc->setTaste(xAOD::TrackCaloCluster::Taste::TrackOnly);
            ATH_MSG_VERBOSE ("Created TCC with pt " << tcc->pt() << " eta " << tcc->eta() << " phi " << tcc->phi() << " mass " << tcc->m() << " taste " << tcc->getTaste());
        }
    } // for all clusters
    
}

