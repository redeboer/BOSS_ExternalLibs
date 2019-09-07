#include "MdcAlignAlg/ResiAlign.h"
#include "MdcAlignAlg/MdcAlignAlg.h"
#include "MdcAlignAlg/Alignment.h"

#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "MdcRawEvent/MdcDigi.h"
#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"

#include "TF1.h" 
#include "TCanvas.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>

using namespace std;

ResiAlign::ResiAlign(){
     //m_ndiv = 6;
     m_ndiv = 12;
     m_resiCut = 1.0;
     for(int i=0; i<NEP; i++) m_npoint[i] = 0;

     for(int lay=0; lay<LAYERNMAX; lay++){
	  if(lay < 8){
	       m_docaMin[lay] = 0.6; // mm
	       m_docaMax[lay] = 4.8; // mm
	  } else{
	       m_docaMin[lay] = 0.8; // mm
	       m_docaMax[lay] = 6.4; // mm
	  }
     }
     for(int lay=0; lay<LAYERNMAX; lay++){
	  if((0==lay) || (7==lay) || (8==lay) || (19==lay) || (20==lay) ||
	     (35==lay) || (36==lay) || (42==lay) ) m_layBound[lay] = true;
	  else m_layBound[lay] = false;
     }

     m_ncut1 = 0;
     m_ncut2 = 0;
     m_ncut3 = 0;
     m_ncut4 = 0;
     m_ncut5 = 0;
     m_ncut6 = 0;
     m_ncut7 = 0;
     m_ncut8 = 0;
     m_ncut9 = 0;
     m_ncut10 = 0;
     m_ncut11 = 0;
     m_ncut12 = 0;
     m_ncut13 = 0;
}

ResiAlign::~ResiAlign(){
}

void ResiAlign::clear(){
     delete m_hresAll;
     delete m_hresInn;
     delete m_hresStp;
     delete m_hresOut;
     for(int lay=0; lay<LAYERNMAX; lay++) delete m_hresLay[lay];
     for(int i=0; i<NEP; i++) delete m_gr[i];
}

