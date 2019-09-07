#include "MdcCalibAlg/MdcCalRecHit.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include <iostream>

const double MdcCalRecHit::m_df = 10.0;
const double MdcCalRecHit::m_qf = 1.0;

void MdcCalRecHit::setRecHit(HitRefVec::iterator it_hit){
     IMessageSvc *msgSvc;
     Gaudi::svcLocator()->service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcCalRecHit");
     log << MSG::DEBUG << "MdcCalRecHit::setRecHit()" << endreq;

     Identifier identifier;
     MdcID mdcid;

//      log << MSG::DEBUG << "hit Id: " << (*it_hit)->getId()
// 	       << "  hits drift time " << (*it_hit)->getDriftT()
//       	 << "  hits doca " << (*it_hit)->getDoca()
// 	       << endreq;

     identifier = (*it_hit)->getMdcId();
     m_layid = mdcid.layer(identifier);
     m_cellid = mdcid.wire(identifier);
     m_lr = (*it_hit) -> getFlagLR();
     m_stat = (*it_hit) -> getStat();

     m_docaInc = (*it_hit) -> getDoca();
     m_docaExc = m_docaInc;

     if( 0 == m_lr ){
	  m_dmeas = (*it_hit) -> getDriftDistLeft();
	  m_errdmeas = (*it_hit) -> getErrDriftDistLeft();
     }else{
	  m_dmeas = (*it_hit) -> getDriftDistRight();
	  m_errdmeas = (*it_hit) -> getErrDriftDistRight();
     }

     m_tdc = (*it_hit) -> getTdc();
     m_tdrift = (*it_hit) -> getDriftT();

     m_qhit = (*it_hit) -> getAdc();
     m_zhit = (*it_hit) -> getZhit();
     m_entra = (*it_hit) -> getEntra();
     m_chisqadd = (*it_hit) -> getChisqAdd();

     double flen = (*it_hit) -> getFltLen(); // cm
     m_tof = flen / 30.0;

     // the following is for cm to mm
     m_docaInc *= m_df;
     m_docaExc *= m_df;
     m_dmeas *= m_df;
     m_errdmeas *= m_df;
     m_zhit *= m_df;
     m_qhit *= m_qf;

     m_resiInc = fabs(m_dmeas) - fabs(m_docaInc);
     if( 0 == m_lr )   m_resilrInc = -1.0 * m_resiInc;
     else              m_resilrInc = m_resiInc;

     m_resiExc = fabs(m_dmeas) - fabs(m_docaExc);
     if( 0 == m_lr )   m_resilrExc = -1.0 * m_resiExc;
     else              m_resilrExc = m_resiExc;
}

void MdcCalRecHit::setKalHit(HelixSegRefVec::iterator it_hit){
     IMessageSvc *msgSvc;
     Gaudi::svcLocator()->service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcCalRecHit");
     log << MSG::DEBUG << "MdcCalRecHit::setKalHit()" << endreq;

     Identifier identifier;
     MdcID mdcid;

     identifier = (*it_hit) -> getMdcId();
     m_layid = mdcid.layer(identifier);
     m_cellid = mdcid.wire(identifier);
     m_stat = 1;

     m_lr = (*it_hit) -> getFlagLR();
     if(-1 == m_lr) m_lr = 0;	// definition not same as MdcRecHit

     m_docaInc = (*it_hit) -> getDocaIncl();
     m_docaExc = (*it_hit) -> getDocaExcl();
     if(1 == m_lr){
	  m_docaInc *= -1.0;
	  m_docaExc *= -1.0;
     }

     m_dmeas = (*it_hit) -> getDD();
     m_errdmeas = 0.013;

     m_tdc = (*it_hit) -> getTdc();
     m_tdrift = (*it_hit) -> getDT();

     m_qhit = (*it_hit) -> getAdc();
     m_zhit = (*it_hit) -> getZhit();
     m_entra = (*it_hit) -> getEntra();
     m_chisqadd = 0.0;
     m_tof = (*it_hit) -> getTof();

     m_Dr = (*it_hit) -> getDrExcl();
     m_Fi0 = (*it_hit) -> getFi0Excl();
     m_Cpa = (*it_hit) -> getCpaExcl();
     m_Dz = (*it_hit) -> getDzExcl();
     m_Tanl = (*it_hit) -> getTanlExcl();
     m_helix = (*it_hit)->getHelixExcl();
     m_helixerr = (*it_hit)->getErrorExcl();

     // the following is for cm to mm
     m_docaInc *= m_df;
     m_docaExc *= m_df;
     m_dmeas *= m_df;
     m_errdmeas *= m_df;
     m_zhit *= m_df;
     m_qhit *= m_qf;

     m_resiInc = fabs(m_dmeas) - fabs(m_docaInc);
     if( 0 == m_lr )   m_resilrInc = -1.0 * m_resiInc;
     else              m_resilrInc = m_resiInc;

     m_resiExc = fabs(m_dmeas) - fabs(m_docaExc);
     if( 0 == m_lr )   m_resilrExc = -1.0 * m_resiExc;
     else              m_resilrExc = m_resiExc;

}

