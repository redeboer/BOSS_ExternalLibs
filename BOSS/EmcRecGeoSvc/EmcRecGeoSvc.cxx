#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
//#include "GaudiKernel/ISvcFactory.h"
//#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"

#include "EmcRecGeoSvc/EmcRecGeoSvc.h" 
#include "EmcRecGeoSvc/EmcRecROOTGeo.h"

//static SvcFactory<EmcRecGeoSvc> s_factory;
//const ISvcFactory& EmcRecGeoSvcFactory = s_factory;

EmcRecGeoSvc::EmcRecGeoSvc( const std::string& name, ISvcLocator* svcloc ) : Service(name, svcloc) 
{
  fGdml = true;

  // Declare the properties  
  declareProperty("Gdml",fGdml);

  if(fGdml) {
    fROOTGeo = new EmcRecROOTGeo();
  }
}

EmcRecGeoSvc::~EmcRecGeoSvc()
{
  if(fGdml) {
    if(fROOTGeo) delete fROOTGeo;
  }
}

StatusCode EmcRecGeoSvc::queryInterface (const InterfaceID& riid, void** ppvInterface ){

  if ( IID_IEmcRecGeoSvc.versionMatch(riid) ) { 
    *ppvInterface = static_cast<IEmcRecGeoSvc*> (this); 
  } else { 
    return Service::queryInterface(riid, ppvInterface) ; 
  }
  return StatusCode::SUCCESS;
}

StatusCode EmcRecGeoSvc::initialize ( ) {
  MsgStream log(messageService(), name());
  log << MSG::INFO << name() << ": Start of run initialisation" << endreq;

  StatusCode sc = Service::initialize();
  if ( sc.isFailure() ) return sc;

  // initialize geometry from gdml
  if(fGdml) {
    fROOTGeo->InitFromXML();
  }
  return StatusCode::SUCCESS;
}


StatusCode EmcRecGeoSvc::finalize ( ) {
  MsgStream log(messageService(), name());
  log << MSG::INFO << name() << ": End of Run" << endreq;
  return StatusCode::SUCCESS;
}

EmcRecCrystal EmcRecGeoSvc::GetCrystal(const Identifier& id) const{
  if(fGdml) {
    return fROOTGeo->GetCrystal(id);
  } else {
    EmcRecCrystal cry;

    if(EmcID::is_barrel(id)) {
      cry=fBarrel.GetCrystal(id);
    } else {
      cry=fEndCap.GetCrystal(id);
    }

    return cry;
  }

}

HepPoint3D EmcRecGeoSvc::GetCrystalPoint(const Identifier& id, const int i) const{
  EmcRecCrystal cry;
  if(fGdml) {
    cry=fROOTGeo->GetCrystal(id);
  } else {
    if(EmcID::is_barrel(id)) {
      cry=fBarrel.GetCrystal(id);
    } else {
      cry=fEndCap.GetCrystal(id);
    }
  }
  return cry.Get(i)/cm;
}

HepPoint3D EmcRecGeoSvc::GetCCenter(const Identifier& id) const{
  if(fGdml) {
    return fROOTGeo->GetCCenter(id)/cm;
  } else {
    HepPoint3D center;

    if(EmcID::is_barrel(id)) {
      center=fBarrel.GetCCenter(id);
    } else {
      center=fEndCap.GetCCenter(id);
    }

    return center/cm;
  }
}

HepPoint3D EmcRecGeoSvc::GetCFrontCenter(const Identifier& id) const{
  if(fGdml) {
    return fROOTGeo->GetCFrontCenter(id)/cm;
  } else {
    HepPoint3D frontCenter;

    if(EmcID::is_barrel(id)) {
      frontCenter=fBarrel.GetCFrontCenter(id);
    } else {
      frontCenter=fEndCap.GetCFrontCenter(id);
    }

    return frontCenter/cm;
  }
}

double EmcRecGeoSvc::GetBarrelR() const
{
  return fBarrel.GetBarrelR()/cm;
}

double EmcRecGeoSvc::GetBarrelOffset1() const
{
  return fBarrel.GetBarrelOffset1()/cm;
}

double EmcRecGeoSvc::GetBarrelOffset2() const
{
  return fBarrel.GetBarrelOffset2()/cm;
}

double EmcRecGeoSvc::GetBarrelh1() const
{
  return fBarrel.GetBarrelh1()/cm;
}

double EmcRecGeoSvc::GetBarrelh2() const
{
  return fBarrel.GetBarrelh2()/cm;
}

double EmcRecGeoSvc::GetBarrelh3() const
{
  return fBarrel.GetBarrelh3()/cm;
}

double EmcRecGeoSvc::GetBarrelL() const
{
  return fBarrel.GetBarrelL()/cm;
}

int EmcRecGeoSvc::GetBarrelNPhiMax() const
{
  return fBarrel.GetBarrelNPhiMax();
}

int EmcRecGeoSvc::GetBarrelNThetaMax() const
{
  return fBarrel.GetBarrelNThetaMax();
}
