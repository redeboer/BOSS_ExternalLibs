#include "MdcCalibFunSvc/MdcCalibFunSvc.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataSvc.h"

#include "CalibData/CalibModel.h"
#include "CalibData/Mdc/MdcCalibData.h"
#include "CalibData/Mdc/MdcDataConst.h"
#include "EventModel/EventHeader.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "TFile.h"
#include "TTree.h"
#include "TList.h"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

typedef map<int, double>::value_type valType;

MdcCalibFunSvc::MdcCalibFunSvc( const string& name, ISvcLocator* svcloc) :
     Service (name, svcloc), m_layInfSig(-1) {

     // declare properties
     declareProperty("CheckConst", m_checkConst = false);
     declareProperty("LayerInfSig", m_layInfSig);
     declareProperty("XtMode", m_xtMode = 1);
     declareProperty("NewXtFile", m_xtfile);
     declareProperty("ReadWireEffDb", m_readWireEffDb = true);
     declareProperty("WireEffFile", m_wireEffFile);
     declareProperty("LinearXT", m_linearXT = false);
     declareProperty("FixSigma", m_fixSigma = false);
     declareProperty("FixSigmaValue", m_fixSigmaValue = 130.0); // micron
     m_outputXtMode = true;
}

MdcCalibFunSvc::~MdcCalibFunSvc(){
}

StatusCode MdcCalibFunSvc::queryInterface(const InterfaceID& riid, void** ppvInterface){
     if( IID_IMdcCalibFunSvc.versionMatch(riid) ){
	  *ppvInterface = static_cast<IMdcCalibFunSvc*> (this);
     } else{
	  return Service::queryInterface(riid, ppvInterface);
     }
     return StatusCode::SUCCESS;
}

StatusCode MdcCalibFunSvc::initialize(){
     MsgStream log(messageService(), name());
     log << MSG::INFO << "MdcCalibFunSvc::initialize()" << endreq;

     StatusCode sc = Service::initialize();
     if( sc.isFailure() ) return sc;

     IIncidentSvc* incsvc;
     sc = service("IncidentSvc", incsvc);
     int priority = 100;
     if( sc.isSuccess() ){
	  incsvc -> addListener(this, "NewRun", priority);
     }

     sc = service("CalibDataSvc", m_pCalDataSvc, true);
     if( sc == StatusCode::SUCCESS ){
	  log << MSG::INFO << "Retrieve IDataProviderSvc" << endreq;
     }else{
	  log << MSG::FATAL << "can not get IDataProviderSvc" << endreq;
     }

     sc = service("MdcGeomSvc", m_pMdcGeomSvc);
     if( sc != StatusCode::SUCCESS ){
	  log << MSG::ERROR << "can not use MdcGeomSvc" << endreq;
	  return StatusCode::FAILURE;
     }

     if(m_fixSigma) cout << "Fix MDC sigma to " << m_fixSigmaValue << " micron." << endl;

     m_updateNum = 0;
     for(int wir=0; wir<6796; wir++) m_wireEff[wir] = 1.0;
     for(int lay=0; lay<NLAYER; lay++){
	  for(int iEntr=0; iEntr<NXTENTR; iEntr++){
	       for(int lr=0; lr<2; lr++) m_nR2t[lay][iEntr][lr] = 0;
	  }
     }

     return StatusCode::SUCCESS;
}

StatusCode MdcCalibFunSvc::finalize(){
     MsgStream log(messageService(), name());
     log << MSG::INFO << "MdcCalibFunSvc::finalize()" << endreq;

     m_xtmap.clear();
     m_t0.clear();
     m_delt0.clear();
     m_qtpar0.clear();
     m_qtpar1.clear();
     m_sdmap.clear();

     return StatusCode::SUCCESS;
}

void MdcCalibFunSvc::handle(const Incident& inc){
     MsgStream log( messageService(), name() );
     log << MSG::DEBUG << "handle: " << inc.type() << endreq;

     if ( inc.type() == "NewRun" ){
	  log << MSG::DEBUG << "NewRun" << endreq;

	  if( ! initCalibConst() ){
	       log << MSG::ERROR 
		   << "can not initilize Mdc Calib Constants" << endl
		   << "  Please insert the following statement "
		   << "in your \"jobOption.txt\" "
		   << "before the include file of Mdc Reconstruction: "
		   << endl << "        "
		   << "#include \"$CALIBSVCROOT/share/job-CalibData.txt\""
		   << endl
		   << "  If still error, please contact with Wu Linghui "
		   << "(wulh@mail.ihep.ac.cn)."
		   << endreq;
	  }
     } 
}

