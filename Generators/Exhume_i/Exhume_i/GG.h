/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-*-c++-*-
//-*-GG.h-*-
//   Written by James Monk and Andrew Pilkington
/////////////////////////////////////////////////////////////////////////////
#ifndef GG_HH
#define GG_HH

#include "TwoSpace.h"

namespace Exhume{
  class GG : public TwoSpace{

  public:
    GG(std::string _ExhumeControlParams);
   
    //declare inherited functions
    double SubProcess();
    void LIPS2Amp();
    void Amp2LIPS();

  private:
    double EighteenPI;
    double InvSinTheta;

    /*
   void SetPartons();
    void SetSubParameters();
    double SubParameterWeight();
    void MaximiseSubParameters();
    double SubParameterRange();
      
    //declare sub-process specific functions
    void SetThetaMin(const double&);

  private:
    //internal functions
    double WeightFunc(const double&);
    //internal Parameters
    double CosTheta,CosThetaMin,CosThetaMax;
    double ThetaMin,ThetaMax, SinThetaMin;
    double Theta,SinTheta,InvSinTheta; 
    double E, Px,Py,Pz,Phi;
    double SubWgt;
    double EighteenPI;
    */
  };
}
#endif