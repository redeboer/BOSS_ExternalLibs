//*****************************************************************************
//
// Mcgpj.cxx
//
// General 2-body generator for e^+e^-, mu^+mu^-, pi^+pi^-, tau^+tau^-, 
// K_S K_L, K^+K^-, gamma gamma with precision better 0.2 %
//
// Mar 2009 Original BES3 code by Alexei Sibidanov
//
//*****************************************************************************

// Header for this module:-
#include "Mcgpj/Mcgpj.h"

// Framework Related Headers:-
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "BesKernel/IBesRndmGenSvc.h"

#include "GaudiKernel/SmartDataPtr.h"

#include "GeneratorObject/McGenEvent.h"

#include "TRandom3.h"
#include "TRadCor.h"
#include "TEPCrossPart.h"
#include "TMuCrossPart.h"
#include "TPiCrossPart.h"
#include "TKnCrossPart.h"
#include "TKcCrossPart.h"
#include "TGGCrossPart.h"
#include "TRadGlobal.h"
#include "TKinemCut.h"

#include "TPhoton_o.h"
#include "TDFun_o.h"
#include "T5piCrossPart.h"
#include "T4piCrossPart.h"
#include "T3piCrossPart.h"

using namespace CLHEP;

TRadCor *gRad;
TRadGlobal *gGlobal;
TKinemCut *gCut;
TConstants *gConst;
TVCrossPart *MatrixElements;
TCrossPart *CS;
//-------------------------

Mcgpj::Mcgpj(const std::string& name, ISvcLocator* pSvcLocator):Algorithm(name, pSvcLocator){
  declareProperty("Data", m_datapath = "${MCGPJROOT}/data"); // Path to data files
  declareProperty("VpolFname", m_vpolfname = "vpol.dat"); // Vacuum polarization data file name
  declareProperty("CMEnergy", cmE = 3.097); // 2*Ebeam [GeV]
  declareProperty("Process", proc = 0); // process
  declareProperty("NRad", NRad = 20000);
  declareProperty("HardPhoton", IsHardPhoton = 1);
  declareProperty("NoVacPol", IsNoVacPol = 0);
  declareProperty("FSR", IsFSR = 1);
  declareProperty("AcolAngle", pc  = 0.);
  declareProperty("DeltaE", de  = -1.); // [GeV]
  declareProperty("NTheta0", nt0 = -1.); // [1/sqrt(gamma)]
  declareProperty("DeltaTheta", dt  = 0.5); //[rad]
  declareProperty("DeltaPhi", dp  = 0.3); // [rad]
  declareProperty("AverageTheta", at  = 1.1); //[rad]
  declareProperty("ThetaDetector", td  = 1.1 - 0.5/2); //[rad]
  declareProperty("AverageMomentum", am  = 0.090); //[GeV/c]
  declareProperty("CrossMomentum", cm  = 0.090); //[GeV/c]
  declareProperty("MinimumEnergy", em  = 0.050);//[GeV]
  declareProperty("ThetaIntermediate", ti  = 0.473);// [rad]
  declareProperty("AlphaScale", al  = 1.); 
  declareProperty("ThetaMinus", thm = -1.);  //[rad]
  declareProperty("ThetaPlus", thp = -1.);  //[rad]
  declareProperty("AbsoluteError", te  = 0.05); // absolute error in [nb]
  declareProperty("RelativeError", re  = 0.05); // realative error
//  declareProperty("InitialSeed", Seed = 0);  
  declareProperty("BeamSpread", spread  = -1);// [MeV]
//  declareProperty("PhaseShift", phase  = -90);// [deg]
  declareProperty("Mode5pi", m_fmode5pi  = 0);// 0 - via b1+/- pi-/+, 1 via b10 pi0
}