double MdcCalibFunSvc::getTprop(int lay, double z) const{
     double vp = getVprop(lay);
     double tp = fabs(z - m_zst[lay]) / vp;
     return tp;
}

double MdcCalibFunSvc::driftTimeToDist(double drifttime, int layid, int cellid,
				       int lr, double entrance) const {
     double dist;
     if(0 == m_xtMode){
	  dist = t2dPoly(drifttime, layid, cellid, lr, entrance);
     } else{
	  if((0==lr) || (1==lr)) dist = t2dInter(drifttime, layid, cellid, lr, entrance);
	  else{
	       double dl = t2dInter(drifttime, layid, cellid, lr, entrance);
	       double dr = t2dInter(drifttime, layid, cellid, lr, entrance);
	       dist = (dl + dr) * 0.5;
	  }
     }
//      cout << setw(15) << drifttime << setw(15) << dist << endl;
     if(m_linearXT) dist = 0.03 * drifttime;
     return dist;
}

double MdcCalibFunSvc::t2dPoly(double drifttime, int layid, int cellid,
			       int lr, double entrance) const {
     int ord;
     double xtpar[8];
     double dist = 0.0;

     int entr = getXtEntrIndex(entrance);
     getXtpar(layid, entr, lr, xtpar);

     if(drifttime < xtpar[6]){
	  for(ord=0; ord<6; ord++){
	       dist += xtpar[ord] * pow(drifttime, ord);
	  }
     } else{
	  for(ord=0; ord<6; ord++){
	       dist += xtpar[ord] * pow(xtpar[6], ord);
	  }
	  dist += xtpar[7] * (drifttime - xtpar[6]);
     }

     return dist;
}

double MdcCalibFunSvc::t2dInter(double drifttime, int layid, int cellid,
				int lr, double entrance) const {
     double dist;
     int iEntr = getXtEntrIndex(entrance);
     int nBin = m_nNewXt[layid][iEntr][lr];
     if(drifttime < m_vt[layid][iEntr][lr][0]){
	  dist = m_vd[layid][iEntr][lr][0];
     } else if(drifttime < m_vt[layid][iEntr][lr][nBin-1]){
	  for(int i=0; i<(nBin-1); i++){
	       if((drifttime>=m_vt[layid][iEntr][lr][i]) && (drifttime<m_vt[layid][iEntr][lr][i+1])){
		    double t1 = m_vt[layid][iEntr][lr][i];
		    double t2 = m_vt[layid][iEntr][lr][i+1];
		    double d1 = m_vd[layid][iEntr][lr][i];
		    double d2 = m_vd[layid][iEntr][lr][i+1];
		    dist = (drifttime-t1) * (d2-d1) / (t2-t1) + d1;
		    break;
	       }
	  }
     } else{
	  dist = m_vd[layid][iEntr][lr][nBin-1];
     }
     return dist;
}

double MdcCalibFunSvc::distToDriftTime(double dist, int layid, int cellid,
				       int lr, double entrance) const {
     int i = 0;
     double time;
     int ord;
     double xtpar[8];
     double dxdtpar[5];
     double x;
     double dxdt;
     double deltax;

     int entr = getXtEntrIndex(entrance);
     getXtpar(layid, entr, lr, xtpar);

     double tm1 = xtpar[6];
     double tm2 = 2000.0;
     double dm1 = driftTimeToDist(tm1, layid, cellid, lr, entrance);
     double dm2 = driftTimeToDist(tm2, layid, cellid, lr, entrance);

     if(dist < 0){
	  cout << "Warning in MdcCalibFunSvc: driftDist < 0" << endl;
	  time = 0.0;
     } else if(dist < xtpar[0]){
	  time = 0.0;
     } else if(dist < dm1){
	  for(ord=0; ord<5; ord++){
	       dxdtpar[ord] = (double)(ord+1) * xtpar[ord+1];
	  }
	  time = dist / 0.03;
	  while(1){
	       if( i > 50 ){
		    cout << "Warning in MdcCalibFunSvc: "
			 << "can not get the exact value in the dist-to-time conversion." 
			 << endl;
		    time = dist / 0.03;
		    break;
	       }

	       x = 0.0;
	       for(ord=0; ord<6; ord++)
		    x += xtpar[ord] * pow(time, ord);

	       deltax = x - dist;
	       if( fabs(deltax) < 0.001 ){
		    break;
	       }

	       dxdt = 0.0;
	       for(ord=0; ord<5; ord++)
		    dxdt += dxdtpar[ord] * pow(time, ord);

	       time = time - (deltax / dxdt);
	       i++;
	  }
     } else if(dist < dm2){
	  time = (dist - dm1) * (tm2 - tm1) / (dm2 - dm1) + tm1;
     } else{
	  time = tm2;
     }

     if(m_linearXT) time = dist / 0.03;
     return time;
}

