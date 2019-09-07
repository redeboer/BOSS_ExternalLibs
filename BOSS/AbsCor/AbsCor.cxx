#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
//#
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EmcRecEventModel/RecEmcEventModel.h"
#include "DstEvent/DstEmcShower.h"


#include "TMath.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"
using CLHEP::Hep3Vector;
using CLHEP::Hep2Vector;
using CLHEP::HepLorentzVector;
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
   typedef HepGeom::Point3D<double> HepPoint3D;
#endif

#include "EmcRecEventModel/RecEmcEventModel.h"
#include "EmcRecGeoSvc/EmcRecBarrelGeo.h"
#include "EmcRecGeoSvc/EmcRecGeoSvc.h"
#include "Identifier/TofID.h"
#include "EvTimeEvent/RecEsTime.h"



#include "AbsCor/AbsCor.h"
#include "Identifier/Identifier.h"
#include "TGraphErrors.h"
#include "TGraph2D.h"
#include "TGraph2DErrors.h"
#include <vector>
#include <string>
#include <fstream>
#include <strstream>




/////////////////////////////////////////////////////////////////////////////
//
double cbeam[56];
double ai[4];
AbsCor::AbsCor(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {

    //Declare the properties  
  ntOut = true;
  declareProperty("NTupleOut",ntOut=false);
  declareProperty("McCor", mccor=0);
  declareProperty("EdgeCor", edgecor=0);
  declareProperty("HotCellMask", hotcellmask=0);
  declareProperty("UseTof", usetof=1);
  declareProperty("DoDataCor", dodatacor = 1);
  declareProperty("DoPi0Cor",dopi0Cor=1);
  declareProperty("MCUseTof", MCuseTof=1);
}


//TGraphErrors *dt = new TGraphErrors();
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode AbsCor::initialize(){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endmsg;
  StatusCode status;
  if(ntOut == true){
    NTuplePtr nt1(ntupleSvc(), "FILE1/ec");
    if ( nt1 ) m_tuple1 = nt1;
    else {
      m_tuple1 = ntupleSvc()->book ("FILE1/ec", CLID_ColumnWiseTuple, "ks N-Tuple example");
      if ( m_tuple1 )    {
        status = m_tuple1->addItem ("ef",   m_ef);
        status = m_tuple1->addItem ("e5",   m_e5);
        status = m_tuple1->addItem ("ec",    m_ec);
        status = m_tuple1->addItem ("ct",   m_ct);
        status = m_tuple1->addItem ("cedge",   m_cedge);
      }
      else    { 
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple1) << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }

  m_index = new int*[56];
  for (int j=0;j<56;j++ ) {
    m_index[j] = new int[120];
    for ( int k=0; k<120; k++) {
      m_index[j][k]=-1;
    }
  }

  m_par = new double*[22];
  for (int j=0;j<22;j++ ) {
    m_par[j] = new double[11];
    for ( int k=0; k<11; k++) {
      m_par[j][k]=-99;
    }
  }

  m_parphi = new double*[22];
  for (int j=0;j<22;j++ ) {
    m_parphi[j] = new double[5];
    for ( int k=0; k<5; k++) {
      m_parphi[j][k]=-99;
    }
  }

  ifstream EnParIn;
  //EnParIn.exceptions( ifstream::failbit | ifstream::badbit );
  string EnParInFile;
  EnParInFile = getenv("ABSCORROOT");
  EnParInFile += "/share/para.dat";
  EnParIn.open(EnParInFile.c_str());
  for(int i=0;i<22;i++) {

    EnParIn>>m_par[i][0]>>m_par[i][1]>>m_par[i][2]>>m_par[i][3]>>m_par[i][4]
           >>m_par[i][5]>>m_par[i][6]>>m_par[i][7]>>m_par[i][8]>>m_par[i][9]
           >>m_par[i][10];
  } 
  EnParIn.close();
    
  ifstream EnParInphi;
  //EnParInphi.exceptions( ifstream::failbit | ifstream::badbit );
  string EnParInFilephi = getenv("ABSCORROOT"); 
  EnParInFilephi += "/share/paraphi.dat"; 
  EnParInphi.open(EnParInFilephi.c_str());
  for(int i=0;i<22;i++) {
    EnParInphi>>m_parphi[i][0]>>m_parphi[i][1]>>m_parphi[i][2]>>m_parphi[i][3]>>m_parphi[i][4];
    
  } 
  EnParInphi.close();


  /* liucx
  string DataPathevse;
  DataPathevse = getenv("ABSCORROOT");
  DataPathevse += "/share/barmccorevse10bin.txt";
  ifstream inpi0;
  inpi0.exceptions( ifstream::failbit | ifstream::badbit );
  inpi0.open(DataPathevse.c_str(),ios::in);

  double epoint[11],peak[11],peakerr[11];
  for(Int_t i=0;i<11;i++){
    inpi0>>epoint[i];
    inpi0>>peak[i];
    inpi0>>peakerr[i];
  }
  for(Int_t i=0;i<11;i++){
    dt->SetPoint(i, epoint[i],peak[i]);
    dt->SetPointError(i,0,peakerr[i]);
  }
  */

  string DataPathcbeam;
  DataPathcbeam = getenv("ABSCORROOT");
  DataPathcbeam += "/share/cbeam.txt";
  ifstream incbeam;
  incbeam.exceptions( ifstream::failbit | ifstream::badbit );
  incbeam.open(DataPathcbeam.c_str(),ios::in);
  for(int i=0; i<56; i++){
    double tid,peak,peake,sig,sige;
    incbeam>>tid;
    incbeam>>peak;
    incbeam>>peake;
    incbeam>>sig;
    incbeam>>sige;
    cbeam[i]=1.0/peak;
  }

  /////////////////////////////
  string DataPathc3p;
  DataPathc3p = getenv("ABSCORROOT");

  string DataPathc3ptof;
  DataPathc3ptof = getenv("ABSCORROOT");

  cout<<"mccor = "<<mccor<<endl;

  DataPathc3p += "/share/c3p.txt";
  DataPathc3ptof += "/share/c3ptof.txt";

  ifstream inc3p;
  inc3p.exceptions( ifstream::failbit | ifstream::badbit );
  if(usetof==0)inc3p.open(DataPathc3p.c_str(),ios::in);
  if(usetof==1)inc3p.open(DataPathc3ptof.c_str(),ios::in);
  for(int i=0; i<4; i++){
    double am,ame;
    inc3p>>am;
    inc3p>>ame;
    ai[i]=am;
  }

  string paraPath = getenv("ABSCORROOT");
  if(paraPath==""){} //cout<<"EmcRec environment not set!";

  // Read energy correction parameters from PhotonCor/McCor
  if(MCuseTof==1){
    paraPath += "/share/evsetTof.txt";
  }
  if(MCuseTof==0){
    paraPath += "/share/evset.txt";
  }

  ifstream in2;
  in2.open(paraPath.c_str());
  assert(in2);
  double energy,thetaid,peak1,peakerr1,res,reserr;
  dt = new TGraph2DErrors();
  dtErr = new TGraph2DErrors();
  //for(int i=0;i<560;i++){
  for(int i=0;i<1484;i++){  //53*28
    in2>>energy;
    in2>>thetaid;
    in2>>peak1;
    in2>>peakerr1;
    in2>>res;
    in2>>reserr;
    dt->SetPoint(i,energy,thetaid,peak1);
    dt->SetPointError(i,0,0,peakerr1);
    dtErr->SetPoint(i,energy,thetaid,res);
    dtErr->SetPointError(i,0,0,reserr);
    if(i<28) e25min[int(thetaid)]=energy;
    if(i>=1484-28) e25max[int(thetaid)]=energy;
    // if(i>=560-28) e25max[int(thetaid)]=energy;
  }
  in2.close();
  //-------------------------
  // Suppression of hot crystals
  // Reading the map from hotcry.txt (Hajime, Jul 2013)
  for(int ih=0;ih<10;ih++){
    hrunstart[ih]=-1;
    hrunend[ih]=-1;
    hcell[ih]=-1;
  }
  int numhots=4; // numbers of hot crystals
  int dumring,dumphi,dummod,dumid;
  string HotList;
  HotList = getenv("ABSCORROOT");
  HotList += "/share/hotcry.txt";
  ifstream hotcrys;
  hotcrys.exceptions( ifstream::failbit | ifstream::badbit );
  hotcrys.open(HotList.c_str(),ios::in);
  for(int il=0; il<numhots; il++){
    hotcrys>>hrunstart[il];
    hotcrys>>hrunend[il];
    hotcrys>>hcell[il];
    hotcrys>>dumring;
    hotcrys>>dumphi;
    hotcrys>>dummod;
    hotcrys>>dumid;
  }
  hotcrys.close();
  //-------------------------


  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;




}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode AbsCor::execute() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;
//  SmartDataPtr<Event::EventH> eventHeader(eventSvc(),"/Event");
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  int runNo=eventHeader->runNumber();

  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),  EventModel::EvtRec::EvtRecTrackCol);
  if(evtRecEvent->totalTracks()>evtRecTrkCol->size())return SUCCESS;
  if(evtRecEvent->totalTracks()>50)return SUCCESS;

  IEmcRecGeoSvc* iGeoSvc;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("EmcRecGeoSvc",iGeoSvc);
  if(sc!=StatusCode::SUCCESS) {
    cout<<"Error: Can't get EmcRecGeoSvc"<<endl;
  }

  for(int i = 0; i< evtRecEvent->totalTracks(); i++) {
    EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isEmcShowerValid())continue;
    RecEmcShower *emcTrk = (*itTrk)->emcShower();
