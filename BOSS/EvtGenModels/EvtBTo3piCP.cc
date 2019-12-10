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
// Module: EvtBTo3piCP.cc
//
// Description: Routine to decay B->pi+ pi- pi0
//              and has CP violation.
//
// Modification history:
//
//    RYD/VERSILLE     March 2, 1997        Module created
//
//------------------------------------------------------------------------
//
#include "EvtGenBase/EvtPatches.hh"
#include <stdlib.h>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtCPUtil.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenModels/EvtBTo3piCP.hh"
#include "EvtGenBase/EvtId.hh"
#include <string>
#include "EvtGenBase/EvtConst.hh"

extern "C" {
  extern void evt3pions_(double *,int *,double *,
			 double *,double *,double *,double *,
			 double *,double *,double *);
}


EvtBTo3piCP::~EvtBTo3piCP() {}


void EvtBTo3piCP::getName(std::string& model_name){

  model_name="BTO3PI_CP";     

}


EvtDecayBase* EvtBTo3piCP::clone(){

  return new EvtBTo3piCP;

}

void EvtBTo3piCP::init(){

  // check that there are 2 arguments
  checkNArg(2);
  checkNDaug(3);

  checkSpinParent(EvtSpinType::SCALAR);

  checkSpinDaughter(0,EvtSpinType::SCALAR);
  checkSpinDaughter(1,EvtSpinType::SCALAR);
  checkSpinDaughter(2,EvtSpinType::SCALAR);
}



void EvtBTo3piCP::initProbMax(){

  setProbMax(1.5);

}

void EvtBTo3piCP::decay( EvtParticle *p){

  //added by Lange Jan4,2000
  static EvtId B0=EvtPDL::getId("B0");
  static EvtId B0B=EvtPDL::getId("anti-B0");

  double t;
  EvtId other_b;

  EvtCPUtil::OtherB(p,t,other_b);

  EvtParticle *pip,*pim,*pi0;

  p->makeDaughters(getNDaug(),getDaugs());

  //  p->init_daug(SCALAR,&pip,SCALAR,&pim,SCALAR,&pi0);
  pip=p->getDaug(0);
  pim=p->getDaug(1);
  pi0=p->getDaug(2);

  EvtVector4R p4[3];

  double dm=getArg(0);
  double alpha=getArg(1);
  int iset;

  static int first=1;

  if (first==1) {
    iset=10000;
    first=0;
  }
  else{
    iset=0;
  }

  double p4piplus[4],p4piminus[4],p4gamm1[4],p4gamm2[4]; 

  double realA,imgA,realbarA,imgbarA;

  evt3pions_(&alpha,&iset,p4piplus,p4piminus,p4gamm1,p4gamm2,
	     &realA,&imgA,&realbarA,&imgbarA);

  p4[0].set(p4piplus[3],p4piplus[0],p4piplus[1],p4piplus[2]);
  p4[1].set(p4piminus[3],p4piminus[0],p4piminus[1],p4piminus[2]);
  p4[2].set(p4gamm1[3]+p4gamm2[3],p4gamm1[0]+p4gamm2[0],
	    p4gamm1[1]+p4gamm2[1],p4gamm1[2]+p4gamm2[2]);

  if (pip->getId()==EvtPDL::getId("pi+")) {
    pip->init( getDaug(0), p4[0] );
    pim->init( getDaug(1), p4[1] );
  }
  else {
    pip->init( getDaug(0), p4[1] );
    pim->init( getDaug(1), p4[0] );  
  }

   pi0->init( getDaug(2), p4[2] );
   
   EvtComplex amp;

   EvtComplex A(realA,imgA);
   EvtComplex Abar(realbarA,imgbarA);

   if (other_b==B0B){
     amp=A*cos(dm*t/(2*EvtConst::c))+
       EvtComplex(0.,1.)*Abar*sin(dm*t/(2*EvtConst::c));
   }
   if (other_b==B0){
     amp=Abar*cos(dm*t/(2*EvtConst::c))+
       EvtComplex(0.,1.)*A*sin(dm*t/(2*EvtConst::c));
   }

   vertex(amp);

  return ;
}

