// $Id$
#include <TFile.h>
#include <TError.h>
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"

// ASG include(s):
#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "TruthWeightTools/TruthWeightTool.h"
#include "TruthWeightTools/HiggsWeightTool.h"
#include <TRandom3.h>

int main( int argc, char* argv[] ) {

   // Get the application's name:
   const char* APP_NAME = argv[ 0 ];

   std::vector<TString> files;
   bool forceNNLOPS=false;
   for (int i=1;i<argc;++i) {
     TString arg(argv[i]);
     if (arg=="--forceNNLOPS") forceNNLOPS=true;
     else if (arg.Contains(".root")) files.push_back(arg);
     else std::runtime_error(TString("Cannot intepret argument: "+arg).Data());
   }

   if (files.size()==0) {
     ::Error( APP_NAME, "Usage: %s <xAOD file1> [xAOD file2]...", APP_NAME );
     return 1;
   }

   // Initialise the application:
   RETURN_CHECK( APP_NAME, xAOD::Init( APP_NAME ) );
   
   // Create a TEvent object:
   xAOD::TEvent event( xAOD::TEvent::kBranchAccess );

   xAOD::HiggsWeightTool *higgsMCtool = new xAOD::HiggsWeightTool( "HiggsWeightTool" );
   higgsMCtool->setProperty( "OutputLevel", MSG::DEBUG ).ignore();
   if (forceNNLOPS) higgsMCtool->setProperty("ForceNNLOPS",true);
   higgsMCtool->initialize().ignore();

   // Open the input file:
   TString fn = files.front(); 
   std::unique_ptr< ::TFile > ifile( ::TFile::Open( fn, "READ" ) );
   if( ! ifile.get() ) {
     ::Error( APP_NAME, "Couldn't open file: %s", fn.Data() );
     return 1;
   }
   ::Info( APP_NAME, "Opened file: %s", fn.Data() );
     
   // Connect the TEvent object to it:
   RETURN_CHECK( APP_NAME, event.readFrom( ifile.get() ) );
     
   // Loop over a few events:
   const ::Long64_t entries = event.getEntries();
     
   for ( ::Long64_t entry = 0; entry < entries; ++entry ) {
       
     // Get the current entry:
     event.getEntry( entry );
     const xAOD::EventInfo *evtInfo;
     RETURN_CHECK( APP_NAME, event.retrieve( evtInfo, "EventInfo" ) );
     std::vector<float> weights = evtInfo->mcEventWeights();
     
     const xAOD::TruthEventContainer *tevts;
     RETURN_CHECK( APP_NAME, event.retrieve( tevts, "TruthEvents" ) );
     
     // in reality we should pass the Higgs pT and Njets30 from HTXS
     // but standard input files don't have these, so let's randomly sample them..
     int HTXS_Njets30 = gRandom->Poisson(0.9);
     double HTXS_pTH = std::abs(gRandom->Gaus(0.0,40.0));

     if ( entry == 0 ) {
       ::Info(APP_NAME,"There are %lu weights in EventInfo and %lu in TruthEvents",
	      weights.size(),tevts->at(0)->weights().size());
       std::vector<float> ws = tevts->at(0)->weights();
       for (size_t i=0;i<10;++i)
	 ::Info(APP_NAME,"Weight %lu %.3f and %.3f. %lu weights and %lu names",
		i,weights[i],ws[i],ws.size(),higgsMCtool->getWeightNames().size());
       higgsMCtool->getHiggsWeights(HTXS_Njets30,HTXS_pTH).print();
     }

     // Give some feedback of where we are:
     if( (entry+1) % 10000 == 0 ) {
       ::Info( APP_NAME, "Processed %5llu/%5llu events",entry+1,entries);
       
       higgsMCtool->getHiggsWeights(HTXS_Njets30,HTXS_pTH).print();
     }
   }

   // Return gracefully:
   return 0;
}