//    if(emcTrk->energy()<0.0005)continue;

    /* by liucx
    int st = emcTrk->status();
    if(st>10000)continue;
     emcTrk->setStatus(st+20000);
    */



//    cout<<"REC after calib EMC status =  "<<emcTrk->status()<<endl;

    //if(emcTrk->e5x5()<0.015)continue;

//    if(emcTrk->getTime()<10||emcTrk->getTime()>30)continue;
/*
    if(emcTrk->e5x5()<0.02){
      emcTrk->setEnergy(emcTrk->e5x5()*1.1);
      continue;
    }
*/
/*
    int intid = emcTrk->cellId();
    Identifier id=Identifier(intid);
    int getthetaid = EmcID::theta_module(id);
    int getmodule = EmcID::barrel_ec(id);
    if(getmodule==1)getthetaid=getthetaid+6;
    if(getmodule==2)getthetaid=55-getthetaid;
*/

    RecEmcID id = Identifier(emcTrk->cellId());

    unsigned int module, ntheta, nphi;
    module = EmcID::barrel_ec(id);
    ntheta = EmcID::theta_module(id);
    nphi = EmcID::phi_module(id);

    // id=EmcID::crystal_id(module,ntheta,nphi);

    unsigned int thetaModule = EmcID::theta_module(id);
    unsigned int phiModule = EmcID::phi_module(id);

 
    double e5x5=emcTrk->e5x5();
    double etof=0;

    if(usetof==1 && (*itTrk)->isTofTrackValid()){
      SmartRefVector<RecTofTrack> recTofTrackVec=(*itTrk)->tofTrack();
      if(!recTofTrackVec.empty())etof=recTofTrackVec[0]->energy();
      if(etof>100.)etof=0;
    }

    double energyC;

    int thetaId;
    if (module==0||module==2) thetaId = thetaModule;
    if (module==1 && thetaModule<=21) thetaId = thetaModule + 6;
    if (module==1 && thetaModule>21)  thetaId = 43 - thetaModule + 6; 

    if(MCuseTof==1){
      energyC=ECorrMC(e5x5+etof,thetaId);

    } 
    if (MCuseTof==0) {
      energyC=ECorrMC(e5x5,thetaId);
  
    }


    /*
    if(usetof==1){
      energyC=ECorrMC(e5x5+etof,thetaId);
    } 
    if (usetof==0) {
      energyC=emcTrk->energy();
    }
    */
    //  double energyC=emcTrk->energy()+etof;
