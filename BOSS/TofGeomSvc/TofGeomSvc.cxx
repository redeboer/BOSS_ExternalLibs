#include "TofGeomSvc/TofGeomSvc.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"

  
TofGeomSvc::TofGeomSvc( const std::string& name, ISvcLocator* svcloc ) : Service(name, svcloc) {}

StatusCode TofGeomSvc::queryInterface (const InterfaceID& riid, void** ppvInterface ){
  
  if ( IID_ITofGeomSvc.versionMatch(riid) ) { 
    *ppvInterface = static_cast<ITofGeomSvc*> (this); 
  } else { 
    return Service::queryInterface(riid, ppvInterface) ; 
  }
  return StatusCode::SUCCESS;
}

StatusCode TofGeomSvc::initialize ( ) {
  MsgStream log(messageService(), name());
  log << MSG::INFO << name() << ": Start of run initialisation" << endreq;
  
  StatusCode sc = Service::initialize();
  if ( sc.isFailure() ) return sc;

  //  get geometry data
 // Fill();    
  return StatusCode::SUCCESS;
}  
  
StatusCode TofGeomSvc::finalize ( ) {
  MsgStream log(messageService(), name());
  log << MSG::INFO << name() << ": End of Run" << endreq;
  return StatusCode::SUCCESS;
}

TofGeomSvc::~TofGeomSvc(){
  for(vector<BTofGeo*>::iterator it1 = fBTofGeo.begin(); it1 != fBTofGeo.end(); it1++) delete *it1;
  for(vector<ETofGeo*>::iterator it2 = fETofGeo.begin(); it2 != fETofGeo.end(); it2++) delete *it2;  
  fBTofGeo.clear();
  fETofGeo.clear();
}
/*
void TofGeomSvc::Fill(){       	
    DB2BesGeoBarTOF obj1;
    DB2BesGeoEndTOF obj2;
    vector<DBBesGeoBarTOF> VecA;
    vector<DBBesGeoEndTOF> VecB;
    obj1.get_DBBesGeoBarTOF(VecA);
    obj2.get_DBBesGeoEndTOF(VecB);
    double inradm = VecA[0].get_inrad();
    double length = VecA[0].get_length();
    double thick  = VecA[0].get_thick();
   for(int ib=0;ib<176;ib++){
        BTofGeo* btof= new BTofGeo;
        double phi=0,phiMin=0,phiMax=0,inrad=0;
        double dphi = 0.0714;
	if(ib<88){
	   inrad=inradm;	
           phi  = dphi/2.+ib*dphi;      //phi at center of the module
           phiMin = phi-dphi/2.;
           phiMax = phi+dphi/2.;
	}
	else if(ib>87) {
	   int ibb=ib-88;
           inrad=inradm+5.1;	   
           phi    = ibb*dphi;      //phi at center of the module
           phiMin = phi-dphi/2.;
           phiMax = phi+dphi/2.;
	}	   	
	btof->setPhiMax(phiMax);
	btof->setPhiMin(phiMin);
        btof->setInrad(inrad);
        btof->setLength(length);
	btof->setThick(thick);          	
        fBTofGeo.push_back(btof);
	
    }   
    for(int ie=0;ie<96;ie++){
     double inrad_e  = VecB[0].get_inrad();
    double outrad_e = VecB[0].get_outrad();        
    double thick_e  = VecB[0].get_thick();	      
        double phi_e=0,phiMin_e=0,phiMax_e=0,thetaMax_e=0,thetaMin_e=0;
        ETofGeo* etof=new ETofGeo;
        double dphi = 0.1309;
	if(ie<48){
           phi_e  = dphi/2.+ie*dphi;      //phi at center of the counter
           phiMin_e = phi_e-dphi/2.;
           phiMax_e = phi_e+dphi/2.;
	   thetaMax_e=0.93;
	   thetaMin_e=0.83;
	}else{
	   int ie2=ie-48;
           phi_e  = dphi/2.+ie2*dphi;      //phi at center of the counter
           phiMin_e = phi_e-dphi/2.;
           phiMax_e = phi_e+dphi/2.;
	   thetaMax_e=-0.83;
	   thetaMin_e=-0.93;
	}	
	etof->setPhiMin(phiMin_e);
	etof->setPhiMax(phiMax_e);
	etof->setThetaMax(thetaMax_e);
	etof->setThetaMin(thetaMin_e);
	etof->setInrad(inrad_e);
	etof->setOutrad(outrad_e);
	etof->setThick(thick_e);
        fETofGeo.push_back(etof);	
    }    
}
*/


