#ifndef MdcKalTrackCnv_CXX
#define MdcKalTrackCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"
#include "DstEvent/DstEvent.h"   //TDS object
#include "DstEvent/DstMdcKalTrack.h"   //TDS object
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "RootEventData/TMdcKalTrack.h" // standard root object
#include "RootEventData/TDstEvent.h"

#include "RootCnvSvc/Dst/DstCnv.h"
#include "RootCnvSvc/Dst/MdcKalTrackCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Point3D.h"
#include <vector>
#include "string.h"
#include <cmath>

using CLHEP::HepVector;
using CLHEP::HepSymMatrix;


MdcKalTrackCnv::MdcKalTrackCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "MdcKalTrackCnv");
    m_rootBranchname ="m_mdcKalTrackCol";
    m_adresses.push_back(&m_mdcKalTrackCol);
    m_mdcKalTrackCol=0;
}

StatusCode MdcKalTrackCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

    MsgStream log(msgSvc(), "MdcKalTrackCnv");
    log << MSG::DEBUG << "MdcKalTrackCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the MdcKalTrack Collection
    DstMdcKalTrackCol* mdcKalTrackTdsCol = new DstMdcKalTrackCol;
    refpObject=mdcKalTrackTdsCol;

    // now convert
    if (!m_mdcKalTrackCol) return sc;
    TIter mdcKalTrackIter(m_mdcKalTrackCol);
    TMdcKalTrack *mdcKalTrackRoot = 0;
    while ((mdcKalTrackRoot = (TMdcKalTrack*)mdcKalTrackIter.Next())) {
     std::vector<HepVector> zhelixs;
     std::vector<HepSymMatrix> zerrors;
     std::vector<HepPoint3D> pocas;
     std::vector<HepVector> fhelixs;
     std::vector<HepSymMatrix> ferrors;

     HepVector zhelix(5); 
     HepSymMatrix zerror(5); 
     HepPoint3D poca(0,0,0);

     HepVector zhelix_e(5); 
     HepSymMatrix zerror_e(5); 
     HepPoint3D poca_e(0,0,0);
  
     HepVector zhelix_mu(5); 
     HepSymMatrix zerror_mu(5); 
     HepPoint3D poca_mu(0,0,0);
      
     HepVector zhelix_k(5); 
     HepSymMatrix zerror_k(5);  
     HepPoint3D poca_k(0,0,0);
       
     HepVector zhelix_p(5);  
     HepSymMatrix zerror_p(5);
     HepPoint3D poca_p(0,0,0);
  
     HepVector fhelix(5); 
     HepSymMatrix ferror(5); 

     HepVector fhelix_e(5); 
     HepSymMatrix ferror_e(5); 
  
     HepVector fhelix_mu(5); 
     HepSymMatrix ferror_mu(5); 
      
     HepVector fhelix_k(5); 
     HepSymMatrix ferror_k(5);  
       
     HepVector fhelix_p(5);  
     HepSymMatrix ferror_p(5);
  
     
     DstMdcKalTrack *mdcKalTrackTds = new DstMdcKalTrack();
     m_common.m_rootMdcKalTrackMap[mdcKalTrackRoot] = mdcKalTrackTds;
     
     /* 
     for(int ii=0; ii<3; ii++){
       poca_e[ii]  =  mdcKalTrackRoot->getPocaE(ii);
       poca_mu[ii] =  mdcKalTrackRoot->getPocaMu(ii);
       poca[ii]    =  mdcKalTrackRoot->getPoca(ii);
       poca_k[ii]  =  mdcKalTrackRoot->getPocaK(ii);
       poca_p[ii]  =  mdcKalTrackRoot->getPocaP(ii);
     }
     */

     log<<MSG::DEBUG<<"T to DST, poca: "<<poca<<endreq;
     
     for (int i=0; i<5; i++){
     	 zhelix[i]    = mdcKalTrackRoot->getZHelix(i);
     	 zhelix_e[i]  = mdcKalTrackRoot->getZHelixE(i);
     	 zhelix_mu[i] = mdcKalTrackRoot->getZHelixMu(i);
     	 zhelix_k[i]  = mdcKalTrackRoot->getZHelixK(i);
     	 zhelix_p[i]  = mdcKalTrackRoot->getZHelixP(i);
     	 
     	 fhelix[i]    = mdcKalTrackRoot->getFHelix(i);
     	 fhelix_e[i]  = mdcKalTrackRoot->getFHelixE(i);
     	 fhelix_mu[i] = mdcKalTrackRoot->getFHelixMu(i);
     	 fhelix_k[i]  = mdcKalTrackRoot->getFHelixK(i);
     	 fhelix_p[i]  = mdcKalTrackRoot->getFHelixP(i);

     	 for (int j=0; j<=i; j++){
     	  zerror[i][j]    = mdcKalTrackRoot->getZError(i,j);
     	  zerror_e[i][j]  = mdcKalTrackRoot->getZErrorE(i,j);
     	  zerror_mu[i][j] = mdcKalTrackRoot->getZErrorMu(i,j);
     	  zerror_k[i][j]  = mdcKalTrackRoot->getZErrorK(i,j);
     	  zerror_p[i][j]  = mdcKalTrackRoot->getZErrorP(i,j);
     	  
     	  zerror[j][i]    = zerror[i][j];    
     	  zerror_e[j][i]  = zerror_e[i][j];  
     	  zerror_mu[j][i] = zerror_mu[i][j]; 
     	  zerror_k[j][i]  = zerror_k[i][j];  
     	  zerror_p[j][i]  = zerror_p[i][j];  

     	  ferror[i][j]    = mdcKalTrackRoot->getFError(i,j);
     	  ferror_e[i][j]  = mdcKalTrackRoot->getFErrorE(i,j);
     	  ferror_mu[i][j] = mdcKalTrackRoot->getFErrorMu(i,j);
     	  ferror_k[i][j]  = mdcKalTrackRoot->getFErrorK(i,j);
     	  ferror_p[i][j]  = mdcKalTrackRoot->getFErrorP(i,j);
     	  
     	  ferror[j][i]    = ferror[i][j];    
     	  ferror_e[j][i]  = ferror_e[i][j];  
     	  ferror_mu[j][i] = ferror_mu[i][j]; 
     	  ferror_k[j][i]  = ferror_k[i][j];  
     	  ferror_p[j][i]  = ferror_p[i][j];  
        }
      }
	
     
     double dr    = zhelix[0];
     double phi0  = zhelix[1];
     double kappa = zhelix[2];
     double dz    = zhelix[3];
     double tanl  = zhelix[4];
     poca[0]=dr*cos(phi0);
     poca[1]=dr*sin(phi0);
     poca[2]=dz;

     dr    = zhelix_e[0];
     phi0  = zhelix_e[1];
     kappa = zhelix_e[2];
     dz    = zhelix_e[3];
     tanl  = zhelix_e[4];
     poca_e[0]=dr*cos(phi0);
     poca_e[1]=dr*sin(phi0);
     poca_e[2]=dz;

     dr    = zhelix_mu[0];
     phi0  = zhelix_mu[1];
     kappa = zhelix_mu[2];
     dz    = zhelix_mu[3];
     tanl  = zhelix_mu[4];
     poca_mu[0]=dr*cos(phi0);
     poca_mu[1]=dr*sin(phi0);
     poca_mu[2]=dz;

     dr    = zhelix_k[0];
     phi0  = zhelix_k[1];
     kappa = zhelix_k[2];
     dz    = zhelix_k[3];
     tanl  = zhelix_k[4];
     poca_k[0]=dr*cos(phi0);
     poca_k[1]=dr*sin(phi0);
     poca_k[2]=dz;

     dr    = zhelix_p[0];
     phi0  = zhelix_p[1];
     kappa = zhelix_p[2];
     dz    = zhelix_p[3];
     tanl  = zhelix_p[4];
     poca_p[0]=dr*cos(phi0);
     poca_p[1]=dr*sin(phi0);
     poca_p[2]=dz;

     
     log<<MSG::DEBUG<<"T to DST, zhelix: "<<zhelix<<endreq;
     log<<MSG::DEBUG<<"T to DST, zerror: "<<zerror<<endreq;

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

     pocas.push_back(poca_e);
     pocas.push_back(poca_mu);
     pocas.push_back(poca);
     pocas.push_back(poca_k);
     pocas.push_back(poca_p);
     
     for(int m=0; m<5; m++){
       int charge;
       double pxy(0.),px(0.),py(0.),pz(0.),ptot(0.),dr(0.),phi0(0.),kappa(0.),dz(0.),tanl(0.),x(0.),y(0.),z(0.),vx0(0.),vy0(0.),vz0(0.);
       dr    = zhelixs[m][0];
       phi0  = zhelixs[m][1];
       kappa = zhelixs[m][2];
       dz    = zhelixs[m][3];
       tanl  = zhelixs[m][4];
       
//       x = pocas[m][0];
//       y = pocas[m][1];
//       z = pocas[m][2];
       
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
       
       mdcKalTrackTds->setCharge(charge,m);
       mdcKalTrackTds->setPxy(pxy,m);
       mdcKalTrackTds->setPx(px,m);
       mdcKalTrackTds->setPy(py,m);
       mdcKalTrackTds->setPz(pz,m);
       mdcKalTrackTds->setP(ptot,m);
       mdcKalTrackTds->setTheta(acos(pz/ptot),m);
       mdcKalTrackTds->setPhi(atan2(py,px),m);
       mdcKalTrackTds->setX(x,m);
       mdcKalTrackTds->setY(y,m);
       mdcKalTrackTds->setZ(z,m);
       mdcKalTrackTds->setR(sqrt(x*x+y*y),m);
  
       mdcKalTrackTds->setStat(mdcKalTrackRoot->getStat(m),0,m);
       mdcKalTrackTds->setStat(mdcKalTrackRoot->getStat2(m),1,m);
       mdcKalTrackTds->setChisq(mdcKalTrackRoot->getChisq(m),0,m);
       mdcKalTrackTds->setNdf(mdcKalTrackRoot->getNdf(m),0,m);
       mdcKalTrackTds->setNlayer(mdcKalTrackRoot->getNlayer(m),m);
     }  
     
      mdcKalTrackTds->setTrackId(mdcKalTrackRoot->getTrackId());

      for(int jj=0; jj<5; jj++){
	mdcKalTrackTds->setZHelix(zhelixs[jj],jj);          
	mdcKalTrackTds->setZError(zerrors[jj],jj);          
	mdcKalTrackTds->setFHelix(fhelixs[jj],jj);          
	mdcKalTrackTds->setFError(ferrors[jj],jj);          
	mdcKalTrackTds->setPoca(pocas[jj],jj);
      }
    
      mdcKalTrackTdsCol->push_back(mdcKalTrackTds);
    }
    // m_mdcKalTrackCol->Delete();  // wensp add 2005/12/30
    delete m_mdcKalTrackCol;
    m_mdcKalTrackCol = 0;  
    return StatusCode::SUCCESS;
}

