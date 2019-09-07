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


#include "DSemilepAlg/DSemilepAlg.h"

#include "VertexFit/KinematicFit.h"
#include "VertexFit/Helix.h"
#include "VertexFit/VertexFit.h"
#include "McTruth/McParticle.h"

#include <unistd.h>
#include <fstream>
#include <vector>
typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;

//CONSTANTS
const double me = 0.000511;
const double mkaon = 0.4934;

///////////////////////////

DSemilepAlg::DSemilepAlg(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator){
}

//
// INITIALIZE
//
StatusCode DSemilepAlg::initialize(){

	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "in initialize()" << endmsg;
	StatusCode status;

	//NTUPLE
	NTuplePtr nt0(ntupleSvc(), "FILE1/dsemilep");
	if ( nt0 ) m_tuple0 = nt0;
	else {
		m_tuple0 = ntupleSvc()->book ("FILE1/dsemilep", CLID_ColumnWiseTuple, "track N-Tuple example");
		if ( m_tuple0 )    {
			status = m_tuple0->addItem ("U", m_U);
			status = m_tuple0->addItem ("MM2", m_MM2);
			status = m_tuple0->addItem ("mBC", m_mBC);
			status = m_tuple0->addItem ("q2", m_q2);
			status = m_tuple0->addItem ("deltaE", m_deltaE);
			status = m_tuple0->addItem ("mode", m_mode);
		}
		else    {
			log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple0) << endmsg;
			return StatusCode::FAILURE;
		}
	}

	StatusCode sc = service("SimplePIDSvc", m_simplePIDSvc);
	if ( sc.isFailure())	{
		log << MSG::FATAL << "Could not load SimplePIDSvc!" << endreq;
		return sc;
	}

	log << MSG::INFO << "successfully return from initialize()" <<endmsg;
	return StatusCode::SUCCESS;
}

//
// EXECUTE
//
StatusCode DSemilepAlg::execute() {

	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "in execute()" << endreq;

	SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
	int runNo=eventHeader->runNumber();
	int eventNo=eventHeader->eventNumber();

	//
	//if(eventNo % 1000 == 0)
	//cout << "Event: "<< eventNo << endl;

	SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), "/Event/EvtRec/EvtRecEvent");
	if ( ! evtRecEvent ) {
		log << MSG::FATAL << "Could not find EvtRecEvent" << endreq;
		return StatusCode::FAILURE;
	}

	SmartDataPtr<EvtRecTrackCol> evtRecTrackCol( eventSvc(), "/Event/EvtRec/EvtRecTrackCol");
	if ( ! evtRecTrackCol ) {
		log << MSG::FATAL << "Could not find EvtRecTrackCol" << endreq;
		return StatusCode::FAILURE;
	}

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

	// DTAGTOOL
	DTagTool dtagTool;


	if( dtagTool.isDTagListEmpty() ){
		// cout<<"no D candidates found"<<endl;
		return StatusCode::SUCCESS;
	}
	//else{
	//   cout<<"found D candidates found"<<endl;
	//}

	DTagToolIterator iter_begin = dtagTool.modes_begin();
	DTagToolIterator iter_end = dtagTool.modes_end();

	//find semileptonic decay candidate
	vector<DTagToolIterator> vsditer;

	// Using modes 0,1 and 3 (D0->KPi, D0->KPiPi0, D0->KPiPiPi ) fill dtagtool
	// Combining three modes to look at the signal side of all of them
	if(dtagTool.findSTag( EvtRecDTag::kD0toKPi,1) && dtagTool.cosmicandleptonVeto())
		vsditer.push_back(dtagTool.stag());
	if(dtagTool.findSTag( EvtRecDTag::kD0toKPi,-1) && dtagTool.cosmicandleptonVeto())
		vsditer.push_back(dtagTool.stag());
	if(dtagTool.findSTag( EvtRecDTag::kD0toKPiPi0,1))
		vsditer.push_back(dtagTool.stag());
	if(dtagTool.findSTag( EvtRecDTag::kD0toKPiPi0,-1))
		vsditer.push_back(dtagTool.stag());
	if(dtagTool.findSTag( EvtRecDTag::kD0toKPiPiPi,1) )
		vsditer.push_back(dtagTool.stag());
	if(dtagTool.findSTag( EvtRecDTag::kD0toKPiPiPi,-1) )
		vsditer.push_back(dtagTool.stag());


	typedef vector<DTagToolIterator>::size_type vec_sz;

	//loop over single tag to reconstruct signal side
	for(vec_sz i = 0 ; i < vsditer.size() ; i++){

		//fill single tags only before selection
		m_deltaE = (*vsditer[i])->deltaE();
		m_mode = (*vsditer[i])->decayMode();
		m_mBC = (*vsditer[i])->mBC();

		//Get the DTagToolIterator of the tag for easier usage in the code
		DTagToolIterator sditer = vsditer[i];

		// Check signal side for good tracks and charge of the tracks
		SmartRefVector<EvtRecTrack> othertracks = (*sditer)->otherTracks();
		vector<int> iGood;      int tcharge=0;

		for(int i = 0 ; i < othertracks.size() ; i++){
			if(isGoodTrack(othertracks[i],xorigin)){
				iGood.push_back(i);
				RecMdcKalTrack *mdcKalTrk = othertracks[i]->mdcKalTrack();
				tcharge += mdcKalTrk->charge();
			}
		}

		// Keeping only events with 2 good signal tracks with zero total charge
		if(iGood.size() != 2 || tcharge != 0)
			continue;

		// Use SimplePIDSvc package to identify the tracks
		m_simplePIDSvc->preparePID(othertracks[iGood[0]]);
		bool FtrkElectron = m_simplePIDSvc->iselectron(); 
		bool FtrkKaon 		= m_simplePIDSvc->iskaon();

		m_simplePIDSvc->preparePID(othertracks[iGood[1]]);
		bool StrkElectron = m_simplePIDSvc->iselectron();
		bool StrkKaon 		= m_simplePIDSvc->iskaon();

		//
		// As at the signal side tracks are not listed in a particular order, there are two senarios
		//
		// Senario 1: Track 0 is electron, track 1 is kaon
		if(FtrkElectron && StrkKaon) {

			RecMdcKalTrack* ftrk = othertracks[iGood[0]]->mdcKalTrack();
			RecMdcKalTrack* strk = othertracks[iGood[1]]->mdcKalTrack();

			SmartRefVector<EvtRecTrack> tracks = (*sditer)->tracks();   
			RecMdcKalTrack* tagsidektrk = tracks[0]->mdcKalTrack();

			//Requiring electron having the same charge as the tag side kaon
			if(ftrk->charge() * tagsidektrk->charge() > 0){
				double U_1 = 0;
				double MM2_1 = 0;
				double q2_1 = 0;
	
				//Calculate U and MM2 using a function
				calU(sditer,strk,ftrk,U_1,MM2_1,q2_1);

				m_U = U_1;
				m_MM2 = MM2_1;
				m_q2 = q2_1;

				m_tuple0->write();
			}


		}//end of senario 1 

		//Senario 2: Track 0 is kaon, track 1 is electon
		if(StrkElectron && FtrkKaon){

			RecMdcKalTrack* ftrk = othertracks[iGood[0]]->mdcKalTrack();
			RecMdcKalTrack* strk = othertracks[iGood[1]]->mdcKalTrack();

			SmartRefVector<EvtRecTrack> tracks = (*sditer)->tracks();  

			RecMdcKalTrack* tagsidektrk = tracks[0]->mdcKalTrack();

			//Requiring electron having the same charge as the tag side kaon
			if(strk->charge() * tagsidektrk->charge() > 0){

				double U_1 = 0;
				double MM2_1 = 0;
				double q2_1 = 0;
	
				//Calculate U and MM2 using a function
				calU(sditer,strk,ftrk,U_1,MM2_1,q2_1);

				m_U = U_1;
				m_MM2 = MM2_1;
				m_q2 = q2_1;

				m_tuple0->write();
			}
		}//end of senario 2

	}

	//Clear DTagTool
	dtagTool.clear();
}

