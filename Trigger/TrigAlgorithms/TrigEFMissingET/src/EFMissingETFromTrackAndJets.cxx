/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETFromTrackAndJets.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Renjie Wang (renjie.wang@cern.ch)
CREATED:  Nov 18, 2016

// using tracks as soft-term of MET instead of low pt jets in mht algorith
// JVT is also implemented to veto pileup jets in central region
 ********************************************************************/
#include "TrigEFMissingET/EFMissingETFromTrackAndJets.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "CxxUtils/sincosf.h"

#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"
//#include "FourMomUtils/P4DescendingSorters.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

#include "EventKernel/ISignalState.h"
#include "EventKernel/SignalStateHelper.h"

#include <cmath>
#include <string>
using namespace std;

EFMissingETFromTrackAndJets::EFMissingETFromTrackAndJets(const std::string& type,
        const std::string& name,
        const IInterface* parent) :
    EFMissingETBaseTool(type, name, parent),
    m_trackselTool("InDet::InDetTrackSelectionTool/TrackSelectionTool", this )
{
    declareProperty("EtaSeparation", m_etacut = 2.2 ,"Cut to split into forward and central jets -- needs to be positive");
    declareProperty("CentralpTCut", m_central_ptcut = 0.0 ,"pT Cut for central jets");
    declareProperty("ForwardpTCut", m_forward_ptcut = 0.0 ,"pT Cut for forward jets");
    declareProperty("TrackpTCut", m_track_ptcut = 0.0 ,"pT Cut for online tracks");
    declareProperty("CentralJetJVTCut", m_central_jvtcut = 0.9 ,"Jet JVT Cut for central jets");
    declareProperty("TrackSelectionTool", m_trackselTool );

    m_fextype = FexType::JET;
    m_etacut = fabs(m_etacut);

    m_methelperposition = 8;
}


EFMissingETFromTrackAndJets::~EFMissingETFromTrackAndJets()
{
}


