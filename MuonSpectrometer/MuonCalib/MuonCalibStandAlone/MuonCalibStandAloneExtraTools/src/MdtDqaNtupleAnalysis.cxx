/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibStandAloneExtraTools/MdtDqaNtupleAnalysis.h"

#include "MuonCalibStandAloneExtraTools/PhiEtaUtils.h"
#include "MuonCalibStandAloneExtraTools/MDTName.h"

#include "MuonCalibStandAloneExtraTools/HistogramManager.h"
#include "MuonCalibStandAloneExtraTools/MDTDeadElementsAnalysis.h"
#include "MuonCalibStandAloneExtraTools/MDTDqaDeadElements.h"
#include "MuonCalibStandAloneExtraTools/TimeAndAdcFitter.h"

//CalibNtupleAnalysis container classes
#include "CalibNtupleUtils/TimedSegment.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibEventBase/MdtCalibHitBase.h"
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibTruthCollection.h"
#include "MuonCalibEventBase/MuonCalibTruth.h"
#include "MuonCalibEventBase/MuonCalibMdtTruthHit.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"

//MuonCalib container classes
#include "MuonCalibEventBase/MuonCalibPattern.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"

#include "TKey.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"

namespace MuonCalib {

//====================================================================================
MdtDqaNtupleAnalysis::MdtDqaNtupleAnalysis(bool verbose, std::string outputFileName) :
  m_histoManager(NULL), m_DeadElementsAlgorithm(-1), m_SectorMin(-1), m_SectorMax(-1),
  m_analyseBarrel(false), m_analyseEndcapA(false), m_analyseEndcapC(false), m_ADCCUT(-1.) {
  m_verbose = verbose ;
  m_outputFileName = outputFileName ;
  p_reg_sel_svc = NULL ;
}

//====================================================================================
StatusCode MdtDqaNtupleAnalysis::initialize(RegionSelectionSvc *reg_sel_svc, HistogramManager *histoManager, int DeadElementsAlgorithm, float ADCCUT) {
  p_reg_sel_svc = reg_sel_svc;
  m_histoManager = histoManager;
  m_DeadElementsAlgorithm = DeadElementsAlgorithm;
  m_ADCCUT=ADCCUT;
  
  PhiEtaNameConverter phiEtaConverter;
  std::string testName="BIL1A01";
  MDTName NameConverter(testName);
  m_SectorMin = 20;
  m_SectorMax = 0;
  const std::vector<MuonCalib::NtupleStationId> stationsInRegion = p_reg_sel_svc->GetStationsInRegions();
  std::vector<MuonCalib::NtupleStationId>::const_iterator itstation;
  for (itstation = stationsInRegion.begin(); itstation!=stationsInRegion.end(); itstation++) {
    int stationNameId = itstation->GetStation();
    int phi = itstation->GetPhi();
    int sector = phiEtaConverter.phi_8to16(stationNameId,phi);
    if(sector>m_SectorMax) m_SectorMax = sector;
    if(sector<m_SectorMin) m_SectorMin = sector;
  }

  m_evtCounter=0;  

  return StatusCode::SUCCESS;
}
  
//====================================================================================
void MdtDqaNtupleAnalysis::handleEvent( const MuonCalibEvent& event, int /*eventnumber*/, 
const std::vector<MuonCalibSegment *> &/*segments*/,unsigned int /*position*/){    
  
 std::string histoType;

 TFile *mdtDqaRoot =  m_histoManager->rootFile();

 float ADCCUT=m_ADCCUT;

 if( !p_reg_sel_svc){
   return;
 }

 ToString ts;
 PhiEtaNameConverter phiEtaConverter;

 //Event counting
 m_evtCounter++; 
 
 // Some checks. ..... -----------------------
 int minNumberHits = 0;
 int maxNumberHits = 1000000;
 int minEventCounter = 0;
 int maxEventCounter = 10000000;
 
 const MuonCalibRawHitCollection* rawColl = event.rawHitCollection();

 // SELECT RANGE IN EVENT COUNTER !!!!!!
 if( m_evtCounter<minEventCounter || m_evtCounter>maxEventCounter ) return;

 if( rawColl->numberOfMuonCalibRawMdtHits() + event.numberOfHits() > maxNumberHits) return;
 if( rawColl->numberOfMuonCalibRawMdtHits() + event.numberOfHits() < minNumberHits) return;
 //------------------------------------------------------------------------------
    
 MuonCalibRawHitCollection::MuonCalibRawMdtHitVecCit rmdt_hit_it = rawColl->rawMdtHitCollectionBegin();
 MuonCalibRawHitCollection::MuonCalibRawMdtHitVecCit rmdt_hit_it_end = rawColl->rawMdtHitCollectionEnd();

 //===== RAW HITS LOOP ===========================================================

 const int nStations=36;
 const int nEta=8;
 const int nSectors=16;
 const int nSides=2;
 int chamberMulti[nSides][nSectors][nStations][nEta];
 int chamberMultiAdcCut[nSides][nSectors][nStations][nEta];

 for(int iside=0;iside<nSides;iside++){
   for(int isec=0;isec<nSectors;isec++){
     for(int istation=0;istation<nStations;istation++){
       for(int ieta=0;ieta<nEta;ieta++){
	 chamberMulti[iside][isec][istation][ieta]=0;
	 chamberMultiAdcCut[iside][isec][istation][ieta]=0;
       }}}}

 // === RAW HITS LOOP START  ======================================================= 
 for(; rmdt_hit_it!=rmdt_hit_it_end; ++rmdt_hit_it){

   if (p_reg_sel_svc->isInRegion((*rmdt_hit_it)->identify())) {  

     MuonFixedId id = (*rmdt_hit_it)->identify();

     MDTName chamb(id);

     int rawCharge = (*rmdt_hit_it)->adcCount();
     int rawTime = (*rmdt_hit_it)->tdcCount();

     int rstn=id.stationName();
     int rphi2=chamb.getOnlineSector();

     double localPos_y = (*rmdt_hit_it)->localPosition().y();
     double localPos_z = (*rmdt_hit_it)->localPosition().z();

     bool doSector=false;
     if(rphi2>= m_SectorMin && rphi2<=m_SectorMax) doSector=true;
     if(!doSector) continue;

     std::string region = chamb.getRegion();
     // THE FOLLOWING ASSIGNMENT TO Endcap Chambers can change if, 
     // for example, BEE or BIS7,8 chambers have to be assigned to Endcap

     std::string stn = id.stationNumberToFixedStationString(rstn);
     std::string side=chamb.getSide();
     int rside = 1;
     if(chamb.isBackward()) rside=-1; 
     
     std::string phisec;
     if(rphi2<10) phisec = "0"+ts(rphi2);
     if(rphi2>=10) phisec = ts(rphi2);

     int absEta=chamb.getOnlineEta();
     //format BIL5A03
     std::string chamberName = chamb.getOnlineName();

     //-- Multiplicity
     int iside = 1;
     if(chamb.isBackward()) iside=0; 
     chamberMulti[iside][rphi2-1][rstn-1][absEta-1]++;  //Controllare!!!!
     if(rawCharge > ADCCUT) chamberMultiAdcCut[iside][rphi2-1][rstn-1][absEta-1]++;
     
     //---------------------- 
     int tubestot;
     int tubesPerLayer;
     int imezz = 0;
     int tubeOffset[2];
     tubeOffset[0]=m_histoManager->GetTubeOffsetML1(chamberName) ;
     tubeOffset[1]=0;

     int itubeLong=0;

     int nLayersPerML=3;
     if(stn.substr(0,2)=="BI" || stn.substr(0,2)=="EI" || stn.substr(0,3)=="BEE") { // CHECK THIS FURTHER !!!!!!!!!
       nLayersPerML=4;
     }
     
     int nLayers=nLayersPerML*2;

     if(stn.substr(0,3)=="BIS" && absEta==8) {
       nLayersPerML=3;
       nLayers=nLayersPerML;
     }

     if(stn.substr(0,3)=="BEE") nLayers=nLayersPerML;
      
     // hit tube :
     int tube  = id.mdtTube();
     int layer = id.mdtTubeLayer();
     int Mlayer= id.mdtMultilayer();
     if (Mlayer==1) tube = tube+tubeOffset[0]; // this conver OFFLINE to ONLINE tube numbering
     if (Mlayer==2) tube = tube+tubeOffset[1]; // this conver OFFLINE to ONLINE tube numbering
     
     TH1F *h;
     TH2F *h2;
  
     // Chamber Hit occupancy 
     histoType = "a_ChamberHitOccupancy";
     h2 = (TH2F*) m_histoManager->GetMdtHisto(histoType,chamb);
     if (h2 && rawCharge > ADCCUT) {
       float layMl = layer + (Mlayer-1)*6;
       h2->Fill( (float)tube, layMl );
     }

     std::string chamberDirName = m_histoManager->GetMdtDirectoryName(chamb);
     std::string expertDirName = chamberDirName+"/Expert";
     TDirectory *expertRootDir = mdtDqaRoot->GetDirectory(expertDirName.c_str());

     expertRootDir->cd();

     // moved to Expert folder by Dani 01.03.2010
     histoType = "ChamberYZOccupancy";
     h2 = (TH2F*) expertRootDir->FindObjectAny(histoType.c_str());
     if (h2 && rawCharge > ADCCUT) {
       h2->Fill( (float)localPos_y, (float)localPos_z );
     }

     // Hits per tube
     histoType = "a_HitsPerTube";
     h = (TH1F*) m_histoManager->GetMdtHisto(histoType,chamb);
     if (h) {
       tubestot=h->GetNbinsX();
       tubesPerLayer=tubestot/nLayers;
       imezz=(id.mdtMezzanine())%100;       
       itubeLong=tube+tubesPerLayer*(layer-1)+tubesPerLayer*nLayersPerML*(Mlayer-1);
       h->Fill( itubeLong*1. );
     }
     histoType = "a_HitsPerTubeAdcCut";
     h = (TH1F*) m_histoManager->GetMdtHisto(histoType,chamb);
     if( (*rmdt_hit_it)->adcCount() > ADCCUT){
       if (h) h->Fill( itubeLong*1. );
     }
     
     //Hits per tube ML 1,2
     histoType = "HitsPerTube_ML"+ts(Mlayer)+"_L"+ts(layer);
     h = (TH1F*) m_histoManager->GetMdtHisto(histoType,chamb);
     if(rawCharge > ADCCUT){
       if(h) h->Fill( tube*1.);
     }
     
     //tdc ML 1,2
     histoType = "A_TDC_ML"+ts(Mlayer);
     h = (TH1F*) m_histoManager->GetMdtHisto(histoType,chamb);
     if(h && rawCharge > ADCCUT) h->Fill( rawTime );
     
     //adc ML 1,2
     histoType = "C_ADC_ML"+ts(Mlayer);
     h = (TH1F*) m_histoManager->GetMdtHisto(histoType,chamb);
     if(h) h->Fill( rawCharge );
     
     //tdc vs adc mezz 
     histoType = "tdc_Vs_adc_mezz_"+ts(imezz);
     if(imezz<20) {
       h2 = (TH2F*) m_histoManager->GetMdtHisto(histoType,chamb);
       if(h2) h2->Fill( rawCharge, rawTime );
     }

     // OVERVIEW
      // region Overview plot
     std::string stationLayer = "UNDEFINED";
     if(chamb.isInner()) stationLayer = "Inner";
     if(chamb.isMiddle()) stationLayer = "Middle";
     if(chamb.isOuter()) stationLayer = "Outer";
     if(chamb.isExtra()) stationLayer = "extra";  //for now, extra station stored in middle station histogram

     std::string title2="";
     if(region=="Barrel") title2+="B";
     else title2+="E";
     
     title2+=side;
     title2+="_";

     histoType = "A_HitsPerML_"+stationLayer;
     h2 = (TH2F*) m_histoManager->GetMdtHisto(histoType,region,side);
     float xbin = absEta;
     if (stn == "BIM" ) xbin=absEta+6;
     if (stn == "EEL") xbin=absEta+2;
     if (stn == "EES") xbin=absEta+2;
     if(h2 && rawCharge > ADCCUT) h2->Fill( xbin , rphi2 + 0.25*(2*Mlayer-3) );

     // GLOBAL plot --- All Chambers together
     if(region=="Barrel")  histoType = "Global_HitsPerML_Barrel";
     if(region=="Endcap")  histoType = "Global_HitsPerML_Endcap";
     h2 = (TH2F*) m_histoManager->GetHisto("GLOBAL",histoType);
     float shiftGlobalEta(0);
     if(stationLayer=="Inner") shiftGlobalEta = 0;
     if(stationLayer=="Middle") shiftGlobalEta = 14;
     if(stationLayer=="Outer") shiftGlobalEta = 24;
     if(stationLayer=="extra") shiftGlobalEta = 34;
     float xbinGlobal = (xbin+shiftGlobalEta)*rside;
     if(h2 && rawCharge >  ADCCUT) h2->Fill( xbinGlobal , rphi2 + 0.25*(2*Mlayer-3) );

     // GLOBAL plot --- Chambers divided in Inner,Middle,Outer stations (extra goes with Middle)
     histoType = "Global_HitsPerML_"+stationLayer;
     if(stationLayer=="extra") histoType = "Global_HitsPerML_Middle";
     h2 = (TH2F*) m_histoManager->GetHisto("GLOBAL",histoType);
     xbinGlobal = xbin*rside;
     if (stn.substr(0,1)=="E") xbinGlobal = rside*(absEta+14);
     if (chamb.getName()=="BEE") xbinGlobal= rside*(absEta+8);
     if (chamb.getName()=="EEL") xbinGlobal= rside*(absEta+11);
     if (chamb.getName()=="EES") xbinGlobal= rside*(absEta+11);
     
     if(h2 && rawCharge >  ADCCUT) h2->Fill( xbinGlobal , rphi2 + 0.25*(2*Mlayer-3) );
     
     histoType = "HitsPerML_"+stationLayer;
     h = (TH1F*) m_histoManager->GetMdtHisto(histoType,region,side,rphi2);
     if(h && rawCharge >  ADCCUT) h->Fill( absEta + 0.1*(2*Mlayer-3) );

     histoType = "TDC_AllChambers_"+stationLayer;
     h = (TH1F*) m_histoManager->GetMdtHisto(histoType,region,side,rphi2);
     if(h && rawCharge > ADCCUT) h->Fill( rawTime );
     
     histoType = "z_HitsVsMezzanine";
     h2 = (TH2F*) m_histoManager->GetMdtHisto(histoType,region,side,rphi2);
     if( h2 && rawCharge > ADCCUT) h2->Fill(chamberName.c_str(),imezz,1);
     
   } // p_reg -> hit IsInRegion    
 }//for 

 //---- Fill Multiplicity ---------
 TH1F *hmulti;

 std::string side="C";
 std::string region = "Barrel";

 for (int nregions=0;nregions<2;nregions++) {
   if (nregions==1) region = "Endcap";
   for (int iside=0;iside<nSides;iside++){
     if (iside==1) side="A";
     for (int isec=0;isec<nSectors;isec++){
       float sectorMulti = 0;
       float sectorMultiAdcCut = 0; 
       for (int istation=0;istation<nStations;istation++){
	 // THE FOLLOWING ASSIGNMENT TO Endcap Chambers can change if, 
	 // for example, BEE or BIS7,8 chambers have to be assigned to Endcap
	 if ( nregions==0 && istation+1 > 13 ) continue; // the loop was on the barrel
	 if ( nregions==1 && istation+1 <= 13 ) continue; // the loop was on the endcap
	 
	 for (int ieta=0;ieta<nEta;ieta++){
	   sectorMulti += chamberMulti[iside][isec][istation][ieta];
	   sectorMultiAdcCut += chamberMultiAdcCut[iside][isec][istation][ieta];
	   
	   double multi =  chamberMulti[iside][isec][istation][ieta];
	   double multiAdcCut = chamberMultiAdcCut[iside][isec][istation][ieta];
	   
	   if (multi !=0 ){
	     MuonFixedId id;
	     std::string stn=id.stationNumberToFixedStationString(istation+1);
	     
	     MDTName chamb_mult(stn,ieta+1,side,isec+1);
	     
	     histoType="B_ChamberHitMultiplicity";
	     //	     if(m_verbose) cout<<" histoType "<<histoType<<endl;
	     hmulti = (TH1F*) m_histoManager->GetMdtHisto(histoType,chamb_mult);
	     if(hmulti) hmulti->Fill(multiAdcCut);
	     
	   }// if multi!=0
	 }// loop on eta
       }//loop on  chamber
     }//sector
   }//side
 }//regions

 
}  //end MdtDqaNtupleAnalysis::handleEvent
  
void MdtDqaNtupleAnalysis::histogramAnalysis(TFile *f) {    

  bool WriteMdtDqaDbToTextFile = false; 
  
  ToString ts;

  if(f->IsZombie()) return;

  bool doDeadElementsAnalysis = true;
  bool doTubeEfficiency = true;
  bool doTDCADCfitting = true;
  bool doResidualsFit = true;
 
  if (doDeadElementsAnalysis) {
    if(m_DeadElementsAlgorithm==0) {
      MDTDeadElementsAnalysis *mdtTubeAna = new MDTDeadElementsAnalysis();
      mdtTubeAna->setNoVerbose();
      mdtTubeAna->WriteAsciFile();
      mdtTubeAna->histogramScanCalibCenters(f);
      delete mdtTubeAna; mdtTubeAna=0;
    } else {
      MDTDqaDeadElements *mdtTubeAna = new MDTDqaDeadElements();
      mdtTubeAna->setVerbose(false);
      mdtTubeAna->setWriteReport(false); // this is for debug
      mdtTubeAna->setWriteCompactReport(true);
      mdtTubeAna->setWriteListOfDeadTubes(false);
      mdtTubeAna->setDoNoisy(false);
      mdtTubeAna->MDTDqaDeadElementsAnalysis(f);
      delete mdtTubeAna; mdtTubeAna=0;
    }
  }
  
  if (doTubeEfficiency) {
  // LOOP over chambers and apply algorithms per chamber.
    std::string region;
    std::string sector;
    std::string chamber;
    std::string regionDirName;
    std::string sectorDirName;
    std::string chamberDirName;
    std::string deadStatusDirName;
    std::string expertDirName;
    std::string effiDirName;
    TDirectory *RegionDir;
    TDirectory *SectorDir;
    TDirectory *ChamberDir;
    TDirectory *DeadStatusDir;
    TDirectory *ExpertDir;
    TDirectory *EffiDir;

    for (int iregion=1; iregion<=4; iregion++) {
      if (iregion==1) region="Barrel_A";
      if (iregion==2) region="Barrel_C";
      if (iregion==3) region="Endcap_A";
      if (iregion==4) region="Endcap_C";

      regionDirName = "/MDT/"+region;
      RegionDir = (TDirectory*)f->Get(regionDirName.c_str());
      if (!RegionDir) continue;
      
      // reset output histograms 
      TH1F *hgI = (TH1F*) RegionDir->Get("TubeEfficiency_Inner");
      TH1F *hgM = (TH1F*) RegionDir->Get("TubeEfficiency_Middle");
      TH1F *hgO = (TH1F*) RegionDir->Get("TubeEfficiency_Outer");
      
      TH1F *hgE(NULL);
      if(iregion>2) hgE =  (TH1F*) RegionDir->Get("TubeEfficiency_extra");
      if(hgI) hgI->Reset();
      if(hgM) hgM->Reset();
      if(hgO) hgO->Reset();
      if(iregion>2 && hgE) hgE->Reset();
      
      for (int isector=1; isector<=16; isector++) {
	
	if (isector<10) sector="0"+ts(isector);
	if (isector>=10) sector=ts(isector);
	sectorDirName = "/MDT/"+region+"/Sector"+sector;

	SectorDir = (TDirectory*)f->Get(sectorDirName.c_str());
	if (!SectorDir) continue;
	TIter next(SectorDir->GetListOfKeys());
	TKey *key;
	while ( (key = (TKey*)next()) ) {
	chamber=key->GetName();
	if (chamber=="OVERVIEW") continue;

	MDTName chamb(chamber);

	chamberDirName = sectorDirName+"/"+chamber;
	effiDirName = chamberDirName+"/Efficiency";
	deadStatusDirName = chamberDirName+"/DeadStatus";
	expertDirName = chamberDirName+"/Expert";
	
	ChamberDir = (TDirectory*)f->Get(chamberDirName.c_str());
	EffiDir = (TDirectory*)f->Get(effiDirName.c_str());
	DeadStatusDir = (TDirectory*)f->Get(deadStatusDirName.c_str());
	ExpertDir = (TDirectory*)f->Get(expertDirName.c_str());
	
	if (!ChamberDir) continue;
	if (!EffiDir) { 
	  continue;
	}
	if (!DeadStatusDir) { 
	  continue;
	}
	if (!ExpertDir) { 
	  continue;
	}

	//
	// APPLY ALGORITHMS TO chamber histograms HERE 
	//
	std::string histoName;
	
	TH1F *heffiEntries;
	TH1F *heffiCounts;
	TH1F *heffi;
	TH1F *heffiPerLy;
	TH1F *hg;

	ExpertDir->cd();
	 
	histoName = "EfficiencyEntries";
	heffiEntries = (TH1F*) ExpertDir->Get(histoName.c_str());
	histoName = "EfficiencyCounts";
	heffiCounts = (TH1F*) ExpertDir->Get(histoName.c_str()); 

	ChamberDir->cd();
	histoName = "b_EfficiencyPerTube";
	heffi = (TH1F*) ChamberDir->Get(histoName.c_str());

	if (!heffiEntries || !heffiCounts || !heffi) { 
	  continue;
	}

	histoName = "UNDEFINED";
	if (chamb.isInner()) histoName = "TubeEfficiency_Inner";
	if (chamb.isMiddle()) histoName = "TubeEfficiency_Middle";
	if (chamb.isOuter()) histoName = "TubeEfficiency_Outer";
	if (chamb.isExtra()) histoName = "TubeEfficiency_extra"; 
	hg = (TH1F*) RegionDir->Get(histoName.c_str());
	if (!hg) {
	  continue;
	}

        // reset output histograms 
        heffi->Reset();

        // Now we use this strange strategy to obtain chamber layout parameters:
        EffiDir->cd();
        int numberOfML=0;
        int numberOfLayers=0;
        int numberOfTubesPerLy[2];
        numberOfTubesPerLy[0]=0;
        numberOfTubesPerLy[1]=0;

        int tubeOffset[2];
        tubeOffset[0]=m_histoManager->GetTubeOffsetML1(chamber) ;
        tubeOffset[1]=0;

        int tubeOffset_atend[2];
        tubeOffset_atend[0]=m_histoManager->GetTubeOffsetAtEndML1(chamber) ;
        tubeOffset_atend[1]=0;

        for (int ML=1; ML<=2; ML++) {
	  for (int Ly=1; Ly<=4; Ly++) {
	    histoName = "EffiPerTube_ML"+ts(ML)+"_L"+ts(Ly);
	    heffiPerLy= (TH1F*) EffiDir->FindObjectAny(histoName.c_str());
	    if (heffiPerLy) {
	      numberOfML = ML;
	      numberOfLayers = Ly;
	      if (ML==1 && !numberOfTubesPerLy[0] ) numberOfTubesPerLy[0] = heffiPerLy->GetNbinsX();
	      if (ML==2 && !numberOfTubesPerLy[1] ) numberOfTubesPerLy[1] = heffiPerLy->GetNbinsX();
	    }
	  }
        }
        
	ChamberDir->cd();

        // HERE NOW COMPUTE EFFICIENCIES, ERRORS AND FILL THE HISTOGRAMS
        if (heffiEntries->GetEntries() != 0 ) {
        //loop over multilayers
	  for (int k=0; k<numberOfML; k++) {
        //loop over layers
	    for (int l=0; l<numberOfLayers; l++) {
	  //loop over tubes
	      for (int m=0; m<numberOfTubesPerLy[k]; m++) {
		int iML = k+1;
		int iLy = l+1;
		int iTube = m+1;
	    
		// THE FOLLOWING HAS TO BE FIXED AFTER THE EfficiencyPerTube HISTOGRAM HAS BEEN FIXED!
		// FOR NOW WE CONSIDER OFFSET AT 0 ....WHICH IS WRONG !
		iTube = iTube + tubeOffset[iML-1];

		int ibin = (iML-1)*numberOfLayers*numberOfTubesPerLy[k]+(iLy-1)*(numberOfTubesPerLy[k]+tubeOffset[iML-1]+tubeOffset_atend[iML-1])+iTube;
		//calculate efficiency and errors

		// HERE WE USE THE Efficiency definition and Error using the Bayesian Statistics: 
		float entries    = heffiEntries->GetBinContent(ibin);
		float counts     = heffiCounts->GetBinContent(ibin);
		float efficiency = (counts+1.)/(entries+2.);
		float error      = std::sqrt(efficiency*(1-efficiency))/std::sqrt(entries+3.);
		
		// Fill MdtDqa Histos
		heffi->SetBinContent(ibin,efficiency);
		heffi->SetBinError(ibin,error);
		// Filling Global plots
		if (hg && efficiency>0. && error >0. && entries>40) {
		  hg->Fill(efficiency);
		}
	      } // loop over tube
	    } // loop over Layer 
	  } // loop over ML
        }

        // Now we should fill the histograms histoName = "EffiPerTube_ML"+ts(iML)+"_L"+ts(iLy);
        EffiDir->cd();
        //loop over multilayers
        for (int ML=1; ML<=numberOfML; ML++) {
          //loop over layers
          for (int Ly=1; Ly<=numberOfLayers; Ly++) {
	    histoName = "EffiPerTube_ML"+ts(ML)+"_L"+ts(Ly);
	    heffiPerLy= (TH1F*) EffiDir->FindObjectAny(histoName.c_str());
	    if (!heffiPerLy) continue;
	    // reset output histograms 
	    heffiPerLy->Reset();
	    //loop over tubes
	    for (int TubeBin=1; TubeBin<=numberOfTubesPerLy[ML-1]; TubeBin++) {

	      if (heffiPerLy) {
		// THE FOLLOWING HAS TO BE FIXED AFTER THE EfficiencyPerTube HISTOGRAM HAS BEEN FIXED!
		// FOR NOW WE CONSIDER OFFSET AT 0 ....WHICH IS WRONG !
		int Tube = TubeBin + tubeOffset[ML-1];
		
		// AND ATTENTION WHEN THE EfficiencyPerTube WILL BE OK YOU MUST UNCOMMENT THE FOLLOWING LINE !!!!
		int ibin = (ML-1)*numberOfLayers*numberOfTubesPerLy[ML-1]+(Ly-1)*(numberOfTubesPerLy[ML-1]+tubeOffset[ML-1]+tubeOffset_atend[ML-1])+Tube;

		// get from heffi(ibin) and put in heffiPerLy(TubeBin)
		heffiPerLy->SetBinContent(TubeBin, heffi->GetBinContent(ibin));
		heffiPerLy->SetBinError(TubeBin, heffi->GetBinError(ibin));
	      }
	    } // loop over tube
          } // loop over Layer 
        } // loop over ML

	} // end of chambers loop
      } //end of sectors loop
    } //end of regions loop
  
  }   //end if doTubeEfficiency

  //
  if (doTDCADCfitting ) {
    TimeAndAdcFitter *myFitter = new TimeAndAdcFitter();
    float tdc2ns = 0.78125;

    double minEntries = 1000.;
    double entries(0);

    std::string histoName;
    TH1F *hh;
    TH1F *htpar;
    TH1F *hapar;
    const int nParams = 8;
    double pdefault[nParams];
    pdefault[0]= 0.;
    pdefault[1]= 100.;
    pdefault[2]= 5.;
    pdefault[3]= 100.;
    pdefault[4]= 600.;
    pdefault[5]= 1300.;
    pdefault[6]= 10.;
    pdefault[7]= 10.;
    
    const int nParamsAdc = 4;
    double AdcpfitDefault[nParamsAdc];
    AdcpfitDefault[0]= 0.;
    AdcpfitDefault[1]= 0.;
    AdcpfitDefault[2]= 0.;
    AdcpfitDefault[3]= 0.;
    
    std::string region;
    std::string sector;
    std::string chamber;
    std::string chamberType;
    std::string regionDirName;
    std::string sectorDirName;
    std::string chamberDirName;
    TDirectory *RegionDir;
    TDirectory *SectorDir;
    TDirectory *ChamberDir;

    for (int iregion=1; iregion<=4; iregion++) {
      if (iregion==1) region="Barrel_A";
      if (iregion==2) region="Barrel_C";
      if (iregion==3) region="Endcap_A";
      if (iregion==4) region="Endcap_C";
      
      std::string Region="Barrel";
      std::string Side="A";
      if (iregion==3||iregion==4 ) Region="Endcap";
      if (iregion==2||iregion==4 ) Side="C";
      
      regionDirName = "/MDT/"+region;
      RegionDir = (TDirectory*)f->Get(regionDirName.c_str());
      if (!RegionDir) continue;

      for (int isector=1; isector<=16; isector++) {
	if (isector<10) sector="0"+ts(isector);
	if (isector>=10) sector=ts(isector);
	sectorDirName = "/MDT/"+region+"/Sector"+sector;
	
	SectorDir = (TDirectory*)f->Get(sectorDirName.c_str());
	if (!SectorDir) continue;

      //
      // APPLY ALGORITHMS TO SECTOR histograms HERE 
      //
	int KMAX=3;
	if(iregion>2) KMAX=4;
	for (int k=1;k<=KMAX;k++) {
	  std::string stationLayer;
	  if (k==1)  stationLayer = "Inner";
	  if (k==2)  stationLayer = "Middle";
	  if (k==3)  stationLayer = "Outer";
	  if (k==4)  stationLayer = "extra";
	  histoName = "TDC_AllChambers_"+stationLayer;
	  hh = (TH1F*) m_histoManager->GetMdtHisto(histoName,Region,Side,isector);

	  if (!hh) {
	    continue;
	  }
        
	  entries = hh->GetEntries(); 
	  if ( entries >= minEntries ) {
            double *pfit = new double[nParams];
            double *errfit = new double[nParams];
            double chi2 = 0.;
            int ndof = 0;
            for ( int ii=0; ii<nParams; ii++ ) errfit[ii]=0.;
            for ( int ii=0; ii<nParams; ii++ ) pfit[ii]=pdefault[ii];

            myFitter->doTimeFit(hh,nParams,&pfit[0],&errfit[0],&chi2,&ndof );
	    
	  }
	}
      
      //
      // Now go inside the SectorDir to loop over chambers
      //
	TIter next(SectorDir->GetListOfKeys());
	TKey *key;
	while ( (key = (TKey*)next()) ) {
	  chamber=key->GetName();
	  if (chamber=="OVERVIEW") continue;
	  chamberDirName = sectorDirName+"/"+chamber;
	  ChamberDir = (TDirectory*)f->Get(chamberDirName.c_str());
	  if (!ChamberDir) continue;
	  ChamberDir->cd();
	  chamberType = chamber.substr(0,3);
	  MDTName chamb(chamber);
	  std::string side = chamb.getSide();
	  std::string stationLayer = "UNDEFINED";
	  if(chamb.isInner()) stationLayer = "Inner";
	  if(chamb.isMiddle()) stationLayer = "Middle";
	  if(chamb.isOuter()) stationLayer = "Outer";
	  if(chamb.isExtra()) stationLayer = "extra";
	  std::string hnamet0Sect=regionDirName+"/t0PerSector_"+stationLayer;
	  std::string hnametdriftSect=regionDirName+"/tdriftPerSector_"+stationLayer;
	  TH2F *ht0Sect = (TH2F*) f->Get(hnamet0Sect.c_str());
	  TH2F *htdriftSect = (TH2F*) f->Get(hnametdriftSect.c_str());
	  
	  int eta_id = atoi((chamber.substr(3,1).c_str()));
	  //
	  // APPLY ALGORITHMS TO chamber histograms HERE 
	  //
	  MdtDqaDb *chamberDqaDb = new MdtDqaDb(chamber);
	  for (int ML=1; ML<=2; ML++) {    // loop over ML
            // start the TDC FIT :
            if (ML==1)  histoName = "A_TDC_ML1";
            if (ML==2)  histoName = "A_TDC_ML2";
            hh = (TH1F*) ChamberDir->Get(histoName.c_str());
            if (!hh) {
              continue;
            }

            entries=hh->GetEntries();
	    if ( entries >= minEntries ) {
	      double *pfit = new double[nParams];
	      double *errfit = new double[nParams];
	      double chi2 = 0.;
	      int ndof = 0;
	      for ( int ii=0; ii<nParams; ii++ ) errfit[ii]=0.;
	      for ( int ii=0; ii<nParams; ii++ ) pfit[ii]=pdefault[ii];

	      myFitter->doTimeFit(hh,nParams,&pfit[0],&errfit[0],&chi2,&ndof );

	      float t0 = pfit[4]*tdc2ns;
	      float t0slope = pfit[6]*tdc2ns;
	      float tdrift = (pfit[5]-pfit[4])*tdc2ns;
	      float chi2ndof = chi2/ndof;
	      float t0err = errfit[4]*tdc2ns;
	      float tdrifterr = std::pow(errfit[4]*errfit[4] + errfit[5]*errfit[5], 0.5)*tdc2ns;
	      if (ML==1) chamberDqaDb->SetML1(t0,tdrift,chi2ndof,t0err,tdrifterr );
	      if (ML==2) chamberDqaDb->SetML2(t0,tdrift,chi2ndof,t0err,tdrifterr );
	      // here add t0 and tdrift to the Sector OVERVIEW histograms :
	      std::string hnamet0=sectorDirName+"/OVERVIEW/t0PerML"+chamberType;
	      std::string hnametdrift=sectorDirName+"/OVERVIEW/tdriftPerML"+chamberType;
	      if (chamberType=="BOG" || chamberType=="BOF") {
		hnamet0=sectorDirName+"/OVERVIEW/t0PerMLBOGBOF";
		hnametdrift=sectorDirName+"/OVERVIEW/tdriftPerMLBOGBOF";
	      }
	      TH1F *ht0 = (TH1F*) f->Get(hnamet0.c_str());
	      TH1F *htdrift = (TH1F*) f->Get(hnametdrift.c_str());

	      if (!ht0) {
		continue;
	      }
	      if (!htdrift) {
		continue;
	      }
	      int ibin = eta_id*2-1;
	      if (ML==2) ibin = eta_id*2;
	      if ((chamberType=="BOG" || chamberType=="BOF") && side=="A") ibin = ibin+2;
	      
	      float MLshift = 0.;
	      if (ML==1) MLshift = -0.25;
	      if (ML==2) MLshift =  0.25;
	      
	      int truebin = ht0->FindBin(eta_id+MLshift);
	      
	      ht0->SetBinContent(truebin,t0);
	      ht0->SetBinError(truebin,t0err);
	      htdrift->SetBinContent(truebin,tdrift);
	      htdrift->SetBinError(truebin,tdrifterr);
	      
	      if (ht0Sect) ht0Sect->Fill(float(isector),t0);
	      if (htdriftSect) htdriftSect->Fill(float(isector),tdrift);
	      
	      htpar = (TH1F*) m_histoManager->GetMdtHisto("time_Fit_t0",Region,Side);
	      if (htpar) htpar->Fill(t0);
	      htpar = (TH1F*) m_histoManager->GetMdtHisto("time_Fit_tdrift",Region,Side);
	      if (htpar) htpar->Fill(tdrift);
	      htpar = (TH1F*) m_histoManager->GetMdtHisto("time_Fit_tslope",Region,Side);
	      if (htpar) htpar->Fill(t0slope);
	      htpar = (TH1F*) m_histoManager->GetMdtHisto("time_Fit_chi2",Region,Side);
	      if(chi2ndof > 49.) chi2ndof=49.;
	      if (htpar) htpar->Fill(chi2ndof); 
            }

            // TDC FIT DONE

            //
            // start the ADC FIT
            if (ML==1)  histoName = "C_ADC_ML1";
            if (ML==2)  histoName = "C_ADC_ML2";
            hh = (TH1F*) ChamberDir->Get(histoName.c_str());
            if (!hh) {
              continue;
            }
            float ADCCUT= m_ADCCUT;   // MUST BE SET BY JOB OPTION !

	    entries =hh->GetEntries();
	    int binADCCUT = hh->GetXaxis()->FindBin(ADCCUT);
	    double belowADCCUT = hh->Integral(1,binADCCUT);
	    hapar = (TH1F*) m_histoManager->GetMdtHisto("hits_perML_belowADCCUT",Region,Side);
	    if (hapar && entries>0) hapar->Fill(belowADCCUT/entries);
 
	    if ( entries >= minEntries ) {
	      double *Adcpfit = new double[nParamsAdc];
	      double *errAdcpfit = new double[nParamsAdc];
	      double chi2Adc = 0.;
	      int ndofAdc = 0;
	      for ( int ii=0; ii<nParamsAdc; ii++ ) errAdcpfit[ii]=0.;
	      for ( int ii=0; ii<nParamsAdc; ii++ ) Adcpfit[ii]=AdcpfitDefault[ii];
	      
	      myFitter->doAdcFit(hh,nParamsAdc,&Adcpfit[0],&errAdcpfit[0],&chi2Adc,&ndofAdc );
	      hapar = (TH1F*) m_histoManager->GetMdtHisto("adc_Fit_peak",Region,Side);
	      if (hapar) hapar->Fill((float)Adcpfit[1]);
	      hapar = (TH1F*) m_histoManager->GetMdtHisto("adc_Fit_width",Region,Side);
	      if (hapar) hapar->Fill((float)Adcpfit[2]);
	      
            }
            // ADC FIT DONE
	  }  // end loop over ML

	  if ( WriteMdtDqaDbToTextFile )  m_MdtDqaDbList.push_back(chamberDqaDb);

	} // endo of chambers loop
      } //end of sectors loop
    } //end of regions loop
    
  } // close if doTDCADCfitting
  
  if (doResidualsFit) {
  // LOOP over chambers and apply algorithms per chamber.
  //
    std::string region;
    std::string sector;
    std::string chamber;
    std::string regionDirName;
    std::string sectorDirName;
    std::string chamberDirName;
    TDirectory *RegionDir;
    TDirectory *SectorDir;
    TDirectory *ChamberDir;

    for (int iregion=1; iregion<=4; iregion++) {
      if (iregion==1) region="Barrel_A";
      if (iregion==2) region="Barrel_C";
      if (iregion==3) region="Endcap_A";
      if (iregion==4) region="Endcap_C";
      
      regionDirName = "/MDT/"+region;
      RegionDir = (TDirectory*)f->Get(regionDirName.c_str());
      if (!RegionDir) continue;

      // reset output histograms 
      TH1F *hMean = (TH1F*) RegionDir->Get("ResidualsMean");
      TH1F *hWidth = (TH1F*) RegionDir->Get("ResidualsWidth");
      if(hMean) hMean->Reset();
      if(hWidth) hWidth->Reset();

      for (int isector=1; isector<=16; isector++) {

	if (isector<10) sector="0"+ts(isector);
	if (isector>=10) sector=ts(isector);
	sectorDirName = "/MDT/"+region+"/Sector"+sector;

	SectorDir = (TDirectory*)f->Get(sectorDirName.c_str());
	if (!SectorDir) continue;
	TIter next(SectorDir->GetListOfKeys());
	TKey *key;
	while ( (key = (TKey*)next()) ) {
	  chamber=key->GetName();
	  if (chamber=="OVERVIEW") continue;
	  chamberDirName = sectorDirName+"/"+chamber;
	  ChamberDir = (TDirectory*)f->Get(chamberDirName.c_str());
	  if (!ChamberDir) continue;
	  ChamberDir->cd();

	  //
	  // APPLY ALGORITHMS TO chamber histograms HERE 
	  //
	  std::string histoName;
	  
	  TH2F *hsegResid;
	  
	  histoName = "b_SegResidVsRadius";
	  hsegResid = (TH2F*) ChamberDir->Get(histoName.c_str());

	  if (!hsegResid) { 
	    continue;
	  }

	  // HERE make Projection and Fit Residuals with double gaussian
	  if (hsegResid->GetEntries() != 0 ) {
	    TH1D *hresidProx = hsegResid->ProjectionX("ResidProx");

	    // PREPARE LOOP OVER Radius Slices
	    int nbins = hresidProx->GetNbinsX();
	    int binStart = 1;
	    int groupBins = 2;
	    int nsteps = (nbins-binStart)/groupBins + 1;
	    int bin1;
	    int bin2 = binStart-1;
	    for (int n=0; n<nsteps; n++){
	      bin1 = bin2+1;
	      bin2 = bin1+groupBins-1;
	      float r1 = hresidProx->GetBinCenter(bin1);
	      float r2 = hresidProx->GetBinCenter(bin2);
	      float radius = (r1+r2)/2.;
	      TH1D *hres = hsegResid->ProjectionY("Residuals",bin1,bin2);
	      if (hres->GetEntries() < 100.) {
		hres->Delete();
		continue;
	      }

// FIT Constant+Gaussian :

	      TF1 *func = new TF1("cgaus","pol0(0)+gaus(1)");
	      double hmax = hres->GetMaximum();
	      int binMax = hres->GetMaximumBin();
	      double hmean = hres->GetBinCenter(binMax);
	      double hrms = hres->GetRMS();
	      func->SetParameter(0,hmax/20.);
	      func->SetParameter(1,hmax);
	      func->SetParameter(2,hmean);
	      func->SetParameter(3,0.2);
	      double xmin = hmean - hrms;
	      double xmax = hmean + hrms;
	      hres->Fit("cgaus","Q,0","",xmin,xmax);
	      double amplitNoise = func->GetParameter(0);
	      double amplitNarrow = func->GetParameter(1);
	      double meanNarrow = func->GetParameter(2);
	      double widthNarrow = func->GetParameter(3);

	      bool badChi2 = (func->GetChisquare())/(func->GetNDF()) > 20.;
	      bool badWidths = (widthNarrow>1.);
	      bool badMean = (meanNarrow<-1.) || (meanNarrow>1.);
	      bool badNoise = (amplitNoise>hmax/3.) || (amplitNoise>amplitNarrow/2. );

	      if (badChi2 || badWidths || badMean || badNoise ) {
		widthNarrow = 0.9;
		meanNarrow = 0.9;
	      }

	      if(hWidth) hWidth->Fill(radius,(float)widthNarrow);
	      if(hMean) hMean->Fill(radius,(float)meanNarrow);
	      
	      hres->Delete();
	    } //END LOOP over SLICES
	    hresidProx->Delete();

	  } // Entries > 0

	} // endo of chambers loop
      } //end of sectors loop
    } //end of regions loop
  } // end doResidualsFit

    // HERE is the OUTPUT to stdOutput and to TEXT file and EVENTUALLY to DataBase
    // TO BE ACTIVATED VIA JOB_OPTION - still to do
    //
  if ( WriteMdtDqaDbToTextFile ) {
    std::string mdtDqaTextFileName = m_outputFileName+".txt";
    std::ofstream outputFile(mdtDqaTextFileName.c_str());
    std::vector<MdtDqaDb*>::iterator it;
    for (it = m_MdtDqaDbList.begin();
	 it!=m_MdtDqaDbList.end();++it) {
      (*it)-> Print();
      (*it)-> Print(&outputFile);
    }
    outputFile.close();
  }

  // Clean (reset) m_MdtDqaDbList 

  return;
} // MdtDqaNtupleAnalysis::histogramAnalysis

}//namespace MuonCalib

