/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopCPTools/TopJetMETCPTools.h"

#include <map>
#include <string>

// Top includes
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

// PathResolver include(s):
#include "PathResolver/PathResolver.h"

// Jet include(s):
#include "JetCalibTools/JetCalibrationTool.h"
#include "JetUncertainties/JetUncertaintiesTool.h"
#include "JetMomentTools/JetVertexTaggerTool.h"
#include "JetMomentTools/JetForwardJvtTool.h"
#include "JetSelectorTools/JetCleaningTool.h"
#include "JetResolution/JERTool.h"
#include "JetResolution/JERSmearingTool.h"
#include "JetJvtEfficiency/JetJvtEfficiency.h"

// MET include(s):
#include "METUtilities/METMaker.h"
#include "METUtilities/METSystematicsTool.h"

namespace top {

JetMETCPTools::JetMETCPTools(const std::string& name) :
    asg::AsgTool(name),

    m_jetJVT_ConfigFile("JVTlikelihood_20140805.root"),

    // Updated to December 2016 recommendations
    // config names are the same for Data and FS, for EM or LC jets
    m_jetAntiKt4_Data_ConfigFile("JES_data2016_data2015_Recommendation_Dec2016.config"),
    m_jetAntiKt4_Data_CalibSequence("JetArea_Residual_Origin_EtaJES_GSC_Insitu"),

    m_jetAntiKt4_MCFS_ConfigFile("JES_data2016_data2015_Recommendation_Dec2016.config"),
    m_jetAntiKt4_MCFS_CalibSequence("JetArea_Residual_Origin_EtaJES_GSC"),

    // AFII is different - still June 2015 pre-recommendations
    m_jetAntiKt4_MCAFII_ConfigFile("JES_MC15Prerecommendation_AFII_June2015.config"),
    m_jetAntiKt4_MCAFII_CalibSequence("JetArea_Residual_Origin_EtaJES_GSC"),

