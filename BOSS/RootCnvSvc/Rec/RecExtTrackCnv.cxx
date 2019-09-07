#ifndef RecExtTrackCnv_CXX
#define RecExtTrackCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"

#include "ReconEvent/ReconEvent.h"

#include "ExtEvent/RecExtTrack.h"
//#include "RootEventData/TMucTrack.h" // standard root object
#include "RootEventData/TRecMucTrack.h" // standard root object
//#include "RootEventData/TDstEvent.h"
#include "RootEventData/TRecTrackEvent.h"

#include "RootCnvSvc/Rec/RecTrackCnv.h"
#include "RootCnvSvc/Rec/RecExtTrackCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include <vector>

using namespace std;

// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<RecExtTrackCnv> s_factory;
//const ICnvFactory& RecExtTrackCnvFactory = s_factory;

RecExtTrackCnv::RecExtTrackCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{

	// Here we associate this converter with the /Event path on the TDS.
	MsgStream log(msgSvc(), "RecExtTrackCnv");
	//log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
	//m_rootTreename ="Rec";
	//m_rootBranchname ="m_RecExtTrackCol";
	m_rootBranchname ="m_extTrackCol";
	//declareObject(EventModel::Recon::ExtTrackCol, objType(), m_rootTreename, m_rootBranchname);
	m_adresses.push_back(&m_extTrackCol);
	m_extTrackCol=0;
}

