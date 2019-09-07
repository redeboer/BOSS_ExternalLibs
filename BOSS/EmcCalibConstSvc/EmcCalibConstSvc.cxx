#include "EmcCalibConstSvc/EmcCalibConstSvc.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataSvc.h"

#include "EmcGeneralClass/EmcStructure.h"
#include "CalibData/CalibModel.h"
#include "CalibData/Emc/EmcCalibData.h"

#include <vector>
#include <math.h>
#include <fstream>


EmcCalibConstSvc::EmcCalibConstSvc( const std::string& name, ISvcLocator* svcloc) :
  Service (name, svcloc) {


  for(int i=0;i<6240;i++){
    m_CrystalEmaxData[i]=0;
  }
 

  string paraPath = getenv("EMCCALIBCONSTSVCROOT");
  paraPath += "/share/emax_data.dat";

  ifstream in;
  in.open(paraPath.c_str());
  assert(in);
  int ixtal;
  double emaxData;
  int Nixt=0;
  while (in.peek() != EOF) 
    { 
      
      in >> ixtal>>emaxData;

      m_CrystalEmaxData[ixtal]=emaxData;
      Nixt++; 
    }

  in.close();
 

}

EmcCalibConstSvc::~EmcCalibConstSvc(){

}

StatusCode EmcCalibConstSvc::queryInterface(const InterfaceID& riid, void** ppvInterface){
     if( IID_IEmcCalibConstSvc.versionMatch(riid) ){
	  *ppvInterface = static_cast<IEmcCalibConstSvc*> (this);
     } else{
	  return Service::queryInterface(riid, ppvInterface);
     }
     return StatusCode::SUCCESS;
}

StatusCode EmcCalibConstSvc::initialize(){
  MsgStream log(messageService(), name());
  log << MSG::INFO << "EmcCalibConstSvc::initialize()" << endreq;
  
  StatusCode sc = Service::initialize();
  if( sc.isFailure() ) return sc;
  
  sc = service("CalibDataSvc", m_calDataSvc, true);
  if( sc == StatusCode::SUCCESS ){
    log << MSG::INFO << "Retrieve IDataProviderSvc" << endreq;
  }else{
    log << MSG::FATAL << "can not get IDataProviderSvc" << endreq;
  }

  //     sc = service("EmcRecGeoSvc", m_emcGeomSvc);
  //     if( sc != StatusCode::SUCCESS ){
  //	  log << MSG::ERROR << "can not use EmcRecGeoSvc" << endreq;
  // }
  
  m_theEmcStruc= new EmcStructure() ; 
  m_theEmcStruc->setEmcStruc();
 
  // Dump();

  return StatusCode::SUCCESS;
}

StatusCode EmcCalibConstSvc::finalize(){
 
     delete m_theEmcStruc;

     MsgStream log(messageService(), name());
     log << MSG::INFO << "EmcCalibConstSvc::finalize()" << endreq;
     return StatusCode::SUCCESS;

}

double EmcCalibConstSvc::getDigiCalibConst(int No)   const
 {
     double digiCalibConst = 0.0;
     MsgStream log(messageService(), name());
     
     std::string fullPath = "/Calib/EmcCal";
     SmartDataPtr<CalibData::EmcCalibData> calConst(m_calDataSvc, fullPath);
     if( ! calConst ){
	  log << MSG::ERROR << "can not access to EmcDigi CalibData via SmartPtr" 
	      << endreq;
     }else {
         digiCalibConst = calConst->getDigiCalibConst(No);
     }
     
     return digiCalibConst;
}

int EmcCalibConstSvc::getIxtalNumber(int No)   const
 {
     int IxtalNumber = 999999;
     MsgStream log(messageService(), name());
     
     std::string fullPath = "/Calib/EmcCal";
     SmartDataPtr<CalibData::EmcCalibData> calConst(m_calDataSvc, fullPath);
     if( ! calConst ){
	  log << MSG::ERROR << "can not access to EmcDigi CalibData via SmartPtr" 
	      << endreq;
     }else {
         IxtalNumber = calConst->getIxtalNumber(No);
     }
     
     return IxtalNumber;
}

int EmcCalibConstSvc::getDigiCalibConstNo( ) const
 {
     int digiCalibConstNo = 0;
     MsgStream log(messageService(), name());
     
     std::string fullPath = "/Calib/EmcCal";
     SmartDataPtr<CalibData::EmcCalibData> calConst(m_calDataSvc, fullPath);
     if( ! calConst ){
	  log << MSG::ERROR << "can not access to EmcDigiNo CalibData via SmartPtr" 
	      << endreq;
     }else {
        digiCalibConstNo = calConst->getDigiCalibConstNo();
     }
     
     return digiCalibConstNo;
}

int EmcCalibConstSvc::getIxtalNumberNo( ) const
 {
     int IxtalNumberNo = 0;
     MsgStream log(messageService(), name());
     
     std::string fullPath = "/Calib/EmcCal";
     SmartDataPtr<CalibData::EmcCalibData> calConst(m_calDataSvc, fullPath);
     if( ! calConst ){
	  log << MSG::ERROR << "can not access to EmcDigiNo CalibData via SmartPtr" 
	      << endreq;
     }else {
        IxtalNumberNo = calConst->getIxtalNumberNo();
     }
     
     return IxtalNumberNo;
}



int EmcCalibConstSvc::getIndex( unsigned int PartId, 
				unsigned int ThetaIndex, 
				unsigned int PhiIndex)  const
{
  return m_theEmcStruc->getGeomIndex(PartId, ThetaIndex, PhiIndex);
}


unsigned int EmcCalibConstSvc::getPartID(int Index) const
{
  return m_theEmcStruc->getPartId(Index);
}

unsigned int EmcCalibConstSvc::getThetaIndex( int Index)  const 
{
  return m_theEmcStruc->getTheta(Index);
}

unsigned int EmcCalibConstSvc::getPhiIndex( int Index)  const  
{
  return m_theEmcStruc->getPhi(Index);
}



double EmcCalibConstSvc::getCrystalEmaxData( int Index)  const
{
  return m_CrystalEmaxData[Index];
}
 


void   EmcCalibConstSvc::Dump()
{
  /*
  for(int iNo=0;iNo<6;iNo++){
    cout<<"getDigiCalibConst "<<getDigiCalibConst(iNo)<<endl;
  }

  cout<<"getDigiCalibConstNo "<<getDigiCalibConstNo()<<endl;

  
   cout<<"ind"<<"  "<<"getThetaIndex(ind)"<<"  "
       <<"getPhiIndex(ind)"<<"getPartID"<<endl;

  for(int ind=0; ind<6240;ind++){

    cout<<ind<<"  "<<getThetaIndex(ind)<<"  "
  	<<getPhiIndex(ind)<<"  "<<getPartID(ind)<<endl;  
  
  }

  cout<<"getIndex(0,5,95)="<<getIndex(0,5,95)<<endl;
  cout<<"getIndex(1,43,119)="<<getIndex(1,43,119)<<endl;
  cout<<"getIndex(2,5,95)="<<getIndex(2,5,95)<<endl;
  cout<<"getIndex(0,5,96)="<<getIndex(0,5,96)<<endl;
  cout<<"getIndex(1,43,120)="<<getIndex(1,43,120)<<endl;
  cout<<"getIndex(2,5,96)="<<getIndex(2,5,96)<<endl;

  for(int i=0;i<6240;i++){
    cout<<i<<"\t"<<getCrystalEmaxData(i) <<endl;
  }
  */

}
