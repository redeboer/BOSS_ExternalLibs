// -*- C++ -*-

//
// Author:      Dan Ambrose
// Created:     Mon Mar 5 2012
// based on Cleo package by Eric White and Warner Sun

#ifndef DALITZ_H
#define DALITZ_H

//#include "EvtGen/EvtComplex.h"
#include <complex>
using namespace std;
typedef complex<double> TComplex;

// For using "string"
/*
#include "Experiment/Experiment.h"
#include "Experiment/report.h"
#include "Experiment/units.h"
#include "DataHandler/Record.h"
#include "DataHandler/Frame.h"
#include "FrameAccess/extract.h"
#include "FrameAccess/FAItem.h"
#include "FrameAccess/FATable.h"
*/
using std::string;

// Class Dalitz
class Dalitz {

   public:
      Dalitz();
      Dalitz(int binNum);
      TComplex Amplitude(double x, double y, double z);
      double Phase(double x, double y, double z, int Babar=1); //1 for Babar model, 2 for CLEO model, else for Belle model
      bool Point_on_DP(double x, double y);
      bool Point_on_DP2(double x, double y);
      TComplex CLEO_resAmp(double mAC, double mBC, double mAB,
                      double mA, double mB, double mC,
                      double _ampl, double _theta, double _gamma,
                      double _bwm, int _spin);
      TComplex resAmp(double mAC, double mBC, double mAB,
                      double mA, double mB, double mC,
                      double _ampl, double _theta, double _gamma,
                      double _bwm, int _spin);
      TComplex f_980(double mPP, double mR,
                     double _ampl, double _theta );
      TComplex sakurai(double mkp, double mkm, double mpp, 
                       double _ampl, double _theta,
                       double gamma_r, double m_r);
      TComplex Babar_sakurai(double mkp, double mkm, double mpp,
                             double _ampl, double _theta,
                             double gamma_r, double m_r);
      TComplex Babar_resAmp(double mAC, double mBC, double mAB,
                            double mB , double mA , double mC ,
                            double _ampl, double _theta, double _gamma,
                            double _bwm, int _spin);
      TComplex Babar_Amplitude(double x, double y, double z);
      TComplex CLEO_Amplitude(double x, double y, double z);
      int getBin(double mx, double my, double mz);

   private:
      static double PI; // pi
      int N; // Bin number, set in constructor
      static const int Nbin = 1000; // number of dx for integral

};

#endif