double MdcCalibFunSvc::getSigma(int layid, int lr, double dist,
				double entrance, double tanlam,
				double z, double Q) const {
     double sigma;
     if( (0 == lr) || (1 == lr) ){
	  sigma = getSigmaLR(layid, lr, dist, entrance, tanlam, z, Q);
     } else{
	  double sl = getSigmaLR(layid, 0, dist, entrance, tanlam, z, Q);
	  double sr = getSigmaLR(layid, 1, dist, entrance, tanlam, z, Q);
	  sigma = (sl + sr) * 0.5;
     }

     if(m_fixSigma) sigma = 0.001 * m_fixSigmaValue;
     if(layid == m_layInfSig) sigma = 9999.0;
     return sigma;
}

double MdcCalibFunSvc::getSigmaLR(int layid, int lr, double dist,
				  double entrance, double tanlam,
				  double z, double Q) const {

     double sigma = 9999.0;
     double par[NSDBIN];

     int entr = getSdEntrIndex(entrance);
     getSdpar(layid, entr, lr, par);

     int nmaxBin;
     double dw = 0.5;		// width of each distance bin
     double dmin = 0.25;	// mm
     if(layid < 8){
	  nmaxBin = 20; // 11->20 2011-12-10
     } else{
	  nmaxBin = 20; // 15->20 2011-12-10
     }

     double dref[2];
     double distAbs = fabs(dist);
     if(distAbs < dmin){
	  sigma = par[0];
     } else{
	  int bin = (int)((distAbs - dmin) / dw);
	  if(bin >= nmaxBin){
	       sigma = par[nmaxBin];
	  } else if(bin < 0){
	       sigma = par[0];
	  } else{
	       dref[0] = (double)bin * dw + 0.25;
	       dref[1] = (double)(bin+1) * dw + 0.25;
	       if((dref[1] - dref[0]) <= 0){
		    sigma = 9999.0;
	       } else{
		    sigma = (par[bin+1] - par[bin]) * (distAbs - dref[0]) /
			 (dref[1] - dref[0]) + par[bin];
	       }
	  }
     }
     return sigma;
}

double MdcCalibFunSvc::getSigma1(int layid, int lr, double dist,
				 double entrance, double tanlam,
				 double z, double Q) const {
     double sigma1 = getSigma(layid, lr, dist, entrance, tanlam, z, Q);
     return sigma1;
}

double MdcCalibFunSvc::getSigma2(int layid, int lr, double dist,
				 double entrance, double tanlam,
				 double z, double Q) const {

     return 0.0;
}

double MdcCalibFunSvc::getF(int layid, int lr, double dist,
			    double entrance, double tanlam,
			    double z, double Q) const {

     return 1.0;
}

double MdcCalibFunSvc::getSigmaToT(int layid, int lr, double tdr, double entrance,
				   double tanlam, double z, double Q) const{
     if(!m_fgR2t){
	  cout << "ERROR: can not get sigma-time functions" << endl;
	  return -999.0;
     } else if( (0 == lr) || (1 == lr) ){
	  return getSigmaToTLR(layid, lr, tdr, entrance, tanlam, z, Q);
     } else{
	  double sl = getSigmaToTLR(layid, 0, tdr, entrance, tanlam, z, Q);
	  double sr = getSigmaToTLR(layid, 1, tdr, entrance, tanlam, z, Q);
	  double sigma = (sl + sr) * 0.5;
	  return sigma;
     }
}

