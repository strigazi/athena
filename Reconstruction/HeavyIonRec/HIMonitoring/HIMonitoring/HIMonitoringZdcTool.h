/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIMONITORINGZDCTOOL_H
#define HIMONITORINGZDCTOOL_H

#include <vector>
#include <string>

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TRandom3.h"

#include <xAODHIEvent/HIEventShape.h>
#include <xAODHIEvent/HIEventShapeContainer.h>

#include <xAODForward/ZdcModule.h>
#include <xAODForward/ZdcModuleAuxContainer.h>
#include <xAODForward/ZdcModuleContainer.h>
#include <xAODForward/xAODForwardDict.h>

// #include <xAODTrigEgamma/TrigElectronContainer.h>



class TH1;
class TGraph;
class TTree;
class TH1D;
class TH1D_LW;
class TH2D_LW;
class TProfile_LW;
const int Nsamp = 7;
const int Nmod = 4;
const int Nside = 2;

class HIMonitoringZdcTool : public ManagedMonitorToolBase
{
   public:

      HIMonitoringZdcTool( const std::string & type, const std::string & name,
            const IInterface* parent ); 

      virtual ~HIMonitoringZdcTool();

      virtual StatusCode bookHistogramsRecurrent();
      virtual StatusCode bookHistograms();
      virtual StatusCode fillHistograms();
      virtual StatusCode procHistograms();

      
	  void book_hist();


	 private:
		double AEM, AHAD1, AHAD2, AHAD3;
		double CEM, CHAD1, CHAD2, CHAD3;
		double G0AEM, G0AHAD1, G0AHAD2, G0AHAD3;
		double G0CEM, G0CHAD1, G0CHAD2, G0CHAD3;
		double G1AEM, G1AHAD1, G1AHAD2, G1AHAD3;
		double G1CEM, G1CHAD1, G1CHAD2, G1CHAD3;
		double SumSideA, SumSideC;


		/// histograms 

	TH1D_LW* hamp[Nmod][Nside];
		TH1D_LW* hampG0[Nmod][Nside];
		TH1D_LW* hampG1[Nmod][Nside];
		TH1D_LW* hSumSideAmp[Nside];
		TH1D_LW* hSumSideAmpG0[Nside];
		TH1D_LW* hSumSideAmpG1[Nside];
		TH2D_LW* hEM_HAD1[Nside];
		TH2D_LW* hHAD1_HAD2[Nside];
		TH2D_LW* hHAD2_HAD3[Nside];
		TH2D_LW* hSideAC;

	    
};

#endif
