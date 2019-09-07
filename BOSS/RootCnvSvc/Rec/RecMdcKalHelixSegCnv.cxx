#ifndef RecMdcKalHelixSegCnv_CXX
#define RecMdcKalHelixSegCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"

#include "ReconEvent/ReconEvent.h"
#include "MdcRecEvent/RecMdcKalHelixSeg.h"
#include "Identifier/Identifier.h"

#include "RootEventData/TRecMdcKalHelixSeg.h" // standard root object
#include "RootEventData/TRecTrackEvent.h"

#include "RootCnvSvc/Rec/RecTrackCnv.h"
#include "RootCnvSvc/Rec/RecMdcKalHelixSegCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include <vector>

using namespace std;


	RecMdcKalHelixSegCnv::RecMdcKalHelixSegCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{

	// Here we associate this converter with the /Event path on the TDS.
	MsgStream log(msgSvc(), "RecMdcKalHelixSegCnv");
	//log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
	//m_rootTreename ="Rec";
	m_rootBranchname ="m_recMdcKalHelixSegCol";
	//declareObject(EventModel::Recon::RecMdcKalHelixSegCol, objType(), m_rootTreename, m_rootBranchname);
	m_adresses.push_back(&m_recMdcKalHelixSegCol);
	m_recMdcKalHelixSegCol=0;
}

StatusCode RecMdcKalHelixSegCnv::TObjectToDataObject(DataObject*& refpObject) {
	// creation of TDS object from root object
	MsgStream log(msgSvc(), "RecMdcKalHelixSegCnv");
	log << MSG::DEBUG << "RecMdcKalHelixSegCnv::TObjectToDataObject" << endreq;

	// create the TDS location for the RecMdcKalHelixSeg Collection
	RecMdcKalHelixSegCol* recMdcKalHelixSegCol = new RecMdcKalHelixSegCol;
	refpObject = recMdcKalHelixSegCol;

	// now convert
	if (!m_recMdcKalHelixSegCol) return StatusCode::SUCCESS;
	TIter mdcHitIter(m_recMdcKalHelixSegCol);
	TRecMdcKalHelixSeg *recMdcKalHelixSegRoot = 0;
	int nSeg=0;
	while ((recMdcKalHelixSegRoot = (TRecMdcKalHelixSeg*)mdcHitIter.Next())) {
		int      trackId = recMdcKalHelixSegRoot->getTrackId();
		int      flagLR = recMdcKalHelixSegRoot->getFlagLR(); 
		Identifier mdcId(recMdcKalHelixSegRoot->getMdcId()); 
		double   tdc = recMdcKalHelixSegRoot->getTdc();
		double   adc = recMdcKalHelixSegRoot->getAdc();
		double   zhit = recMdcKalHelixSegRoot->getZhit();
		double   tof = recMdcKalHelixSegRoot->getTof();
		double   docaIncl = recMdcKalHelixSegRoot->getDocaIncl();
		double   docaExcl = recMdcKalHelixSegRoot->getDocaExcl();
		double   driftDist= recMdcKalHelixSegRoot->getDD();
		double   entra = recMdcKalHelixSegRoot->getEntra();
		double   driftT = recMdcKalHelixSegRoot->getDT();
		//  int      layerId = recMdcKalHelixSegRoot->getLayerId();
		//  double   drIncl = recMdcKalHelixSegRoot->getDrIncl();
		//  double   phi0Incl = recMdcKalHelixSegRoot->getFi0Incl();
		//  double   kappaIncl = recMdcKalHelixSegRoot->getCpaIncl();
		//  double   dzIncl = recMdcKalHelixSegRoot->getDzIncl();
		//  double   tanlIncl = recMdcKalHelixSegRoot->getTanlIncl();
		//  double   drExcl = recMdcKalHelixSegRoot->getDrExcl();
		//  double   phi0Excl = recMdcKalHelixSegRoot->getFi0Excl();
		//  double   kappaExcl = recMdcKalHelixSegRoot->getCpaExcl();
		//  double   dzExcl = recMdcKalHelixSegRoot->getDzExcl();
		//  double   tanlExcl = recMdcKalHelixSegRoot->getTanlExcl();
		double helixIncl[5]; // errIncl[15]; 
		double helixExcl[5]; // errExcl[15];
		for(int i=0, k=0; i<5; i++){
			helixIncl[i] = recMdcKalHelixSegRoot->getHelixIncl(i);
			helixExcl[i] = recMdcKalHelixSegRoot->getHelixExcl(i);
			// for(int j=0; j<=i; j++){
			//         errIncl[k] = recMdcKalHelixSegRoot->getErrorIncl(i,j);
			//         errExcl[k] = recMdcKalHelixSegRoot->getErrorExcl(i,j);
			// }
		}

		RecMdcKalHelixSeg *recMdcKalHelixSeg= new RecMdcKalHelixSeg();
		m_common.m_rootRecMdcKalHelixSegMap[recMdcKalHelixSegRoot] = recMdcKalHelixSeg;

		recMdcKalHelixSeg->setTrackId( trackId );
		recMdcKalHelixSeg->setFlagLR( flagLR );
		recMdcKalHelixSeg->setMdcId( Identifier(mdcId) );
		recMdcKalHelixSeg->setTdc( tdc );
		recMdcKalHelixSeg->setAdc( adc );
		recMdcKalHelixSeg->setZhit( zhit );
		recMdcKalHelixSeg->setTof( tof );
		recMdcKalHelixSeg->setDocaIncl( docaIncl );
		recMdcKalHelixSeg->setDocaExcl( docaExcl );
		recMdcKalHelixSeg->setDD( driftDist );
		recMdcKalHelixSeg->setEntra( entra );
		recMdcKalHelixSeg->setDT( driftT );
		//    recMdcKalHelixSeg->setLayerId( layerId );
		//    recMdcKalHelixSeg->setDrIncl( drIncl );
		//    recMdcKalHelixSeg->setFi0Incl( phi0Incl );
		//    recMdcKalHelixSeg->setCpaIncl( kappaIncl );
		//    recMdcKalHelixSeg->setDzIncl( dzIncl );
		//    recMdcKalHelixSeg->setTanlIncl( tanlIncl );
		//    recMdcKalHelixSeg->setDrExcl( drIncl );
		//    recMdcKalHelixSeg->setFi0Excl( phi0Excl );
		//    recMdcKalHelixSeg->setCpaExcl( kappaExcl );
		//    recMdcKalHelixSeg->setDzExcl( dzExcl );
		//    recMdcKalHelixSeg->setTanlExcl( tanlExcl );
		recMdcKalHelixSeg->setHelixIncl( helixIncl );
		recMdcKalHelixSeg->setHelixExcl( helixExcl );
		//    recMdcKalHelixSeg->setErrorIncl( errIncl );
		//    recMdcKalHelixSeg->setErrorExcl( errExcl );

		recMdcKalHelixSegCol->push_back(recMdcKalHelixSeg);
		nSeg++;
	}
	//cout<<"rec -> Tds, nSeg = "<<nSeg<<", recMdcKalHelixSegCol.size="<<recMdcKalHelixSegCol->size()<<endl;
	return StatusCode::SUCCESS;
}

StatusCode RecMdcKalHelixSegCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

	MsgStream log(msgSvc(), "RecMdcKalHelixSegCnv");
	log << MSG::DEBUG << "RecMdcKalHelixSegCnv::DataObjectToTObject" << endreq;

	RecMdcKalHelixSegCol * mdcHitColTds=dynamic_cast<RecMdcKalHelixSegCol *> (obj);
	if (!mdcHitColTds) {
		log << MSG::ERROR << "Could not downcast to RecMdcKalHelixSegCol" << endreq;
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
		log << MSG::ERROR << "RecMdcKalHelixSegCnv:Could not downcast to TDS ReconEvent" << endreq;
	}

	IOpaqueAddress *addr;

	m_cnvSvc->getRecTrackCnv()->createRep(evt,addr); 
	TRecTrackEvent *recEvt=m_cnvSvc->getRecTrackCnv()->getWriteObject();

	const TObjArray *m_recMdcKalHelixSegCol = recEvt->getRecMdcKalHelixSegCol();
	if (!m_recMdcKalHelixSegCol) return StatusCode::SUCCESS;
	recEvt->clearRecMdcKalHelixSegCol(); //necessary in case there is I/O at the same time since array is static

	RecMdcKalHelixSegCol::const_iterator recMdcKalHelixSeg;
	int nSeg=0;
	for (recMdcKalHelixSeg = mdcHitColTds->begin(); recMdcKalHelixSeg != mdcHitColTds->end(); recMdcKalHelixSeg++) {
		Int_t      trackId =(*recMdcKalHelixSeg)->getTrackId();
		Int_t      flagLR = (*recMdcKalHelixSeg)->getFlagLR();
		UInt_t     mdcId = (*recMdcKalHelixSeg)->getMdcId().get_value();
		Double_t   tdc = (*recMdcKalHelixSeg)->getTdc();
		Double_t   adc = (*recMdcKalHelixSeg)->getAdc();
		Double_t   zhit = (*recMdcKalHelixSeg)->getZhit();
		Double_t   tof = (*recMdcKalHelixSeg)->getTof();
		Double_t   docaIncl = (*recMdcKalHelixSeg)->getDocaIncl();
		Double_t   docaExcl = (*recMdcKalHelixSeg)->getDocaExcl();
		Double_t   driftDist= (*recMdcKalHelixSeg)->getDD();
		Double_t   entra = (*recMdcKalHelixSeg)->getEntra();
		Double_t   driftT =  (*recMdcKalHelixSeg)->getDT();
		//     Int_t      layerId = (*recMdcKalHelixSeg)->getLayerId();
		//     Double_t   drIncl =  (*recMdcKalHelixSeg)->getDrIncl();
		//     Double_t   phi0Incl =(*recMdcKalHelixSeg)->getFi0Incl();
		//     Double_t   kappaIncl = (*recMdcKalHelixSeg)->getCpaIncl();
		//     Double_t   dzIncl =  (*recMdcKalHelixSeg)->getDzIncl();
		//     Double_t   tanlIncl = (*recMdcKalHelixSeg)->getTanlIncl();
		//     Double_t   drExcl =  (*recMdcKalHelixSeg)->getDrExcl();
		//     Double_t   phi0Excl = (*recMdcKalHelixSeg)->getFi0Excl();
		//     Double_t   kappaExcl = (*recMdcKalHelixSeg)->getCpaExcl();
		//     Double_t   dzExcl =   (*recMdcKalHelixSeg)->getDzExcl();
		//     Double_t   tanlExcl = (*recMdcKalHelixSeg)->getTanlExcl();
		HepVector  helixIncl_vct = (*recMdcKalHelixSeg)->getHelixIncl();
		HepVector  helixExcl_vct = (*recMdcKalHelixSeg)->getHelixExcl();
		//     HepSymMatrix  errIncl_mtx = (*recMdcKalHelixSeg)->getErrorIncl();
		//     HepSymMatrix  errExcl_mtx = (*recMdcKalHelixSeg)->getErrorExcl();

		Double_t helixIncl[5], helixExcl[5]; //errIncl[5][5], errExcl[5][5];
		for(Int_t i=0; i<5; i++){
			helixIncl[i] = helixIncl_vct[i];
			helixExcl[i] = helixExcl_vct[i];
		}
		//     for (Int_t i=0; i<5; i++){
		//             for(Int_t j=0; j<5; j++){
		//        	     errIncl[i][j] = errIncl_mtx[i][j];
		//        	     errExcl[i][j] = errExcl_mtx[i][j];
		//             }
		//     }

		TRecMdcKalHelixSeg *recMdcKalHelixSegRoot = new TRecMdcKalHelixSeg();
		//m_common.m_recMdcKalHelixSegMap[(*recMdcKalHelixSeg)] = recMdcKalHelixSegRoot;

		recMdcKalHelixSegRoot->setTrackId( trackId );
		recMdcKalHelixSegRoot->setFlagLR( flagLR );
		recMdcKalHelixSegRoot->setMdcId( Identifier(mdcId) );
		recMdcKalHelixSegRoot->setTdc( tdc );
		recMdcKalHelixSegRoot->setAdc( adc );
		recMdcKalHelixSegRoot->setZhit( zhit );
		recMdcKalHelixSegRoot->setTof( tof );
		recMdcKalHelixSegRoot->setDocaIncl( docaIncl );
		recMdcKalHelixSegRoot->setDocaExcl( docaExcl );
		recMdcKalHelixSegRoot->setDD( driftDist );
		recMdcKalHelixSegRoot->setEntra( entra );
		recMdcKalHelixSegRoot->setDT( driftT );
		//     recMdcKalHelixSegRoot->setLayerId( layerId );
		//     recMdcKalHelixSegRoot->setDrIncl( drIncl );
		//     recMdcKalHelixSegRoot->setFi0Incl( phi0Incl );
		//     recMdcKalHelixSegRoot->setCpaIncl( kappaIncl );
		//     recMdcKalHelixSegRoot->setDzIncl( dzIncl );
		//     recMdcKalHelixSegRoot->setTanlIncl( tanlIncl );
		//     recMdcKalHelixSegRoot->setDrExcl( drIncl );
		//     recMdcKalHelixSegRoot->setFi0Excl( phi0Excl );
		//     recMdcKalHelixSegRoot->setCpaExcl( kappaExcl );
		//     recMdcKalHelixSegRoot->setDzExcl( dzExcl );
		//     recMdcKalHelixSegRoot->setTanlExcl( tanlExcl );
		recMdcKalHelixSegRoot->setHelixIncl( helixIncl );
		recMdcKalHelixSegRoot->setHelixExcl( helixExcl );
		//     recMdcKalHelixSegRoot->setErrorIncl( errIncl );
		//     recMdcKalHelixSegRoot->setErrorExcl( errExcl );

		log<<MSG::INFO<<" recMdcKalHelixSegRoot.dd: "<<recMdcKalHelixSegRoot->getDD()
			<<" recMdcKalHelixSegRoot.entra: "<<recMdcKalHelixSegRoot->getEntra()
			<<endreq;

		recEvt->addRecMdcKalHelixSeg(recMdcKalHelixSegRoot);
		nSeg++;
	}
	//cout<<"writing nSeg = "<<nSeg<<endl;
	return StatusCode::SUCCESS;
}
#endif
