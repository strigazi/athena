/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGL2CALORINGERFEX_H
#define TRIGL2CALORINGERFEX_H

///std include(s)
#include <string>
#include <vector>

///Base from trigger
#include "TrigInterfaces/FexAlgo.h"
#include "TrigTimeAlgs/TrigTimer.h"

///xAOD include(s)
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRNNOutput.h"
#include "xAODTrigCalo/TrigEMCluster.h"

/// Luminosity tool
#include "LumiBlockComps/ILumiBlockMuTool.h"
#include "TrigMultiVarHypo/tools/MultiLayerPerceptron.h"
#include "TrigMultiVarHypo/tools/TrigL2CaloRingerReader.h"
#include "TrigMultiVarHypo/preproc/TrigRingerPreprocessor.h"


class TrigL2CaloRingerFex: public HLT::FexAlgo {

  public:

    TrigL2CaloRingerFex(const std::string & name, ISvcLocator* pSvcLocator);
    virtual ~TrigL2CaloRingerFex()
    {;}

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* /*inputTE*/, HLT::TriggerElement* outputTE);
    HLT::ErrorCode hltFinalize();

  protected:
    ///Time monitoring
    TrigTimer* m_storeTimer;
    TrigTimer* m_normTimer;
    TrigTimer* m_decisionTimer;

  private:

    /* Helper functions for feature extraction */
    const xAOD::TrigRingerRings* get_rings(const HLT::TriggerElement* te);

    std::string m_calibPath;
    ///feature keys
    std::string m_hlt_feature;
    std::string m_feature;
    std::string m_key;

    float       m_lumiCut;
    float       m_output;
    bool        m_useLumiTool;
    bool        m_useEtaVar;
    bool        m_useLumiVar;
		/// Specialization for the algorithm without retrieving hadronic information
		bool m_useHad;
		/// Upper bound to not retrieve hadronic information
		float m_noHadGeV_limit;


    //LumiTool
    ToolHandle<ILumiBlockMuTool>  m_lumiBlockMuTool;
    ///Prepoc configuration
    std::vector<unsigned int>  m_nRings;
    std::vector<unsigned int>  m_normRings;
    std::vector<unsigned int>  m_sectionRings;
    ///Prepoc configuration
    std::vector<unsigned int>  m_nRingsNoHad;
    std::vector<unsigned int>  m_normRingsNoHad;
    std::vector<unsigned int>  m_sectionRingsNoHad;

    ///Discriminator holder
    std::vector<MultiLayerPerceptron*>       m_discriminators;
    std::vector<TrigRingerPreprocessor*>     m_preproc;
    std::vector<TrigRingerPreprocessor*>     m_preprocNoHad;
    TrigL2CaloRingerReader                   m_reader;

};
//!===============================================================================================
// get the ringer rings inside of container
const xAOD::TrigRingerRings* TrigL2CaloRingerFex::get_rings(const HLT::TriggerElement* te){
    const xAOD::TrigRingerRings *pattern = nullptr;
    HLT::ErrorCode status = getFeature(te, pattern, m_feature);
    return (status == HLT::OK) ? pattern : nullptr;
}
//!===============================================================================================
#endif /* TRIGL2CALORINGERHYPO_H */
