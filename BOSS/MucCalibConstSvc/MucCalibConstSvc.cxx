//------------------------------------------------------------------------------|
//      [File  ]:                 MucCalibConstSvc.cxx                          |
//      [Brief ]: Algorithom of MUC calibration constants service               |
//                constants read from CalibDataSvc                              |
//      [Author]: Xie Yuguang, <ygxie@mail.ihep.ac.cn>                          |
//      [Date  ]: Mar 15, 2007                                                  |
//      [Log   ]: See ChangLog                                                  |
//------------------------------------------------------------------------------|

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
//#include "GaudiKernel/ISvcFactory.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataSvc.h"

#include "CalibData/CalibModel.h"
#include "CalibData/Muc/MucCalibData.h"
#include "CalibDataSvc/ICalibRootSvc.h"
#include "CalibDataSvc/CalibDataSvc.h"

#include "MucCalibConstSvc/MucCalibConstSvc.h"

//static SvcFactory<MucCalibConstSvc> s_factory;
//const ISvcFactory& MucCalibConstSvcFactory = s_factory;

MucCalibConstSvc::MucCalibConstSvc( const std::string& name, ISvcLocator* svcloc) : Service (name, svcloc),
  m_pCalibDataSvc(0) 
{ 
  declareProperty("ConfigMode",     m_fConfigMode=3);
  declareProperty("UniformEff",     m_fUniformEff=0.95);
  declareProperty("UniformCnt",     m_fUniformEff=0.05);
  declareProperty("UniformNos",     m_fUniformEff=0.001);
  declareProperty("UniformNosRatio",m_fUniformNos=0.05);
  declareProperty("UniformClst",    m_fUniformClst=4);
}

MucCalibConstSvc::~MucCalibConstSvc() { }

StatusCode MucCalibConstSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if( IID_IMucCalibConstSvc.versionMatch(riid) ){
    *ppvInterface = static_cast<IMucCalibConstSvc*> (this);
  } else {
    return Service::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}

StatusCode MucCalibConstSvc::initialize()
{
  MsgStream log(messageService(), name());
  log << MSG::INFO << endreq << "initialize()" << endreq << endreq;
  log << MSG::INFO << "Config mode:\t" << m_fConfigMode << endreq;  
  StatusCode sc = Service::initialize();
  if( sc.isFailure() ) return sc;
  
  sc = service("CalibDataSvc", m_pCalibDataSvc, true);
  if( sc == StatusCode::SUCCESS ) {
    log << MSG::INFO << "Retrieve IDataProviderSvc" << endreq;
  }else{
    log << MSG::FATAL << "can not get IDataProviderSvc" << endreq;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode MucCalibConstSvc::finalize()
{
  MsgStream log(messageService(), name());
  log << MSG::INFO << endreq << "finalize()" << endreq << endreq;
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------------|
//------------------------------- Access by users -----------------------------------|
//-----------------------------------------------------------------------------------|

// Get config level
inline int MucCalibConstSvc::getLevel() const {return  m_fConfigMode;}

// Get efficiency(Eff)
inline double MucCalibConstSvc::getEff(int part, int segment, int layer, int strip ) const
{
  double eff = 0.0;
  MsgStream log(messageService(), name());
  //	log << MSG::INFO << "In MucCalibDataSvc" << endreq;
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "getEff() can not access to MucCalibData via SmartPtr" << endreq;
  }else
  {
    switch( m_fConfigMode ) 
    {  
      case 0 : 
              if( m_fUniformEff < 0 ) 
                eff = pMucCalibConst->getUniformEff();
              else
                eff = m_fUniformEff;
              break;
      case 1 :
              eff = pMucCalibConst->getLayerEff( layer );
              break;
      case 2 :
              eff = pMucCalibConst->getBoxEff( part, segment, layer );
              break;
      case 3 :
              eff = pMucCalibConst->getStripEff( part, segment, layer, strip ); 
              break;
      default: ;	
    }
  }
  
  return eff;     
}

// Get counting rate(Cnt)
inline double MucCalibConstSvc::getCnt(int part, int segment, int layer, int strip ) const
{
  double cnt = 0.0;
  MsgStream log(messageService(), name());
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "getCnt() can not access to MucCalibData via SmartPtr" << endreq;
  }else
  { 
    switch( m_fConfigMode ) 
    {  
      case 0 : 
              if( m_fUniformCnt < 0 ) 
                cnt = pMucCalibConst->getUniformCnt();
              else 
                cnt = m_fUniformCnt;
              break;
      case 1 :
              cnt = pMucCalibConst->getLayerCnt( layer );
              break;
      case 2 :
              cnt = pMucCalibConst->getBoxCnt( part, segment, layer );
              break;
      case 3 :
              cnt = pMucCalibConst->getStripCnt( part, segment, layer, strip ); 
              break;
      default: ;	
    }  
  }
  
  return cnt;     
}

// Get noise(Nos)
inline double MucCalibConstSvc::getNos(int part, int segment, int layer, int strip ) const
{
  double nos = 0.0;
  MsgStream log(messageService(), name());
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "getNos() can not access to MucCalibData via SmartPtr" << endreq;
  }else
  { 
    switch( m_fConfigMode ) 
    {  
      case 0 : 
              if( m_fUniformNos < 0 ) 
                nos = pMucCalibConst->getUniformNos();
              else 
                nos = m_fUniformNos;
              break;
      case 1 :
              nos = pMucCalibConst->getLayerNos( layer );
              break;
      case 2 :
              nos = pMucCalibConst->getBoxNos( part, segment, layer );
              break;
      case 3 :
              nos = pMucCalibConst->getStripNos( part, segment, layer, strip ); 
              break;
      default: ;	
    }  
  }
  
  return nos;     
}

