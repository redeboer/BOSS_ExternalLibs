#ifndef RecMdcKalTrackCnv_CXX
#define RecMdcKalTrackCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "TClonesArray.h"
#include "TClonesArray.h"
#include "Identifier/MdcID.h"
#include "Identifier/Identifier.h"

#include "EventModel/EventModel.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcKalHelixSeg.h"
#include "ReconEvent/ReconEvent.h"
//#include "RootEventData/TDstEvent.h"
#include "RootEventData/TRecTrackEvent.h"
#include "RootEventData/TRecMdcKalTrack.h"

#include "RootCnvSvc/Rec/RecTrackCnv.h"
#include "RootCnvSvc/Rec/RecMdcKalTrackCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include <vector>

using namespace std;

// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<RecMdcKalTrackCnv> s_factory;
//const ICnvFactory& RecMdcKalTrackCnvFactory = s_factory;

	RecMdcKalTrackCnv::RecMdcKalTrackCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{

	// Here we associate this converter with the /Event path on the TDS.
	MsgStream log(msgSvc(), "RecMdcKalTrackCnv");
	//log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
	//m_rootTreename ="Rec";
	m_rootBranchname ="m_recMdcKalTrackCol";
	// declareObject(EventModel::Recon::MdcKalTrackCol, objType(), m_rootTreename, m_rootBranchname);
	m_adresses.push_back(&m_recMdcKalTrackCol);
	m_recMdcKalTrackCol = 0;
}

