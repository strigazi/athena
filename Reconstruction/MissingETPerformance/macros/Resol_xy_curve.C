/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
///////////////////////////////////////////////////////////////////////////////////
// histos for Etmiss Resol vs Sumet  
// take from the resolution plots in Sumet bins the
// fit parameters, plot and fit them to make the curve 
///////////////////////////////////////////////////////////////////////////////////
  #include "utils.h"
  // Open the root file
  TFile file("METPerformance.root");
  file->cd( "Resolution/Binned" );

  gROOT->SetStyle("ATLAS");
  gROOT->ForceStyle();
  gStyle->SetOptStat(111111);
  gStyle->SetOptFit(0);

  int m_fitrange=3;
  float m_ns=2.;
  float  m_percerr=0.15;
  int m_entries=100;
  int m_numptt;
  int m_binpt;
  int m_startpt;
  float m_curvasres=15;
  m_numptt=20;
  m_binpt=10;
  m_startpt=5;
  float m_curvasET=m_numptt*m_binpt;
  
  char name[m_numptt];
  m_curvasET=m_startpt+m_numptt*m_binpt;
  double mean[m_numptt];
  double sigma[m_numptt];
  double mean_err[m_numptt];
  double sigma_err[m_numptt];
  double ex[m_numptt];
  double nxbin[m_numptt];
  float expsigmaf[m_numptt]=0.;
  int nbinplot=0;
  float xbin;

  // book histo curva resol vs SumET 
  he_resvset = new TH2F("resvset","resvset",100,0.,m_curvasET,100,0.,m_curvasres);

  int cp=sqrt((float)m_numptt)+1;
  TCanvas *c1 = new TCanvas("c1", "c1",293,139,700,500);
  c1.Update();
  c1.Divide(cp,cp);

  int ih=0;
  for (int i= 0; i< m_numptt ; i++){
    c1->cd(i+1);
    sprintf(name,"h_%d",i);
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
  
      he_resvset->Fill(xbin, sigma[ih]);
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
  c1.Print("many_plot_xy.ps");

  TCanvas* c2 = new TCanvas("c2"," Resolution vs Sumet from Topo (A0300)",293,139,700,500);
  c2.Update();
  c2->Divide(1,1);
  c2.cd( 1 );
  gStyle->SetLineColor( 2 );

  TGraphErrors* gr = new TGraphErrors(nbinplot++,nxbin,sigma,ex,sigma_err);
  gr->SetTitle();
  gr->SetMarkerColor(4);
  gr->SetMarkerStyle(21);
  gr->Draw("AP"); 
  gr->SetMaximum(m_curvasres);
  gr->SetMinimum(0);
  gr->GetYaxis()->SetTitle("Resolution (GeV)");
  gr->GetXaxis()->SetTitle("#Sigma E_{T}  (GeV)");

  TF1 *f1 = new TF1("f1","[0]*sqrt(x)",0, m_curvasET);
  f1->SetLineColor(4);
  gr->Fit("f1","R");

  TLegend *leg = new TLegend(0.55, 0.75, 0.85, 0.95);
  char message[50];
  sprintf(message,"W#rightarrow e#nu: fit %3.2f #sqrt{Sumet} " , f1->GetParameter(0) );
  leg->AddEntry(gr,message,"pl");

  c2.Update();
  leg->Draw();

  c2.Print("curva_wenu_xy.eps");
}//end Resol_xy_curve.C