double MdcCalibFunSvc::getSigmaToTLR(int layid, int lr, double tdr, double entrance,
				     double tanlam, double z, double Q) const{
     double sigma;
     int iEntr = getXtEntrIndex(entrance);
     int nBin = m_nR2t[layid][iEntr][lr];
     if(tdr < m_tR2t[layid][iEntr][lr][0]){
	  sigma = m_sR2t[layid][iEntr][lr][0];
     } else if(tdr < m_tR2t[layid][iEntr][lr][nBin-1]){
	  for(int i=0; i<(nBin-1); i++){
	       if((tdr>=m_tR2t[layid][iEntr][lr][i]) && (tdr<m_tR2t[layid][iEntr][lr][i+1])){
		    double t1 = m_tR2t[layid][iEntr][lr][i];
		    double t2 = m_tR2t[layid][iEntr][lr][i+1];
		    double s1 = m_sR2t[layid][iEntr][lr][i];
		    double s2 = m_sR2t[layid][iEntr][lr][i+1];
		    sigma = (tdr-t1) * (s2-s1) / (t2-t1) + s1;
		    break;
	       }
	  }
     } else{
	  sigma = m_sR2t[layid][iEntr][lr][nBin-1];
     }
     return sigma;
}

int MdcCalibFunSvc::getNextXtpar(int& key, double& par){
     if( m_xtiter != m_xtmap.end() ){
	  key = (*m_xtiter).first;
	  par = (*m_xtiter).second;
	  m_xtiter++;
	  return 1;
     }
     else return 0;
}

void MdcCalibFunSvc::getXtpar(int layid, int entr, int lr, double par[]) const{
     int parId;
     for(int ord=0; ord<8; ord++){
	  parId = getXtparId(layid, entr, lr, ord);
	  par[ord] = m_xtpar[parId];
     }
}

bool MdcCalibFunSvc::getNewXtpar() {
     MsgStream log(messageService(), name());
     log << MSG::INFO << "read calib const from TCDS" << endreq;

     for(int layid=0; layid<NLAYER; layid++){
	  for(int entr=0; entr<NXTENTR; entr++){
	       for(int lr=0; lr<2; lr++){
		    double br_t,br_d;
		    TTree* newXtTree = getNewXtparTree(layid,entr,lr);
		    if(!newXtTree) return false;
		    newXtTree -> SetBranchAddress("t", &br_t);
		    newXtTree -> SetBranchAddress("d", &br_d);
		    int nEntries = newXtTree -> GetEntries();
		    if((nEntries<10) || (nEntries>=200)){
			 log << MSG::ERROR << "wrong X-T constants: layer " << layid
			     << ", iEntr " << entr << ", lr " << lr << endreq;
			 return false;
		    }
		    m_nNewXt[layid][entr][lr] = nEntries;
		    for(int i=0; i<nEntries; i++){
			 newXtTree->GetEntry(i);
			 m_vt[layid][entr][lr][i] = br_t;
			 m_vd[layid][entr][lr][i] = br_d;
		    }//end loop entries
	       }//end lr
	  }//end entr
     }//end layid

     return true;
}

TTree* MdcCalibFunSvc::getNewXtparTree(int layid, int entr, int lr) const{
     MsgStream log(messageService(), name());
     string fullPath = "/Calib/MdcCal";
     SmartDataPtr<CalibData::MdcCalibData> calConst(m_pCalDataSvc, fullPath);
     if( ! calConst ){
	  log << MSG::ERROR << "can not get MdcCalibConst via SmartPtr" << endreq;
	  return NULL;
     }

     TTree* newXtTree = calConst->getNewXtpar(layid,entr,lr);
     return newXtTree;
}