StatusCode RecExtTrackCnv::TObjectToDataObject(DataObject*& refpObject) {
	// creation of TDS object from root object
	MsgStream log(msgSvc(), "RecExtTrackCnv");
	log << MSG::DEBUG << "RecExtTrackCnv::TObjectToDataObject" << endreq;
	StatusCode sc=StatusCode::SUCCESS;

	// create the TDS location for the ExtTrack Collection
	RecExtTrackCol* extTrackTdsCol = new RecExtTrackCol;
	refpObject=extTrackTdsCol;


	// now convert
	if (!m_extTrackCol) return sc;
	TIter extTrackIter(m_extTrackCol);
	TRecExtTrack *extTrackRoot = 0;
	while ((extTrackRoot = (TRecExtTrack*)extTrackIter.Next())) {

		RecExtTrack *extTrackTds = new RecExtTrack();
		m_common.m_rootExtTrackMap[extTrackRoot] = extTrackTds;
		int trackId = extTrackRoot->GetTrackId();
		extTrackTds->SetTrackId(trackId);
		for(int iPart=0; iPart<5; iPart++) {
			extTrackTds->SetParType(iPart);
			//Tof layer1:
			double      p1x = extTrackRoot->GetTof1PositionX(iPart);
			double      p1y = extTrackRoot->GetTof1PositionY(iPart);
			double      p1z = extTrackRoot->GetTof1PositionZ(iPart);
			double      m1x = extTrackRoot->GetTof1MomentumX(iPart);
			double      m1y = extTrackRoot->GetTof1MomentumY(iPart);
			double      m1z = extTrackRoot->GetTof1MomentumZ(iPart);
			Hep3Vector tof1p(p1x,p1y,p1z);
			Hep3Vector tof1m(m1x,m1y,m1z);

			// char * pTof1 = extTrackRoot->GetTof1VolumeName().Data();
			string v1s = extTrackRoot->GetTof1VolumeName(iPart).Data();
			int     v1n = extTrackRoot->GetTof1VolumeNumber(iPart) ;
			double tof1= extTrackRoot->GetTof1(iPart);
			double tof1path= extTrackRoot->GetTof1Path(iPart);
			double z1 = extTrackRoot->GetTof1PosSigmaAlongZ(iPart);
			double t1 = extTrackRoot->GetTof1PosSigmaAlongT(iPart);
			double x1 = extTrackRoot->GetTof1PosSigmaAlongX(iPart);
			double y1 = extTrackRoot->GetTof1PosSigmaAlongY(iPart);
			HepSymMatrix e1m(6) ;
			for (int i = 0;i<6 ;i++ )
				for (int j= 0;j<6 ;j++ )
					e1m[i][j]= extTrackRoot->GetTof1ErrorMatrix(i , j, iPart) ;

			//Tof layer2:
			double      p2x = extTrackRoot->GetTof2PositionX(iPart) ;
			double      p2y = extTrackRoot->GetTof2PositionY(iPart) ;
			double      p2z = extTrackRoot->GetTof2PositionZ(iPart) ;
			double      m2x = extTrackRoot->GetTof2MomentumX(iPart) ;
			double      m2y = extTrackRoot->GetTof2MomentumY(iPart) ;
			double      m2z = extTrackRoot->GetTof2MomentumZ(iPart);
			Hep3Vector tof2p(p2x,p2y,p2z);
			Hep3Vector tof2m(m2x,m2y,m2z);

			string v2s = extTrackRoot->GetTof2VolumeName(iPart).Data();
			int     v2n = extTrackRoot->GetTof2VolumeNumber(iPart) ;
			double tof2= extTrackRoot->GetTof2(iPart);
			double tof2path = extTrackRoot->GetTof2Path(iPart);
			double z2 = extTrackRoot->GetTof2PosSigmaAlongZ(iPart) ;
			double t2 = extTrackRoot->GetTof2PosSigmaAlongT(iPart);
			double x2 = extTrackRoot->GetTof2PosSigmaAlongX(iPart);
			double y2 = extTrackRoot->GetTof2PosSigmaAlongY(iPart);
			HepSymMatrix e2m(6) ;
			for (int i = 0;i<6 ;i++ )
				for (int j= 0;j<6 ;j++ )
					e2m[i][j]= extTrackRoot->GetTof2ErrorMatrix(i,j,iPart) ;

			//Emc
			double      pEx = extTrackRoot->GetEmcPositionX(iPart);
			double      pEy = extTrackRoot->GetEmcPositionY(iPart);
			double      pEz = extTrackRoot->GetEmcPositionZ(iPart);
			double      mEx = extTrackRoot->GetEmcMomentumX(iPart);
			double      mEy = extTrackRoot->GetEmcMomentumY(iPart);
			double      mEz = extTrackRoot->GetEmcMomentumZ(iPart);
			Hep3Vector Ep(pEx,pEy,pEz);
			Hep3Vector Em(mEx,mEy,mEz);

			string vEs = extTrackRoot->GetEmcVolumeName(iPart).Data();
			int     vEn = extTrackRoot->GetEmcVolumeNumber(iPart);
			double theta = extTrackRoot->GetEmcPosSigmaAlongTheta(iPart);
			double phi = extTrackRoot->GetEmcPosSigmaAlongPhi(iPart);
			HepSymMatrix eEm(6) ;
			for(int i=0;i<6 ;i++ )
				for(int j= 0;j<6 ;j++ )
					eEm[i][j]= extTrackRoot->GetEmcErrorMatrix(i,j,iPart);
			double emcPath=extTrackRoot->emcPath(iPart);

			//Muc
			double      pMx = extTrackRoot->GetMucPositionX(iPart) ;
			double      pMy = extTrackRoot->GetMucPositionY(iPart) ;
			double      pMz = extTrackRoot->GetMucPositionZ(iPart) ;
			double      mMx = extTrackRoot->GetMucMomentumX(iPart) ;
			double      mMy = extTrackRoot->GetMucMomentumY(iPart) ;
			double      mMz = extTrackRoot->GetMucMomentumZ(iPart);
			Hep3Vector Mp(pMx,pMy,pMz);
			Hep3Vector Mm(mMx,mMy,mMz);

			string vMs = extTrackRoot->GetMucVolumeName(iPart).Data();
			int     vMn = extTrackRoot->GetMucVolumeNumber(iPart) ;

			double zM = extTrackRoot->GetMucPosSigmaAlongZ(iPart) ;
			double tM = extTrackRoot->GetMucPosSigmaAlongT(iPart);
			double xM = extTrackRoot->GetMucPosSigmaAlongX(iPart);
			double yM = extTrackRoot->GetMucPosSigmaAlongY(iPart);
			HepSymMatrix eMm(6) ;
			for (int i = 0;i<6 ;i++ )
				for (int j= 0;j<6 ;j++ )
					eMm[i][j]= extTrackRoot->GetMucErrorMatrix(i,j,iPart) ;


			//ExtMucHitVec
			int size = extTrackRoot->GetSize(iPart);
			ExtMucHit aExtMucHit;
			// Hep3Vector p(0,0,0);
			// Hep3Vector m(0,0,0);
			string vs;
			int vn;
			double z;
			double t;
			double x;
			double y;
			HepSymMatrix matrix(6);

			for(int i =0;i<size;i++){
				double px = extTrackRoot->GetPositionX(i,iPart) ;
				double py = extTrackRoot->GetPositionY(i,iPart) ;
				double pz = extTrackRoot->GetPositionZ(i,iPart) ;
				Hep3Vector p(px,py,pz);
				double mx = extTrackRoot->GetMomentumX(i,iPart) ;
				double my = extTrackRoot->GetMomentumY(i,iPart) ;
				double mz = extTrackRoot->GetMomentumZ(i,iPart);
				Hep3Vector m(mx,my,mz);
				vs = extTrackRoot->GetVolumeName(i,iPart).Data();
				vn = extTrackRoot->GetVolumeNumber(i,iPart) ;
				z = extTrackRoot->GetPosSigmaAlongZ(i,iPart) ;
				t = extTrackRoot->GetPosSigmaAlongT(i,iPart);
				x = extTrackRoot->GetPosSigmaAlongX(i,iPart);
				y = extTrackRoot->GetPosSigmaAlongY(i,iPart);
				std::vector<double> vecError = extTrackRoot->GetErrorMatrix(i,iPart);
				for(int k =0;k<6;k++){
					for(int j =0; j<6;j++){
						matrix[k][j] = vecError[k*6+j];
					}
				}
				aExtMucHit.SetExtMucHit(p,m,vs,vn,matrix,z,t,x,y);
				extTrackTds->AddExtMucHit(aExtMucHit);
			}

			// DstExtTrack *extTrackTds = new DstExtTrack();
			extTrackTds->SetTof1Data( tof1p, tof1m, v1s, v1n , tof1, tof1path, e1m, z1, t1, x1, y1 );
			extTrackTds->SetTof2Data( tof2p, tof2m, v2s, v2n , tof2, tof2path, e2m, z2, t2, x2, y2 );
			extTrackTds->SetEmcData( Ep, Em, vEs, vEn , theta, phi , eEm);
			extTrackTds->SetEmcPath(emcPath);
			extTrackTds->SetMucData( Mp, Mm, vMs, vMn , eMm, zM, tM, xM, yM );

			extTrackTdsCol->push_back(extTrackTds);


			//delete extTrackTds; // wensp add 2005/12/31
			// extTrackTds = NULL;
		}
		extTrackTds->SetParType(2);// default particle: pion
	}
	//m_extTrackCol->Delete();  // wensp add 2005/12/30
	delete m_extTrackCol;
	m_extTrackCol = 0;


	return StatusCode::SUCCESS;
}

