#include "MdcAlignAlg/MilleAlign.h"
#include "MdcAlignAlg/MdcAlignAlg.h"
#include "MdcAlignAlg/Alignment.h"

#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "Identifier/MdcID.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string> 
#include <cstring>
#include <cmath>

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatblty wll be enabled ONLY n CLHEP 1.9
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
 
#include "TSpline.h"

using namespace std;

MilleAlign::MilleAlign(){
     for(int lay=0; lay<LAYERNMAX; lay++){
	  m_resiCut[lay] = 1.5;
	  if(lay<8){
	       m_docaCut[lay][0] = 0.5;
	       m_docaCut[lay][1] = 5.5;
	  } else{
	       m_docaCut[lay][0] = 0.5;
	       m_docaCut[lay][1] = 7.5;
	  }
     }
}

MilleAlign::~MilleAlign(){
}

void MilleAlign::clear(){
     delete m_hresAll;
     delete m_hresInn;
     delete m_hresStp;
     delete m_hresOut;
     for(int lay=0; lay<LAYERNMAX; lay++) delete m_hresLay[lay];
     delete m_hresAllRec;
     for(int lay=0; lay<LAYERNMAX; lay++) delete m_hresLayRec[lay];
     delete m_hddoca;
     delete m_pMilleAlign;
}

void MilleAlign::initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
			    IMdcCalibFunSvc* mdcFunSvc){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MilleAlign");
     log << MSG::INFO << "MilleAlign::initialize()" << endreq;

     // Initialze MdcUtilitySvc
     IMdcUtilitySvc* imdcUtilitySvc;
     StatusCode sc = Gaudi::svcLocator() -> service ("MdcUtilitySvc",imdcUtilitySvc);
     m_mdcUtilitySvc= dynamic_cast<MdcUtilitySvc*> (imdcUtilitySvc);
     if ( sc.isFailure() ){
	  log << MSG::FATAL << "Could not load MdcUtilitySvc!" << endreq;
     }

     m_hlist = hlist;
     m_mdcGeomSvc = mdcGeomSvc;
     m_mdcFunSvc = mdcFunSvc;

     // initialize hitograms
     m_hresAll = new TH1F("HResAllInc", "", 200, -1.0, 1.0);
     m_hlist->Add(m_hresAll);

     m_hresInn = new TH1F("HResInnInc", "", 200, -1.0, 1.0);
     m_hlist->Add(m_hresInn);

     m_hresStp = new TH1F("HResStpInc", "", 200, -1.0, 1.0);
     m_hlist->Add(m_hresStp);

     m_hresOut = new TH1F("HResOutInc", "", 200, -1.0, 1.0);
     m_hlist->Add(m_hresOut);

     char hname[200];
     for(int lay=0; lay<LAYERNMAX; lay++){
	  sprintf(hname, "Res_Layer%02d", lay);
	  m_hresLay[lay] = new TH1F(hname, "", 200, -1.0, 1.0);
	  m_hlist->Add(m_hresLay[lay]);
     }

     m_hresAllRec = new TH1F("HResAllRecInc", "", 200, -1.0, 1.0);
     m_hlist->Add(m_hresAllRec);
     for(int lay=0; lay<LAYERNMAX; lay++){
	  sprintf(hname, "Res_LayerRec%02d", lay);
	  m_hresLayRec[lay] = new TH1F(hname, "", 200, -1.0, 1.0);
	  m_hlist->Add(m_hresLayRec[lay]);
     }

     // for debug
     m_hddoca = new TH1F("delt_doca", "", 200, -1.0, 1.0);
     m_hlist->Add(m_hddoca);

     for(int lay=0; lay<LAYERNMAX; lay++){
	  sprintf(hname, "delt_docaLay%02d", lay);
	  m_hddocaLay[lay] = new TH1F(hname, "", 200, -1.0, 1.0);
	  m_hlist->Add(m_hddocaLay[lay]);
     }

     // initialize millepede
     m_nglo = NEP;
     m_nloc = NTRKPAR;
     m_nGloHit = 2 * NDOFALIGN;
     m_npar = NDOFALIGN * m_nglo;

     int i;
     for(i=0; i<NDOFALIGN; i++){
	  m_dofs[i] = g_dofs[i];
	  m_sigm[i] = g_Sigm[i];
     }

     m_pMilleAlign = new Millepede();
     m_pMilleAlign -> InitMille(&m_dofs[0], &m_sigm[0], m_nglo, m_nloc,
				g_start_chi_cut, 3, g_res_cut, g_res_cut_init);

     m_derGB.resize(m_npar);
     m_derNonLin.resize(m_npar);
     m_par.resize(m_npar);
     m_error.resize(m_npar);
     m_pull.resize(m_npar);

     m_derLC.resize(m_nloc);

     // contraints
     std::vector<double> constTX;
     std::vector<double> constTY;
     std::vector<double> constRZ;

     std::vector<double> constTXE;
     std::vector<double> constTXW;
     std::vector<double> constTYE;
     std::vector<double> constTYW;
     std::vector<double> constRZE;
     std::vector<double> constRZW;

     constTX.resize(m_npar);
     constTY.resize(m_npar);
     constRZ.resize(m_npar);

     constTXE.resize(m_npar);
     constTXW.resize(m_npar);
     constTYE.resize(m_npar);
     constTYW.resize(m_npar);
     constRZE.resize(m_npar);
     constRZW.resize(m_npar);

     for(i=0; i<m_npar; i++){
	  constTX[i] = 0.0;
	  constTY[i] = 0.0;
	  constRZ[i] = 0.0;

	  constTXE[i] = 0.0;
	  constTXW[i] = 0.0;
	  constTYE[i] = 0.0;
	  constTYW[i] = 0.0;
	  constRZE[i] = 0.0;
	  constRZW[i] = 0.0;
     }
     constTX[7] = 1.0;
     constTX[15] = 1.0;
     constTY[23] = 1.0;
     constTY[31] = 1.0;
     constRZ[39] = 1.0;
     constRZ[47] = 1.0;

     constTXE[7] = 1.0;
     constTXW[15] = 1.0;
     constTYE[23] = 1.0;
     constTYW[31] = 1.0;
     constRZE[39] = 1.0;
     constRZW[47] = 1.0;

     //m_pMilleAlign -> ConstF(&constTX[0], 0.0);
     //m_pMilleAlign -> ConstF(&constTY[0], 0.0);
//      m_pMilleAlign -> ConstF(&constRZ[0], 0.0);

     m_pMilleAlign -> ConstF(&constTXE[0], 0.0);
     m_pMilleAlign -> ConstF(&constTXW[0], 0.0);
     m_pMilleAlign -> ConstF(&constTYE[0], 0.0);
     m_pMilleAlign -> ConstF(&constTYW[0], 0.0);
     m_pMilleAlign -> ConstF(&constRZE[0], 0.0);
     m_pMilleAlign -> ConstF(&constRZW[0], 0.0);
}

