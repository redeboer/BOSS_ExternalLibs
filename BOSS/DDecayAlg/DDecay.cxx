#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EvtRecEvent/EvtRecDTag.h"
#include "EvtRecEvent/EvtRecVeeVertex.h"
#include "EvtRecEvent/EvtRecPi0.h"
#include "DstEvent/TofHitStatus.h"

#include "TMath.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "CLHEP/Geometry/Point3D.h"

using CLHEP::Hep3Vector;
using CLHEP::Hep2Vector;
using CLHEP::HepLorentzVector;
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

#include "DTagTool/DTagTool.h"
#include "DDecayAlg/DDecay.h"

#include "VertexFit/KinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "ParticleID/ParticleID.h"

#include <vector>
typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;


///////////////////////////////////////////////////////////////

DDecay::DDecay(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator){

}

StatusCode DDecay::initialize(){

  MsgStream log(msgSvc(), name());
  
  log << MSG::INFO << "in initialize()" << endmsg;
  
  StatusCode status;
  NTuplePtr nt1(ntupleSvc(), "FILE1/vxyz");
  if ( nt1 ) m_tuple1 = nt1;
  else {
    m_tuple1 = ntupleSvc()->book ("FILE1/vxyz", CLID_ColumnWiseTuple, "track N-Tuple example");
    if ( m_tuple1 )    {
      status = m_tuple1->addItem ("vx0", m_vx0);
      status = m_tuple1->addItem ("vy0", m_vy0);
      status = m_tuple1->addItem ("vz0", m_vz0);
      status = m_tuple1->addItem ("vr0", m_vr0);
    }
    else    { 
      log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple1) << endmsg;
      return StatusCode::FAILURE;
    }
  }

  NTuplePtr nt2(ntupleSvc(), "FILE1/ks");
  if ( nt2 ) m_tuple2 = nt2;
  else {
    m_tuple2 = ntupleSvc()->book ("FILE1/ks", CLID_ColumnWiseTuple, "ks N-Tuple example");
    if ( m_tuple2 )    {
      status = m_tuple2->addItem ("ksmass", m_ksmass);
      status = m_tuple2->addItem ("ksd",    m_ksd);
      status = m_tuple2->addItem ("ksmode", m_ksmode);
    }
    else    { 
      log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple2) << endmsg;
      return StatusCode::FAILURE;
    }
  }

  NTuplePtr nt3(ntupleSvc(), "FILE1/pi0");
  if ( nt3 ) m_tuple3 = nt3;
  else {
    m_tuple3 = ntupleSvc()->book ("FILE1/pi0", CLID_ColumnWiseTuple, "pi0 N-Tuple example");
    if ( m_tuple3 )    {
      status = m_tuple3->addItem ("pi0mass", m_pi0mass);
      status = m_tuple3->addItem ("pi0mode", m_pi0mode);
    }
    else    { 
      log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple3) << endmsg;
      return StatusCode::FAILURE;
    }
  }

  NTuplePtr nt4(ntupleSvc(), "FILE1/tagD");
  if ( nt4 ) m_tuple4 = nt4;
  else {
    m_tuple4 = ntupleSvc()->book ("FILE1/tagD", CLID_ColumnWiseTuple, "DTag N-Tuple example");
    if ( m_tuple4 )    {
      status = m_tuple4->addItem ("mode",          m_mode);
      status = m_tuple4->addItem ("type",          m_type);
      status = m_tuple4->addItem ("charge",        m_charge);
      status = m_tuple4->addItem ("charm",         m_charm);
      status = m_tuple4->addItem ("numofchildren", m_numofchildren);
      status = m_tuple4->addItem ("mass",          m_mass);
      status = m_tuple4->addItem ("mBC",           m_mBC);
      status = m_tuple4->addItem ("deltaE",        m_deltae);
      status = m_tuple4->addItem ("E",             m_e);
      status = m_tuple4->addItem ("notherTrk",     m_ntrk);
    }
    else    { 
      log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple4) << endmsg;
      return StatusCode::FAILURE;
    }
  }


  //
  //--------end of book--------
  //
  
  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;
  
}

