/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TileEvent/TileRawChannel.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

// Gaudi includes
#include "GaudiKernel/Property.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

// Atlas includes
#include "AthAllocators/DataPool.h"
#include "AthenaKernel/errorcheck.h"
#include "xAODEventInfo/EventInfo.h"

// Tile includes
#include "TileRecUtils/TileRawChannelBuilderWienerFilter.h"
#include "TileConditions/TilePulseShapes.h"
#include "CLHEP/Matrix/Matrix.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileDigits.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileInfo.h"

//using namespace std;
#include <algorithm>

//interface stuff
static const InterfaceID IID_ITileRawChannelBuilderWienerFilter("TileRawChannelBuilderWienerFilter", 1, 0);


const InterfaceID& TileRawChannelBuilderWienerFilter::interfaceID() {
  return IID_ITileRawChannelBuilderWienerFilter;
}


#define TILE_WienerFilterBUILDERVERBOSE false

// initialize Wiener coefficients ( ZeroBias <mu>=40 )
const float TileRawChannelBuilderWienerFilter::m_gfcWiener[8] = {
  -0.194081,
  0.467562,
  -1.25846,
  2.18528,
  -1.2648,
  0.63341,
  -0.276375,
  -14.2093
};
const float TileRawChannelBuilderWienerFilter::m_ofcWiener[7][8] = {
  {0.04382, -0.999, 0.5212, 1.179, -0.8052, 0.4123, -0.1756, -5.393},
  {0.1806, 0.02539, -1.253, 2.187, -1.254, 0.6214, -0.2732, -14.53},
  {-0.3267, 0.6014, -1.346, 2.224, -1.258, 0.6043, -0.276, -14.41},
  {-0.1914, 0.4693, -1.246, 2.169, -1.243, 0.6191, -0.2633, -15.15},
  {-0.1831, 0.4734, -1.253, 2.241, -1.424, 0.9173, -0.5364, -15.43},
  {-0.2245, 0.5627, -1.398, 2.433, -1.722, 1.204, -0.5617, -15.26},
  {-0.2367, 0.617, -1.502, 2.539, -1.729, 0.7092, -0.1435, -14.15}
};

TileRawChannelBuilderWienerFilter::TileRawChannelBuilderWienerFilter(const std::string& type,
    const std::string& name, const IInterface *parent)
  : TileRawChannelBuilder(type, name, parent)
  , m_tileToolTiming("TileCondToolTiming")
  , m_tileCondToolOfc("TileCondToolOfc")
  , m_tileToolNoiseSample("TileCondToolNoiseSample")
  , m_bunchCrossingTool("BunchCrossingTool")
  , m_nSignal(0)
  , m_nNegative(0)
  , m_nCenter(0)
  , m_nConst(0)
  , m_nSamples(0)
  , m_t0SamplePosition(0)
  , m_maxTime(0.0)
  , m_minTime(0.0)
{
  //declare interfaces
  declareInterface< TileRawChannelBuilder >( this );
  declareInterface< TileRawChannelBuilderWienerFilter >(this);

  m_TileRawChannelContainerID = "TileRawChannelWiener";
  
  //declare properties
  declareProperty("TileCondToolTiming", m_tileToolTiming);
  declareProperty("TileCondToolOfc",    m_tileCondToolOfc  ,"TileCondToolOfc");
  declareProperty("TileCondToolNoiseSample", m_tileToolNoiseSample,"TileCondToolNoiseSample");
  declareProperty("BunchCrossingTool", m_bunchCrossingTool);
  declareProperty("MaxIterations",m_maxIterations = 5);
  declareProperty("PedestalMode",m_pedestalMode = 17);
  declareProperty("TimeForConvergence",m_timeForConvergence = 0.5);
  declareProperty("ConfTB",m_confTB = false);
  declareProperty("Minus1Iteration",m_minus1Iter = false);
  declareProperty("AmplitudeCorrection",m_correctAmplitude = false);
  declareProperty("TimeCorrection", m_correctTimeNI = false);
  declareProperty("BestPhase",m_bestPhase = false);
  declareProperty("EmulateDSP",m_emulateDsp = false);
  declareProperty("MC",m_isMC = false);
}


TileRawChannelBuilderWienerFilter::~TileRawChannelBuilderWienerFilter() {
}