bool MilleAlign::fillHist(MdcAliEvent* event){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MilleAlign");
     log << MSG::DEBUG << "MilleAlign::fillTree()" << endreq;

     int recFlag;
     int itrk;
     int ihit;
     int fgGetDoca;
     int lr;
     int lay;
     int cel;
     double doca;
     double dmeas;
     double zhit;
     double resi;
     double resiRec;
     double deri;
     double hitSigm;
     double hitpos[3];
     double wpos[7];		// wpos[6] is wire tension
     const MdcGeoWire* pWire;

     double trkpar[NTRKPAR];
     double trkparms[NTRKPARALL];	// track parameters and errors

     // numerical derivative
     int ipar;
     int iparGB;

     MdcAliRecTrk* rectrk;
     MdcAliRecHit* rechit;

     int ntrk = event -> getNTrk();
     if( (ntrk<m_param.nTrkCut[0]) || (ntrk>m_param.nTrkCut[1])) return false;

     for(itrk=0; itrk<ntrk; itrk++){
	  rectrk = event->getRecTrk(itrk);
	  recFlag = rectrk->getStat();

	  trkpar[0] = rectrk -> getDr();
	  trkpar[1] = rectrk -> getPhi0();
	  trkpar[2] = rectrk -> getKappa();
	  trkpar[3] = rectrk -> getDz();
	  trkpar[4] = rectrk -> getTanLamda();

	  int nHit = rectrk -> getNHits();
	  if(nHit < m_param.nHitCut) continue;
	  if(fabs(trkpar[0]) > m_param.drCut) continue;
	  if(fabs(trkpar[3]) > m_param.dzCut) continue;

	  HepVector rechelix = rectrk->getHelix();
	  HepVector helix = rectrk->getHelix();
	  HepSymMatrix helixErr = rectrk->getHelixErr();

	  int nHitUse = 0;
	  for(ihit=0; ihit<nHit; ihit++){
	       rechit = rectrk -> getRecHit(ihit);
	       lr = rechit->getLR();
	       lay = rechit -> getLayid();
	       cel = rechit -> getCellid();
	       pWire = m_mdcGeomSvc -> Wire(lay, cel); 
	       dmeas = rechit -> getDmeas();
	       zhit = rechit -> getZhit();
	       hitSigm = rechit -> getErrDmeas();

	       wpos[0] = pWire -> Backward().x(); // east end
	       wpos[1] = pWire -> Backward().y();
	       wpos[2] = pWire -> Backward().z();
	       wpos[3] = pWire -> Forward().x(); // west end
	       wpos[4] = pWire -> Forward().y();
	       wpos[5] = pWire -> Forward().z();
	       wpos[6] = pWire -> Tension();

	       double docaRec = rechit->getDocaInc();
	       double doca = (m_mdcUtilitySvc->doca(lay, cel, helix, helixErr))*10.0;

	       resi = -1.0*dmeas - doca;
	       if ((fabs(doca) < m_docaCut[lay][0]) || (fabs(doca) > m_docaCut[lay][1]) || 
		   (fabs(resi) > m_resiCut[lay])) continue;
	       nHitUse++;

	       resiRec = rechit -> getResiIncLR();

	       double dd = fabs(doca) - fabs(rechit->getDocaInc());
	       m_hddoca -> Fill(dd);
	       m_hddocaLay[lay] -> Fill(dd);

	       // fill histograms
	       m_hresAll->Fill(resi);
	       if(lay < 8) m_hresInn->Fill(resi);
	       else if(lay < 20) m_hresStp->Fill(resi);
	       else m_hresOut->Fill(resi);
	       m_hresLay[lay]->Fill(resi);

	       m_hresAllRec->Fill(resiRec);
	       m_hresLayRec[lay]->Fill(resiRec);

	       // reset the derivatives arrays
	       m_pMilleAlign -> ZerLoc(&m_derGB[0], &m_derLC[0], &m_derNonLin[0]);

	       // derivatives of local parameters
	       for(ipar=0; ipar<m_nloc; ipar++){
		    if( ! getDeriLoc(ipar, lay, cel ,rechelix, helixErr, deri) ){
			 cout << "getDeriLoc == false!" << setw(12) << itrk << setw(12) << ipar << endl; 
			 return false;
		    }
		    m_derLC[ipar] = deri;
	       }

	       // derivatives of global parameters
	       // ipar 0 - 5: dx_east, dx_west, dy_east, dy_west, rz_east, rz_west
	       for(ipar=0; ipar<m_nGloHit; ipar++){
		    iparGB = getAlignParId(lay, ipar);
		    if( ! getDeriGlo(ipar, iparGB, lay, cel, helix, helixErr, wpos, deri) )
		    {
			 cout << "getDeriGlo == false!" << setw(12) << itrk << setw(12) << ipar << endl; 
			 return false;
		    }
		    m_derGB[iparGB] = deri;
	       }
	       m_pMilleAlign -> EquLoc(&m_derGB[0], &m_derLC[0], &m_derNonLin[0], resi, hitSigm);
	  } // loop of nhit

	  // local fit in Millepede
	  bool sc = m_pMilleAlign -> FitLoc(m_pMilleAlign->GetTrackNumber(), trkparms, 0);
	  if(sc) m_pMilleAlign -> SetTrackNumber( m_pMilleAlign->GetTrackNumber()+1 );
     } // track loop 
     return true;
}


