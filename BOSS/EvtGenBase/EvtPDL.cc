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
// Module: EvtPDL.cc
//
// Description: routines to store particle properties in EvtPDL structure.
//
// Modification history:
//
//    DJL/RYD     September 25, 1996         Module created
//
//------------------------------------------------------------------------
// 
#include "EvtGenBase/EvtPatches.hh"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtPartProp.hh"
#include "EvtGenBase/EvtId.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtReport.hh"
using std::endl;
using std::fstream;
using std::ifstream;

static int first=1;

int EvtPDL::_firstAlias;
int EvtPDL::_nentries;

std::map<std::string,int> EvtPDL::_particleNameLookup;

EvtPDL::EvtPDL() {

  if (first!=0) { 
    first=0;
    _nentries=0;
    _firstAlias=999999;
  }

}


EvtPDL::~EvtPDL(){
  
}

void EvtPDL::read(const char* fname)
{
  readPDT(fname);
}

void EvtPDL::readPDT(const std::string fname){


  ifstream indec;
  
  indec.open(fname.c_str());

  char cmnd[100];
  char xxxx[100];

  char pname[100];
  int  stdhepid;
  double mass;
  double pwidth;
  double pmaxwidth;
  int    chg3;  
  int    spin2;
  double ctau;
  int    lundkc;
  EvtId i;

  if (!indec) {
    report(ERROR,"EvtGen") << "Could not open:"<<fname.c_str()<<"EvtPDL"<<endl;
    return;
  }

  do{

    char ch,ch1;

    do{

      indec.get(ch);  
      if (ch=='\n') indec.get(ch);
      if (ch!='*') {
	indec.putback(ch);
      }
      else{
	while (indec.get(ch1),ch1!='\n');
      }
    } while(ch=='*');

    indec >> cmnd;

    if (strcmp(cmnd,"end")){

      if (!strcmp(cmnd,"add")){

        indec >> xxxx;
        indec >> xxxx;
        indec >> pname;
        indec >> stdhepid;
        indec >> mass;
        indec >> pwidth;
        indec >> pmaxwidth;
        indec >> chg3;  
        indec >> spin2;
        indec >> ctau;
        indec >> lundkc;


        i=EvtId(_nentries,_nentries);

	EvtPartProp tmp;

	tmp.setSpinType(EvtSpinType::SCALAR);
	
	if (spin2==0) tmp.setSpinType(EvtSpinType::SCALAR);
	if (spin2==1) tmp.setSpinType(EvtSpinType::DIRAC);
	if (spin2==2) tmp.setSpinType(EvtSpinType::VECTOR);
	if (spin2==3) tmp.setSpinType(EvtSpinType::RARITASCHWINGER);
	if (spin2==4) tmp.setSpinType(EvtSpinType::TENSOR);
	if (spin2==5) tmp.setSpinType(EvtSpinType::SPIN5HALF);
	if (spin2==6) tmp.setSpinType(EvtSpinType::SPIN3);
	if (spin2==7) tmp.setSpinType(EvtSpinType::SPIN7HALF);
	if (spin2==8) tmp.setSpinType(EvtSpinType::SPIN4);
	if (spin2==2 && mass < 0.0001 ) tmp.setSpinType(EvtSpinType::PHOTON);
	if (spin2==1 && mass < 0.0001 ) tmp.setSpinType(EvtSpinType::NEUTRINO);
	
	
	if (!strcmp(pname,"string")){
	  tmp.setSpinType(EvtSpinType::STRING);
	}
	
	if (!strcmp(pname,"vpho")){
	  tmp.setSpinType(EvtSpinType::VECTOR);
	}
	
	
	tmp.setId(i);
	tmp.setIdChgConj(EvtId(-1,-1));
	tmp.setStdHep(stdhepid);
	tmp.setLundKC(lundkc);
	tmp.setName(pname);
        assert(_particleNameLookup.find(std::string(pname))==
	       _particleNameLookup.end());
	_particleNameLookup[std::string(pname)]=_nentries;
	tmp.setctau(ctau);
	tmp.setChg3(chg3);
	//report(INFO,"EvtGen") << "particle,chg3:"<<i<<","<<chg3<<endl;
	
	tmp.initLineShape(mass,pwidth,pmaxwidth);


	partlist().push_back(tmp);
	_nentries++;

      }

      // if find a set read information and discard it

      if (!strcmp(cmnd,"set")){

        indec >> xxxx;
        indec >> xxxx;
        indec >> xxxx;
        indec >> xxxx;
      }

    }

  }while(strcmp(cmnd,"end"));
  
  setUpConstsPdt();

}


