/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// KalmanFitter
//   Source file for class KalmanFitter
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Markus.Elsing@cern.ch
///////////////////////////////////////////////////////////////////

#include "TrkKalmanFitter/KalmanFitter.h"

#include "GaudiKernel/ListItem.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkParameters/TrackParameters.h"

#include "TrkValInterfaces/IValidationNtupleTool.h"
#include "TrkDetDescrInterfaces/IAlignableSurfaceProvider.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkFitterInterfaces/IMeasurementRecalibrator.h"
#include "TrkEventUtils/PrepRawDataComparisonFunction.h"
#include "TrkEventUtils/MeasurementBaseComparisonFunction.h"
#include "TrkEventUtils/IdentifierExtractor.h"
#include "TrkFitterUtils/MeasBaseIndexComparisonFunction.h"
#include "TrkFitterUtils/DNA_MaterialEffects.h"
#include "TrkFitterUtils/ProtoTrajectoryUtility.h"
#include "TrkFitterUtils/TrackFitInputPreparator.h"
#include "TrkFitterUtils/KalmanMatEffectsController.h"

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

#include <algorithm>
#include <cmath>


Trk::KalmanFitter::KalmanFitter(const std::string& t,const std::string& n,
                                const IInterface* p) :
  AthAlgTool (t,n,p),
  m_log(msgSvc(), n),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_updator("Trk::KalmanUpdator/KalmanUpdator"),
  m_ROTcreator("Trk::RIO_OnTrackCreator/RIO_OnTrackCreator"),
  m_dynamicNoiseAdjustor(""),
  m_brempointAnalyser(""),
  m_alignableSfProvider(""),
  m_recalibrator(""),
  m_internalDAF(), // or "Trk::KalmanPiecewiseAnnealingFilter/KalmanInternalDAF"
  m_forwardFitter("Trk::ForwardKalmanFitter/FKF"),
  m_smoother("Trk::KalmanSmoother/BKS"),
  m_outlierLogic("Trk::KalmanOutlierLogic/KOL"),
  m_outlierRecovery("Trk::KalmanOutlierRecovery_InDet/KOL_RecoveryID"),
  m_FitterValidationTool(""),
  m_option_callValidationToolForFailedFitsOnly(false),
  m_option_sortingRefPoint{0.,0.,0.},
  m_callValidationTool(false),
  m_haveValidationTool(false),
  m_doDNAForElectronsOnly(false),
  m_tparScaleSetter(nullptr),
  m_sortingRefPoint(0., 0., 0.),
  m_idHelper(nullptr),
  m_utility(nullptr),
  m_inputPreparator(nullptr),
  m_cov0(std::vector<double>(0)),
  m_fitStatus(Trk::FitterStatusCode::BadInput),
  m_maximalNdof(0),
  m_fitStatistics(0),
  m_chiSquaredAfb{}, m_chiSquaredAfbNontriviality{}
{
  declareInterface<ITrackFitter>( this );
  
  m_trajectory.reserve(100);
  
  // --- tools used by KalmanFitter, passed as ToolHandles
  declareProperty("ExtrapolatorHandle",m_extrapolator,
                  "Extrapolation tool for transporting track pars and handling material effects");
  declareProperty("RIO_OnTrackCreatorHandle",m_ROTcreator,
                  "Tool to create RIO_OnTrack out of PrepRawData input");
  declareProperty("MeasurementUpdatorHandle",m_updator,
                  "Tool to perform measurement update and chi2 calculation");
  declareProperty("DynamicNoiseAdjustorHandle",m_dynamicNoiseAdjustor,
                  "Tool to handle brem as dynamically adjusted q/p noise");
  declareProperty("BrempointAnalyserHandle",m_brempointAnalyser,
                  "Tool to confirm if DNA activity is due to brem or not");
  declareProperty("AlignableSurfaceProviderHandle",m_alignableSfProvider,
                  "Tool to replace measurement surface by an alignable one");
  declareProperty("RecalibratorHandle", m_recalibrator);
  declareProperty("InternalDAFHandle", m_internalDAF);
  declareProperty("ForwardKalmanFitterHandle",m_forwardFitter,
                  "Tool for running the forward filter along the internal trajectory");
  declareProperty("KalmanSmootherHandle",m_smoother,
                  "Tool for performing the backward smoothing on the internal trajectory");
  declareProperty("KalmanOutlierLogicHandle",m_outlierLogic,
                  "Tool for fit quality analysis and outlier flagging");
  declareProperty("KalmanOutlierRecoveryHandle",m_outlierRecovery,
                  "Tool for fit quality analysis and outlier recovery");
  declareProperty("FitterValidationToolHandle", m_FitterValidationTool,
                  "Tool for fitter validation (writes intermediate results to ntuple)");

  // -- job options - do NOT modify defaults to achieve detector-specific tuning!
  declareProperty("DoDNAForElectronsOnly",m_doDNAForElectronsOnly,
		  "for new ID bremfit: DNA active only for tracks with particleHypothesis electron");
  declareProperty("DoHitSorting",m_option_enforceSorting=false,
                  "Enables hit sorting along the reference direction when the KF starts");
  declareProperty("DoSmoothing",m_option_doSmoothing=true,
                  "Enables the backward filter to produce smoothed track parameters at every surface");
  declareProperty("SortingReferencePoint",m_option_sortingRefPoint,
                  "Ref. point for hit pre-sorting and identifying reference parameters on a track");
  declareProperty("MaxNumberOfFitIterations",m_option_max_N_iterations=4,
                  "The max number of iterations when removing outliers");
  declareProperty("CalculatePerigeeAtOrigin",m_option_PerigeeAtOrigin=true,
                  "Make reference parameters as Perigee near origin");
  declareProperty("ReintegrateOutliers", m_option_reintegrateOutliers=false,
                  "Enables resetting all outlier flags in track refits");
  declareProperty("InitialCovariances", m_cov0);
  declareProperty("DoValidationAction",m_option_doValidationAction=false,
                  "Enables calling validation methods in the extrapolation engine");
  declareProperty("CallValidationToolForFailedFitsOnly",    m_option_callValidationToolForFailedFitsOnly,
                  "Call the validation tool given by FitterValidationToolHandle only for failed fits");

}

// destructor
Trk::KalmanFitter::~KalmanFitter()
{}