void MilleAlign::updateConst(MdcAlignPar* alignPar){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MilleAlign");
     log << MSG::INFO << "MilleAlign::updateConst()" << endreq;

     m_pMilleAlign -> MakeGlobalFit(&m_par[0], &m_error[0], &m_pull[0]);

     int iEP;
     int ipar;
     double val;
     double err;
     for(int i=0; i<NDOFALIGN; i++){
	  for(iEP=0; iEP<NEP; iEP++){
	       ipar = i * NEP + iEP;
	       if(m_dofs[i]){
		    val = m_par[ipar];
		    err = m_error[ipar];
	       } else{
		    val = 0.0;
		    err = 0.0;
	       }

	       if(0 == i){
		    alignPar->setDelDx(iEP, val);
		    alignPar->setErrDx(iEP, err);
	       } else if(1 == i){
		    alignPar->setDelDy(iEP, val);
		    alignPar->setErrDy(iEP, err);
	       } else if(2 == i){
		    alignPar->setDelRz(iEP, val/1000.0); // mrad -> rad
		    alignPar->setErrRz(iEP, err/1000.0); // mrad -> rad
	       }
	  }
     }
}

int MilleAlign::getAlignParId(int lay, int iparHit){
     int ip;
     if(lay < 8) ip = 0;
     else if(lay < 10) ip = 1;
     else if(lay < 12) ip = 2;
     else if(lay < 14) ip = 3;
     else if(lay < 16) ip = 4;
     else if(lay < 18) ip = 5;
     else if(lay < 20) ip = 6;
     else ip = 7;

     // iparHit 0 - 5: dx_east, dx_west, dy_east, dy_west, rz_east, rz_west
     int ipar = iparHit * 8 + ip;
     return ipar;
}

