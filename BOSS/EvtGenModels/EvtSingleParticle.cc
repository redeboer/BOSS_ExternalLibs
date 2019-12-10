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
// Module: EvtSingleParticle.cc
//
// Description: Special model to generate single particles.
//
// Modification history:
//
//    RYD       Febuary 17,1998       Module created
//
//------------------------------------------------------------------------
//
#include "EvtGenBase/EvtPatches.hh"
#include <stdlib.h>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenModels/EvtSingleParticle.hh"
#include "EvtGenBase/EvtReport.hh"
#include <string>
#include "EvtGenBase/EvtConst.hh"
using std::endl;

EvtSingleParticle::~EvtSingleParticle() {}

void EvtSingleParticle::getName(std::string& model_name){

  model_name="SINGLE";     

}

EvtDecayBase* EvtSingleParticle::clone(){

  return new EvtSingleParticle();

}

void EvtSingleParticle::init(){


  //turn off checks for charge conservation
  disableCheckQ();

  if ((getNArg()==6)||(getNArg()==4)||(getNArg()==2)) {
    
    if (getNArg()==6){
      //copy the arguments into eaiser to remember names!

      pmin=getArg(0);
      pmax=getArg(1);
      
      cthetamin=getArg(2);
      cthetamax=getArg(3);
      
      phimin=getArg(4);
      phimax=getArg(5);

    }

    if (getNArg()==4){
      //copy the arguments into eaiser to remember names!

      pmin=getArg(0);
      pmax=getArg(1);
      
      cthetamin=getArg(2);
      cthetamax=getArg(3);
      
      phimin=0.0;
      phimax=EvtConst::twoPi;

    }

    if (getNArg()==2){
      //copy the arguments into eaiser to remember names!

      pmin=getArg(0);
      pmax=getArg(1);
      
      cthetamin=-1.0;
      cthetamax=1.0;
      
      phimin=0.0;
      phimax=EvtConst::twoPi;

    }


  }else{
    
    report(ERROR,"EvtGen") << "EvtSingleParticle generator expected "
                           << " 6, 4, or 2 arguments but found:"<<getNArg()<<endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
    ::abort();

  }


  report(INFO,"EvtGen") << "The single particle generator has been configured:"
    <<endl;
  report(INFO,"EvtGen") << pmax << " > p > " << pmin <<endl;
  report(INFO,"EvtGen") << cthetamax << " > costheta > " << cthetamin <<endl;
  report(INFO,"EvtGen") << phimax << " > phi > " << phimin <<endl;

}

void EvtSingleParticle::decay( EvtParticle *p ){

  EvtParticle *d;
  EvtVector4R p4;

  double mass=EvtPDL::getMass(getDaug(0));

  p->makeDaughters(getNDaug(),getDaugs());
  d=p->getDaug(0);

  //generate flat distribution in p 
  //we are now in the parents restframe! This means the 
  //restframe of the e+e- collison.
  double pcm;
  if (pmin==pmax) pcm=pmin;  //pingrg, 2008-04-15
  if (pmin< pmax) pcm=EvtRandom::Flat(pmin,pmax);

  //generate flat distribution in phi.
  double phi;
  if(phimin==phimax) phi=phimin; //pingrg 2008-10-22
  else phi=EvtRandom::Flat(phimin,phimax);
  
  double cthetalab;

  do{
    //generate flat distribution in costheta
    double ctheta;
    if(cthetamin==cthetamax) ctheta=cthetamin;
    else ctheta=EvtRandom::Flat(cthetamin,cthetamax);
    double stheta=sqrt(1.0-ctheta*ctheta);
    p4.set(sqrt(mass*mass+pcm*pcm),pcm*cos(phi)*stheta,
	   pcm*sin(phi)*stheta,pcm*ctheta);

    d->init( getDaug(0),p4);

    //get 4 vector in the lab frame!
    EvtVector4R p4lab=d->getP4Lab();
    cthetalab=p4lab.get(3)/p4lab.d3mag();
  }while (cthetalab>cthetamax||cthetalab<cthetamin);

  return ;
}