StatusCode EFMissingETFromTrackAndJets::initialize()
{
  ATH_MSG_DEBUG( "called EFMissingETFromTrackAndJets::initialize()" );

    /// timers
    if( service( "TrigTimerSvc", m_timersvc).isFailure() )
        ATH_MSG_WARNING( name() << ": Unable to locate TrigTimer Service" );

    if (m_timersvc) {
        // global time
        std::string basename=name()+".TotalTime";
        m_glob_timer = m_timersvc->addItem(basename);
    } // if timing service

    // JVT likelihood histogram
    TString jvtFile = "JVTlikelihood_20140805.root";
    TString jvtName = "JVTRootCore_kNN100trim_pt20to50_Likelihood";
    m_jvtLikelihood = (TH2F *) getHistogramFromFile(jvtName, jvtFile);
    if (m_jvtLikelihood == nullptr) {
        ATH_MSG_ERROR( "Failed to retrieve JVT likelihood file, exiting." );
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}


StatusCode EFMissingETFromTrackAndJets::execute()
{

    ATH_MSG_DEBUG( name() << ": Executing Jet algorithm for ETMiss" );
    return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromTrackAndJets::finalize()
{
  ATH_MSG_DEBUG( "called EFMissingETFromTrackAndJets::finalize()" );


    return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromTrackAndJets::execute(xAOD::TrigMissingET *,
        TrigEFMissingEtHelper *metHelper,
        const xAOD::CaloClusterContainer * /* caloCluster */,
        const xAOD::JetContainer *MHTJetContainer,
        const xAOD::TrackParticleContainer *trackContainer,
        const xAOD::VertexContainer *vertexContainer,
        const xAOD::MuonContainer *muonContainer)
{

  ATH_MSG_DEBUG( "called EFMissingETFromTrackAndJets::execute()" ); // EFMissingET_Fex_Jets

  if(m_timersvc)
    m_glob_timer->start(); // total time

  ATH_MSG_DEBUG( "started MET jet CPU timer" );

  TrigEFMissingEtComponent* metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition); // fetch Jet component

  if (metComp==0) {
    ATH_MSG_ERROR( "cannot fetch Topo. cluster component!" );
    return StatusCode::FAILURE;
  }
  if(string(metComp->m_name, 0, 3) != "JET") {
    ATH_MSG_ERROR( "fetched " << metComp->m_name << " instead of the Jet component!" );
    return StatusCode::FAILURE;
  }


  std::vector<const xAOD::Jet*> MHTJetsVec(MHTJetContainer->begin(), MHTJetContainer->end());
  ATH_MSG_DEBUG( "num of jets: " << MHTJetsVec.size() );

  std::vector<const xAOD::TrackParticle*> TrackVec(trackContainer->begin(), trackContainer->end());
  ATH_MSG_DEBUG( "num of tracks: " << TrackVec.size() );

  std::vector<const xAOD::Vertex*> VertexVec(vertexContainer->begin(), vertexContainer->end());
  ATH_MSG_DEBUG( "num of vertices: " << VertexVec.size() );

  std::vector<const xAOD::Muon*> MuonVec;
  if(muonContainer!=nullptr) {
        for (auto muon : *muonContainer) {
            MuonVec.push_back(muon);
        }
  }
  ATH_MSG_DEBUG( "num of muons: " << MuonVec.size() );


  //#################################################################
  std::vector<const xAOD::TrackParticle*> vecOfMuonTrk;
  for (const xAOD::Muon* muon : MuonVec) {
        const xAOD::Muon::MuonType muontype = muon->muonType();
        // combined or segment tagged muon
        if(muontype == xAOD::Muon::MuonType::Combined || muontype == xAOD::Muon::MuonType::SegmentTagged ) {
            const xAOD::TrackParticle* idtrk = muon->trackParticle( xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle );
            if(idtrk==0) continue;
            if(fabs(muon->pt())<5000) continue;

            ATH_MSG_DEBUG( "Found muon " << "pt = " << muon->pt()/1000. << " eta= " <<
                           muon->eta() << " phi= " << muon->phi() );

	    //check if duplicate muon track 
            bool find_duplicate(false);
            for(std::vector<const xAOD::TrackParticle*>::size_type idx=0; idx<vecOfMuonTrk.size(); idx++) {
                float deltaR_ = idtrk->p4().DeltaR(vecOfMuonTrk[idx]->p4());
                if(deltaR_<0.01) {
                    find_duplicate = true;
                    break;
                }
            }
	    if(find_duplicate) continue;

            vecOfMuonTrk.push_back(idtrk);
        }
  }



  //bool hasGoodVtx = false;
  size_t m_pvind(0);
  const xAOD::Vertex* primaryVertex =  nullptr;
  for (const xAOD::Vertex* vertex : VertexVec) {

    ATH_MSG_DEBUG( "\tx: " << vertex->x() << "\ty: " << vertex->y() << "\tz: " << vertex->z()
                   << "\tntracks: " <<  vertex->nTrackParticles()
                   << "\ttype: " << vertex->vertexType()
                   << "\txAOD::VxType::PriVtx: " << xAOD::VxType::PriVtx );

    // ntracks is not working now in rel21
    // if ( vertex->nTrackParticles() < 2 ) continue;
    if ( vertex->vertexType() == xAOD::VxType::PriVtx ) {
      //hasGoodVtx = true;
      primaryVertex = vertex;
      m_pvind = vertex->index();
      break;
    }

  }



    //having FTK vertex, and find the associated tracks
    std::vector<const xAOD::TrackParticle*> TrackVec_PV;
    TrackVec_PV.clear();
    if(primaryVertex) {
        const std::vector< ElementLink< xAOD::TrackParticleContainer> > tpLinks = primaryVertex->trackParticleLinks();
        ATH_MSG_DEBUG ( "  tpLinks size: " << tpLinks.size());
        if(tpLinks.size() != 0) {
            for(const auto& tp_elem : tpLinks ) {
                if (tp_elem != nullptr && tp_elem.isValid()) {
                    const xAOD::TrackParticle* itrk = *tp_elem;
                    TrackVec_PV.push_back(itrk);
                }
            }
        }
    } // having vertex




  std::vector<const xAOD::Jet*> goodJets;
  goodJets = MHTJetsVec;
  std::vector<const xAOD::TrackParticle*> tracksSeparatedWithJets;


  if(primaryVertex) {

    unsigned int n_putracks = 0;
    for (const xAOD::TrackParticle* itrk : TrackVec) {
        if (itrk == nullptr) continue;

        bool isfromPV(false);
        for( const xAOD::TrackParticle* itrkPV : TrackVec_PV ) {
            float deltaR_ = itrk->p4().DeltaR(itrkPV->p4());
            if(deltaR_ < 0.01) {
                isfromPV=true;
                break;
            }
        }

      if(!isfromPV && itrk->pt()<30e3 && m_trackselTool->accept(*itrk,primaryVertex)) n_putracks++;
    } // end for loop over tracks
    if (!n_putracks) n_putracks++;



    //########################################
    // calculation of R_{pt}^i for each for each jet
    // R_{pt}^i == Sum{trk} pT_trk (PVi) / pT_j
    // Dr_pt = { R_{pt}^i_max - R_{pt}^i_medium } / pT_j
    //########################################

    std::map<const xAOD::Jet*, std::pair<double, std::vector<double> > > JVTRpt_jets;
    JVTRpt_jets.clear();


    goodJets.clear();
    for (const xAOD::Jet* jet : MHTJetsVec) {

      double ptsum_all = 0;
      double ptsum_pv = 0;
      double ptsum_pileup = 0;

      for (const xAOD::TrackParticle* itrk : TrackVec) {

        float deltaR_trackj = jet->p4().DeltaR(itrk->p4());
        if(deltaR_trackj>0.4) continue;

        bool isfromPV(false);
        for( const xAOD::TrackParticle* itrkPV : TrackVec_PV ) {
            float deltaR_ = itrk->p4().DeltaR(itrkPV->p4());
            if(deltaR_ < 0.01) {
                isfromPV=true;
                break;
            }
        }

        bool accept = (itrk->pt()>500 && m_trackselTool->accept(*itrk, primaryVertex));
        if (accept) ptsum_all += itrk->pt();
        if (accept && isfromPV) ptsum_pv += itrk->pt();
        if (accept && !isfromPV) ptsum_pileup += itrk->pt();
      }
      //double JVF = ptsum_all>0 ? ptsum_pv/ptsum_all : -1;
      double Rpt = ptsum_pv/jet->pt();
      double corrJVF = ptsum_pv+ptsum_pileup>0 ? ptsum_pv/(ptsum_pv+100*ptsum_pileup/n_putracks) : -1;
      double JVT = corrJVF>=0 ? m_jvtLikelihood->Interpolate(corrJVF,std::min(Rpt,1.0)) : -0.1;


      //############# forward jvt #################

        std::vector<double> sumPtTrkPt500;
        sumPtTrkPt500.clear();
        for(const xAOD::Vertex* vx : VertexVec) {
            if(vx->vertexType()!=xAOD::VxType::PriVtx && vx->vertexType()!=xAOD::VxType::PileUp) continue;
            sumPtTrkPt500.push_back(0);
        }

        for( const xAOD::TrackParticle* itrk : TrackVec ) {
            if(itrk->pt()<500) continue;
            
            //find track associated within jets
            float deltaR_trackj = jet->p4().DeltaR(itrk->p4());
            if(deltaR_trackj>0.4) continue;
            
            //looking the best matched tracks
            bool found_matched_track(false);
            int  found_matched_vertex(-1);
            for(const xAOD::Vertex* vx : VertexVec) {
                if(vx->vertexType()!=xAOD::VxType::PriVtx && vx->vertexType()!=xAOD::VxType::PileUp) continue;
                const std::vector< ElementLink< xAOD::TrackParticleContainer> > tpLinks = vx->trackParticleLinks();
                if(tpLinks.size() != 0) {
                    for(const auto& tp_elem : tpLinks ) {
                        if (tp_elem != nullptr && tp_elem.isValid()) {
                            const xAOD::TrackParticle* ivtx_trk = *tp_elem;
                            float deltaR_trk_trk = itrk->p4().DeltaR(ivtx_trk->p4());
                            if(deltaR_trk_trk<0.01) {
                                found_matched_track=true;
                                break;
                            }
                        }
                    }
                }
                if(found_matched_track) {
                    found_matched_vertex = vx->index();
                    break;
                }
            }
            
            //std::cout << "sumPtTrkPt500 size: " << sumPtTrkPt500.size() << std::endl;
            if(found_matched_track) {
                double tmp_val = sumPtTrkPt500[found_matched_vertex];
                sumPtTrkPt500[found_matched_vertex] = tmp_val+itrk->pt();
            }

        }

        std::vector<double> Rpt_ijet;
        Rpt_ijet.clear();

        for (size_t i = 0; i < sumPtTrkPt500.size(); i++) {
            Rpt_ijet.push_back( sumPtTrkPt500[i]/jet->pt() );
        }

        std::pair<double, std::vector<double> > JVT_Rpt_ijet;
        JVT_Rpt_ijet = std::make_pair (JVT, Rpt_ijet);

        JVTRpt_jets[jet] = JVT_Rpt_ijet;

    }


    // forward JVT calculation
    std::vector<TVector2> m_pileupMomenta;
    m_pileupMomenta.clear();
    for(const xAOD::Vertex* vx : VertexVec) {
        if(vx->vertexType()!=xAOD::VxType::PriVtx && vx->vertexType()!=xAOD::VxType::PileUp) continue;

        float trk_x(0), trk_y(0);
        const std::vector< ElementLink< xAOD::TrackParticleContainer> > tpLinks = vx->trackParticleLinks();
        if(tpLinks.size() != 0) {
            for(const auto& tp_elem : tpLinks ) {
                if (tp_elem != nullptr && tp_elem.isValid()) {
                    const xAOD::TrackParticle* itrk = *tp_elem;
                    trk_x += itrk->pt()*cos(itrk->phi());
                    trk_y += itrk->pt()*sin(itrk->phi());
                }
            }
        }

        float m_jetScaleFactor         = 0.4;
        m_pileupMomenta.push_back((vx->index()==m_pvind?0:-(1./m_jetScaleFactor))*TVector2(0.5*trk_x,0.5*trk_y));
    }
    //std::cout << "m_pileupMomenta size " << m_pileupMomenta.size() << std::endl;


    for (std::map<const xAOD::Jet*, std::pair<double, std::vector<double> > >::iterator it=JVTRpt_jets.begin(); it!=JVTRpt_jets.end(); ++it) {
        const xAOD::Jet* jet = it->first;
        double JVT_ijet = it->second.first;
        std::vector<double> Rpt_ijet = it->second.second;
        
        if ( !centralJet(jet, JVT_ijet, Rpt_ijet) ) continue;
        
        double firstVal = 0;
        int bestMatchVertex = -1;
        for (size_t i = 0; i < Rpt_ijet.size(); i++) {
            if (Rpt_ijet[i]>firstVal) {
                bestMatchVertex = i;
                firstVal = Rpt_ijet[i];
            }
        }
        if (bestMatchVertex>=0) m_pileupMomenta[bestMatchVertex] += TVector2(-0.5*jet->pt()*cos(jet->phi()),-0.5*jet->pt()*sin(jet->phi()));
    }



    for (std::map<const xAOD::Jet*, std::pair<double, std::vector<double> > >::iterator it=JVTRpt_jets.begin(); it!=JVTRpt_jets.end(); ++it) {
        const xAOD::Jet* jet = it->first;
        double JVT_ijet = it->second.first;
        std::vector<double> Rpt_ijet = it->second.second;


        //forward JVT
        double fJVT_ijet = 0;
        if( forwardJet(jet) ) {
            TVector2 fjet(jet->pt()*cos(jet->phi()),jet->pt()*sin(jet->phi()));
            for (size_t pui = 0; pui < m_pileupMomenta.size(); pui++) {
                if (pui==m_pvind) continue;
                double projection = m_pileupMomenta[pui].Px() * jet->pt()*cos(jet->phi()) + m_pileupMomenta[pui].Py() * jet->pt()*sin(jet->phi());
                projection /= fjet.Mod2();

                if (projection>fJVT_ijet) fJVT_ijet = projection;
            }
        }


      bool isCentralPUjets(false);
      if(jet->pt()<50e3 && jet->pt()>20e3 && fabs(jet->eta())<2.4 && JVT_ijet<m_central_jvtcut) isCentralPUjets = true;

      bool isForwardPUjets(false);
      if(fJVT_ijet>0.2) isForwardPUjets = true;

      //###########################################
      if(!isCentralPUjets && !isForwardPUjets) goodJets.push_back(jet);
    }






    for (const xAOD::TrackParticle* track : TrackVec) {

      //checking the track coming from PV
        bool isfromPV(false);
        for( const xAOD::TrackParticle* itrkPV : TrackVec_PV ) {
            float deltaR_ = track->p4().DeltaR(itrkPV->p4());
            if(deltaR_ < 0.01) {
                isfromPV=true;
                break;
            }
        }

      if(!isfromPV) continue;
      if(fabs(track->eta())>2.4 || track->pt()/1000. < m_track_ptcut) continue;
      if(!m_trackselTool->accept(*track,primaryVertex)) continue;
            
      //remove muon tracks
      float mindeltaR_trackj(999.);
      for (const xAOD::TrackParticle* muontrk: vecOfMuonTrk) {
      	  float deltaR_trackj = track->p4().DeltaR(muontrk->p4());
      	  if(deltaR_trackj<mindeltaR_trackj) mindeltaR_trackj=deltaR_trackj;
      }
      if(mindeltaR_trackj<0.1) continue;


      ATH_MSG_DEBUG( "\ttrack pt: " << track->pt()/1000. << "\teta: " << track->eta() << "\tphi: " << track->phi()
                     << "\tvertex: " << track->vertex()
                     << "\tz0: " << track->z0()
                     << "\tvz: " << track->vz()
                     << "\ttheta: " << track->theta()
                     << "\tdZ: " << fabs((track->z0()+track->vz()-primaryVertex->z())*sin(track->theta()))
                     << "\tisfromPV: " << isfromPV );

      bool findWithinjets(false);
      for (const xAOD::Jet* jet : goodJets) {
        if( fabs(jet->eta())<2.4 && jet->pt()/1000. < m_central_ptcut) continue;
        float deltaR_trackj = track->p4().DeltaR(jet->p4());
        if(deltaR_trackj<0.4) {
          findWithinjets = true;
          break;
        }
      }

      if(!findWithinjets) {
        tracksSeparatedWithJets.push_back(track);
      }

    }

  }
  //##################################################################

  //--- fetching the topo. cluster component
  float upperlim[4] = {m_etacut,0,5,-m_etacut};
  float lowerlim[4] = {0,-m_etacut,m_etacut,-5};

  for(int i = 0; i < 5; i++) {

    metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition + i); // fetch Cluster component

    for (const xAOD::TrackParticle* track : tracksSeparatedWithJets) {
        metComp->m_ex -= track->p4().Px();
        metComp->m_ey -= track->p4().Py();
        metComp->m_ez -= track->p4().Pz();
        metComp->m_sumEt += track->pt();
        metComp->m_sumE  += track->e();
    }

    for (const xAOD::Jet* aJet : goodJets) {

      if( fabs(aJet->eta())<2.4 && aJet->pt()/1000 < m_central_ptcut ) continue;

      if(i == 0) {
        metComp->m_ex -= aJet->px();
        metComp->m_ey -= aJet->py();
        metComp->m_ez -= aJet->pz();
        metComp->m_sumEt += aJet->pt();
        metComp->m_sumE  += aJet->e();
        metComp->m_usedChannels += 1;
        metComp->m_sumOfSigns += copysign(1.0, aJet->pt() );
      } else if (i > 0) {
        float eta = aJet->eta();
        if( eta >= lowerlim[i-1] && eta <= upperlim[i-1]) {
          metComp->m_ex -= aJet->px();
          metComp->m_ey -= aJet->py();
          metComp->m_ez -= aJet->pz();
          metComp->m_sumEt += aJet->pt();
          metComp->m_sumE  += aJet->e();
          metComp->m_usedChannels += 1;
          metComp->m_sumOfSigns += copysign(1.0, aJet->pt() );
        }

      }

    } // End loop over all jets

    // move from "processing" to "processed" state
    metComp->m_status ^= m_maskProcessing; // switch off bit
    metComp->m_status |= m_maskProcessed;  // switch on bit

  }

  metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition); // fetch Cluster component

  ATH_MSG_DEBUG( " calculated MET: " << sqrt((metComp->m_ex)*(metComp->m_ex)+(metComp->m_ey)*(metComp->m_ey)) );


  if(m_timersvc)
    m_glob_timer->stop(); // total time

  return StatusCode::SUCCESS;
}