// Get noise ratio(NosRatio)
inline double MucCalibConstSvc::getNosRatio(int part, int segment, int layer, int strip ) const
{
  double nosRatio = 0.0;
  MsgStream log(messageService(), name());
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "getNosRatio() can not access to MucCalibData via SmartPtr" << endreq;
  }else
  { 
    switch( m_fConfigMode ) 
    {  
      case 0 : 
              if( m_fUniformNosRatio < 0 ) 
                nosRatio = pMucCalibConst->getUniformNosRatio();
              else 
                nosRatio = m_fUniformNosRatio;
              break;
      case 1 :
              nosRatio = pMucCalibConst->getLayerNosRatio( layer );
              break;
      case 2 :
              nosRatio = pMucCalibConst->getBoxNosRatio( part, segment, layer );
              break;
      case 3 :
              nosRatio = pMucCalibConst->getStripNosRatio( part, segment, layer, strip ); 
              break;
      default: ;	
    }  
  }
  
  return nosRatio;     
}

// Get cluster size(Clst) 
inline double MucCalibConstSvc::getClst(int part, int segment, int layer, double prob ) const
{
  double cluster = 0.0;
  MsgStream log(messageService(), name());
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
  log << MSG::ERROR << "getClst() can not access to MucCalibData via SmartPtr" << endreq;
  }else 
  {
    switch( m_fConfigMode ) 
    {
      case 0 : 
              if( m_fUniformClst < 0 )
                cluster = pMucCalibConst->getUniformClst();
              else
                cluster = m_fUniformClst;
              break;
      case 1 :
              cluster = pMucCalibConst->getLayerClst( layer, prob );
              break;
      case 2 :
              cluster = pMucCalibConst->getBoxClst( part, segment, layer, prob );
              break;
      case 3 :
              cluster = 0.0; 
              break;
      default: ;	
    }
  }
  
  return cluster;     
}


// Uniform constants
inline double MucCalibConstSvc::getUniformEff() const
{
  double fUniformEff = 0.0;
  MsgStream log(messageService(), name());
  
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "can not access to MucCalibData via SmartPtr" << endreq;
  }else {
    fUniformEff= pMucCalibConst->getUniformEff();
  }
     
  return fUniformEff;
}

inline double MucCalibConstSvc::getUniformCnt() const
{
  double fUniformCnt = 0.0;
  MsgStream log(messageService(), name());

  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "can not access to MucCalibData via SmartPtr" << endreq;
  }else {
    fUniformCnt= pMucCalibConst->getUniformCnt();
  }

  return fUniformCnt;
}

inline double MucCalibConstSvc::getUniformNosRatio() const
{
  double fUniformNosRatio = 0.0;
  MsgStream log(messageService(), name());

  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "can not access to MucCalibData via SmartPtr" << endreq;
  }else {
    fUniformNosRatio= pMucCalibConst->getUniformNosRatio();
  }

  return fUniformNosRatio;
}


inline double MucCalibConstSvc::getUniformNos() const
{
  double fUniformNos = 0.0;
  MsgStream log(messageService(), name());
  
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "can not access to MucCalibData via SmartPtr" << endreq;
  }else {
    fUniformNos= pMucCalibConst->getUniformNos();
  }
  
  return fUniformNos;
}

inline double MucCalibConstSvc::getUniformClst() const
{
  double fUniformClst = 0.0;
  MsgStream log(messageService(), name());
  
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "can not access to MucCalibData via SmartPtr" << endreq;
  }else {
    fUniformClst= pMucCalibConst->getUniformClst();
  }
  
  return fUniformClst;
}
 
// Level0, layer constants
inline double MucCalibConstSvc::getLayerEff( int layer ) const 
{
  double fEff = 0.0;
  MsgStream log(messageService(), name());
  
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "can not access to MucCalibData via SmartPtr" << endreq;
  }else {
    fEff= pMucCalibConst->getLayerEff( layer );
  }
       
  // log << MSG::DEBUG << "Layer:\t" << layer << "\t" << fEff << endreq; 
  return fEff;
}

inline double MucCalibConstSvc::getLayerCnt( int layer ) const
{
  double fCnt = 0.0;
  MsgStream log(messageService(), name());
  
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "can not access to MucCalibData via SmartPtr" << endreq;
  }else {
    fCnt= pMucCalibConst->getLayerCnt( layer );
  }
  
  return fCnt;
}