bool MdcCalibFunSvc::getR2tpar() {
     for(int layid=0; layid<NLAYER; layid++){
	  int br_iEntr,br_lr;
	  double br_s,br_t;
	  TTree* r2tTree = getR2tTree(layid);
	  if(!r2tTree) return false;
	  r2tTree -> SetBranchAddress("iEntr", &br_iEntr);
	  r2tTree -> SetBranchAddress("lr", &br_lr);
	  r2tTree -> SetBranchAddress("s", &br_s);
	  r2tTree -> SetBranchAddress("t", &br_t);
	  int nEntries = r2tTree -> GetEntries();
	  for(int i=0; i<nEntries; i++){
	       r2tTree->GetEntry(i);
	       int bin = m_nR2t[layid][br_iEntr][br_lr];
	       if(bin>=200){
		    cout << "Error: number of sigma-time bins overflow" << endl;
		    return false;
	       }
	       m_tR2t[layid][br_iEntr][br_lr][bin] = br_t;
	       m_sR2t[layid][br_iEntr][br_lr][bin] = br_s;
	       m_nR2t[layid][br_iEntr][br_lr]++;
	  }
     }
     for(int layid=0; layid<NLAYER; layid++){
	  for(int iEntr=0; iEntr<NXTENTR; iEntr++){
	       for(int lr=0; lr<2; lr++){
		    if((m_nR2t[layid][iEntr][lr]<10) || (m_nR2t[layid][iEntr][lr]>=200)) return false;
	       }
	  }
     }
     return true;
}

TTree* MdcCalibFunSvc::getR2tTree(int layid) const{
     MsgStream log(messageService(), name());
     string fullPath = "/Calib/MdcCal";
     SmartDataPtr<CalibData::MdcCalibData> calConst(m_pCalDataSvc, fullPath);
     if( ! calConst ){
	  log << MSG::ERROR << "can not get MdcCalibConst via SmartPtr" << endreq;
	  return NULL;
     }

     TTree* r2tTree = calConst->getR2tpar(layid);
     return r2tTree;
}


double MdcCalibFunSvc::getT0(int layid, int cellid) const {
     int wireid = m_pMdcGeomSvc->Wire(layid, cellid)->Id();
     double t0 = getT0(wireid);

     return t0;
}

double MdcCalibFunSvc::getTimeWalk(int layid, double Q) const {
     double tw = 0.0;
     double qtpar[2];
     int ord;

     if(Q < 0.0001) Q = 0.0001;

     for(ord=0; ord<2; ord++){
	  qtpar[ord] = getQtpar(layid, ord);
     }

     tw = qtpar[0] + qtpar[1] / sqrt( Q );
     if(m_run < 0) tw = 0.0;	// for MC

     return tw;
}

double MdcCalibFunSvc::getWireEff(int layid, int cellid) const {
     int wireid = m_pMdcGeomSvc->Wire(layid, cellid)->Id();
     return m_wireEff[wireid];
}

double MdcCalibFunSvc::getQtpar(int layid, int ord) const {
     if(0 == ord)       return m_qtpar0[layid];
     else if(1 == ord)  return m_qtpar1[layid];
     else {
	  cout << "wrong order number" << endl;
	  return 0.0;
     }
}

void MdcCalibFunSvc::getSdpar(int layid, int entr, int lr, double par[]) const{
     int parId;
     if( (entr < 0) || (entr >= 18) ){
	  entr = 17;
     }
     for(int bin=0; bin<NSDBIN; bin++){
	  parId = getSdparId(layid, entr, lr, bin);
	  par[bin] = m_sdpar[parId];
     }
}

int MdcCalibFunSvc::getNextSdpar(int& key, double& par){
     if( m_sditer != m_sdmap.end() ){
	  key = (*m_sditer).first;
	  par = (*m_sditer).second;
	  m_sditer++;
	  return 1;
     }
     else return 0;
}

int MdcCalibFunSvc::getXtEntrIndex(double entrance) const {
     int i;
     int index;
     int idmax = 17;
     double aglpi = 3.141592653;
     double aglmin = -1.570796327; // -90 degree
     double aglmax = 1.570796327; // 90 degree
     double delAngle = 0.174532925; // 10 degree

     MsgStream log(messageService(), name());
     if(entrance < aglmin){
	  log << MSG::WARNING << "entrance angle < -pi/2" << endreq;
	  while(1){
	       entrance += aglpi;
	       if(entrance >= aglmin) break;
	  }
     } else if(entrance > aglmax){
	  log << MSG::WARNING << "entrance angle > pi/2" << endreq;
	  while(1){
	       entrance -= aglpi;
	       if(entrance <= aglmax) break;
	  }
     }

     index = (int)((entrance-aglmin) / delAngle);
     if(index < 0) index = 0;
     else if(index > idmax) index = idmax;

     return index;
}

