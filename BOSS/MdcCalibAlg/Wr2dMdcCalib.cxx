#include "MdcCalibAlg/Wr2dMdcCalib.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "TF1.h"
#include "TMinuit.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>

using namespace std;

bool Wr2dMdcCalib::fgBIN[MdcCalWrNBin];
double Wr2dMdcCalib::xBIN[MdcCalWrNBin];
double Wr2dMdcCalib::yBIN[MdcCalWrNBin];
double Wr2dMdcCalib::zBIN[MdcCalWrNBin];
double Wr2dMdcCalib::zBINERR[MdcCalWrNBin];
double Wr2dMdcCalib::zMIN;
double Wr2dMdcCalib::zMAX;

Wr2dMdcCalib::Wr2dMdcCalib(){
}

Wr2dMdcCalib::~Wr2dMdcCalib(){
}

void Wr2dMdcCalib::clear(){
     int bin;
     for(int i=0; i<MdcCalTotCell; i++){
	  for(bin=0; bin<MdcCalWrNBin; bin++){
	       delete m_hl[i][bin];
	       delete m_hr[i][bin];
	  }
     }
     delete m_fdWire;

     MdcCalib::clear();
}

void Wr2dMdcCalib::initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
			      IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc) {
     IMessageSvc *msgSvc;
     Gaudi::svcLocator()->service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "Wr2dMdcCalib");
     log << MSG::INFO << "Wr2dMdcCalib::initialize()" << endreq;

     m_hlist = hlist;
     m_mdcGeomSvc = mdcGeomSvc;
     m_mdcFunSvc = mdcFunSvc;
     m_mdcUtilitySvc = mdcUtilitySvc;

     MdcCalib::initialize(m_hlist, m_mdcGeomSvc, m_mdcFunSvc, m_mdcUtilitySvc);

     int i;
     int bin;
     int lay;
     int cel;
     char hname[200];

     m_fdWire = new TFolder("WireCor", "WireCor");
     m_hlist->Add(m_fdWire);

     for(i=0; i<MdcCalTotCell; i++){
	  lay = m_mdcGeomSvc -> Wire(i) -> Layer();
	  cel = m_mdcGeomSvc -> Wire(i) -> Cell();

	  for(bin=0; bin<MdcCalWrNBin; bin++){
	       sprintf(hname, "h%04d_L_%02d", i, bin);
	       m_hl[i][bin] = new TH1F(hname, "", 300, -1.5, 1.5);
	       m_fdWire->Add(m_hl[i][bin]);

	       sprintf(hname, "h%04d_R_%02d", i, bin);
	       m_hr[i][bin] = new TH1F(hname, "", 300, -1.5, 1.5);
	       m_fdWire->Add(m_hr[i][bin]);
	  }
     }

     for(lay=0; lay<MdcCalNLayer; lay++){
	  m_zwest[lay] = m_mdcGeomSvc->Wire(lay, 0)->Forward().z();
	  m_zeast[lay] = m_mdcGeomSvc->Wire(lay, 0)->Backward().z();
	  m_zwid[lay] = (m_zeast[lay] - m_zwest[lay]) / (double)MdcCalWrNBin;

	  for(bin=0; bin<MdcCalWrNBin; bin++){
	       m_zbinCen[lay][bin] = ((double)bin + 0.5) * m_zwid[lay] + m_zwest[lay];
	  }
     }
}

