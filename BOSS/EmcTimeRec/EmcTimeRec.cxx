#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "EventModel/EventHeader.h"
#include "EmcRecEventModel/RecEmcEventModel.h"
#include "EvTimeEvent/RecEsTime.h"
#include "EmcTimeRec/EmcTimeRec.h"

using namespace std;
using namespace Event;

/////////////////////////////////////////////////////////////////////////////

EmcTimeRec::EmcTimeRec(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{   
  declareProperty("Output",m_output=false);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EmcTimeRec::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  if(m_output) {
    NTuplePtr nt(ntupleSvc(),"FILE303/n1");
    if ( nt ) m_tuple = nt;
    else {
      m_tuple=ntupleSvc()->book("FILE303/n1",CLID_ColumnWiseTuple,"EmcTimeRec");
      if( m_tuple ) {
        m_tuple->addItem ("emcTime", m_nt_emcTime);
        m_tuple->addItem ("estTime", m_nt_estTime);
        m_tuple->addItem ("corTime", m_nt_corTime);
        m_tuple->addItem ("npart",   m_nt_npart);
        m_tuple->addItem ("ntheta",  m_nt_ntheta);
        m_tuple->addItem ("nphi",    m_nt_nphi);
        m_tuple->addItem ("e5x5",    m_nt_e5x5);
        m_tuple->addItem ("status",  m_nt_status);
      }
      else    {   // did not manage to book the N tuple....
        log << MSG::ERROR << "Cannot book N-tuple:" << long(m_tuple) << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EmcTimeRec::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in execute()" << endreq;   

  SmartDataPtr<RecEsTimeCol> estimeCol(eventSvc(),"/Event/Recon/RecEsTimeCol");

  if( !estimeCol || ( estimeCol->size() == 0 ) ) {
    log << MSG::WARNING << "Could not find RecEsTimeCol!" << endreq;
    return StatusCode::SUCCESS;
  } 

  RecEsTimeCol::iterator iter_ESTime=estimeCol->begin();
  double t0  = (*iter_ESTime)->getTest(); //get t0

  SmartDataPtr<RecEmcShowerCol> emcRecShowerCol(eventSvc(), EventModel::Recon::RecEmcShowerCol);
  if(!emcRecShowerCol) {
    log << MSG::WARNING << "Could not find RecEmcShowerCol!" << endreq;
    return StatusCode::SUCCESS;
  } 

  RecEmcShowerCol::iterator iShowerCol;
  for(iShowerCol=emcRecShowerCol->begin();
      iShowerCol!=emcRecShowerCol->end();
      iShowerCol++) {

    if((*iShowerCol)->status()>=100) continue;  //estTime has been subtracted

    double tEmc = (*iShowerCol)->time();
    (*iShowerCol)->setTime(tEmc-(int)(t0/50));
    (*iShowerCol)->setStatus((*iShowerCol)->status()%100+100);

    if(m_output) {
      m_nt_emcTime = tEmc;
      m_nt_estTime = (double)(int)(t0/50);
      m_nt_corTime = (*iShowerCol)->time();
      RecEmcID id((*iShowerCol)->getShowerId());
      m_nt_npart = EmcID::barrel_ec(id);
      m_nt_ntheta = EmcID::theta_module(id);
      m_nt_nphi = EmcID::phi_module(id);
      m_nt_e5x5 = (*iShowerCol)->e5x5();
      m_nt_status = (*iShowerCol)->status();
      m_tuple->write();
    }
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EmcTimeRec::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;

  return StatusCode::SUCCESS;
}