TH1* EFMissingETFromTrackAndJets::getHistogramFromFile(TString hname, TString fname)
{
  fname = PathResolverFindCalibFile(fname.Data());
  TFile *file = TFile::Open(fname.Data(), "READ");

  if (file == nullptr) {
    ATH_MSG_DEBUG( "getHistogramFromFile() : Couldn't open file " << fname.Data() << ", returning nullptr." );
    return nullptr;
  }

  TH1 *temp = (TH1*)file->Get(hname.Data());

  if (temp == nullptr) {
    ATH_MSG_DEBUG( "getHistogramFromFile() : Couldn't find histogram " << hname.Data() << " in file " << fname.Data() << ", returning nullptr." );
    return nullptr;
  }

  hname = "cloned_" + hname;
  TH1 *hist = (TH1*)temp->Clone(hname.Data());
  hist->SetDirectory(0);
  SafeDelete(file);

  return hist;
}






bool EFMissingETFromTrackAndJets::forwardJet(const xAOD::Jet *jet) const
{
    double m_etaThresh          = 2.4;
    double m_forwardMinPt       = 20e3;
    double m_forwardMaxPt       = 50e3;
    if (fabs(jet->eta())<m_etaThresh) return false;
    if (jet->pt()<m_forwardMinPt || jet->pt()>m_forwardMaxPt) return false;
    return true;
}


