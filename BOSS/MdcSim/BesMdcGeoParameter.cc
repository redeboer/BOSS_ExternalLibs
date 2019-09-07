//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: Handle database I/O and user interface 
//             for MDC geometry parameters
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created: 4 Dec, 2003
//Modified:
//Comment: Used in "BesMdc" now, should be insert in framwork later
//         The units are "mm" and "rad". 
//         Datum plane is the East Endplane of MDC.
//---------------------------------------------------------------------------//

#include <iostream>
#include <fstream>

#include <CLHEP/Units/PhysicalConstants.h>

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ISvcLocator.h"
#include "MdcGeomSvc/MdcGeomSvc.h"

#include "BesMdcGeoParameter.hh"
#include "globals.hh"
#include <cstdlib>
#include "ReadBoostRoot.hh"
#include "G4Svc/IG4Svc.h"
#include "G4Svc/G4Svc.h"

BesMdcGeoParameter * BesMdcGeoParameter::fPointer=0;

BesMdcGeoParameter * BesMdcGeoParameter::GetGeo(void){
      if (! fPointer) fPointer = new BesMdcGeoParameter();
     return fPointer;
}

BesMdcWire::BesMdcWire(double length, double phi, double r,double rotateAngle){
  fLength=length;
  if(phi<0){
    fPhi = phi + 2*pi;
  }else if(phi>=2*pi){
    fPhi = phi - 2*pi;
  }else{    
    fPhi=phi;
  }
  fRadius=r;
  fRotateAngle=rotateAngle;
  
  fX=r*cos(phi);
  fY=r*sin(phi);
}

double BesMdcWire:: Phi(double z) const{
  //double phi=fPhi+fRotateAngle*2*(fLength/2-z)/fLength;
  //yzhang 2011-12-01 
  double OB = R()*sin(RotateAngle());
  double OC = OB*z*2./fLength;
  double phi=fPhi+RotateAngle()-atan2(OC,R()*cos(RotateAngle()));
  //zhangy

  if(phi<0){
    phi += 2*pi;
  }else if(phi>=2*pi){
    phi -= 2*pi;
  }    
  return phi;
}

double BesMdcWire::X(double){
  return fX;
}
double BesMdcWire::Y(double){
  return fY;
}

BesMdcGeoParameter::BesMdcGeoParameter(void){
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IG4Svc* tmpSvc;
  G4Svc* m_G4Svc;
  StatusCode sc=svcLocator->service("G4Svc", tmpSvc);
  m_G4Svc=dynamic_cast<G4Svc *>(tmpSvc); 
  if (!sc.isSuccess())  
   std::cout<<"BesMdcGeoParameter::Could not open G4 Service"<<std::endl;
  if(m_G4Svc->GetMdcDataInput()== 0){ 
    cout<<"------- get BesMdcGeoParameter from file --------"<<endl;
    InitFromFile();
  }
  if(m_G4Svc->GetMdcDataInput()== 1) {
    cout<<"=======get BesMdcGeoParameter from MdcGeomSvc======="<<endl;
    InitFromSvc(); 
  }
  
  //Dump();
  if(fPointer)
  { G4Exception("BesMdcGeoParameter constructed twice."); }
  fPointer=this;
}


BesMdcWire BesMdcGeoParameter::Wire(int wireNo){

    int i;
    for(i=0; i<fLayerNo; i++){
      if(fLayer[i].BeginWireNo()<=wireNo && wireNo<fLayer[i].SumWireNo()){

	break;
      }
    }

   BesMdcWire temp(fLayer[i].Length(), fWirePhi[wireNo], fLayer[i].R(), fLayer[i].RotateAngle());
    return temp;
}



BesMdcWire BesMdcGeoParameter::SignalWire(int signalLayerNo, int wireNo)
{

    int i=fSignalLayer[signalLayerNo];
    int wireNoInLayer=2*wireNo+1-fLayer[i].FirstWire();//FirstWire():0,field;1,signal
    double phi=fLayer[i].Phi();
    double shiftPhi=fLayer[i].ShiftPhi();
    double wirePhi;
    wirePhi= wireNoInLayer*shiftPhi+phi;

    BesMdcWire temp(fLayer[i].Length(), fWirePhi[fLayer[i].BeginWireNo()+wireNoInLayer], fLayer[i].R(),fLayer[i].RotateAngle());
    return temp;
}


const BesMdcLayer& BesMdcGeoParameter::Layer(int layerNumber) const {
    if(layerNumber<0 || layerNumber>89){
    cout<<"Error: Wrong layerNo: "<<layerNumber<<endl;
    }
    return fLayer[layerNumber];
}