void TofGeomSvc::Dump(){
  std::cout<<"Now can get the TOF Geometry Service!!"<<std::endl;
/*
  std::cout<<"Barrel TOF Counter Number = "<<fBTofGeo.size()<<std::endl;
  std::cout<<"Endcap TOF Counter Number = "<<fETofGeo.size()<<std::endl;
  for(int ib=0; ib<176; ib++){  
     cout<<"The"<<" "<<ib<<" "<<"Barrel TOF phiMax is"<<" "<<BTof(ib)->getPhiMax()<<endl;
     cout<<"The"<<" "<<ib<<" "<<"Barrel TOF phiMin is"<<" "<<BTof(ib)->getPhiMin()<<endl; 
  }
  for(int ie=0; ie<96; ie++){  
     cout<<"The"<<" "<<ie<<" "<<"Endcap TOF phiMax is"<<" "<<ETof(ie)->getPhiMax()<<endl;
     cout<<"The"<<" "<<ie<<" "<<"Endcap TOF phiMin is"<<" "<<ETof(ie)->getPhiMin()<<endl; 
  }  
  cout<<"The 33rd  Barrel TOF Counter  inrad is"<<" "<<BTof(33)->getInrad()<<endl; 
  cout<<"The 133rd Barrel TOF Counter  inrad is"<<" "<<BTof(133)->getInrad()<<endl; 
  cout<<"The 133rd Barrel TOF Counter  length is"<<" "<<BTof(133)->getLength()<<endl; 
  cout<<"The 133rd Barrel TOF Counter  thick  is"<<" "<<BTof(133)->getThick()<<endl; 
  cout<<"The 22rd  Endcap TOF Counter  inrad is"<<" "<<ETof(22)->getInrad()<<endl; 
  cout<<"The 22rd  Endcap TOF Counter  outrad is"<<" "<<ETof(22)->getOutrad()<<endl; 
  cout<<"The 22rd  Endcap TOF Counter  thick  is"<<" "<<ETof(22)->getThick()<<endl; 
  cout<<"The 22rd  Endcap TOF Counter  thetaMax is"<<" "<<ETof(22)->getThetaMax()<<endl; 
  cout<<"The 22rd  Endcap TOF Counter  thetaMin is"<<" "<<ETof(22)->getThetaMin()<<endl; 
  cout<<"The 90rd  Endcap TOF Counter  thetaMax is"<<" "<<ETof(90)->getThetaMax()<<endl; 
  cout<<"The 90rd  Endcap TOF Counter  thetaMin is"<<" "<<ETof(90)->getThetaMin()<<endl; 
*/
}

const double  TofGeomSvc::getBPhiMax(unsigned id){
  if (id < fBTofGeo.size())
        return  fBTofGeo[id]->getPhiMax();
    return 0;
}

const double  TofGeomSvc::getBPhiMin(unsigned id){
  if (id < fBTofGeo.size())
        return  fBTofGeo[id]->getPhiMin();
    return 0;
}

const double  TofGeomSvc::getEPhiMax(unsigned id){
  if (id < fETofGeo.size())
        return  fETofGeo[id]->getPhiMax();
    return 0;
}

const double  TofGeomSvc::getEPhiMin(unsigned id){
  if (id < fETofGeo.size())
        return  fETofGeo[id]->getPhiMin();
    return 0;
}
	
BTofGeo* TofGeomSvc::BTof(unsigned id) const{
    if (id < fBTofGeo.size())
        return  fBTofGeo[id];
    return 0;
}

ETofGeo* TofGeomSvc::ETof(unsigned id) const{
    if (id < fETofGeo.size())
        return  fETofGeo[id];
    return 0;
}