int Wr2dMdcCalib::fillHist(MdcCalEvent* event){
     IMessageSvc *msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "Wr2dMdcCalib");
     log << MSG::DEBUG << "Wr2dMdcCalib::fillHist()" << endreq;

     MdcCalib::fillHist(event);

     // get EsTimeCol
     bool esCutFg = event->getEsCutFlag();
     if( ! esCutFg ) return -1;

     int i;
     int k;
     int ntrk;
     int nhit;

     int bin;
     int lay;
     int cel;
     int wir;
     int lr;
     double dmeas;
     double doca;
     double residual;
     double zhit;

     MdcCalRecTrk* rectrk;
     MdcCalRecHit* rechit;

     ntrk = event->getNTrk();
     log << MSG::DEBUG << "number of tracks: " << ntrk << endreq;

     for(i=0; i<ntrk; i++){
	  rectrk = event -> getRecTrk(i);
	  nhit = rectrk -> getNHits();

	  log << MSG::DEBUG << "number of hits: " << nhit << endreq;
	  for(k=0; k<nhit; k++){
	       rechit = rectrk -> getRecHit(k);
	       lay = rechit -> getLayid();
	       cel = rechit -> getCellid();
	       wir = m_mdcGeomSvc ->	Wire(lay, cel) -> Id();
	       lr = rechit -> getLR();
	       doca = rechit -> getDocaInc();
	       dmeas = rechit -> getDmeas();
	       residual = rechit -> getResiInc();
	       zhit = rechit -> getZhit();

	       if((wir<MdcCalTotCell) && (fabs(zhit) < m_zeast[lay])){
		    bin = (int)(zhit / m_zwid[lay]);
		    if( 0 == lr ){
			 m_hl[wir][bin] -> Fill(residual);
		    }else if( 1 == lr ){
			 m_hr[wir][bin] -> Fill(residual);
		    }
	       }else{
		    std::cout << "wir: " << wir << std::endl;
	       }
	  }
     }
     return 1;
}