/*
    if(energyC<=0||energyC>4.99)continue;
    double cor = 1.0; 
    if(runNo>0)cor = dt->Eval(energyC);
    if(cor<0.001)continue;
//  cout<<cor<<endl;
    double energyCC= energyC/cor;
    emcTrk->setEnergy(energyCC);
*/

    double lnE = std::log(energyC);

    if (energyC>1.0) lnE=std::log(1.0);
    if (energyC<0.07) lnE=std::log(0.07);

    double lnEcor=1.0;
    if(dopi0Cor==1){
      if(runNo>0 && dodatacor==1) {
	lnEcor = ai[0]+ai[1]*lnE+ai[2]*lnE*lnE+ai[3]*lnE*lnE*lnE;
      }
    }
    if(lnEcor<0.5)continue;
//    cout<<lnEcor<<" "<<etof<<endl;


//////////////////////////////////////now  no using in the following
    HepPoint3D pos(emcTrk->position());

    // If it is "hot", return "9999" (Hajime, Jul 2013)
    double enecor=1.;
    if(hotcellmask==1){
      for(int ih=0;ih<10;ih++){
	if(hrunstart[ih]==-1||hrunend[ih]==-1||hcell[ih]==-1)continue;
	if(abs(runNo)<hrunstart[ih]||abs(runNo)>hrunend[ih])continue;
	if (  emcTrk->cellId()==hcell[ih] ) {emcTrk->setStatus(9999);}
      }
    }

    if(edgecor==1){

      if(module==1) {   //barrel
	unsigned int theModule;
	if(thetaModule>21) {
	  theModule = 43 - thetaModule;
	  id = EmcID::crystal_id(module,theModule,phiModule);
	  pos.setZ(-pos.z());
	} else {
	  theModule = thetaModule;
	}
	
	double IRShift;
	if(theModule==21) {
	  IRShift = 0;
	  
	} else if(theModule==20) {
	  IRShift = 2.5;
	  
	} else {
	  IRShift = 5.;
	  
	}


	HepPoint3D IR(0,0,IRShift);
	HepPoint3D center01, center23;  //center of point0,1 and point2,3 in crystal
	center01 = (iGeoSvc->GetCrystalPoint(id,0)+iGeoSvc->GetCrystalPoint(id,1))/2;
	center23 = (iGeoSvc->GetCrystalPoint(id,2)+iGeoSvc->GetCrystalPoint(id,3))/2;
	
	double theta01,theta23,length,d;
	theta01 = (center01-IR).theta();
	theta23 = (center23-IR).theta();
	length = (center01-IR).mag();
	d = length*tan(theta01-theta23);  //length in x direction
	
	double xIn;
	xIn = length*tan(theta01-(pos-IR).theta())-d/2;
	if (xIn < (-d/2) && theModule!=21){
	  
	  theModule=theModule+1 ;
	  
	  id = EmcID::crystal_id(module,theModule,phiModule);
	  if(theModule==21) {
	    IRShift = 0;
	    
	  } else if(theModule==20) {
	    IRShift = 2.5;
	    
	  } else {
	    IRShift = 5.;
	    
	  }
	  HepPoint3D IR1(0,0,IRShift);
	  IR=IR1;
	  center01 = (iGeoSvc->GetCrystalPoint(id,0)+iGeoSvc->GetCrystalPoint(id,1))/2;
	  center23 = (iGeoSvc->GetCrystalPoint(id,2)+iGeoSvc->GetCrystalPoint(id,3))/2;
	  theta01 = (center01-IR).theta();
	  theta23 = (center23-IR).theta();
	  length = (center01-IR).mag();
	  d = length*tan(theta01-theta23);  //length in x direction
	  
	  xIn = length*tan(theta01-(pos-IR).theta())-d/2;
	  
	} else if (xIn > (d/2) && theModule!=0 ){
	  
	  theModule=theModule-1 ;
	  id = EmcID::crystal_id(module,theModule,phiModule);
	  if(theModule==21) {
	    IRShift = 0;
	    
	  } else if(theModule==20) {
	    IRShift = 2.5;
	    
	  } else {
	    IRShift = 5.;
	    
	  }
	  
	  HepPoint3D IR1(0,0,IRShift);
	  IR=IR1;
	  center01 = (iGeoSvc->GetCrystalPoint(id,0)+iGeoSvc->GetCrystalPoint(id,1))/2;
	  center23 = (iGeoSvc->GetCrystalPoint(id,2)+iGeoSvc->GetCrystalPoint(id,3))/2;
	  theta01 = (center01-IR).theta();
	  theta23 = (center23-IR).theta();
	  length = (center01-IR).mag();
	  d = length*tan(theta01-theta23);  //length in x direction
	  
	  xIn = length*tan(theta01-(pos-IR).theta())-d/2;
	}
	
	double yIn;
	//    yIn = pos.phi() < 0 ? pos.phi()*180./CLHEP::pi+360.-phiModule*3.-1.1537
	//      : pos.phi()*180./CLHEP::pi-phiModule*3.-1.1537;
	
	yIn = pos.phi() < 0 ? pos.phi()*180./CLHEP::pi+360.-phiModule*3.-1.843
	  : pos.phi()*180./CLHEP::pi-phiModule*3.-1.843;
	
	if(nphi==0&&yIn>100) yIn=yIn-360;
	if(nphi==119&&yIn<-100) yIn=yIn+360;
	
	if(yIn<-1.5-0.15){
	  
	  if(nphi!=0) phiModule=phiModule-1;
	  if(nphi==0) phiModule=119;
	  yIn = pos.phi() < 0 ? pos.phi()*180./CLHEP::pi+360.-phiModule*3.-1.843
	    : pos.phi()*180./CLHEP::pi-phiModule*3.-1.843;
	  if(phiModule==0&&yIn>100) yIn=yIn-360;
	  if(phiModule==119&&yIn<-100) yIn=yIn+360;
	  
	}
	
	if(yIn>1.5-0.15){
	  
	  
	  if(nphi!=119) phiModule=phiModule+1;
	  if(nphi==119) phiModule=0;
	  
	  yIn = pos.phi() < 0 ? pos.phi()*180./CLHEP::pi+360.-phiModule*3.-1.843
	    : pos.phi()*180./CLHEP::pi-phiModule*3.-1.843;
	  if(phiModule==0&&yIn>100) yIn=yIn-360;
	  if(phiModule==119&&yIn<-100) yIn=yIn+360;
	}
	
	enecor=m_par[theModule][0]
	  +m_par[theModule][1]*xIn
	  +m_par[theModule][2]*xIn*xIn
	  +m_par[theModule][3]*xIn*xIn*xIn
	  +m_par[theModule][4]*xIn*xIn*xIn*xIn
	  +m_par[theModule][5]*xIn*xIn*xIn*xIn*xIn
	  +m_par[theModule][6]*xIn*xIn*xIn*xIn*xIn*xIn
	  // +m_par[theModule][7]*xIn*xIn*xIn*xIn*xIn*xIn*xIn
	  // +m_par[theModule][8]*xIn*xIn*xIn*xIn*xIn*xIn*xIn*xIn
	  +m_par[theModule][7]*yIn
	  +m_par[theModule][8]*yIn*yIn
	  +m_par[theModule][9]*yIn*yIn*yIn
	  +m_par[theModule][10]*yIn*yIn*yIn*yIn;
	//////////////////////
      } else{
	enecor=1;
      }
    } 