StatusCode Mcgpj::initialize(){
  MsgStream log(messageService(), name());
  
  log << MSG::INFO << "Mcgpj initialize" << endreq;

  static const bool CREATEIFNOTTHERE(true);                                                            
  StatusCode RndmStatus = service("BesRndmGenSvc", p_BesRndmGenSvc, CREATEIFNOTTHERE);                 
  if (!RndmStatus.isSuccess() || 0 == p_BesRndmGenSvc)                                                 
  {                                                                                                    
    log << MSG::ERROR << " Could not initialize Random Number Service" << endreq;                      
    return RndmStatus;                                                                                 
  }                                                                                                    
  CLHEP::HepRandomEngine* engine  = p_BesRndmGenSvc->GetEngine("Mcgpj");                            
  engine->showStatus();

  // GeV -> MeV
  cmE *= 1e3;
  de *= 1e3;
  am *= 1e3;
  cm *= 1e3;
  em *= 1e3;

  if(gRandom) delete gRandom;
  gRandom = new TRandom3();
  gRandom->SetSeed(engine->getSeed());
  //gRandom->Dump();

  if (proc<100){

    gConst  = new TConstants();
    gGlobal = new TRadGlobal();
    gCut    = new TKinemCut();

    if((proc%10)==3){
      td = 0.025;
      dt = 10;
      dp = 10;
      cm = 0;
      am = 0;
    }
  
    const int MaxList = 20;
    bool InList[MaxList];
    for(int j = 0; j<MaxList; j++) InList[j] = true;

    double EBeam = 0.5*cmE;

    if( ( EBeam < 100 || EBeam > 2500 ) && !IsNoVacPol ){
      log<<MSG::ERROR<<"Invalid value of beam energy:"<<EBeam<<endreq;
      return StatusCode::FAILURE;
    }

    gGlobal->Set_TotalError(te);
  
    gGlobal->Set_RelativeError(re);

    gGlobal->Set_Type((proc%10));

    gGlobal->Set_E(EBeam);
    
    gGlobal->Set_ThetaInt(ti);

    if(0>td)
      gGlobal->Set_ThetaMin(at-dt/2);
    else
      gGlobal->Set_ThetaMin(td);

    if(0>de){
      if((proc%10)==0) 
	gGlobal->Set_dE_Abs(0.015*EBeam);
      else 
	gGlobal->Set_dE_Abs(0.0003*EBeam);
    }
    else
      gGlobal->Set_dE_Abs(de);

    if(0>nt0){
      if(proc>10) 
	gGlobal->Set_Theta0_Rel(0.0);
      else
	gGlobal->Set_Theta0_Rel(1.5);
    }else
      gGlobal->Set_Theta0_Rel(nt0);

    if(0<thm){
      gCut->ThetaMinM(thm);
      gGlobal->Set_ThetaMin(thm);
    }

    if(0<thp)
      gCut->ThetaMinP(thp);

    gCut->CosPsi(cos(pc));

    gCut->DeltaTheta(dt);
  
    gCut->AverageTheta(at);

    gCut->DeltaPhi(dp);
  
    gCut->PAverage(am);
    
    gCut->PCross(cm);

    gCut->EMin(em);
  
    gConst->SetAlphaScale(al);

    gConst->Print();

    try{
      gGlobal->Init();
    }catch(std::logic_error lge){
      log<<MSG::ERROR<<lge.what()<<endreq;
      return StatusCode::FAILURE;
    }

    gGlobal->SetDatadir(m_datapath);
    gGlobal->SetVpolFname(m_vpolfname);
//    gGlobal->SetIntFname("");

    gGlobal->Print();

    gCut->Init();
    gCut->Print();

    log<<MSG::INFO<<"Cross-section statistical precision will be better than "
       <<gGlobal->Get_TotalError()<<" nb and "
       <<gGlobal->Get_RelativeError()*100<<"%"<<endreq;

    if(!IsHardPhoton)
      log<<MSG::INFO<<"Hard photon on big angle is not included!"<<endreq;
    
    if(IsNoVacPol)
      log<<MSG::INFO<<"Vacuum polarization is not included!"<<endreq;

    if(!IsFSR)
      log<<MSG::INFO<<"Final state radiation is not included!"<<endreq;

    if(proc>10)
      log<<MSG::INFO<<"Alpha order generation only!"<<endreq;

    log<<std::flush;

    if(proc==0){
      MatrixElements = new TEPCrossPart();
      InList[18] = false;
    }
    else if(proc==1 || proc==5)
      MatrixElements = new TMuCrossPart();
    else if(proc==2)
      MatrixElements = new TPiCrossPart();
    else if(proc==3)
      MatrixElements = new TKnCrossPart();
    else if(proc==4)
      MatrixElements = new TKcCrossPart();
    else if(proc==6)
      MatrixElements = new TGGCrossPart();
    else if(proc==10){
      MatrixElements = new TEPCrossPart();
      for(int j = 0; j<MaxList; j++) InList[j] = false;
      InList[16] = true;
      InList[17] = true;
      InList[18] = true;
    }
    else
      return StatusCode::FAILURE;

    if(IsNoVacPol)
      MatrixElements->SetZeroVP();

    if(!IsFSR)
      MatrixElements->SetNoFSR();

    gRad = new TRadCor(MatrixElements);
    gRad->SetNEvents(NRad);
    gRad->SetPartList(InList);
    gRad->Init();

    MatrixElements->SetHardPhoton(IsHardPhoton);
    gRad->MakeCrossSection();
    if((proc%10)==2)((TPiCrossPart*)MatrixElements)->GetFormFactor()->Print();

    if((proc%10)==0){
      fpid[0] = 11; fpid[1] = -11; fM = 0.51099891;
    }
    if((proc%10)==1){
      fpid[0] = 13; fpid[1] = -13;  fM = 105.658367;
    }
    if((proc%10)==2){
      fpid[0] = 211; fpid[1] = -211; fM = 139.57018;
    }
    if((proc%10)==3){
      fpid[0] = 130; fpid[1] = 310; fM = 497.614;
    }
    if((proc%10)==4){
      fpid[0] = 321; fpid[1] = -321; fM = 493.677;
    }
    if((proc%10)==5){
      fpid[0] = 15; fpid[1] = -15;  fM = 1776.84;
    }
    if((proc%10)==6){
      fpid[0] = 22; fpid[1] = 22; fM = 0;
    }
  }else{
    double EBeam = 0.5*cmE;
    if(0>de) de = 0.005*EBeam;
    
    if(0>nt0) nt0 = 1;

    if(proc==100)
    {
      CS = new T3piCrossPart(EBeam,de,nt0);
      if(spread>0) CS->SetBeamSpread(spread);
    } 
    else if( proc==101)
    { 
      CS = new T4piCrossPart(EBeam,de,nt0);
      if(spread>0) CS->SetBeamSpread(spread);   
    }
    else if( proc==102)
    {
      CS = new T5piCrossPart(EBeam,de,nt0,m_fmode5pi);
      if(spread>0) CS->SetBeamSpread(spread);
    }
    CS->MakeParts(te);
  }
  
  log << MSG::INFO << "Mcgpj initialize finished" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode Mcgpj::execute(){ 
  MsgStream log(messageService(), name());
  log << MSG::INFO << "Mcgpj executing" << endreq;
	log<<MSG::WARNING<<"execute start"<<endreq;

  // Fill event information
  GenEvent* evt = new GenEvent(1,1);

  GenVertex* prod_vtx = new GenVertex();

  evt->add_vertex( prod_vtx );

  // incoming beam e+
  GenParticle* p = 
    new GenParticle(HepLorentzVector(0,0,0.5*cmE*1e-3,0.5*cmE*1e-3),-11, 3);
  p->suggest_barcode(1);
  prod_vtx->add_particle_in(p);

  // incoming beam e-
  p = 
    new GenParticle(HepLorentzVector(0,0,0.5*cmE*1e-3, 0.5*cmE*1e-3), 11, 3);
  p->suggest_barcode(2);
  prod_vtx->add_particle_in(p);
  
  int npart = 2;
  if(proc<100){
    double mom[4*6];
    int np;
    gRad->MakeEvent(mom,np);
    
    // final particle 1
    for(int i=0; i<np;i++){
      double ptot = mom[i*4+3];
      double px = ptot*mom[i*4+0];
      double py = ptot*mom[i*4+1];
      double pz = ptot*mom[i*4+2];
      double mass = 0;	  
      int pid = 22;
      if(i<2){
		pid = fpid[i];
        mass = fM;
      }
      double etot = sqrt(ptot*ptot + mass*mass*1e-6);
      p = new GenParticle( HepLorentzVector(px,py,pz,etot), pid, 1); 
      p->suggest_barcode(i+3);
      prod_vtx->add_particle_out(p);
      npart++;
    }
  } else {
    int ipart = CS->GenUnWeightedEvent();
    size_t nmax = CS->GetNfinal() + ((ipart==0)?1:2);
    for(size_t i=0;i<nmax;i++){
      TLorentzVector &q = *(CS->GetParticles()[i]);
      int pid = CS->GetPid(i);
      p = 
	new GenParticle( HepLorentzVector(q.X()*1e-3,q.Y()*1e-3,q.Z()*1e-3,q.T()*1e-3), pid, 1);
      p->suggest_barcode(i+3);
      prod_vtx->add_particle_out(p);
      npart++;
    }
  }

  if( log.level() < MSG::INFO ){
    evt->print();  
  }

  // Check if the McCollection already exists
  SmartDataPtr<McGenEventCol> anMcCol(eventSvc(), "/Event/Gen");
  if (anMcCol!=0){
    // Add event to existing collection
	log<<MSG::WARNING<<"add event"<<endreq;
    MsgStream log(messageService(), name());
    log << MSG::INFO << "Add McGenEvent to existing collection" << endreq;
    McGenEvent* mcEvent = new McGenEvent(evt);
    anMcCol->push_back(mcEvent);
  } else {
    // Create Collection and add  to the transient store
	log<<MSG::WARNING<<"create collection"<<endreq;
    McGenEventCol *mcColl = new McGenEventCol;
    McGenEvent* mcEvent = new McGenEvent(evt);
    mcColl->push_back(mcEvent);
    StatusCode sc = eventSvc()->registerObject("/Event/Gen",mcColl);
    if (sc != StatusCode::SUCCESS) {
      log << MSG::ERROR << "Could not register McGenEvent" << endreq;
      delete mcColl;
      delete evt;
      delete mcEvent;
      return StatusCode::FAILURE;
    } else {
      log << MSG::INFO << "McGenEventCol created and " << npart 
	  <<" particles stored in McGenEvent" << endreq;
    }
  }

	log<<MSG::WARNING<<"execute end"<<endreq;
  return StatusCode::SUCCESS;
}

StatusCode Mcgpj::finalize(){
  MsgStream log(messageService(), name());

  delete gRad;
  delete MatrixElements;
  delete gRandom;
  delete gConst;
  delete gGlobal;
  delete gCut;

  log << MSG::INFO << "Mcgpj finalized" << endreq;

  return StatusCode::SUCCESS;
}
