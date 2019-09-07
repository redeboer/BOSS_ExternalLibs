#include "MdcAlignAlg/MdcAliRecTrk.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

const double MdcAliRecTrk::m_df = 10.0;

MdcAliRecTrk::~MdcAliRecTrk(){
     unsigned int i;
     for(i=0; i<m_rechit.size(); i++){
	  delete m_rechit[i];
     }
     m_rechit.clear();
}

void MdcAliRecTrk::setRecTrk(RecMdcTrackCol::iterator it_trk){
     IMessageSvc *msgSvc;
     Gaudi::svcLocator()->service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcAliRecTrk");
     log << MSG::DEBUG << "MdcAliRecTrk::setRecTrk()" << endreq;

     m_stat = (*it_trk)->stat();
     m_dr = (*it_trk) -> helix(0);
     m_phi0 = (*it_trk) -> helix(1);
     m_kappa = (*it_trk) -> helix(2);
     m_dz = (*it_trk) -> helix(3);
     m_tanl = (*it_trk) -> helix(4);

     m_helix = (*it_trk)->helix();
     m_helixerr = (*it_trk)->err();

     m_chisq = (*it_trk) -> chi2();
     m_nhits = (*it_trk) -> getNhits();

     m_dr *= m_df;		// cm -> mm
     m_dz *= m_df;		// cm -> mm

     if(m_kappa < 0.01){	// no magnetic field
	  m_pt = 0.0;
	  m_p = 0.0;
     }else{
	  m_pt = 1.0 / m_kappa;
	  m_p = m_pt * sqrt(m_tanl * m_tanl + 1.0);
     }

     HitRefVec gothits = (*it_trk) -> getVecHits();
     HitRefVec::iterator it_hit = gothits.begin();
     MdcAliRecHit* rechit;
     for(; it_hit != gothits.end(); it_hit++){
	  rechit = new MdcAliRecHit();
	  rechit->setRecHit(it_hit);
	  m_rechit.push_back(rechit);
     }
}

void MdcAliRecTrk::setKalTrk(RecMdcKalTrackCol::iterator it_trk){
     IMessageSvc *msgSvc;
     Gaudi::svcLocator()->service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcAliRecTrk");
     log << MSG::DEBUG << "MdcAliRecTrk::setKalTrk()" << endreq;

     int m_pid = 1;
     if(0 == m_pid) RecMdcKalTrack::setPidType(RecMdcKalTrack::electron);
     else if(1 == m_pid) RecMdcKalTrack::setPidType(RecMdcKalTrack::muon);
     else RecMdcKalTrack::setPidType(RecMdcKalTrack::electron);

     m_stat = 0;
     m_dr = (*it_trk)->dr();
     m_phi0 = (*it_trk)->fi0();
     m_kappa = (*it_trk)->kappa();
     m_dz = (*it_trk)->dz();
     m_tanl = (*it_trk)->tanl();
     m_chisq = (*it_trk)->chi2();

     m_dr *= 10.0;		// cm -> mm
     m_dz *= 10.0;		// cm -> mm

     m_pt = 1.0 / fabs(m_kappa);
     m_p = m_pt * sqrt( m_tanl * m_tanl + 1.0 );

     HelixSegRefVec gothelixsegs = (*it_trk)->getVecHelixSegs();
     HelixSegRefVec::iterator it_hit = gothelixsegs.begin();
     MdcAliRecHit* rechit;

     int k = 0;
     for(; it_hit != gothelixsegs.end(); it_hit++){
	  rechit = new MdcAliRecHit();
	  rechit->setKalHit(it_hit);
	  m_rechit.push_back(rechit);

	  k++;
     }
     m_nhits = k;
}
