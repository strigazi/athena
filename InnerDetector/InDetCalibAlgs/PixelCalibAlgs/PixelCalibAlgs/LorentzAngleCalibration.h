/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LorentzAngleCalibration_h
#define LorentzAngleCalibration_h

class string;
class TProfile;
template <class ht> class MultiHisto;
template <class type> class TVectorT;

using namespace std;

namespace PixelCalib{

class LorentzAngleCalibration{

public :
	LorentzAngleCalibration(int layer = 0, std::string cosmicORbeam="beam");
	virtual ~LorentzAngleCalibration(){};
	bool Fill(Int_t Layer, Int_t EtaModule, Int_t PhiModule,
			Double_t ClusterSize, Double_t Angle);
	int Analyze(ofstream &logfile);
	int Write();
	int Read();
	void PlotValidation(TCanvas *c1, std::string outname);
	
private:
	int m_curLayer;
	std::string m_curType;
	std::string m_layername;
	MultiHisto<TProfile> *m_LayerProfile;
	TVectorT<double> *m_LorentzAngle0;
	TVectorT<double> *m_LorentzAngle0Errors;
	TVectorT<double> *m_LorentzAngle1;
	TVectorT<double> *m_LorentzAngle1Errors;
	TVectorT<double> *m_LorentzAngle2;
	TVectorT<double> *m_LorentzAngle2Errors;
	TVectorT<double> *m_EtaModule;
	TVectorT<double> *m_PhiModule;

};

}
#endif // #ifdef LorentzAngleCalibration_h
