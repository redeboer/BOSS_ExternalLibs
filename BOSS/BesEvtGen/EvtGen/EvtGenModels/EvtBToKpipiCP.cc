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
// Module: EvtBToKpipiCP.cc
//
// Description: Routine to decay B->K pi pi
//              and has CP violation.
//       --- This is the routine to be called by the Main generator
//          to get the decay of B0    -->-- K+ pi- pi0
//          The decay proceeeds through three channels:
//          a) B0 -->-- K*+ pi-  ; K*+    -->-- K+ pi0
//          b)          K*0 pi0  ; K*0bar -->-- K+ pi-
//          c)          K-  rho+ ; rho+   -->-- pi+ pi0
//         It provides at the same time the CP conjugate decay
//                              B0bar -->-- K- pi+ pi0
//
// Modification history:
//
//    Versille     September, 1997         Module created
//
//------------------------------------------------------------------------
//
#include "EvtGenBase/EvtPatches.hh"
#include <stdlib.h>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtCPUtil.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenModels/EvtBToKpipiCP.hh"
#include "EvtGenBase/EvtId.hh"
#include <string>

extern "C" {
  extern void evtkpipi_(double *, double *, int *,double *,
			 double *,double *,double *,double *,
			 double *,double *,double *);
}

EvtBToKpipiCP::~EvtBToKpipiCP() {}


void EvtBToKpipiCP::getName(std::string& model_name){

  model_name="BTOKPIPI_CP";     

}


EvtDecayBase* EvtBToKpipiCP::clone(){

  return new EvtBToKpipiCP;

}

void EvtBToKpipiCP::init(){

  // check that there are 3 arguments
  checkNArg(3);
  checkNDaug(3);

  checkSpinParent(EvtSpinType::SCALAR);

  checkSpinDaughter(0,EvtSpinType::SCALAR);
  checkSpinDaughter(1,EvtSpinType::SCALAR);
  checkSpinDaughter(2,EvtSpinType::SCALAR);

}


void EvtBToKpipiCP::decay( EvtParticle *p){

  //added by Lange Jan4,2000
  static EvtId B0=EvtPDL::getId("B0");
  static EvtId B0B=EvtPDL::getId("anti-B0");

  double t;
  EvtId other_b;

  EvtCPUtil::OtherB(p,t,other_b);

  EvtParticle *Kp,*pim,*pi0;

  p->makeDaughters(getNDaug(),getDaugs());
  Kp=p->getDaug(0);
  pim=p->getDaug(1);
  pi0=p->getDaug(2);

  EvtVector4R p4[3];

  //double dm=getArg(0);
  double alpha=getArg(1);
  double beta=getArg(2);
  int iset;

  static int first=1;

  if (first==1) {
    iset=10000;
    first=0;
  }
  else{
    iset=0;
  }

  double p4Kplus[4],p4piminus[4],p4gamm1[4],p4gamm2[4]; 

  double realA,imgA,realbarA,imgbarA;

  evtkpipi_(&alpha,&beta,&iset,p4Kplus,p4piminus,p4gamm1,p4gamm2,
	     &realA,&imgA,&realbarA,&imgbarA);

  p4[0].set(p4Kplus[3],p4Kplus[0],p4Kplus[1],p4Kplus[2]);
  p4[1].set(p4piminus[3],p4piminus[0],p4piminus[1],p4piminus[2]);
  p4[2].set(p4gamm1[3]+p4gamm2[3],p4gamm1[0]+p4gamm2[0],
	    p4gamm1[1]+p4gamm2[1],p4gamm1[2]+p4gamm2[2]);

   Kp->init( getDaug(0), p4[0] );
   pim->init( getDaug(1), p4[1] );
   pi0->init( getDaug(2), p4[2] );

   EvtComplex amp;

   EvtComplex A(realA,imgA);
   EvtComplex Abar(realbarA,imgbarA);

   if (other_b==B0B){
     amp=Abar;
   }
   if (other_b==B0){
     amp=A;
   }

   vertex(amp);

  return ;
}