const BesMdcLayer& BesMdcGeoParameter::SignalLayer(int layerNumber) const {
    if(layerNumber<0 || layerNumber>42){
    cout<<"Error: Wrong SignallayerNo: "<<layerNumber<<endl;
    }
    return fLayer[fSignalLayer[layerNumber]];
}


void BesMdcGeoParameter::InitFromFile(void){
  int wireNo, firstWire;
  double length, phi, r, rotateCell,rotateAngle;
  double innerR, outR, z;
  string name, line;
 
 G4String geoPath = ReadBoostRoot::GetBoostRoot(); 
  if(!geoPath){ 
    G4Exception("BOOST environment not set!");
  }
  geoPath += "/dat/Mdc.txt";
  
  ifstream inFile(geoPath);
  if(!inFile.good()){
    cout<<"Error, mdc parameters file not exist"<<endl; 
    return;
  }

  getline(inFile, line);
  inFile>>fLayerNo>>fWireNo>>fSignalLayerNo>>fSignalWireR>>fFieldWireR;

  inFile.seekg(1,ios::cur);
  getline(inFile, line);
  int i,signalLayer;
  for(i=0; i<fSignalLayerNo; i++){
    inFile>>signalLayer;
    fSignalLayer[i]=signalLayer-1;
 }

  inFile.seekg(1,ios::cur);
  getline(inFile, line);
  getline(inFile, line);
  for( i=0; i<fLayerNo; i++){
    inFile>>name>>wireNo>>length>>r>>phi>>firstWire>>rotateCell;
    getline(inFile, line);
    
    rotateAngle=2*pi*rotateCell/wireNo;
     
    fLayer[i].SetName(name);fLayer[i].SetRadius(r);    
    fLayer[i].SetLength(length); fLayer[i].SetRotateCell(rotateCell);
    fLayer[i].SetRotateAngle(rotateAngle); fLayer[i].SetWireNo(wireNo); 
    fLayer[i].SetShiftPhi(twopi/wireNo); fLayer[i].SetFirstWire(firstWire);    
    
    phi*=(pi/180); 
    if(phi<0)phi += fLayer[i].ShiftPhi();
    fLayer[i].SetPhi(phi); 
    
    if(i==0){
      fLayer[i].SetSumWireNo(wireNo); fLayer[i].SetBeginWireNo(0);
    }else{
      fLayer[i].SetBeginWireNo(fLayer[i-1].SumWireNo());
      fLayer[i].SetSumWireNo(fLayer[i-1].SumWireNo()+wireNo); 
    }
    
    for(int j=0; j<wireNo; j++){
      fWirePhi[fLayer[i].BeginWireNo()+j]=j*fLayer[i].ShiftPhi()+phi; 
    }
  }  

  if(fLayer[fLayerNo-1].SumWireNo()!= fWireNo){
    cout<<"Total wire number is not consistant!"<<endl;
  }    
  
  getline(inFile, line);
  inFile>>fSegmentNo;  
  inFile.seekg(1,ios::cur);
  getline(inFile, line);
  getline(inFile, line);

  for(i=0; i<fSegmentNo; i++){
    inFile>>length>>innerR>>outR>>z>>name;
    getline(inFile,line);

    fMdcSegment[i].SetLength(length); fMdcSegment[i].SetInnerR(innerR);
    fMdcSegment[i].SetOutR(outR); fMdcSegment[i].SetZ(z);
    fMdcSegment[i].SetName(name);
    }

}

 // get BesMdcGeoParameter from MdcGeomSvc