void ResiAlign::initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
			   IMdcCalibFunSvc* mdcFunSvc){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "ResiAlign");
     log << MSG::INFO << "ResiAlign::initialize()" << endreq;

     m_hlist = hlist;
     m_mdcGeomSvc = mdcGeomSvc;
     m_mdcFunSvc = mdcFunSvc;

     double zeast;
     for(int lay=0; lay<43; lay++){
	  zeast = m_mdcGeomSvc->Wire(lay, 0)->Backward().z();
	  m_zrange[lay][1] = 2.0 * fabs(zeast) / (double)m_ndiv;
	  m_zrange[lay][0] = -1.0 * m_zrange[lay][1];

	  m_radii[lay] = m_mdcGeomSvc->Layer(lay)->Radius();
     }

     for(int wir=0; wir<WIRENMAX; wir++){
	  m_xe[wir] = m_mdcGeomSvc->Wire(wir)->Backward().x();
	  m_ye[wir] = m_mdcGeomSvc->Wire(wir)->Backward().y();
	  m_ze[wir] = m_mdcGeomSvc->Wire(wir)->Backward().z();
	  m_xw[wir] = m_mdcGeomSvc->Wire(wir)->Forward().x();
	  m_yw[wir] = m_mdcGeomSvc->Wire(wir)->Forward().y();
	  m_zw[wir] = m_mdcGeomSvc->Wire(wir)->Forward().z();
     }

     char hname[200];
     int iEP;

     INTupleSvc* ntupleSvc;
     Gaudi::svcLocator() -> service("NTupleSvc", ntupleSvc);
     for(iEP=0; iEP<=NEP; iEP++){
	  if(iEP < NEP) sprintf(hname, "FILE137/align%02d", iEP);
	  else sprintf(hname, "FILE137/alignAll");

	  NTuplePtr nt(ntupleSvc, hname);
	  if( nt ) m_tuple[iEP] = nt;
	  else{
	       m_tuple[iEP] = ntupleSvc->book(hname, CLID_ColumnWiseTuple,"align");
	       if (m_tuple[iEP]) {
		    m_tuple[iEP]->addItem ("run", m_iRun[iEP]);
		    m_tuple[iEP]->addItem ("evt", m_iEvt[iEP]);
		    m_tuple[iEP]->addItem ("resi", m_resi[iEP]);
		    m_tuple[iEP]->addItem ("p", m_p[iEP]);
		    m_tuple[iEP]->addItem ("pt", m_pt[iEP]);
		    m_tuple[iEP]->addItem ("phi", m_phi[iEP]);
		    m_tuple[iEP]->addItem ("lay", m_lay[iEP]);
		    m_tuple[iEP]->addItem ("lr", m_lr[iEP]);
		    m_tuple[iEP]->addItem ("cel", m_cel[iEP]);
	       }
	       else {
		    log << MSG::FATAL << "Cannot book N-tuple:"
			<< long(m_tuple[iEP]) << endmsg;
	       }
	  }
     }

     m_hnTrk = new TH1F("HNtrack", "", 10, -0.5, 9.5);
     m_hlist->Add(m_hnTrk);

     m_hnHit = new TH1F("HNhit", "", 100, -0.5, 99.5);
     m_hlist->Add(m_hnHit);

     m_hlayHitmap = new TH1F("Hitmap", "", 43, -0.5, 42.5);
     m_hlist->Add(m_hlayHitmap);

     m_hresAll = new TH1F("HResAllInc", "", 200, -1.0, 1.0);
     m_hlist->Add(m_hresAll);

     m_hresInn = new TH1F("HResInnInc", "", 200, -1.0, 1.0);
     m_hlist->Add(m_hresInn);

     m_hresStp = new TH1F("HResStpInc", "", 200, -1.0, 1.0);
     m_hlist->Add(m_hresStp);

     m_hresOut = new TH1F("HResOutInc", "", 200, -1.0, 1.0);
     m_hlist->Add(m_hresOut);

     int lay;
     for(lay=0; lay<LAYERNMAX; lay++){
	  sprintf(hname, "Res_Layer%02d", lay);
	  m_hresLay[lay] = new TH1F(hname, "", 200, -1.0, 1.0);
	  m_hlist->Add(m_hresLay[lay]);
     }

     for(iEP=0; iEP<NEP; iEP++){
	  m_gr[iEP] = new TGraph();
	  sprintf(hname, "grResi%02d", iEP);
	  m_gr[iEP]->SetName(hname);
	  m_hlist->Add(m_gr[iEP]);
     }
     m_fevt.open("evt.txt");
}

