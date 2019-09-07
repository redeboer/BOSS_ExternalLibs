#include "TrackUtil/MdcTrackUtil.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "EventModel/EventHeader.h"
#include "RawEvent/RawDataUtil.h"
#include "CLHEP/Units/PhysicalConstants.h"
// Mointe-Carlo data
#include "Identifier/MdcID.h"
#include "MdcRawEvent/MdcDigi.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatblty wll be enabled ONLY n CLHEP 1.9
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

// MDC reconstructed data
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcHit.h"

// MDC Geometory
#include "MdcGeomSvc/MdcGeomSvc.h"

using namespace std;
using namespace Event;

MdcTrackUtil* MdcTrackUtil::_myself = 0;


MdcTrackUtil* MdcTrackUtil::instance() {
  if( 0 == _myself ) {
    _myself = new MdcTrackUtil();
  }
  return _myself;
}


MdcTrackUtil::MdcTrackUtil(){
  //Initalze magnetic flied 
  IService* svc;
  Gaudi::svcLocator()->getService("MagneticFieldSvc",svc);
  m_pIMF = dynamic_cast<IMagneticFieldSvc*> (svc);
  if(! m_pIMF){
    std::cout<<" ERROR Unable to open Magnetic field service "<<std::endl;
  }
  //get Bz for Check TEMP, Bz may be changed by run
  double gaussToTesla = 1000.;
  Bz = m_pIMF->getReferField()*gaussToTesla;

  // Initialize MdcGeomSvc
  Gaudi::svcLocator()->getService("MdcGeomSvc",svc);
  m_mdcGeomSvc= dynamic_cast<MdcGeomSvc*> (svc);
  if(! m_mdcGeomSvc){
    std::cout<<" FATAL Could not load MdcGeomSvc!  "<<std::endl;
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
int MdcTrackUtil::nLayerTrackPassed(const HepVector helix){
  double helixParam[5];
  for(int i=0; i<5; ++i) helixParam[i] = helix[i];

  return nLayerTrackPassed(helixParam);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
int MdcTrackUtil::nLayerTrackPassed(const double helix[5]){
  int nLayer = 0;

  for(unsigned iLayer=0; iLayer<43; iLayer++){
    //flightLength is the path length of track in the x-y plane
    //guess flightLength by the radius in middle of the wire.
    double rMidLayer = m_mdcGeomSvc->Layer(iLayer)->Radius();
    double flightLength = rMidLayer;

    HepPoint3D pivot(0.,0.,0.);
    double dz = helix[3];
    double c = CLHEP::c_light * 100.; //unit from m/s to cm/s
    double alpha = 1/(c * Bz);//~333.567
    double kappa = helix[2];
    double rc = (-1.)*alpha/kappa;
    //std::cout<<"MdcTrackUtil alpha   "<<alpha<<std::endl;
    double tanl = helix[4];
    double phi0 = helix[1];
    double phi = flightLength/rc + phi0;//turning angle
    double z = pivot.z() + dz - (alpha/kappa) * tanl * phi;

    double layerHalfLength = m_mdcGeomSvc->Layer(iLayer)->Length()/2.;

    //std::cout<<"MdcTrackUtil length  "<<layerHalfLength<<std::endl;

    if (fabs(z) < fabs(layerHalfLength)) ++nLayer;
  }

  return nLayer;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
HepVector MdcTrackUtil::patRecPar2BesPar(const HepVector& helixPar){
  HepVector helix(5,0);
  double d0 = -helixPar[0];    //cm
  double phi0 = helixPar[1]+ CLHEP::halfpi;
  double omega = Bz*helixPar[2]/-333.567;
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
HepSymMatrix MdcTrackUtil::patRecErr2BesErr(const HepSymMatrix& err){
  //error matrix
  //std::cout<<" err1  "<<err<<" "<<err.num_row()<<std::endl;
  //V(Y)=S * V(X) * ST , mS = S , mVy = V(Y) , err() = V(X)
  //int n = err.num_row();
  HepSymMatrix mS(err.num_row(),0);
  mS[0][0]=-1.;//dr0=-d0
  mS[1][1]=1.;
  mS[2][2]=Bz/-333.567;//pxy -> cpa
  mS[3][3]=1.;
  mS[4][4]=1.;
  HepSymMatrix mVy= err.similarity(mS);
  //std::cout<<" err2  "<<n<<" "<<mVy<<std::endl;
  return mVy;
}