void EvtPDL::aliasChgConj(EvtId a,EvtId abar){

  if (EvtPDL::chargeConj(EvtId(a.getId(),a.getId()))!=
		 EvtId(abar.getId(),abar.getId())) {

    report(ERROR,"EvtGen")<<"Can't charge conjugate the two aliases:"
			  <<EvtPDL::name(a).c_str()<<" and "<<EvtPDL::name(abar).c_str()<<endl;
      
    ::abort();

  }

  partlist()[a.getAlias()].setIdChgConj(abar);
  partlist()[abar.getAlias()].setIdChgConj(a);

}

EvtId EvtPDL::chargeConj(EvtId id){
  EvtId idchg=partlist()[id.getAlias()].getIdChgConj();

  if (idchg!=EvtId(-1,-1)) return idchg;

  if (id.getId()!=id.getAlias()){
    if (chargeConj(EvtId(id.getId(),id.getId()))==EvtId(id.getId(),id.getId())){
    
      partlist()[id.getAlias()].setIdChgConj(id);
      return id;
    }
  }

  if (id.getAlias()!=id.getId()) {

    report(ERROR,"EvtGen")<<"Trying to charge conjugate alias particle:"
			  <<name(id).c_str()<<" without defining the alias!"<<endl;
      
    ::abort();

  }

  int i;

  for (i=0;i<partlist().size();i++){
    if (partlist()[i].getStdHep()==-partlist()[id.getId()].getStdHep()){
      partlist()[id.getId()].setIdChgConj(partlist()[i].getId());
      return partlist()[i].getId();
    }
  }
  
  partlist()[id.getId()].setIdChgConj(id);
  return id;
  
}

EvtId EvtPDL::evtIdFromStdHep(int stdhep){

  int i;

  for (i=0;i<partlist().size();i++){
    if (partlist()[i].getStdHep()==stdhep)
      return partlist()[i].getId();
  }
  
  return EvtId(-1,-1);
  
}



void EvtPDL::alias(EvtId num,const std::string& newname){

  int i;
  
  if ( _firstAlias < partlist().size() ) {
    for(i=_firstAlias;i<partlist().size();i--){
      if (newname==partlist()[i].getName()){
	report(WARNING,"EvtGen")<<"Redefining alias:"<<newname.c_str()<<" will be ignored!"<<endl;
	return;
      }
    }
  }
  else{
    _firstAlias=partlist().size();
  }

  partlist().push_back(partlist()[num.getId()]);
  int entry=partlist().size()-1;
  partlist()[entry].setName(newname);
  assert(_particleNameLookup.find(std::string(newname))==
	 _particleNameLookup.end());
  _particleNameLookup[std::string(newname)]=entry;
  partlist()[entry].setId(EvtId(num.getId(),entry));
  //Lange - Dec7, 2003. Unset the charge conjugate.
  partlist()[entry].setIdChgConj(EvtId(-1,-1));

}

EvtId EvtPDL::getId(const std::string& name ){

  std::map<std::string,int>::iterator it=_particleNameLookup.find(std::string(name));
  if (it==_particleNameLookup.end()) return EvtId(-1,-1);

  return partlist()[it->second].getId();
  
}

void EvtPDL::setUpConstsPdt(){

}