StatusCode RecMdcKalTrackCnv::TObjectToDataObject(DataObject*& refpObject) {
	// creation of TDS object from root object
	MsgStream log(msgSvc(), "RecMdcKalTrackCnv");
	log << MSG::DEBUG << "RecMdcKalTrackCnv::TObjectToDataObject" << endreq;
	StatusCode sc = StatusCode::SUCCESS;

	// create the TDS location for the MdcKalTrack Collection
	RecMdcKalTrackCol* recMdcKalTrackCol = new RecMdcKalTrackCol;
	refpObject = recMdcKalTrackCol;

	// now convert
	if (!m_recMdcKalTrackCol) return sc;
	TIter recMdcKalTrackIter(m_recMdcKalTrackCol);

	IDataProviderSvc* dataSvc = 0;
	sc = serviceLocator()->getService ("EventDataSvc",  
			IDataProviderSvc::interfaceID(), (IInterface*&)dataSvc);
	if (!sc.isSuccess()) {
		log << MSG::FATAL << "Could not get EventDataSvc in RecMdcTrackCnv" << endreq;
		return( StatusCode::FAILURE);
	}
	SmartDataPtr<RecMdcKalHelixSegCol> recMdcKalHelixSegCol(dataSvc,"/Event/Recon/RecMdcKalHelixSegCol");
	if (!recMdcKalHelixSegCol) {
		log << MSG::FATAL << "Could not find RecMdcKalHelixSegCol" << endreq;
		return( StatusCode::FAILURE);
	}

	TRecMdcKalTrack *recMdcKalTrackRoot = 0;
	while ((recMdcKalTrackRoot = (TRecMdcKalTrack*)recMdcKalTrackIter.Next())) {

		std::vector<HepVector> zhelixs;
		std::vector<HepSymMatrix> zerrors;
		std::vector<HepVector> fhelixs;
		std::vector<HepSymMatrix> ferrors;
		std::vector<HepVector> lhelixs;
		std::vector<HepSymMatrix> lerrors;
		std::vector<HepPoint3D> pocas;
		std::vector<HepPoint3D> lpivots;
		std::vector<HepPoint3D> lpoints;


		HepVector zhelix(5); 
		HepSymMatrix zerror(5); 
		HepVector fhelix(5);
		HepSymMatrix ferror(5);
		HepVector lhelix(5);
		HepSymMatrix lerror(5);
		HepVector thelix(5);
		HepSymMatrix terror(5);
		HepPoint3D poca(0,0,0);
		HepPoint3D lpivot(0,0,0); 
		HepPoint3D lpoint(0,0,0); 


		HepVector zhelix_e(5); 
		HepSymMatrix zerror_e(5); 
		HepVector fhelix_e(5);
		HepSymMatrix ferror_e(5); 
		HepVector lhelix_e(5);
		HepSymMatrix lerror_e(5);
		HepPoint3D poca_e(0,0,0);
		HepPoint3D lpivot_e(0,0,0); 
		HepPoint3D lpoint_e(0,0,0); 


		HepVector zhelix_mu(5); 
		HepSymMatrix zerror_mu(5); 
		HepVector fhelix_mu(5);
		HepSymMatrix ferror_mu(5);
		HepVector lhelix_mu(5);
		HepSymMatrix lerror_mu(5);
		HepPoint3D poca_mu(0,0,0);
		HepPoint3D lpivot_mu(0,0,0); 
		HepPoint3D lpoint_mu(0,0,0); 


		HepVector zhelix_k(5); 
		HepSymMatrix zerror_k(5);  
		HepVector fhelix_k(5);
		HepSymMatrix ferror_k(5);
		HepVector lhelix_k(5);
		HepSymMatrix lerror_k(5);
		HepPoint3D poca_k(0,0,0);
		HepPoint3D lpivot_k(0,0,0); 
		HepPoint3D lpoint_k(0,0,0); 


		HepVector zhelix_p(5);  
		HepSymMatrix zerror_p(5);
		HepVector fhelix_p(5);
		HepSymMatrix ferror_p(5);
		HepVector lhelix_p(5);
		HepSymMatrix lerror_p(5);
		HepPoint3D poca_p(0,0,0);
		HepPoint3D lpivot_p(0,0,0); 
		HepPoint3D lpoint_p(0,0,0); 


		int stat[2][5];
		double chisq[2][5];
		int ndf[2][5];

		double  mass[5];
		double  length[5];
		double  tof[5]; 
		int     nhits[5];
		int     nlayer[5];
		int     trackId;
		double  pathSM[5];
		double  fiTerm[5]; 
		//std::cout<<" step 0: "<<std::endl;


		RecMdcKalTrack *recMdcKalTrackTds = new RecMdcKalTrack();

		//std::cout<<" step 1: "<<std::endl;

		trackId = recMdcKalTrackRoot->getTrackId();
		// std::cout<<"track id: "<<trackId<<std::endl;
		recMdcKalTrackTds->setTrackId(trackId);

		for (int u=0; u<5; u++){                     
			mass[u]   = recMdcKalTrackRoot->getMass(u);          
			length[u] = recMdcKalTrackRoot->getLength(u);      
			tof[u]    = recMdcKalTrackRoot->getTof(u);            
			nhits[u]  = recMdcKalTrackRoot->getNhits(u);          
			fiTerm[u]=recMdcKalTrackRoot->getfiTerm(u);
			pathSM[u]=recMdcKalTrackRoot->getPathSM(u); 
			nlayer[u]=recMdcKalTrackRoot->getNlayer(u); 

			recMdcKalTrackTds->setMass(mass[u],u);     
			recMdcKalTrackTds->setLength(length[u],u); 
			recMdcKalTrackTds->setTof(tof[u],u);       
			recMdcKalTrackTds->setNhits(nhits[u],u);     
			recMdcKalTrackTds->setFiTerm(fiTerm[u],u);     
			recMdcKalTrackTds->setPathSM(pathSM[u],u);     
			recMdcKalTrackTds->setNlayer(nlayer[u],u);     
		}    


		for(int v=0; v<3; v++){
			poca_e[v]  = recMdcKalTrackRoot->getPocaE(v);   
			poca_mu[v] = recMdcKalTrackRoot->getPocaMu(v);
			poca[v]    = recMdcKalTrackRoot->getPoca(v);
			poca_k[v]  = recMdcKalTrackRoot->getPocaK(v);
			poca_p[v]  = recMdcKalTrackRoot->getPocaP(v);
			lpivot_e[v]  = recMdcKalTrackRoot->getLPivotE(v);
			lpivot_mu[v] = recMdcKalTrackRoot->getLPivotMu(v);
			lpivot[v]    = recMdcKalTrackRoot->getLPivot(v);
			lpivot_k[v]  = recMdcKalTrackRoot->getLPivotK(v);
			lpivot_p[v]  = recMdcKalTrackRoot->getLPivotP(v);
			lpoint_e[v]  = recMdcKalTrackRoot->getLPointE(v);
			lpoint_mu[v] = recMdcKalTrackRoot->getLPointMu(v);
			lpoint[v]    = recMdcKalTrackRoot->getLPoint(v);
			lpoint_k[v]  = recMdcKalTrackRoot->getLPointK(v);
			lpoint_p[v]  = recMdcKalTrackRoot->getLPointP(v);
			//std::cout<<"poca_e[v]: "<<setprecision(6)<<poca_e[v]<<std::endl;
			//std::cout<<"lpoint_e[v]: "<<setprecision(6)<<lpoint_e[v]<<std::endl;
			//std::cout<<"lpivot_e[v]: "<<setprecision(6)<<lpivot_e[v]<<std::endl;

		}

		for (int i=0, k=0; i<5; i++){
			zhelix[i]    = recMdcKalTrackRoot->getZHelix(i);
			zhelix_e[i]  = recMdcKalTrackRoot->getZHelixE(i);
			zhelix_mu[i] = recMdcKalTrackRoot->getZHelixMu(i);
			zhelix_k[i]  = recMdcKalTrackRoot->getZHelixK(i);
			zhelix_p[i]  = recMdcKalTrackRoot->getZHelixP(i);


			fhelix[i]    = recMdcKalTrackRoot->getFHelix(i);
			fhelix_e[i]  = recMdcKalTrackRoot->getFHelixE(i);
			fhelix_mu[i] = recMdcKalTrackRoot->getFHelixMu(i);
			fhelix_k[i]  = recMdcKalTrackRoot->getFHelixK(i);
			fhelix_p[i]  = recMdcKalTrackRoot->getFHelixP(i);

			lhelix[i]    = recMdcKalTrackRoot->getLHelix(i);
			lhelix_e[i]  = recMdcKalTrackRoot->getLHelixE(i);
			lhelix_mu[i] = recMdcKalTrackRoot->getLHelixMu(i);
			lhelix_k[i]  = recMdcKalTrackRoot->getLHelixK(i);
			lhelix_p[i]  = recMdcKalTrackRoot->getLHelixP(i);

			thelix[i]    = recMdcKalTrackRoot->getTHelix(i);

			for (int j=0; j<=i; j++){

				zerror[i][j]    = recMdcKalTrackRoot->getZError(i,j);
				zerror_e[i][j]  = recMdcKalTrackRoot->getZErrorE(i,j);
				zerror_mu[i][j] = recMdcKalTrackRoot->getZErrorMu(i,j);
				zerror_k[i][j]  = recMdcKalTrackRoot->getZErrorK(i,j);
				zerror_p[i][j]  = recMdcKalTrackRoot->getZErrorP(i,j);
				zerror[j][i]    = zerror[i][j];    
				zerror_e[j][i]  = zerror_e[i][j];  
				zerror_mu[j][i] = zerror_mu[i][j]; 
				zerror_k[j][i]  = zerror_k[i][j];  
				zerror_p[j][i]  = zerror_p[i][j];  

				ferror[i][j]    = recMdcKalTrackRoot->getFError(i,j);
				ferror_e[i][j]  = recMdcKalTrackRoot->getFErrorE(i,j);
				ferror_mu[i][j] = recMdcKalTrackRoot->getFErrorMu(i,j);
				ferror_k[i][j]  = recMdcKalTrackRoot->getFErrorK(i,j);
				ferror_p[i][j]  = recMdcKalTrackRoot->getFErrorP(i,j);
				ferror[j][i]    = ferror[i][j];
				ferror_e[j][i]  = ferror_e[i][j];
				ferror_mu[j][i] = ferror_mu[i][j];
				ferror_k[j][i]  = ferror_k[i][j];
				ferror_p[j][i]  = ferror_p[i][j];

				lerror[i][j]    = recMdcKalTrackRoot->getLError(i,j);
				lerror_e[i][j]  = recMdcKalTrackRoot->getLErrorE(i,j);
				lerror_mu[i][j] = recMdcKalTrackRoot->getLErrorMu(i,j);
				lerror_k[i][j]  = recMdcKalTrackRoot->getLErrorK(i,j);
				lerror_p[i][j]  = recMdcKalTrackRoot->getLErrorP(i,j);
				lerror[j][i]    = lerror[i][j];
				lerror_e[j][i]  = lerror_e[i][j];
				lerror_mu[j][i] = lerror_mu[i][j];
				lerror_k[j][i]  = lerror_k[i][j];
				lerror_p[j][i]  = lerror_p[i][j];

				terror[i][j] = recMdcKalTrackRoot->getTError(k++);
				terror[j][i] = terror[i][j];

			}	
		}

		//std::cout<<" step 2: "<<std::endl;

		//std::cout<<"T to rec tds: "<<endl;
		//std::cout<<"lpoint_e: "<<lpoint_e<<std::endl;
		//std::cout<<"lpivot_e: "<<lpivot_e<<std::endl;
		/// for zero point
		zhelixs.push_back(zhelix_e);
		zhelixs.push_back(zhelix_mu);
		zhelixs.push_back(zhelix);
		zhelixs.push_back(zhelix_k);
		zhelixs.push_back(zhelix_p);	
		zerrors.push_back(zerror_e);
		zerrors.push_back(zerror_mu);
		zerrors.push_back(zerror);
		zerrors.push_back(zerror_k);
		zerrors.push_back(zerror_p);
		/// first hit point
		fhelixs.push_back(fhelix_e);
		fhelixs.push_back(fhelix_mu);
		fhelixs.push_back(fhelix);
		fhelixs.push_back(fhelix_k);
		fhelixs.push_back(fhelix_p);
		ferrors.push_back(ferror_e);
		ferrors.push_back(ferror_mu);
		ferrors.push_back(ferror);
		ferrors.push_back(ferror_k);
		ferrors.push_back(ferror_p);
		/// last hit point
		lhelixs.push_back(lhelix_e);
		lhelixs.push_back(lhelix_mu);
		lhelixs.push_back(lhelix);
		lhelixs.push_back(lhelix_k);
		lhelixs.push_back(lhelix_p); 
		lerrors.push_back(lerror_e);
		lerrors.push_back(lerror_mu);
		lerrors.push_back(lerror);
		lerrors.push_back(lerror_k);
		lerrors.push_back(lerror_p);
		/// track finding helix for dE/dx usage

		pocas.push_back(poca_e);
		pocas.push_back(poca_mu);
		pocas.push_back(poca);
		pocas.push_back(poca_k);
		pocas.push_back(poca_p);
		lpivots.push_back(lpivot_e);
		lpivots.push_back(lpivot_mu);
		lpivots.push_back(lpivot);
		lpivots.push_back(lpivot_k);
		lpivots.push_back(lpivot_p);
		lpoints.push_back(lpoint_e);
		lpoints.push_back(lpoint_mu);
		lpoints.push_back(lpoint);
		lpoints.push_back(lpoint_k);
		lpoints.push_back(lpoint_p);

		for(int m=0; m<5; m++){
			int charge;
			double pxy(0.),px(0.),py(0.),pz(0.),ptot(0.),dr(0.),phi0(0.),kappa(0.),dz(0.),tanl(0.),x(0.),y(0.),z(0.),vx0(0.),vy0(0.),vz0(0.);
			dr    = zhelixs[m][0];
			phi0  = zhelixs[m][1];
			kappa = zhelixs[m][2];
			dz    = zhelixs[m][3];
			tanl  = zhelixs[m][4];

			// x = pocas[m][0];
			// y = pocas[m][1];
			// z = pocas[m][2];
			x = dr * cos(phi0);
			y = dr * sin(phi0);
			z = dz;
			//  std::cout<<"x: "<<x<<"y: "<<y<<"z: "<<z<<std::endl;

			if (kappa > 0.0000000001)
				charge = 1;
			else if (kappa < -0.0000000001)
				charge = -1;
			else
				charge = 0;

			if(kappa!=0) pxy = 1.0/fabs(kappa);
			else pxy = 0;

			px = pxy * (-sin(phi0));
			py = pxy * cos(phi0);
			pz = pxy * tanl;
			ptot = sqrt(px*px+py*py+pz*pz);
			// cout<<"x, y, z, charge, px, py, pz, ptot: "<<x<<" , "<<y<<" , "<<z<<" , "<<charge<<" , "<<px<<" , "<<py<<" , "<<pz<<" , "<<ptot<<endl;
			recMdcKalTrackTds->setCharge(charge,m);
			recMdcKalTrackTds->setPxy(pxy,m);
			recMdcKalTrackTds->setPx(px,m);
			recMdcKalTrackTds->setPy(py,m);
			recMdcKalTrackTds->setPz(pz,m);
			recMdcKalTrackTds->setP(ptot,m);
			recMdcKalTrackTds->setTheta(acos(pz/ptot),m);
			recMdcKalTrackTds->setPhi(atan2(py,px),m);
			recMdcKalTrackTds->setX(x,m);
			recMdcKalTrackTds->setY(y,m);
			recMdcKalTrackTds->setZ(z,m);
			recMdcKalTrackTds->setR(sqrt(x*x+y*y),m);
			/// the next three function need more consideration, temporarily

			for(int n=0; n<2; n++){
				int stat  = recMdcKalTrackRoot->getStat(n,m);
				int chisq = recMdcKalTrackRoot->getChisq(n,m);
				int ndf   = recMdcKalTrackRoot->getNdf(n,m);
				recMdcKalTrackTds->setStat(stat,n,m);
				recMdcKalTrackTds->setChisq(chisq,n,m);
				recMdcKalTrackTds->setNdf(ndf,n,m);
			}
		}

		//std::cout<<" step 3: "<<std::endl;



		for(int jj=0; jj<5; jj++){
			//std::cout<<" step 3.0: "<<std::endl;
			//std::cout<<"zhelixs[jj]: "<<zhelixs[jj]<<std::endl;

			recMdcKalTrackTds->setZHelix(zhelixs[jj],jj);
			//std::cout<<" step 3.1: "<<std::endl;

			recMdcKalTrackTds->setZError(zerrors[jj],jj);
			recMdcKalTrackTds->setFHelix(fhelixs[jj],jj);
			recMdcKalTrackTds->setFError(ferrors[jj],jj);
			//std::cout<<" step 3.5: "<<std::endl;

			recMdcKalTrackTds->setLHelix(lhelixs[jj],jj);
			recMdcKalTrackTds->setLError(lerrors[jj],jj);
			recMdcKalTrackTds->setPoca(pocas[jj],jj);


			recMdcKalTrackTds->setLPoint(lpoints[jj],jj);
			recMdcKalTrackTds->setLPivot(lpivots[jj],jj);
		}
		recMdcKalTrackTds->setTHelix(thelix);
		recMdcKalTrackTds->setTError(terror);

		//std::cout<<" step 4: "<<std::endl;


		log<<MSG::DEBUG<<"T to REC TDS, zhelix: "<<zhelix<<endreq;
		log<<MSG::DEBUG<<"T to REC TDS, zerror: "<<zerror<<endreq;

		m_common.m_rootRecMdcKalTrackMap[recMdcKalTrackRoot] = recMdcKalTrackTds;

		int nSegTot=0;
		for(int i=0; i<5; i++) {
			HelixSegRefVec theKalHelixSegRefVec;

			int nSeg=recMdcKalTrackRoot->getNseg(i);
			int iSeg=0;
			RecMdcKalHelixSegCol::iterator iter = recMdcKalHelixSegCol->begin();
			for (;iter != recMdcKalHelixSegCol->end(); iter++){

				//cout<<" (*iter)->getTrkId(): "<<(*iter)->getTrkId()<<endl;
				if((*iter)->getTrackId() == trackId){
					if(iSeg>=nSegTot&&iSeg<nSeg+nSegTot) {
						SmartRef<RecMdcKalHelixSeg> refhit(*iter);
						theKalHelixSegRefVec.push_back(refhit);
					}
					iSeg++;
				}
			}
			nSegTot+=nSeg;

			recMdcKalTrackTds->setVecHelixSegs(theKalHelixSegRefVec,i);
		}

		int nhelixsegs = recMdcKalTrackTds->getVecHelixSegs().size();

		//std::cout<<" mdc hits: "<<nhelixsegs<< std::endl;

		for(int ii=0; ii <nhelixsegs ; ii++){

			//cout<<"ddl: "<<(recMdcTrack->getVecKalHelixSegs()[ii])->getDriftDistLeft()<<endl;
			//cout<<"erddl: "<<(recMdcTrack->getVecKalHelixSegs()[ii])->getErrDriftDistLeft()<<endl;
			Identifier id(recMdcKalTrackTds->getVecHelixSegs()[ii]->getMdcId());
			int layer = MdcID::layer(id);
			int wire = MdcID::wire(id);
			//cout<<"layer: "<<layer<<" wire: "<<wire<<endl;
		} 

		recMdcKalTrackCol->push_back(recMdcKalTrackTds);

	}

	delete m_recMdcKalTrackCol;

	m_recMdcKalTrackCol = 0;  

	return StatusCode::SUCCESS;
}