StatusCode TileRawChannelBuilderWienerFilter::initialize() {

  ATH_MSG_INFO( "initialize()" );

  m_rChType = TileFragHash::WienerFilterOffline; // type for offline Wiener Filter

  // init in superclass
  CHECK( TileRawChannelBuilder::initialize() );

  if (m_maxIterations != 1) m_correctTimeNI = false;

  // bits 12-15 - various options
  // if (m_correctTimeNI)  m_bsflags |= 0x1000;
  if (m_correctAmplitude) m_bsflags |= 0x2000;
  if (m_maxIterations > 1) m_bsflags |= 0x4000;
  if (m_bestPhase) m_bsflags |= 0x8000;

  ATH_MSG_DEBUG( " MaxIterations=" << m_maxIterations
              << " PedestalMode=" << m_pedestalMode
              << " TimeForConvergence=" << m_timeForConvergence
              << " ConfTB=" << m_confTB
              << " Minus1Iteration=" << m_minus1Iter
              << " AmplitudeCorrection=" << m_correctAmplitude
              << " TimeCorrection=" << m_correctTimeNI
              << " Best Phase " << m_bestPhase );

  m_nSamples = m_tileInfo->NdigitSamples();
  m_t0SamplePosition = m_tileInfo->ItrigSample();
  m_maxTime = 25 * (m_nSamples - m_t0SamplePosition - 1);
  m_minTime = -25 * m_t0SamplePosition;
  ATH_MSG_DEBUG(" NSamples=" << m_nSamples
             << " T0Sample=" << m_t0SamplePosition
             << " minTime=" << m_minTime
             << " maxTime=" << m_maxTime );

  if (m_pedestalMode % 10 > 2 && m_nSamples != m_pedestalMode % 10) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Changing PedestalMode from " << m_pedestalMode;
    m_pedestalMode = (m_pedestalMode / 10) * 10 + m_nSamples;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " to " << m_pedestalMode << endmsg;
  }


  if (m_nSamples != 7 && (m_pedestalMode == 71 || m_pedestalMode == 7621)) {
    ATH_MSG_ERROR( "Incompatable pedestal mode [" << m_pedestalMode
                   << "] and number of samples [" << m_nSamples << "]" );
    return StatusCode::FAILURE;
  }

  m_nSignal = 0;
  m_nNegative = 0;
  m_nCenter = 0;
  m_nConst = 0;

  const IGeoModelSvc *geoModel = 0;
  CHECK( service("GeoModelSvc", geoModel) );
  
  // dummy parameters for the callback:
  int dummyInt = 0;
  std::list<std::string> dummyList;
  
  if (geoModel->geoInitialized()) {
    return geoInit(dummyInt, dummyList);
  } else {
    CHECK( detStore()->regFcn(&IGeoModelSvc::geoInit, geoModel,
        &TileRawChannelBuilderWienerFilter::geoInit, this) );
  }

  return StatusCode::SUCCESS;
}


StatusCode TileRawChannelBuilderWienerFilter::geoInit(IOVSVC_CALLBACK_ARGS) {
  
  //=== get TileCondToolOfc
  CHECK( m_tileCondToolOfc.retrieve() );
  
  //=== get TileCondToolNoiseSample
  CHECK( m_tileToolNoiseSample.retrieve() );

  if (m_bestPhase) {
    //=== get TileToolTiming
    CHECK( m_tileToolTiming.retrieve() );
  }

  if (m_isMC) {
      m_bunchCrossingTool.setTypeAndName("Trig::MCBunchCrossingTool/BunchCrossingTool");
  } else { // is data
      m_bunchCrossingTool.setTypeAndName("Trig::LHCBunchCrossingTool/BunchCrossingTool");
  }
  //=== get TrigBunchCrossingTool
  CHECK( m_bunchCrossingTool.retrieve() );
  
  ATH_MSG_INFO( "initialization completed" );

  return StatusCode::SUCCESS;
}


