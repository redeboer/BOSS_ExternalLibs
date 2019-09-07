#ifndef TofTrackCnv_CXX
#define TofTrackCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"

#include "DstEvent/DstEvent.h"   //TDS object
#include "DstEvent/DstTofTrack.h"   //TDS object
#include "DstEvent/TofHitStatus.h"
#include "RootEventData/TTofTrack.h" // standard root object
#include "RootEventData/TDstEvent.h"

#include "RootCnvSvc/Dst/DstCnv.h"
#include "RootCnvSvc/Dst/TofTrackCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include <vector>

using namespace std;

// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<TofTrackCnv> s_factory;
//const ICnvFactory& TofTrackCnvFactory = s_factory;

TofTrackCnv::TofTrackCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "TofTrackCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    m_rootBranchname ="m_tofTrackCol";
    //declareObject(EventModel::Dst::DstTofTrackCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_tofTrackCol);
    m_tofTrackCol=0;
}

StatusCode TofTrackCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

    MsgStream log(msgSvc(), "TofTrackCnv");
    log << MSG::DEBUG << "TofTrackCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;
    // create the TDS location for the TofTrack Collection
    DstTofTrackCol* tofTrackTdsCol = new DstTofTrackCol;
    refpObject=tofTrackTdsCol;


    // now convert
    if (!m_tofTrackCol) return sc;
    TIter tofTrackIter(m_tofTrackCol);
    TTofTrack *tofTrackRoot = 0;
    while ((tofTrackRoot = (TTofTrack*)tofTrackIter.Next())) {

      int tofTrackID = tofTrackRoot->tofTrackID();
      int trackID    = tofTrackRoot->trackID();
      int tofID      = tofTrackRoot->tofID();
      unsigned int status = tofTrackRoot->status();
      double path    = tofTrackRoot->path();
      double zrhit   = tofTrackRoot->zrhit();
      double ph      = tofTrackRoot->ph();
      double tof     = tofTrackRoot->tof();
      double errtof  = tofTrackRoot->errtof();
      double beta    = tofTrackRoot->beta();
      double texpElectron = tofTrackRoot->texpElectron();
      double texpMuon     = tofTrackRoot->texpMuon();
      double texpPion     = tofTrackRoot->texpPion();
      double texpKaon     = tofTrackRoot->texpKaon();
      double texpProton   = tofTrackRoot->texpProton();
      double toffsetElectron = tofTrackRoot->toffsetElectron();
      double toffsetMuon     = tofTrackRoot->toffsetMuon();
      double toffsetPion     = tofTrackRoot->toffsetPion();
      double toffsetKaon     = tofTrackRoot->toffsetKaon();
      double toffsetProton   = tofTrackRoot->toffsetProton();
      double toffsetAntiProton   = tofTrackRoot->toffsetAntiProton();
      double sigmaElectron = tofTrackRoot->sigmaElectron();
      double sigmaMuon     = tofTrackRoot->sigmaMuon();
      double sigmaPion     = tofTrackRoot->sigmaPion();
      double sigmaKaon     = tofTrackRoot->sigmaKaon();
      double sigmaProton   = tofTrackRoot->sigmaProton();
      double sigmaAntiProton   = tofTrackRoot->sigmaAntiProton();
      int quality    = tofTrackRoot->quality();
      double t0      = tofTrackRoot->t0();
      double errt0   = tofTrackRoot->errt0();
      double errz    = tofTrackRoot->errz();
      double phi     = tofTrackRoot->phi();
      double errphi  = tofTrackRoot->errphi();
      double energy    = tofTrackRoot->energy();
      double errenergy = tofTrackRoot->errenergy();

      DstTofTrack *tofTrackTds = new DstTofTrack();
      m_common.m_rootTofTrackMap[tofTrackRoot] = tofTrackTds;
 
      tofTrackTds->setTofTrackID(tofTrackID);
      tofTrackTds->setTrackID(trackID);
      tofTrackTds->setTofID(tofID);
      tofTrackTds->setStatus(status);
      tofTrackTds->setPath(path);
      tofTrackTds->setZrHit(zrhit);
      tofTrackTds->setPh(ph);
      tofTrackTds->setTof(tof);
      tofTrackTds->setErrTof(errtof);
      tofTrackTds->setBeta(beta);
      tofTrackTds->setTexpElectron(texpElectron);
      tofTrackTds->setTexpMuon(texpMuon);
      tofTrackTds->setTexpPion(texpPion);
      tofTrackTds->setTexpKaon(texpKaon);
      tofTrackTds->setTexpProton(texpProton);
      tofTrackTds->setToffsetElectron(toffsetElectron);
      tofTrackTds->setToffsetMuon(toffsetMuon);
      tofTrackTds->setToffsetPion(toffsetPion);
      tofTrackTds->setToffsetKaon(toffsetKaon);
      tofTrackTds->setToffsetProton(toffsetProton);
      tofTrackTds->setToffsetAntiProton(toffsetAntiProton);
      tofTrackTds->setSigmaElectron(sigmaElectron);
      tofTrackTds->setSigmaMuon(sigmaMuon);
      tofTrackTds->setSigmaPion(sigmaPion);
      tofTrackTds->setSigmaKaon(sigmaKaon);
      tofTrackTds->setSigmaProton(sigmaProton);
      tofTrackTds->setSigmaAntiProton(sigmaAntiProton);
      tofTrackTds->setQuality(quality);
      tofTrackTds->setT0(t0);
      tofTrackTds->setErrT0(errt0);
      tofTrackTds->setErrZ(errz);
      tofTrackTds->setPhi(phi);
      tofTrackTds->setErrPhi(errphi);
      tofTrackTds->setEnergy(energy);
      tofTrackTds->setErrEnergy(errenergy);
     
      tofTrackTdsCol->push_back(tofTrackTds);

     //     delete tofTrackTds; // wensp add 2005/12/31
     //     tofTrackTds = NULL;	 
     }

    //m_tofTrackCol->Delete();  // wensp add 2005/12/30
    delete m_tofTrackCol;
    m_tofTrackCol = 0; 
    return StatusCode::SUCCESS;
}

