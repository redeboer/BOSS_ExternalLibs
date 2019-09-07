#ifndef ExtTrackCnv_CXX
#define ExtTrackCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"

#include "DstEvent/DstEvent.h"   //TDS object
#include "DstEvent/DstExtTrack.h"   //TDS object
//#include "ExtEvent/ExtMucHit.h"
#include "RootEventData/TExtTrack.h" // standard root object
#include "RootEventData/TDstEvent.h"

#include "RootCnvSvc/Dst/DstCnv.h"
#include "RootCnvSvc/Dst/ExtTrackCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"

#include "string.h"

using namespace std;
//using CLHEP::HepVector;
using CLHEP::Hep3Vector;
using CLHEP::HepSymMatrix;

// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<ExtTrackCnv> s_factory;
//const ICnvFactory& ExtTrackCnvFactory = s_factory;

ExtTrackCnv::ExtTrackCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "ExtTrackCnv");
    // log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    m_rootBranchname ="m_extTrackCol";
    //declareObject(EventModel::Dst::DstExtTrackCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_extTrackCol);
    m_extTrackCol=0;
}

StatusCode ExtTrackCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

    MsgStream log(msgSvc(), "ExtTrackCnv");
    log << MSG::DEBUG << "ExtTrackCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the ExtTrack Collection
    DstExtTrackCol* extTrackTdsCol = new DstExtTrackCol;
    refpObject=extTrackTdsCol;


    // now convert
    if (!m_extTrackCol) return sc;
    TIter extTrackIter(m_extTrackCol);
    TExtTrack *extTrackRoot = 0;
    while ((extTrackRoot = (TExtTrack*)extTrackIter.Next())) {
          
     int trackId = extTrackRoot->GetTrackId();
  	    //Tof layer1:
     double      p1x = extTrackRoot->GetTof1PositionX();               
     double      p1y = extTrackRoot->GetTof1PositionY();               
     double      p1z = extTrackRoot->GetTof1PositionZ();               
     double      m1x = extTrackRoot->GetTof1MomentumX();
     double      m1y = extTrackRoot->GetTof1MomentumY();
     double      m1z = extTrackRoot->GetTof1MomentumZ();
     Hep3Vector tof1p(p1x,p1y,p1z);
     Hep3Vector tof1m(m1x,m1y,m1z);
	  
     // char * pTof1 = extTrackRoot->GetTof1VolumeName().Data();    
      string v1s = extTrackRoot->GetTof1VolumeName().Data();
      int     v1n = extTrackRoot->GetTof1VolumeNumber() ;
      double tof1= extTrackRoot->GetTof1();
      double tof1path= extTrackRoot->GetTof1Path();
      double z1 = extTrackRoot->GetTof1PosSigmaAlongZ();
      double t1 = extTrackRoot->GetTof1PosSigmaAlongT();
      double x1 = extTrackRoot->GetTof1PosSigmaAlongX(); 
      double y1 = extTrackRoot->GetTof1PosSigmaAlongY(); 
      HepSymMatrix e1m(6) ;
      for (int i = 0;i<6 ;i++ )
                for (int j= 0;j<6 ;j++ )
			e1m[i][j]= extTrackRoot->GetTof1ErrorMatrix(i , j) ;

	  //Tof layer2:
     double      p2x = extTrackRoot->GetTof2PositionX() ;             
     double      p2y = extTrackRoot->GetTof2PositionY() ;             
     double      p2z = extTrackRoot->GetTof2PositionZ() ;             
     double      m2x = extTrackRoot->GetTof2MomentumX() ;
     double      m2y = extTrackRoot->GetTof2MomentumY() ;
     double      m2z = extTrackRoot->GetTof2MomentumZ(); 
     Hep3Vector tof2p(p2x,p2y,p2z);
     Hep3Vector tof2m(m2x,m2y,m2z);
     
      string v2s = extTrackRoot->GetTof2VolumeName().Data();
      int     v2n = extTrackRoot->GetTof2VolumeNumber() ;
      double tof2= extTrackRoot->GetTof2();
      double tof2path = extTrackRoot->GetTof2Path();
      double z2 = extTrackRoot->GetTof2PosSigmaAlongZ() ;
      double t2 = extTrackRoot->GetTof2PosSigmaAlongT(); 
      double x2 = extTrackRoot->GetTof2PosSigmaAlongX(); 
      double y2 = extTrackRoot->GetTof2PosSigmaAlongY(); 
      HepSymMatrix e2m(6) ;
      for (int i = 0;i<6 ;i++ )
                for (int j= 0;j<6 ;j++ )
			e2m[i][j]= extTrackRoot->GetTof2ErrorMatrix(i,j) ;

	  //Emc
     double      pEx = extTrackRoot->GetEmcPositionX();              
     double      pEy = extTrackRoot->GetEmcPositionY();              
     double      pEz = extTrackRoot->GetEmcPositionZ();              
     double      mEx = extTrackRoot->GetEmcMomentumX();
     double      mEy = extTrackRoot->GetEmcMomentumY();
     double      mEz = extTrackRoot->GetEmcMomentumZ();
     Hep3Vector Ep(pEx,pEy,pEz);
     Hep3Vector Em(mEx,mEy,mEz);
     
      string vEs = extTrackRoot->GetEmcVolumeName().Data();    
      int     vEn = extTrackRoot->GetEmcVolumeNumber();
      double theta = extTrackRoot->GetEmcPosSigmaAlongTheta();
      double phi = extTrackRoot->GetEmcPosSigmaAlongPhi();
      HepSymMatrix eEm(6) ;
      for(int i=0;i<6 ;i++ )
                for(int j= 0;j<6 ;j++ )
			eEm[i][j]= extTrackRoot->GetEmcErrorMatrix(i,j);
      double emcPath=extTrackRoot->emcPath();

	 //Muc
     double      pMx = extTrackRoot->GetMucPositionX() ;             
     double      pMy = extTrackRoot->GetMucPositionY() ;             
     double      pMz = extTrackRoot->GetMucPositionZ() ;             
     double      mMx = extTrackRoot->GetMucMomentumX() ;
     double      mMy = extTrackRoot->GetMucMomentumY() ;
     double      mMz = extTrackRoot->GetMucMomentumZ(); 
     Hep3Vector Mp(pMx,pMy,pMz);
     Hep3Vector Mm(mMx,mMy,mMz);
     
      string vMs = extTrackRoot->GetMucVolumeName().Data();     
      int     vMn = extTrackRoot->GetMucVolumeNumber() ;

      double zM = extTrackRoot->GetMucPosSigmaAlongZ() ;
      double tM = extTrackRoot->GetMucPosSigmaAlongT(); 
      double xM = extTrackRoot->GetMucPosSigmaAlongX(); 
      double yM = extTrackRoot->GetMucPosSigmaAlongY(); 
      HepSymMatrix eMm(6) ;
      for (int i = 0;i<6 ;i++ )
                for (int j= 0;j<6 ;j++ )
			eMm[i][j]= extTrackRoot->GetMucErrorMatrix(i,j) ;
 
      DstExtTrack *extTrackTds = new DstExtTrack();
      m_common.m_rootExtTrackMap[extTrackRoot] = extTrackTds;
      
       //ExtMucHitVec
      /*int size = extTrackRoot->GetSize();
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
        double px = extTrackRoot->GetPositionX(i) ;
        double py = extTrackRoot->GetPositionY(i) ;
        double pz = extTrackRoot->GetPositionZ(i) ;
        Hep3Vector p(px,py,pz); 
        double mx = extTrackRoot->GetMomentumX(i) ;
        double my = extTrackRoot->GetMomentumY(i) ;
        double mz = extTrackRoot->GetMomentumZ(i);
        Hep3Vector m(mx,my,mz);
        vs = extTrackRoot->GetVolumeName(i).Data();
        vn = extTrackRoot->GetVolumeNumber(i) ;
        z = extTrackRoot->GetPosSigmaAlongZ(i) ;
        t = extTrackRoot->GetPosSigmaAlongT(i);
        x = extTrackRoot->GetPosSigmaAlongX(i);
        y = extTrackRoot->GetPosSigmaAlongY(i);
        std::vector<double> vecError = extTrackRoot->GetErrorMatrix(i);
        for(int k =0;i<6;k++){
         for(int j =0; j<6;j++){
           matrix[k][j] = vecError[k*6+j];
         }
        }
        aExtMucHit.SetExtMucHit(p,m,vs,vn,matrix,z,t,x,y);
        extTrackTds->AddExtMucHit(aExtMucHit);  
      }*/
         
     // DstExtTrack *extTrackTds = new DstExtTrack();
      extTrackTds->SetTrackId(trackId);
      extTrackTds->SetTof1Data( tof1p, tof1m, v1s, v1n , tof1, tof1path, e1m, z1, t1, x1, y1 );
      extTrackTds->SetTof2Data( tof2p, tof2m, v2s, v2n , tof2, tof2path, e2m, z2, t2, x2, y2 );
      extTrackTds->SetEmcData( Ep, Em, vEs, vEn , theta, phi , eEm);
      extTrackTds->SetEmcPath(emcPath);
      extTrackTds->SetMucData( Mp, Mm, vMs, vMn , eMm, zM, tM, xM, yM );
        
    extTrackTdsCol->push_back(extTrackTds);
    

    //delete extTrackTds; // wensp add 2005/12/31
    // extTrackTds = NULL;	 
     }
    //m_extTrackCol->Delete();  // wensp add 2005/12/30
    delete m_extTrackCol;
    m_extTrackCol = 0;  
   return StatusCode::SUCCESS;
}