StatusCode TileRawChannelBuilderWienerFilter::finalize() {

  if (msgLvl(MSG::VERBOSE)) {
    if (m_maxIterations == 1) { // Without iterations
      msg(MSG::VERBOSE) << "Counters: Signal=" << m_nSignal
                        << " Constant=" << m_nConst
                        << " Total=" << m_nSignal + m_nConst << endmsg;
    } else {
      msg(MSG::VERBOSE) << "Counters: Signal=" << m_nSignal
                        << " Negat=" << m_nNegative
                        << " Center=" << m_nCenter
                        << " Constant=" << m_nConst
                        << " Total=" << m_nSignal + m_nNegative + m_nConst + m_nCenter << endmsg;
    }
  }

  ATH_MSG_DEBUG( "Finalizing" );

  return StatusCode::SUCCESS;
}


TileRawChannel * TileRawChannelBuilderWienerFilter::rawChannel(const TileDigits* digits) {

  ++m_chCounter;

  double pedestal = 0.;
  double energy = 0.;
  double time = 0.;
  double chi2 = 0.;
  m_digits = digits->samples();
  const HWIdentifier adcId = digits->adc_HWID();
  int gain = m_tileHWID->adc(adcId);
  
  ATH_MSG_VERBOSE( "Building Raw Channel, with WienerFilter, HWID:" << m_tileHWID->to_string(adcId)
                << " gain=" << gain );

  int ros = m_tileHWID->ros(adcId);
  int drawer = m_tileHWID->drawer(adcId);
  int channel = m_tileHWID->channel(adcId);
  chi2 = filter(ros, drawer, channel, gain, pedestal, energy, time);
  
  if (m_calibrateEnergy) {
    energy = m_tileInfo->CisCalib(adcId, energy);
  }
  
  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Creating WienerFilter RawChannel"
                      << " a=" << energy
                      << " t=" << time
                      << " ped=" << pedestal
                      << " q=" << chi2 << endmsg;

    msg(MSG::VERBOSE) << "digits:";

    for (unsigned int i = 0; i < m_digits.size(); ++i)
      msg(MSG::VERBOSE) << " " << m_digits[i];

    msg(MSG::VERBOSE) << " " << endmsg;
  }
  
  // return new TileRawChannel
  DataPool<TileRawChannel> tileRchPool(m_dataPoollSize);
  TileRawChannel *rawCh = tileRchPool.nextElementPtr();
  rawCh->assign (adcId,
                 energy,
                 time,
                 chi2,
                 pedestal);

  if (m_correctTime
      && (time != 0
          && time < m_maxTime
          && time > m_minTime)) {

    rawCh->insertTime(m_tileInfo->TimeCalib(adcId, time));
    ATH_MSG_VERBOSE( "Correcting time, new time=" << rawCh->time() );

  }

  if (TileID::HIGHGAIN == gain) {
    ++m_nChH;
    m_RChSumH += energy;
  } else {
    ++m_nChL;
    m_RChSumL += energy;
  }
  
  return rawCh;
}


int TileRawChannelBuilderWienerFilter::findMaxDigitPosition() {

  ATH_MSG_VERBOSE( "  findMaxDigitPosition()" );

  int iMaxDigit = 0;
  float maxDigit = 0.;
  bool saturated = false;
  
  for (unsigned int i = 0; i < m_digits.size(); i++) {
    if (m_digits[i] > 1022.99) saturated = true;
    if (maxDigit < m_digits[i]) {
      maxDigit = m_digits[i];
      iMaxDigit = i;
    }
  }
  
  if (msgLvl(MSG::VERBOSE)) {
    for (unsigned int i = 0; i < m_digits.size(); i++) {
      msg(MSG::VERBOSE) << " " << m_digits[i];
    }

    msg(MSG::VERBOSE) << "; Max: digit[" << iMaxDigit << "]=" << maxDigit << endmsg;

    if (saturated)  msg(MSG::VERBOSE) << " Samples saturated" << endmsg;
  }
  
  return iMaxDigit;
}


float TileRawChannelBuilderWienerFilter::getPedestal(int ros, int drawer, int channel, int gain) {
  float pedestal = 0.;
  
  switch (m_pedestalMode) {
    case -1:
      // use pedestal from conditions DB
      pedestal = m_tileToolNoiseSample->getPed(TileCalibUtils::getDrawerIdx(ros, drawer), channel, gain);
      break;
    case 7:
      pedestal = m_digits[6];
      break;
    case 9:
      pedestal = m_digits[8];
      break;
    case 12:
      pedestal = .5 * (m_digits[0] + m_digits[1]);
      break;
    case 17:
      pedestal = .5 * (m_digits[0] + m_digits[6]);
      break;
    case 19:
      pedestal = .5 * (m_digits[0] + m_digits[8]);
      break;
    case 71:
      pedestal = std::min(m_digits[0], m_digits[6]);
      break;
    case 7621:
      pedestal = 0.5 * std::min(m_digits[0] + m_digits[1], m_digits[5] + m_digits[6]); 
      break;
    default:
      pedestal = m_digits[0];
      break;
  }

  ATH_MSG_VERBOSE("getPedestal(): pedestal=" << pedestal);
  
  return pedestal;
}

