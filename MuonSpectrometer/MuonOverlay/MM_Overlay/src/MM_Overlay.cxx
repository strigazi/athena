/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007
// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008
// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011
// Alexandre Laurier <alexandre.laurier@cern.ch>, May 2019

#include <MM_Overlay/MM_Overlay.h>

#include <StoreGate/ReadHandle.h>
#include <StoreGate/WriteHandle.h>

#include <IDC_OverlayBase/IDC_OverlayHelpers.h>


//================================================================
namespace Overlay
{
  /** template specialization function to add 2 MM Digits - basically the operator+=
   * A declaration of the specialization must happen before the template is used from
   * the overlayContainer() method.  So we just put this implementation at the beginning
   * of this file.
   */
  template<>
  void mergeChannelData(MmDigit& signalDigit,
                        const MmDigit& bkgDigit,
                        IDC_MuonOverlayBase *algorithm)
  {
    // We want to use the integration window provided by MmOverlay, thus the constraint
    MM_Overlay *parent = dynamic_cast<MM_Overlay *>(algorithm);
    if (!parent) {
      throw std::runtime_error("mergeChannelData<MmDigit>() called by a wrong parent algorithm? Must be MM_Overlay.");
    }

    float sig_time = signalDigit.stripResponseTime()[0]; 
    float bkg_time = bkgDigit.stripResponseTime()[0]; 
    // As of now, we need to decide how to overlay MM digits
    // NEEDS TO BE ADDRESSED
    // For this preliminary version of July 2019, use only the data from the 1st digit in vector. 
    /** signal masks the background */
    if ( abs(sig_time - bkg_time) > parent->timeIntegrationWindow() && sig_time < bkg_time ) {
      // do nothing - keep baseDigit.
    }
    /** Background hit masks the signal hit */
    else if ( abs(sig_time - bkg_time) > parent->timeIntegrationWindow() && sig_time > bkg_time ) {
      // Use the background digit as the final answer
      signalDigit = bkgDigit;
    }
    /** the 2 hits overlap withing the time integration window
      For now, just add total charge */     
    else if ( abs(sig_time - bkg_time) < parent->timeIntegrationWindow() )  {
      // Use the earliest time
      // And use all the other values of the signal digit.
      float time = std::min( sig_time, bkg_time );
      float charge = signalDigit.stripResponseCharge()[0] + bkgDigit.stripResponseCharge()[0];
      std::vector<float> Time;
      std::vector<float> Charge;
      Time.push_back(time);
      Charge.push_back(charge);
      signalDigit = MmDigit(signalDigit.identify(), Time, signalDigit.stripResponsePosition(),Charge,signalDigit.chipResponseTime(),signalDigit.chipResponsePosition(),signalDigit.chipResponseCharge(),signalDigit.stripTimeForTrigger(),signalDigit.stripPositionForTrigger(),signalDigit.stripChargeForTrigger(),signalDigit.MMFE_VMM_idForTrigger(),signalDigit.VMM_idForTrigger());
    }
  }
} // namespace Overlay


//================================================================
MM_Overlay::MM_Overlay(const std::string &name, ISvcLocator *pSvcLocator)
  : IDC_MuonOverlayBase(name, pSvcLocator)
{
}

//================================================================
StatusCode MM_Overlay::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  if (!m_includeBkg) {
    ATH_MSG_DEBUG("Disabling use of background RDOs...");
    ATH_CHECK( m_bkgInputKey.assign("") );
  }

  // Check and initialize keys
  ATH_CHECK( m_bkgInputKey.initialize(!m_bkgInputKey.key().empty()) );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_bkgInputKey );
  ATH_CHECK(m_signalInputKey.initialize());
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_signalInputKey );
  ATH_CHECK(m_outputKey.initialize());
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey );

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode MM_Overlay::execute() {
  ATH_MSG_DEBUG("execute() begin");


  const MmDigitContainer *bkgContainerPtr = nullptr;
  if (m_includeBkg) {
    SG::ReadHandle<MmDigitContainer> bkgContainer (m_bkgInputKey);
    if (!bkgContainer.isValid()) {
      ATH_MSG_ERROR("Could not get background MM container " << bkgContainer.name() << " from store " << bkgContainer.store());
      return StatusCode::FAILURE;
    }
    bkgContainerPtr = bkgContainer.cptr();

    ATH_MSG_DEBUG("Found background MMDigitContainer called " << bkgContainer.name() << " in store " << bkgContainer.store());
    ATH_MSG_DEBUG("MM Background = " << Overlay::debugPrint(bkgContainer.cptr()));
    ATH_MSG_VERBOSE("MM background has digit_size " << bkgContainer->digit_size());
  }

  SG::ReadHandle<MmDigitContainer> signalContainer(m_signalInputKey);
  if(!signalContainer.isValid() ) {
    ATH_MSG_ERROR("Could not get signal MM container " << signalContainer.name() << " from store " << signalContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal MmDigitContainer called " << signalContainer.name() << " in store " << signalContainer.store());
  ATH_MSG_DEBUG("MM Signal     = " << Overlay::debugPrint(signalContainer.cptr()));
  ATH_MSG_VERBOSE("MM signal has digit_size " << signalContainer->digit_size());

  SG::WriteHandle<MmDigitContainer> outputContainer(m_outputKey);
  ATH_CHECK(outputContainer.record(std::make_unique<MmDigitContainer>(signalContainer->size())));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output MmDigitContainer called " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output MmDigitContainer called " << outputContainer.name() << " in store " << outputContainer.store());

  // Do the actual overlay
  ATH_CHECK(overlayContainer(bkgContainerPtr, signalContainer.cptr(), outputContainer.ptr()));
  ATH_MSG_DEBUG("MM Result     = " << Overlay::debugPrint(outputContainer.cptr()));

  ATH_MSG_DEBUG("execute() end");

  return StatusCode::SUCCESS;
}