StatusCode TofTrackCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "TofTrackCnv");
  log << MSG::DEBUG << "TofTrackCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
 
  DstTofTrackCol * tofTrackColTds=dynamic_cast<DstTofTrackCol *> (obj);
  if (!tofTrackColTds) {
    log << MSG::ERROR << "Could not downcast to DstTofTrackCol" << endreq;
    return StatusCode::FAILURE;
  }
 
  DataObject *evt;
  m_eds->findObject(EventModel::Dst::Event,evt);
  if (evt==NULL) {
    log << MSG::ERROR << "Could not get DstEvent in TDS "  << endreq;
    return StatusCode::FAILURE;
  }
  
  DstEvent * devtTds=dynamic_cast<DstEvent *> (evt);
  if (!devtTds) {
    log << MSG::ERROR << "TofTrackCnv:Could not downcast to TDS ReconEvent" << endreq;
  }
  IOpaqueAddress *addr;

  m_cnvSvc->getDstCnv()->createRep(evt,addr); 
  TDstEvent *recEvt=m_cnvSvc->getDstCnv()->getWriteObject();

  const TObjArray *m_tofTrackCol = recEvt->getTofTrackCol();
  if (!m_tofTrackCol) return sc;
  recEvt->clearTofTrackCol(); //necessary in case there is I/O at the same time since array is static
  DstTofTrackCol::const_iterator tofTrackTds;

  for (tofTrackTds = tofTrackColTds->begin(); tofTrackTds != tofTrackColTds->end(); tofTrackTds++) {
    
      Int_t tofTrackID = (*tofTrackTds)->tofTrackID();
      Int_t trackID    = (*tofTrackTds)->trackID();
      Int_t tofID      = (*tofTrackTds)->tofID();
      Int_t strip      = (*tofTrackTds)->strip();
      UInt_t status    = (*tofTrackTds)->status();
      Double_t path    = (*tofTrackTds)->path();
      Double_t zrhit   = (*tofTrackTds)->zrhit();
      Double_t ph      = (*tofTrackTds)->ph();
      Double_t tof     = (*tofTrackTds)->tof();
      Double_t errtof  = (*tofTrackTds)->errtof();
      Double_t beta    = (*tofTrackTds)->beta();
      Double_t texpElectron = (*tofTrackTds)->texpElectron();
      Double_t texpMuon     = (*tofTrackTds)->texpMuon();
      Double_t texpPion     = (*tofTrackTds)->texpPion();
      Double_t texpKaon     = (*tofTrackTds)->texpKaon();
      Double_t texpProton   = (*tofTrackTds)->texpProton();
      Double_t toffsetElectron = (*tofTrackTds)->toffsetElectron();
      Double_t toffsetMuon     = (*tofTrackTds)->toffsetMuon();
      Double_t toffsetPion     = (*tofTrackTds)->toffsetPion();
      Double_t toffsetKaon     = (*tofTrackTds)->toffsetKaon();
      Double_t toffsetProton   = (*tofTrackTds)->toffsetProton();
      Double_t toffsetAntiProton   = (*tofTrackTds)->toffsetAntiProton();
      Double_t sigmaElectron = (*tofTrackTds)->sigmaElectron();
      Double_t sigmaMuon     = (*tofTrackTds)->sigmaMuon();
      Double_t sigmaPion     = (*tofTrackTds)->sigmaPion();
      Double_t sigmaKaon     = (*tofTrackTds)->sigmaKaon();
      Double_t sigmaProton   = (*tofTrackTds)->sigmaProton();
      Double_t sigmaAntiProton   = (*tofTrackTds)->sigmaAntiProton();
      Int_t quality    = (*tofTrackTds)->quality();
      Double_t t0      = (*tofTrackTds)->t0();
      Double_t errt0   = (*tofTrackTds)->errt0();
      Double_t errz    = (*tofTrackTds)->errz();
      Double_t phi     = (*tofTrackTds)->phi();
      Double_t errphi  = (*tofTrackTds)->errphi();
      Double_t energy    = (*tofTrackTds)->energy();
      Double_t errenergy = (*tofTrackTds)->errenergy();

      if( TofHitStatus::is_mrpc( status ) ) { tofID = tofID*12 + strip; }

      TTofTrack *tofTrackRoot = new TTofTrack();    
      //m_common.m_tofTrackMap[(*tofTrackTds)] = tofTrackRoot;

      tofTrackRoot->setTofTrackID(tofTrackID);
      tofTrackRoot->setTrackID(trackID);
      tofTrackRoot->setTofID(tofID);
      tofTrackRoot->setStatus(status);
      tofTrackRoot->setPath(path);
      tofTrackRoot->setZrHit(zrhit);
      tofTrackRoot->setPh(ph);
      tofTrackRoot->setTof(tof);
      tofTrackRoot->setErrTof(errtof);
      tofTrackRoot->setBeta(beta);
      tofTrackRoot->setTexpElectron(texpElectron);
      tofTrackRoot->setTexpMuon(texpMuon);
      tofTrackRoot->setTexpPion(texpPion);
      tofTrackRoot->setTexpKaon(texpKaon);
      tofTrackRoot->setTexpProton(texpProton);
      tofTrackRoot->setToffsetElectron(toffsetElectron);
      tofTrackRoot->setToffsetMuon(toffsetMuon);
      tofTrackRoot->setToffsetPion(toffsetPion);
      tofTrackRoot->setToffsetKaon(toffsetKaon);
      tofTrackRoot->setToffsetProton(toffsetProton);
      tofTrackRoot->setToffsetAntiProton(toffsetAntiProton);
      tofTrackRoot->setSigmaElectron(sigmaElectron);
      tofTrackRoot->setSigmaMuon(sigmaMuon);
      tofTrackRoot->setSigmaPion(sigmaPion);
      tofTrackRoot->setSigmaKaon(sigmaKaon);
      tofTrackRoot->setSigmaProton(sigmaProton);
      tofTrackRoot->setSigmaAntiProton(sigmaAntiProton);
      tofTrackRoot->setQuality(quality);
      tofTrackRoot->setT0(t0);
      tofTrackRoot->setErrT0(errt0);
      tofTrackRoot->setErrZ(errz);
      tofTrackRoot->setPhi(phi);
      tofTrackRoot->setErrPhi(errphi);
      tofTrackRoot->setEnergy(energy);
      tofTrackRoot->setErrEnergy(errenergy);

      recEvt->addTofTrack(tofTrackRoot);
  }

  return StatusCode::SUCCESS;
}
#endif