//***********************************************************************
StatusCode DDecay::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  int runNo=eventHeader->runNumber();
  int eventNo=eventHeader->eventNumber();
  
  //cout<<"**************************************"<<endl;
  //cout<<"event "<<eventNo<<endl;
  //cout<<"**************************************"<<endl;

  /// Accessing Ks list
  SmartDataPtr<EvtRecVeeVertexCol> evtRecVeeVertexCol(eventSvc(), "/Event/EvtRec/EvtRecVeeVertexCol");
  if ( ! evtRecVeeVertexCol ) {
    log << MSG::FATAL << "Could not find EvtRecVeeVertexCol" << endreq;
    return StatusCode::FAILURE;
  }

  /// Accessing pi0 list
  SmartDataPtr<EvtRecPi0Col> recPi0Col(eventSvc(), "/Event/EvtRec/EvtRecPi0Col");
  if ( ! recPi0Col ) {
    log << MSG::FATAL << "Could not find EvtRecPi0Col" << endreq;
    return StatusCode::FAILURE;
  }


  //get primary vertex from db
  Hep3Vector xorigin(0,0,0);
  IVertexDbSvc*  vtxsvc;
  Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
  if (vtxsvc->isVertexValid()) {
  
    //vertex[0] = vx; vertex[1]= vy; vertex[2] = vz;
    double* vertex = vtxsvc->PrimaryVertex();
    xorigin.setX(vertex[0]);
    xorigin.setY(vertex[1]);
    xorigin.setZ(vertex[2]);
  }


  //****************************
  
  //use util tool to get dtaglist

  DTagTool dtagTool;
  if( dtagTool.isDTagListEmpty() ){
    //cout<<"no D candidates found"<<endl;
    return StatusCode::SUCCESS;
  }
  
  DTagToolIterator iter_begin = dtagTool.modes_begin();
  DTagToolIterator iter_end =dtagTool.modes_end();
  
  cout<<"size of dtag:******:"<<iter_end-iter_begin<<endl;
  
  int nCharge = 0;


  //test to get one mode only
  cout<<"test single mode search :"<< EvtRecDTag::kD0toKPi<<endl;  
  
  //access all canddidates of one mode
  vector<int> mode = dtagTool.mode( EvtRecDTag::kD0toKPi );
  cout<<" there are "<< mode.size() <<" candidates for this  mode" <<endl;
  for( int i=0; i < mode.size(); i++){
    
    DTagToolIterator iter= dtagTool.modes_begin()+ mode[i];
    cout<<"No."<<i+1<<" candidate deltaE is : "<< (*iter)->deltaE()<<endl; 
    
  }

  
  //loop the whole DTag list with order when they are filled , do all kinds of fit in side the loop
  
  for (DTagToolIterator iter_dtag=iter_begin; iter_dtag != iter_end; iter_dtag++){
    
    //decay mode name
    cout<<"***********"<<endl;
    cout<<"***********"<<endl;
    dtagTool<< iter_dtag;
    
    
    //****************************************************
    //mode D0 to KPi only
    //****************************************************
    
    if((*iter_dtag)->decayMode()==EvtRecDTag::kD0toKPi) {
      
      HepLorentzVector p4=(*iter_dtag)->p4();
      p4.boost(-0.011,0,0);
      
      Hep3Vector p3=p4.v();
      
      m_mode=(*iter_dtag)->decayMode();
      m_type=(*iter_dtag)->type();
      m_charge=(*iter_dtag)->charge();
      m_charm=(*iter_dtag)->charm();
      m_numofchildren=(*iter_dtag)->numOfChildren();
      m_mass=(*iter_dtag)->mass();
      m_mBC=(*iter_dtag)->mBC();
      m_e=(*iter_dtag)->beamE();
      m_deltae=(*iter_dtag)->deltaE();

      SmartRefVector<EvtRecTrack> tracks=(*iter_dtag)->tracks();
      SmartRefVector<EvtRecTrack> othertracks=(*iter_dtag)->otherTracks();
      SmartRefVector<EvtRecTrack> othershowers=(*iter_dtag)->otherShowers();
      m_ntrk=othertracks.size();

      m_tuple4->write();

 
      RecMdcKalTrack *mdcKalTrk1 = tracks[0]->mdcKalTrack();
      RecMdcKalTrack *mdcKalTrk2 = tracks[1]->mdcKalTrack();
      cout<<"same side track 1 charge is:"<<mdcKalTrk1->charge()<<endl;
      cout<<"same side track 2 charge is:"<<mdcKalTrk2->charge()<<endl;

      for(int tk=0; tk<othertracks.size(); tk++){
	RecMdcTrack *mdcTrk = othertracks[tk]->mdcTrack();
	double pch=mdcTrk->p();
	double x0=mdcTrk->x();
	double y0=mdcTrk->y();
	double z0=mdcTrk->z();
	double phi0=mdcTrk->helix(1);
	double xp=xorigin.x();
	double yp=xorigin.y();
	double Rxy=(x0-xp)*cos(phi0)+(y0-yp)*sin(phi0);
	
	m_vx0=x0;
	m_vy0=y0;
	m_vz0=z0;
	m_vr0=Rxy;
	m_tuple1->write();
	nCharge += mdcTrk->charge();
	
	std::cout<<"other side track ID is: "<<othertracks[tk]->trackId()<<std::endl;
	
	if(dtagTool.isPion(othertracks[tk]) )
	  cout<<"it is pion"<<endl;
	if(dtagTool.isKaon(othertracks[tk]) )
	  cout<<"it is kaon"<<endl;
	
	//std::cout<<"momentum="<<pch<<", x0="<<x0<<std::endl;
	
      } //end of looping charged othertracks
      
      for(int i=0; i<othershowers.size(); i++){
	//RecEmcShower *shr = othershowers[i]->emcShower();
	//std::cout<<"shower ID is: "<<othershowers[i]->trackId()<<std::endl;
      }

    } //end of D0 to K Pi mode



    //****************************************************
    //mode D0 to KPiPi0 only
    //****************************************************
    if((*iter_dtag)->decayMode()==EvtRecDTag::kD0toKPiPi0) {
      
      m_mode=(*iter_dtag)->decayMode();
      m_type=(*iter_dtag)->type();
      m_charge=(*iter_dtag)->charge();
      m_charm=(*iter_dtag)->charm();
      m_numofchildren=(*iter_dtag)->numOfChildren();
      m_mass=(*iter_dtag)->mass();
      m_mBC=(*iter_dtag)->mBC();
      m_e=(*iter_dtag)->beamE();
      m_deltae=(*iter_dtag)->deltaE();

      SmartRefVector<EvtRecTrack> othertracks=(*iter_dtag)->otherTracks();
      m_ntrk=othertracks.size();
 
      m_tuple4->write();


      /// access pi0 using dtagtool
      // users have to use second argument to tell pi0Id function the number of pi0s from the mode
      // default number is 1 
      vector<int> pi0id= dtagTool.pi0Id(iter_dtag);
      cout<<"xxxxxxxxxxxxxxxxxxxxxxxxx"<<"num of pi0 is:"<<pi0id.size()<<endl;
     
      for(int i=0; i<pi0id.size(); i++){
	pi0Iterator pi0Itr= dtagTool.pi0_begin()+pi0id[i];
	cout<<"pi0Mass: " << (*pi0Itr)->unconMass() << endl;
	
      }
      
      /// or accessing pi0 information this way
      SmartRefVector<EvtRecTrack> showers=(*iter_dtag)->showers();

      for(EvtRecPi0Col::iterator pi0Itr = recPi0Col->begin(); 
	  pi0Itr < recPi0Col->end(); pi0Itr++){

	/// Access pi0 children
	EvtRecTrack* heGammaTrk = const_cast<EvtRecTrack*>((*pi0Itr)->hiEnGamma());
	EvtRecTrack* leGammaTrk = const_cast<EvtRecTrack*>((*pi0Itr)->loEnGamma());

	int heGammaTrkId = heGammaTrk->trackId();
	int leGammaTrkId = leGammaTrk->trackId();

	if((heGammaTrkId != showers[0]->trackId())&&
	   (heGammaTrkId != showers[1]->trackId())) continue;
	if((leGammaTrkId != showers[0]->trackId())&&
	   (leGammaTrkId != showers[1]->trackId())) continue;

	const RecEmcShower* heGamma = heGammaTrk->emcShower();
	const RecEmcShower* leGamma = leGammaTrk->emcShower();

	cout<<"pi0Mass: " << (*pi0Itr)->unconMass() << endl;
	cout<<" E(high): " << heGamma->energy() << endl;
	cout<<" E(low) : " << leGamma->energy() << endl;

	m_pi0mass = (*pi0Itr)->unconMass();
	m_pi0mode = (*iter_dtag)->decayMode();
	m_tuple3->write();

	/// ONLY Break out of loop if tag mode has one pi0 child
	break;

      }  // End "pi0Itr" FOR Loop
     

    }// end of D0 to KPIPI0

      
    //****************************************************
    //mode D0 to KsPiPi only
    //****************************************************
    if((*iter_dtag)->decayMode()==EvtRecDTag::kD0toKsPiPi) {
  
      m_mode=(*iter_dtag)->decayMode();
      m_type=(*iter_dtag)->type();
      m_charge=(*iter_dtag)->charge();
      m_charm=(*iter_dtag)->charm();
      m_numofchildren=(*iter_dtag)->numOfChildren();
      m_mass=(*iter_dtag)->mass();
      m_mBC=(*iter_dtag)->mBC();
      m_e=(*iter_dtag)->beamE();
      m_deltae=(*iter_dtag)->deltaE();

      SmartRefVector<EvtRecTrack> othertracks=(*iter_dtag)->otherTracks();
      m_ntrk=othertracks.size();
 
      m_tuple4->write();

      /// accessing Ks using dtagtool
      // users have to use second argument to tell KsId function the number of Ks from the mode
      // default number is 1 
      vector<int> ksid= dtagTool.ksId(iter_dtag);
      cout<<"xxxxxxxxxxxxxxxxxxxxxxxxx"<<"num of ks is:"<<ksid.size()<<endl;
      
      for(int i=0; i<ksid.size(); i++){
	ksIterator ksItr= dtagTool.ks_begin()+ksid[i];
	cout<<"ksMass: " << (*ksItr)->mass() << endl;
	
      }


      /// Ks information
      SmartRefVector<EvtRecTrack> tracks=(*iter_dtag)->tracks();

      for(EvtRecVeeVertexCol::iterator ksItr = evtRecVeeVertexCol->begin(); 
	  ksItr < evtRecVeeVertexCol->end(); ksItr++){
	
	/// Needed to reject Lambda (and conversion?) combinations
	if((*ksItr)->vertexId() != 310) continue; 

	EvtRecTrack* aKsChild1Trk = (*ksItr)->daughter(0);
	EvtRecTrack* aKsChild2Trk = (*ksItr)->daughter(1);

	int ksChild1TrkId = aKsChild1Trk->trackId();
	int ksChild2TrkId = aKsChild2Trk->trackId();

	if((ksChild1TrkId != tracks[0]->trackId())&&
	   (ksChild1TrkId != tracks[1]->trackId())) continue;
	if((ksChild2TrkId != tracks[0]->trackId())&&
	   (ksChild2TrkId != tracks[1]->trackId())) continue;
	
	cout<<"ksMass: " << (*ksItr)->mass() << endl;

	Hep3Vector ks_D3(0,0,0);
	ks_D3.set((*ksItr)->w()[4],(*ksItr)->w()[5],(*ksItr)->w()[6]);

	m_ksmass = (*ksItr)->mass();
	m_ksd    = ks_D3.mag();
	m_ksmode = (*iter_dtag)->decayMode();
	m_tuple2->write();

	/// ONLY Break out of loop if tag mode has one Ks child
	break;

      }  // End "ksItr" FOR Loop

    }// end of D0 to KSpipi


    //****************************************************
    //mode D+ to K- pi+ pi- only
    //****************************************************
    if((*iter_dtag)->decayMode()==EvtRecDTag::kDptoKPiPi) {
      
      m_mode=(*iter_dtag)->decayMode();
      m_type=(*iter_dtag)->type();
      m_charge=(*iter_dtag)->charge();
      m_charm=(*iter_dtag)->charm();
      m_numofchildren=(*iter_dtag)->numOfChildren();
      m_mass=(*iter_dtag)->mass();
      m_mBC=(*iter_dtag)->mBC();
      m_e=(*iter_dtag)->beamE();
      m_deltae=(*iter_dtag)->deltaE();

      SmartRefVector<EvtRecTrack> othertracks=(*iter_dtag)->otherTracks();
      m_ntrk=othertracks.size();
 
      m_tuple4->write();
    }// end of Dp to K pi pi

     //****************************************************
    //mode Ds to Ks pi+ only
    //****************************************************
    if((*iter_dtag)->decayMode()==EvtRecDTag::kDstoKsPi) {
                  
      m_mode=(*iter_dtag)->decayMode();
      m_type=(*iter_dtag)->type();
      m_charge=(*iter_dtag)->charge();
      m_charm=(*iter_dtag)->charm();
      m_numofchildren=(*iter_dtag)->numOfChildren();
      m_mass=(*iter_dtag)->mass();
      m_mBC=(*iter_dtag)->mBC();
      m_e=(*iter_dtag)->beamE();
      m_deltae=(*iter_dtag)->deltaE();

      SmartRefVector<EvtRecTrack> othertracks=(*iter_dtag)->otherTracks();
      m_ntrk=othertracks.size();
 
      m_tuple4->write();
    }// end of Ds to Ks pi


  } //end of looping over the entire DTag list
  

  //************************
  cout<<"**************"<<endl;
  cout<<"**************"<<endl;
  cout<<"test print only D0/Dp/Ds modes"<<endl;
  
  //looop only D0 modes
  vector<int> d0itindex= dtagTool.D0modes();
  for( int i=0; i< d0itindex.size(); i++){
    DTagToolIterator iter= dtagTool.modes_begin()+d0itindex[i];
    cout<<"No."<<i+1<<" D0  mode is :"<< (*iter)->decayMode()<<endl;
  } 
    
  //loop only Dp modes
  vector<int> dpitindex= dtagTool.Dpmodes();
  for( int i=0; i< dpitindex.size(); i++){
    DTagToolIterator iter= dtagTool.modes_begin()+dpitindex[i];
    cout<<"No."<<i+1<<" Dp  mode is :"<< (*iter)->decayMode()<<endl;
  } 
  
  //loop only Ds modes
  vector<int> dsitindex= dtagTool.Dsmodes();
  for( int i=0; i< dsitindex.size(); i++){
    DTagToolIterator iter= dtagTool.modes_begin()+dsitindex[i];
    cout<<"No."<<i+1<<" Ds  mode is :"<< (*iter)->decayMode()<<endl;
  } 
  

  //check single tag
  //************************
  cout<<"**************"<<endl;
  cout<<"**************"<<endl;
  cout<<"test single tag "<<endl;

  if( dtagTool.findSTag( EvtRecDTag::kD0toKPi , +1 ) )
    cout<<" find single tag mode: "<< (*dtagTool.stag())->decayMode() <<endl;

  //cosmic and lepton veto for 2 daughter decay modes, like D0->K pi
  if( dtagTool.findSTag( EvtRecDTag::kD0toKPi , +1 ) && dtagTool.cosmicandleptonVeto() ){
    cout<<"cosmic and lepton backgaround veto"<<endl;
    cout<<" find single tag mode: "<< (*dtagTool.stag())->decayMode() <<endl;
  }

  //check double tag
  //************************
  cout<<"**************"<<endl;
  cout<<"**************"<<endl;
  cout<<"test double tag "<<endl;

  //charm calculated automatically, and charge conjucation considered
  //nameley, will consider D0->K- pi+      & D0_bar->K+ pi- pi0 pair
  //          or           D0_bar->K+ pi-  & D0->K- pi+ pi0     pair
  bool dtagflag=dtagTool.findDTag( EvtRecDTag::kD0toKPi , EvtRecDTag::kD0toKPiPi0);
  if( dtagflag){
    cout<<" find double tag mode 1 :"<<endl;
    dtagTool<<dtagTool.dtag1();
    cout<<" find double tag mode 2:  "<< endl;
    dtagTool<<dtagTool.dtag2();
  }
  //no charge conjucation considered
  //nameley, will only consider  D0->K- pi+      & D0_bar->K+ pi- pi0 pair
  dtagflag=dtagTool.findDTag( EvtRecDTag::kD0toKPi , +1, EvtRecDTag::kD0toKPiPi0, -1 );
  if( dtagflag){
    cout<<" find double tag mode 1 :"<<endl;
    dtagTool<<dtagTool.dtag1();
    cout<<" find double tag mode 2:  "<< endl;
    dtagTool<<dtagTool.dtag2();
  }


  //use invariant mass to select best double tag
  dtagflag=dtagTool.findDTag( EvtRecDTag::kD0toKPi , +1, EvtRecDTag::kD0toKPiPi0, -1 ,"inv");
  if( dtagflag){
    cout<<" double tag by invariant mass:"<<endl;
    cout<<" find double tag mode 1 :"<<endl;
    dtagTool<<dtagTool.dtag1();
    cout<<" find double tag mode 2:  "<< endl;
    dtagTool<<dtagTool.dtag2();
  }
  
  //return all valid double tag lists
  dtagflag=dtagTool.findADTag( EvtRecDTag::kD0toKPi , +1, EvtRecDTag::kD0toKPiPi0, -1);
  if( dtagflag){

    vector<DTagToolIterator> vdtag1=dtagTool.vdtag1();
    vector<DTagToolIterator> vdtag2=dtagTool.vdtag2();
    
    cout<<" list of all doule tags:"<<endl;
    for(int i=0;i<vdtag1.size();i++){
      cout<<" find double tag mode 1 :"<<endl;
      dtagTool<<vdtag1[i];
      cout<<" find double tag mode 2:  "<< endl;
      dtagTool<<vdtag2[i];
    }
    
  }
  
  dtagTool.clear();
  
} //end of execute()



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DDecay::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}