//
// FINALIZE
//
StatusCode DSemilepAlg::finalize() {
	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "in finalize()" << endmsg;

	return StatusCode::SUCCESS;
}

//
// MEMBER FUNCTIONS
//

//
// isGoodTrack
bool DSemilepAlg::isGoodTrack(EvtRecTrack* trk, Hep3Vector xorigin){
	if(!(trk->isMdcKalTrackValid())) return false;

	RecMdcKalTrack *mdcKalTrk = trk->mdcKalTrack();
	mdcKalTrk->setPidType(RecMdcKalTrack::pion);
	HepVector    a  = mdcKalTrk->getZHelix();
	HepSymMatrix Ea = mdcKalTrk->getZError();
	HepPoint3D pivot(0.,0.,0.);
	HepPoint3D IP(xorigin[0],xorigin[1],xorigin[2]);
	VFHelix helixp(pivot,a,Ea);
	helixp.pivot(IP);
	HepVector vec    = helixp.a();
	double    vrl    = vec[0];
	double    vzl    = vec[3];
	double costheta  = cos(mdcKalTrk->theta());

	//Event selection criteria
	if(fabs(vrl) < 1 && fabs(vzl) < 10 && fabs(costheta) < 0.93)
		return true;
	return false;
}

//
//3 Momentum of the tagged D
Hep3Vector DSemilepAlg::tagDP3(DTagToolIterator iter_dtag){

	SmartRefVector<EvtRecTrack> tracks=(*iter_dtag)->tracks();
	SmartRefVector<EvtRecTrack> showers=(*iter_dtag)->showers();

	HepLorentzVector p4=(*iter_dtag)->p4();
	p4.boost(-0.011,0,0);
	Hep3Vector p3=p4.v();
	return p3;
}

//
// Calculate U = E_miss - P_miss and MM2 = U * (E_miss + P_miss)
void DSemilepAlg::calU(DTagToolIterator sditer,RecMdcKalTrack* Etrack, RecMdcKalTrack* Ktrack,double& U, double& MM2, double& q2){

	Etrack->setPidType(RecMdcKalTrack::electron);
	Ktrack->setPidType(RecMdcKalTrack::kaon);

	Hep3Vector P3_tag = tagDP3(sditer);

	Hep3Vector P3_E(Etrack->px(), Etrack->py(),Etrack->pz());
	Hep3Vector P3_K(Ktrack->px(), Ktrack->py(),Ktrack->pz());

	HepLorentzVector P4_E(P3_E, sqrt( P3_E.mag2() + me * me));
	HepLorentzVector P4_K(P3_K, sqrt( P3_K.mag2() + mkaon * mkaon));

	// Boost
	P4_E.boost(-0.011,0,0);
	P4_K.boost(-0.011,0,0);

	double e_miss = (*sditer)->beamE() - P4_E.e() - P4_K.e();
	Hep3Vector P3_miss = -P3_tag - P4_E.vect() - P4_K.vect();

	U = e_miss - P3_miss.mag();
	MM2 = U * (e_miss + P3_miss.mag());

	HepLorentzVector P4_W(P3_E+P3_miss*fabs(e_miss/P3_miss.mag()),e_miss+P4_E.e());
  q2 = P4_W.m2();

}
