#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"
//#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "MagneticField/IMagneticFieldSvc.h"
#include "MagneticField/MagFieldReader.h"
#include <fstream>
#include <iostream>
#include "CLHEP/Random/RandFlat.h"
using namespace std;
//Test with root
#include <TGraph.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TGraph2D.h>
#include <TStyle.h>
#include "TAxis.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatibility will be enabled ONLY in CLHEP 1.9
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Vector3D<double> HepVector3D;
#endif
using namespace CLHEP;
//-----------------------------------------------------------------------------
// Implementation file for class : MagFieldReader
//
//-----------------------------------------------------------------------------
// Declaration of the Algorithm Factory
//static const  AlgFactory<MagFieldReader>          s_factory ;
//const        IAlgFactory& MagFieldReaderFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MagFieldReader::MagFieldReader( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
  , m_pIMF(0) 
{
  declareProperty("Zmin", m_zMin =  -1200.0*mm);
  declareProperty("Zmax", m_zMax = 1200.0*mm);  
  declareProperty("Step", m_step =   50.0*mm);  
  declareProperty("Xmin", m_xMin =  -900.0*mm);  
  declareProperty("Xmax", m_xMax =  900.0*mm);
  declareProperty("Ymin", m_yMin =  -900.0*mm);  
  declareProperty("Ymax", m_yMax =  900.0*mm);
  declareProperty("filename", m_filename ="rawmode3_out.dat");
}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MagFieldReader::initialize() {

  MsgStream msg(msgSvc(), name());
  
  msg << MSG::INFO << "FieldReader intialize() has been called" << endreq;
  StatusCode status = service("MagneticFieldSvc", m_pIMF, true);
  if( !status.isSuccess() ) {
    msg << MSG::FATAL << "Unable to open Magnetic field service" 
        << endreq;
    return StatusCode::FAILURE;
  }

  msg << MSG::DEBUG << " Book ntuple" << endreq;

  NTuplePtr nt1(ntupleSvc(), "FILE1/n1");
  if(nt1) m_tuple1 = nt1;
  else {
    m_tuple1 = ntupleSvc()->book("FILE1/n1",CLID_ColumnWiseTuple,"Field");
    if( m_tuple1 ) {
      status = m_tuple1->addItem("x",  m_x );
      status = m_tuple1->addItem("y",  m_y );
      status = m_tuple1->addItem("z",  m_z );
      status = m_tuple1->addItem("r",  m_r );
      status = m_tuple1->addItem("Bx",  m_Bx );
      status = m_tuple1->addItem("By",  m_By );
      status = m_tuple1->addItem("Bz",  m_Bz );
      status = m_tuple1->addItem("SigmaBx", m_sigma_bx );
      status = m_tuple1->addItem("SigmaBy", m_sigma_by );
      status = m_tuple1->addItem("SigmaBz", m_sigma_bz );
    }
    else {  
      msg << MSG::ERROR << " Cannot book N-tuple:" <<long(m_tuple1)<< endreq;
      return StatusCode::FAILURE;
    }
  }   

  NTuplePtr nt2(ntupleSvc(), "FILE1/n2");
  if(nt2) m_tuple2 = nt2;
  else {
    m_tuple2 = ntupleSvc()->book("FILE1/n2",CLID_ColumnWiseTuple,"Field");
    if( m_tuple2 ) {
      status = m_tuple2->addItem("x",  m_x2 );
      status = m_tuple2->addItem("y",  m_y2 );
      status = m_tuple2->addItem("z",  m_z2 );
      status = m_tuple2->addItem("r",  m_r2 );
      status = m_tuple2->addItem("Bx",  m_Bx2 );
      status = m_tuple2->addItem("By",  m_By2 );
      status = m_tuple2->addItem("Bz",  m_Bz2 );
    }
    else {
      msg << MSG::ERROR << " Cannot book N-tuple:" <<long(m_tuple2)<< endreq;
      return StatusCode::FAILURE;
    }
  }

  NTuplePtr nt3(ntupleSvc(), "FILE1/n3");
  if(nt3) m_tuple3 = nt3;
  else {
    m_tuple3 = ntupleSvc()->book("FILE1/n3",CLID_ColumnWiseTuple,"Field");
    if( m_tuple3 ) {
      status = m_tuple3->addItem("x",  m_x3 );
      status = m_tuple3->addItem("y",  m_y3 );
      status = m_tuple3->addItem("z",  m_z3 );
      status = m_tuple3->addItem("r",  m_r3 );
      status = m_tuple3->addItem("phi",  m_phi3 );
      status = m_tuple3->addItem("Bx",  m_Bx3 );
      status = m_tuple3->addItem("By",  m_By3 );
      status = m_tuple3->addItem("Bz",  m_Bz3 );
    }   
    else { 
      msg << MSG::ERROR << " Cannot book N-tuple:" <<long(m_tuple3)<< endreq;
      return StatusCode::FAILURE;
    }
  }
 
  NTuplePtr nt4(ntupleSvc(), "FILE1/n4");
  if(nt4) m_tuple4 = nt4;
  else {
    m_tuple4 = ntupleSvc()->book("FILE1/n4",CLID_ColumnWiseTuple,"Field");
    if( m_tuple4 ) {
      status = m_tuple4->addItem("time",  m_time );
    }
    else {
      msg << MSG::ERROR << " Cannot book N-tuple:" <<long(m_tuple4)<< endreq;
      return StatusCode::FAILURE;
    }
  }

  status = service("BesTimerSvc", m_timersvc);
  if (status.isFailure()) {
    msg << MSG::ERROR << name() << ": Unable to locate BesTimer Service" << endreq;
    return StatusCode::FAILURE;
  }
  m_timer = m_timersvc->addItem("Read field Time");

  msg << MSG::INFO << "MagFieldReader initialized" << endreq;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MagFieldReader::execute() {
  
  MsgStream  msg( msgSvc(), name() );

  msg << MSG::DEBUG << "==> Execute MagFieldReader" << endreq;
/*
  //calculate the error of Bfield
  std::ifstream infile( m_filename.c_str() );
  if(!infile.good()) msg << MSG::FATAL << "Unable to read the file: " << m_filename << endreq;
  int nLine = 0;
  char line[ 255 ];
  while( infile ) {
    // parse each line of the file, 
    // comment lines begin with '#' in the cdf file
    infile.getline( line, 255 );
    if ( line[0] == '#' ) continue;
    std::string  gridx, gridy, gridz, sFx, sFy, sFz;
    char* token = strtok( line, " " );
    if ( token ) { gridx = token; token = strtok( NULL, " " );} else continue;
    if ( token ) { gridy = token; token = strtok( NULL, " " );} else continue;
    if ( token ) { gridz = token; token = strtok( NULL, " " );} else continue;
    if ( token ) { sFx = token; token = strtok( NULL, " " );} else continue;
    if ( token ) { sFy = token; token = strtok( NULL, " " );} else continue;
    if ( token ) { sFz = token; token = strtok( NULL, " " );} else continue;
    if ( token != NULL ) continue;
    nLine++;
    double x,y,z,bx,by,bz;
    //Grid position
    x = atof( gridx.c_str() );
    y = atof( gridy.c_str() );
    z = atof( gridz.c_str() );
    // Field values are given in tesla in CDF file. Convert to CLHEP units
    bx = atof( sFx.c_str() );
    by = atof( sFy.c_str() );
    bz = atof( sFz.c_str() );
    //std::cout<<"x,y,z: "<<x<<" "<<y<<" "<<z<<" "<<"bx,by,bz: "<<bx<<" "<<by<<" "<<bz<<std::endl;
    x = -x*m;
    y = y*m;
    z = -z*m;

    HepPoint3D r(x,y,z);
    HepVector3D b;
    m_pIMF->fieldVector(r,b);
    m_Bx = b.x()/tesla;
    m_By = b.y()/tesla;
    m_Bz = b.z()/tesla;
    m_sigma_bx = (b.x()/tesla + bx)*10000.;
    m_sigma_by = (b.y()/tesla - by)*10000.;
    m_sigma_bz = (b.z()/tesla + bz)*10000.;
    m_r = std::sqrt(x/m*x/m+y/m*y/m);
    m_x = x/m;
    m_y = y/m;
    m_z = z/m;
    m_tuple1->write();
  }
  infile.close();
  std::cout<<"Totally "<<nLine<<" in file "<<m_filename<<std::endl;
  
  for(int k = 0; k < 5; k++) {
    double rr;
    if(k==0) rr = 0;
    if(k==1) rr = 200;
    if(k==2) rr = 400;
    if(k==3) rr = 600;
    if(k==4) rr = 800;
    for(int zz = -1195; zz <1200; zz+=50) {
      double bxt = 0.,byt= 0.,bzt = 0.;
      for(int i = 0; i < 100000; i++) {  
        double phi = CLHEP::twopi*CLHEP::RandFlat::shoot();
        double xx = rr*std::cos(phi); 
        double yy = rr*std::sin(phi);
        HepPoint3D r(xx,yy,double(zz));
        HepVector3D b;
        m_pIMF->fieldVector(r,b);
        bxt+=b.x()/tesla;
        byt+=b.y()/tesla;
        bzt+=b.z()/tesla;
      }
      m_z2 = zz;
      m_r2 = rr;
      m_Bx2 = bxt/100000;
      m_By2 = byt/100000;
      m_Bz2 = bzt/100000;
      m_tuple2->write();
    }
  }

  for(int k = 0; k < 3; k++) {
    double zz;
    if(k==0) zz = 0;
    if(k==1) zz = -800;
    if(k==2) zz = 800;
    for(int rr = 200; rr <=600; rr+=400) {
      for(double phi = 0; phi <= 2*3.14159; phi+=0.1745) {
        //double phi = CLHEP::twopi*CLHEP::RandFlat::shoot();
        double xx = rr*std::cos(phi);
        double yy = rr*std::sin(phi);
        HepPoint3D r(xx,yy,double(zz));
        HepVector3D b;
        m_pIMF->fieldVector(r,b);
        m_Bx3 = b.x()/tesla;
        m_By3 = b.y()/tesla;
        m_Bz3 = b.z()/tesla;
        m_phi3 = phi*180/CLHEP::pi;
        m_x3 = xx;
        m_y3 = yy;
        m_z3 = zz;
        m_r3 = rr;
        m_tuple3->write();
      }
    }
  } 
*/
  m_timer->start();
  for(int i = 0; i < 20000; i++)
  {
    double px,py,pz,bx,by,bz;
    double max_x = 1.8*m, min_x = -1.8*m, max_y = 1.8*m, min_y = -1.8*m, max_z = 2.1*m, min_z = -2.1*m;
    px = min_x + (max_x - min_x)*RandFlat::shoot();
    py = min_y + (max_y - min_y)*RandFlat::shoot();
    pz = min_z + (max_z - min_z)*RandFlat::shoot();
    HepPoint3D r(px,py,pz);
    HepVector3D b;
    m_pIMF->fieldVector(r,b);
    bx = b.x()/tesla;
    by = b.y()/tesla;
    bz = b.z()/tesla;
  }
  m_timer->stop();
  m_time = m_timer->elapsed();  
  m_tuple4->write();
/*  
  StatusCode sc = this->readField();
  if( sc.isFailure() ) {
    msg << MSG::FATAL << "Unable to execute MagFieldReader" 
        << endreq;
    return sc;
  }
*/
  msg << MSG::INFO << "MagFieldReader executed" << endreq;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MagFieldReader::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize MagFieldReader" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
//=============================================================================
// The MagFieldReader print out info messages 
// with the field value at different locations.
//=============================================================================
StatusCode MagFieldReader::readField() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "m_pIMF = " << m_pIMF << endreq;

  double referfield = m_pIMF->getReferField()/tesla;
  msg << MSG::DEBUG << "The reference field is " << referfield << " tesla" <<endreq;
  bool ifrealfield = m_pIMF->ifRealField();
  if(ifrealfield) msg << MSG::DEBUG << "Use the real field"<<endreq;
  else msg << MSG::DEBUG << "Use the fake field"<<endreq;

  HepVector3D B(0.0,0.0,0.0);
/*  for ( double z = m_zMin; z <= m_zMax; z += m_step ) {
    for( double y = m_yMin; y <= m_yMax; y += m_step ) {
      for( double x = m_xMin; x <= m_xMax; x += m_step ) {
        HepPoint3D P( x, y, z );
        // get field at point P
        m_pIMF->fieldVector( P, B );
        // fill ntuple
        m_x = P.x()/cm;
        m_y = P.y()/cm;
        m_z = P.z()/cm;
        m_Bx = B.x()/tesla;
        m_By = B.y()/tesla;
        m_Bz = B.z()/tesla;
        m_ntuple->write();
      }
    }
    HepPoint3D P0( 0.0, 0.0, z );
    m_pIMF->fieldVector( P0, B );
    msg << MSG::DEBUG << "Magnetic Field at ("
        << P0.x() << ", " << P0.y() << ", " << P0.z() << " ) = "
        << (B.x())/tesla << ", "
        << (B.y())/tesla << ", "
        << (B.z())/tesla << " Tesla " 
        << endreq;
  }*/
//magnetic field check bz vs z(m_zMin,m_zMax), x=0,200,400,600,800mm, y=0
  const int n1=241;
  double x[n1],y[n1],y1[n1],y2[n1],y3[n1],y4[n1];
//magnetic field check bz,br,bendp,bphi vs z(0,m_zMax), x=50,100,200,400,600,800mm, y=0
  const int n2=121;
  double x1[n2],bz1[n2],bz2[n2],bz3[n2],bz4[n2],bz5[n2],bz6[n2];
  double br1[n2],br2[n2],br3[n2],br4[n2],br5[n2],br6[n2];
  double bp1[n2],bp2[n2],bp3[n2],bp4[n2],bp5[n2],bp6[n2];
  double bphi1[n2],bphi2[n2],bphi3[n2],bphi4[n2],bphi5[n2],bphi6[n2];
//magnetic field check bx,by vs y(m_yMin,m_yMax)
  const int n3=161;
  double x2[n3],bx1[n3],bx2[n3],bx3[n3],bx4[n3],bx5[n3],bx6[n3];
  double by1[n3],by2[n3],by3[n3],by4[n3],by5[n3],by6[n3];
//check globle field value bz vs x (-2.6m,2.6m),y=0,z=0.
  const int n4=521;
  double globle_x[n4],globle_bz[n4];
  int i=0;
  double px,py,pz;
  HepPoint3D pos(px,py,pz);
  for( px = -2600; px <= 2600; px += 10 ) {
    pos[0] = px;  pos[1] = 0;  pos[2] = 0;
    m_pIMF->fieldVector( pos, B );
    globle_x[i]=px/m;
    globle_bz[i]=B.z()/tesla;
    i++;
  }
  i=0;
  for ( pz = m_zMin; pz <= m_zMax; pz += 10 ) {
    pos[0] = 0;  pos[1] = 0;  pos[2] = pz;
    m_pIMF->fieldVector( pos, B );
    x[i]=pz/m;
    y[i]=B.z()/tesla;
 
    pos[0] = 200;  pos[1] = 0;  pos[2] = pz;
    m_pIMF->fieldVector( pos, B );
    y1[i]=B.z()/tesla;
      
    pos[0] = 400;  pos[1] = 0;  pos[2] = pz;
    m_pIMF->fieldVector( pos, B );
    y2[i]=B.z()/tesla;

    pos[0] = 600;  pos[1] = 0;  pos[2] = pz;
    m_pIMF->fieldVector( pos, B );
    y3[i]=B.z()/tesla;
 
    pos[0] = 800;  pos[1] = 0;  pos[2] = pz;
    m_pIMF->fieldVector( pos, B );
    y4[i]=B.z()/tesla;
    i++;
  }
  int j = 0;
  double tbx,tby,tbz,tbr,tbp,tbphi;
  for ( pz = 0; pz <= m_zMax; pz += 10 ) { 
    pos[0] = 50;  pos[1] = 0;  pos[2] = pz;
    m_pIMF->fieldVector( pos, B );
    x1[j]=pz/m;
    tbx=B.x()/tesla;
    tby=B.y()/tesla;
    tbz=B.z()/tesla;
    tbr=tbx*pos[0]/sqrt(pos[0]*pos[0]+pos[1]*pos[1])+tby*pos[1]/sqrt(pos[0]*pos[0]+pos[1]*pos[1]);
    tbphi=tbx*pos[1]/sqrt(pos[0]*pos[0]+pos[1]*pos[1])+tby*pos[0]/sqrt(pos[0]*pos[0]+pos[1]*pos[1]);
    tbp=tbz-tbr*pz/sqrt(pos[0]*pos[0]+pos[1]*pos[1]);
    bz1[j] = tbz;
    br1[j] = tbr*10000;
    bp1[j] = tbp;
    bphi1[j] = tbphi*10000;
 
    pos[0] = 100;  pos[1] = 0;  pos[2] = pz;
    m_pIMF->fieldVector( pos, B );
    tbx=B.x()/tesla;
    tby=B.y()/tesla;
    tbz=B.z()/tesla;
    tbr=tbx*pos[0]/sqrt(pos[0]*pos[0]+pos[1]*pos[1])+tby*pos[1]/sqrt(pos[0]*pos[0]+pos[1]*pos[1]);
    tbphi=tbx*pos[1]/sqrt(pos[0]*pos[0]+pos[1]*pos[1])+tby*pos[0]/sqrt(pos[0]*pos[0]+pos[1]*pos[1]);
    tbp=tbz-tbr*pz/sqrt(pos[0]*pos[0]+pos[1]*pos[1]);
    bz2[j] = tbz;
    br2[j] = tbr*10000;
    bp2[j] = tbp;
    bphi2[j] = tbphi*10000;

    pos[0] = 200;  pos[1] = 0;  pos[2] = pz;
    m_pIMF->fieldVector( pos, B );
    tbx=B.x()/tesla;
    tby=B.y()/tesla;
    tbz=B.z()/tesla;
    tbr=tbx*pos[0]/sqrt(pos[0]*pos[0]+pos[1]*pos[1])+tby*pos[1]/sqrt(pos[0]*pos[0]+pos[1]*pos[1]);
    tbphi=tbx*pos[1]/sqrt(pos[0]*pos[0]+pos[1]*pos[1])+tby*pos[0]/sqrt(pos[0]*pos[0]+pos[1]*pos[1]);
    tbp=tbz-tbr*pz/sqrt(pos[0]*pos[0]+pos[1]*pos[1]);
    bz3[j] = tbz;
    br3[j] = tbr*10000;
    bp3[j] = tbp;
    bphi3[j] = tbphi*10000;

    pos[0] = 400;  pos[1] = 0;  pos[2] = pz;
    m_pIMF->fieldVector( pos, B );
    tbx=B.x()/tesla;
    tby=B.y()/tesla;
    tbz=B.z()/tesla;
    tbr=tbx*pos[0]/sqrt(pos[0]*pos[0]+pos[1]*pos[1])+tby*pos[1]/sqrt(pos[0]*pos[0]+pos[1]*pos[1]);
    tbphi=tbx*pos[1]/sqrt(pos[0]*pos[0]+pos[1]*pos[1])+tby*pos[0]/sqrt(pos[0]*pos[0]+pos[1]*pos[1]);
    tbp=tbz-tbr*pz/sqrt(pos[0]*pos[0]+pos[1]*pos[1]);
    bz4[j] = tbz;
    br4[j] = tbr*10000;
    bp4[j] = tbp;
    bphi4[j]  = tbphi*10000;

    pos[0] = 600;  pos[1] = 0;  pos[2] = pz;
    m_pIMF->fieldVector( pos, B );
    tbx=B.x()/tesla;
    tby=B.y()/tesla;
    tbz=B.z()/tesla;
    tbr=tbx*pos[0]/sqrt(pos[0]*pos[0]+pos[1]*pos[1])+tby*pos[1]/sqrt(pos[0]*pos[0]+pos[1]*pos[1]);
    tbphi=tbx*pos[1]/sqrt(pos[0]*pos[0]+pos[1]*pos[1])+tby*pos[0]/sqrt(pos[0]*pos[0]+pos[1]*pos[1]);
    tbp=tbz-tbr*pz/sqrt(pos[0]*pos[0]+pos[1]*pos[1]);
    bz5[j] = tbz;
    br5[j] = tbr*10000;
    bp5[j] = tbp;
    bphi5[j] = tbphi*10000;

    pos[0] = 800;  pos[1] = 0;  pos[2] = pz;
    m_pIMF->fieldVector( pos, B );
    tbx=B.x()/tesla;
    tby=B.y()/tesla;
    tbz=B.z()/tesla;
    tbr=tbx*pos[0]/sqrt(pos[0]*pos[0]+pos[1]*pos[1])+tby*pos[1]/sqrt(pos[0]*pos[0]+pos[1]*pos[1]);
    tbphi=tbx*pos[1]/sqrt(pos[0]*pos[0]+pos[1]*pos[1])+tby*pos[0]/sqrt(pos[0]*pos[0]+pos[1]*pos[1]);
    tbp=tbz-tbr*pz/sqrt(pos[0]*pos[0]+pos[1]*pos[1]);
    bz6[j] = tbz;
    br6[j] = tbr*10000;
    bp6[j] = tbp;
    bphi6[j] = tbphi*10000;
    j++;
  }
  int l = 0;
  for(py = m_yMin; py<= m_yMax; py +=10){
    pos[0] = 0;  pos[1] = py;  pos[2] = 0;
    m_pIMF->fieldVector( pos, B );
    tbx=B.x()/tesla*10000;
    tby=B.y()/tesla*10000;
    tbz=B.z()/tesla*10000;
    x2[l] = py/m;
    bx1[l] = tbx;
    by1[l] = tby;
    
    pos[0] = 100;  pos[1] = py;  pos[2] = 100;
    m_pIMF->fieldVector( pos, B );
    tbx=B.x()/tesla*10000;
    tby=B.y()/tesla*10000;
    tbz=B.z()/tesla*10000;
    bx2[l] = tbx;
    by2[l] = tby;

    pos[0] = 400;  pos[1] = py;  pos[2] = 400;
    m_pIMF->fieldVector( pos, B );
    tbx=B.x()/tesla*10000;
    tby=B.y()/tesla*10000;
    tbz=B.z()/tesla*10000;
    bx3[l] = tbx;
    by3[l] = tby;
    l++;
  }

  TGraph2D *dt = new TGraph2D();
  int k = 0;
  for ( pz = -3000; pz <= 3000; pz += 50 ) {
     for( py = -2700; py <= 2700; py += 50){
       pos[0] = 0;  pos[1] = py;  pos[2] = pz;
       m_pIMF->fieldVector( pos, B );
       tbx=B.x()/tesla;
       tby=B.y()/tesla;
       tbz=B.z()/tesla;
       dt->SetPoint(k,pz/m,py/m,tbz);
       k++; 
    }
  }
  TGraph2D *dt1 = new TGraph2D();
  k = 0;
  for ( pz = -3000; pz <= 3000; pz += 50 ) {
     for( py = -3000; py <= 3000; py += 50){
       pos[0] = 0;  pos[1] = py;  pos[2] = pz;
       m_pIMF->fieldVector( pos, B );
       tbx=B.x()/tesla;
       tby=B.y()/tesla;
       tbz=B.z()/tesla;
       double btot = sqrt(tbx*tbx+tby*tby+tbz*tbz);
       dt1->SetPoint(k,pz/m,py/m,btot);
       k++;
    }
  }
  TGraph2D *dt2 = new TGraph2D();
  k = 0;
  for ( pz = m_zMin; pz <= m_zMax; pz += 50 ) {
     for( py = m_yMin; py <= m_yMax; py += 50){
       pos[0] = 0;  pos[1] = py;  pos[2] = pz;
       m_pIMF->fieldVector( pos, B );
       tbx=B.x()/tesla;
       tby=B.y()/tesla;
       tbz=B.z()/tesla;
       //double btot = sqrt(tbx*tbx+tby*tby+tbz*tbz);
       dt2->SetPoint(k,pz/m,py/m,tbz);
       k++;
    }
  }
  gStyle->SetPalette(1);
  gStyle->SetOptTitle(1);
  gStyle->SetOptStat(0);

  TFile* f1=new TFile("graph.root","RECREATE");
  TGraph *gr1 = new TGraph(n1,x,y);
  TGraph *gr2 = new TGraph(n1,x,y1);
  TGraph *gr3 = new TGraph(n1,x,y2);
  TGraph *gr4 = new TGraph(n1,x,y3);
  TGraph *gr5 = new TGraph(n1,x,y4);

  TGraph *g1 = new TGraph(n2,x1,bz1);
  TGraph *g2 = new TGraph(n2,x1,bz2);
  TGraph *g3 = new TGraph(n2,x1,bz3);
  TGraph *g4 = new TGraph(n2,x1,bz4);
  TGraph *g5 = new TGraph(n2,x1,bz5);
  TGraph *g6 = new TGraph(n2,x1,bz6);

  TGraph *g7 = new TGraph(n2,x1,br1);
  TGraph *g8 = new TGraph(n2,x1,br2);
  TGraph *g9 = new TGraph(n2,x1,br3);
  TGraph *g10 = new TGraph(n2,x1,br4);
  TGraph *g11 = new TGraph(n2,x1,br5);
  TGraph *g12 = new TGraph(n2,x1,br6);

  TGraph *g13 = new TGraph(n2,x1,bp1);
  TGraph *g14 = new TGraph(n2,x1,bp2);
  TGraph *g15 = new TGraph(n2,x1,bp3);
  TGraph *g16 = new TGraph(n2,x1,bp4);
  TGraph *g17 = new TGraph(n2,x1,bp5);
  TGraph *g18 = new TGraph(n2,x1,bp6);

  TGraph *g19 = new TGraph(n2,x1,bphi1);
  TGraph *g20 = new TGraph(n2,x1,bphi2);
  TGraph *g21 = new TGraph(n2,x1,bphi3);
  TGraph *g22 = new TGraph(n2,x1,bphi4);
  TGraph *g23 = new TGraph(n2,x1,bphi5);
  TGraph *g24 = new TGraph(n2,x1,bphi6);

  TGraph *g25 = new TGraph(n3,x2,bx1);
  TGraph *g26 = new TGraph(n3,x2,bx2);
  TGraph *g27 = new TGraph(n3,x2,bx3);

  TGraph *g28 = new TGraph(n3,x2,by1);
  TGraph *g29 = new TGraph(n3,x2,by2);
  TGraph *g30 = new TGraph(n3,x2,by3);

  TGraph *g31 = new TGraph(n4,globle_x,globle_bz);

  TCanvas *c1 = new TCanvas("c1","Two Graphs",200,10,600,400);
  TCanvas *c2 = new TCanvas("c2","Two Graphs",200,10,600,400);
  c1->Divide(2,2);
  c2->Divide(2,2);
  c1->cd(1);
  gr1->SetLineColor(2);
  gr1->SetLineWidth(2);
  gr1->Draw("ALP");
  gr1->SetTitle("bz vs z (y=0,x=0,200,400,600,800mm)");
  gr1->GetXaxis()->SetTitle("m");
  gr1->GetYaxis()->SetTitle("tesla");
  gr2->SetLineWidth(2);
  gr2->SetLineColor(3);
  gr2->Draw("LP");
  gr3->SetLineWidth(2);
  gr3->SetLineColor(4);
  gr3->Draw("LP");
  gr4->SetLineWidth(2);
  gr4->SetLineColor(5);
  gr4->Draw("LP");
  gr5->SetLineWidth(2);
  gr5->SetLineColor(6);
  gr5->Draw("LP");

  c1->cd(2);
  g1->SetLineColor(2);
  g1->SetLineWidth(2);
  g1->Draw("ALP");
  g1->SetTitle("bz(red),bendp vs z (y=0,x=50,100,200,400,600,800mm)");
  g1->GetXaxis()->SetTitle("m");
  g1->GetYaxis()->SetTitle("tesla");
  g1->GetYaxis()->SetRangeUser(0.2,2);
  g2->SetLineWidth(2);
  g2->SetLineColor(2);
  g2->Draw("LP");
  g3->SetLineWidth(2);
  g3->SetLineColor(2);
  g3->Draw("LP");
  g4->SetLineWidth(2);
  g4->SetLineColor(2);
  g4->Draw("LP");
  g5->SetLineWidth(2);
  g5->SetLineColor(2);
  g5->Draw("LP");
  g6->SetLineColor(2);
  g6->SetLineWidth(2);
  g6->Draw("LP");

  g13->SetLineWidth(2);
  g13->SetLineColor(4);
  g13->Draw("LP");
  g14->SetLineWidth(2);
  g14->SetLineColor(4);
  g14->Draw("LP");
  g15->SetLineWidth(2);
  g15->SetLineColor(4);
  g15->Draw("LP");
  g16->SetLineWidth(2);
  g16->SetLineColor(4);
  g16->Draw("LP");
  g17->SetLineWidth(2);
  g17->SetLineColor(4);
  g17->Draw("LP");
  g18->SetLineWidth(2);
  g18->SetLineColor(4);
  g18->Draw("LP");

  c1->cd(3);
  g7->SetLineWidth(2);
  g7->SetLineColor(2);
  g7->Draw("ALP");
  g7->SetTitle("br vs z (y=0,x=50,100,200,400,600,800mm)");
  g7->GetXaxis()->SetTitle("m");
  g7->GetYaxis()->SetTitle("gauss");
  g7->GetYaxis()->SetRangeUser(-1100,1000);
  g8->SetLineWidth(2);
  g8->SetLineColor(3);
  g8->Draw("LP");
  g9->SetLineWidth(2);
  g9->SetLineColor(4);
  g9->Draw("LP");
  g10->SetLineWidth(2);
  g10->SetLineColor(5);
  g10->Draw("LP");
  g11->SetLineWidth(2);
  g11->SetLineColor(6);
  g11->Draw("LP");
  g12->SetLineWidth(2);
  g12->SetLineColor(7);
  g12->Draw("LP");

  c1->cd(4);
  g19->SetLineWidth(2);
  g19->SetLineColor(2);
  g19->Draw("ALP");
  g19->SetTitle("bphi vs z (y=0,x=50,100,200,400,600,800mm)");
  g19->GetXaxis()->SetTitle("m");
  g19->GetYaxis()->SetTitle("gauss");
  g19->GetYaxis()->SetRangeUser(-1000,200);
  g20->SetLineWidth(2);
  g20->SetLineColor(3);
  g20->Draw("LP");
  g21->SetLineWidth(2);
  g21->SetLineColor(4);
  g21->Draw("LP");
  g22->SetLineWidth(2);
  g22->SetLineColor(5);
  g22->Draw("LP");
  g23->SetLineWidth(2);
  g23->SetLineColor(6);
  g23->Draw("LP");
  g24->SetLineWidth(2);
  g24->SetLineColor(7);
  g24->Draw("LP");
 
  TCanvas *c3 = new TCanvas("c3","Two Graphs",200,10,600,400);
  c3->cd(1);
  //dt->Draw("surf2");
  //dt->Draw("tril p3");
  //dt->Draw("surf1z");
  dt->Draw("z sinusoidal");
  dt->SetTitle("bz vs y,z (x=0)");

  TCanvas *c4 = new TCanvas("c4","Two Graphs",200,10,600,400);
  c4->cd(1);
  dt1->Draw("z sinusoidal");
  dt1->SetTitle("btot vs y,z (x=0)");

  TCanvas *c5 = new TCanvas("c5","Two Graphs",200,10,600,400);
  c5->cd(1);
  dt2->Draw("z sinusoidal");
  dt2->SetTitle("bz vs y,z (x=0)");

  c2->cd(2);
  g25->SetLineWidth(2);
  g25->SetLineColor(2);
  g25->Draw("ALP");
  g25->SetTitle("bx(red),by vs y (x,z=0,100,400mm)");
  g25->GetXaxis()->SetTitle("m");
  g25->GetYaxis()->SetTitle("gauss");
  g25->GetYaxis()->SetRangeUser(-200,300);
  g26->SetLineWidth(2);
  g26->SetLineColor(2);
  g26->Draw("LP");
  g27->SetLineWidth(2);
  g27->SetLineColor(2);
  g27->Draw("LP");

  g28->SetLineWidth(2);
  g28->SetLineColor(3);
  g28->Draw("LP");
  g29->SetLineWidth(2);
  g29->SetLineColor(3);
  g29->Draw("LP");
  g30->SetLineWidth(2);
  g30->SetLineColor(3);
  g30->Draw("LP");

  c2->cd(3);
  g31->SetLineWidth(2);
  g31->SetLineColor(2);
  g31->Draw("ALP");
  g31->SetTitle("bz vs x (y,z=0)");
  g31->GetXaxis()->SetTitle("m");
  g31->GetYaxis()->SetTitle("tesla");

  c1->Write();
  c2->Write();
  c3->Write();
  c4->Write();
  c5->Write();
/*   HepVector3D B1(0.0,0.0,0.0);
   int ii=0;
   double posx,posy,posz;
   ofstream outfile("tmp.txt",ios_base::app);
   for ( double posz = -2800;posz <= 2800; posz += 10 ) {
     for(double posy = -2650; posy <= 2650; posy += 10){
       posx=1500; posy = 0;
       HepPoint3D p1(posx,posy,posz);
       m_pIMF->fieldVector( p1, B1 );
       tbx=B1.x()/tesla;
       tby=B1.y()/tesla;
       tbz=B1.z()/tesla;
       tbr=tbx*posx/sqrt(posx*posx+posy*posy)+tby*posy/sqrt(posx*posx+posy*posy);
       tbp=tbz-tbr*posz/sqrt(posx*posx+posy*posy);
       outfile<<posz/m<<" "<<tby<<endl;
       ii++;
    }
  }
   outfile.close();
*/
  // Return status code.
  return StatusCode::SUCCESS;
}