int MdcCalibFunSvc::getSdEntrIndex(double entrance) const {
     int i;
     int index;
     int idmax = 5;
     double aglpi = 3.141592653;
     double aglmin = -1.570796327; // -90 degree
     double aglmax = 1.570796327; // 90 degree
     double delAngle = 0.523598776; // 30 degree

     MsgStream log(messageService(), name());
     if(entrance < aglmin){
	  log << MSG::WARNING << "entrance angle < -pi/2" << endreq;
	  while(1){
	       entrance += aglpi;
	       if(entrance >= aglmin) break;
	  }
     } else if(entrance > aglmax){
	  log << MSG::WARNING << "entrance angle > pi/2" << endreq;
	  while(1){
	       entrance -= aglpi;
	       if(entrance <= aglmax) break;
	  }
     }

     index = (int)((entrance-aglmin) / delAngle);
     if(index < 0) index = 0;
     else if(index > idmax) index = idmax;

     return index;
}

bool MdcCalibFunSvc::initCalibConst(){
     MsgStream log(messageService(), name());
     log << MSG::INFO << "read calib const from TCDS" << endreq;

     IDataProviderSvc* eventSvc = NULL;
     Gaudi::svcLocator()->service("EventDataSvc", eventSvc);
     SmartDataPtr<Event::EventHeader> eventHeader(eventSvc,"/Event/EventHeader");
     if (!eventHeader) {
	  log << MSG::FATAL << "Could not find Event Header" << endreq;
	  return( StatusCode::FAILURE);
     }
     m_run = eventHeader->runNumber();

     // clear calibration constants vectors
     m_xtmap.clear();
     m_xtpar.clear();
     m_t0.clear();
     m_delt0.clear();
     m_qtpar0.clear();
     m_qtpar1.clear();
     m_sdmap.clear();
     m_sdpar.clear();

     string fullPath = "/Calib/MdcCal";
     SmartDataPtr<CalibData::MdcCalibData> calConst(m_pCalDataSvc, fullPath);
     if( ! calConst ){
	  log << MSG::ERROR << "can not get MdcCalibConst via SmartPtr" 
	      << endreq;
	  return false;
     }

     // initialize XT parameter
     int layid;
     int entr;
     int lr;
     int ord;
     int key;
     double par;
     calConst -> setXtBegin();
     while( calConst->getNextXtpar(key, par) ){
	  m_xtmap.insert( valType(key, par) );
     }

     int parId;
     unsigned mapsize = m_xtmap.size();
     m_xtpar.resize(mapsize);
     log << MSG::INFO << "size of xtmap: " << mapsize << endreq;

     calConst -> setXtBegin();
     while( calConst->getNextXtpar(key, par) ){
	  layid = (key >> XTLAYER_INDEX) & XTLAYER_DECO;
	  entr = (key >> XTENTRA_INDEX) & XTENTRA_DECO;
	  lr = (key >> XTLR_INDEX) & XTLR_DECO;
	  ord = (key >> XTORDER_INDEX) & XTORDER_DECO;

	  parId = getXtparId(layid, entr, lr, ord);
	  m_xtpar[parId] = par;
     }

     // initialize T0 parameter
     int wid;
     double t0;
     double delt0;
     for(wid=0; wid<NWIRE; wid++){
	  t0 = calConst->getT0(wid);
	  delt0 = calConst->getDelT0(wid);

	  m_t0.push_back(t0);
	  m_delt0.push_back(delt0);
     }

     // initialize QT parameter
     double qtpar0;
     double qtpar1;
     for(layid=0; layid<NLAYER; layid++){
	  qtpar0 = calConst -> getQtpar0(layid);
	  qtpar1 = calConst -> getQtpar1(layid);

	  m_qtpar0.push_back(qtpar0);
	  m_qtpar1.push_back(qtpar1);
     }

     // initialize resolution parameter
     calConst -> setSdBegin();
     while( calConst -> getNextSdpar(key, par) ){
	  m_sdmap.insert( valType(key, par) );
// 	  cout << setw(15) << key << setw(15) << par << endl;
     }

     mapsize = m_sdmap.size();
     m_sdpar.resize(mapsize);
     log << MSG::INFO << "size of sdmap: " << mapsize << endreq;

     calConst -> setSdBegin();
     while( calConst -> getNextSdpar(key, par) ){
	  layid = (key >> SDLAYER_INDEX) & SDLAYER_DECO;
	  entr = (key >> SDENTRA_INDEX) & SDENTRA_DECO;
	  lr = (key >> SDLR_INDEX) & SDLR_DECO;
	  ord = (key >> SDBIN_INDEX) & SDBIN_DECO;

	  parId = getSdparId(layid, entr, lr, ord);
	  m_sdpar[parId] = par;
     }

     double zeast;
     double zwest;
     for(layid=0; layid<NLAYER; layid++){
	  zwest = m_pMdcGeomSvc->Wire(layid, 0)->Forward().z();
	  zeast = m_pMdcGeomSvc->Wire(layid, 0)->Backward().z();

	  if(0 == (layid % 2)) m_zst[layid] = zwest;	// west end
	  else m_zst[layid] = zeast; // east end
     }

     // read new-XT
     log << MSG::INFO << "read new xt from TCDS" << endreq;
     if (!getNewXtpar()){
	  log << MSG::WARNING << "can not get MDC New XT Trees" << endreq;
	  m_xtMode = 0;
     }
     if(m_run < 0) m_xtMode = 0;
     if(0 == m_xtMode) log << MSG::INFO << "use old X-T functions " << endreq;
     else log << MSG::INFO << "use new X-T functions " << endreq;
     if(m_outputXtMode){
	  if(0 == m_xtMode) cout << "use old X-T functions " << endl;
	  else cout << "use new X-T functions " << endl;
	  m_outputXtMode = false;
     }

     // read r-t
     for(layid=0; layid<NLAYER; layid++){
	  for(entr=0; entr<NXTENTR; entr++){
	       for(lr=0; lr<2; lr++) m_nR2t[layid][entr][lr] = 0;
	  }
     }
     m_fgR2t = true;
     log << MSG::INFO << "read new sigma-time from TCDS" << endreq;
     if (!getR2tpar()){
	  log << MSG::WARNING << "can not get sigma-time functions" << endreq;
	  m_fgR2t = false;
     } else{
	  log << MSG::INFO << "read sigma-time successfully " << endreq;
     }

     // read wire efficiency
     if(m_readWireEffDb){
	  fullPath = "/Calib/MdcDataConst";
	  log << MSG::INFO <<"Read Wire Eff from TCDS: "<< fullPath << endreq;
	  log << MSG::INFO << "Read wire eff!" << endreq;

	  SmartDataPtr<CalibData::MdcDataConst> dataConst(m_pCalDataSvc, fullPath);
	  if(!dataConst){
	       log << MSG::ERROR << "can not get MdcDataConst via SmartPtr" << endreq;
	       return false; 
	  }
	  for(int wir=0; wir<NWIRE; wir++) {
	       m_wireEff[wir] = dataConst->getWireEff(wir);
	  }
     } else{
	  log << MSG::INFO <<"Read Wire Eff from file: "<< m_wireEffFile << endreq;
	  ifstream fEff(m_wireEffFile.c_str());
	  if(!fEff.is_open()){
	       log << MSG::ERROR << "can not open wire eff file: " << m_wireEffFile << endreq;
	       return false;
	  } else{
	       string strtmp;
	       for(int i=0; i<4; i++) fEff >> strtmp;
	       for(int wir=0; wir<NWIRE; wir++) fEff >> strtmp >> strtmp >> strtmp >> m_wireEff[wir];
	       fEff.close();
	  }
     }
     if(m_checkConst) checkConst();
     m_updateNum++;

     return true;
}