StatusCode MdcKalTrackCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "MdcKalTrackCnv");
  log << MSG::DEBUG << "MdcKalTrackCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
 
  DstMdcKalTrackCol * mdcKalTrackColTds=dynamic_cast<DstMdcKalTrackCol *> (obj);
  //RecMdcKalTrackCol * mdcKalTrackColTds=dynamic_cast<RecMdcKalTrackCol *> (obj);
  if (!mdcKalTrackColTds) {
    log << MSG::ERROR << "Could not downcast to DstMdcKalTrackCol" << endreq;
    return StatusCode::FAILURE;
  }
      
  DataObject *evt;
  m_eds->findObject(EventModel::Dst::Event,evt);
  if (evt==NULL) {
    log << MSG::ERROR << "Could not get ReconEvent in TDS "  << endreq;
    return StatusCode::FAILURE;
  }

  DstEvent * devtTds=dynamic_cast<DstEvent *> (evt);
  if (!devtTds) {
    log << MSG::ERROR << "MdcKalTrackCnv:Could not downcast to TDS Dst Event" << endreq;
  }
  IOpaqueAddress *addr;

  m_cnvSvc->getDstCnv()->createRep(evt,addr); 
  TDstEvent *recEvt=m_cnvSvc->getDstCnv()->getWriteObject();

  const TObjArray *m_mdcKalTrackCol = recEvt->getMdcKalTrackCol();
  
  if (!m_mdcKalTrackCol) return sc;
  
  recEvt->clearMdcKalTrackCol(); //necessary in case there is I/O at the same time since array is static
  DstMdcKalTrackCol::const_iterator mdcKalTrackTds;

  for (mdcKalTrackTds = mdcKalTrackColTds->begin(); mdcKalTrackTds != mdcKalTrackColTds->end(); mdcKalTrackTds++) {
	  
    // Get Data from DST

     //Double_t poca[3]; 
     Double_t zhelix[5];        
     Double_t zerror[5][5];     

     //Double_t poca_e[3];
     Double_t zhelix_e[5];      
     Double_t zerror_e[5][5];   

     //Double_t poca_mu[3];
     Double_t zhelix_mu[5];     
     Double_t zerror_mu[5][5];  

     //Double_t poca_k[3];
     Double_t zhelix_k[5];      
     Double_t zerror_k[5][5];   

     //Double_t poca_p[3];
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
    
     TMdcKalTrack *mdcKalTrackRoot = new TMdcKalTrack();             
     //m_common.m_mdcKalTrackMap[(*mdcKalTrackTds)] = mdcKalTrackRoot;
     RecMdcKalTrack* recMdcKalTrackTds=(RecMdcKalTrack*) (*mdcKalTrackTds);
   
     for(int k=0; k<5; k++){
      mdcKalTrackRoot->setStat((*mdcKalTrackTds)->getStat(k),k);
      mdcKalTrackRoot->setStat2(recMdcKalTrackTds->getStat(1,k),k);
      mdcKalTrackRoot->setChisq((*mdcKalTrackTds)->getChisq(k),k);
      mdcKalTrackRoot->setNdf((*mdcKalTrackTds)->getNdf(k),k);
      mdcKalTrackRoot->setNlayer((*mdcKalTrackTds)->getNlayer(k),k);
      //o mdcKalTrackRoot->setNster((*mdcKalTrackTds)->getNster(k),k);
      //o mdcKalTrackRoot->setFirstLayer((*mdcKalTrackTds)->getFirstLayer(k),k);
      //o mdcKalTrackRoot->setLastLayer((*mdcKalTrackTds)->getLastLayer(k),k);
     }
     
     //HepPoint3D h_poca        = (*mdcKalTrackTds)->getPoca(2);
     HepVector h_zhelix       = (*mdcKalTrackTds)->getZHelix(2);           
     HepSymMatrix h_zerror    = (*mdcKalTrackTds)->getZError(2);           

     //HepPoint3D h_poca_e      = (*mdcKalTrackTds)->getPoca(0); 
     HepVector h_zhelix_e     = (*mdcKalTrackTds)->getZHelix(0);          
     HepSymMatrix h_zerror_e  = (*mdcKalTrackTds)->getZError(0);          

     //HepPoint3D h_poca_mu     = (*mdcKalTrackTds)->getPoca(1);
     HepVector h_zhelix_mu    = (*mdcKalTrackTds)->getZHelix(1);         
     HepSymMatrix h_zerror_mu = (*mdcKalTrackTds)->getZError(1);         

     //HepPoint3D h_poca_k      = (*mdcKalTrackTds)->getPoca(3);
     HepVector h_zhelix_k     = (*mdcKalTrackTds)->getZHelix(3);          
     HepSymMatrix h_zerror_k  = (*mdcKalTrackTds)->getZError(3);          

     //HepPoint3D h_poca_p      = (*mdcKalTrackTds)->getPoca(4);
     HepVector h_zhelix_p     = (*mdcKalTrackTds)->getZHelix(4);          
     HepSymMatrix h_zerror_p  = (*mdcKalTrackTds)->getZError(4);          
                                
     HepVector h_fhelix       = (*mdcKalTrackTds)->getFHelix(2);           
     HepSymMatrix h_ferror    = (*mdcKalTrackTds)->getFError(2);           
     HepVector h_fhelix_e     = (*mdcKalTrackTds)->getFHelix(0);          
     HepSymMatrix h_ferror_e  = (*mdcKalTrackTds)->getFError(0);          
     HepVector h_fhelix_mu    = (*mdcKalTrackTds)->getFHelix(1);         
     HepSymMatrix h_ferror_mu = (*mdcKalTrackTds)->getFError(1);         
     HepVector h_fhelix_k     = (*mdcKalTrackTds)->getFHelix(3);          
     HepSymMatrix h_ferror_k  = (*mdcKalTrackTds)->getFError(3);          
     HepVector h_fhelix_p     = (*mdcKalTrackTds)->getFHelix(4);          
     HepSymMatrix h_ferror_p  = (*mdcKalTrackTds)->getFError(4);          
                                
     mdcKalTrackRoot->setTrackId((*mdcKalTrackTds)->getTrackId());

     /*
     for(int k=0; k<3; k++){
       poca[k] = h_poca[k];
       poca_e[k] = h_poca_e[k];
       poca_mu[k] = h_poca_mu[k];
       poca_k[k] = h_poca_k[k];
       poca_p[k] = h_poca_p[k];
     }
     */
     
     for (int i=0; i<5; i++){
       zhelix[i] = h_zhelix[i];
       zhelix_e[i] = h_zhelix_e[i];
       zhelix_mu[i] = h_zhelix_mu[i];
       zhelix_p[i] = h_zhelix_p[i];
       zhelix_k[i] = h_zhelix_k[i];
       fhelix[i] = h_fhelix[i];
       fhelix_e[i] = h_fhelix_e[i];
       fhelix_mu[i] = h_fhelix_mu[i];
       fhelix_p[i] = h_fhelix_p[i];
       fhelix_k[i] = h_fhelix_k[i];
       for (int j=0; j<=i; j++){
	 zerror[i][j] = h_zerror[i][j];       
	 zerror_e[i][j] = h_zerror_e[i][j];   
	 zerror_mu[i][j] = h_zerror_mu[i][j]; 
	 zerror_p[i][j] = h_zerror_p[i][j];   
	 zerror_k[i][j] = h_zerror_k[i][j];   
	 zerror[j][i] = h_zerror[i][j];        
	 zerror_e[j][i] = h_zerror_e[i][j];  
	 zerror_mu[j][i] = h_zerror_mu[i][j];
	 zerror_p[j][i] = h_zerror_p[i][j];  
	 zerror_k[j][i] = h_zerror_k[i][j];  
	 ferror[i][j] = h_ferror[i][j];       
	 ferror_e[i][j] = h_ferror_e[i][j];   
	 ferror_mu[i][j] = h_ferror_mu[i][j]; 
	 ferror_p[i][j] = h_ferror_p[i][j];   
	 ferror_k[i][j] = h_ferror_k[i][j];   
	 ferror[j][i] = h_ferror[i][j];        
	 ferror_e[j][i] = h_ferror_e[i][j];  
	 ferror_mu[j][i] = h_ferror_mu[i][j];
	 ferror_p[j][i] = h_ferror_p[i][j];  
	 ferror_k[j][i] = h_ferror_k[i][j];  
       }      
     }

     //mdcKalTrackRoot->setPoca(poca);
     mdcKalTrackRoot->setZHelix(zhelix);     
     mdcKalTrackRoot->setZError(zerror);     

     //mdcKalTrackRoot->setPocaE(poca_e);
     mdcKalTrackRoot->setZHelixE(zhelix_e);   
     mdcKalTrackRoot->setZErrorE(zerror_e);   

     //mdcKalTrackRoot->setPocaMu(poca_mu);
     mdcKalTrackRoot->setZHelixMu(zhelix_mu);  
     mdcKalTrackRoot->setZErrorMu(zerror_mu);  

     //mdcKalTrackRoot->setPocaK(poca_k);
     mdcKalTrackRoot->setZHelixK(zhelix_k);   
     mdcKalTrackRoot->setZErrorK(zerror_k);   

     //mdcKalTrackRoot->setPocaP(poca_p);
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

     recEvt->addMdcKalTrack(mdcKalTrackRoot);
  }

  return StatusCode::SUCCESS;
}
#endif