bool ResiAlign::fillHist(MdcAliEvent* event){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "ResiAlign");
     log << MSG::DEBUG << "ResiAlign::fillHist()" << endreq;

     bool esCutFg = event->getEsCutFlag();
     if( ! esCutFg ){
	  m_ncut1++;
	  return true;
     }

     int i = 0;
     int k;

     int trkStat;
     double dr;
     double phi0;
     double kappa;
     double dz;
     double tanl;
     double chisq;
     double p;
     double pt;

     int nhits;
     int lay;
     int cel;
     int wir;
     int lr;
     int iEnd;
     int iEP;

     double doca;
     double resi;
     double zhit;
     double wphi;
     double dphi;
     double hitPhi;
     double xx;
     double yy;
     double rr;
     int stat;
     MdcAliRecTrk* rectrk;
     MdcAliRecHit* rechit;
     int nhitlay;
     bool fgHitLay[LAYERNMAX];

     IDataProviderSvc* eventSvc = NULL;
     Gaudi::svcLocator()->service("EventDataSvc", eventSvc);
     SmartDataPtr<Event::EventHeader> eventHeader(eventSvc,"/Event/EventHeader");
     if (!eventHeader) {
	  log << MSG::FATAL << "Could not find Event Header" << endreq;
	  m_ncut3++;
	  return( StatusCode::FAILURE);
     }
     int iEvt = eventHeader->eventNumber();
     int iRun = eventHeader->runNumber();

     int nTrk = event -> getNTrk();
     m_hnTrk->Fill(nTrk);
     m_fevt << setw(10) << iRun << setw(10) << iEvt << setw(10) << nTrk << endl;
     if((nTrk < m_param.nTrkCut[0]) || (nTrk > m_param.nTrkCut[1])){
	  m_ncut2++;
	  return true;
     }

     for(i=0; i<nTrk; i++){
	  rectrk = event->getRecTrk(i);
	  nhits = rectrk->getNHits();
	  trkStat = rectrk->getStat();
// 	  if (0 != trkStat) continue;

	  // dr cut
	  dr = rectrk -> getDr();
	  if(fabs(dr) > m_param.drCut){ m_ncut4++; continue; }

	  phi0 = rectrk -> getPhi0();
	  kappa = rectrk -> getKappa();

	  // dz cut
	  dz = rectrk -> getDz();
	  if(fabs(dz) > m_param.dzCut){ m_ncut5++; continue; }

	  for(lay=0; lay<LAYERNMAX; lay++){
	       fgHitLay[lay] = false;
	  }

	  m_hnHit->Fill(nhits);
	  for(k=0; k<nhits; k++){
	       rechit = rectrk -> getRecHit(k);
	       lay = rechit -> getLayid();
	       fgHitLay[lay] = true;
	  }

	  nhitlay = 0;
	  for(lay=0; lay<LAYERNMAX; lay++){
	       if(fgHitLay[lay]) nhitlay++;
	  }
	  if(nhitlay < m_param.nHitLayCut){ m_ncut6++; continue; }

	  tanl = rectrk -> getTanLamda();
	  chisq = rectrk -> getChisq();
	  p = rectrk -> getP();
	  pt = rectrk -> getPt();

	  if((fabs(pt)<m_param.ptCut[0]) || (fabs(pt)>m_param.ptCut[1])){ m_ncut7++; continue;}

	  for(k=0; k<nhits; k++){
	       rechit = rectrk->getRecHit(k);
	       lay = rechit->getLayid();
	       cel = rechit->getCellid();
	       lr = rechit->getLR();
	       doca = rechit -> getDocaInc();
	       zhit = rechit->getZhit();

	       stat = rechit -> getStat();
	       if((1 == m_param.hitStatCut) && (1 != stat)){ m_ncut8++; continue; }

	       if (1 == m_param.resiType) {
		    resi = rechit->getResiExcLR();
	       } else {
		    resi = rechit->getResiIncLR();
	       }
	       resi *= -1.0;
	       if( (1==isnan(resi)) || (fabs(resi) > m_resiCut) ||
		   (fabs(doca) > m_docaMax[lay]) || (fabs(doca) < m_docaMin[lay]) ){
		    m_ncut9++;
		    continue;
	       }

	       if(m_param.fgAdjacLayerCut){
		    if(0 == lay){
			 if( ! fgHitLay[1] ){ m_ncut10++; continue; }
		    } else if(42 == lay){
			 if( ! fgHitLay[41] ){ m_ncut11++; continue; }
		    } else{
			 if( (!fgHitLay[lay-1]) && (!fgHitLay[lay+1]) ){ m_ncut12++; continue; }
			 // for boundary layers
			 if( m_param.fgBoundLayerCut && m_layBound[lay] && 
			     ((!fgHitLay[lay-1]) || (!fgHitLay[lay+1])) ){
			      m_ncut13++;
			      continue;
			 }
		    }
	       }

	       m_hlayHitmap->Fill(lay);

	       // fill alignment trees
	       if((zhit < m_zrange[lay][0]) || (zhit > m_zrange[lay][1])){
		    wir = m_mdcGeomSvc -> Wire(lay, cel) -> Id();
		    xx = (zhit - m_zw[wir]) * (m_xe[wir] - m_xw[wir]) / 
			 (m_ze[wir] - m_zw[wir]) + m_xw[wir];
		    yy = (zhit - m_zw[wir]) * (m_ye[wir] - m_yw[wir]) / 
			 (m_ze[wir] - m_zw[wir]) + m_yw[wir];
		    rr = sqrt( (xx * xx) + (yy * yy) );
		    dphi = fabs(doca) / m_radii[lay];

		    if( yy >= 0 )  wphi = acos(xx / rr);
		    else           wphi = PI2 - acos(xx / rr);
		    if(1 == lr)    hitPhi = wphi + dphi; // mention
		    else hitPhi = wphi - dphi;
		    if(hitPhi < 0) hitPhi += PI2;
		    else if(hitPhi > PI2) hitPhi -= PI2;

		    if(zhit < m_zrange[lay][0]) iEnd = 0; // west
		    else iEnd = 1; // east
		    iEP = Alignment::getEpId(lay, iEnd);

		    m_iRun[iEP] = iRun;
		    m_iEvt[iEP] = iEvt;
		    m_resi[iEP] = resi;
		    m_p[iEP] = p;
		    m_pt[iEP] = pt;
		    m_phi[iEP] = hitPhi;
		    m_lay[iEP] = lay;
		    m_lr[iEP] = lr;
		    m_cel[iEP] = cel;
		    m_tuple[iEP]->write();

		    m_resi[NEP] = resi;
		    m_p[NEP] = p;
		    m_pt[NEP] = pt;
		    m_phi[NEP] = hitPhi;
		    m_lay[NEP] = lay;
		    m_lr[NEP] = lr;
		    m_cel[NEP] = cel;
		    m_tuple[NEP]->write();

		    m_hresAll->Fill(resi);
		    if(lay < 8) m_hresInn->Fill(resi);
		    else if(lay < 20) m_hresStp->Fill(resi);
		    else m_hresOut->Fill(resi);
		    m_hresLay[lay]->Fill(resi);

		    m_gr[iEP]->SetPoint(m_npoint[iEP], hitPhi, resi);
		    m_npoint[iEP]++;
	       }
	  }
     }

     return true;
}

