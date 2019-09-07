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
#include "EvtRecEvent/EvtRecPi0.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "MdcRecEvent/RecMdcKalTrack.h"

#include "BFieldCorr/BFieldCorr.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Matrix.h"
using CLHEP::HepVector;
using CLHEP::HepLorentzVector;
using CLHEP::Hep3Vector;
using CLHEP::HepMatrix;
using CLHEP::HepSymMatrix;

#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

#include <cmath>
#include <vector>
using namespace std;

BFieldCorr::BFieldCorr(const std::string& name, ISvcLocator* pSvcLocator):Algorithm(name,pSvcLocator)
{
	declareProperty("CorrectionFactor",  m_factor = 1.0000);
}

StatusCode BFieldCorr::initialize()
{
	m_Ew = HepSymMatrix(5, 0);
	m_Ew[0][0] = 1.0; 
	m_Ew[1][1] = 1.0; 
	m_Ew[2][2] = fabs(m_factor) < 1e-6 ? 1.0 : 1.0 / m_factor; 
	m_Ew[3][3] = 1.0; 
	m_Ew[4][4] = 1.0; 
	RUN_BEGIN_10 = 11414;
	RUN_END_10   = 14604;
	RUN_BEGIN_11 = 20448;
	RUN_END_11   = 23454;

	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "in MagCorr::initialize()" <<endmsg;
	return StatusCode::SUCCESS;
}

StatusCode BFieldCorr::execute()
{
	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "in BFieldCorr::execute()" << endreq;
	SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
	int run_no = eventHeader->runNumber();
	if (run_no < 0) return StatusCode::SUCCESS;

	SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
	if (!evtRecEvent)
	{
		log << MSG::FATAL << "Could not find EvtRecEvent" << endreq;
		return StatusCode::SUCCESS;
	}

	SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),  EventModel::EvtRec::EvtRecTrackCol);
	if (!evtRecTrkCol)
	{
		log << MSG::FATAL << "Could not find EvtRecTrackCol" << endreq;
		return StatusCode::SUCCESS;
	}

	if ((unsigned int)evtRecEvent->totalTracks() > evtRecTrkCol->size()) return StatusCode::SUCCESS;
	//if (evtRecEvent->totalTracks() > 50) return StatusCode::SUCCESS;

	//set Ew if by default
	double factor;
	if (fabs(m_factor - 1.000) < 1e-6)
	{
		if (run_no >= RUN_BEGIN_10 && run_no <= RUN_END_10)
			factor = 1.0004;
		else if (run_no >= RUN_BEGIN_11 && run_no <= RUN_END_11)
			factor = 1.0002;
		else
			factor = 1.0000;
	}	
	else
	{
		factor = m_factor;
	}
	m_Ew[2][2] = fabs(factor) < 1e-6 ? 1.0 : 1.0 / factor; 

	for (int i = 0; i < evtRecEvent->totalCharged(); i++)
	{
		EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + i;
		if (!(*itTrk)->isMdcTrackValid()) continue;
		if (!(*itTrk)->isMdcKalTrackValid()) continue;

		//RecMdcTrack correction 
		RecMdcTrack *mdc_trk = (*itTrk)->mdcTrack();
		mdc_trk->setHelix( m_Ew * mdc_trk->helix() );
		mdc_trk->setError( mdc_trk->err().similarity(m_Ew) );
		mdc_trk->setP(   factor * mdc_trk->p()   );
		mdc_trk->setPxy( factor * mdc_trk->pxy() );
		mdc_trk->setPx(  factor * mdc_trk->px()  );
		mdc_trk->setPy(  factor * mdc_trk->py()  );
		mdc_trk->setPz(  factor * mdc_trk->pz()  );

		//RecMdcKalTrack correction 
		RecMdcKalTrack *kal_trk = (*itTrk)->mdcKalTrack();
		RecMdcKalTrack::PidType trk_type[] = {
			RecMdcKalTrack::electron,
			RecMdcKalTrack::muon,
			RecMdcKalTrack::pion,
			RecMdcKalTrack::kaon,
			RecMdcKalTrack::proton,
		};
		
		for (int j = 0; j < 5; j++)
		{
			kal_trk->setPidType( trk_type[j] );
			kal_trk->setZHelix( m_Ew * kal_trk->helix(), j );
			kal_trk->setZError( kal_trk->err().similarity(m_Ew), j );
			kal_trk->setP(   factor * kal_trk->p(),   j );
			kal_trk->setPxy( factor * kal_trk->pxy(), j );
			kal_trk->setPx(  factor * kal_trk->px(),  j );
			kal_trk->setPy(  factor * kal_trk->py(),  j );
			kal_trk->setPz(  factor * kal_trk->pz(),  j );
		}
	}
	return StatusCode::SUCCESS;
}

StatusCode BFieldCorr::finalize()
{
	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "in BFieldCorr::finalize()" << endreq;
	return StatusCode::SUCCESS;
}
