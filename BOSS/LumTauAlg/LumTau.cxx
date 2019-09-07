#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "VertexFit/IVertexDbSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "McTruth/McParticle.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "DstEvent/TofHitStatus.h"
#include "EvTimeEvent/RecEsTime.h"
#include "EventModel/EventHeader.h"
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
#include "VertexFit/KinematicFit.h"
#include "VertexFit/KalmanKinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "VertexFit/Helix.h"
#include "VertexFit/SecondVertexFit.h"
#include "ParticleID/ParticleID.h"
#include "LumTauAlg/LumTau.h"
#include <vector>
typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;
const double ecms = 3.097;
const double velc = 299.792458;
const double xmass[5] = 
{
	0.000511, 0.105658, 0.139570, 0.493677, 0.938272
};
const double pai=3.1415926;
////////////////////////////////////////////////////////////////////
LumTau::LumTau(const std::string& name, ISvcLocator* pSvcLocator) :
	Algorithm(name, pSvcLocator) 
{  
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode LumTau::initialize()
{
	MsgStream log(msgSvc(), name());

	log << MSG::INFO << "in initialize()" << endmsg;

	StatusCode status;

	NTuplePtr nt2(ntupleSvc(), "LumTau/event");
	if ( nt2 ) m_tuple2 = nt2;
	else 
	{
		m_tuple2 = ntupleSvc()->book ("LumTau/event", CLID_ColumnWiseTuple, "Bhabha N-Tuple signal");
		if ( m_tuple2 )
		{
			status = m_tuple2->addItem ("run",    m_run);
			status = m_tuple2->addItem ("rec",    m_rec);
			status = m_tuple2->addItem ("time",  m_time);  
			status = m_tuple2->addItem ("etot",  m_etot);
			status = m_tuple2->addItem ("e1",    m_e1);
			status = m_tuple2->addItem ("e2",    m_e2);
			status = m_tuple2->addItem ("costht1",    m_costht1);
			status = m_tuple2->addItem ("costht2",    m_costht2);
			status = m_tuple2->addItem ("dltphi",    m_dltphi);
			status = m_tuple2->addItem ("dlttht",    m_dlttht);
			status = m_tuple2->addItem ("phi1",    m_phi1);
			status = m_tuple2->addItem ("phi2",    m_phi2);
		}
		else
		{
			log << MSG::ERROR << "Cannot book N-tuple2:"<<long(m_tuple2)<<endmsg;
			return StatusCode::FAILURE;
		}
	}
	return StatusCode::SUCCESS;
}
//*************************************************
StatusCode LumTau::execute()
{
	StatusCode sc=StatusCode::SUCCESS;

	MsgStream log(msgSvc(),name());
	log<<MSG::INFO<<"in execute()"<<endreq;

	SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
	SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);

	log<<MSG::DEBUG<<"ncharg, nneu, tottks = "<<evtRecEvent->totalCharged()<<" , "<<evtRecEvent->totalNeutral()<<" , "<<evtRecEvent->totalTracks()<<endreq;
	SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),EventModel::EvtRec::EvtRecTrackCol);

	log<<MSG::DEBUG <<"ncharg, nneu, tottks = "<<evtRecEvent->totalCharged()<<" , "<<evtRecEvent->totalNeutral()<<" , "<<evtRecEvent->totalTracks()<<endreq;
	SmartDataPtr<RecEsTimeCol> evTimeCol(eventSvc(),"/Event/Recon/RecEsTimeCol");

	double time = eventHeader->time();
	m_time = time;

	m_run = eventHeader->runNumber();
	m_rec = eventHeader->eventNumber();

	if(m_rec%1000==0)cout<<"Run   "<<m_run<<"     Event   "<<m_rec<<endl;

	double Emax1 = -1;
	double Emax2 = -1;
	int Imax[2];

	if((evtRecEvent->totalTracks() >= 2)){
		double etot = 0.;
		for(int i = 0;i < evtRecEvent->totalTracks(); i++)
		{
			if(i>=evtRecTrkCol->size()) break;
			EvtRecTrackIterator  itTrk=evtRecTrkCol->begin() + i;
			if(!(*itTrk)->isEmcShowerValid()) continue;
			RecEmcShower *emcTrk = (*itTrk)->emcShower();
			double Ener=emcTrk->energy();
			if(Ener>Emax2)
			{
				Emax2=Ener;
				Imax[1]=i;
			}
			if(Ener>Emax1)
			{
				Emax2=Emax1;
				Imax[1]=Imax[0];
				Emax1=Ener;
				Imax[0]=i;
			}
			etot += Ener;
		}

		m_etot = etot;
		m_e1 = Emax1;
		m_e2 = Emax2;

		log << MSG::INFO << "Emax1 = " << Emax1 <<"Emax2= "<<Emax2<< endreq;
		if(Emax1 > 0 && Emax2 > 0){
			double emcphi[2],emctht[2];
			for(int i = 0;i < 2; i++)
			{
				if(i>=evtRecTrkCol->size()) break;
				EvtRecTrackIterator  itTrk=evtRecTrkCol->begin() + Imax[i];
				if(!(*itTrk)->isEmcShowerValid()) continue;
				RecEmcShower *emcTrk = (*itTrk)->emcShower();
				emcphi[i]=emcTrk->phi();
				emctht[i]=emcTrk->theta();
			}

			double dltphi=(fabs(emcphi[0]-emcphi[1])-pai)*180./pai;
			double dlttht=(fabs(emctht[0]+emctht[1])-pai)*180./pai;
			m_costht1=cos(emctht[0]);
			m_costht2=cos(emctht[1]);
			m_phi1=emcphi[0];
			m_phi2=emcphi[1];
			m_dlttht=dlttht;
			m_dltphi=dltphi;
		}
		else{
			m_etot = -1;
			m_e1 = -1;
			m_e2 = -1;
			m_costht1 = -1;
			m_costht2 = -1;
			m_phi1 = -1;
			m_phi2 = -1;
			m_dlttht = -1;
			m_dltphi = -1;
		}
	}
	else{
		m_etot = -1;
		m_e1 = -1;
		m_e2 = -1;
		m_costht1 = -1;
		m_costht2 = -1;
		m_phi1 = -1;
		m_phi2 = -1;
		m_dlttht = -1;
		m_dltphi = -1;
	}

//	m_tuple2->write();

	int DiskWrite = m_tuple2->write();
	if(DiskWrite != 1){
		log<<MSG::FATAL<<"ERROR In LumTau DiskWrite!"<<endreq;
		exit(1);
	}

	return StatusCode::SUCCESS;
}

StatusCode LumTau::finalize() 
{
	cout<<"Event Finalize"<<endl;
	return StatusCode::SUCCESS;
}

