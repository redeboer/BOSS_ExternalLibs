#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"

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
#include "McCor/McCor.h"

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
TGraph2DErrors *dt = new TGraph2DErrors();

McCor::McCor(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {

    //Declare the properties  
  ntOut = true;
  declareProperty("NTupleOut",ntOut);

/*
  declareProperty("b0",m_b[0] = 0.9976);
  declareProperty("b1",m_b[1] = -0.01718);
  declareProperty("b2",m_b[2] = 0.01634);
*/

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode McCor::initialize(){
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
      }
      else    { 
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple1) << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }
/*
  string CoeffPath=getenv("MCCORROOT");
  CoeffPath +="/share/McCorCoeff.txt";
  ifstream in;
  in.open(CoeffPath.c_str(),ios::in);
  for(int i=0;i<4;i++){
    in>>m_a[i];
    in>>m_ae[i];
  }
*/
  double energy,thetaid,peak,peakerr,res,reserr;
  string DataPath;
  DataPath=getenv("MCCORROOT");
  DataPath += "/share/evset.txt";
  ifstream in1;
  in1.open(DataPath.c_str(),ios::in);
//  in.open("$MCCORROOT/share/evsc.txt");
  double ep[18]={0.03,0.04,0.05,0.075,0.1,0.125,0.15,0.2,0.25,0.3,0.4,0.5,0.75,1.0,1.25,1.5,1.75,2.0};
  for(int i=0;i<504;i++){
    in1>>energy;
    in1>>thetaid;
    in1>>peak;
    in1>>peakerr;
    in1>>res;
    in1>>reserr;
    int j = i/28;
    dt->SetPoint(i,energy,thetaid,peak);
    dt->SetPointError(i,0,0,peakerr);
  }
  in1.close();
  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode McCor::execute() {
  

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  SmartDataPtr<Event::EventH> eventHeader(eventSvc(),"/Event");
  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  //  log << MSG::INFO << "get event tag OK" << endreq;
    log << MSG::DEBUG <<"ncharg, nneu, tottks = " 
      << evtRecEvent->totalCharged() << " , "
      << evtRecEvent->totalNeutral() << " , "
      << evtRecEvent->totalTracks() <<endreq;
      
  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),  EventModel::EvtRec::EvtRecTrackCol);


  for(int i = 0; i< evtRecEvent->totalTracks(); i++) {

    EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isEmcShowerValid()) continue;
    RecEmcShower *emcTrk = (*itTrk)->emcShower();

    int intid = emcTrk->cellId();
    Identifier id=Identifier(intid);
    int getthetaid = EmcID::theta_module(id);
    int getmodule = EmcID::barrel_ec(id);
    if(getthetaid>21)getthetaid=43-getthetaid;
    if(getmodule==1)getthetaid=getthetaid+6;
    double energyF = emcTrk->energy();
    double e5x5 = emcTrk->e5x5();
    double costheta = cos(emcTrk->theta());
    double dthetaid=double(getthetaid);
    double energyC = McCor::corEnergyMc(e5x5,dthetaid);

    if(ntOut == true){ 
      m_ct = costheta;
      m_ef = energyF;
      m_e5 = e5x5;
      m_ec = energyC;
      m_tuple1->write();
    }
    emcTrk->setEnergy(energyC);

   }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode McCor::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}


double McCor::corEnergyMc(double eg,double theid){

   double Energy5x5=eg;
   if(eg<0.029)eg=0.029;
   if(eg>1.76)eg=1.76;
   if(theid<=0)theid=0.001;
   if(theid>=27)theid=26.999;
   Float_t einter = eg + 0.00001;
   Float_t tinter = theid+0.0001;
   double ecor=dt->Interpolate(einter,tinter);
   if(!(ecor))return Energy5x5;
   if(ecor<0.5)return Energy5x5;
   double EnergyCor=Energy5x5/ecor;
   return EnergyCor;
}