inline double MucCalibConstSvc::getLayerNos( int layer ) const
{
  double fNos = 0.0;
  MsgStream log(messageService(), name());

  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "can not access to MucCalibData via SmartPtr" << endreq;
  }else {
    fNos= pMucCalibConst->getLayerNos( layer );
  }

  return fNos;
}

inline double MucCalibConstSvc::getLayerNosRatio( int layer ) const
{
  double fNosRatio = 0.0;
  MsgStream log(messageService(), name());

  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "can not access to MucCalibData via SmartPtr" << endreq;
  }else {
    fNosRatio= pMucCalibConst->getLayerNosRatio( layer );
  }

  return fNosRatio;
}

double MucCalibConstSvc::getLayerClst( int layer, double prob ) const
{
  double fClst = 0.0;
  MsgStream log(messageService(), name());
  
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "can not access to MucCalibData via SmartPtr" << endreq;
  }else {
    fClst= pMucCalibConst->getLayerClst( layer, prob );
  }
       
  return fClst;	
}

// Level1, box constants
inline double MucCalibConstSvc::getBoxEff( int part, int segment, int layer ) const
{
  double fEff = 0.0;
  MsgStream log(messageService(), name());
  
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "can not access to MucCalibData via SmartPtr" << endreq;
  }else {
    fEff= pMucCalibConst->getBoxEff( part, segment, layer );
  }
       
  // log << MSG::DEBUG << "Box:\t" << part << "\t" << segment <<"\t" << layer << "\t" << fEff << endreq; 
  return fEff;			
}

inline double MucCalibConstSvc::getBoxCnt( int part, int segment, int layer ) const
{
  double fCnt = 0.0;
  MsgStream log(messageService(), name());
  
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "can not access to MucCalibData via SmartPtr" << endreq;
  }else {
    fCnt= pMucCalibConst->getBoxCnt( part, segment, layer );
  }
  
  return fCnt;	
}

inline double MucCalibConstSvc::getBoxNos( int part, int segment, int layer ) const
{
  double fNos = 0.0;
  MsgStream log(messageService(), name());
  
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "can not access to MucCalibData via SmartPtr" << endreq;
  }else {
    fNos= pMucCalibConst->getBoxNos( part, segment, layer );
  }
  
  return fNos;	
}

inline double MucCalibConstSvc::getBoxNosRatio( int part, int segment, int layer ) const
{
  double fNosRatio = 0.0;
  MsgStream log(messageService(), name());
  
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "can not access to MucCalibData via SmartPtr" << endreq;
  }else {
    fNosRatio= pMucCalibConst->getBoxNosRatio( part, segment, layer );
  }
  
  return fNosRatio;	
}

double MucCalibConstSvc::getBoxClst( int part, int segment, int layer, double prob ) const
{
  double fClst = 0.0;
  MsgStream log(messageService(), name());
  
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "can not access to MucCalibData via SmartPtr" << endreq;
  }else {
    fClst= pMucCalibConst->getBoxClst( part, segment, layer, prob );
  }
       
  return fClst;		
}

// Level2, strip constants
inline double MucCalibConstSvc::getStripEff( int part, int segment, int layer, int strip ) const
{
  double fEff = 0.0;
  MsgStream log(messageService(), name());
  
  log << MSG::INFO << "in  MucCalibConstSvc::getStripEff" << endreq;
  
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "can not access to MucCalibData via SmartPtr" << endreq;
  }else {  
    fEff = pMucCalibConst->getStripEff( part, segment, layer, strip );
  }
     
  // log << MSG::DEBUG << "Strip:\t" << part << "\t" << segment <<"\t" << layer << "\t" << strip << "\t" << fEff << endreq; 
  return fEff;	
}

inline double MucCalibConstSvc::getStripCnt( int part, int segment, int layer, int strip ) const
{
  double fCnt = 0.0;
  MsgStream log(messageService(), name());
  
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "can not access to MucCalibData via SmartPtr" << endreq;
  }else {
    fCnt= pMucCalibConst->getStripCnt( part, segment, layer, strip );
  }
       
  return fCnt;	
}

inline double MucCalibConstSvc::getStripNos( int part, int segment, int layer, int strip ) const
{
  double fNos = 0.0;
  MsgStream log(messageService(), name());
  
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "can not access to MucCalibData via SmartPtr" << endreq;
  }else {
    fNos= pMucCalibConst->getStripNos( part, segment, layer, strip );
  }
       
  return fNos;	
}

inline double MucCalibConstSvc::getStripNosRatio( int part, int segment, int layer, int strip ) const
{
  double fNosRatio = 0.0;
  MsgStream log(messageService(), name());
  
  std::string fullPath = "/Calib/MucCal";
  SmartDataPtr<CalibData::MucCalibData> pMucCalibConst(m_pCalibDataSvc, fullPath);
  if( ! pMucCalibConst ){
    log << MSG::ERROR << "can not access to MucCalibData via SmartPtr" << endreq;
  }else {
    fNosRatio= pMucCalibConst->getStripNosRatio( part, segment, layer, strip );
  }
       
  return fNosRatio;	
}

//END