StatusCode ExtTrackCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "ExtTrackCnv");
  log << MSG::DEBUG << "ExtTrackCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
 
  DstExtTrackCol * extTrackColTds=dynamic_cast<DstExtTrackCol *> (obj);
  if (!extTrackColTds) {
    log << MSG::ERROR << "Could not downcast to DstExtTrackCol" << endreq;
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
    log << MSG::ERROR << "ExtTrackCnv:Could not downcast to TDS Dst Event" << endreq;
  }
  IOpaqueAddress *addr;

  m_cnvSvc->getDstCnv()->createRep(evt,addr); 
  TDstEvent *recEvt=m_cnvSvc->getDstCnv()->getWriteObject();

  const TObjArray *m_extTrackCol = recEvt->getExtTrackCol();
  
  if (!m_extTrackCol) return sc;
  
  recEvt->clearExtTrackCol(); //necessary in case there is I/O at the same time since array is static
  DstExtTrackCol::const_iterator extTrackTds;

  for (extTrackTds = extTrackColTds->begin(); extTrackTds != extTrackColTds->end(); extTrackTds++) {
	  //Get Data from TDS
     Int_t trackId = (*extTrackTds)->GetTrackId();
		//Tof layer1 
     Double_t  p1x =  (*extTrackTds)->tof1Position().x();               
     Double_t  p1y =  (*extTrackTds)->tof1Position().y();                
     Double_t  p1z =  (*extTrackTds)->tof1Position().z();               
     Double_t  m1x =  (*extTrackTds)->tof1Momentum().x(); 
     Double_t  m1y =  (*extTrackTds)->tof1Momentum().y(); 
     Double_t  m1z =  (*extTrackTds)->tof1Momentum().z(); 
    
      TString v1s = (*extTrackTds)->tof1VolumeName() ;    
      Int_t     v1n = (*extTrackTds)->tof1VolumeNumber() ;
      Double_t tof1= (*extTrackTds)->tof1();
      Double_t tof1p = (*extTrackTds)->tof1Path();
      Double_t z1 = (*extTrackTds)->tof1PosSigmaAlongZ();
      Double_t t1 = (*extTrackTds)->tof1PosSigmaAlongT();
      Double_t x1 = (*extTrackTds)->tof1PosSigmaAlongX(); 
      Double_t y1 = (*extTrackTds)->tof1PosSigmaAlongY(); 
      Double_t e1m[6][6];
      HepSymMatrix e1mTds = (*extTrackTds)->tof1ErrorMatrix();
      for (int i = 0;i<6 ;i++ )
               for (int j= 0;j<6 ;j++ )
			e1m[i][j] = e1mTds[i][j];
      
      //Tof llayer2
     Double_t  p2x =  (*extTrackTds)->tof2Position().x();               
     Double_t  p2y =  (*extTrackTds)->tof2Position().y();                
     Double_t  p2z =  (*extTrackTds)->tof2Position().z();               
     Double_t  m2x =  (*extTrackTds)->tof2Momentum().x(); 
     Double_t  m2y =  (*extTrackTds)->tof2Momentum().y(); 
     Double_t  m2z =  (*extTrackTds)->tof2Momentum().z(); 
    
      TString v2s = (*extTrackTds)->tof2VolumeName() ;    
      Int_t     v2n = (*extTrackTds)->tof2VolumeNumber() ;
      Double_t tof2= (*extTrackTds)->tof2();
      Double_t tof2p = (*extTrackTds)->tof2Path();
      Double_t z2 = (*extTrackTds)->tof2PosSigmaAlongZ();
      Double_t t2 = (*extTrackTds)->tof2PosSigmaAlongT();
      Double_t x2 = (*extTrackTds)->tof2PosSigmaAlongX(); 
      Double_t y2 = (*extTrackTds)->tof2PosSigmaAlongY(); 
      Double_t e2m[6][6];
      HepSymMatrix e2mTds = (*extTrackTds)->tof2ErrorMatrix();
      for (int i = 0;i<6 ;i++ )
                for (int j= 0;j<6 ;j++ )
			e2m[i][j] = e2mTds[i][j]; 
    //Emc
	 Double_t  pEx =  (*extTrackTds)->emcPosition().x();               
     Double_t  pEy =  (*extTrackTds)->emcPosition().y();                
     Double_t  pEz =  (*extTrackTds)->emcPosition().z();               
     Double_t  mEx =  (*extTrackTds)->emcMomentum().x(); 
     Double_t  mEy =  (*extTrackTds)->emcMomentum().y(); 
     Double_t  mEz =  (*extTrackTds)->emcMomentum().z(); 
    
      TString vEs = (*extTrackTds)->emcVolumeName() ;    
      Int_t     vEn = (*extTrackTds)->emcVolumeNumber() ;

      Double_t theta = (*extTrackTds)->emcPosSigmaAlongTheta();
      Double_t phi = (*extTrackTds)->emcPosSigmaAlongPhi();

      Double_t eEm[6][6];
	  HepSymMatrix eEmTds = (*extTrackTds)->emcErrorMatrix();
      for (int i = 0;i<6 ;i++ )
                for (int j= 0;j<6 ;j++ )
			eEm[i][j] = eEmTds[i][j];
      Double_t emcPath=(*extTrackTds)->emcPath();
			
    //Muc
	   Double_t  pMx =  (*extTrackTds)->mucPosition().x();               
     Double_t  pMy =  (*extTrackTds)->mucPosition().y();                
     Double_t  pMz =  (*extTrackTds)->mucPosition().z();               
     Double_t  mMx =  (*extTrackTds)->mucMomentum().x(); 
     Double_t  mMy =  (*extTrackTds)->mucMomentum().y(); 
     Double_t  mMz =  (*extTrackTds)->mucMomentum().z(); 
    
      TString vMs = (*extTrackTds)->mucVolumeName() ;    
      Int_t     vMn = (*extTrackTds)->mucVolumeNumber() ;

      Double_t zM = (*extTrackTds)->mucPosSigmaAlongZ();
      Double_t tM = (*extTrackTds)->mucPosSigmaAlongT();
      Double_t xM = (*extTrackTds)->mucPosSigmaAlongX(); 
      Double_t yM = (*extTrackTds)->mucPosSigmaAlongY(); 

      Double_t eMm[6][6];
	    HepSymMatrix eMmTds = (*extTrackTds)->mucErrorMatrix();
      for (int i = 0;i<6 ;i++ )
                for (int j= 0;j<6 ;j++ )
			eMm[i][j]= eMmTds[i][j];

      //ExtMucHitVec
      /*ExtMucHitVec vecHit = (*extTrackTds)->GetExtMucHitVec();
      Int_t size = vecHit.size();*/

     TExtTrack *extTrackRoot = new TExtTrack();
     //m_common.m_extTrackMap[(*extTrackTds)] = extTrackRoot;

     //Set to Root
 
     //ExtMucHitVec
     /*extTrackRoot->SetSize(size);
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
            m[i][j] = mTds[i][j];
        extTrackRoot->SetExtMucHit(px,py,pz,mx,my,mz,vs,vn,z,t,y,x,m);
     }*/
     
    //Set to Root
     extTrackRoot->SetTrackId(trackId);
	//Tof layer1
		extTrackRoot->SetTof1PositionX(p1x);
		extTrackRoot->SetTof1PositionY(p1y);
		extTrackRoot->SetTof1PositionZ(p1z);
		extTrackRoot->SetTof1MomentumX(m1x);
		extTrackRoot->SetTof1MomentumY(m1y);
		extTrackRoot->SetTof1MomentumZ(m1z);

		extTrackRoot->SetTof1VolumeName(v1s);
		extTrackRoot->SetTof1VolumeNumber(v1n);

		extTrackRoot->SetTof1(tof1);
		extTrackRoot->SetTof1Path(tof1p);

		extTrackRoot->SetTof1PosSigmaAlongZ(z1);
		extTrackRoot->SetTof1PosSigmaAlongT(t1);
		extTrackRoot->SetTof1PosSigmaAlongX(x1);
		extTrackRoot->SetTof1PosSigmaAlongY(y1);

		extTrackRoot->SetTof1ErrorMatrix(e1m);
	//Tof layer2
	    extTrackRoot->SetTof2PositionX(p2x);
		extTrackRoot->SetTof2PositionY(p2y);
		extTrackRoot->SetTof2PositionZ(p2z);
		extTrackRoot->SetTof2MomentumX(m2x);
		extTrackRoot->SetTof2MomentumY(m2y);
		extTrackRoot->SetTof2MomentumZ(m2z);

		extTrackRoot->SetTof2VolumeName(v2s);
		extTrackRoot->SetTof2VolumeNumber(v2n);

		extTrackRoot->SetTof2(tof2);
		extTrackRoot->SetTof2Path(tof2p);

		extTrackRoot->SetTof2PosSigmaAlongZ(z2);
		extTrackRoot->SetTof2PosSigmaAlongT(t2);
		extTrackRoot->SetTof2PosSigmaAlongX(x2);
		extTrackRoot->SetTof2PosSigmaAlongY(y2);

		extTrackRoot->SetTof2ErrorMatrix(e2m);
    //Emc      
	    extTrackRoot->SetEmcPositionX(pEx);
		extTrackRoot->SetEmcPositionY(pEy);
		extTrackRoot->SetEmcPositionZ(pEz);
		extTrackRoot->SetEmcMomentumX(mEx);
		extTrackRoot->SetEmcMomentumY(mEy);
		extTrackRoot->SetEmcMomentumZ(mEz);

		extTrackRoot->SetEmcVolumeName(vEs);
		extTrackRoot->SetEmcVolumeNumber(vEn);

		extTrackRoot->SetEmcPosSigmaAlongTheta(theta);
		extTrackRoot->SetEmcPosSigmaAlongPhi(phi);

		extTrackRoot->SetEmcErrorMatrix(eEm);
		extTrackRoot->SetEmcPath(emcPath);
	//Muc      
	    extTrackRoot->SetMucPositionX(pMx);
		extTrackRoot->SetMucPositionY(pMy);
		extTrackRoot->SetMucPositionZ(pMz);
		extTrackRoot->SetMucMomentumX(mMx);
		extTrackRoot->SetMucMomentumY(mMy);
		extTrackRoot->SetMucMomentumZ(mMz);

		extTrackRoot->SetMucVolumeName(vMs);
		extTrackRoot->SetMucVolumeNumber(vMn);

		extTrackRoot->SetMucPosSigmaAlongZ(zM);
		extTrackRoot->SetMucPosSigmaAlongT(tM);
		extTrackRoot->SetMucPosSigmaAlongX(xM);
		extTrackRoot->SetMucPosSigmaAlongY(yM);

		extTrackRoot->SetMucErrorMatrix(eMm);       
		
	recEvt->addExtTrack(extTrackRoot);
  }

  return StatusCode::SUCCESS;
}
#endif
