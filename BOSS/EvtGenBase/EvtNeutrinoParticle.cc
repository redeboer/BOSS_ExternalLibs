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
// Module: EvtNeutrinoParticle.cc
//
// Description: Class to describe neutrinos
//
// Modification history:
//
//    DJL/RYD     September 25, 1996         Module created
//
//------------------------------------------------------------------------
// 
#include "EvtGenBase/EvtPatches.hh"
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenBase/EvtNeutrinoParticle.hh"
#include "EvtGenBase/EvtVector4R.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"

using std::endl;


EvtNeutrinoParticle::~EvtNeutrinoParticle(){}

EvtNeutrinoParticle::EvtNeutrinoParticle(){

  return;
}

void EvtNeutrinoParticle::init(EvtId part_n,const EvtVector4R& p4){
  
  _validP4=true;
  setp(p4);
  setpart_num(part_n);
   
  double e,px,py,pz;
  e=p4.get(0);
  px=p4.get(1);
  py=p4.get(2);
  pz=p4.get(3);

  if (EvtPDL::getStdHep(part_n)==0){
    report(ERROR,"EvtGen") << "Error in EvtNeutrinoParticle::init, part_n="
			   << part_n.getId()<<endl;
  }

  if (EvtPDL::getStdHep(part_n)>0){  
  
    double beta,alpha,p2,norm;
  
    // See Sakurai p. 167-169
    // and Renton p. 126
  
    p2=px*px+py*py+pz*pz;
  
    beta=acos(pz/sqrt(p2));
    alpha=atan2(py,px);
  
    norm=sqrt(2*e);
  
    double cosb,sinb,cosa,sina;
  
    cosb=cos(0.5*beta);
    sinb=sin(0.5*beta);
  
    cosa=cos(0.5*alpha);
    sina=sin(0.5*alpha);
  
    spinor_parent.set(-norm*sinb*EvtComplex(cosa,-sina),
	  	    norm*cosb*EvtComplex(cosa,sina),
		    norm*sinb*EvtComplex(cosa,-sina),
		    -norm*cosb*EvtComplex(cosa,sina));

  }
  else{

    px=-p4.get(1);
    py=-p4.get(2);
    pz=-p4.get(3);
   
    double pn,sqrpn;

    pn=e;
    sqrpn=sqrt(pn-pz);
   
    spinor_parent.set((1.0/sqrpn)*EvtComplex(px,-py),
                      EvtComplex(sqrpn,0.0),
	 	      (-1.0/sqrpn)*EvtComplex(px,-py),
		      -EvtComplex(sqrpn,0.0)); 


  }

  setLifetime();

} 


EvtDiracSpinor EvtNeutrinoParticle::spParentNeutrino() const {
  
  return spinor_parent;
}

EvtDiracSpinor EvtNeutrinoParticle::spNeutrino() const {

  report(ERROR,"EvtGen") << "Tried to get neutrino spinor in restframe"; 
  report(ERROR,"EvtGen") << "Will terminate execution."; 

  ::abort();

  return spinor_rest;
}


EvtSpinDensity EvtNeutrinoParticle::rotateToHelicityBasis() const{

  report(ERROR,"EvtGen") << "rotateToHelicityBasis not implemented for neutrino."; 
  report(ERROR,"EvtGen") << "Will terminate execution."; 

  ::abort();

  EvtSpinDensity rho;
  return rho;
  
}

EvtSpinDensity EvtNeutrinoParticle::rotateToHelicityBasis(double alpha,
							  double beta,
							  double gamma) const{

  report(ERROR,"EvtGen") << "rotateToHelicityBasis(alpha,beta,gamma) not implemented for neutrino."; 
  report(ERROR,"EvtGen") << "Will terminate execution."; 

  ::abort();

  EvtSpinDensity rho;
  return rho;


}