//////////////////////////////////////now  no using in the above
    
    double energyCC= energyC/(lnEcor*enecor);
    //     cout<<"Trk Level Corr. and Orig. "<<energyCC<<" "<<emcTrk->energy()<<endl;
    emcTrk->setEnergy(energyCC);

    if(ntOut==true){
      m_ef=energyCC;
      m_e5=emcTrk->e5x5();
      m_ec=energyC;
      m_ct=lnEcor ;
      m_cedge=enecor;
      m_tuple1->write();
    }


   }
//==============Modify Dst===============================================================
   SmartDataPtr<RecEmcShowerCol> recEmcShowerCol(eventSvc(),  EventModel::Recon::RecEmcShowerCol);
   if(!recEmcShowerCol){
     return( StatusCode::SUCCESS);
   }
   SmartDataPtr<DstEmcShowerCol> dstEmcShowerCol(eventSvc(),  EventModel::Dst::DstEmcShowerCol);
   if(!dstEmcShowerCol){
     return( StatusCode::SUCCESS);
   }

//   cout<<"Rec and Dst Size "<<recEmcShowerCol->size()<<" "<<dstEmcShowerCol->size()<<endl;
   if(recEmcShowerCol->size()!=dstEmcShowerCol->size())return SUCCESS;
   for(int i=0;i<recEmcShowerCol->size();i++){
     RecEmcShowerCol::iterator iter_rec = recEmcShowerCol->begin()+i;
     DstEmcShowerCol::iterator iter_dst = dstEmcShowerCol->begin()+i;
//     cout<<"Rec and Dst energy       "<<(*iter_rec)->energy()<<" "<<(*iter_dst)->energy()<<endl;
     (*iter_dst)->setEnergy((*iter_rec)->energy());
     (*iter_dst)->setStatus((*iter_rec)->status());
//     cout<<"DST after calib EMC status =  "<<(*iter_dst)->status()<<endl;
//     cout<<"Rec == Dst?              "<<(*iter_rec)->energy()<<" "<<(*iter_dst)->energy()<<endl;
   }
  return( StatusCode::SUCCESS);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode AbsCor::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}