int TileRawChannelBuilderWienerFilter::getBCIDIndex() {
  int bcidIndex = -1;

  const xAOD::EventInfo* eventInfo(0);

  if (evtStore()->retrieve(eventInfo).isSuccess()) {
    int bcid      = eventInfo->bcid();
    int distFront = m_bunchCrossingTool->distanceFromFront(bcid, Trig::IBunchCrossingTool::BunchCrossings);
    int distTail  = m_bunchCrossingTool->distanceFromTail(bcid, Trig::IBunchCrossingTool::BunchCrossings);

    ATH_MSG_VERBOSE( "EventInfo loaded: "
                  << " BCID=" << bcid
                  << " DistFront=" << distFront
                  << " DistTail=" << distTail);

    if (distFront == -1 || distTail == -1) {
      bcidIndex = -1;
    } else if (distFront < 3 && distTail > distFront) {
      bcidIndex = distFront;
    } else if (distTail < 3 && distTail < distFront) {
      bcidIndex = 6 - distTail;
    } else {
      bcidIndex = 3;
    }

  } else {
    ATH_MSG_VERBOSE("EventInfo not available");
  }

  ATH_MSG_VERBOSE("getBCIDIndex(): bcidIndex=" << bcidIndex);

  return bcidIndex;
}


double TileRawChannelBuilderWienerFilter::filter(int ros, int drawer, int channel
    , int &gain, double &pedestal, double &amplitude, double &time) {

  ATH_MSG_VERBOSE( "filter()" );

  amplitude = 0.;
  time = 0.;
  double chi2 = 0.;

  auto minMaxDigits = std::minmax_element(m_digits.begin(), m_digits.end());
  float minDigit = *minMaxDigits.first;
  float maxDigit = *minMaxDigits.second;

  if (maxDigit - minDigit < 0.01) { // constant value in all samples

    pedestal = minDigit;
    chi2 = 0.;
    ATH_MSG_VERBOSE( "CASE NO SIGNAL: maxdig-mindig = " << maxDigit << "-" << minDigit
        << " = " << maxDigit - minDigit );

    m_nConst++;

  } else {
    float weights[8];
    memset(weights, 0, sizeof(weights));

    int bcidIndex = getBCIDIndex();

    if (ros > 1 && channel == 1 && bcidIndex >= 0) {
      ATH_MSG_VERBOSE( "Switch to optimal mode");
      for (int wi = 0; wi < 8; wi++) {
        weights[wi] = m_ofcWiener[bcidIndex][wi];
      }
    } else {
      ATH_MSG_VERBOSE( "Switch to general mode");
      for (int wi = 0; wi < 8; wi++) {
        weights[wi] = m_gfcWiener[wi];
      }
    }

    for (unsigned int i = 0; i < m_digits.size(); i++) {
      amplitude += weights[i] * m_digits[i]; // Wiener coefs
    }
    amplitude += weights[7]; // Wiener bias

    double phase = 0.;
    pedestal = getPedestal(ros, drawer, channel, gain);

    chi2 = compute(ros, drawer, channel, gain, pedestal, amplitude, time, phase);

    m_nSignal++;
  }

  return chi2;
}

double TileRawChannelBuilderWienerFilter::compute(int ros, int drawer, int channel, int gain,
    double &pedestal, double &amplitude, double &time, double& phase) {

 ATH_MSG_VERBOSE( "compute();"
               << " ros=" << ros
               << " drawer="  << drawer
               << " channel=" << channel
               << " gain=" << gain
               << " pedestal=" << pedestal
               << " amplitude=" << amplitude
               << " time=" << time
               << " phase=" << phase );

  double chi2 = 0.;
  return chi2;
}