void BesMdcGeoParameter::InitFromSvc() {
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    IMdcGeomSvc* ISvc;
    MdcGeomSvc* mdcGeomSvc;
    StatusCode sc=svcLocator->service("MdcGeomSvc", ISvc);
    mdcGeomSvc=dynamic_cast<MdcGeomSvc *>(ISvc);
    if (!sc.isSuccess()) 
    std::cout<<"BesMdcGeoParameter::Could not open Geometry Service"<<std::endl;

    fLayerNo= mdcGeomSvc->Misc()->LayerNo();
    fWireNo=mdcGeomSvc->Misc()->WireNo();
    fSignalLayerNo=mdcGeomSvc->Misc()->SLayerNo();
    fSignalWireR=mdcGeomSvc->Misc()->SWireR();
    fFieldWireR=mdcGeomSvc->Misc()->FWireR();
   
    int i,signalLayer;
    for(i=0; i<fSignalLayerNo; i++){
      signalLayer=mdcGeomSvc->Layer(i)->SLayer();  
      fSignalLayer[i]=signalLayer-1;
     }
     
    string name;
    int wireNo,firstWire;
    double length, r, phi,rotateCell,rotateAngle;
    for(i=0;i<fLayerNo;i++){    
      name=mdcGeomSvc->GeneralLayer(i)->LayerName();   
      wireNo=mdcGeomSvc->GeneralLayer(i)->NCell()*2;
      length= mdcGeomSvc->GeneralLayer(i)->Length();   
      r= mdcGeomSvc->GeneralLayer(i)->Radius();
      phi=mdcGeomSvc->GeneralLayer(i)->nomPhi();
      firstWire=mdcGeomSvc->GeneralLayer(i)->First();
      rotateCell= mdcGeomSvc->GeneralLayer(i)->nomShift();
      
      rotateAngle=2*pi*rotateCell/wireNo;

      fLayer[i].SetName(name);fLayer[i].SetRadius(r);             
      fLayer[i].SetLength(length); fLayer[i].SetRotateCell(rotateCell);
      fLayer[i].SetRotateAngle(rotateAngle); fLayer[i].SetWireNo(wireNo); 
      fLayer[i].SetShiftPhi(twopi/wireNo); fLayer[i].SetFirstWire(firstWire);
      fLayer[i].SetPhi(phi);

     if(i==0){
       fLayer[i].SetSumWireNo(wireNo); fLayer[i].SetBeginWireNo(0);
     }else{
       fLayer[i].SetBeginWireNo(fLayer[i-1].SumWireNo());
       fLayer[i].SetSumWireNo(fLayer[i-1].SumWireNo()+wireNo);
     }

    for(int j=0; j<wireNo; j++){
      fWirePhi[fLayer[i].BeginWireNo()+j]=j*fLayer[i].ShiftPhi()+phi;
      }
  }

  if(fLayer[fLayerNo-1].SumWireNo()!= fWireNo){
    cout<<"Total wire number is not consistant!"<<endl;
  }
  

  double innerR,outR,z;
  fSegmentNo=mdcGeomSvc->getSegmentNo();
  for(i=0;i<fSegmentNo;i++){    
    length=mdcGeomSvc->End(i)->Length();
    innerR=mdcGeomSvc->End(i)->InnerR();
    outR=mdcGeomSvc->End(i)->OutR();
    z=mdcGeomSvc->End(i)->Z();
    name=mdcGeomSvc->End(i)->Name();

    fMdcSegment[i].SetLength(length); fMdcSegment[i].SetInnerR(innerR);
    fMdcSegment[i].SetOutR(outR); fMdcSegment[i].SetZ(z);
    fMdcSegment[i].SetName(name);
    }

}

void BesMdcGeoParameter::Dump() {
   //cout<<"------BesMdcGeoParameter info :--------"<<endl;
    cout<<" fLayerNo: "<<fLayerNo<<endl;
    cout<<" fWireNo: "<<fWireNo<<endl;
    cout<<" fSignalLayerNo: "<<fSignalLayerNo<<endl;
    cout<<" fSignalWireR: "<<fSignalWireR<<endl;
    cout<<" fFieldWireR: "<<fFieldWireR<<endl;
    
    cout<<"fSingalLayer:"<<endl;
    for(int i=0; i<fSignalLayerNo; i++){
      cout<<fSignalLayer[i]+1<<' '; }
    cout<<endl;
   
    for(int i=0;i<fLayerNo;i++){
      cout<<"Layer["<<i<<"]: "
          <<" name:"<<fLayer[i].Name() <<"  wireNo:"<<fLayer[i].WireNo()
          <<"  length: "<<fLayer[i].Length() <<"  r: "<<fLayer[i].R();
      if (i<75)  cout<<"  phi:"<<fLayer[i].Phi()*180/pi;
      else cout<<"  phi:"<<(fLayer[i].Phi()-fLayer[i].ShiftPhi())*180/pi;
      cout<<"  firstWire: "<<fLayer[i].FirstWire()
          <<"  rotateCell: "<<fLayer[i].RotateCell()<<endl;
    }    

    cout<<"fSegmentNo:"<<fSegmentNo<<endl;
    for(int j=0;j<fSegmentNo;j++){
      cout<<"length:"<<fMdcSegment[j].Length()
          <<"  innerR:"<<fMdcSegment[j].InnerR()
          <<"  outR:"<<fMdcSegment[j].OutR()
          <<"  z:"<<fMdcSegment[j].Z()
          <<"  name:"<<fMdcSegment[j].Name()<<endl;
   }

}

