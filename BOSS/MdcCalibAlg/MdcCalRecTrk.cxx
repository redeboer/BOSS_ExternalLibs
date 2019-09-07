#include "MdcCalibAlg/MdcCalRecTrk.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "MdcRawEvent/MdcDigi.h"
#include "McTruth/MdcMcHit.h"
#include "CLHEP/Units/PhysicalConstants.h"

MdcCalRecTrk::MdcCalRecTrk(int pid){
     m_pid = pid;
}

MdcCalRecTrk::~MdcCalRecTrk(){
     unsigned int i;
     for(i=0; i<m_rechit.size(); i++){
	  delete m_rechit[i];
     }
     m_rechit.clear();
}

void MdcCalRecTrk::setRecTrk(RecMdcTrackCol::iterator it_trk){
     IMessageSvc *msgSvc;
     Gaudi::svcLocator()->service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcCalRecTrk");
     log << MSG::DEBUG << "MdcCalRecTrk::setRecTrk()" << endreq;

     m_dr = (*it_trk)->helix(0);
     m_phi0 = (*it_trk)->helix(1);
     m_kappa = (*it_trk)->helix(2);
     m_dz = (*it_trk)->helix(3);
     m_tanl = (*it_trk)->helix(4);
     m_chisq = (*it_trk)->chi2();
     m_nhits = (*it_trk)->getNhits();
     m_helix = (*it_trk)->helix();
     m_helixerr = (*it_trk)->err();
     m_fgNoiseRatio = fgNoiseRatio(m_phi0);

     double mass;
     if(0 == m_pid) mass = 0.000511;
     else if(1 == m_pid) mass = 0.105658;
     else if(2 == m_pid) mass = 0.139570;
     else if(3 == m_pid) mass = 0.493677;
     else if(4 == m_pid) mass = 0.938272;
     else if(5 == m_pid) mass = 0.139570; // cosmic-ray
     else log << MSG::FATAL << "wrong particle type" << endreq;
     m_p4 = (*it_trk)->p4(mass);

     m_dr *= 10.0;		// cm -> mm
     m_dz *= 10.0;		// cm -> mm

     m_pt = 1.0 / m_kappa;
     m_p = m_pt * sqrt( m_tanl * m_tanl + 1.0 );

     HitRefVec gothits = (*it_trk) -> getVecHits();
     HitRefVec::iterator it_hit = gothits.begin();
     MdcCalRecHit* rechit;
     for(; it_hit != gothits.end(); it_hit++){
	  rechit = new MdcCalRecHit();
	  rechit->setRecHit(it_hit);
	  m_rechit.push_back(rechit);
     }
}

void MdcCalRecTrk::setKalTrk(RecMdcKalTrackCol::iterator it_trk){
     IMessageSvc *msgSvc;
     Gaudi::svcLocator()->service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcCalRecTrk");
     log << MSG::DEBUG << "MdcCalRecTrk::setKalTrk()" << endreq;

     if(0 == m_pid) RecMdcKalTrack::setPidType(RecMdcKalTrack::electron);
     else if(1 == m_pid) RecMdcKalTrack::setPidType(RecMdcKalTrack::muon);
     else if(2 == m_pid) RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);
     else if(3 == m_pid) RecMdcKalTrack::setPidType(RecMdcKalTrack::kaon);
     else if(4 == m_pid) RecMdcKalTrack::setPidType(RecMdcKalTrack::proton);
     else if(5 == m_pid) RecMdcKalTrack::setPidType(RecMdcKalTrack::muon); // cosmic-ray
     else log << MSG::FATAL << "wrong particle type" << endreq;

     m_dr = (*it_trk)->dr();
     m_phi0 = (*it_trk)->fi0();
     m_kappa = (*it_trk)->kappa();
     m_dz = (*it_trk)->dz();
     m_tanl = (*it_trk)->tanl();
     m_chisq = (*it_trk)->chi2();
     m_fgNoiseRatio = fgNoiseRatio(m_phi0);

     m_p4.setPx(- sin(m_phi0) / fabs(m_kappa));
     m_p4.setPy(cos(m_phi0) / fabs(m_kappa));
     m_p4.setPz(m_tanl / fabs(m_kappa));
     double p3 = m_p4.mag();
     double mass;
     if(0 == m_pid) mass = 0.000511;
     else if(1 == m_pid) mass = 0.105658;
     else if(2 == m_pid) mass = 0.139570;
     else if(3 == m_pid) mass = 0.493677;
     else if(4 == m_pid) mass = 0.938272;
     else if(5 == m_pid) mass = 0.139570; // cosmic-ray
     m_p4.setE(sqrt(p3 * p3 + mass * mass));

     m_dr *= 10.0;		// cm -> mm
     m_dz *= 10.0;		// cm -> mm

     m_pt = 1.0 / m_kappa;
     m_p = m_pt * sqrt( m_tanl * m_tanl + 1.0 );