void ResiAlign::updateConst(MdcAlignPar* alignPar){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "ResiAlign");
     log << MSG::INFO << "ResiAlign::updateConst()" << endreq;
     m_fevt.close();

     int iEP;
     double par[3];
     double err[3];
     double dx;
     double dy;
     double rz;
     double rLayer[] = { 120.225, 205.0, 237.55, 270.175,
			 302.625, 334.775, 366.65, 500.0,
			 120.225, 205.0, 237.55, 270.175,
			 302.625, 334.775, 366.65, 500.0 };

     TCanvas c1("c1", "c1", 10, 10, 700, 500);

     TF1* fResPhi = new TF1("fResPhi", funResi, 0, PI2, 3);
     fResPhi->SetParameter(0, 0.0);
     fResPhi->SetParameter(1, 0.0);
     fResPhi->SetParameter(2, 0.0);

     for(iEP=0; iEP<NEP; iEP++){
	  if((m_gr[iEP]->GetN()) > 500){
	       m_gr[iEP]->Fit("fResPhi", "V");
	       par[0] = fResPhi->GetParameter(0);
	       par[1] = fResPhi->GetParameter(1);
	       par[2] = fResPhi->GetParameter(2);

	       err[0] = fResPhi->GetParError(0);
	       err[1] = fResPhi->GetParError(1);
	       err[2] = fResPhi->GetParError(2);

	       dx = -1.0 * par[1];
	       dy = par[2];
	       rz = par[0] / rLayer[iEP];

	       // assume the shift of the outer section is 0
	       if (7==iEP || 15==iEP) {
		    dx = 0.0;
		    dy = 0.0;
		    rz = 0.0;
		    par[0] = 0.0;
		    par[1] = 0.0;
		    par[2] = 0.0;
	       }
	       alignPar->setDelDx(iEP, dx);
	       alignPar->setDelDy(iEP, dy);
	       alignPar->setDelRz(iEP, rz);

	       alignPar->setErrDx(iEP, err[1]);
	       alignPar->setErrDy(iEP, err[2]);
	       alignPar->setErrRz(iEP, err[0]/rLayer[iEP]);
	  }
     }

     cout << "TrackCut: cut1: " << m_ncut1 << ",  cut2: " << m_ncut2 << ",  cut3: " << m_ncut3
	  << ",  cut4: " << m_ncut4 << ",  cut5: " << m_ncut5 << ",  cut6: " << m_ncut6
	  << ",  cut7: " << m_ncut7 << endl;
     cout << "HitCut:   cut8: " << m_ncut8 << ",  cut9: " << m_ncut9 << ",  cut10: " << m_ncut10
	  << ",  cut11: " << m_ncut11 << ",  cut12: " << m_ncut12 << ",  cut13: " << m_ncut13 << endl;

     delete fResPhi;
}

Double_t ResiAlign::funResi(Double_t* x, Double_t* par){
     Double_t val;
     val = par[0] + par[1]*sin(x[0]) + par[2]*cos(x[0]);
     return val;
}