int Wr2dMdcCalib::updateConst(MdcCalibConst* calconst){
     IMessageSvc *msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "Wr2dMdcCalib");
     log << MSG::INFO << "Wr2dMdcCalib::updateConst()" << endreq;

     int i;
     int k;
     int bin;
     int lay;
     int cel;
     double dw;

     // minuit for wires
     Int_t ierflg;
     Int_t istat;
     Int_t nvpar;
     Int_t nparx;
     Double_t fmin;
     Double_t edm;
     Double_t errdef;
     Double_t arglist[10];

     TMinuit *gmtrw = new TMinuit(5);
     gmtrw -> SetPrintLevel(-1);
     gmtrw -> SetFCN(fcnWireParab);
     gmtrw -> SetErrorDef(1.0);
     gmtrw -> mnparm(0, "xf", 0.0, 0.01, 0, 0, ierflg);
     gmtrw -> mnparm(1, "yf", 0.0, 0.01, 0, 0, ierflg);
     gmtrw -> mnparm(2, "xb", 0.0, 0.01,  0, 0, ierflg);
     gmtrw -> mnparm(3, "yb", 0.0, 0.01,  0, 0, ierflg);
     gmtrw -> mnparm(4, "ten", 0.0, 0.1,  0, 0, ierflg);
     arglist[0] = 0;
     gmtrw -> mnexcm("Set NOW", arglist, 0, ierflg);

     double a;
     double dx;
     double dy;
     double dz;
     double length;
     double ten[] = {15, 15, 15, 16, 16, 17, 17, 18, 14, 14,
		     19, 19, 24, 24, 31, 31, 37, 37, 45, 45,
		     46, 47, 47, 47, 47, 48, 48, 48, 48, 49,
		     49, 49, 49, 50, 50, 50, 51, 51, 51, 52,
		     52, 52, 52};
     double wpar[5];
     double wparErr[5];
     double sinphiw;
     double cosphiw;
     double deldw;
     double delxw;
     double delyw;

     int nFit;
     Stat_t entryL;
     Stat_t entryR;
     Double_t hcen;
     Double_t cenL[MdcCalWrNBin];
     Double_t errL[MdcCalWrNBin];
     Double_t cenR[MdcCalWrNBin];
     Double_t errR[MdcCalWrNBin];

     ofstream fwlog("logWireCor.txt");
     ofstream fwpc("wireposCor.txt");
     ofstream fwpcErr("wireposErr.txt");

     fwpc << setw(5) << "wireId" << setw(15) << "dx_East(mm)"
	  << setw(15) << "dy_East(mm)" << setw(15) << "dz_East(mm)"
	  << setw(15) << "dx_West(mm)" << setw(15) << "dy_West(mm)"
	  << setw(15) << "dz_West(mm)" << endl;

     for(i=0; i<MdcCalTotCell; i++){
	  for(k=0; k<5; k++) wparErr[k] = 999.0;
	  nFit = 0;
	  for(bin=0; bin<MdcCalWrNBin; bin++){
	       entryL = m_hl[i][bin] -> GetEntries();
	       entryR = m_hr[i][bin] -> GetEntries();
	       if((entryL < 100) && (entryR < 100)){
		    fgBIN[bin] = false;
		    continue;
	       } else{
		    fgBIN[bin] = true;
	       }
	       nFit++;

	       if(1 == m_param.fgCalib[lay]){
		    hcen = m_hl[i][bin] -> GetMean();
		    if(entryL > 500){
			 m_hl[i][bin] -> Fit("gaus", "Q", "", hcen-1.5, hcen+1.5);
			 cenL[bin] = m_hl[i][bin]->GetFunction("gaus")->GetParameter(1);
			 errL[bin] = m_hl[i][bin]->GetFunction("gaus")->GetParameter(2);
		    }
		    else{
			 cenL[bin] = hcen;
			 errL[bin] = m_hl[i][bin] -> GetRMS();
		    }

		    hcen = m_hr[i][bin] -> GetMean();
		    if(entryR > 500){
			 m_hr[i][bin] -> Fit("gaus", "Q", "", hcen-1.5, hcen+1.5);
			 cenR[bin] = m_hr[i][bin]->GetFunction("gaus")->GetParameter(1);
			 errR[bin] = m_hr[i][bin]->GetFunction("gaus")->GetParameter(2);
		    }
		    else{
			 cenR[bin] = hcen;
			 errR[bin] = m_hr[i][bin] -> GetRMS();
		    }
	       } else{
		    cenL[bin] = 0.0;
		    errL[bin] = 0.15;
		    cenR[bin] = 0.0;
		    errR[bin] = 0.15;
	       }
	  }
	  if(nFit < 8) continue;

	  lay = m_mdcGeomSvc -> Wire(i) -> Layer();
	  cel = m_mdcGeomSvc -> Wire(i) -> Cell();
	  zMIN = m_zwest[lay];
	  zMAX = m_zeast[lay];
	  wpar[0] = m_mdcGeomSvc->Wire(lay, 0)->Backward().x();
	  wpar[1] = m_mdcGeomSvc->Wire(lay, 0)->Backward().y();
	  wpar[2] = m_mdcGeomSvc->Wire(lay, 0)->Forward().x();
	  wpar[3] = m_mdcGeomSvc->Wire(lay, 0)->Forward().y();
	  wpar[4] = ten[lay];

	  a = 9.47e-06 / (2 * wpar[4]);
	  dx = wpar[0] - wpar[2];
	  dy = wpar[1] - wpar[3];
	  dz = zMAX - zMIN;
	  length = sqrt(dx*dx + dz*dz);

	  for(bin=0; bin<MdcCalWrNBin; bin++){
	       zBIN[bin] = m_zbinCen[lay][bin];
	       xBIN[bin] = (wpar[0]-wpar[2])*(zBIN[bin]-zMIN)/(zMAX-zMIN) + wpar[2];
	       yBIN[bin] = a*zBIN[bin]*zBIN[bin] + (wpar[1]-wpar[3])*zBIN[bin]/length
		    + 0.5*(wpar[1]+wpar[3]) - a*length*length/4.0;

	       sinphiw = yBIN[bin] / sqrt(xBIN[bin]*xBIN[bin] + yBIN[bin]*yBIN[bin]);
	       cosphiw = xBIN[bin] / sqrt(xBIN[bin]*xBIN[bin] + yBIN[bin]*yBIN[bin]);

	       deldw = - (cenL[bin]-cenR[bin])/2.0;
	       delxw = -deldw * sinphiw;
	       delyw = deldw * cosphiw;

	       fwlog << setw(3) << lay << setw(4) << cel << setw(5) << i
		     << setw(4) << bin << setw(15) << zBIN[bin]
		     << setw(15) << deldw << setw(15) << delxw
		     << setw(15) << delyw << endl;

	       xBIN[bin] += delxw;
	       yBIN[bin] += delyw;

	       zBINERR[bin] = sqrt((errL[bin]*errL[bin]) + (errR[bin]*errR[bin]))/2.0;
	  }

	  arglist[0] = 1;
	  arglist[1] = wpar[0];
	  gmtrw -> mnexcm("SET PARameter", arglist, 2, ierflg);
	  arglist[0] = 2;
	  arglist[1] = wpar[1];
	  gmtrw -> mnexcm("SET PARameter", arglist, 2, ierflg);
	  arglist[0] = 3;
	  arglist[1] = wpar[2];
	  gmtrw -> mnexcm("SET PARameter", arglist, 2, ierflg);
	  arglist[0] = 4;
	  arglist[1] = wpar[3];
	  gmtrw -> mnexcm("SET PARameter", arglist, 2, ierflg);
	  arglist[0] = 5;
	  arglist[1] = wpar[4];
	  gmtrw -> mnexcm("SET PARameter", arglist, 2, ierflg);
	  gmtrw -> mnexcm("FIX", arglist, 1, ierflg);

	  arglist[0] = 2000;
	  arglist[1] = 0.1;
	  gmtrw -> mnexcm("MIGRAD", arglist, 2, ierflg);
	  gmtrw -> mnstat(fmin, edm, errdef, nvpar, nparx, istat);

	  if( (0==ierflg) && (3==istat) ){
	       gmtrw -> GetParameter(0, wpar[0], wparErr[0]);
	       gmtrw -> GetParameter(1, wpar[1], wparErr[1]);
	       gmtrw -> GetParameter(2, wpar[2], wparErr[2]);
	       gmtrw -> GetParameter(3, wpar[3], wparErr[3]);
	       gmtrw -> GetParameter(4, wpar[4], wparErr[4]);
	  }
	  gmtrw -> mnexcm("RELease", arglist, 1, ierflg);

	  fwlog << setw(5) << i << setw(15) << wpar[0] << setw(15) << wpar[1]
		<< setw(15) << wpar[2] << setw(15) << wpar[3] << endl;
	  fwpc << setw(5) << i << setw(15) << wpar[0] << setw(15) << wpar[1]
	       << setw(15) << "0" << setw(15) << wpar[2] 
	       << setw(15) << wpar[3] << setw(15) << "0" << endl;
	  fwpcErr << setw(5) << i << setw(15) << wparErr[0] << setw(15) << wparErr[1]
		  << setw(15) << wparErr[2] << setw(15) << wparErr[3] << endl;
     }
     fwlog.close();
     fwpc.close();
     fwpcErr.close();

     delete gmtrw;
     return 1;
}

void Wr2dMdcCalib::fcnWireParab(Int_t &npar, Double_t *gin,
				Double_t &f, Double_t *par, Int_t iflag){
     Int_t bin;
     Double_t xfit;
     Double_t yfit;

     double a = 9.47e-06 / (2 * par[4]);
     double dx = par[0] - par[2];
     double dy = par[1] - par[3];
     double dz = zMAX - zMIN;
     double length = sqrt(dx*dx + dz*dz);

     Double_t chisq = 0.0;
     Double_t deta;

     for(bin=0; bin<MdcCalWrNBin; bin++){
	  if( fgBIN[bin] ){
	       xfit = (par[0] - par[2]) * (zBIN[bin] - zMIN) / (zMAX - zMIN) + par[2];
	       yfit = a*zBIN[bin]*zBIN[bin] + (par[1] - par[3])*zBIN[bin]/length
		    + 0.5*(par[1] + par[3]) - a*length*length/4.0;

	       deta = ( (xfit-xBIN[bin])*(xfit-xBIN[bin])+
			(yfit-yBIN[bin])*(yfit-yBIN[bin]) ) / (zBINERR[bin]*zBINERR[bin]);
	       chisq += deta;
	  }
     }
     f = chisq;
}