int MdcCalibFunSvc::getXtKey(int layid, int lr, int ord, int entr) const{

     int key = ( (layid << XTLAYER_INDEX) & XTLAYER_MASK ) |
	  ( (entr << XTENTRA_INDEX) & XTENTRA_MASK ) |
	  ( (lr << XTLR_INDEX) & XTLR_MASK ) |
	  ( (ord << XTORDER_INDEX) & XTORDER_MASK );

     return key;
}

int MdcCalibFunSvc::getSdKey(int layid, int entr, int lr, int ord) const {
     int key = ( (layid << SDLAYER_INDEX) & SDLAYER_MASK ) |
	  ( (entr << SDENTRA_INDEX) & SDENTRA_MASK ) |
	  ( (lr << SDLR_INDEX) & SDLR_MASK ) |
	  ( (ord << SDBIN_INDEX) & SDBIN_MASK );

     return key;
}

void MdcCalibFunSvc::checkConst(){
     char fname[200];
     sprintf(fname, "checkXt_%d.txt", m_updateNum);
     ofstream fxt(fname);
     unsigned mapsize = m_xtmap.size();
     unsigned vsize = m_xtpar.size();
     fxt << setw(10) << mapsize << setw(10) << vsize << endl << endl;
     int key;
     double par;
     std::map<int, double>::iterator xtiter = m_xtmap.begin();
     while(  xtiter != m_xtmap.end() ){
	  key = (*xtiter).first;
	  par = (*xtiter).second;
	  fxt << setw(20) << key << setw(20) << par << endl;
	  xtiter++;
     }
     fxt << endl;
     for(unsigned i=0; i<vsize; i++){
	  fxt << setw(5) << i << setw(15) << m_xtpar[i] << endl;
     }
     fxt.close();

     sprintf(fname, "checkT0_%d.txt", m_updateNum);
     ofstream ft0(fname);
     ft0 << setw(10) << m_t0.size() << setw(10) << m_delt0.size() << endl;
     for(unsigned i=0; i<m_t0.size(); i++){
	  ft0 << setw(5) << i << setw(15) << m_t0[i] << setw(15) << m_delt0[i] << endl;
     }
     ft0.close();

     sprintf(fname, "checkQt_%d.txt", m_updateNum);
     ofstream fqt(fname);
     fqt << setw(10) << m_qtpar0.size() << setw(10) << m_qtpar1.size() << endl;
     for(unsigned i=0; i<m_qtpar0.size(); i++){
	  fqt << setw(5) << i << setw(15) << m_qtpar0[i] << setw(15) << m_qtpar1[i] << endl;
     }
     fqt.close();

     sprintf(fname, "checkSd_%d.txt", m_updateNum);
     ofstream fsd(fname);
     mapsize = m_sdmap.size();
     vsize = m_sdpar.size();
     fsd << setw(10) << mapsize << setw(10) << vsize << endl << endl;
     std::map<int, double>::iterator sditer = m_sdmap.begin();
     while(  sditer != m_sdmap.end() ){
	  key = (*sditer).first;
	  par = (*sditer).second;
	  fsd << setw(20) << key << setw(20) << par << endl;
	  sditer++;
     }
     fsd << endl;
     for(unsigned i=0; i<vsize; i++){
	  fsd << setw(5) << i << setw(15) << m_sdpar[i] << endl;
     }
     fsd.close();

     sprintf(fname, "checkNewXt_%d.txt", m_updateNum);
     ofstream fnewxt(fname);
     for(int lay=0; lay<43; lay++){
	  for(int iEntr=0; iEntr<18; iEntr++){
	       for(int lr=0; lr<2; lr++){
		    fnewxt << setw(5) << lay << setw(5) << iEntr << setw(3) << lr
			   << setw(5) << m_nNewXt[lay][iEntr][lr] << endl;
		    for(int bin=0; bin<m_nNewXt[lay][iEntr][lr]; bin++){
			 fnewxt << setw(15) << m_vt[lay][iEntr][lr][bin]
				<< setw(15) << m_vd[lay][iEntr][lr][bin] << endl;
		    }
	       }
	  }
     }
     fnewxt.close();

     sprintf(fname, "checkR2t_%d.txt", m_updateNum);
     ofstream fr2t(fname);
     for(int lay=0; lay<43; lay++){
	  for(int iEntr=0; iEntr<18; iEntr++){
	       for(int lr=0; lr<2; lr++){
		    fr2t << setw(5) << lay << setw(5) << iEntr << setw(3) << lr
			 << setw(5) << m_nR2t[lay][iEntr][lr] << endl;
		    for(int bin=0; bin<m_nR2t[lay][iEntr][lr]; bin++){
			 fr2t << setw(15) << m_tR2t[lay][iEntr][lr][bin]
			      << setw(15) << m_sR2t[lay][iEntr][lr][bin] << endl;
		    }
	       }
	  }
     }
     fr2t.close();
}

