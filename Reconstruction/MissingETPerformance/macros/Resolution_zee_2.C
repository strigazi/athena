/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  #include "utils.h"
  // Open the root file
  TFile file("../METPerformance_zeeall_HadAc.root");

  //need to change this directory name to the one you want
//  file->cd( "Zee/Resolutions/Data_Res_Perp_HadActivity" );
  file->cd( "Zee/Resolutions/Data_Res_Par_HadActivity" );

/*
//truth-reco along perpendicular axis
  file->cd( "Zee/Resolutions/Res_Perp_HadRecoil" );
//truth-reco along parallel axis
  file->cd( "Zee/Resolutions/Res_Par_HadRecoil" );
//truth_EtMiss-reco_EtMiss
  file->cd( "Zee/Resolutions/Res_Pt_HadRecoil" );
//truth-reco along zpt direction
  file->cd( "Zee/Resolutions/Res_ZPt_HadRecoil" );

//...the same versus truth_EtMiss
  file->cd( "Zee/Resolutions/Res_Pt_Perp" );
  file->cd( "Zee/Resolutions/Res_Pt_Par" );
  file->cd( "Zee/Resolutions/Res_Pt" );
//
//MET (reco) resolved along the perpendicular
  file->cd( "Zee/Resolutions/Data_Res_Perp_HadRecoil" );
//MET (reco) resolved along the parallel
  file->cd( "Zee/Resolutions/Data_Res_Par_HadRecoil" );
//MET resolved along ZPt
  file->cd( "Zee/Resolutions/Data_Res_ZPt_HadRecoil" );

//...the same versus Hadronic Activity
  file->cd( "Zee/Resolutions/Data_Res_Perp_HadActivity" );
  file->cd( "Zee/Resolutions/Data_Res_Par_HadActivity" );
*/

//  file->cd( "Zee/Resolutions/Resolutions" );
  file.ls();

  gROOT->SetStyle("ATLAS");
  gROOT->ForceStyle();
  gStyle->SetOptStat(111111);
  gStyle->SetOptFit(0);

/*
  TCanvas* c22 = new TCanvas("c22"," Resolution",293,139,700,500);
  c22->Divide(1,1);
  c22.cd( 1 );
z_mass->Draw();
  c22.Print("zee_mass.eps");
*/

  int m_fitrange=2;
  float m_ns=2.;
  float  m_percerr=0.15;
  int m_entries=100;
  int m_numptt;
  int m_binpt;
  int m_startpt;
  float m_curvasres=15;
  m_numptt=100;
  m_binpt=3.5;
  m_startpt=0.;

  char name[m_numptt];
  double mean[m_numptt];
  double sigma[m_numptt];
  double mean_err[m_numptt];
  double sigma_err[m_numptt];
  double ex[m_numptt];
  double nxbin[m_numptt];
  float expsigmaf[m_numptt]=0.;
  int nbinplot=0;
  float xbin;

  int cp=sqrt((float)m_numptt)+1;
  TCanvas *c1 = new TCanvas("c1", "c1",293,139,700,500);
  c1.Update();
  c1.Divide(cp,cp);

  int ih=0;
  TF1 *FitFunc = new TF1("FitFunc", "gaus");

  for (int i= 0; i< m_numptt ; i++){
    c1->cd(i+1);
    sprintf(name,"ResolutionInBin_%d",i);
    TH1F* hist  =(TH1F*)gDirectory->Get(name);

    int entries = hist->GetEntries();
    sigma[ih]=-10005.;
    sigma_err[ih]=1000.;
    xbin = m_startpt+m_binpt/2.+ i * m_binpt;
    nxbin[ih]=xbin;
    ex[ih]=0.;

    if  ( entries> m_entries) {
      //fit on the whole plot range............................
      if (m_fitrange == 2) {
	hist->Fit( "gaus","","");
      }
      //fit in m_ns sigma (sigma=0.5*sqrt(SumET bin)..........
      if (m_fitrange == 3) {
	float expsigma=0.5*sqrt(xbin);
	hist->Fit( "gaus","","",-m_ns*expsigma,m_ns*expsigma );
      }
      TF1 *fitsf = hist->GetFunction("gaus");

      nbinplot++;
      mean[ih] = fitsf->GetParameter(1);
      sigma[ih] = fitsf->GetParameter(2);
      mean_err[ih] = fitsf->GetParError(1);
      sigma_err[ih] = fitsf->GetParError(2);
      expsigmaf[ih] =m_ns*expsigma;
  
      //choice of points.....................
      //choice of points.....................
      if(sigma[ih] > 0.) {
	double rappo= sigma_err[ih]/sigma[ih];
	if ( rappo > m_percerr ) {
	  sigma[ih]=-10005.;
	  sigma_err[ih]=1000.;
	}
      }
      ih++;
    }
  }
  c1.Print("Zee_many_plot_xy.ps");

  // Open the root file
  TFile file2("../METPerformance_zeeall_HadAc.root");
