#include "MdcUtilitySvc/MdcUtilitySvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "EventModel/EventHeader.h"
#include "MdcGeom/Constants.h"
#include "MdcGeom/BesAngle.h"
#include "TrkBase/HelixTraj.h"
#include "TrkBase/TrkPoca.h"
#include "MdcGeom/MdcLayer.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include <cmath>
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatblty wll be enabled ONLY n CLHEP 1.9
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

using namespace std;
using namespace Event;

MdcUtilitySvc::MdcUtilitySvc( const string& name, ISvcLocator* svcloc) :
  Service (name, svcloc) {
    declareProperty("debug", 		m_debug = 0);
  }

MdcUtilitySvc::~MdcUtilitySvc(){
}

StatusCode MdcUtilitySvc::initialize(){
  MsgStream log(messageService(), name());
  log << MSG::INFO << "MdcUtilitySvc::initialize()" << endreq;

  StatusCode sc = Service::initialize();
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Service::initialize() failure" << endreq;
    return sc;
  }

  //Initalze magnetic flied 
  sc = service("MagneticFieldSvc", m_pIMF);
  if(! m_pIMF){
    log << MSG::FATAL <<" ERROR Unable to open Magnetic field service "<< endreq; 
    return StatusCode::FAILURE;
  }

  // Initialize MdcGeomSvc
  sc = service("MdcGeomSvc", m_mdcGeomSvc);
  if(! m_mdcGeomSvc){
    log << MSG::FATAL <<" Could not load MdcGeomSvc! "<< endreq; 
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode MdcUtilitySvc::finalize(){
  MsgStream log(messageService(), name());
  log << MSG::INFO << "MdcUtilitySvc::finalize()" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode MdcUtilitySvc::queryInterface(const InterfaceID& riid, void** ppvInterface){

  if( IID_IMdcUtilitySvc.versionMatch(riid) ){
    *ppvInterface = static_cast<IMdcUtilitySvc*> (this);
  } else{
    return Service::queryInterface(riid, ppvInterface);
  }

  addRef();
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
int MdcUtilitySvc::nLayerTrackPassed(const double helixBes[5]) const{

  HepVector helixBesParam(5,0);
  for(int i=0; i<5; ++i) helixBesParam[i] = helixBes[i];

  return nLayerTrackPassed(helixBesParam);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
int MdcUtilitySvc::nLayerTrackPassed(const HepVector helixBes) const{
  HepVector helix= besPar2PatPar(helixBes);

  int nLayer = 0;

  for(unsigned iLayer=0; iLayer<43; iLayer++){
    //flightLength is the path length of track in the x-y plane
    //guess flightLength by the radius in middle of the wire.
    double rMidLayer = m_mdcGeomSvc->Layer(iLayer)->Radius();
    double flightLength = rMidLayer;

    HepPoint3D pivot(0.,0.,0.);
    double dz = helix[3];
    double c = CLHEP::c_light * 100.; //unit from m/s to cm/s
    double alpha = 1/(c * Bz());//~333.567
    double kappa = helix[2];
    double rc = (-1.)*alpha/kappa;
    //std::cout<<"MdcUtilitySvc alpha   "<<alpha<<std::endl;
    double tanl = helix[4];
    double phi0 = helix[1];
    double phi = flightLength/rc + phi0;//turning angle
    double z = pivot.z() + dz - (alpha/kappa) * tanl * phi;

    double layerHalfLength = m_mdcGeomSvc->Layer(iLayer)->Length()/2.;

    //std::cout<<"MdcUtilitySvc length  "<<layerHalfLength<<std::endl;

    if (fabs(z) < fabs(layerHalfLength)) ++nLayer;
  }

  return nLayer;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
HepVector MdcUtilitySvc::patPar2BesPar(const HepVector& helixPar) const{
  HepVector helix(5,0);
  double d0 = -helixPar[0];    //cm
  double phi0 = helixPar[1]+ CLHEP::halfpi;
  double omega = Bz()*helixPar[2]/-333.567;
  double z0 = helixPar[3];    //cm
  double tanl = helixPar[4];
  helix[0] = d0;
  helix[1] = phi0;
  helix[2] = omega;
  helix[3] = z0;
  helix[4] = tanl;
  //std::cout<<"helix   "<<helix<<std::endl;
  return helix;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
HepSymMatrix MdcUtilitySvc::patErr2BesErr(const HepSymMatrix& err) const{
  //error matrix
  //std::cout<<" err1  "<<err<<" "<<err.num_row()<<std::endl;
  //V(Y)=S * V(X) * ST , mS = S , mVy = V(Y) , err() = V(X)
  //int n = err.num_row();
  HepSymMatrix mS(err.num_row(),0);
  mS[0][0]=-1.;//dr0=-d0
  mS[1][1]=1.;

  mS[2][2]=Bz()/-333.567;//pxy -> cpa
  mS[3][3]=1.;
  mS[4][4]=1.;
  HepSymMatrix mVy= err.similarity(mS);
  //std::cout<<" err2  "<<n<<" "<<mVy<<std::endl;
  return mVy;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//-------position of track pass this layer--------------
HepPoint3D MdcUtilitySvc::pointOnHelix(const HepVector helixBes, int layer, int innerOrOuter) const{
  HepVector helixPat= besPar2PatPar(helixBes);
  return pointOnHelixPatPar(helixPat, layer, innerOrOuter);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//-------position of track pass this layer--------------
HepPoint3D MdcUtilitySvc::pointOnHelixPatPar(const HepVector helixPat, int layer, int innerOrOuter) const{

  double rInner,rOuter;
  //retrieve Mdc geometry information
  double rCize1 =0.1* m_mdcGeomSvc->Layer(layer)->RCSiz1();  //mm -> cm 
  double rCize2 =0.1* m_mdcGeomSvc->Layer(layer)->RCSiz2();  //mm -> cm
  double rLay   =0.1* m_mdcGeomSvc->Layer(layer)->Radius();  //mm -> cm

  //(conversion of the units mm(mc)->cm(rec))
  rInner = rLay - rCize1 ; //radius of inner field wire
  rOuter = rLay + rCize2 ; //radius of outer field wire

  //int sign = -1; //assumed the first half circle
  HepPoint3D piv(0.,0.,0.);
  double r;
  if (innerOrOuter) r = rInner;
  else r = rOuter;

  double d0 = helixPat[0];
  double phi0 = helixPat[1];
  double omega = helixPat[2];
  double z0 = helixPat[3];
  double tanl = helixPat[4];

  double rc; 
  if (abs(omega) <Constants::epsilon) rc = 9999.;
  else rc = 1./omega;
  double xc     = piv.x() + ( d0 + rc) * cos(phi0);
  double yc     = piv.y() + ( d0 + rc) * sin(phi0);
  HepPoint3D helixCenter(xc,yc,0.0);
  rc = sqrt(xc*xc + yc*yc);//?
  double a,b,c;
  a = r;
  b = d0 + rc;
  c = rc;
  double dphi = acos((a*a-b*b-c*c)/(-2.*b*c)); 
  double fltlen = dphi * rc;
  double phi = phi0 * omega * fltlen;
  double x = piv.x()+d0*sin(phi) - (rc+d0)*sin(phi0);
  double y = piv.y()+d0*cos(phi) + (rc+d0)*cos(phi0);
  double z = piv.z()+ z0 + fltlen*tanl;
  //std::cout<<" xc yc "<<xc<<" "<<yc
  if(m_debug) {
    std::cout<<" abc "<<a<<" "<<b<<" "<<c<<" omega "<<omega<<" r "<<r<<" rc "<<rc<<" dphi "<<dphi<<" piv  "<<piv.z()
      <<" z0  "<<z0<<" fltlen  "<<fltlen<<" tanl "<<tanl<<std::endl;
    std::cout<<" pointOnHelixPatPar in Hel  "<<helixPat<<std::endl;
    cout<<"HepPoint3D(x, y, z) = "<<HepPoint3D(x, y, z)<<endl;
  }
  return HepPoint3D(x, y, z);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//1. from five track parameter and layer,cell to calculate the poca postion
double MdcUtilitySvc::doca(int layer, int cell, const HepVector helixBes, const HepSymMatrix errMatBes, bool passCellRequired, bool doSag) const{
  HepVector helixPat = besPar2PatPar(helixBes);
  HepSymMatrix errMatPat= besErr2PatErr(errMatBes);

  return docaPatPar(layer, cell, helixPat, errMatPat, passCellRequired, doSag);//call 4.
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//2. from five track parameter, layer, cell , eastP, westP to calculate the poca postion
double MdcUtilitySvc::doca(int layer, int cell,HepPoint3D eastP,HepPoint3D westP, const HepVector helixBes, const HepSymMatrix errMatBes, bool passCellRequired, bool doSag) const{
  HepVector helixPat = besPar2PatPar(helixBes);
  HepSymMatrix errMatPat= besErr2PatErr(errMatBes);

  return docaPatPar(layer, cell, eastP, westP, helixPat, errMatPat, passCellRequired, doSag);//call 5.
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//3. from five track parameter, layer, cell, sWire to calculate the doca postion
double MdcUtilitySvc::doca(int layer, int cell, const MdcSWire* sWire, const HepVector helixBes,const HepSymMatrix errMatBes, bool passCellRequired) const{
  HepVector helixPat = besPar2PatPar(helixBes);
  HepSymMatrix errMatPat= besErr2PatErr(errMatBes);

  return docaPatPar(layer, cell, sWire, helixPat, errMatPat, passCellRequired);//call 6.
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//4. from five track parameter to calculate the doca postion by Pat Param
double MdcUtilitySvc::docaPatPar(int layer, int cell, const HepVector helixPat, const HepSymMatrix errMat, bool passCellRequired, bool doSag) const{

  const MdcGeoWire *geowir = m_mdcGeomSvc->Wire(layer,cell);
  int id = geowir->Id();
  double sag = 0.;
  if(doSag) sag = geowir->Sag()/10.;//mm2cm
  HepPoint3D eastP = geowir->Backward()/10.0;//mm2cm
  HepPoint3D westP = geowir->Forward() /10.0;//mm2cm
  const MdcSWire* sWire = new MdcSWire(eastP, westP, sag, id , cell);

  double doca = docaPatPar(layer, cell, sWire, helixPat, errMat, passCellRequired);//call 6.

  delete sWire;
  return doca;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//5. from five track parameter to calculate the doca postion by Pat Param
double MdcUtilitySvc::docaPatPar(int layer, int cell,HepPoint3D eastP,HepPoint3D westP, const HepVector helixPat,const HepSymMatrix errMat, bool passCellRequired, bool doSag) const{

  const MdcGeoWire *geowir = m_mdcGeomSvc->Wire(layer,cell);
  int id = geowir->Id();
  double sag = 0.;
  if(doSag) sag = geowir->Sag()/10.;//mm2cm
  const MdcSWire* sWire = new MdcSWire(eastP, westP, sag, id , cell);//cm

  double doca = docaPatPar(layer, cell, sWire, helixPat, errMat, passCellRequired);//call 6.

  delete sWire;
  return doca;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//6. from five track parameter to calculate the doca postion by Pat Param
double MdcUtilitySvc::docaPatPar(int layer, int cell, const MdcSWire* sWire, const HepVector helixPat,const HepSymMatrix errMat, bool passCellRequired) const{

  if(m_debug) std::cout<<" helixPat  "<<helixPat<<std::endl;
  if(m_debug) std::cout<<" layer "<<layer<<" cell "<<cell<<std::endl;
  //-----cell track passed
  int cellId_in  = -1;
  int cellId_out = -1;

  //cellTrackPassedPatPar(helixPat,layer,cellId_in,cellId_out);//yzhang FIXME 2012-07-11 
  cellTrackPassedByPhiPatPar(helixPat,layer,cellId_in,cellId_out);//yzhang FIXME 2012-07-11 

  if(m_debug) {
    std::cout<<" cellId in  "<<cellId_in<<" out "<<cellId_out <<std::endl;
    cout << "cell = " << cell << ", cellId_in = " << cellId_in << ", cellId_out = " << cellId_out << endl;
  }
  if (passCellRequired &&(cell < cellId_in && cell > cellId_out)) return -999.;

  //-----helix trajectory
  HelixTraj* helixTraj = new HelixTraj(helixPat,errMat);
  const Trajectory* trajHelix    = dynamic_cast<const Trajectory*> (helixTraj);

  //-----pointOnHelix
  int innerOrOuter = 1;
  Hep3Vector cell_IO = pointOnHelixPatPar(helixPat,layer,innerOrOuter);
  double fltTrack = 0.1 * m_mdcGeomSvc -> Layer(layer)->Radius();
  if(m_debug) {
    std::cout<<" cell_IO "<<cell_IO<<std::endl;
    std::cout<<" fltTrack "<<fltTrack<<std::endl;
  }

  //------wire trajectory
  const MdcSagTraj* m_wireTraj = sWire->getTraj();
  const Trajectory* trajWire  = dynamic_cast<const Trajectory*> (m_wireTraj);
  const HepPoint3D* start_In  = sWire->getEastPoint();
  //const HepPoint3D* stop_In   = sWire->getWestPoint();
  if(m_debug){
    std::cout<<" sag "<<m_wireTraj->sag()<<std::endl;
    std::cout<< " east -------- "<< start_In->x()<<","<<start_In->y()<<","<<start_In->z()<<std::endl;
  }
  //std::cout<< " west -------- "<< stop_In->x()<<","<<stop_In->y()<<","<<stop_In->z() <<std::endl;

  //------calc poca
  double doca  = -999.;
  TrkPoca* trkPoca;
  double zWire = cell_IO.z();
  //HepPoint3D pos_in(zWire,sWire->xWireDC(zWire),sWire->yWireDC(zWire)) ;
  HepPoint3D pos_in(sWire->xWireDC(zWire),sWire->yWireDC(zWire),zWire) ;
  if(m_debug) std::cout<<" zWire  "<<zWire<<" zEndDC "<<sWire->zEndDC()<<std::endl;
  //if(m_debug) std::cout<<"pos_in "<<pos_in<<" fltWire  "<<fltWire<<std::endl;

  double fltWire = sqrt( (pos_in.x()-start_In->x())*(pos_in.x()-start_In->x()) +
      (pos_in.y()-start_In->y())*(pos_in.y()-start_In->y()) +
      (pos_in.z()-start_In->z())*(pos_in.z()-start_In->z())  );
  trkPoca  = new TrkPoca(*trajHelix, fltTrack, *trajWire, fltWire);
  doca = trkPoca->doca();

  double hitLen   = trkPoca->flt2();
  double startLen = trajWire->lowRange() - 5.;
  double endLen   = trajWire->hiRange()  + 5.;
  if(hitLen < startLen || hitLen > endLen) {
    if(m_debug) std::cout<<"WARNING MdcUtilitySvc::docaPatPar()  isBeyondEndflange! hitLen="<<hitLen <<" startLen="<<startLen<<" endLen "<<endLen<<std::endl;
    doca = -998.;
  }
  //std::cout<<" hitLen  "<<hitLen<<" startLen  "<<startLen<<" endLen  "<<endLen <<" doca "<<doca<<std::endl;

  if(m_debug) std::cout<<" doca  "<<doca<<std::endl;
  delete helixTraj;
  delete trkPoca;

  return doca;

} //----------end of calculatng the doca ---------------------------------//

/*
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
void MdcUtilitySvc::cellPassed(const RecMdcTrack* tk, bool cellTkPassed[43][288]) const{
HepVector helix(5);
helix[0]=tk->helix(0);
helix[1]=tk->helix(1);
helix[2]=tk->helix(2);
helix[3]=tk->helix(3);
helix[4]=tk->helix(4);

for(int i=0;i<43;i++){
for(int j=0; j<288; j++){
cellTkPassed[i][j]=false;
}
}

for (int layer=0; layer<43; layer++){
//-----cell track passed

int cellId_in  = -1;
int cellId_out = -1;
cellTrackPassed(helix,layer,cellId_in,cellId_out);
cellTkPassed[layer][cellId_in] = true;
cellTkPassed[layer][cellId_out] = true;
}
}
*/
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
bool MdcUtilitySvc::cellTrackPassedByPhi(const HepVector helixBes,int layer,int& cellId_in,int& cellId_out) const{
  HepVector helixPat = besPar2PatPar(helixBes);
  return cellTrackPassedByPhiPatPar(helixPat, layer, cellId_in, cellId_out);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// guess cell number of track passed given layer, 
// if passed more than one cell return 0, else return 1.
// calc with phi , PAT track parameter convention
bool MdcUtilitySvc::cellTrackPassedByPhiPatPar(const HepVector helixPat,int layer,int& cellId_in,int& cellId_out) const{
  int nCell = m_mdcGeomSvc->Layer(layer)->NCell();

  double dPhiz = (m_mdcGeomSvc->Wire(layer,0)->Forward().phi() - m_mdcGeomSvc->Wire(layer,0)->Backward().phi())*0.5;
  double rEnd = m_mdcGeomSvc->Wire(layer,0)->Backward().rho()/10.;//mm2cm 
  double rMid = rEnd * cos(dPhiz);
  //std::cout<<"( cell "<<0<<" westPphi "<<m_mdcGeomSvc->Wire(layer,0)->Forward().phi() <<" eastPphi "
  //<<m_mdcGeomSvc->Wire(layer,0)->Backward().phi()<<" twist  "<<dPhiz<<" rend "<<rEnd<<std::endl;
  double fltLenIn = rMid;
  double phiIn = helixPat[1] + helixPat[2]*fltLenIn;//phi0 + omega * L

  double phiEPOffset = m_mdcGeomSvc->Wire(layer,0)->Backward().phi();//east.phi()= BackWard.phi
  BesAngle tmp(phiIn - phiEPOffset);
  if(m_debug) std::cout<<"cellTrackPassed  nCell  "<<nCell<<" layer "<<layer<<" fltLenIn  "<<fltLenIn<<" phiEPOffset "<<phiEPOffset<<std::endl;
  //BesAngle tmp(phiIn - layPtr->phiEPOffset());
  int wlow = (int)floor(nCell * tmp.rad() / twopi );
  int wbig = (int)ceil(nCell * tmp.rad() / twopi );
  if (tmp == 0 ){ wlow = -1; wbig = 1; }

  if ((wlow%nCell)< 0) wlow += nCell;
  cellId_in  = wlow;

  if ((wbig%nCell)< 0) wbig += nCell;
  cellId_out = wbig;

  bool passedOneCell = (cellId_in == cellId_out);

  return passedOneCell;//pass more than one cell
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
bool MdcUtilitySvc::cellTrackPassedPatPar(HepVector helixPat, int layer,int& cellId_in,int& cellId_out) const{
  HepVector helixBes = patPar2BesPar(helixPat);
  return cellTrackPassed(helixBes, layer, cellId_in, cellId_out);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// calc with Belle method, bes3 track parameter convention
bool MdcUtilitySvc::cellTrackPassed(HepVector helixBes, int lay,int& cellId_in,int& cellId_out) const{
  int charge,type,nCell;
  double dr0,phi0,kappa,dz0,tanl;
  double ALPHA_loc,rho,phi,cosphi0,sinphi0,x_hc,y_hc,z_hc;
  double dphi0,IO_phi,C_alpha,xx,yy;
  double inlow,inup,outlow,outup,phi_in,phi_out,phi_bin;
  double rCize1,rCize2,rLay,length,phioffset,slant,shift; 
  double m_crio[2], phi_io[2], stphi[2],phioff[2],dphi[2];

  dr0   = helixBes[0];  
  phi0  = helixBes[1];
  kappa = helixBes[2];
  dz0   = helixBes[3];
  tanl  = helixBes[4];

  ALPHA_loc = 1000/(2.99792458*Bz()); //magnetic field const
  charge    = ( kappa >=0 ) ? 1 : -1 ;
  rho       = ALPHA_loc/kappa ;
  double pi = Constants::pi;
  phi      = fmod(phi0 + 4*pi , 2*pi);
  cosphi0  = cos(phi);
  sinphi0  = (1.0 - cosphi0 ) * (1.0 + cosphi0 );
  sinphi0  = sqrt(( sinphi0 > 0.) ? sinphi0 : 0.);
  if( phi > pi ) sinphi0 = -sinphi0 ;

  x_hc     = 0. + ( dr0 + rho ) * cosphi0;
  y_hc     = 0. + ( dr0 + rho ) * sinphi0;
  z_hc     = 0. + dz0;


  HepPoint3D piv(0.,0.,0.); 
  HepPoint3D hcenter(x_hc,y_hc,0.0);

  double m_c_perp(hcenter.perp());
  Hep3Vector m_c_unit((HepPoint3D)hcenter.unit());
  HepPoint3D IO = (-1) * charge * m_c_unit;

  dphi0  = fmod(IO.phi()+4*pi, 2*pi) - phi;
  IO_phi = fmod(IO.phi()+4*pi, 2*pi);

  if(dphi0 > pi) dphi0 -= 2*pi;
  else if(dphi0 < -pi) dphi0 += 2*pi; 

  phi_io[0] = -(1+charge)*pi/2 - charge*dphi0;
  phi_io[1] = phi_io[0]+1.5*pi;


  bool outFlag = false;
  //retrieve Mdc geometry information
  rCize1 = 0.1 * m_mdcGeomSvc->Layer(lay)->RCSiz1();  //mm -> cm 
  rCize2 = 0.1 * m_mdcGeomSvc->Layer(lay)->RCSiz2();  //mm -> cm
  rLay   = 0.1 * m_mdcGeomSvc->Layer(lay)->Radius();  //mm -> cm
  length = 0.1 * m_mdcGeomSvc->Layer(lay)->Length();  //mm -> cm
  //double halfLength=length/2.;
  //(conversion of the units mm(mc)->cm(rec))
  nCell  = m_mdcGeomSvc->Layer(lay)->NCell();
  phioffset = m_mdcGeomSvc->Layer(lay)->Offset();
  slant  = m_mdcGeomSvc->Layer(lay)->Slant();
  shift  = m_mdcGeomSvc->Layer(lay)->Shift();
  type   = m_mdcGeomSvc->Layer(lay)->Sup()->Type();

  m_crio[0] = rLay - rCize1 ; //radius of inner field wir ( beam pipe)
  m_crio[1] = rLay + rCize2 ; //radius of outer field wir ( beam pipe)

  int sign = -1; //assumed the first half circle

  Hep3Vector iocand[2];
  Hep3Vector cell_IO[2];

  for(int ii =0; ii<2; ii++){
    // By law of cosines to calculate the alpha(up and down field wir_Ge)
    double cos_alpha = (m_c_perp*m_c_perp + m_crio[ii]*m_crio[ii] - rho*rho)
      / ( 2 * m_c_perp * m_crio[ii] ); 
    if(fabs(cos_alpha)>1&&(ii==0)){
      cos_alpha = -1;
      outFlag=true;
    }
    if(fabs(cos_alpha)>1&&(ii==1)){
      cos_alpha = (m_c_perp*m_c_perp + m_crio[0]*m_crio[0] - rho*rho)
	/ ( 2 * m_c_perp * m_crio[0] );
      C_alpha   = 2*pi - acos( cos_alpha);
    }else {
      C_alpha   = acos( cos_alpha );
    }

    iocand[ii] = m_c_unit;
    iocand[ii].rotateZ( charge*sign*C_alpha );
    iocand[ii] *= m_crio[ii];

    xx = iocand[ii].x() - x_hc ;
    yy = iocand[ii].y() - y_hc ;

    dphi[ii] = atan2(yy,xx) - phi0 - 0.5*pi*(1-charge);
    dphi[ii] = fmod( dphi[ii] + 8.0*pi,2*pi);


    if( dphi[ii] < phi_io[0] ) {
      dphi[ii] += 2*pi;
    }else if( dphi[ii] > phi_io[1] ){  //very very nausea
      dphi[ii] -= 2*pi;
    }

    cell_IO[ii] = Hel(piv, dr0, phi, ALPHA_loc, kappa,dz0, dphi[ii], tanl); 

    //cout<<" cell_IO["<<ii<<"] : "<<cell_IO[ii]<<endl;
    if( (cell_IO[ii].x()==0 ) && (cell_IO[ii].y()==0) && (cell_IO[ii].z()==0)) continue;
  }
  //if(((fabs(cell_IO[0].z())*10-halfLength)>-7.) && ((fabs(cell_IO[1].z())*10-halfLength)>-7.))return true; //Out sensitive area

  cellId_in = cellId_out = -1 ;
  phi_in  = cell_IO[0].phi();
  phi_in = fmod ( phi_in + 4 * pi, 2 * pi );
  phi_out = cell_IO[1].phi();
  phi_out = fmod ( phi_out + 4 * pi, 2 * pi );
  phi_bin = 2.0 * pi / nCell ;

  //determine the in/out cell id
  stphi[0] = shift * phi_bin * (0.5 - cell_IO[0].z()/length);
  stphi[1] = shift * phi_bin * (0.5 - cell_IO[1].z()/length);
  //stphi[0],stphi[1] to position fo z axis ,the angle of deflxsion in x_Y

  phioff[0] = phioffset + stphi[0];
  phioff[1] = phioffset + stphi[1];

  for(int kk = 0; kk<nCell ; kk++){
    //for stereo layer
    inlow  = phioff[0] + phi_bin*kk - phi_bin*0.5;
    inlow  = fmod( inlow + 4.0 * pi , 2.0 * pi);
    inup   = phioff[0] + phi_bin*kk + phi_bin*0.5;
    inup   = fmod( inup  + 4.0 * pi , 2.0 * pi);
    outlow = phioff[1] + phi_bin*kk - phi_bin*0.5;
    outlow = fmod( outlow + 4.0 * pi ,2.0 * pi);
    outup  = phioff[1] + phi_bin*kk + phi_bin*0.5;
    outup  = fmod( outup + 4.0 * pi , 2.0 * pi);

    if((phi_in>=inlow && phi_in<=inup))   cellId_in = kk;
    if((phi_out>=outlow&&phi_out<outup))  cellId_out = kk;
    if(inlow > inup ){
      if((phi_in>=inlow&&phi_in<2.0*pi)||(phi_in>=0.0&&phi_in<inup)) cellId_in = kk;
    }
    if(outlow>outup){
      if((phi_out>=outlow&&phi_out<=2.0*pi)||(phi_out>=0.0&&phi_out<outup)) cellId_out = kk;
    }
  }//end of nCell loop	

  return (cellId_in==cellId_out);
}

HepPoint3D MdcUtilitySvc::Hel(HepPoint3D piv, double dr,double phi0,double Alpha_L,double kappa,double dz,double dphi,double tanl) const{
  double x = piv.x() + dr*cos(phi0) + (Alpha_L/kappa) * (cos(phi0) - cos(phi0+dphi));
  double y = piv.y() + dr*sin(phi0) + (Alpha_L/kappa) * (sin(phi0) - sin(phi0+dphi));
  double z = piv.z() + dz - (Alpha_L/kappa) * dphi * tanl;
  //cout<<"HepPoint3D(x, y, z) = "<<HepPoint3D(x, y, z)<<endl;
  if((x>-1000. && x<1000.) || (y >-1000. && y <1000. ) ||(z>-1000. && z<1000.)){
    return HepPoint3D(x, y, z);
  }else{
    return HepPoint3D(0,0,0);
  }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
HepVector MdcUtilitySvc::besPar2PatPar(const HepVector& helixPar) const{
  HepVector helix(5,0);
  double d0 = -helixPar[0];    //cm
  double phi0 = helixPar[1]+ CLHEP::halfpi;
  double omega = Bz()*helixPar[2]/-333.567;
  double z0 = helixPar[3];    //cm
  double tanl = helixPar[4];
  helix[0] = d0;
  helix[1] = phi0;
  helix[2] = omega;
  helix[3] = z0;
  helix[4] = tanl;
  return helix;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
HepSymMatrix MdcUtilitySvc::besErr2PatErr(const HepSymMatrix& err) const{
  //error matrix
  //std::cout<<" err1  "<<err<<" "<<err.num_row()<<std::endl;
  //V(Y)=S * V(X) * ST , mS = S , mVy = V(Y) , err() = V(X)
  //int n = err.num_row();
  HepSymMatrix mS(err.num_row(),0);
  mS[0][0]=-1.;//dr0=-d0
  mS[1][1]=1.;
  mS[2][2]=Bz()/-333.567;//pxy -> cpa
  mS[3][3]=1.;
  mS[4][4]=1.;
  HepSymMatrix mVy= err.similarity(mS);
  //std::cout<<" err2  "<<n<<" "<<mVy<<std::endl;
  return mVy;
}

double MdcUtilitySvc::p_cms(HepVector helix, int runNo, double mass) const{
  HepLorentzVector p4;
  p4.setPx(- sin(helix[1]) / fabs(helix[2]));
  p4.setPy(cos(helix[1]) / fabs(helix[2]));
  p4.setPz(helix[4] / fabs(helix[2]));
  double p3 = p4.mag();
  mass = 0.000511;
  p4.setE(sqrt(p3 * p3 + mass * mass));

  double ecm;
  if (runNo > 9815) {
    ecm = 3.097;
  }else{
    ecm = 3.68632;
  }
  double zboost = 0.0075;
  //HepLorentzVector psip(0.011 * 3.68632, 0, 0.0075, 3.68632);
  HepLorentzVector psip(0.011 * ecm, 0, zboost, ecm);
  //cout << setw(15) << ecm << setw(15) << zboost << endl;
  Hep3Vector boostv = psip.boostVector();

  //std::cout<<__FILE__<<" boostv "<<boostv<<  std::endl;
  p4.boost(- boostv);

  //std::cout<<__FILE__<<" p4 "<<p4<<  std::endl;
  double p_cms = p4.rho();
  //phicms = p4.phi();
  //p4.boost(boostv);

  return p_cms;
}

Hep3Vector MdcUtilitySvc::momentum(const RecMdcTrack* trk) const{
  // double dr   = trk->helix(0);
  double fi0  = trk->helix(1);
  double cpa  = trk->helix(2);
  double tanl = trk->helix(4);

  double pxy = 0.;
  if(cpa != 0) pxy = 1/fabs(cpa);

  double px = pxy * (-sin(fi0));
  double py = pxy * cos(fi0);
  double pz = pxy * tanl;

  Hep3Vector p;
  p.set(px, py, pz); // MeV/c
  return p;
}

double MdcUtilitySvc::probab(const int& ndof, const double& chisq) const{

  //constants
  double srtopi=2.0/sqrt(2.0*M_PI);
  double upl=100.0;

  double prob=0.0;
  if(ndof<=0) {return prob;}
  if(chisq<0.0) {return prob;}
  if(ndof<=60) {
    //full treatment
    if(chisq>upl) {return prob;}
    double sum=exp(-0.5*chisq);
    double term=sum;

    int m=ndof/2;
    if(2*m==ndof){
      if(m==1){return sum;}
      for(int i=2; i<=m;i++){
	term=0.5*term*chisq/(i-1);
	sum+=term;
      }//(int i=2; i<=m)
      return sum;
      //even

    }else{
      //odd
      double srty=sqrt(chisq);
      double temp=srty/M_SQRT2;
      prob=erfc(temp);
      if(ndof==1) {return prob;}
      if(ndof==3) {return (srtopi*srty*sum+prob);}
      m=m-1;
      for(int i=1; i<=m; i++){
	term=term*chisq/(2*i+1);
	sum+=term;
      }//(int i=1; i<=m; i++)
      return (srtopi*srty*sum+prob);

    }//(2*m==ndof)

  }else{
    //asymtotic Gaussian approx
    double srty=sqrt(chisq)-sqrt(ndof-0.5);
    if(srty<12.0) {prob=0.5*erfc(srty);};

  }//ndof<30

  return prob;
}//endof probab