// initialize
StatusCode Trk::KalmanFitter::initialize()
{
  //AthAlgTool::initialize().ignore();
  m_log.setLevel(msgLevel());            // individual outputlevel not known before initialise
  
  // get all the track fitter sub-tools (REQUIRED tools, obviously)
  ATH_CHECK(m_forwardFitter.retrieve());
  ATH_CHECK(m_smoother.retrieve());
  if (m_outlierLogic.empty() || m_outlierLogic.retrieve().isFailure()) {
    ATH_MSG_FATAL ("can not retrieve outlier logic of type " << m_outlierLogic.typeAndName());
    return StatusCode::FAILURE;
  }

  if (m_forwardFitter->needsReferenceTrajectory()) m_outlierRecovery.setTypeAndName("");
  if (!m_outlierRecovery.empty() && m_outlierRecovery.retrieve().isFailure()) {
    ATH_MSG_FATAL ("can not retrieve outlier recovery of type " << m_outlierRecovery.typeAndName());
    return StatusCode::FAILURE;
  }

  // get bread-and-butter components, extrapolator+updator (REQUIRED tools)
  if (m_updator.empty() || m_updator.retrieve().isFailure()) {
    ATH_MSG_FATAL ("can not retrieve meas't updator of type " << m_updator.typeAndName());
    return StatusCode::FAILURE;
  } ATH_MSG_INFO ("retrieved tool " << m_updator.typeAndName());

  if (m_extrapolator.empty() || m_extrapolator.retrieve().isFailure()) {
    ATH_MSG_FATAL ("can not retrieve extrapolator of type " << m_extrapolator.typeAndName());
    return StatusCode::FAILURE;
  } ATH_MSG_INFO ("retrieved tool " << m_extrapolator.typeAndName());
  
  // --- get ROT creator (OPTIONAL tool)
  if (!m_ROTcreator.empty()) {
    if (m_ROTcreator.retrieve().isFailure()) {
      ATH_MSG_FATAL("can not retrieve ROT creator of type " << m_ROTcreator.typeAndName());
      return StatusCode::FAILURE;
    } ATH_MSG_INFO ("retrieved tool " << m_ROTcreator.typeAndName());
  }

  // --- get DynamicNoiseAdjustor for brem fits (OPTIONAL tools)
  if (!m_dynamicNoiseAdjustor.empty()) {
    if (m_dynamicNoiseAdjustor.retrieve().isFailure()) {
      ATH_MSG_ERROR ("DNA is configured but tool is not accessible - " << m_dynamicNoiseAdjustor.typeAndName());
      return StatusCode::FAILURE;
    } ATH_MSG_INFO ("retrieved tool for electron noise model " << m_dynamicNoiseAdjustor.typeAndName());
  }
  if (!m_brempointAnalyser.empty()) {
    if (m_brempointAnalyser.retrieve().isFailure()) {
      ATH_MSG_ERROR ("DNA separator/brempoint analyser is configured but not accessible - "
		     << m_brempointAnalyser.typeAndName());
      return StatusCode::FAILURE;
    } ATH_MSG_INFO ("retrieved tool " << m_brempointAnalyser.typeAndName() );
  }

  // --- get AlignableSurfaceProvider (OPTIONAL tool)
  if (!m_alignableSfProvider.empty()) {
    if (m_alignableSfProvider.retrieve().isFailure()) {
      ATH_MSG_ERROR( "AlignableSfPrv is configured but tool is not accessible - "<< m_alignableSfProvider.typeAndName() );
      return StatusCode::FAILURE;
    } ATH_MSG_DEBUG( "retrieved tool " << m_alignableSfProvider.typeAndName());
  }

  // Get recalibrator, if it exists it also flags re-calibration (OPTIONAL tool)
  if (!m_recalibrator.empty()) {
    ATH_MSG_INFO( "will re-create RIO_OnTracks through a Recalibrator.");
    if (m_recalibrator.retrieve().isFailure()) {
      ATH_MSG_ERROR( "can not retrieve configured recalibrator of type " << m_recalibrator.typeAndName() );
      return StatusCode::FAILURE;
    } ATH_MSG_DEBUG("retrieved tool " << m_recalibrator.typeAndName() );
  } else ATH_MSG_INFO("RIO_OnTracks will be preserved and not recalibrated unless PRD given as input." );
  if (m_ROTcreator.empty() && !m_recalibrator.empty()) {
    ATH_MSG_ERROR( "can not demand re-calibration without configured RIO_OnTrackCreator!" );
    return StatusCode::FAILURE;
  }

  // Get tool to switch to a different concept during parts of fwd filter, eg DAF for L/R
  if (!m_internalDAF.empty() && m_internalDAF.retrieve().isFailure()) {
    ATH_MSG_FATAL( "can not retrieve internal annealing filter tool "<< m_internalDAF.typeAndName() );
    return StatusCode::FAILURE;
  }
  if (!m_internalDAF.empty()) {
    ATH_MSG_INFO( "retrieved " << m_internalDAF.typeAndName()<< " (Kalman internal piecewise DAF)");
  }


  // Get Validation Tool (OPTIONAL tool)
  if ( ! m_FitterValidationTool.empty() ) {
      StatusCode sc = m_FitterValidationTool.retrieve();
      if (sc.isFailure()) {
          ATH_MSG_FATAL( "Could not retrieve validation tool: "<< m_FitterValidationTool );
          return sc;
      }
      m_haveValidationTool = true;
      if (!m_option_callValidationToolForFailedFitsOnly) m_callValidationTool = true;
  } else {
    ATH_MSG_DEBUG ("No Tool for validation given.");
  }

  m_sortingRefPoint[0] = m_option_sortingRefPoint[0];
  m_sortingRefPoint[1] = m_option_sortingRefPoint[1];
  m_sortingRefPoint[2] = m_option_sortingRefPoint[2];
  m_tparScaleSetter = new Trk::TrkParametersComparisonFunction(m_sortingRefPoint);
  if (m_option_enforceSorting) {
    ATH_MSG_INFO( "fitter inputs will be verified and sorted along momentum direction." );
    ATH_MSG_INFO( "if used, a sorting reference point is given as "
        << m_sortingRefPoint.x() << ", " << m_sortingRefPoint.y() << ", "
        << m_sortingRefPoint.z() << "."  );
  } else ATH_MSG_INFO ("fitter inputs are assumed to be always ordered in the direction along the track.");

  ATH_MSG_INFO ("Reference parameters will be calculated at the "
		<< (m_option_PerigeeAtOrigin ? "physics origin":"input reference surface"));

  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    ATH_MSG_ERROR ("Could not get AtlasDetectorID helper" );
    return StatusCode::FAILURE;
  }
  // helper for subdetector identifiaction in internal DAF and outlier logics
  m_utility = new ProtoTrajectoryUtility(m_idHelper);

  // main helper to convert different type of inputs into unified trajectory
  m_inputPreparator = new TrackFitInputPreparator(m_sortingRefPoint,
                                                  m_forwardFitter->needsReferenceTrajectory()?
                                                  (&(*m_extrapolator)) : nullptr);

  // configure internal DAF
  if (!m_internalDAF.empty() && 
      m_internalDAF->configureWithTools((!m_extrapolator.empty()?(&(*m_extrapolator)):nullptr),
                                        (!m_updator.empty()?(&(*m_updator)):nullptr),
                                        (!m_recalibrator.empty()?(&(*m_recalibrator)):nullptr),
                                        m_utility                                       ).isFailure()) {
    ATH_MSG_WARNING ("failure while configuring internal DAF!");
    ATH_MSG_INFO ("debugging info: ex = " << m_extrapolator << " up = " << m_updator << 
		  " rc " << m_recalibrator<<" ut = " << m_utility);
    return StatusCode::FAILURE;
  }

  // configure ForwardKalmanFitter
  StatusCode sc = m_forwardFitter->configureWithTools((!m_extrapolator.empty()?(&(*m_extrapolator)):nullptr),
						     (!m_updator.empty()?(&(*m_updator)):nullptr),
						     (!m_ROTcreator.empty()?(&(*m_ROTcreator)):nullptr),
						     (!m_dynamicNoiseAdjustor.empty()?(&(*m_dynamicNoiseAdjustor)):nullptr),
						     (!m_alignableSfProvider.empty()?(&(*m_alignableSfProvider)):nullptr),
						     (!m_recalibrator.empty()?(&(*m_recalibrator)):nullptr),
						     nullptr // for hybrid-fitter use: (!m_internalDAF.empty()?(&(*m_internalDAF)):0)
						     );
  if(sc.isFailure()) return sc;

  // configure KalmanSmoother
  sc = m_smoother->configureWithTools((!m_extrapolator.empty()?(&(*m_extrapolator)):nullptr),
                                     (!m_updator.empty()?(&(*m_updator)):nullptr),
                                     (!m_dynamicNoiseAdjustor.empty()?(&(*m_dynamicNoiseAdjustor)):nullptr),
                                     (!m_alignableSfProvider.empty()?(&(*m_alignableSfProvider)):nullptr),
                                     m_option_doSmoothing, true);
  if(sc.isFailure()) return sc;

  // configure outlier logic
  sc = m_outlierLogic->configureWithTools(&(*m_extrapolator),
                                         &(*m_updator),
                                         (!m_recalibrator.empty()?(&(*m_recalibrator)):nullptr));
  if(sc.isFailure()) return sc;

  // configure outlier logic
  if (!m_outlierRecovery.empty()) {
    sc = m_outlierRecovery->configureWithTools(&(*m_extrapolator),
                                              &(*m_updator),
                                              (!m_recalibrator.empty()?(&(*m_recalibrator)):nullptr));
    if(sc.isFailure()) return sc;
  }

  // configure DNA tool (if present)
  if (!m_dynamicNoiseAdjustor.empty()) {
    sc = m_dynamicNoiseAdjustor->configureWithTools((!m_extrapolator.empty()?(&(*m_extrapolator)):nullptr),
                                                   (!m_updator.empty()?(&(*m_updator)):nullptr));
    if(sc.isFailure()) return sc;
  }
  if (!m_dynamicNoiseAdjustor.empty() && !m_alignableSfProvider.empty()) {
    ATH_MSG_ERROR ("Dynamic Noise Adjustment and Alignable Surface Provider can not be both active "
		   <<"at the same time!\nWill need interface change to work with both.");
    return StatusCode::FAILURE;
  }

  if (!m_option_doSmoothing) {
    ATH_MSG_WARNING ("doSmoothing is off: the smoothed track parameters at every detector will not be calculated.\nnot recommended to use in conjunction with running outlier removal.");
  }
  if (m_option_max_N_iterations < 1 ) {
    m_option_max_N_iterations = 3;
    ATH_MSG_WARNING ("Bad number configured for maximum fit iterations -> set to 3");
  }

  if (m_option_reintegrateOutliers) {
    ATH_MSG_INFO ("outliers on an input Trk::Track will be refitted.");
  } else {
    ATH_MSG_INFO ("outliers on an input Trk::Track will stay outliers.");
  }

  if (m_cov0.size() != 5) {
    m_cov0 = {250., 250.,0.25, 0.25, 0.000001};
  }
  ATH_MSG_INFO ("Initial covariance, set with the KF: " << m_cov0);

  
  std::vector<int> statVec(nStatIndex, 0);
  m_fitStatistics.resize(nFitStatsCodes, statVec);
  m_chiSquaredAfb.fill(0.);
  m_chiSquaredAfbNontriviality.fill(0.);
  ATH_MSG_INFO ("initialize() successful in " << name() << " ("<<PACKAGE_VERSION<<")");
  return StatusCode::SUCCESS;
}