bool EFMissingETFromTrackAndJets::centralJet(const xAOD::Jet *jet, float jvt, std::vector<double> sumpts) const
{

    double m_etaThresh          = 2.4;
    double m_centerMinPt        = 20e3;
    double m_centerMaxPt        = -1;
    double m_centerJvtThresh    = 0.14;
    double m_maxStochPt         = 35e3;
    double m_centerDrptThresh   = 0.2;

    if (fabs(jet->eta())>m_etaThresh) return false;
    if (jet->pt()<m_centerMinPt || (m_centerMaxPt>0 && jet->pt()>m_centerMaxPt)) return false;
    if (jvt>m_centerJvtThresh) return false;
    if (jet->pt()<m_maxStochPt && getDrpt(jet,sumpts)<m_centerDrptThresh) return false;
    return true;
}


float EFMissingETFromTrackAndJets::getDrpt(const xAOD::Jet *jet, std::vector<double> sumpts) const
{
    if (sumpts.size()<2) return 0;
    std::vector<double> sumpts_;
    for (size_t i=0; i<sumpts.size(); i++) {
        if(sumpts[i]>0) sumpts_.push_back( sumpts[i] );
    }

    std::sort (sumpts_.begin(), sumpts_.end(), std::greater<float>());
    if (sumpts_.size()<2) return 0;
    double median = sumpts_[sumpts_.size()/2];
    double max = sumpts_[0];
    return (max-median)/jet->pt();
}