StatusCode RecMdcKalTrackCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {


	MsgStream log(msgSvc(), "RecMdcKalTrackCnv");
	log << MSG::DEBUG << "RecMdcKalTrackCnv::DataObjectToTObject" << endreq;
	StatusCode sc=StatusCode::SUCCESS;

	RecMdcKalTrackCol * mdcKalTrackColTds=dynamic_cast<RecMdcKalTrackCol *> (obj);
	if (!mdcKalTrackColTds) {
		log << MSG::ERROR << "Could not downcast to RecMdcKalTrackCol" << endreq;
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
		log << MSG::ERROR << "MdcKalTrackCnv:Could not downcast to TDS Recon Event" << endreq;
	}
	IOpaqueAddress *addr;

	m_cnvSvc->getRecTrackCnv()->createRep(evt,addr); 
	TRecTrackEvent *recEvt=m_cnvSvc->getRecTrackCnv()->getWriteObject();

	const TObjArray *m_recMdcKalTrackCol = recEvt->getRecMdcKalTrackCol();

	if (!m_recMdcKalTrackCol) return sc;

	recEvt->clearRecMdcKalTrackCol(); //necessary in case there is I/O at the same time since array is static
	RecMdcKalTrackCol::const_iterator mdcKalTrackTds;

	for (mdcKalTrackTds = mdcKalTrackColTds->begin(); mdcKalTrackTds != mdcKalTrackColTds->end(); mdcKalTrackTds++) {

		// Get Data from TDS
		// root data
		Int_t trackId;
		Double_t mass[5];          
		Double_t length[5];        
		Double_t tof[5];           
		Int_t nhits[5];           
		Int_t nlayer[5];        
		Int_t stat[2][5];            
		Double_t chisq[2][5];      
		Int_t ndf[2][5];        
		Int_t nSeg[5];
		Double_t fiTerm[5];
		Double_t pathSM[5];

		Double_t poca_e[3];
		Double_t poca_mu[3];
		Double_t poca[3];
		Double_t poca_k[3];
		Double_t poca_p[3];

		Double_t zhelix[5];        
		Double_t zerror[5][5];     
		Double_t zhelix_e[5];      
		Double_t zerror_e[5][5];   
		Double_t zhelix_mu[5];     
		Double_t zerror_mu[5][5];  
		Double_t zhelix_k[5];      
		Double_t zerror_k[5][5];   
		Double_t zhelix_p[5];      
		Double_t zerror_p[5][5];   

		Double_t fhelix[5];        
		Double_t ferror[5][5];        
		Double_t fhelix_e[5];         
		Double_t ferror_e[5][5];      
		Double_t fhelix_mu[5];        
		Double_t ferror_mu[5][5];     
		Double_t fhelix_k[5];         
		Double_t ferror_k[5][5];      
		Double_t fhelix_p[5];         
		Double_t ferror_p[5][5];      

		Double_t lhelix[5];           
		Double_t lerror[5][5];        		
		Double_t lhelix_e[5];            
		Double_t lerror_e[5][5];          
		Double_t lhelix_mu[5];           
		Double_t lerror_mu[5][5];  
		Double_t lhelix_k[5];      
		Double_t lerror_k[5][5];   
		Double_t lhelix_p[5];      
		Double_t lerror_p[5][5];  

		Double_t thelix[5];        
		Double_t terror[15];        

		Double_t lpoint_e[3];
		Double_t lpoint_mu[3];
		Double_t lpoint[3];
		Double_t lpoint_k[3];
		Double_t lpoint_p[3];

		Double_t lpivot_e[3];
		Double_t lpivot_mu[3];
		Double_t lpivot[3];
		Double_t lpivot_k[3];
		Double_t lpivot_p[3];

		TRecMdcKalTrack *mdcKalTrackRoot = new TRecMdcKalTrack();             

		trackId = (*mdcKalTrackTds)->getTrackId();
		for (Int_t i = 0 ; i < 5 ; i++){                     
			mass[i] = (*mdcKalTrackTds)->getMass(i);          
			length[i] = (*mdcKalTrackTds)->getLength(i);      
			tof[i] = (*mdcKalTrackTds)->getTof(i);            
			nhits[i] = (*mdcKalTrackTds)->getNhits(i);         
			nlayer[i] = (*mdcKalTrackTds)->getNlayer(i);         
			fiTerm[i]=(*mdcKalTrackTds)->getFiTerm(i); 
			pathSM[i]=(*mdcKalTrackTds)->getPathSM(i); 
			//stat[i] = (*mdcKalTrackTds)->getStat(i);                
			nSeg[i]=((*mdcKalTrackTds)->getVecHelixSegs(i)).size();
			mdcKalTrackRoot->setNseg(nSeg[i],i);
			mdcKalTrackRoot->setNlayer(nlayer[i],i);         
			for (Int_t j = 0 ; j< 2 ; j++){                    
				stat[j][i] = (*mdcKalTrackTds)->getStat(j,i);
				chisq[j][i] = (*mdcKalTrackTds)->getChisq(j,i);
				ndf[j][i] = (*mdcKalTrackTds)->getNdf(j,i);
				mdcKalTrackRoot->setStat(stat[j][i],j,i);
				mdcKalTrackRoot->setChisq(chisq[j][i],j,i);     
				mdcKalTrackRoot->setNdf(ndf[j][i],j,i);         
			}                                                 
		}    
		for(Int_t i=0;i<5;i++){
			log<<MSG::INFO<<" recMdcKalTrack.helix("<<i<<"): "<<(*mdcKalTrackTds)->getTHelix(i)<<endreq;
			thelix[i] = (*mdcKalTrackTds)->getTHelix(i);
		}
		for(Int_t i=0; i<15; i++){
			terror[i] = (*mdcKalTrackTds)->getTError(i);
		}
		HepPoint3D h_poca_e     = (*mdcKalTrackTds)->getPocaE();
		HepPoint3D h_poca_mu    = (*mdcKalTrackTds)->getPocaMu();
		HepPoint3D h_poca       = (*mdcKalTrackTds)->getPoca();
		HepPoint3D h_poca_k     = (*mdcKalTrackTds)->getPocaK();
		HepPoint3D h_poca_p     = (*mdcKalTrackTds)->getPocaP();
		HepPoint3D h_lpoint_e   = (*mdcKalTrackTds)->getLPointE();
		HepPoint3D h_lpoint_mu  = (*mdcKalTrackTds)->getLPointMu();
		HepPoint3D h_lpoint     = (*mdcKalTrackTds)->getLPoint();
		HepPoint3D h_lpoint_k   = (*mdcKalTrackTds)->getLPointK();
		HepPoint3D h_lpoint_p   = (*mdcKalTrackTds)->getLPointP();
		HepPoint3D h_lpivot_e   = (*mdcKalTrackTds)->getLPivotE();
		HepPoint3D h_lpivot_mu  = (*mdcKalTrackTds)->getLPivotMu();
		HepPoint3D h_lpivot     = (*mdcKalTrackTds)->getLPivot();
		HepPoint3D h_lpivot_k   = (*mdcKalTrackTds)->getLPivotK();
		HepPoint3D h_lpivot_p   = (*mdcKalTrackTds)->getLPivotP();

		//std::cout<<" h_poca_mu: "<<h_poca_mu<<std::endl;
		//std::cout<<" h_poca: "<<h_poca<<std::endl;
		//std::cout<<" h_poca_k: "<<h_poca_k<<std::endl;
		//std::cout<<" h_poca_p: "<<h_poca_p<<std::endl;


		HepVector h_zhelix      = (*mdcKalTrackTds)->getZHelix();           
		HepSymMatrix h_zerror   = (*mdcKalTrackTds)->getZError();           
		HepVector h_zhelix_e    = (*mdcKalTrackTds)->getZHelixE();          
		HepSymMatrix h_zerror_e = (*mdcKalTrackTds)->getZErrorE();          
		HepVector h_zhelix_mu   = (*mdcKalTrackTds)->getZHelixMu();         
		HepSymMatrix h_zerror_mu= (*mdcKalTrackTds)->getZErrorMu();         
		HepVector h_zhelix_k    = (*mdcKalTrackTds)->getZHelixK();          
		HepSymMatrix h_zerror_k = (*mdcKalTrackTds)->getZErrorK();          
		HepVector h_zhelix_p    = (*mdcKalTrackTds)->getZHelixP();          
		HepSymMatrix h_zerror_p = (*mdcKalTrackTds)->getZErrorP();          

		HepVector h_fhelix      = (*mdcKalTrackTds)->getFHelix();    
		HepSymMatrix h_ferror   = (*mdcKalTrackTds)->getFError();    
		HepVector h_fhelix_e    = (*mdcKalTrackTds)->getFHelixE();   
		HepSymMatrix h_ferror_e = (*mdcKalTrackTds)->getFErrorE();   
		HepVector h_fhelix_mu   = (*mdcKalTrackTds)->getFHelixMu();  
		HepSymMatrix h_ferror_mu= (*mdcKalTrackTds)->getFErrorMu();  
		HepVector h_fhelix_k    = (*mdcKalTrackTds)->getFHelixK();   
		HepSymMatrix h_ferror_k = (*mdcKalTrackTds)->getFErrorK();   
		HepVector h_fhelix_p    = (*mdcKalTrackTds)->getFHelixP();   
		HepSymMatrix h_ferror_p = (*mdcKalTrackTds)->getFErrorP();   

		HepVector h_lhelix      = (*mdcKalTrackTds)->getLHelix();    
		HepSymMatrix h_lerror   = (*mdcKalTrackTds)->getLError();    
		HepVector h_lhelix_e    = (*mdcKalTrackTds)->getLHelixE();   
		HepSymMatrix h_lerror_e = (*mdcKalTrackTds)->getLErrorE();   
		HepVector h_lhelix_mu   = (*mdcKalTrackTds)->getLHelixMu();  
		HepSymMatrix h_lerror_mu= (*mdcKalTrackTds)->getLErrorMu();  
		HepVector h_lhelix_k    = (*mdcKalTrackTds)->getLHelixK();   
		HepSymMatrix h_lerror_k = (*mdcKalTrackTds)->getLErrorK();   
		HepVector h_lhelix_p    = (*mdcKalTrackTds)->getLHelixP();   
		HepSymMatrix h_lerror_p = (*mdcKalTrackTds)->getLErrorP();   

		mdcKalTrackRoot->setTrackId(trackId);
		mdcKalTrackRoot->setMass(mass);     
		mdcKalTrackRoot->setLength(length); 
		mdcKalTrackRoot->setTof(tof);       
		mdcKalTrackRoot->setNhits(nhits);     
		mdcKalTrackRoot->setFiTerm(fiTerm);     
		mdcKalTrackRoot->setPathSM(pathSM);     
		// mdcKalTrackRoot->setStat(stat);     

		for(int s=0; s<3; s++){
			poca_e[s]  = h_poca_e[s];
			poca_mu[s] = h_poca_mu[s];
			poca[s]    = h_poca[s];
			poca_k[s]  = h_poca_k[s];
			poca_p[s]  = h_poca_p[s];
			lpoint_e[s]  = h_lpoint_e[s];
			lpoint_mu[s] = h_lpoint_mu[s];
			lpoint[s]    = h_lpoint[s];
			lpoint_k[s]  = h_lpoint_k[s];
			lpoint_p[s]  = h_lpoint_p[s];
			lpivot_e[s]  = h_lpivot_e[s];
			lpivot_mu[s] = h_lpivot_mu[s];
			lpivot[s]    = h_lpivot[s];
			lpivot_k[s]  = h_lpivot_k[s];
			lpivot_p[s]  = h_lpivot_p[s];
		}

		for (int i=0; i<5; i++){
			zhelix[i]    = h_zhelix[i];
			zhelix_e[i]  = h_zhelix_e[i];
			zhelix_mu[i] = h_zhelix_mu[i];
			zhelix_p[i]  = h_zhelix_p[i];
			zhelix_k[i]  = h_zhelix_k[i];

			fhelix[i]    = h_fhelix[i];        
			fhelix_e[i]  = h_fhelix_e[i];    
			fhelix_mu[i] = h_fhelix_mu[i];  
			fhelix_p[i]  = h_fhelix_p[i];    
			fhelix_k[i]  = h_fhelix_k[i]; 

			lhelix[i]    = h_lhelix[i];      
			lhelix_e[i]  = h_lhelix_e[i];  
			lhelix_mu[i] = h_lhelix_mu[i];
			lhelix_p[i]  = h_lhelix_p[i];  
			lhelix_k[i]  = h_lhelix_k[i];  

			for (int j=0; j<=i; j++){
				zerror[i][j]    = h_zerror[i][j];       
				zerror_e[i][j]  = h_zerror_e[i][j];   
				zerror_mu[i][j] = h_zerror_mu[i][j]; 
				zerror_p[i][j]  = h_zerror_p[i][j];   
				zerror_k[i][j]  = h_zerror_k[i][j];   


				ferror[i][j]    = h_ferror[i][j];       
				ferror_e[i][j]  = h_ferror_e[i][j];   
				ferror_mu[i][j] = h_ferror_mu[i][j]; 
				ferror_p[i][j]  = h_ferror_p[i][j];   
				ferror_k[i][j]  = h_ferror_k[i][j];   


				lerror[i][j]    = h_lerror[i][j];       
				lerror_e[i][j]  = h_lerror_e[i][j];   
				lerror_mu[i][j] = h_lerror_mu[i][j]; 
				lerror_p[i][j]  = h_lerror_p[i][j];   
				lerror_k[i][j]  = h_lerror_k[i][j]; 

				zerror[j][i]    = h_zerror[i][j];        
				zerror_e[j][i]  = h_zerror_e[i][j];  
				zerror_mu[j][i] = h_zerror_mu[i][j];
				zerror_p[j][i]  = h_zerror_p[i][j];  
				zerror_k[j][i]  = h_zerror_k[i][j];  


				ferror[j][i]    = h_ferror[i][j];      
				ferror_e[j][i]  = h_ferror_e[i][j];  
				ferror_mu[j][i] = h_ferror_mu[i][j];
				ferror_p[j][i]  = h_ferror_p[i][j];  
				ferror_k[j][i]  = h_ferror_k[i][j];  


				lerror[j][i]    = h_lerror[i][j];      
				lerror_e[j][i]  = h_lerror_e[i][j];  
				lerror_mu[j][i] = h_lerror_mu[i][j];
				lerror_p[j][i]  = h_lerror_p[i][j];  
				lerror_k[j][i]  = h_lerror_k[i][j];  

			}      
		}

		mdcKalTrackRoot->setZHelix(zhelix);     
		mdcKalTrackRoot->setZError(zerror);   
		for(int k=0; k<5; k++){
			log<<MSG::INFO<<" RecMdcKalTrackRoot.ZHelix "<<"["<<k<<"]"
				<<mdcKalTrackRoot->getZHelix(k)<<endreq;
		}
		mdcKalTrackRoot->setZHelixE(zhelix_e);   
		mdcKalTrackRoot->setZErrorE(zerror_e);   
		mdcKalTrackRoot->setZHelixMu(zhelix_mu);  
		mdcKalTrackRoot->setZErrorMu(zerror_mu);  
		mdcKalTrackRoot->setZHelixK(zhelix_k);   		
		mdcKalTrackRoot->setZErrorK(zerror_k);   
		mdcKalTrackRoot->setZHelixP(zhelix_p);   
		mdcKalTrackRoot->setZErrorP(zerror_p);
		mdcKalTrackRoot->setFHelix(fhelix);          
		mdcKalTrackRoot->setFError(ferror);       
		mdcKalTrackRoot->setFHelixE(fhelix_e);    
		mdcKalTrackRoot->setFErrorE(ferror_e);    
		mdcKalTrackRoot->setFHelixMu(fhelix_mu);  
		mdcKalTrackRoot->setFErrorMu(ferror_mu);  
		mdcKalTrackRoot->setFHelixK(fhelix_k);    
		mdcKalTrackRoot->setFErrorK(ferror_k);    
		mdcKalTrackRoot->setFHelixP(fhelix_p);    
		mdcKalTrackRoot->setFErrorP(ferror_p);   

		mdcKalTrackRoot->setLHelix(lhelix);         
		mdcKalTrackRoot->setLError(lerror);        
		mdcKalTrackRoot->setLHelixE(lhelix_e);     
		mdcKalTrackRoot->setLErrorE(lerror_e);     
		mdcKalTrackRoot->setLHelixMu(lhelix_mu);   
		mdcKalTrackRoot->setLErrorMu(lerror_mu);   
		mdcKalTrackRoot->setLHelixK(lhelix_k);     
		mdcKalTrackRoot->setLErrorK(lerror_k);     
		mdcKalTrackRoot->setLHelixP(lhelix_p);     
		mdcKalTrackRoot->setLErrorP(lerror_p);    

		mdcKalTrackRoot->setTHelix(thelix);         
		mdcKalTrackRoot->setTError(terror);        

		mdcKalTrackRoot->setPocaE(poca_e);
		mdcKalTrackRoot->setPocaMu(poca_mu);
		mdcKalTrackRoot->setPoca(poca);
		mdcKalTrackRoot->setPocaK(poca_k);
		mdcKalTrackRoot->setPocaP(poca_p);

		mdcKalTrackRoot->setLPointE(lpoint_e);
		mdcKalTrackRoot->setLPointMu(lpoint_mu);
		mdcKalTrackRoot->setLPoint(lpoint);
		mdcKalTrackRoot->setLPointK(lpoint_k);
		mdcKalTrackRoot->setLPointP(lpoint_p);

		mdcKalTrackRoot->setLPivotE(lpivot_e);
		mdcKalTrackRoot->setLPivotMu(lpivot_mu);
		mdcKalTrackRoot->setLPivot(lpivot);
		mdcKalTrackRoot->setLPivotK(lpivot_k);
		mdcKalTrackRoot->setLPivotP(lpivot_p);
		//std::cout<<" mdcKalTrackRoot->getPivotE(1): "<<mdcKalTrackRoot->getLPivotE(1)<<std::endl;

		recEvt->addRecMdcKalTrack(mdcKalTrackRoot);
	}


	return StatusCode::SUCCESS;
}
#endif