//  file2->cd( "Zee/Resolutions/Data_Res_Par_HadActivity" );
  file2->cd( "Zee/Resolutions/Data_Res_Perp_HadActivity" );
  file2.ls();

  char name2[m_numptt];
  double mean2[m_numptt];
  double sigma2[m_numptt];
  double mean_err2[m_numptt];
  double sigma_err2[m_numptt];
  double ex2[m_numptt];
  double nxbin2[m_numptt];
  float expsigmaf2[m_numptt]=0.;
  int nbinplot2=0;
  float xbin;

  int cp=sqrt((float)m_numptt)+1;
  TCanvas *c12 = new TCanvas("c12", "c12",293,139,700,500);
  c12.Update();
  c12.Divide(cp,cp);

  int ih=0;
  TF1 *FitFunc = new TF1("FitFunc", "gaus");

  for (int i= 0; i< m_numptt ; i++){
    c12->cd(i+1);
    sprintf(name,"ResolutionInBin_%d",i);
    TH1F* hist  =(TH1F*)gDirectory->Get(name);

    int entries2 = hist->GetEntries();
    sigma2[ih]=-10005.;
    sigma_err2[ih]=1000.;
    xbin = m_startpt+m_binpt/2.+ i * m_binpt;
    nxbin2[ih]=xbin;
    ex2[ih]=0.;

    if  ( entries2> m_entries) {
      //fit on the whole plot range............................
      if (m_fitrange == 2) {
	hist->Fit( "gaus","","");
      }
      //fit in m_ns sigma (sigma=0.5*sqrt(SumET bin)..........
      if (m_fitrange == 3) {
	float expsigma=0.5*sqrt(xbin);
	hist->Fit( "gaus","","",-m_ns*expsigma,m_ns*expsigma );
      }
      TF1 *fitsf = hist->GetFunction("gaus");

      nbinplot2++;
      mean2[ih] = fitsf->GetParameter(1);
      sigma2[ih] = fitsf->GetParameter(2);
      mean_err2[ih] = fitsf->GetParError(1);
      sigma_err2[ih] = fitsf->GetParError(2);
      expsigmaf2[ih] =m_ns*expsigma;
  
      //choice of points.....................
      if(sigma2[ih] > 0.) {
	double rappo= sigma_err2[ih]/sigma2[ih];
	if ( rappo > m_percerr ) {
	  sigma2[ih]=-10005.;
	  sigma_err2[ih]=1000.;
	}
      }
      ih++;
    }
  }
  c12.Print("Zee_many_plot_xy2.ps");

  TCanvas* c2 = new TCanvas("c2"," Resolution along axis vs Had Activity",293,139,700,500);
  c2.Update();
  c2->Divide(1,1);
  c2.cd( 1 );
  gStyle->SetLineColor( 2 );
float xxx=0.25;
float yyy=0.85;
float yyy2=0.75;

  TGraphErrors* gr = new TGraphErrors(nbinplot,nxbin,sigma,ex,sigma_err);
  gr->SetTitle();
  gr->SetMarkerColor(4);
  gr->SetMarkerStyle(21);
  gr->Draw("AP"); 
  gr->SetMaximum(m_curvasres);
  gr->SetMinimum(0);
  gr->GetYaxis()->SetTitle("Resolution(Data_Res_axis_HadActivity)");
  gr->GetXaxis()->SetTitle("HadActivity");
myText( 0.72,0.85,1,"Z#rightarrow ee");
myMarkerText(xxx,yyy,4,21,"Perp");

  TGraphErrors* gr = new TGraphErrors(m_numptt,nxbin2,sigma2,ex2,sigma_err2);
  gr->SetTitle();
  gr->SetMarkerColor(1);
  gr->SetMarkerStyle(27);
  gr->Draw("P"); 
  gr->SetMaximum(m_curvasres);
  gr->SetMinimum(0);
  gr->GetYaxis()->SetTitle("Resolution(Data_Res_axis_HadActivity)");
  gr->GetXaxis()->SetTitle("HadActivity");
//myText( 0.72,0.85,1,"Z#rightarrow ee");
myMarkerText(xxx,yyy2,1,27,"Par");

  c2.Print("Zee_Resol_Data_Res_ParPerp2_HadActivity.eps");


}//end Resol_xy_curve.C