//      cout << setw(15) << m_p << setw(15) << m_dr << setw(15) << m_phi0
// 	  << setw(15) << m_kappa << setw(15) << m_dz << setw(15) << m_tanl << endl;

     HelixSegRefVec gothelixsegs = (*it_trk)->getVecHelixSegs();
     HelixSegRefVec::iterator it_hit = gothelixsegs.begin();
     MdcCalRecHit* rechit;

     int k = 0;
     for(; it_hit != gothelixsegs.end(); it_hit++){
	  rechit = new MdcCalRecHit();
	  rechit->setKalHit(it_hit);
	  m_rechit.push_back(rechit);

	  k++;
     }
     m_nhits = k;
//      cout<<"hits "<<m_nhits <<endl;
}

bool MdcCalRecTrk::fgNoiseRatio(double phi0){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcCalib");

     IDataProviderSvc* eventSvc = NULL;
     Gaudi::svcLocator()->service("EventDataSvc", eventSvc);
     SmartDataPtr<MdcDigiCol> mdcDigiCol(eventSvc,"/Event/Digi/MdcDigiCol");
     if (!mdcDigiCol) {
          log << MSG::FATAL << "Could not find event" << endreq;
     }

     int lay;
     int cel;
     bool hitCel[43][288];
     for(lay=0; lay<43; lay++){
	  for(cel=0; cel<288; cel++){
	       hitCel[lay][cel] = false;
	  }
     }

     MdcDigiCol::iterator iter = mdcDigiCol->begin();
     unsigned fgOverFlow;
     int digiId = 0;
     Identifier id;
     for(; iter != mdcDigiCol->end(); iter++, digiId++) {
          MdcDigi *aDigi = (*iter);
          id = (aDigi)->identify();
          lay = MdcID::layer(id);
          cel = MdcID::wire(id);
	  fgOverFlow = (aDigi) -> getOverflow();

	  bool goodTQ = true;
	  if ( ((fgOverFlow & 3) !=0 ) || ((fgOverFlow & 12) != 0) ||
	       (aDigi->getTimeChannel() == 0x7FFFFFFF) ||
	       (aDigi->getChargeChannel() == 0x7FFFFFFF) ) goodTQ = false;

	  bool goodT = true;
	  if ( ((fgOverFlow & 1) !=0 ) || (aDigi->getTimeChannel() == 0x7FFFFFFF) ) goodT = false;

// 	  if(!goodTQ) continue;
	  if(!goodT) continue;

	  hitCel[lay][cel] = true;
     }

     SmartDataPtr<RecMdcTrackCol> newtrkCol(eventSvc, "/Event/Recon/RecMdcTrackCol");
     if(!newtrkCol){
	  log << MSG::ERROR << "Could not find RecMdcTrackCol" << endreq;
	  return -1;
     }

     int nNoiRange = 4;		// number of cells
     double dphi = 1.0;
     RecMdcTrackCol::iterator it_trk = newtrkCol->begin();
     for(it_trk = newtrkCol->begin(); it_trk != newtrkCol->end(); it_trk++){
	  HitRefVec gothits = (*it_trk) -> getVecHits();
	  HitRefVec::iterator it_hit = gothits.begin();
	  HepVector helix = (*it_trk)->helix();
	  HepSymMatrix helixErr = (*it_trk)->err();
	  double phi0Rec = (*it_trk)->helix(1);
	  double delphi;
	  if(phi0Rec>phi0) delphi = phi0Rec - phi0;
	  else delphi = phi0 - phi0Rec;
	  if(delphi > CLHEP::pi) delphi -= CLHEP::pi;
	  if(delphi > (CLHEP::pi*0.17)) continue;

	  int nhitLay1 = 0;	// lay0-3
	  int nhitLay2 = 0;	// lay4-7
	  int nhitLay3 = 0;	// lay8-19
	  int nhitLay4 = 0;	// lay20-42
	  int nhitT1 = 0;
	  int nhitT2 = 0;
	  int nhitT3 = 0;
	  int nhitT4 = 0;
	  for(lay=0; lay<8; lay++){
	       double docamin = 0.7; // cm
	       if(lay>7) docamin = 0.9;
	       int celmin = -1;
	       int ncel = m_mdcGeomSvc->Layer(lay)->NCell();
	       for(cel=0; cel<ncel; cel++){
		    double wphi;
		    const MdcGeoWire* pWire = m_mdcGeomSvc -> Wire(lay, cel);
		    double xx = pWire->Backward().x();
		    double yy = pWire->Backward().y();
		    double rr = sqrt( (xx * xx) + (yy * yy) );
		    if( yy >= 0 )  wphi = acos(xx / rr);
		    else           wphi = CLHEP::twopi - acos(xx / rr);

		    double phiTrk = phi0 + CLHEP::halfpi;
		    if(phiTrk > CLHEP::twopi) phiTrk -= CLHEP::twopi;
		    if( !( (fabs(wphi-phiTrk) < dphi) || (fabs(wphi+CLHEP::twopi-phiTrk) < dphi) ||
			   (fabs(wphi-CLHEP::twopi-phiTrk) < dphi) ) ){
			 continue;
		    }

		    double doca = m_mdcUtilitySvc->doca(lay, cel, helix, helixErr);
// 		    cout << setw(5) << lay << setw(5) << cel << setw(15) << doca << endl;
		    if(fabs(doca) < fabs(docamin)){
			 docamin = doca;
			 celmin = cel;
		    }
	       }
	       if(celmin > -1){
		    if(lay<4) nhitLay1++;
		    else if(lay<8) nhitLay2++;
		    else if(lay<20) nhitLay3++;
		    else nhitLay4++;
		    for(int ii=(-nNoiRange); ii<=nNoiRange; ii++){
			 if(0==ii) continue;
			 int icell = celmin + ii;
			 if(icell >= ncel) icell -= ncel;
			 if(icell < 0) icell += ncel;
// 			 cout << "hit " << setw(5) << lay << setw(5) << celmin << setw(5) << icell << setw(5) << hitCel[lay][icell]<<endl;
			 if(hitCel[lay][icell]){
			      if(lay<4) nhitT1++;
			      else if(lay<8) nhitT2++;
			      else if(lay<20) nhitT3++;
			      else nhitT4++;
			 }
		    }
	       }
	  }
	  if((nhitLay1<=0) || (nhitLay2<=0) || (nhitLay3<=0) || (nhitLay4<=0)) return false;
	  double ratio1 = (double)nhitT1 / ((double)nhitLay1 * (double)nNoiRange*2.0);
	  double ratio2 = (double)nhitT2 / ((double)nhitLay2 * (double)nNoiRange*2.0);
	  double ratio3 = (double)nhitT3 / ((double)nhitLay3 * (double)nNoiRange*2.0);
	  double ratio4 = (double)nhitT4 / ((double)nhitLay4 * (double)nNoiRange*2.0);

	  if((ratio1>0.08) || (ratio2>0.08) || (ratio3>0.03) || (ratio4>0.03)) return false;
	  else return true;
     }
     return false;
}
