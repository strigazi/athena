/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// Based on CPToolTester.cxx (A. Kraznahorkay) 
/// and ut_ath_checkTrigger_test.cxx (W. Buttinger)
/// Contact: jmaurer@cern.ch
/*
 *    The set of examples 3a - 3e illustrates the use of lepton selection tags
 * for various scenarios:
 * 
 * - Example 3a: trigger = 2e12_lhloose_L12EM10VH, selecting events containing
 *               >=2 loose electrons, the leading-pT electron always satisfying
 *               in addition tight PID+isolation requirements.
 * 
 * - Example 3b: trigger = e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose
 *                         || 2e12_lhloose_L12EM10VH
 *               selecting events with >=2 loose electrons where the leading-pT
 *               electron also satisfies medium PID requirements.
 *               Only the latter is allowed to fire the single-electron trigger.
 * 
 * - Example 3c: trigger = e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose
 *                         || 2e12_lhloose_L12EM10VH
 *               selecting events with >=2 loose electrons. Any electron also
 *               satisfying medium PID requirements is allowed to fire the
 *               single-electron trigger.
 * 
 * - Example 3d: trigger = 2e17_lhvloose_nod0 || e7_lhmedium_nod0_mu24
 *       || e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0
 *               same type of selection as example 3c but with 3 PID working
 *               points, using two distinct decorations.
 * 
 * - Example 3e: same scenario as example 3d, but using an alternative
 *               implementation that requires only one decoration.
 * 
 */
/*
 *    We use two decorations, 'MyMedium' and 'MyTight'
 * Note that the TrigGlob tool considers only one single tag per electron. When
 * an electron has >=2 non-zero decorations (e.g. 'MyMedium' + 'MyTight'),
 * the tag associated to the electron by the tool is chosen as the first one
 * appearing in the list of decorations provided in the 'LeptonTagDecorations'
 * property.
 *     
 */

// ROOT include(s):
#include <TFile.h>
#include <TError.h>

// Infrastructure include(s):
#ifdef XAOD_STANDALONE
    #include "xAODRootAccess/Init.h"
    #include "xAODRootAccess/TEvent.h"
    #include "xAODRootAccess/TStore.h"
#else
    #include "AthAnalysisBaseComps/AthAnalysisHelper.h"
    #include "POOLRootAccess/TEvent.h"
#endif

// EDM include(s):
#include "AsgTools/AnaToolHandle.h"
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
#include "MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h"
#include "TriggerAnalysisInterfaces/ITrigGlobalEfficiencyCorrectionTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "PATCore/PATCoreEnums.h"
#include "AthContainers/AuxElement.h"

// stdlib include(s):
#include <sstream>
#include <random>
#include <vector>
#include <array>
using std::vector;
using std::string;

/// Helper function to split comma-delimited strings
namespace { vector<string> split_comma_delimited(const std::string&); }

#define MSGSOURCE "Example 3d"