bool MilleAlign::getDeriLoc(int ipar, int lay, int cel, HepVector rechelix, HepSymMatrix &helixErr, double &deri){
     int i;
     double doca;
     HepVector sampar(NTRKPAR, 0);
     double xxLC[gNsamLC];
     double yyLC[gNsamLC];

     for(i=0; i<m_nloc; i++) sampar[i] = rechelix[i];
     double startpar = rechelix[ipar] - 0.5*gStepLC[ipar]*(double)gNsamLC;

     for(i=0; i<gNsamLC; i++){
	  sampar[ipar] = startpar + (double)i * gStepLC[ipar];
	  xxLC[i] = sampar[ipar];
	  if(0==ipar || 3==ipar) xxLC[i] *= 10.;	// cm -> mm

	  HepVector helix = sampar;
	  bool passCellRequired = false;
	  doca = (m_mdcUtilitySvc->doca(lay, cel, helix, helixErr,passCellRequired))*10.0;

	  if(NULL == doca){
// 	     cout << "in getDeriLoc, doca = " << doca << endl;
	       return false;
	  }
	  yyLC[i] = doca;
     }

     if (0==ipar || 3==ipar) rechelix[ipar] *= 10.; // cm -> mm
     TSpline3* pSpline3 = new TSpline3("deri", xxLC, yyLC, gNsamLC);
     deri = pSpline3->Derivative(rechelix[ipar]);
     delete pSpline3;
     return true;
}

bool MilleAlign::getDeriGlo(int iparHit, int iparGB, int lay, int cel, HepVector helix, 
			    HepSymMatrix &helixErr, double wpos[], double &deri){
     int i;
     double doca;
     double xxGB[gNsamGB];
     double yyGB[gNsamGB];
     double dAlignPar;
     double dAlignParini = 0.0;
     double startpar = dAlignParini - 0.5*gStepGB[iparGB]*(double)gNsamGB;
     double wposSam[7];
     for(i=0; i<7; i++) wposSam[i] = wpos[i]; // 0-2:east; 3-5:west

     for(i=0; i<gNsamGB; i++){
	  dAlignPar = startpar + (double)i * gStepGB[iparGB];
	  xxGB[i] = dAlignPar;
	  if(0 == iparHit){	// dx_east
	       wposSam[0] = wpos[0] + dAlignPar;
	  } else if(1 == iparHit){	// dx_west
	       wposSam[3] = wpos[3] + dAlignPar;
	  } else if(2 == iparHit){	// dy_east
	       wposSam[1] = wpos[1] + dAlignPar;
	  } else if(3 == iparHit){	// dy_west
	       wposSam[4] = wpos[4] + dAlignPar;
	  } else if(4 == iparHit){	// rz_east
	       wposSam[0] = wpos[0] - (wpos[1] * dAlignPar * 0.001);
	       wposSam[1] = wpos[1] + (wpos[0] * dAlignPar * 0.001);
	  } else if(5 == iparHit){	// rz_west
	       wposSam[3] = wpos[3] - (wpos[4] * dAlignPar * 0.001);
	       wposSam[4] = wpos[4] + (wpos[3] * dAlignPar * 0.001);
	  }

	  HepPoint3D eastP(wposSam[0]/10., wposSam[1]/10., wposSam[2]/10.);
	  HepPoint3D westP(wposSam[3]/10., wposSam[4]/10., wposSam[5]/10.);
	  doca = (m_mdcUtilitySvc->doca(lay, cel, eastP, westP, helix, helixErr))*10.0; // cm->mm

	  if(NULL == doca) return false; 
 
	  yyGB[i] = doca;
     }

     TSpline3* pSpline3 = new TSpline3("deri", xxGB, yyGB, gNsamGB);
     deri = pSpline3 -> Derivative(dAlignParini);
     delete pSpline3;
     return true;
}