// finalize
StatusCode Trk::KalmanFitter::finalize()
{
  delete m_tparScaleSetter;
  delete m_utility;
  delete m_inputPreparator;
  
  if (msgLvl(MSG::INFO)) {
    std::stringstream ss;
    int iw=9;
    ss << "-------------------------------------------------------------------------------\n" ;
    ss<< "  track fits by eta range          ------All---Barrel---Trans.-- Endcap-- \n" ;
    std::vector<std::string> statusNames{
    "  Number of fitter calls          :",
    "  Number of successful track fits :",
    "  Number of fits w/ seed recovery :",
    "  failed fits not recovered by DAF:",
    "  Number of fits with electron DNA:",
    "  Number of fits with brem found  :",
    "  Number of straight line fits    :",
    "  fits with failed updator maths  :",
    "  fits with failed extrapolation  :",
    "  fits w/ failed outlier strategy :",
    "  insufficient meas'ts after KOL  :",
    "  fits w/ failed Perigee making   :",
    "  fits w/ bad input or bad logic  :"};
    for (unsigned int i=0; i<statusNames.size(); i++) {
      ss << (statusNames[i]) << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< (m_fitStatistics[i])[iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< (m_fitStatistics[i])[iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< (m_fitStatistics[i])[iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< (m_fitStatistics[i])[iEndcap] << "\n";
    }
    ss << "-------------------------------------------------------------------------------\n";

    ss<< "  fw-bw filter chi2 asymmetry     :" << std::setiosflags(std::ios::dec) << std::setw(iw)
             << m_chiSquaredAfb[Trk::KalmanFitter::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
             << m_chiSquaredAfb[Trk::KalmanFitter::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
             << m_chiSquaredAfb[Trk::KalmanFitter::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
             << m_chiSquaredAfb[Trk::KalmanFitter::iEndcap] << "\n";
    ss<< "  absolute fw-bw chi2 asymmetry   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
             << m_chiSquaredAfbNontriviality[Trk::KalmanFitter::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
             << m_chiSquaredAfbNontriviality[Trk::KalmanFitter::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
             << m_chiSquaredAfbNontriviality[Trk::KalmanFitter::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
             << m_chiSquaredAfbNontriviality[Trk::KalmanFitter::iEndcap] << "\n";
    ss << "-------------------------------------------------------------------------------\n";
    ATH_MSG_INFO (name() << "'s fit statistics:\n"<<ss.str());
  }
  
  ATH_MSG_INFO ("finalize() successful in " << name());
  return StatusCode::SUCCESS;
}


// refit a track
// -------------------------------------------------------
std::unique_ptr<Trk::Track>
Trk::KalmanFitter::fit(const EventContext& ctx,
                       const Trk::Track& inputTrack,
                       const RunOutlierRemoval runOutlier,
                       const Trk::ParticleHypothesis prtHypothesis) const
{
  m_fitStatus = Trk::FitterStatusCode::BadInput;
  ATH_MSG_VERBOSE ("--> enter KalmanFitter::fit(Track,,)    with Track from author = "
		   << inputTrack.info().dumpInfo());

  // protection against not having measurements on the input track
  if (!inputTrack.trackStateOnSurfaces() || inputTrack.trackStateOnSurfaces()->size() < 2) {
    ATH_MSG_INFO ("called to refit empty track or track with too little information, reject fit");
    return nullptr;
  }

  /*  determine the Track Parameter which is the start of the trajectory,
      i.e. closest to the reference point */
  if (!inputTrack.trackParameters() || inputTrack.trackParameters()->empty()) {
    ATH_MSG_INFO ("input fails to provide track parameters for seeding the KF, reject fit");
    return nullptr;
  }
  if (msgLvl(MSG::VERBOSE)) {
    ATH_MSG_VERBOSE( "list of parameters as they are on the input track:");
    DataVector<const TrackParameters>::const_iterator it
      = inputTrack.trackParameters()->begin();
    for(int i=0 ; it!=inputTrack.trackParameters()->end(); ++it, ++i)
      ATH_MSG_VERBOSE( "TrackPar" << (i<10 ? "  " : " ") << i 
            << " position mag : " << (*it)->position().mag()
            << ", to ref is " << ((*it)->position()-m_sortingRefPoint).mag());
      ATH_MSG_VERBOSE( "Now getting track parameters near origin " 
                              << (m_option_enforceSorting? "via STL sort" : "as first TP (convention)"));
  }
  // fill internal trajectory through external preparator class
  const TrackParameters* minPar = nullptr;
  if (m_inputPreparator->copyToTrajectory(m_trajectory, minPar,
                                          inputTrack,m_option_enforceSorting,
                                          m_option_reintegrateOutliers, prtHypothesis)
      == StatusCode::FAILURE) {
    ATH_MSG_WARNING ("Could not decode input track!");
    m_trajectory.clear(); return nullptr;
  }
  m_utility->identifyMeasurements(m_trajectory);
  m_maximalNdof = m_utility->rankedNumberOfMeasurements(m_trajectory)-5;
  ATH_MSG_VERBOSE( "These TrackPars are chosen as seed: "<<*minPar);
  bool doDNA = !m_dynamicNoiseAdjustor.empty();
  if (m_doDNAForElectronsOnly && prtHypothesis != Trk::electron) doDNA = false;
  Trk::KalmanMatEffectsController kalMec(prtHypothesis, doDNA);
  ATH_MSG_VERBOSE( "set up MatEffController with PH " << prtHypothesis << ", have-DNA "
         << ( !m_dynamicNoiseAdjustor.empty() ?
              ( kalMec.aggressiveDNA()?"yes, aggressive tuning":"yes, generic tuning"):"no."));
  /* start the Kalman filtering */
  float this_eta=0.0;     // statistics
  if (msgLvl(MSG::DEBUG)) {
    this_eta = minPar->eta();
    monitorTrackFits( Trk::KalmanFitter::Call, this_eta );
  }
  
  // --- perform first forward filter on measurement set extracted from track
  ATH_MSG_VERBOSE ("\n***** call forward kalman filter, iteration # 1 *****\n");
  if (m_forwardFitter->enterSeedIntoTrajectory(m_trajectory,*minPar,m_cov0,kalMec,true)
      != Trk::FitterStatusCode::Success) return nullptr;
  m_fitStatus = m_forwardFitter->fit(m_trajectory,*minPar,runOutlier,kalMec,true);
  if (m_callValidationTool)
    callValidation(ctx, 0, kalMec.particleType(), m_fitStatus);

  // call KalmanFilter with iterations on the outliers
  FitQuality* fitQual  = nullptr;
  if (iterateKalmanFilter(ctx, minPar, fitQual, runOutlier, kalMec, this_eta) ||
      invokeAnnealingFilter(minPar, fitQual, runOutlier, kalMec, this_eta)) {
    // make output track from the internal trajectory
    assert( fitQual );
    Track* fittedTrack =
      makeTrack(ctx, fitQual, *minPar, &kalMec, this_eta, &(inputTrack.info()));
    m_trajectory.clear();
    if (!fittedTrack) delete fitQual;
    return std::unique_ptr<Trk::Track>(fittedTrack);
  } 
    delete fitQual;
    m_trajectory.clear();
    // iterations failed:
    //    if m_option_callValidationToolForFailedFitsOnly repeat the track fit with calls of validation tool
    if (m_option_callValidationToolForFailedFitsOnly && (!m_callValidationTool) && m_haveValidationTool) {
        m_callValidationTool = true;
        if (fit(inputTrack, runOutlier, kalMec.particleType())) {
            ATH_MSG_WARNING( "Error: fit succeeded! Should not happen, if we repeat a failed fit!" );
        }
        m_callValidationTool = false;
    }
    ATH_MSG_DEBUG( "fit(track) during iterations failed." );
    return nullptr;
  
}


// fit a set of PrepRawData objects
// --------------------------------
std::unique_ptr<Trk::Track>
Trk::KalmanFitter::fit(const EventContext& ctx,
                       const Trk::PrepRawDataSet& inputPRDColl,
                       const Trk::TrackParameters& estimatedStartParameters,
                       const RunOutlierRemoval runOutlier,
                       const Trk::ParticleHypothesis prtHypothesis) const
{
  m_fitStatus = Trk::FitterStatusCode::BadInput;
  if (!check_operability(1, runOutlier, prtHypothesis,inputPRDColl.empty())) return nullptr;
  bool doDNA = !m_dynamicNoiseAdjustor.empty();
  if (m_doDNAForElectronsOnly && prtHypothesis != Trk::electron) doDNA = false;
  Trk::KalmanMatEffectsController kalMec(prtHypothesis, doDNA);

  if (m_option_enforceSorting) {
    // sort PrepRawData Set in increasing distance from origin using STL sorting
    Trk::PrepRawDataComparisonFunction* PRD_CompFunc = new Trk::PrepRawDataComparisonFunction
      (estimatedStartParameters.position(),
       estimatedStartParameters.momentum());
    PrepRawDataSet orderedPRDColl = PrepRawDataSet(inputPRDColl); // wouldn't be able to sort on const Set
    if ( ! is_sorted( orderedPRDColl.begin(), orderedPRDColl.end(), *PRD_CompFunc ) ) {
      std::sort( orderedPRDColl.begin(), orderedPRDColl.end(), *PRD_CompFunc );
    };
    delete PRD_CompFunc;
    if (msgLvl(MSG::VERBOSE)) {
      ATH_MSG_VERBOSE(" Sorting passed, sorted list by distance to ref point:" );
      PrepRawDataSet::const_iterator it1    = orderedPRDColl.begin();
      for( ; it1!=orderedPRDColl.end(); it1++) {
        if ( !(*it1)->detectorElement() ) {
          ATH_MSG_ERROR( "corrupt data - PrepRawData has no element link.\n"
          << "The track fitter won't help you here -> segfault expected." );
        } else {
          ATH_MSG_ERROR( " radius of PRD detElement's GPos is "
          << (*it1)->detectorElement()->surface( (*it1)->identify() ).center().mag() << ", transverse r "
                      <<  (*it1)->detectorElement()->surface( (*it1)->identify() ).center().perp() );
        }
      }
    } // end if output to be made

    // run forward kalman fit, including ROT creation
    ATH_MSG_VERBOSE("call forward kalman filter" ) ;
    m_fitStatus = m_forwardFitter->fit(m_trajectory, orderedPRDColl,
                                       estimatedStartParameters, runOutlier, kalMec);
  } else {
    // run forward kalman fit, including ROT creation
    ATH_MSG_VERBOSE( "call forward kalman filter" );
    m_fitStatus = m_forwardFitter->fit(m_trajectory, inputPRDColl,
                                       estimatedStartParameters, runOutlier, kalMec);
  }
  if (m_callValidationTool)
    callValidation(ctx, 0, kalMec.particleType(), m_fitStatus);
  float this_eta=0.0;     // statistics
  m_maximalNdof = m_utility->rankedNumberOfMeasurements(m_trajectory)-5;
  if (msgLvl(MSG::DEBUG)) {
    this_eta = estimatedStartParameters.eta();
    monitorTrackFits( Trk::KalmanFitter::Call, this_eta );
  }

  // call KalmanFilter with iterations on the outliers
  const Trk::TrackParameters* startPar = &estimatedStartParameters;
  FitQuality* fitQual  = nullptr;
  if (iterateKalmanFilter(
        ctx, startPar, fitQual, runOutlier, kalMec, this_eta) ||
      invokeAnnealingFilter(startPar, fitQual, runOutlier, kalMec, this_eta)) {
    // make output track from the internal trajectory
    assert( fitQual );
    Track* fittedTrack =
      makeTrack(ctx, fitQual, *startPar, &kalMec, this_eta, nullptr);
    m_trajectory.clear();
    if (!fittedTrack) delete fitQual;
    return std::unique_ptr<Trk::Track>(fittedTrack);
  } 
    delete fitQual;
    m_trajectory.clear();
    return nullptr;
  
}

// fit a set of MeasurementBase objects
// --------------------------------
std::unique_ptr<Trk::Track>
Trk::KalmanFitter::fit(const EventContext& ctx,
                       const Trk::MeasurementSet& inputMeasSet,
                       const Trk::TrackParameters& estimatedStartParameters,
                       const RunOutlierRemoval runOutlier,
                       const Trk::ParticleHypothesis matEffects) const
{
  m_fitStatus = Trk::FitterStatusCode::BadInput;
  if (!check_operability(2 ,runOutlier, matEffects,inputMeasSet.empty())) return nullptr;
  bool doDNA = !m_dynamicNoiseAdjustor.empty();
  if (m_doDNAForElectronsOnly && matEffects != Trk::electron) doDNA = false;
  Trk::KalmanMatEffectsController kalMec(matEffects, doDNA);
  ATH_MSG_VERBOSE ("set up MatEffController with PH " << matEffects << ", have-DNA "
		   << ( doDNA ? ( kalMec.aggressiveDNA()?"yes, aggressive tuning":"yes, generic tuning"):"no."));

  // --- prepare input set (sorting, start-parameters)

  if (m_option_enforceSorting) {
    // input vector is const, so copy it before sorting.
    bool worstCaseSorting = // invest n*(logN)**2 sorting time only for lowPt
      std::abs(estimatedStartParameters.parameters()[Trk::qOverP]) > 0.002;
    MeasurementSet sortedHitSet = MeasurementSet(inputMeasSet);
    Trk::MeasurementBaseComparisonFunction* MeasB_CompFunc
      = new Trk::MeasurementBaseComparisonFunction(estimatedStartParameters.position(),
						   estimatedStartParameters.momentum());
    // sort ROTColl in increasing distance from origin using STL sorting
    if ( ! is_sorted( sortedHitSet.begin(), sortedHitSet.end(), *MeasB_CompFunc ) ) {
      worstCaseSorting ?
      stable_sort( sortedHitSet.begin(), sortedHitSet.end(), *MeasB_CompFunc ):
      sort( sortedHitSet.begin(), sortedHitSet.end(), *MeasB_CompFunc ) ;
    };
    // some debug output
    if (msgLvl(MSG::VERBOSE)){
      ATH_MSG_VERBOSE ("-K- The list of MeasurementBase has been ordered along the initial direction.");
      MeasurementSet::const_iterator it1    = sortedHitSet.begin();
      MeasurementSet::const_iterator it1End = sortedHitSet.end();
      for( ; it1!=it1End; ++it1) {
	      ATH_MSG_VERBOSE( "-K- globalPos() magnitude is " 
	      << (*it1)->globalPosition().mag() << ", transverse r "
	      << (*it1)->globalPosition().perp() );
      }
    }
    delete MeasB_CompFunc;
    
    // fill measurements into fitter-internal trajectory: no outlier, external meas't
    MeasurementSet::const_iterator it    = sortedHitSet.begin();
    MeasurementSet::const_iterator itEnd = sortedHitSet.end();
    for(int istate=1 ; it!=itEnd; it++, istate++) {
      m_trajectory.push_back(ProtoTrackStateOnSurface((*it),false,false,istate));
      m_trajectory.back().identifier(Trk::IdentifierExtractor::extract(*it));
    }
  } else {
    MeasurementSet::const_iterator it    = inputMeasSet.begin();
    MeasurementSet::const_iterator itEnd = inputMeasSet.end();
    for(int istate=1 ; it!=itEnd; it++, istate++) {
      m_trajectory.push_back(ProtoTrackStateOnSurface((*it),false,false,istate));
      m_trajectory.back().identifier(Trk::IdentifierExtractor::extract(*it));
    }
  }
  float this_eta=0.0;     // statistics
  m_maximalNdof = m_utility->rankedNumberOfMeasurements(m_trajectory)-5;
  if (msgLvl(MSG::DEBUG)) {
    this_eta = estimatedStartParameters.eta();
    monitorTrackFits( Trk::KalmanFitter::Call, this_eta );
  }
  
    ATH_MSG_VERBOSE( "These TrackPars are chosen as seed: "<<estimatedStartParameters);

  /* --- perform first forward filter on measurement set. Assume that clients who
     call this interface never want re-making of the ROTs. Recalibrate = false.*/

  if (m_forwardFitter->enterSeedIntoTrajectory(m_trajectory,estimatedStartParameters,
                                               m_cov0,kalMec,true)
      != Trk::FitterStatusCode::Success) return nullptr;
  ATH_MSG_VERBOSE ("\n***** call forward kalman filter, iteration # 1 *****\n");
  m_fitStatus = m_forwardFitter->fit(m_trajectory, estimatedStartParameters,
                                     runOutlier, kalMec, false);
  if (m_callValidationTool) callValidation(ctx,0, kalMec.particleType(), m_fitStatus);

  // --- call KalmanFilter with iterations on the outliers
  const Trk::TrackParameters* startPar = &estimatedStartParameters;
  FitQuality* fitQual  = nullptr;
  if (iterateKalmanFilter(
        ctx, startPar, fitQual, runOutlier, kalMec, this_eta) ||
      invokeAnnealingFilter(startPar, fitQual, runOutlier, kalMec, this_eta)) {
    // make output track from the internal trajectory:
    Track* fittedTrack =
      makeTrack(ctx, fitQual, *startPar, &kalMec, this_eta, nullptr);
    m_trajectory.clear();
    if (!fittedTrack) delete fitQual;
    return std::unique_ptr<Trk::Track>(fittedTrack);
  } 
    if (fitQual) delete fitQual;
    m_trajectory.clear();
    // iterations failed:
    //    if m_option_callValidationToolForFailedFitsOnly repeat the track fit with calls of validation tool
    if (m_option_callValidationToolForFailedFitsOnly && (!m_callValidationTool) && m_haveValidationTool) {
        m_callValidationTool = true;
        if (fit(inputMeasSet, estimatedStartParameters, runOutlier, kalMec.particleType())) {
            ATH_MSG_WARNING( "Error: fit succeeded! Should not happen, if we repeat a failed fit!" );
        }
        m_callValidationTool = false;
    }
    ATH_MSG_DEBUG( "fit(vec<MB>) during iteration failed." );
    return nullptr;
  
}

// extend a track fit to include an additional set of PrepRawData objects
// --------------------------------
std::unique_ptr<Trk::Track>
Trk::KalmanFitter::fit(const EventContext& ctx,
                       const Trk::Track& inputTrack,
                       const Trk::PrepRawDataSet& addPrdColl,
                       const Trk::RunOutlierRemoval runOutlier,
                       const Trk::ParticleHypothesis matEffects) const
{
  m_fitStatus = Trk::FitterStatusCode::BadInput;
  ATH_MSG_VERBOSE ("--> enter KalmanFitter::fit(Track,PrdSet,,)");
  ATH_MSG_VERBOSE ("    with Track from author = " << inputTrack.info().dumpInfo());
  
  // protection, if empty PrepRawDataSet
  if (addPrdColl.empty()) {
    ATH_MSG_WARNING ("client tries to add an empty PrepRawDataSet to the track fit.");
    return fit(ctx,inputTrack, runOutlier, matEffects);
  }

  /*  determine the Track Parameter which is the start of the trajectory,
      i.e. closest to the reference point */
  ATH_MSG_VERBOSE ("get track parameters near origin " 
                   << (m_option_enforceSorting? "via STL sort" : "from 1st state"));
  const TrackParameters* estimatedStartParameters = m_option_enforceSorting
    ?  *(std::min_element(inputTrack.trackParameters()->begin(),
                          inputTrack.trackParameters()->end(),
                          *m_tparScaleSetter))
    :  *inputTrack.trackParameters()->begin() ;


  // use external preparator class to prepare PRD set for fitter interface
  PrepRawDataSet orderedPRDColl = 
    m_inputPreparator->stripPrepRawData(inputTrack,addPrdColl,m_option_enforceSorting,
                                        true /* do not lose outliers! */);
  std::unique_ptr<Trk::Track> fittedTrack =
    fit(ctx, orderedPRDColl, *estimatedStartParameters, runOutlier, matEffects);
  const TrackInfo& existingInfo = inputTrack.info(); 
  if (fittedTrack) fittedTrack->info().addPatternRecoAndProperties(existingInfo);
  return fittedTrack;
}

// extend a track fit to include an additional set of MeasurementBase objects
// re-implements the TrkFitterUtils/TrackFitter.cxx general code in a more
// mem efficient and stable way
// --------------------------------
std::unique_ptr<Trk::Track>
Trk::KalmanFitter::fit(const EventContext& ctx,
                       const Trk::Track& inputTrack,
                       const Trk::MeasurementSet& addMeasColl,
                       const Trk::RunOutlierRemoval runOutlier,
                       const Trk::ParticleHypothesis matEffects) const
{
  m_fitStatus = Trk::FitterStatusCode::BadInput;
  ATH_MSG_VERBOSE ("--> enter KalmanFitter::fit(Track,Meas'BaseSet,,)");
  ATH_MSG_VERBOSE ("    with Track from author = " << inputTrack.info().dumpInfo());
  
  // protection, if empty MeasurementSet
  if (addMeasColl.empty()) {
    ATH_MSG_WARNING( "client tries to add an empty MeasurementSet to the track fit." );
    return fit(ctx,inputTrack, runOutlier, matEffects);
  }

  // fill internal trajectory through external preparator class
  const TrackParameters* estimatedStartParameters = nullptr;
  if (m_inputPreparator->copyToTrajectory(m_trajectory, estimatedStartParameters,
                                          inputTrack,addMeasColl,
                                          m_option_enforceSorting,
                                          m_option_reintegrateOutliers, matEffects)
      == StatusCode::FAILURE) {
    ATH_MSG_WARNING( "Could not decode input track!" );
    m_trajectory.clear(); return nullptr;
  }
  m_utility->identifyMeasurements(m_trajectory);
  m_maximalNdof = m_utility->rankedNumberOfMeasurements(m_trajectory)-5;
  ATH_MSG_VERBOSE( "These TrackPars are chosen as seed: "
         <<*estimatedStartParameters);

  float this_eta=0.0;     // statistics
  if (msgLvl(MSG::DEBUG)) {
    this_eta = estimatedStartParameters->eta();
    monitorTrackFits( Trk::KalmanFitter::Call, this_eta );
  }
  bool doDNA = !m_dynamicNoiseAdjustor.empty();
  if (m_doDNAForElectronsOnly && matEffects != Trk::electron) doDNA = false;
  Trk::KalmanMatEffectsController kalMec(matEffects, doDNA);
  ATH_MSG_VERBOSE("set up MatEffController with PH " << matEffects << ", have-DNA "
         << ( !m_dynamicNoiseAdjustor.empty() ?
              ( kalMec.aggressiveDNA()?"yes, aggressive tuning":"yes, generic tuning"):"no.")) ;

  // --- perform first forward filter on measurement set extracted from track
  if (m_forwardFitter->enterSeedIntoTrajectory(m_trajectory,*estimatedStartParameters,m_cov0,
                                               kalMec,true)
      != Trk::FitterStatusCode::Success) return nullptr;
  ATH_MSG_VERBOSE ("\n***** call forward kalman filter, iteration # 1 *****\n");
  m_fitStatus = m_forwardFitter->fit(m_trajectory, *estimatedStartParameters,
                                     runOutlier, kalMec, true);
  if (m_callValidationTool)
    callValidation(ctx, 0, kalMec.particleType(), m_fitStatus);

  // --- call KalmanFilter with iterations on the outliers
  FitQuality* fitQual  = nullptr;
  if (iterateKalmanFilter(
        ctx, estimatedStartParameters, fitQual, runOutlier, kalMec, this_eta) ||
      invokeAnnealingFilter(
        estimatedStartParameters, fitQual, runOutlier, kalMec, this_eta)) {
    // make output track from the internal trajectory:
    assert( fitQual);
    Track* fittedTrack = makeTrack(ctx,
                                   fitQual,
                                   *estimatedStartParameters,
                                   &kalMec,
                                   this_eta,
                                   &(inputTrack.info()));
    m_trajectory.clear();
    if (!fittedTrack) delete fitQual;
    return std::unique_ptr<Trk::Track>(fittedTrack);
  } 
    delete fitQual;
    fitQual = nullptr;
    m_trajectory.clear();
    // iterations failed:
    //    if m_option_callValidationToolForFailedFitsOnly repeat the track fit with calls of validation tool
    if (m_option_callValidationToolForFailedFitsOnly && (!m_callValidationTool) && m_haveValidationTool) {
        m_callValidationTool = true;
        std::unique_ptr<Trk::Track> fittedAgainTrack =
          fit(ctx, inputTrack, addMeasColl, runOutlier, kalMec.particleType());
        if (fittedAgainTrack) {
          ATH_MSG_WARNING ("inconsistent: fit succeeded! Should not happen if we repeat a failed fit!");
        }
        m_callValidationTool = false;
    }
    ATH_MSG_DEBUG ("fit(track,vec<MB>) during iteration failed.");
    return nullptr;
  
}

// combined fit of two tracks
// --------------------------------
std::unique_ptr<Trk::Track>
Trk::KalmanFitter::fit(const EventContext& ctx,
                       const Trk::Track& intrk1,
                       const Trk::Track& intrk2,
                       const Trk::RunOutlierRemoval runOutlier,
                       const Trk::ParticleHypothesis matEffects) const
{
  m_fitStatus = Trk::FitterStatusCode::BadInput;
  ATH_MSG_VERBOSE ("--> enter KalmanFitter::fit(Track,Track,)");
  ATH_MSG_VERBOSE ("    with Tracks from #1 = " << intrk1.info().dumpInfo() 
                   << " and #2 = " << intrk2.info().dumpInfo());

  // protection against not having measurements on the input tracks
  if (!intrk1.trackStateOnSurfaces() || intrk2.trackStateOnSurfaces() ||
      intrk1.trackStateOnSurfaces()->size() < 2) {
    ATH_MSG_WARNING ("called to refit empty track or track with too little information,"
                     << " reject fit");
    return nullptr;
  }

  /*  determine the Track Parameter which is the start of the trajectory,
      i.e. closest to the reference point */
  ATH_MSG_VERBOSE( "get track parameters near origin " 
                              << (m_option_enforceSorting? "via STL sort" : "as first TP (convention)"));
  if (!intrk1.trackParameters() || intrk1.trackParameters()->empty()) {
    ATH_MSG_WARNING( "input #1 fails to provide track parameters for seeding the "
          << "KF, reject fit" );
    return nullptr;
  }

  // fill internal trajectory through external preparator class
  const TrackParameters* minPar = nullptr;
  if (m_inputPreparator->copyToTrajectory(m_trajectory, minPar, 
                                          intrk1,intrk2,m_option_enforceSorting,
                                          m_option_reintegrateOutliers, matEffects)
      == StatusCode::FAILURE) {
    ATH_MSG_WARNING( "Could not decode input tracks!" );
    m_trajectory.clear(); return nullptr;
  }
  m_utility->identifyMeasurements(m_trajectory);
  m_maximalNdof = m_utility->rankedNumberOfMeasurements(m_trajectory)-5;
  bool doDNA = !m_dynamicNoiseAdjustor.empty();
  if (m_doDNAForElectronsOnly && matEffects != Trk::electron) doDNA = false;
  Trk::KalmanMatEffectsController kalMec(matEffects, doDNA);
  ATH_MSG_VERBOSE( "set up MatEffController with PH " << matEffects << ", have-DNA "
         << ( !m_dynamicNoiseAdjustor.empty() ?
              ( kalMec.aggressiveDNA()?"yes, aggressive tuning":"yes, generic tuning"):"no."));

  ATH_MSG_VERBOSE( "These TrackPars are chosen as seed: "<<*minPar);

  /* start the Kalman filtering */
  float this_eta=0.0;     // statistics
  if (msgLvl(MSG::DEBUG)) {
    this_eta = minPar->eta();
    monitorTrackFits( Trk::KalmanFitter::Call, this_eta );
  }
  
  // --- perform first forward filter on measurement set extracted from track
  if (m_forwardFitter->enterSeedIntoTrajectory(m_trajectory,*minPar,m_cov0,kalMec,true)
      != Trk::FitterStatusCode::Success) return nullptr;
  ATH_MSG_VERBOSE ("\n***** call forward kalman filter, iteration # 1 *****\n");
  m_fitStatus = m_forwardFitter->fit(m_trajectory,*minPar,runOutlier, kalMec, true);
  if (m_callValidationTool)
    callValidation(ctx, 0, kalMec.particleType(), m_fitStatus);

  // call KalmanFilter with iterations on the outliers
  FitQuality* fitQual  = nullptr;
  if (iterateKalmanFilter(ctx, minPar, fitQual, runOutlier, kalMec, this_eta) ||
      invokeAnnealingFilter(minPar, fitQual, runOutlier, kalMec, this_eta)) {
    // make output track from the internal trajectory
    assert( fitQual );
    Track* fittedTrack =
      makeTrack(ctx, fitQual, *minPar, &kalMec, this_eta, &(intrk1.info()));
    m_trajectory.clear();
    if (!fittedTrack) delete fitQual;
    const TrackInfo& existingInfo2 = intrk2.info(); 
    if (fittedTrack) fittedTrack->info().addPatternReco(existingInfo2);
    return std::unique_ptr<Trk::Track>(fittedTrack);
  } 
    delete fitQual;
    m_trajectory.clear();
    // iterations failed:
    //    if m_option_callValidationToolForFailedFitsOnly repeat the track fit with calls of validation tool
    if (m_option_callValidationToolForFailedFitsOnly && (!m_callValidationTool) && m_haveValidationTool) {
        m_callValidationTool = true;
        if (fit(ctx, intrk1, intrk2, runOutlier, kalMec.particleType())) {
          ATH_MSG_WARNING ("Error: fit succeeded! Should not happen, if we repeat a failed fit!");
        }
        m_callValidationTool = false;
    }
    return nullptr;
  
}

// Main internal iteration logic for all interfaces:
// run fwd-filter, smoother and then evaluate results with DNA, outliers, local pattern
// ---------------------------------------------------------------------------------
bool Trk::KalmanFitter::iterateKalmanFilter(const EventContext& ctx,
                                            const Trk::TrackParameters*&  startPar,
                                            Trk::FitQuality*&       newFitQuality,
                                            const RunOutlierRemoval       runOutlier,
                                            const Trk::KalmanMatEffectsController& kalMec,
                                            const double& /*this_eta*/ ) const
{
  int nOutlierIterations  = 0; // monitor fit progress: # iterations, success of sub-tools
  int iFilterBeginState   = 0; // point forward filter to other than first state
  //  m_haveUsedDafInThisFit  = false;
  bool fitter_is_ready = false;

  while (!fitter_is_ready) {
    // run forward kalman fit - but the first time is done in the calling method.
    if (++nOutlierIterations>1) {
      this->prepareNextIteration(nOutlierIterations,newFitQuality,iFilterBeginState, *startPar);
      ATH_MSG_VERBOSE ("\n********** call forward kalman filter, iteration #"
		       << nOutlierIterations << " **********");
      // do not recalibrate a second time -- DIFF TO OTHER INTERFACES
      m_fitStatus = m_forwardFitter->fit(m_trajectory, *startPar, runOutlier, kalMec,
                                         /*allowRecalibration=*/false, iFilterBeginState);
      // call validation tool if provided
      if (m_callValidationTool)
        callValidation(ctx,(nOutlierIterations - 1) * 2,
                       kalMec.particleType(),
                       m_fitStatus);
    }
    if (msgLvl(MSG::VERBOSE)) m_utility->dumpTrajectory(m_trajectory, name());

    // protect against failed fit
    if (m_fitStatus.isFailure()) {
      ATH_MSG_VERBOSE ("fit.failure: error status=" << m_fitStatus.getString());
      ATH_MSG_DEBUG   ("forward fitter #" << nOutlierIterations << " rejected fit\n\n");
      //      if (msgLvl(MSG::INFO)) monitorTrackFits( m_NfailFwfilter, this_eta );
      return false;
    }
    ATH_MSG_VERBOSE ("\n***** Forward fit passed, now call smoother #"<<
                     nOutlierIterations<<". *****\n");

    // run backward smoother
    if (m_forwardFitter->needsReferenceTrajectory())
      m_fitStatus = m_smoother->fitWithReference(m_trajectory, newFitQuality, kalMec);
    else 
      m_fitStatus = m_smoother->fit(m_trajectory, newFitQuality, kalMec);
    // call validation tool if provided
    if (m_callValidationTool)
      callValidation(ctx, (nOutlierIterations - 1) * 2 + 1,
                     kalMec.particleType(),
                     m_fitStatus);

    // protect against failed fit
    if (m_fitStatus.isFailure()) {
      ATH_MSG_VERBOSE ("fit.failure: error status=" << m_fitStatus.getString());
      ATH_MSG_DEBUG   ("smoother #" << nOutlierIterations << " rejected fit\n");
      if (m_option_doValidationAction) m_extrapolator->validationAction();
      return false;
    }
    ATH_MSG_VERBOSE ("\n************ Backward smoother #" << nOutlierIterations
		     << " passed. ************* \n" );

    if (m_option_doValidationAction) m_extrapolator->validationAction();

    if (nOutlierIterations==1 && !m_utility->trajectoryHasMefot(m_trajectory)
        && !m_brempointAnalyser.empty() 
        && !kalMec.aggressiveDNA()
        && kalMec.breakpointType()==Trk::BreakpointNotSpecified ) {
      kalMec.updateBreakpoint(Trk::NoBreakpoint);
      ATH_MSG_VERBOSE ("DNA separator spoke: 1 (no breakpoint, in fact DNA-separator is not called.");
    }

    // --- evaluate dna after 1st iteration: keep if breakpoint is real, or undo+refit if not useful
    if (nOutlierIterations==1 && m_utility->trajectoryHasMefot(m_trajectory)
        && !m_brempointAnalyser.empty()
        && !kalMec.aggressiveDNA()
        && kalMec.breakpointType() == Trk::BreakpointNotSpecified) {
      Trk::TrackBreakpointType dnaSeparation
        = m_brempointAnalyser->confirmBreakpoint(m_trajectory);
      if (msgLvl(MSG::VERBOSE)) {
        m_log << MSG::VERBOSE << "DNA separator spoke: " << (int)dnaSeparation;
        if (dnaSeparation==Trk::DnaBremPointNotUseful) m_log << " (false positive DNA)";
        if (dnaSeparation==Trk::DnaBremPointUseful) m_log << " (DNA confirmed)\n";
      }
      kalMec.updateBreakpoint(dnaSeparation);
    }

    // --- undoing-of-DNA and outlier logic are mutually exclusive. Therefore this if-else construct:
    if (kalMec.breakpointType() == Trk::DnaBremPointNotUseful) {
      kalMec.updateBreakpoint(Trk::NoBreakpoint);
      --nOutlierIterations;
      m_utility->defineMeasurementsExceptThis(m_trajectory,0); // reset FKF-misses
      iFilterBeginState = 1;
      this->prepareNextIteration(1,newFitQuality,iFilterBeginState, *startPar);

      ATH_MSG_VERBOSE (  "\n********** call forward kalman filter, reverting DNA (still #1) **********\n");
      // do not recalibrate again - DNA usually guarantees ROTs are made close to real track
      m_fitStatus = m_forwardFitter->fit(m_trajectory, *startPar, runOutlier, kalMec,
                                         false, iFilterBeginState);

      // success-test and backward filter are then called by usual iterateKalmanFilter
      // loop for nOutlierIterations==1.

    } else {

      // --- now go through different exit conditions: NO quality check, good or bad fit quality.

      // --- #1 directly exit iterations because they are switched OFF (runoutlier false)
      if (!runOutlier) {
        ATH_MSG_VERBOSE ("runOutlier is off => give back track without iteration or fit quality improvement.");
        fitter_is_ready = true;
        return true;
      }

      // --- run outlier logic. Check if we end up at ndof<=0 or if outliers are found.
      int nPreviousOutliers = m_utility->numberOfOutliers(m_trajectory);
      if (newFitQuality == nullptr || newFitQuality->numberDoF() <= 0) {
        fitter_is_ready = false;
      } else {
        fitter_is_ready = 
          !m_outlierLogic->flagNewOutliers( m_trajectory,*newFitQuality,
                                            iFilterBeginState,nOutlierIterations );
        if (!m_outlierRecovery.empty()) {
          bool nothingToRecover = 
            !m_outlierRecovery->flagNewOutliers( m_trajectory,*newFitQuality,
                                                 iFilterBeginState,nOutlierIterations );
          ATH_MSG_DEBUG("have outlier-recovery tool, this "<<
                        (nothingToRecover?"did not find any":"suggested to re-integrate some")<<" outliers");
          fitter_is_ready = fitter_is_ready && nothingToRecover;
        }
      }

      // --- #2 again exit if now the fit quality is deemed good.
      if (fitter_is_ready) return true;

      if (msgLvl(MSG::DEBUG)) m_utility->dumpTrajectory(m_trajectory, name());
      bool fit_has_failed =
        ( newFitQuality==nullptr 
	  || newFitQuality->numberDoF() <=0 
          || ( (m_utility->numberOfOutliers(m_trajectory)-nPreviousOutliers)> 0.4*m_trajectory.size())
          || nOutlierIterations >= m_option_max_N_iterations
          || m_utility->rankedNumberOfMeasurements(m_trajectory) < 5);
      if (fit_has_failed)
        m_fitStatus = (newFitQuality && newFitQuality->numberDoF() <=0) ?
          Trk::FitterStatusCode::FewFittableMeasurements                :
          Trk::FitterStatusCode::OutlierLogicFailure                    ;

      // --- #3 if the fit isn't going well but still not failed, consider switching to DAF
      if ( !m_internalDAF.empty() ) {

        // count drift circles and see if something is wrong there
        int nMeasPrecise = 0;  int nMeasTube = 0;  int nOutlPrecise = 0; int nOutlTube = 0;
	int posFirstTrt=0;
        Trajectory::iterator it = m_trajectory.begin();
        for(; it!=m_trajectory.end(); it++) {
          if (it->isDriftCircle()) {
            if (posFirstTrt == 0) posFirstTrt = it->positionOnTrajectory();
            if (it->measurement()->localCovariance()(Trk::locR,Trk::locR) > 1.0) {
              if (it->isOutlier()) ++nOutlTube; else ++nMeasTube;
            } else {
              if (it->isOutlier()) ++nOutlPrecise; else ++nMeasPrecise;
            }
          }
        }
        if ( (nMeasPrecise+nMeasTube+nOutlPrecise+nOutlTube) > 12
	     && ( ( (nOutlPrecise+nOutlTube) > 7 && nOutlierIterations >= m_option_max_N_iterations-1)
		  || (m_utility->numberOfNewOutliers(m_trajectory,posFirstTrt)>3
		      && nOutlierIterations >= m_option_max_N_iterations-1) 
		  || (nOutlierIterations >= m_option_max_N_iterations
		      && m_trajectory.size()>20) )) {
          m_fitStatus = Trk::FitterStatusCode::NoConvergence;
          fit_has_failed = true;
          ATH_MSG_DEBUG ("checking if DAF can be useful - yes: found case of messed-up TRT driftcircle fit.");
	} else {
          ATH_MSG_VERBOSE ("checking if DAF can be useful - no: TRT driftcircle part not a particular problem.");
        }


        // and prevent tracks from getting close to their ndof limit
        int currentNdof = m_utility->rankedNumberOfMeasurements(m_trajectory)-5;

        if ( (m_maximalNdof >= 2 * currentNdof) && (m_maximalNdof < 7) ) {
          m_fitStatus = Trk::FitterStatusCode::NoConvergence;
          fit_has_failed = true;
          ATH_MSG_DEBUG ("checking if DAF can be useful - yes: lost too many measurements! ndof max -> now is: "
                << m_maximalNdof << " -> " << currentNdof );
        } else {
          ATH_MSG_VERBOSE ("checking if DAF can be useful - no: still enough measurements.");
        }
      }

      // --- #4 exit if the fit quality is bad and no more room to improve it.
      if (fit_has_failed) {
        ATH_MSG_DEBUG ("outlier logic has not been successful, stop iterations.\n");
        return false;
      }
    } // end dna-separation skips outliers
    
  } // end loop over outliers
  return true;
}

// Main internal iteration logic for all interfaces:
// run fwd-filter, smoother and then evaluate results with DNA, outliers, local pattern
// ---------------------------------------------------------------------------------
bool Trk::KalmanFitter::invokeAnnealingFilter(const Trk::TrackParameters*&  startPar,
                                              Trk::FitQuality*&       newFitQuality,
                                              const RunOutlierRemoval       runOutlier,
                                              const Trk::KalmanMatEffectsController& kalMec,
                                              const double& this_eta) const
{
  if ( runOutlier && !m_internalDAF.empty() ) {

    // --- There is an alternative: perform enhanced local pattern recognition (DAF)
    
    bool instable_filter = (m_fitStatus == Trk::FitterStatusCode::UpdateFailure)
      ||                   (m_fitStatus == Trk::FitterStatusCode::FitQualityFailure)
      ||                   (m_fitStatus == Trk::FitterStatusCode::ExtrapolationFailure)
      ||                   (m_fitStatus == Trk::FitterStatusCode::ExtrapolationFailureDueToSmallMomentum)
      ||                   (m_fitStatus == Trk::FitterStatusCode::CombineStatesFailure)
      ||                   (m_fitStatus == Trk::FitterStatusCode::NoConvergence);

    bool found_problematic_seed= false;

    if (!instable_filter) {

      // and look for problematic seeds (replaces old seed-recovery by T.Belkind)
      double chi2_AFB = 0.0;
      if (newFitQuality != nullptr) {
        Trk::FitQuality ffq = m_utility->forwardFilterQuality(m_trajectory);
        chi2_AFB = (ffq.chiSquared() - newFitQuality->chiSquared()) / 
          (ffq.chiSquared() + newFitQuality->chiSquared());
      }
      unsigned int count_trt = m_utility->numberOfSpecificStates(m_trajectory,Trk::TrackState::TRT,Trk::TrackState::AnyState);
      found_problematic_seed = m_trajectory.size() < 25 && // don't continue iterating on long tracks
        count_trt<0.9*m_trajectory.size() && 
        (m_utility->numberOfNewOutliers(m_trajectory,6) > 1 ||
         (chi2_AFB > 0.1 && m_trajectory.size()<18) /*FKF returned w/ more problems*/ );
    }

    if (instable_filter || found_problematic_seed) {
      
      // --- prepare trajectory for running DAF as outlier and driftcircle-R-L logic
      m_utility->defineMeasurementsExceptThis(m_trajectory,0);
      m_utility->clearFitResultsAfterOutlier(m_trajectory,newFitQuality,0);
      ATH_MSG_DEBUG ( "Simple outlier detection fails, now using deterministic annealing.");

      FitterStatusCode dafStatus = Trk::FitterStatusCode::OutlierLogicFailure;
      if (m_forwardFitter->enterSeedIntoTrajectory(m_trajectory,*startPar,m_cov0,kalMec,false)
          == Trk::FitterStatusCode::Success) 
        dafStatus = m_internalDAF->filterTrajectory(m_trajectory, kalMec.particleType());
        // remark: there is also an interface for running the DAF on a *part of* the
        // trajectory, intended for TRT, ID, MS subparts of combined tracks

      if (dafStatus == Trk::FitterStatusCode::Success) { 

        ATH_MSG_VERBOSE("internal DAF succeeded, now going back to std fit by running one backward smoother.");
        kalMec.updateBreakpoint(Trk::BreakpointNotSpecified);
        if (m_forwardFitter->needsReferenceTrajectory())
          dafStatus = m_smoother->fitWithReference(m_trajectory, newFitQuality, kalMec);
        else
          dafStatus = m_smoother->fit(m_trajectory, newFitQuality, kalMec);
          ATH_MSG_INFO( "Internal DAF returned with chi2 chain:");
          for (Trk::Trajectory::const_iterator it=m_trajectory.begin();it!=m_trajectory.end(); it++) {
            if (!it->isOutlier()) {
              if (it->fitQuality()) ATH_MSG_INFO( it->fitQuality()->chiSquared() << " % ");
              else                  ATH_MSG_INFO( "Problem - no FitQ % ");
            }
          }
      } 
          
      bool successfulRecovery  = newFitQuality!= nullptr
        && dafStatus==Trk::FitterStatusCode::Success
        && m_utility->rankedNumberOfMeasurements(m_trajectory) >= 5
        && !m_outlierLogic->reject(*newFitQuality);

      if (successfulRecovery) {
        if (msgLvl(MSG::INFO)) monitorTrackFits( InternalDafUsed, this_eta );
        m_fitStatus = Trk::FitterStatusCode::Success;
        return true;
      } 
        ATH_MSG_DEBUG ("intDAF called, but to no good!");
        if (msgLvl(MSG::INFO)) monitorTrackFits( DafNoImprovement, this_eta );
      
    }
  }

  // --- code arrives at this point only if track to be rejected:
  // std iterations did not converge, and either DAF not considerd or DAF failed too.

  ATH_MSG_DEBUG ("kalman fit has not been successful, reject track.\n");
  if (msgLvl(MSG::INFO)) {
    if (m_fitStatus == Trk::FitterStatusCode::OutlierLogicFailure) monitorTrackFits( IterationsNoConvergence, this_eta );
    if (m_fitStatus == Trk::FitterStatusCode::FewFittableMeasurements) monitorTrackFits( MinimalTrackFailure, this_eta );
    if ( (m_fitStatus == Trk::FitterStatusCode::UpdateFailure) ||
         (m_fitStatus == Trk::FitterStatusCode::FitQualityFailure) ||
         (m_fitStatus == Trk::FitterStatusCode::CombineStatesFailure) )
      monitorTrackFits( Trk::KalmanFitter::UpdateFailure, this_eta );
    if ( (m_fitStatus == Trk::FitterStatusCode::ExtrapolationFailure) ||
         (m_fitStatus == Trk::FitterStatusCode::ExtrapolationFailureDueToSmallMomentum) )
      monitorTrackFits( Trk::KalmanFitter::ExtrapolationFailure, this_eta );
    if ( (m_fitStatus == Trk::FitterStatusCode::BadInput) )
      monitorTrackFits( Trk::KalmanFitter::BadInput, this_eta );
  }
  return false;
}

bool Trk::KalmanFitter::prepareNextIteration(const unsigned int& upcomingIteration,
                                             FitQuality*& FQ,
                                             int& iFilterBeginState,
                                             const Trk::TrackParameters& backupParams) const
{
  const Trk::TrackParameters* newSeedPars = nullptr;
  ATH_MSG_VERBOSE ("In ::prepareNextIteration with filterBeginState = "<<iFilterBeginState);

  // get chi2 asymmetry
  double Chi2FilterAfb = 0.0;
  if (FQ  != nullptr) {
    for (Trk::Trajectory::const_iterator it=m_trajectory.begin(); it!=m_trajectory.end(); it++)
      if ( !it->isOutlier() || 
           (it->trackStateType() != Trk::TrackState::ExternalOutlier
            && it->iterationShowingThisOutlier() == int(upcomingIteration-1)) )
        Chi2FilterAfb += it->forwardStateChiSquared();
    Chi2FilterAfb = (Chi2FilterAfb - FQ->chiSquared()) / (Chi2FilterAfb + FQ->chiSquared());
  }

  // idea: if the backward filter was good, use its parameters to restart outliers at state 1
  //       or equally use those pars for undoing DNA. Prepare new seed trackparameters:
  Trk::Trajectory::iterator ffs = m_utility->firstFittableState(m_trajectory);
  if (Chi2FilterAfb > 0.2  || // indicates that backward filter is reliable
      !m_outlierLogic->reject(*FQ)) {
    const Trk::TrackParameters* resultFromPreviousIter
      = ffs->smoothedTrackParameters();
    if (resultFromPreviousIter && resultFromPreviousIter->covariance()) {
      const AmgSymMatrix(5)* cov = resultFromPreviousIter->covariance();
      std::vector<bool> isConstraint(5);
      AmgSymMatrix(5)* covN = new AmgSymMatrix(5)(); // a 5x5 0-matrix
      covN->setZero();
      for (int i=0; i<5; ++i) {
        double scale = i==4 ? 200. : 100.;
        (*covN)(i,i) = (*cov)(i,i) < m_cov0[i] ? (*cov)(i,i)*scale : m_cov0[i];
      }
      newSeedPars = CREATE_PARAMETERS(*resultFromPreviousIter,
                                      resultFromPreviousIter->parameters(),covN); 
    }
  }

  // if that worked: clean up, set filter back to first state and plug in new seed parameters 
  if (newSeedPars) {
    iFilterBeginState = 1;
    m_utility->clearFitResultsAfterOutlier(m_trajectory,FQ,iFilterBeginState);
    if (m_forwardFitter->needsReferenceTrajectory()) {
      AmgVector(5)* x = new AmgVector(5)(newSeedPars->parameters()-ffs->referenceParameters()->parameters());
      ffs->checkinParametersDifference(x);
      ffs->checkinParametersCovariance(new AmgSymMatrix(5)(*newSeedPars->covariance()));
      delete newSeedPars; // FIXME can be made without this new/delete.
    } else ffs->checkinForwardPar(newSeedPars);
    ATH_MSG_VERBOSE ("made new seed parameters");
    // FIXME consider remaking the reference here

    // if not: restart iteration after first outlier, unless there are outliers at the start
  } else if (iFilterBeginState > 1) {
    ATH_MSG_VERBOSE ("Clearing fit results after state T"<<iFilterBeginState);
    m_utility->clearFitResultsAfterOutlier(m_trajectory,FQ,iFilterBeginState); // FIXME develop remake of refs
    if (ffs->positionOnTrajectory() > iFilterBeginState) {
      ATH_MSG_VERBOSE ("New outlier(s) have shifted beginning of trajectory, "<<
                       "therefore need to recreate seed from input ref params.");
      m_forwardFitter->enterSeedIntoTrajectory(m_trajectory,backupParams,m_cov0,KalmanMatEffectsController(),false /*FIXME tune that */);
    }
  } else {
    ATH_MSG_VERBOSE ("reset seed to parameters given by input plus large covariance.");
    m_utility->clearFitResultsAfterOutlier(m_trajectory,FQ,0);
    m_forwardFitter->enterSeedIntoTrajectory(m_trajectory,backupParams,m_cov0,KalmanMatEffectsController(),false);
  }
  return true;
}



// PRIVATE method: create a track object
// --------------------------------------
Trk::Track*
Trk::KalmanFitter::makeTrack(
  const EventContext& ctx,
  const Trk::FitQuality* FQ,
  const Trk::TrackParameters& refPar,
  const Trk::KalmanMatEffectsController* matEffController,
  const double& this_eta,
  const Trk::TrackInfo* existingInfo) const
{
  ATH_MSG_VERBOSE ("--> enter KalmanFitter::makeTrack()");
  if (msgLvl(MSG::VERBOSE)) m_utility->dumpTrajectory(m_trajectory, name());

  // output trajectory - only if # degrees of freedom reasonable
  if (FQ->numberDoF() <= 0) {
    ATH_MSG_DEBUG ("********** warning, ndof<=0 i.e. not enough measurements left "
		   << "to form track! **********\n");
    if (msgLvl(MSG::DEBUG)) monitorTrackFits( MinimalTrackFailure, this_eta );
    m_fitStatus = Trk::FitterStatusCode::FewFittableMeasurements;
    return nullptr;
  } 
    SmoothedTrajectory* finalTrajectory = new SmoothedTrajectory();

    // add new TSoS with parameters on reference surface (e.g. physics Perigee)
    if (m_option_PerigeeAtOrigin) {
      const Trk::PerigeeSurface   perSurf;
      const TrackStateOnSurface* perState =
        internallyMakePerigee(ctx, perSurf, matEffController->particleType());
      if (perState) finalTrajectory->push_back( perState );
      else {
        ATH_MSG_DEBUG ("********** perigee making failed, drop track");
        if (msgLvl(MSG::DEBUG)) monitorTrackFits( Trk::KalmanFitter::PerigeeMakingFailure, this_eta );
        m_fitStatus = Trk::FitterStatusCode::PerigeeMakingFailure;
        delete finalTrajectory;  return nullptr;
      }
    } else {
      const TrackStateOnSurface* refState = makeReferenceState(
        ctx, refPar.associatedSurface(), matEffController->particleType());
      if (refState) {
        finalTrajectory->push_back( refState );
        ATH_MSG_VERBOSE ("added track state at reference surface.");
      } else ATH_MSG_VERBOSE ("no track state at reference surface available, return bare track.");
    }


    bool dnaFitPresent = m_utility->trajectoryHasMefot(m_trajectory);
    Trajectory::iterator it = m_trajectory.begin();
    int i=0;
    for(; it!=m_trajectory.end(); it++, i++) {
      const TrackStateOnSurface* trkState = it->createState();
      if (trkState) finalTrajectory->push_back( trkState );
      else ATH_MSG_WARNING ("fitter inconsistency - no track state #"<<i<<" available!");
    }
    ATH_MSG_VERBOSE ("extracted " << i << " states from KF internal trajectory.");

    // now deal with track info, set new properties etc.
    TrackInfo info(TrackInfo::KalmanFitter, matEffController->originalParticleHypo());
    if (existingInfo) info.addPatternReco(*existingInfo);
    if ( !m_dynamicNoiseAdjustor.empty() //matEffController->doDNA()
         && ( (!m_doDNAForElectronsOnly && matEffController->doDNA()) // avoid new style where BremFit steers subsequent brem fits
              || matEffController->originalParticleHypo()==Trk::electron ) ) {
      info.setTrackProperties (TrackInfo::BremFit);
      if (msgLvl(MSG::INFO)) monitorTrackFits( DNABremFit, this_eta );
    }
    if (dnaFitPresent) {
      info.setTrackProperties (TrackInfo::BremFitSuccessful);
      if (msgLvl(MSG::INFO)) monitorTrackFits( DNAFoundBrem, this_eta );
    }
    Trk::Track* fittedTrack = new Track(info, finalTrajectory,FQ );
    if (!fittedTrack) {
      ATH_MSG_WARNING ("Trk::Track constructor failed!");
      m_fitStatus = Trk::FitterStatusCode::PerigeeMakingFailure;
      return fittedTrack;
    }
    const Trk::Perigee* testParam = fittedTrack->perigeeParameters();
    if (testParam && testParam->covariance()) {
      const AmgSymMatrix(5)& cov = *testParam->covariance();
      if (cov(0,4) == 0.0 && cov(1,4)==0.0 && cov(2,4)==0.0 && cov(3,4)==0) {
	ATH_MSG_VERBOSE ("Detected straight-line track.");
	fittedTrack->info().setTrackProperties(TrackInfo::StraightTrack);
	if (msgLvl(MSG::INFO)) monitorTrackFits( Trk::KalmanFitter::StraightTrackModel, this_eta );
      }
    }
    // we made it !
    ATH_MSG_DEBUG ("\n********** done, track made with Chi2 = " << FQ->chiSquared()
		   << " / " << FQ->numberDoF() << " **********\n");
    if (msgLvl(MSG::DEBUG)) {
      monitorTrackFits( Trk::KalmanFitter::Success, this_eta );
      updateChi2Asymmetry( m_fitStatistics[Trk::KalmanFitter::Success], *FQ, this_eta);
    }
    m_fitStatus = Trk::FitterStatusCode::Success;
    return fittedTrack;
  
}

const Trk::TrackStateOnSurface*
Trk::KalmanFitter::internallyMakePerigee(
  const EventContext& ctx,
  const Trk::PerigeeSurface& perSurf,
  const Trk::ParticleHypothesis matEffects) const
{
  Trajectory::const_iterator it = m_trajectory.begin();
  const Trk::TrackParameters* nearestParam   = nullptr;
  
  // --- simple case to get "first" track parameters: ID track
  if (!it->isOutlier() && (it->smoothedTrackParameters()) &&
      m_sortingRefPoint.mag() < 1.0E-100 &&
      it->smoothedTrackParameters()->position().perp() < 350. ) {
    nearestParam = it->smoothedTrackParameters();
  } else {
    std::vector<const Trk::TrackParameters*> parameterTrajectory;
    for ( ; it!= m_trajectory.end(); ++it) {
      if (!it->isOutlier() && (it->smoothedTrackParameters()))
        parameterTrajectory.push_back(it->smoothedTrackParameters());
    }
    if (parameterTrajectory.empty()) {
      ATH_MSG_WARNING ("Perigee-making failed: no useful parameters on track!");
      return nullptr;
    }
    nearestParam = *(std::min_element(parameterTrajectory.begin(),
				      parameterTrajectory.end(),
				      *m_tparScaleSetter));
  }
  // extrapolate to perigee
  const Trk::TrackParameters* per = m_extrapolator->extrapolate(
    ctx,
    *nearestParam,
    perSurf,
    (m_sortingRefPoint.mag() > 1.0E-10 ? Trk::anyDirection
                                       : Trk::oppositeMomentum),
    false,
    matEffects);
  if (!per) {
    ATH_MSG_DEBUG ("Perigee-making failed: extrapolation did not succeed.");
    return nullptr;
  } ATH_MSG_VERBOSE ("Perigee parameters have been made.");

  std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
  typePattern.set(TrackStateOnSurface::Perigee);
  return new TrackStateOnSurface(nullptr , per, nullptr,  nullptr, typePattern );
}

const Trk::TrackStateOnSurface*
Trk::KalmanFitter::makeReferenceState(
  const EventContext& ctx,
  const Trk::Surface& refSurface,
  const Trk::ParticleHypothesis matEffects) const
{
  Trajectory::const_iterator it = m_trajectory.begin();
  const Trk::TrackParameters* nearestParam   = nullptr;
  
  // --- simple case: ref surface was entered at first measurement
  if (!it->isOutlier() && (&(it->measurement()->associatedSurface()) == &refSurface) )
    return nullptr;
  Trk::TrkParametersComparisonFunction nearestSurfaceDefinition(refSurface.center());
  std::vector<const Trk::TrackParameters*> parameterTrajectory;
  for ( ; it!= m_trajectory.end(); ++it)
    if (!it->isOutlier() && (it->smoothedTrackParameters()))
      parameterTrajectory.push_back(it->smoothedTrackParameters());
  if (parameterTrajectory.empty()) {
    ATH_MSG_WARNING ("Reference state making failed: no useful parameters on track!");
      return nullptr;
  }
  nearestParam = *(std::min_element(parameterTrajectory.begin(),
                                    parameterTrajectory.end(),
                                    nearestSurfaceDefinition));
  const Trk::TrackParameters* fittedRefParams = m_extrapolator->extrapolate(
    ctx,
    *nearestParam,
    refSurface,
    (m_sortingRefPoint.mag() > 1.0E-10 ? // is it 0,0,0 ?
       Trk::anyDirection
                                       : Trk::oppositeMomentum),
    false,
    matEffects);
  if (!fittedRefParams) {
    ATH_MSG_DEBUG (" No ref-params made: extrapolation failed.");
    return nullptr;
  } ATH_MSG_VERBOSE ("Reference parameters have been made.");

  std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
  typePattern.set(TrackStateOnSurface::Perigee);
  return new TrackStateOnSurface(nullptr, fittedRefParams, nullptr, nullptr, typePattern );
}

void Trk::KalmanFitter::monitorTrackFits(FitStatisticsCode code, const double& eta) const {

  // do not count if fit was repeated to write validation data of failed fit:
  if ( m_option_callValidationToolForFailedFitsOnly && m_callValidationTool ) return;
  // count:
  ((m_fitStatistics[code])[Trk::KalmanFitter::iAll])++;
  if (std::abs(eta) < 0.80 ) ((m_fitStatistics[code])[Trk::KalmanFitter::iBarrel])++;
  else if (std::abs(eta) < 1.60) ((m_fitStatistics[code])[Trk::KalmanFitter::iTransi])++;
  else if (std::abs(eta) < 2.50) ((m_fitStatistics[code])[Trk::KalmanFitter::iEndcap])++;
}

void Trk::KalmanFitter::updateChi2Asymmetry(std::vector<int>& Nsuccess,
                                            const Trk::FitQuality& bwFQ,
                                            const double& eta) const {
  if ( m_option_callValidationToolForFailedFitsOnly && m_callValidationTool ) return;
  Trk::FitQuality fwFQ = m_utility->forwardFilterQuality(m_trajectory);
  double chi2_AFB = (fwFQ.chiSquared() - bwFQ.chiSquared())/ (fwFQ.chiSquared() + bwFQ.chiSquared());
  m_chiSquaredAfb[Trk::KalmanFitter::iAll]
    += (chi2_AFB        - m_chiSquaredAfb[Trk::KalmanFitter::iAll])/double(Nsuccess[Trk::KalmanFitter::iAll]);
  m_chiSquaredAfbNontriviality[Trk::KalmanFitter::iAll]
    += (std::abs(chi2_AFB)        - m_chiSquaredAfbNontriviality[Trk::KalmanFitter::iAll])/double(Nsuccess[Trk::KalmanFitter::iAll]);
  if (std::abs(eta) < 0.80 ) {
    m_chiSquaredAfb[Trk::KalmanFitter::iBarrel]
      += (chi2_AFB        - m_chiSquaredAfb[Trk::KalmanFitter::iBarrel])/double(Nsuccess[Trk::KalmanFitter::iBarrel]);
    m_chiSquaredAfbNontriviality[Trk::KalmanFitter::iBarrel]
      += (std::abs(chi2_AFB)        - m_chiSquaredAfbNontriviality[Trk::KalmanFitter::iBarrel])/double(Nsuccess[Trk::KalmanFitter::iBarrel]);
  }
  else if (std::abs(eta) < 1.60) {
    m_chiSquaredAfb[Trk::KalmanFitter::iTransi]
      += (chi2_AFB        - m_chiSquaredAfb[Trk::KalmanFitter::iTransi])/double(Nsuccess[Trk::KalmanFitter::iTransi]);
    m_chiSquaredAfbNontriviality[Trk::KalmanFitter::iTransi]
      += (std::abs(chi2_AFB)        - m_chiSquaredAfbNontriviality[Trk::KalmanFitter::iTransi])/double(Nsuccess[Trk::KalmanFitter::iTransi]);
  }
  else if (std::abs(eta) < 2.50) {
    m_chiSquaredAfb[Trk::KalmanFitter::iEndcap]
      += (chi2_AFB        - m_chiSquaredAfb[Trk::KalmanFitter::iEndcap])/double(Nsuccess[Trk::KalmanFitter::iEndcap]);
    m_chiSquaredAfbNontriviality[Trk::KalmanFitter::iEndcap]
      += (std::abs(chi2_AFB)        - m_chiSquaredAfbNontriviality[Trk::KalmanFitter::iEndcap])/double(Nsuccess[Trk::KalmanFitter::iEndcap]);
  }
}


bool Trk::KalmanFitter::check_operability(int iSet, const RunOutlierRemoval& runOutlier,
                                          const Trk::ParticleHypothesis& matEff,
                                          bool empty) const
{
  // verbose output on the fitter data flow
  std::string mySet = " ";
  iSet==1 ? mySet = "PrepRawData" : mySet = "Measurement";
  if (msgLvl(MSG::VERBOSE)) {
    ATH_MSG_VERBOSE( "--> entering KalmanFitter::fit("<< mySet << "Set,TrackParameters,,)");
    if (runOutlier) ATH_MSG_DEBUG( "-> Outlier removal switched on" );
    if (matEff!=Trk::nonInteracting)
      ATH_MSG_DEBUG( "-> material effects active and switched to "<< matEff );
  }
  
  // check RIO_OnTrack creator necessity
  if (m_ROTcreator.empty() && iSet==1) {
    ATH_MSG_ERROR( "RIO_OnTrackCreator tool not configured, but your fit on PrepRawData will need it!" );
    return false;
  }
  
  // prevent the fitter from running into empty HitSet
  if (empty) {
    ATH_MSG_WARNING("attempt to fit empty vector of "<< mySet << " objects" ) ;
    return false;
  }
  
  // internal consistency check
  if (!m_trajectory.empty()) {
    ATH_MSG_ERROR("coding problem? fitter starts with uncleared internal vectors!" ) ;
    return false;
  }
  
  return true;
}

void
Trk::KalmanFitter::callValidation(const EventContext& ctx,
                                  int iterationIndex,
                                  const Trk::ParticleHypothesis matEffects,
                                  FitterStatusCode fitStatCode) const
{
    ATH_MSG_DEBUG( "call validation for track iteration " << iterationIndex << "with status " << fitStatCode.getCode() );
    // extrapolate to perigee at origin for validation data
    const Trk::PerigeeSurface   perSurf; // default perigee at origin
    const Trk::TrackParameters* nearestParam   = nullptr;
    Trajectory::const_iterator it = m_trajectory.begin();
    for ( ; it != m_trajectory.end(); it++ ) { // FIXME this can be coded more elegantly
        if (!it->isOutlier() && (it->smoothedTrackParameters())) {
            nearestParam = it->smoothedTrackParameters();
            break;
        }
    }
    if (!nearestParam) {
        for ( it = m_trajectory.begin(); it != m_trajectory.end(); it++ ) {
            if (!it->isOutlier() && (it->forwardTrackParameters())) {
                nearestParam = it->forwardTrackParameters();
                break;
            }
        }
    }
    const Trk::Perigee* per = nullptr;
    const Trk::TrackParameters* perPar = nullptr;
    if (nearestParam) {
        // extrapolate to perigee
        perPar = m_extrapolator->extrapolate(ctx,
                                             *nearestParam,
                                             perSurf,
                                             m_sortingRefPoint.mag() > 1.0E-10
                                               ? // is it 0,0,0 ?
                                               Trk::anyDirection
                                               : Trk::oppositeMomentum,
                                             false,
                                             matEffects);
        per = dynamic_cast<const Trk::Perigee*>(perPar);
    } else {
        ATH_MSG_WARNING("Perigee-making for validation failed: no useful parameters on track!" );
    }
    // write validation data for iteration with index
    StatusCode sc = m_FitterValidationTool->writeProtoTrajectoryData(m_trajectory, iterationIndex, per, fitStatCode.getCode());
    // FIXME. just ignore as this is only for validation.
    sc.ignore(); 
    delete perPar;
}
