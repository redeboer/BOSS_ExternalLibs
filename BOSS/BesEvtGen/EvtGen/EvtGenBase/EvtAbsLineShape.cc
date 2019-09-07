//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtLineShape.cc
//
// Description: Store particle properties for one particle.
//
// Modification history:
//
//    Lange     March 10, 2001        Module created
//
//------------------------------------------------------------------------
//
#include "EvtGenBase/EvtPatches.hh"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <algorithm>
#include "EvtGenBase/EvtAbsLineShape.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenBase/EvtTwoBodyVertex.hh"
#include "EvtGenBase/EvtPropBreitWigner.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
using std::fstream;
using namespace std;///////////

EvtAbsLineShape::EvtAbsLineShape() {
}

EvtAbsLineShape::~EvtAbsLineShape() {
}

EvtAbsLineShape::EvtAbsLineShape(double mass, double width, double maxRange, EvtSpinType::spintype sp) {

  _includeDecayFact = false;
  _includeBirthFact = false;
  _applyFixForSP8 = false;
  _mass=mass;
  _width=width;
  _spin=sp;
  _maxRange=maxRange;
  double maxdelta=15.0*width;
  //if ( width>0.001 ) {
  //  if ( 5.0*width < 0.6 ) maxdelta = 0.6;
  //}
  if ( maxRange > 0.00001 ) {
    _massMax=mass+maxdelta;
    _massMin=mass-maxRange;
  }
  else{
    _massMax=mass+maxdelta;
    _massMin=mass-15.0*width;
  }
  if ( _massMin< 0. ) _massMin=0.;
  _massMax=mass+maxdelta;
}

EvtAbsLineShape::EvtAbsLineShape(const EvtAbsLineShape& x){

  _includeDecayFact = x._includeDecayFact;
  _includeBirthFact = x._includeBirthFact;
  _mass=x._mass;
  _massMax=x._massMax;
  _massMin=x._massMin;
  _width=x._width;
  _spin=x._spin;
  _maxRange=x._maxRange;
  _applyFixForSP8 = x._applyFixForSP8;
}

EvtAbsLineShape& EvtAbsLineShape::operator=(const EvtAbsLineShape& x){

  _includeDecayFact = x._includeDecayFact;
  _includeBirthFact = x._includeBirthFact;
  _mass=x._mass;
  _massMax=x._massMax;
  _massMin=x._massMin;
  _width=x._width;
  _spin=x._spin;
  _maxRange=x._maxRange;
  _applyFixForSP8 = x._applyFixForSP8;
  return *this; 
}

EvtAbsLineShape* EvtAbsLineShape::clone() {

  return new EvtAbsLineShape(*this);
}


double EvtAbsLineShape::rollMass() {

  double ymin, ymax;
  double temp;

  if ( _width < 0.0001 ) {
    return _mass;
  }
  else{
    ymin = atan( 2.0*(_massMin-_mass)/_width);
    ymax = atan( 2.0*(_massMax-_mass)/_width);

    temp= ( _mass + ((_width/2.0)*tan(EvtRandom::Flat(ymin,ymax))));

    return temp;
  }
}
double EvtAbsLineShape::getRandMass(EvtId *parId, int nDaug, EvtId *dauId, EvtId *othDaugId, double maxMass, double *dauMasses) {

  if ( _width< 0.0001) return _mass;
  //its not flat - but generated according to a BW

  if (maxMass>0&&maxMass<_massMin) {
    report(ERROR,"EvtGen") << "In EvtAbsLineShape::getRandMass"<<endl;
    report(ERROR,"EvtGen") << "Decaying particle "<<EvtPDL::name(*parId)
                           << " with mass "<<maxMass<<endl;
    report(ERROR,"EvtGen") << " to particle" 
                           << " with a minimal mass of "<< _massMin<<endl;
  }

  double mMin=_massMin;
  double mMax=_massMax;
  if ( maxMass>-0.5 && maxMass< mMax) mMax=maxMass;
  double ymin = atan( 2.0*(mMin-_mass)/_width);
  double ymax = atan( 2.0*(mMax-_mass)/_width);

 loop: 
  double themass = ( _mass + ((_width/2.0)*tan(EvtRandom::Flat(ymin,ymax))));
  
  if(fabs(_addFactorPn)>0.00000001){// addFactorPn, pingrg-2010-1-10
   double   massOthD=EvtPDL::getMeanMass(*othDaugId);
   double   massParent=EvtPDL::getMeanMass(*parId);
   double   phsp,maxp,maxp1,maxp2;
   if(themass+massOthD <massParent ){
     EvtTwoBodyVertex vb(themass,massOthD,massParent,1.0);
     phsp = vb.pD();
   } else {return  themass;}

   if( (massOthD + mMax)< massParent){
     EvtTwoBodyVertex vb1(massOthD,mMax,massParent,1);
     EvtTwoBodyVertex vb2(massOthD,mMin,massParent,1);
     maxp = vb1.pD();
     maxp1 = pow(maxp,_addFactorPn*2.0);
     maxp = vb2.pD();
     maxp2= pow(maxp,_addFactorPn*2.0);
     maxp = max(maxp1,maxp2);
   }else {return  themass;}

   double wt = pow(phsp,_addFactorPn*2.0)/maxp;
   double rdm = EvtRandom::Flat(0.0,1.0);

   if(rdm> wt ) goto loop;

  }
 
  return  themass;
  //  return EvtRandom::Flat(_massMin,_massMax);
}

double EvtAbsLineShape::getMassProb(double mass, double massPar, int nDaug, double *massDau) {

  double dTotMass=0.;
  if ( nDaug>1) {
    int i;
    for (i=0; i<nDaug; i++) {
      dTotMass+=massDau[i];
    }
    //report(INFO,"EvtGen") << mass << " " << massPar << " " << dTotMass << " "<< endl;
    //    if ( (mass-dTotMass)<0.0001 ) return 0.;
    if ( (mass<dTotMass) ) return 0.;
  }
  if ( _width< 0.0001) return 1.;

  // no parent - lets not panic
  if ( massPar>0.0000000001 ) {
    if ( mass > massPar) return 0.;
  }
  //Otherwise return the right value.
  //Fortunately we have generated events according to a non-rel BW, so 
  //just return..
  //EvtPropBreitWigner bw(_mass,_width);
  //EvtPropFactor<EvtTwoBodyVertex> f(bw);
  //EvtComplex fm=f.eval(mass);
  //EvtComplex fm0=f.eval(_mass);
  //return (abs(fm)*abs(fm))/(abs(fm0)*abs(fm0));
  return 1.0;
}