double AbsCor::E25min(int n) const
{
  return e25min[n];
}
double AbsCor::E25max(int n) const
{
  return e25max[n];
}


//The following function is copied from PhotonCor/McCor
double AbsCor::ECorrMC(double eg, double theid) const
{
  double Energy5x5=eg;
  if(eg<E25min(int(theid))) eg=E25min(int(theid));
  if(eg>E25max(int(theid))) eg=E25max(int(theid))-0.001;

  if(theid<=0)theid=0.001;
  if(theid>=27)theid=26.999;
  Float_t einter = eg + 0.00001;
  Float_t tinter = theid+0.0001;
  //cout<<"inter="<< einter<<"   "<<tinter<<endl;
  double ecor=dt->Interpolate(einter,tinter);
  // cout<<"ecor="<<ecor<<endl;
  if(!(ecor))return Energy5x5;
  if(ecor<0.5)return Energy5x5;
  double EnergyCor=Energy5x5/ecor;
  return EnergyCor;
}

// Get energy error
double AbsCor::ErrMC(double eg, double theid) const
{

  if(eg<E25min(int(theid))) eg=E25min(int(theid));
  if(eg>E25max(int(theid))) eg=E25max(int(theid))-0.001;
  if(theid<=0)theid=0.001;
  if(theid>=27)theid=26.999;
  Float_t einter = eg + 0.00001;
  Float_t tinter = theid+0.0001;
  double err=dtErr->Interpolate(einter,tinter);
  return err;
}