StatusCode RecExtTrackCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {



       MsgStream log(msgSvc(), "RecExtTrackCnv");
	log << MSG::DEBUG << "RecExtTrackCnv::DataObjectToTObject" << endreq;
	StatusCode sc=StatusCode::SUCCESS;

	RecExtTrackCol * extTrackColTds=dynamic_cast<RecExtTrackCol *> (obj);
	if (!extTrackColTds) {
		log << MSG::ERROR << "Could not downcast to RecExtTrackCol" << endreq;
		return StatusCode::FAILURE;
	}

	DataObject *evt;
	m_eds->findObject(EventModel::Recon::Event,evt);
	if (evt==NULL) {
		log << MSG::ERROR << "Could not get ReconEvent in TDS "  << endreq;
		return StatusCode::FAILURE;
	}

	ReconEvent * devtTds=dynamic_cast<ReconEvent *> (evt);
	if (!devtTds) {
		log << MSG::ERROR << "RecExtTrackCnv:Could not downcast to TDS Rec Event" << endreq;
	}
	IOpaqueAddress *addr;

	m_cnvSvc->getRecTrackCnv()->createRep(evt,addr);
	TRecTrackEvent *recEvt=m_cnvSvc->getRecTrackCnv()->getWriteObject();

	const TObjArray *m_extTrackCol = recEvt->getExtTrackCol();

	if (!m_extTrackCol) return sc;

	recEvt->clearExtTrackCol(); //necessary in case there is I/O at the same time since array is static
	RecExtTrackCol::const_iterator extTrackTds;

	for (extTrackTds = extTrackColTds->begin(); extTrackTds != extTrackColTds->end(); extTrackTds++) {
		//Get Data from TDS
		Int_t trackId = (*extTrackTds)->GetTrackId();
		TRecExtTrack *extTrackRoot = new TRecExtTrack;
		//Set to Root
		extTrackRoot->SetTrackId(trackId);
		for(int iPart=0; iPart<5; iPart++) {
			//Tof layer1
			Double_t  p1x =  (*extTrackTds)->tof1Position(iPart).x();
			Double_t  p1y =  (*extTrackTds)->tof1Position(iPart).y();
			Double_t  p1z =  (*extTrackTds)->tof1Position(iPart).z();
			Double_t  m1x =  (*extTrackTds)->tof1Momentum(iPart).x();
			Double_t  m1y =  (*extTrackTds)->tof1Momentum(iPart).y();
			Double_t  m1z =  (*extTrackTds)->tof1Momentum(iPart).z();

			TString v1s = (*extTrackTds)->tof1VolumeName(iPart) ;
			Int_t     v1n = (*extTrackTds)->tof1VolumeNumber(iPart) ;
			Double_t tof1= (*extTrackTds)->tof1(iPart);
			Double_t tof1p = (*extTrackTds)->tof1Path(iPart);
			Double_t z1 = (*extTrackTds)->tof1PosSigmaAlongZ(iPart);
			Double_t t1 = (*extTrackTds)->tof1PosSigmaAlongT(iPart);
			Double_t x1 = (*extTrackTds)->tof1PosSigmaAlongX(iPart);
			Double_t y1 = (*extTrackTds)->tof1PosSigmaAlongY(iPart);
			Double_t e1m[6][6];
			HepSymMatrix e1mTds = (*extTrackTds)->tof1ErrorMatrix(iPart);
			for (int i = 0;i<6 ;i++ )
				for (int j= 0;j<6 ;j++ )
					e1m[i][j] = e1mTds[i][j];

			//Tof llayer2
			Double_t  p2x =  (*extTrackTds)->tof2Position(iPart).x();
			Double_t  p2y =  (*extTrackTds)->tof2Position(iPart).y();
			Double_t  p2z =  (*extTrackTds)->tof2Position(iPart).z();
			Double_t  m2x =  (*extTrackTds)->tof2Momentum(iPart).x();
			Double_t  m2y =  (*extTrackTds)->tof2Momentum(iPart).y();
			Double_t  m2z =  (*extTrackTds)->tof2Momentum(iPart).z();

			TString v2s = (*extTrackTds)->tof2VolumeName(iPart) ;
			Int_t     v2n = (*extTrackTds)->tof2VolumeNumber(iPart) ;
			Double_t tof2= (*extTrackTds)->tof2(iPart);
			Double_t tof2p = (*extTrackTds)->tof2Path(iPart);
			Double_t z2 = (*extTrackTds)->tof2PosSigmaAlongZ(iPart);
			Double_t t2 = (*extTrackTds)->tof2PosSigmaAlongT(iPart);
			Double_t x2 = (*extTrackTds)->tof2PosSigmaAlongX(iPart);
			Double_t y2 = (*extTrackTds)->tof2PosSigmaAlongY(iPart);
			Double_t e2m[6][6];
			HepSymMatrix e2mTds = (*extTrackTds)->tof2ErrorMatrix(iPart);
			for (int i = 0;i<6 ;i++ )
				for (int j= 0;j<6 ;j++ )
					e2m[i][j] = e2mTds[i][j];
			//Emc
			Double_t  pEx =  (*extTrackTds)->emcPosition(iPart).x();
			Double_t  pEy =  (*extTrackTds)->emcPosition(iPart).y();
			Double_t  pEz =  (*extTrackTds)->emcPosition(iPart).z();
			Double_t  mEx =  (*extTrackTds)->emcMomentum(iPart).x();
			Double_t  mEy =  (*extTrackTds)->emcMomentum(iPart).y();
			Double_t  mEz =  (*extTrackTds)->emcMomentum(iPart).z();

			TString vEs = (*extTrackTds)->emcVolumeName(iPart) ;
			Int_t     vEn = (*extTrackTds)->emcVolumeNumber(iPart) ;

			Double_t theta = (*extTrackTds)->emcPosSigmaAlongTheta(iPart);
			Double_t phi = (*extTrackTds)->emcPosSigmaAlongPhi(iPart);

			Double_t eEm[6][6];
			HepSymMatrix eEmTds = (*extTrackTds)->emcErrorMatrix(iPart);
			for (int i = 0;i<6 ;i++ )
				for (int j= 0;j<6 ;j++ )
					eEm[i][j] = eEmTds[i][j];
			Double_t emcPath=(*extTrackTds)->emcPath(iPart);

			//Muc
			Double_t  pMx =  (*extTrackTds)->mucPosition(iPart).x();
			Double_t  pMy =  (*extTrackTds)->mucPosition(iPart).y();
			Double_t  pMz =  (*extTrackTds)->mucPosition(iPart).z();
			Double_t  mMx =  (*extTrackTds)->mucMomentum(iPart).x();
			Double_t  mMy =  (*extTrackTds)->mucMomentum(iPart).y();
			Double_t  mMz =  (*extTrackTds)->mucMomentum(iPart).z();

			TString vMs = (*extTrackTds)->mucVolumeName(iPart) ;
			Int_t     vMn = (*extTrackTds)->mucVolumeNumber(iPart) ;

			Double_t zM = (*extTrackTds)->mucPosSigmaAlongZ(iPart);
			Double_t tM = (*extTrackTds)->mucPosSigmaAlongT(iPart);
			Double_t xM = (*extTrackTds)->mucPosSigmaAlongX(iPart);
			Double_t yM = (*extTrackTds)->mucPosSigmaAlongY(iPart);

			Double_t eMm[6][6];
			HepSymMatrix eMmTds = (*extTrackTds)->mucErrorMatrix(iPart);
			for (int i = 0;i<6 ;i++ )
				for (int j= 0;j<6 ;j++ )
					eMm[i][j]= eMmTds[i][j];

			//ExtMucHitVec
			ExtMucHitVec vecHit = (*extTrackTds)->GetExtMucHitVec(iPart);
			Int_t size = vecHit.size();

			//m_common.m_extTrackMap[(*extTrackTds)] = extTrackRoot;

			//Set to Root

			//ExtMucHitVec
			extTrackRoot->SetSize(size,iPart);
			for(int i=0;i<size;i++){
				Double_t px = vecHit[i].GetPosition().x();
				Double_t py = vecHit[i].GetPosition().y();
				Double_t pz = vecHit[i].GetPosition().z();
				Double_t mx = vecHit[i].GetMomentum().x();
				Double_t my = vecHit[i].GetMomentum().y();
				Double_t mz = vecHit[i].GetMomentum().z();
				TString vs = vecHit[i].GetVolumeName();
				Int_t vn = vecHit[i].GetVolumeNumber();
				Double_t z = vecHit[i].GetPosSigmaAlongZ();
				Double_t t = vecHit[i].GetPosSigmaAlongT();
				Double_t x = vecHit[i].GetPosSigmaAlongX();
				Double_t y = vecHit[i].GetPosSigmaAlongY();
				Double_t m[6][6];
				HepSymMatrix mTds = vecHit[i].GetErrorMatrix();
				for (int j = 0;j<6;j++)
					for(int k=0;k<6;k++)
						m[j][k] = mTds[j][k];

				extTrackRoot->SetExtMucHit(px,py,pz,mx,my,mz,vs,vn,z,t,x,y,m,iPart);
			}

			//Tof layer1
			extTrackRoot->SetTof1PositionX(p1x,iPart);
			extTrackRoot->SetTof1PositionY(p1y,iPart);
			extTrackRoot->SetTof1PositionZ(p1z,iPart);
			extTrackRoot->SetTof1MomentumX(m1x,iPart);
			extTrackRoot->SetTof1MomentumY(m1y,iPart);
			extTrackRoot->SetTof1MomentumZ(m1z,iPart);

			extTrackRoot->SetTof1VolumeName(v1s,iPart);
			extTrackRoot->SetTof1VolumeNumber(v1n,iPart);

			extTrackRoot->SetTof1(tof1,iPart);
			extTrackRoot->SetTof1Path(tof1p,iPart);

			extTrackRoot->SetTof1PosSigmaAlongZ(z1,iPart);
			extTrackRoot->SetTof1PosSigmaAlongT(t1,iPart);
			extTrackRoot->SetTof1PosSigmaAlongX(x1,iPart);
			extTrackRoot->SetTof1PosSigmaAlongY(y1,iPart);

			extTrackRoot->SetTof1ErrorMatrix(e1m,iPart);
			//Tof layer2
			extTrackRoot->SetTof2PositionX(p2x,iPart);
			extTrackRoot->SetTof2PositionY(p2y,iPart);
			extTrackRoot->SetTof2PositionZ(p2z,iPart);
			extTrackRoot->SetTof2MomentumX(m2x,iPart);
			extTrackRoot->SetTof2MomentumY(m2y,iPart);
			extTrackRoot->SetTof2MomentumZ(m2z,iPart);

			extTrackRoot->SetTof2VolumeName(v2s,iPart);
			extTrackRoot->SetTof2VolumeNumber(v2n,iPart);

			extTrackRoot->SetTof2(tof2,iPart);
			extTrackRoot->SetTof2Path(tof2p,iPart);

			extTrackRoot->SetTof2PosSigmaAlongZ(z2,iPart);
			extTrackRoot->SetTof2PosSigmaAlongT(t2,iPart);
			extTrackRoot->SetTof2PosSigmaAlongX(x2,iPart);
			extTrackRoot->SetTof2PosSigmaAlongY(y2,iPart);

			extTrackRoot->SetTof2ErrorMatrix(e2m,iPart);
			//Emc
			extTrackRoot->SetEmcPositionX(pEx,iPart);
			extTrackRoot->SetEmcPositionY(pEy,iPart);
			extTrackRoot->SetEmcPositionZ(pEz,iPart);
			extTrackRoot->SetEmcMomentumX(mEx,iPart);
			extTrackRoot->SetEmcMomentumY(mEy,iPart);
			extTrackRoot->SetEmcMomentumZ(mEz,iPart);

			extTrackRoot->SetEmcVolumeName(vEs,iPart);
			extTrackRoot->SetEmcVolumeNumber(vEn,iPart);

			extTrackRoot->SetEmcPosSigmaAlongTheta(theta,iPart);
			extTrackRoot->SetEmcPosSigmaAlongPhi(phi,iPart);

			extTrackRoot->SetEmcErrorMatrix(eEm,iPart);
			extTrackRoot->SetEmcPath(emcPath,iPart);
			//Muc
			extTrackRoot->SetMucPositionX(pMx,iPart);
			extTrackRoot->SetMucPositionY(pMy,iPart);
			extTrackRoot->SetMucPositionZ(pMz,iPart);
			extTrackRoot->SetMucMomentumX(mMx,iPart);
			extTrackRoot->SetMucMomentumY(mMy,iPart);
			extTrackRoot->SetMucMomentumZ(mMz,iPart);

			extTrackRoot->SetMucVolumeName(vMs,iPart);
			extTrackRoot->SetMucVolumeNumber(vMn,iPart);

			extTrackRoot->SetMucPosSigmaAlongZ(zM,iPart);
			extTrackRoot->SetMucPosSigmaAlongT(tM,iPart);
			extTrackRoot->SetMucPosSigmaAlongX(xM,iPart);
			extTrackRoot->SetMucPosSigmaAlongY(yM,iPart);

			extTrackRoot->SetMucErrorMatrix(eMm,iPart);

		}
		recEvt->addExtTrack(extTrackRoot);
	}

	return StatusCode::SUCCESS;
}
#endif