int main(int argc, char* argv[])
{
    const char* filename = nullptr;
    bool debug = false, cmdline_error = false, toys = false;
    for(int i=1;i<argc;++i)
    {
        if(string(argv[i]) == "--debug") debug = true;
        else if(string(argv[i]) == "--toys") toys = true;
        else if(!filename && *argv[i]!='-') filename = argv[i];
        else cmdline_error = true;
    }
    if(!filename || cmdline_error)
    {
        Error(MSGSOURCE, "No file name received!");
        Error(MSGSOURCE, "  Usage: %s [--debug] [--toys] [DxAOD file name]", argv[0]);
        return 1;
    }
    #ifdef XAOD_STANDALONE
        xAOD::Init(MSGSOURCE).ignore();
        TFile* file = TFile::Open(filename, "READ");
        if(!file)
        {
            Error(MSGSOURCE, "Unable to open file!");
            return 2;
        }
        xAOD::TEvent event(xAOD::TEvent::kClassAccess);
        xAOD::TStore store;
        StatusCode::enableFailure();
    #else
       IAppMgrUI* app = POOL::Init();
       POOL::TEvent event(POOL::TEvent::kClassAccess);
       TString file(filename);
    #endif
    event.readFrom(file).ignore();
    Long64_t entries = event.getEntries();
    Info(MSGSOURCE, "Number of events in the file: %lli", entries);

    /* ********************************************************************** */
    
    Info(MSGSOURCE, "Configuring the electron CP tools");
    /// For property 'ElectronEfficiencyTools':
    ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> electronEffTools;
    /// For property 'ElectronScaleFactorTools':
    ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> electronSFTools; 
        /// For property 'ListOfLegsPerTool':
    std::map<std::string,std::string> legsPerTool;
    /// For property 'ListOfTagsPerTool':
    std::map<std::string,std::string> tagsPerTool;
    /// For property 'ElectronLegsPerTag':
    std::map<std::string,std::string> legsPerTag;
    /// To tag electron(s) as 'MyMedium' and 'MyTight'
    SG::AuxElement::Decorator<char> dec_medium("MyMedium");
    SG::AuxElement::Decorator<char> dec_tight("MyTight");
    /// To emulate PID selection (90% loose-to-medium/medium-to-tight eff.)
    std::bernoulli_distribution bernoulliPdf(0.9);

    /// RAII on-the-fly creation of electron CP tools:
    vector<asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool>> factory;
    enum{ cLEGS, cTAG, cKEY, cPID, cISO };
    std::vector<std::array<std::string,5> > toolConfigs = {
        /// <list of legs>, <list of tags>, <key in map file>, <PID WP>, <iso WP>
        /// Single-electron trigger: electrons tagged 'MyTight'
        {"e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0", "MyTight", 
            "2016_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0", "Tight", "GradientLoose"},
        /// Electron-muon trigger: electrons tagged 'MyTight' or 'MyMedium'
        {"e7_lhmedium_nod0", "MyMedium,MyTight", "2016_e7_lhmedium_nod0", "Medium", ""},
        /// Dielectron trigger: all electrons (tagged or not)
        {"e17_lhvloose_nod0", "*,MyMedium,MyTight", "2016_e17_lhvloose_nod0", "LooseBLayer", ""},
     };

    const char* mapPath = "ElectronEfficiencyCorrection/2015_2017/"
            "rel21.2/Moriond_February2018_v2/map6.txt";
    for(auto& cfg : toolConfigs) /// one instance per trigger leg x working point
    for(int j=0;j<2;++j) /// two instances: 0 -> MC efficiencies, 1 -> SFs
    {
        string name = "AsgElectronEfficiencyCorrectionTool/"
                + ((j? "ElTrigEff_" : "ElTrigSF_")
                + std::to_string(factory.size()/2));
        auto t = factory.emplace(factory.end(), name);
        t->setProperty("MapFilePath", mapPath).ignore();
        t->setProperty("TriggerKey", string(j?"":"Eff_") + cfg[cKEY]).ignore();
        t->setProperty("IdKey", cfg[cPID]).ignore();
        t->setProperty("IsoKey", cfg[cISO]).ignore();

        t->setProperty("CorrelationModel", "TOTAL").ignore();
        t->setProperty("ForceDataType", (int)PATCore::ParticleDataType::Full).ignore();
        if(t->initialize() != StatusCode::SUCCESS)
        {
            Error(MSGSOURCE, "Unable to initialize the electron CP tool <%s>!",
                    t->name().c_str());
            return 3;
        }
        auto& handles = (j? electronSFTools : electronEffTools);
        handles.push_back(t->getHandle());
        /// Safer to retrieve the name from the final ToolHandle, it might be
        /// prefixed (by the parent tool name) when the handle is copied    
        name = handles[handles.size()-1].name();
        legsPerTool[name] = cfg[cLEGS];
        tagsPerTool[name] = cfg[cTAG];
        if(!j)
        {
            for(auto& tag : ::split_comma_delimited(cfg[cTAG]))
            {
                if(legsPerTag[tag]=="") legsPerTag[tag] = cfg[cLEGS];
                else legsPerTag[tag] += "," + cfg[cLEGS];
            }
        }

    }

    /* ********************************************************************** */
    
    Info(MSGSOURCE, "Configuring the muon CP tools");
    /// For property 'MuonTools':
    ToolHandleArray<CP::IMuonTriggerScaleFactors> muonTools;
    asg::AnaToolHandle<CP::IMuonTriggerScaleFactors> muonTool("CP::MuonTriggerScaleFactors/MuonTrigEff");
    muonTool.setProperty("MuonQuality", "Tight").ignore();
    muonTool.setProperty("useRel207", false).ignore();
    if(muonTool.initialize() != StatusCode::SUCCESS)
    {
        Error(MSGSOURCE, "Unable to initialize the muon CP tool!");
        return 3;
    }
    muonTools.push_back(muonTool.getHandle());
    
    /* ********************************************************************** */
    
    Info(MSGSOURCE, "Configuring the global trigger SF tool");
    asg::AnaToolHandle<ITrigGlobalEfficiencyCorrectionTool> myTool("TrigGlobalEfficiencyCorrectionTool/TrigGlobal");
    myTool.setProperty("ElectronEfficiencyTools", electronEffTools).ignore();
    myTool.setProperty("ElectronScaleFactorTools", electronSFTools).ignore();
    myTool.setProperty("MuonTools", muonTools).ignore();
    const char* triggers2016 = 
        "e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0"
        "|| e7_lhmedium_nod0_mu24"
        "|| 2e17_lhvloose_nod0";
    myTool.setProperty("TriggerCombination2016", triggers2016).ignore();
    /// Listing 'Tight' first as it has higher priority (an electron with both
    /// non-zero 'Tight'+'Medium' decorations will then be tagged as 'Tight')
    myTool.setProperty("LeptonTagDecorations", "MyTight,MyMedium").ignore();
    myTool.setProperty("ListOfLegsPerTool", legsPerTool).ignore();
    myTool.setProperty("ListOfTagsPerTool", tagsPerTool).ignore();
    myTool.setProperty("ListOfLegsPerTag", legsPerTag).ignore();

    if(debug) myTool.setProperty("OutputLevel", MSG::DEBUG).ignore();
    if(toys) myTool.setProperty("NumberOfToys", 1000).ignore();
    if(myTool.initialize() != StatusCode::SUCCESS)
    {
        Error(MSGSOURCE, "Unable to initialize the TrigGlob tool!");
        return 3;
    }
    
    /// Uniform random run number generation spanning the target dataset.
    /// In real life, use the PileupReweightingTool instead!
    const unsigned periodRuns[] = {
        /// 2016 periods A-L
        296939, 300345, 301912, 302737, 303638, 303943, 305291, 307124, 
        305359, 309311, 310015
    };
    std::uniform_int_distribution<unsigned> uniformPdf(0,
            sizeof(periodRuns)/sizeof(*periodRuns) - 1);
    std::default_random_engine randomEngine;
    
    SG::AuxElement::ConstAccessor<int> truthType("truthType");
    SG::AuxElement::ConstAccessor<int> truthOrigin("truthOrigin");
    
    /* ********************************************************************** */
    
    Info(MSGSOURCE, "Starting the event loop");
    unsigned errors = 0;
    double nSuitableEvents = 0., sumW = 0.;
    for(Long64_t entry = 0; entry < entries; ++entry)
    {
        event.getEntry(entry);
        
        /// Get a random run number, and decorate the event info
        const xAOD::EventInfo* eventInfo = nullptr;
        event.retrieve(eventInfo,"EventInfo").ignore();
        unsigned runNumber = periodRuns[uniformPdf(randomEngine)];
        eventInfo->auxdecor<unsigned>("RandomRunNumber") = runNumber;

        unsigned nTrig_e26 = 0, nTrig_e7 = 0, nTrig_e17 = 0;
        
        vector<const xAOD::Electron*> myTriggeringElectrons;
        const xAOD::ElectronContainer* electrons = nullptr;
        event.retrieve(electrons,"Electrons").ignore();
        for(auto electron : *electrons)
        {
            if(!electron->caloCluster()) continue;
            float eta = fabs(electron->caloCluster()->etaBE(2));
            float pt = electron->pt();
            if(pt<10e3f || eta>=2.47) continue;
            if(!truthType.isAvailable(*electron)) continue;
            if(!truthOrigin.isAvailable(*electron)) continue;
            int t = truthType(*electron), o = truthOrigin(*electron);
            if(t!=2 || !(o==10 || (o>=12 && o<=22) || o==43)) continue;
            /// electron must be above softest trigger threshold (e7 here
            if(pt < 7e3f) continue;
            if(pt >= 18e3f) ++nTrig_e17;

            myTriggeringElectrons.push_back(electron);
        }

        vector<const xAOD::Muon*> myTriggeringMuons;
        const xAOD::MuonContainer* muons = nullptr;
        event.retrieve(muons,"Muons").ignore();
        for(auto muon : *muons)
        {
            if(runNumber >= 324320) break; // delete line once all SFs available for 2017
            float pt = muon->pt();
            if(pt<10e3f || fabs(muon->eta())>=2.5) continue;
            auto mt = muon->muonType();
            if(mt!=xAOD::Muon::Combined && mt!=xAOD::Muon::MuonStandAlone) continue;
            auto& mtp = *(muon->primaryTrackParticle());
            if(!truthType.isAvailable(mtp)) continue;
            if(!truthOrigin.isAvailable(mtp)) continue;
            int t = truthType(mtp), o = truthOrigin(mtp);
            if(t!=6 || !(o==10 || (o>=12 && o<=22) || o==43)) continue;
            /// muon must be above softest trigger threshold (mu24 here)
            if(pt < 25.2e3f) continue;

            myTriggeringMuons.push_back(muon);
        }

        /// Add 'MyMedium' & 'MyTight' decorations to random electrons
        /// also count tight electrons above e26_xxx threshold
        /// and medium electrons above e7_xxx threshold
        for(auto electron : myTriggeringElectrons)
        {
            bool medium = bernoulliPdf(randomEngine);
            dec_medium(*electron) = medium? 1 : 0;
            if(medium && electron->pt()>8e3f) ++nTrig_e7;
            bool tight = medium && bernoulliPdf(randomEngine);
            dec_tight(*electron) = tight? 1 : 0;
            if(tight && electron->pt()>27e3f) ++nTrig_e26;
        }
        
        /// Events must contain enough leptons to trigger
        if(nTrig_e26 < 1 /// single-electron trigger
            && (nTrig_e7==0 || myTriggeringMuons.size()==0) /// electron-muon
            && nTrig_e17 < 2) /// dielectron
        {
            continue;
        }


        /// Finally retrieve the global trigger scale factor
        double sf = 1.;
        auto cc = myTool->getEfficiencyScaleFactor(myTriggeringElectrons,
            myTriggeringMuons, sf);
        if(cc==CP::CorrectionCode::Ok)
        {
            nSuitableEvents += 1;
            sumW += sf;
        }
        else
        {
            Warning(MSGSOURCE, "Scale factor evaluation failed");
            ++errors;
        }
        if(errors>10)
        {
            Error(MSGSOURCE, "Too many errors reported!");
            break;
        }
    }
    Info(MSGSOURCE, "Average scale factor: %f (over %ld events)",
            sumW / nSuitableEvents, long(nSuitableEvents));
    #ifndef XAOD_STANDALONE
        app->finalize();
    #endif
    return errors? 4 : 0;
}

/// Split comma-delimited string
namespace
{
    inline vector<string> split_comma_delimited(const string& s)
    {
        std::stringstream ss(s);
        std::vector<std::string> tokens;
        std::string token;
        while(std::getline(ss, token, ','))
        {
            if(token.length()) tokens.push_back(token);
        }
        return tokens;
    }
}
