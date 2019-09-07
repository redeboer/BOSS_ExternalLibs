/*
 *    2004/09/10   Zhengyun You     Peking University
 *                 
 */

#include "MucGeomSvc/MucGeomSvc.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
//#include "GaudiKernel/ISvcFactory.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"

//static SvcFactory<MucGeomSvc> s_factory;
//const ISvcFactory& MucGeomSvcFactory = s_factory;
  
MucGeomSvc::MucGeomSvc( const std::string& name, ISvcLocator* svcloc ) : Service(name, svcloc) 
{
  //Declare the properties
  declareProperty("GeometryMode",m_Geometry=1);

}

StatusCode MucGeomSvc::queryInterface (const InterfaceID& riid, void** ppvInterface ){
  
  if ( IID_IMucGeomSvc.versionMatch(riid) ) { 
    *ppvInterface = static_cast<IMucGeomSvc*> (this); 
  } else { 
    return Service::queryInterface(riid, ppvInterface) ; 
  }
  return StatusCode::SUCCESS;
}

StatusCode MucGeomSvc::initialize ( ) {
  MsgStream log(messageService(), name());
  log << MSG::INFO << name() << ": Start of run initialisation" << endreq;
  
  StatusCode sc = Service::initialize();
  if ( sc.isFailure() ) return sc;

  //  get geometry data
  Fill();
    
  return StatusCode::SUCCESS;
}
  
  
StatusCode MucGeomSvc::finalize ( ) {
  MsgStream log(messageService(), name());
  log << MSG::INFO << name() << ": End of Run" << endreq;
  return StatusCode::SUCCESS;
}


MucGeomSvc::~MucGeomSvc(){
}

void MucGeomSvc::Fill(){ 
  if(m_Geometry==1){
    m_pMucGeoGeneral = MucGeoGeneral::Instance();
    m_pMucGeoGeneral->Init();  
    //m_pMucGeoGeneral->InitFromASCII();
    m_pMucGeoGeneral->InitFromXML();  
  } else{ //geant4 geo 
    m_pMucG4Geo = MucG4Geo::Instance();
  }
}

const MucGeoGeneral * const 
MucGeomSvc::GetGeoGeneral()
{
  return m_pMucGeoGeneral;
}

const MucGeoGap * const 
MucGeomSvc::GetGap(int part, int seg, int gap)
{
  return m_pMucGeoGeneral->GetGap(part, seg, gap);
}

const MucGeoStrip * const 
MucGeomSvc::GetStrip(int part, int seg, int gap, int strip)
{
  return m_pMucGeoGeneral->GetStrip(part, seg, gap, strip);
}

void MucGeomSvc::Dump(){
  std::cout << "StripNumTotal = " << m_pMucGeoGeneral->GetStripNumTotal() << std::endl;
}

const MucG4Geo * const     //for geant4 geo 2006.11.23
MucGeomSvc::GetMucG4Geo()
{
  return m_pMucG4Geo;
}