    // Particle-Flow jets, August 2016 recommendations, no GSC
    m_jetAntiKt4_PFlow_MCFS_ConfigFile("JES_MC15cRecommendation_PFlow_Aug2016.config"),
    m_jetAntiKt4_PFlow_MCFS_CalibSequence("JetArea_Residual_EtaJES_GSC") {

  declareProperty("config", m_config);
  declareProperty("release_series", m_release_series );

  declareProperty( "JetCalibrationTool" , m_jetCalibrationTool );
  declareProperty( "JetCalibrationToolLargeR" , m_jetCalibrationToolLargeR );

  declareProperty( "JetUncertaintiesTool" , m_jetUncertaintiesTool);
  declareProperty( "JetUncertaintiesToolReducedNPScenario1" , m_jetUncertaintiesToolReducedNPScenario1 );
  declareProperty( "JetUncertaintiesToolReducedNPScenario2" , m_jetUncertaintiesToolReducedNPScenario2 );
  declareProperty( "JetUncertaintiesToolReducedNPScenario3" , m_jetUncertaintiesToolReducedNPScenario3 );
  declareProperty( "JetUncertaintiesToolReducedNPScenario4" , m_jetUncertaintiesToolReducedNPScenario4 );

  declareProperty( "JetCleaningToolLooseBad" , m_jetCleaningToolLooseBad );
  declareProperty( "JetCleaningToolTightBad" , m_jetCleaningToolTightBad );

  declareProperty( "JetJERTool" , m_jetJERTool );
  declareProperty( "JetJERSmearingTool" , m_jetJERSmearingTool );
  declareProperty( "JetUpdateJvtTool" , m_jetUpdateJvtTool );

  declareProperty( "JES_data2016_data2015_Recommendation_Dec2016.config", m_jetAntiKt4_MCFS_ConfigFile);
  declareProperty( "JetCalibrationSequenceFS", m_jetAntiKt4_MCFS_CalibSequence );

  declareProperty( "JES_MC15cRecommendation_PFlow_Aug2016.config", m_jetAntiKt4_PFlow_MCFS_ConfigFile);
  declareProperty( "JetArea_Residual_EtaJES_GSC", m_jetAntiKt4_PFlow_MCFS_CalibSequence );

  declareProperty( "met_maker" , m_met_maker );
  declareProperty( "met_systematics" , m_met_systematics );
}

StatusCode JetMETCPTools::initialize() {
  ATH_MSG_INFO("top::JetMETCPTools initialize...");
  if (m_config->isTruthDxAOD()) {
    ATH_MSG_INFO("top::JetMETCPTools: no need to initialise anything on truth DxAOD");
    return StatusCode::SUCCESS;
  }

  if (m_config->useJets()) {
    if (m_config->makeAllCPTools()) {// skiping calibrations on mini-xAODs
      top::check(setupJetsCalibration(), "Failed to setup jet calibration tools");
    }
    // JVT tool needed for both MC and data (for both selection and SFs)
    top::check(setupJetsScaleFactors(), "Failed to setup jet scale-factor tools");
  }

  if (m_config->useLargeRJets())
    top::check(setupLargeRJetsCalibration(), "Failed to setup large-R jet calibration");

  if (m_config->makeAllCPTools()) {// MET not needed on mini-xAODs
    top::check(setupMET(), "Failed to setup MET tools");
  }

  return StatusCode::SUCCESS;
}

StatusCode JetMETCPTools::setupJetsCalibration() {

  // Release 21 specific
  // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ApplyJetCalibration2016#Calibrating_jets_in_Release_21
  if(m_release_series == 25){
    ATH_MSG_INFO("Updating configuration options for Rel21");
    // Data
    m_jetAntiKt4_Data_ConfigFile          = "JES_data2016_data2015_Recommendation_Dec2016_rel21.config";
    m_jetAntiKt4_Data_CalibSequence       = "JetArea_Residual_EtaJES_GSC_Insitu";
    // FS EM/LC
    m_jetAntiKt4_MCFS_ConfigFile          = "JES_data2016_data2015_Recommendation_Dec2016_rel21.config";
    m_jetAntiKt4_MCFS_CalibSequence       = "JetArea_Residual_EtaJES_GSC";
    // AFII EM/LC
    m_jetAntiKt4_MCAFII_ConfigFile        = ""; // No Rel21
    m_jetAntiKt4_MCAFII_CalibSequence     = ""; 
    // FS PFlow
    m_jetAntiKt4_PFlow_MCFS_ConfigFile    = "JES_MC15cRecommendation_PFlow_Aug2016.config"; // MC15c?
    m_jetAntiKt4_PFlow_MCFS_CalibSequence = "JetArea_Residual_EtaJES_GSC"; 
    // Note there are suggestions for PFlow in Data
    // JES_MC15cRecommendation_PFlow_Aug2016.config
    // JetArea_Residual_EtaJES_GSC
  }

  // Get jet calibration name and erase "Jets" from the end
  std::string caloJets_collection = m_config->sgKeyJets();
  std::string jetCalibrationName;
  if (caloJets_collection == "AntiKt4EMTopoNoElJets"){
    jetCalibrationName="AntiKt4EMTopoJets";
  }
  else{
    jetCalibrationName = caloJets_collection ;
  }
  jetCalibrationName.erase(jetCalibrationName.length() - 4);

  // In case of using JMS calibration
  if ( m_config->jetCalibSequence() == "JMS" ) {
    m_jetAntiKt4_MCFS_ConfigFile.erase(m_jetAntiKt4_MCFS_ConfigFile.length() - 7);//erase ".config" at the end
    m_jetAntiKt4_MCFS_ConfigFile += "_JMS.config";
    m_jetAntiKt4_MCFS_CalibSequence += "_JMS";
  }

  ///-- Calibration --///
  if (asg::ToolStore::contains<IJetCalibrationTool>("JetCalibrationTool")) {
    m_jetCalibrationTool = asg::ToolStore::get<IJetCalibrationTool>("JetCalibrationTool");
  } else {
    std::string calibConfig, calibSequence;
    if (m_config->isMC()) {
      if (m_config->isAFII()) {
        calibConfig = m_jetAntiKt4_MCAFII_ConfigFile;
        calibSequence = m_jetAntiKt4_MCAFII_CalibSequence;
      } else {
        calibConfig = m_jetAntiKt4_MCFS_ConfigFile;
        calibSequence = m_jetAntiKt4_MCFS_CalibSequence;
      }
    } else {
      calibConfig = m_jetAntiKt4_Data_ConfigFile;
      calibSequence = m_jetAntiKt4_Data_CalibSequence;
    }
    if (m_config->useParticleFlowJets()) {
      calibConfig = m_jetAntiKt4_PFlow_MCFS_ConfigFile;
      calibSequence = m_jetAntiKt4_PFlow_MCFS_CalibSequence;
    }

    JetCalibrationTool* jetCalibrationTool = new JetCalibrationTool("JetCalibrationTool");
    top::check(asg::setProperty(jetCalibrationTool, "JetCollection", jetCalibrationName),
                "Failed to set JetCollection " + jetCalibrationName);
    top::check(asg::setProperty(jetCalibrationTool, "ConfigFile", calibConfig),
                "Failed to set ConfigFile " + calibConfig);
    top::check(asg::setProperty(jetCalibrationTool, "CalibSequence", calibSequence),
                "Failed to set CalibSequence " + calibSequence);
    top::check(asg::setProperty(jetCalibrationTool, "IsData", !m_config->isMC()),
                "Failed to set IsData " + !m_config->isMC());

    top::check(jetCalibrationTool->initializeTool(jetCalibrationName),
                "Failed to initialize JetCalibrationTool");
    m_jetCalibrationTool = jetCalibrationTool;
  }

  ///-- Update JVT --///
  const std::string jvt_update_name = "JetUpdateJvtTool";
  if (asg::ToolStore::contains<IJetUpdateJvt>(jvt_update_name)) {
    m_jetUpdateJvtTool = asg::ToolStore::get<IJetUpdateJvt>(jvt_update_name);
  } else {
    IJetUpdateJvt* jetUpdateJvtTool = new JetVertexTaggerTool(jvt_update_name);
    top::check(asg::setProperty(jetUpdateJvtTool, "JVTFileName", "JetMomentTools/" + m_jetJVT_ConfigFile),
                "Failed to set JVTFileName for JetUpdateJvtTool");
    top::check(jetUpdateJvtTool->initialize(), "Failed to initialize");
    m_jetUpdateJvtTool = jetUpdateJvtTool;
  }

  ///-- Update fJVT --///
  const std::string fjvt_tool_name = "fJVTTool";
  if (asg::ToolStore::contains<IJetModifier>(fjvt_tool_name)) {
    m_fjvtTool = asg::ToolStore::get<IJetModifier>(fjvt_tool_name);
  } else {
    IJetModifier* fJVTTool = new JetForwardJvtTool(fjvt_tool_name);
    top::check(asg::setProperty(fJVTTool, "JvtMomentName", "AnalysisTop_JVT"),
                "Failed to set JvtMomentName for JetForwardJvtTool");
    // following instructions from:
    // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/METUtilities#MET_with_forward_JVT
    if (m_config->fwdJetAndMET() == "fJVT") {
      top::check(asg::setProperty(fJVTTool, "CentralMaxPt", 60e3),
                  "Failed to set CentralMaxPt for JetForwardJvtTool");
    }
    top::check(fJVTTool->initialize(), "Failed to initialize " + fjvt_tool_name);
    m_fjvtTool = fJVTTool;
  }

  // TODO(tom.neep@cern.ch): Currently we don't have uncertainties for
  //       particle flow jets, so return successfully
  //       here. Keep an eye on this in the future.
  //       Also: no jet cleaning for particle-flow jets.
  if (m_config->useParticleFlowJets())
    return StatusCode::SUCCESS;

  ///-- Jet Cleaning Tools --///
  m_jetCleaningToolLooseBad = setupJetCleaningTool("LooseBad");
  m_jetCleaningToolTightBad = setupJetCleaningTool("TightBad");

  // Uncertainties
  // Is our MC full or fast simulation?
  std::string MC_type = (m_config->isAFII()) ? "AFII" : "MC15";

  std::string conference = "Moriond2017";

  // Are we doing multiple JES for the reduced NP senarios?
  if (!m_config->doMultipleJES()) {
    m_jetUncertaintiesTool
      = setupJetUncertaintiesTool("JetUncertaintiesTool",
                                  jetCalibrationName, MC_type,
                                  "JES_2016/"
                                  + conference
                                  +"/JES2016_"
                                  + m_config->jetUncertainties_NPModel()
                                  + ".config",nullptr);
  } else {
    m_jetUncertaintiesToolReducedNPScenario1
      = setupJetUncertaintiesTool("JetUncertaintiesToolReducedNPScenario1",
                                  jetCalibrationName, MC_type,
                                  "JES_2016/"
                                  + conference
                                  + "/JES2016_SR_Scenario1.config",nullptr);
    m_jetUncertaintiesToolReducedNPScenario2
      = setupJetUncertaintiesTool("JetUncertaintiesToolReducedNPScenario2",
                                  jetCalibrationName, MC_type,
                                  "JES_2016/"
                                  + conference
                                  + "/JES2016_SR_Scenario2.config",nullptr);
    m_jetUncertaintiesToolReducedNPScenario3
      = setupJetUncertaintiesTool("JetUncertaintiesToolReducedNPScenario3",
                                  jetCalibrationName, MC_type,
                                  "JES_2016/"
                                  + conference
                                  + "/JES2016_SR_Scenario3.config",nullptr);
    m_jetUncertaintiesToolReducedNPScenario4
      = setupJetUncertaintiesTool("JetUncertaintiesToolReducedNPScenario4",
                                  jetCalibrationName, MC_type,
                                  "JES_2016/"
                                  + conference
                                  + "/JES2016_SR_Scenario4.config",nullptr);
  }

  // JER Tool
  if (asg::ToolStore::contains<IJERTool>("JetJERTool")) {
    m_jetJERTool = asg::ToolStore::get<IJERTool>("JetJERTool");
  } else {
    IJERTool* jetJERTool = new JERTool("JetJERTool");
    top::check(asg::setProperty(jetJERTool, "CollectionName", "AntiKt4EMTopoJets"),
                "Failed to set CollectionName to JetJERTool");
    top::check(jetJERTool->initialize(), "Failed to initialize");
    m_jetJERTool = jetJERTool;
  }

  // JER Smearing
  const std::string jersmear_name = "JetJERSmearingTool";
  if (asg::ToolStore::contains<IJERSmearingTool>(jersmear_name)) {
    m_jetJERSmearingTool = asg::ToolStore::get<IJERSmearingTool>(jersmear_name);
  } else {
    IJERSmearingTool* jetJERSmearingTool = new JERSmearingTool(jersmear_name);
    top::check(asg::setProperty(jetJERSmearingTool, "JERTool" , m_jetJERTool),
                "Failed to JERTool to " + jersmear_name);
    top::check(asg::setProperty(jetJERSmearingTool, "ApplyNominalSmearing", false),
                "Failed to ApplyNominalSmearing for " + jersmear_name);
    top::check(asg::setProperty(jetJERSmearingTool, "isMC", m_config->isMC()),
                "Failed to isMC to " + jersmear_name);
    top::check(asg::setProperty(jetJERSmearingTool, "SystematicMode", m_config->jetJERSmearingModel()),
                "Failed to SystematicMode for " + jersmear_name);
    top::check(jetJERSmearingTool->initialize(),
                "Failed to initialize " + jersmear_name);
    m_jetJERSmearingTool = jetJERSmearingTool;
  }

  return StatusCode::SUCCESS;
}

StatusCode JetMETCPTools::setupLargeRJetsCalibration() {
  std::string jetCalibrationNameLargeR = m_config->sgKeyLargeRJets();
  // erase "Jets" from the end
  jetCalibrationNameLargeR.erase(jetCalibrationNameLargeR.length() - 4);

  if (asg::ToolStore::contains<IJetCalibrationTool>("JetCalibrationToolLargeR")) {
    m_jetCalibrationToolLargeR = asg::ToolStore::get<IJetCalibrationTool>("JetCalibrationToolLargeR");
  } else {
    // Only a single calib config/sequence for MC and data
    // so just put it here for now.
    std::string calibConfigLargeR = "";
    const std::string calibChoice = m_config->largeRJESJMSConfig();
    if (calibChoice == "CombinedMass") {
      calibConfigLargeR = "JES_MC15recommendation_FatJet_Nov2016_QCDCombinationUncorrelatedWeights.config";
    }
    else if (calibChoice == "CaloMass") {
      calibConfigLargeR = "JES_MC15recommendation_FatJet_June2015.config";
    }
    else {
      ATH_MSG_ERROR("Unknown largeRJESJMSConfig "+calibChoice);
      return StatusCode::FAILURE;
    }
    const std::string calibSequenceLargeR = "EtaJES_JMS";
    
    JetCalibrationTool* jetCalibrationToolLargeR
      = new JetCalibrationTool("JetCalibrationToolLargeR");
    top::check(asg::setProperty(jetCalibrationToolLargeR, "JetCollection", jetCalibrationNameLargeR),
                "Failed to set JetCollection " + jetCalibrationNameLargeR);
    top::check(asg::setProperty(jetCalibrationToolLargeR, "ConfigFile", calibConfigLargeR),
                "Failed to set ConfigFile " + calibConfigLargeR);
    top::check(asg::setProperty(jetCalibrationToolLargeR, "CalibSequence", calibSequenceLargeR),
                "Failed to set CalibSequence " + calibSequenceLargeR);
    top::check(asg::setProperty(jetCalibrationToolLargeR, "IsData", !m_config->isMC()),
                "Failed to set IsData " + !m_config->isMC());

    top::check(jetCalibrationToolLargeR->initializeTool(jetCalibrationNameLargeR),
                "Failed to initialize JetCalibrationToolLargeR");
    m_jetCalibrationToolLargeR = jetCalibrationToolLargeR;
  }

  // Moriond2017 uncertainty recommendations:
  // names = "UJ_2016/Moriond2017/UJ2016_CaloMass_strong.config"     // strong,medium,weak    
  // names = "UJ_2016/Moriond2017/UJ2016_CombinedMass_strong.config" // strong,medium,weak

  std::string conference("");
  std::string configDir("");
  std::vector<std::string>* variables = nullptr;
  std::string largeRJES_config = m_config->largeRJESUncertaintyConfig();
  std::string MC_type = "MC15";

  conference = "Moriond2017";
  configDir  = "UJ_2016";
  MC_type += "c";

  variables = new std::vector<std::string>;
  variables->push_back("pT");
  std::string variable;
  size_t pos_end = 0;
  while( (pos_end = largeRJES_config.find(",")) != std::string::npos) {
    variable = largeRJES_config.substr(0,pos_end);
    variables->push_back(variable);
    largeRJES_config.erase(0,pos_end+1);
  }
  variables->push_back(largeRJES_config);

  largeRJES_config = m_config->largeRJESJMSConfig();
  if (largeRJES_config.find("UJ2016_") != 0) largeRJES_config.insert(0, "UJ2016_");

  m_jetUncertaintiesToolLargeR_strong
    = setupJetUncertaintiesTool("JetUncertaintiesToolLargeR_Strong",
                                jetCalibrationNameLargeR, MC_type,
                                configDir+"/"+conference
                                + "/"+largeRJES_config+"_strong.config",variables);
  m_jetUncertaintiesToolLargeR_medium
    = setupJetUncertaintiesTool("JetUncertaintiesToolLargeR_Medium",
                                jetCalibrationNameLargeR, MC_type,
                                configDir+"/"+conference
                                + "/"+largeRJES_config+"_medium.config",variables);
  m_jetUncertaintiesToolLargeR_weak
    = setupJetUncertaintiesTool("JetUncertaintiesToolLargeR_Weak",
                                jetCalibrationNameLargeR, MC_type,
                                configDir+"/"+conference
                                + "/"+largeRJES_config+"_weak.config",variables);

  if (variables) delete variables;
  return StatusCode::SUCCESS;
}

StatusCode JetMETCPTools::setupJetsScaleFactors() {
  // <tom.neep@cern.ch> Added 16th Feb 2016.
  // Jet JVT tool - can be used for both selection and for SFs
  // Since we use this for jet selection we also need it for data
  const std::string jvt_tool_name = "JetJvtEfficiencyTool";
  const std::string JVT_SFFile =
      (m_config->sgKeyJets()=="AntiKt4LCTopoJets")?
      "JetJvtEfficiency/Moriond2017/JvtSFFile_LC.root":
      "JetJvtEfficiency/Moriond2017/JvtSFFile_EM.root";// default is EM jets
  if (asg::ToolStore::contains<CP::IJetJvtEfficiency>(jvt_tool_name)) {
    m_jetJvtTool = asg::ToolStore::get<CP::IJetJvtEfficiency>(jvt_tool_name);
  } else {
    CP::JetJvtEfficiency* jetJvtTool = new CP::JetJvtEfficiency(jvt_tool_name);
    // Medium WP default for EM or LC jets; special WP for PFlow jets
    top::check(jetJvtTool->setProperty("WorkingPoint", (m_config->useParticleFlowJets())?"PFlow":"Medium"),
                "Failed to set JVT WP");
    top::check(jetJvtTool->setProperty("SFFile", JVT_SFFile),
                "Failed to set JVT SFFile name");
    top::check(jetJvtTool->setProperty("JetJvtMomentName", "AnalysisTop_JVT"),
                "Failed to set JVT decoration name");
    top::check(jetJvtTool->setProperty("TruthLabel", "AnalysisTop_isHS"),
                "Failed to set JVT TruthLabel decoration name");
    top::check(jetJvtTool->initialize(), "Failed to initialize JVT tool");
    m_jetJvtTool = jetJvtTool;
  }
  return StatusCode::SUCCESS;
}

StatusCode JetMETCPTools::setupMET()
{
  if ( asg::ToolStore::contains<IMETMaker>("met::METMaker") ) {
      m_met_maker = asg::ToolStore::get<IMETMaker>("met::METMaker");
  } else {
    met::METMaker* metMaker = new met::METMaker("met::METMaker");
    top::check( metMaker->setProperty("JetJvtMomentName", "AnalysisTop_JVT"), "Failed to set METMaker JVT moment name" );
    // following instructions from:
    // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EtmissRecommendationsRel20p7Temp#Working_Points_NEW
    // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/METUtilities#MET_with_forward_JVT
    if (m_config->fwdJetAndMET() == "Tight") {
      top::check( metMaker->setProperty("JetSelection", "Tight"), "Failed to set METMaker JetSelection to Tight" );
    }
    else if (m_config->fwdJetAndMET() == "fJVT") {
      top::check( metMaker->setProperty("JetRejectionDec", "passFJVT"), "Failed to set METMaker JetRejectionDec to passFJVT" );
    }
    top::check( metMaker->initialize() , "Failed to initialize" );
    metMaker->msg().setLevel( MSG::INFO );
    m_met_maker = metMaker;
  }

  if ( asg::ToolStore::contains<IMETSystematicsTool>("met::METSystematicsTool") ) {
      m_met_systematics = asg::ToolStore::get<IMETSystematicsTool>("met::METSystematicsTool");
  } else {
    met::METSystematicsTool* metSyst = new met::METSystematicsTool("met::METSystematicsTool");

    top::check( metSyst->setProperty("ConfigSoftTrkFile", "TrackSoftTerms.config"), "Failed to set property" );

    // Turn off soft calo term systematics... if left on we get some warnings
    top::check( metSyst->setProperty("ConfigSoftCaloFile", "" ), "Failed to set property" );
    top::check( metSyst->initialize() , "Failed to initialize" );
    m_met_systematics = metSyst;
  }


  return StatusCode::SUCCESS;
}

ICPJetUncertaintiesTool*
JetMETCPTools::setupJetUncertaintiesTool(const std::string& name,
                                        const std::string& jet_def,
                                        const std::string& mc_type,
                                        const std::string& config_file,
                                        std::vector<std::string>* variables) {
  ICPJetUncertaintiesTool* tool = nullptr;
  if (asg::ToolStore::contains<ICPJetUncertaintiesTool>(name)) {
    tool = asg::ToolStore::get<ICPJetUncertaintiesTool>(name);
  } else {
    tool = new JetUncertaintiesTool(name);
    top::check(asg::setProperty(tool, "JetDefinition", jet_def),
                "Failed to set JetDefinition for " + name);
    top::check(asg::setProperty(tool, "MCType", mc_type),
                "Failed to set MCType for " + name);
    top::check(asg::setProperty(tool, "ConfigFile", config_file),
                "Failed to set ConfigFile for " + name);
    if (variables != nullptr){
      top::check(asg::setProperty(tool, "VariablesToShift", *variables),
          "Failed to set VariablesToShift for LargeR Jes Uncertainty "+ name);
    }
    top::check(tool->initialize(), "Failed to initialize " + name);
  }
  return tool;
}

IJetSelector* JetMETCPTools::setupJetCleaningTool(const std::string& WP) {
  IJetSelector* tool = nullptr;
  std::string name  = "JetCleaningTool" + WP;
  if (asg::ToolStore::contains<IJetSelector>(name)) {
    tool = asg::ToolStore::get<IJetSelector>(name);
  } else {
    tool = new JetCleaningTool(name);
      top::check(asg::setProperty(tool, "CutLevel", WP),
                  "Failed to set CutLevel to " + WP + " for " + name);
      top::check(asg::setProperty(tool, "DoUgly", false),
                  "Failed to set DoUgly for " + name);
      top::check(tool->initialize(), "Failed to initialize " + name);
    }
  return tool;
}

}  // namespace top
