#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "EventModel/EventModel.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecDTag.h"
#include "VertexFit/Helix.h"
#include "BestDTagSvc/BestDTagSvc.h"

BestDTagSvc::BestDTagSvc(const std::string& name, ISvcLocator* svcLoc)
   : Service(name, svcLoc)
{
   //declareProperty("", m_);
   declareProperty("pidTight", m_isPidTight = true);
}

BestDTagSvc::~BestDTagSvc()
{
}

StatusCode BestDTagSvc::initialize()
{
   MsgStream log(messageService(), name());
   log << MSG::INFO << "@initialize()" << endreq;

   StatusCode sc = Service::initialize();

   sc = serviceLocator()->service("EventDataSvc", eventSvc_, true);
   sc = serviceLocator()->service("VertexDbSvc", vtxSvc_, true);

   return sc;
}

StatusCode BestDTagSvc::finalize()
{
   MsgStream log(messageService(), name());
   log << MSG::INFO << "@initialize()" << endreq;

   StatusCode sc = Service::finalize();

   return sc;
}

StatusCode BestDTagSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
   if ( IBestDTagSvc::interfaceID().versionMatch(riid) ) {
      *ppvIF = dynamic_cast<IBestDTagSvc*>(this);
   }
   else {
      return Service::queryInterface(riid, ppvIF);
   }
   addRef();
   return StatusCode::SUCCESS;
}

EvtRecDTag* BestDTagSvc::getSingleTag(int modeid, int charm)
{
   MsgStream log(messageService(), name());

   SmartDataPtr<EvtRecDTagCol> dtags(eventSvc_, EventModel::EvtRec::EvtRecDTagCol);
   if ( !dtags ) {
      log << MSG::ERROR << "Can't retrieve the EvtRecDTagCol!" << endreq;
      return NULL;
   }

   EvtRecDTag* pbest = NULL;

   double bestDE = 999.;

   for (EvtRecDTagCol::iterator it = dtags->begin(); it != dtags->end(); ++it) {
	 if (m_isPidTight ) {
	 if ( (*it)->type() != EvtRecDTag::Tight) continue;
	 }
	 if ( modeid >= 0 && (*it)->decayMode() != modeid ) {
	   continue;
	 }
	 if ( charm != 0 && (*it)->charm() != charm ) {
	   continue;
	 }
	 if ( fabs((*it)->deltaE()) < bestDE ) {
	   bestDE = fabs((*it)->deltaE());
	   pbest  = (*it);
	 }
   }

   return pbest;
}

EvtRecDTag** BestDTagSvc::getDoubleTag(int modeid1, int modeid2, int charm)
{
  static EvtRecDTag** tagpp = new EvtRecDTag*[2];

  //not implemented yet, just a placeholder for the future


  if ( false ) {
	return tagpp;
  }

  return NULL;
}

bool BestDTagSvc::isCosmicOrLepton()
{
  SmartDataPtr<EvtRecEvent> evt(eventSvc_, "/Event/EvtRec/EvtRecEvent");
  SmartDataPtr<EvtRecTrackCol> trks(eventSvc_, EventModel::EvtRec::EvtRecTrackCol);
  if ( !evt || !trks ) {
	return false;
  }

  std::vector<EvtRecTrack*> gtrks;
  for (int i = 0; i < evt->totalCharged(); ++i) {
	EvtRecTrack* trk = *(trks->begin() + i);
	if ( isGoodTrack(trk) ) {
	  gtrks.push_back(trk);
	}
  }
  if ( gtrks.size() != 2 ) return false;

  double time[2] = { -100., -100.};
  for ( int i = 0; i < 2; ++i ) {
	if ( gtrks[i]->isTofTrackValid() ){
	  SmartRefVector<RecTofTrack> tofTrks = gtrks[i]->tofTrack();
	  for ( SmartRefVector<RecTofTrack>::iterator it = tofTrks.begin(); it != tofTrks.end(); ++it ) {
		if ( ( (*it)->status() & 0x8 ) == 0x8 ) {
		  time[i] = (*it)->tof();
		  break;
		}
	  }
	}
  }
  if ( time[0] > -99. && time[1] > -99. && fabs(time[0]-time[1]) > 5 ) return true;

  if ( isElectron(gtrks[0]) && isElectron(gtrks[1]) ) return true;
  if ( isMuon(gtrks[0]) && isMuon(gtrks[1]) ) return true;

  return false;
}

bool BestDTagSvc::isGoodTrack(EvtRecTrack* atrk)
{
  Hep3Vector xorigin(0,0,0);
  if(vtxSvc_->isVertexValid()){
	double* dbv = vtxSvc_->PrimaryVertex();
	double*  vv = vtxSvc_->SigmaPrimaryVertex();
	xorigin.setX(dbv[0]);
	xorigin.setY(dbv[1]);
	xorigin.setZ(dbv[2]);
  }

  if ( atrk->isMdcKalTrackValid() ) {
	RecMdcKalTrack *mdcKalTrk = atrk->mdcKalTrack();
	mdcKalTrk->setPidType(RecMdcKalTrack::pion);
	HepVector    a  = mdcKalTrk->getZHelix();
	HepSymMatrix Ea = mdcKalTrk->getZError();
	HepPoint3D pivot(0., 0., 0.);
	HepPoint3D IP(xorigin[0], xorigin[1], xorigin[2]);
	VFHelix helixp(pivot, a, Ea);
	helixp.pivot(IP);
	HepVector vec    = helixp.a();
	double    vrl    = vec[0];
	double    vzl    = vec[3];
	double costheta  = cos( mdcKalTrk->theta() );

	if ( fabs(vrl) < 1 && fabs(vzl) < 10 && fabs(costheta) < 0.93 ) {
	  return true;
	}
  }

  return false;
}

bool BestDTagSvc::isElectron(EvtRecTrack* atrk)
{
  if ( atrk->isMdcDedxValid() && atrk->isMdcKalTrackValid() && atrk->isEmcShowerValid() ) {
	double trkP = atrk->mdcKalTrack()->p();
	double emcE = atrk->emcShower()->energy();
	double dedxChi = atrk->mdcDedx()->chiE();

	if ( emcE/trkP > 0.8 && fabs(dedxChi) < 5 ) return true;
  }

  return false;
}

bool BestDTagSvc::isMuon(EvtRecTrack* atrk)
{
  if ( atrk->isMucTrackValid() ) {
	if ( atrk->mucTrack()->depth() >= 5 ) {
	  return true;
	}
  }

  return false;
}

