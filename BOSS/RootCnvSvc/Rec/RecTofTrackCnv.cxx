#ifndef RecTofTrackCnv_CXX
#define RecTofTrackCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"

#include "ReconEvent/ReconEvent.h"
#include "DstEvent/TofHitStatus.h"
#include "MucRecEvent/RecMucTrack.h"

#include "RootEventData/TRecTofTrack.h" // standard root object
#include "RootEventData/TRecTrackEvent.h"

#include "RootCnvSvc/Rec/RecTrackCnv.h"
#include "RootCnvSvc/Rec/RecTofTrackCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include <vector>

using namespace std;

// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<RecTofTrackCnv> s_factory;
//const ICnvFactory& RecTofTrackCnvFactory = s_factory;

RecTofTrackCnv::RecTofTrackCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{

    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "RecTofTrackCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    //m_rootTreename ="Rec";
    //m_rootBranchname ="m_RecTofTrackCol";
    //declareObject(EventModel::Recon::RecTofTrackCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_recTofTrackCol);
    m_recTofTrackCol=0;
}

StatusCode RecTofTrackCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object


    MsgStream log(msgSvc(), "RecTofTrackCnv");
    log << MSG::DEBUG << "RecTofTrackCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the MucTrack Collection
    RecTofTrackCol* recTofTrackTdsCol = new RecTofTrackCol;
    refpObject=recTofTrackTdsCol;

    // now convert
    if (!m_recTofTrackCol) return sc;
    TIter recTofTrackIter(m_recTofTrackCol);
    TRecTofTrack *recTofTrackRoot = 0;
    while ((recTofTrackRoot = (TRecTofTrack*)recTofTrackIter.Next())) {

      int tofTrackID = recTofTrackRoot->tofTrackID();
      int trackID    = recTofTrackRoot->trackID();
      int tofID      = recTofTrackRoot->tofID();
      unsigned int status = recTofTrackRoot->status();
      double path    = recTofTrackRoot->path();
      double zrhit   = recTofTrackRoot->zrhit();
      double ph      = recTofTrackRoot->ph();
      double tof     = recTofTrackRoot->tof();
      double errtof  = recTofTrackRoot->errtof();
      double beta    = recTofTrackRoot->beta();
      double texpElectron = recTofTrackRoot->texpElectron();
      double texpMuon     = recTofTrackRoot->texpMuon();
      double texpPion     = recTofTrackRoot->texpPion();
      double texpKaon     = recTofTrackRoot->texpKaon();
      double texpProton   = recTofTrackRoot->texpProton();
      double toffsetElectron = recTofTrackRoot->toffsetElectron();
      double toffsetMuon     = recTofTrackRoot->toffsetMuon();
      double toffsetPion     = recTofTrackRoot->toffsetPion();
      double toffsetKaon     = recTofTrackRoot->toffsetKaon();
      double toffsetProton   = recTofTrackRoot->toffsetProton();
      double toffsetAntiProton = recTofTrackRoot->toffsetAntiProton();
      double sigmaElectron = recTofTrackRoot->sigmaElectron();
      double sigmaMuon     = recTofTrackRoot->sigmaMuon();
      double sigmaPion     = recTofTrackRoot->sigmaPion();
      double sigmaKaon     = recTofTrackRoot->sigmaKaon();
      double sigmaProton   = recTofTrackRoot->sigmaProton();
      double sigmaAntiProton = recTofTrackRoot->sigmaAntiProton();
      int quality    = recTofTrackRoot->quality();
      double t0      = recTofTrackRoot->t0();
      double errt0   = recTofTrackRoot->errt0();
      double errz    = recTofTrackRoot->errz();
      double phi     = recTofTrackRoot->phi();
      double errphi  = recTofTrackRoot->errphi();
      double energy    = recTofTrackRoot->energy();
      double errenergy = recTofTrackRoot->errenergy();

      RecTofTrack *recTofTrackTds = new RecTofTrack();
      m_common.m_rootRecTofTrackMap[recTofTrackRoot] = recTofTrackTds;

      recTofTrackTds->setTofTrackID(tofTrackID);
      recTofTrackTds->setTrackID(trackID);
      recTofTrackTds->setTofID(tofID);
      recTofTrackTds->setStatus(status);
      recTofTrackTds->setPath(path);
      recTofTrackTds->setZrHit(zrhit);
      recTofTrackTds->setPh(ph);
      recTofTrackTds->setTof(tof);
      recTofTrackTds->setErrTof(errtof);
      recTofTrackTds->setBeta(beta);
      recTofTrackTds->setTexpElectron(texpElectron);
      recTofTrackTds->setTexpMuon(texpMuon);
      recTofTrackTds->setTexpPion(texpPion);
      recTofTrackTds->setTexpKaon(texpKaon);
      recTofTrackTds->setTexpProton(texpProton);
      recTofTrackTds->setToffsetElectron(toffsetElectron);
      recTofTrackTds->setToffsetMuon(toffsetMuon);
      recTofTrackTds->setToffsetPion(toffsetPion);
      recTofTrackTds->setToffsetKaon(toffsetKaon);
      recTofTrackTds->setToffsetProton(toffsetProton);
      recTofTrackTds->setToffsetAntiProton(toffsetAntiProton);
      recTofTrackTds->setSigmaElectron(sigmaElectron);
      recTofTrackTds->setSigmaMuon(sigmaMuon);
      recTofTrackTds->setSigmaPion(sigmaPion);
      recTofTrackTds->setSigmaKaon(sigmaKaon);
      recTofTrackTds->setSigmaProton(sigmaProton);
      recTofTrackTds->setSigmaAntiProton(sigmaAntiProton);
      recTofTrackTds->setQuality(quality);
      recTofTrackTds->setT0(t0);
      recTofTrackTds->setErrT0(errt0);
      recTofTrackTds->setErrZ(errz);
      recTofTrackTds->setPhi(phi);
      recTofTrackTds->setErrPhi(errphi);
      recTofTrackTds->setEnergy(energy);
      recTofTrackTds->setErrEnergy(errenergy);

      recTofTrackTdsCol->push_back(recTofTrackTds);
    }

    delete m_recTofTrackCol;
    m_recTofTrackCol = 0;

    return StatusCode::SUCCESS;
}

StatusCode RecTofTrackCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {
 
  MsgStream log(msgSvc(), "RecTofTrackCnv");
  log << MSG::DEBUG << "RecTofTrackCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;

  RecTofTrackCol * recTofTrackColTds=dynamic_cast<RecTofTrackCol *> (obj);
  if (!recTofTrackColTds) {
    log << MSG::ERROR << "Could not downcast to TofTrackCol" << endreq;
    return StatusCode::FAILURE;
  }

  DataObject *evt;
  m_eds->findObject(EventModel::Recon::Event,evt);
  if (evt==NULL) {
    log << MSG::ERROR << "Could not get RecEvent in TDS "  << endreq;
    return StatusCode::FAILURE;
  }

  ReconEvent * devtTds=dynamic_cast<ReconEvent *> (evt);
  if (!devtTds) {
    log << MSG::ERROR << "RecMucTrackCnv:Could not downcast to TDS DigiEvent" << endreq;
  }

  IOpaqueAddress *addr;

  m_cnvSvc->getRecTrackCnv()->createRep(evt,addr); 
  TRecTrackEvent *recEvt=m_cnvSvc->getRecTrackCnv()->getWriteObject();

  const TObjArray *m_recTofTrackCol = recEvt->getTofTrackCol();
  if (!m_recTofTrackCol) return sc;
  recEvt->clearTofTrackCol(); //necessary in case there is I/O at the same time since array is static
  RecTofTrackCol::const_iterator recTofTrackTds;

  for (recTofTrackTds = recTofTrackColTds->begin(); recTofTrackTds != recTofTrackColTds->end(); recTofTrackTds++) {

    Int_t tofTrackID = (*recTofTrackTds)->tofTrackID();
    Int_t trackID    = (*recTofTrackTds)->trackID();
    Int_t tofID      = (*recTofTrackTds)->tofID();
    Int_t strip      = (*recTofTrackTds)->strip();
    UInt_t status    = (*recTofTrackTds)->status();
    Double_t path    = (*recTofTrackTds)->path();
    Double_t zrhit   = (*recTofTrackTds)->zrhit();
    Double_t ph      = (*recTofTrackTds)->ph();
    Double_t tof     = (*recTofTrackTds)->tof();
    Double_t errtof  = (*recTofTrackTds)->errtof();
    Double_t beta    = (*recTofTrackTds)->beta();
    Double_t texpElectron = (*recTofTrackTds)->texpElectron();
    Double_t texpMuon     = (*recTofTrackTds)->texpMuon();
    Double_t texpPion     = (*recTofTrackTds)->texpPion();
    Double_t texpKaon     = (*recTofTrackTds)->texpKaon();
    Double_t texpProton   = (*recTofTrackTds)->texpProton();
    Double_t toffsetElectron = (*recTofTrackTds)->toffsetElectron();
    Double_t toffsetMuon     = (*recTofTrackTds)->toffsetMuon();
    Double_t toffsetPion     = (*recTofTrackTds)->toffsetPion();
    Double_t toffsetKaon     = (*recTofTrackTds)->toffsetKaon();
    Double_t toffsetProton   = (*recTofTrackTds)->toffsetProton();
    Double_t toffsetAntiProton = (*recTofTrackTds)->toffsetAntiProton();
    Double_t sigmaElectron = (*recTofTrackTds)->sigmaElectron();
    Double_t sigmaMuon     = (*recTofTrackTds)->sigmaMuon();
    Double_t sigmaPion     = (*recTofTrackTds)->sigmaPion();
    Double_t sigmaKaon     = (*recTofTrackTds)->sigmaKaon();
    Double_t sigmaProton   = (*recTofTrackTds)->sigmaProton();
    Double_t sigmaAntiProton = (*recTofTrackTds)->sigmaAntiProton();
    Int_t quality    = (*recTofTrackTds)->quality();
    Double_t t0      = (*recTofTrackTds)->t0();
    Double_t errt0   = (*recTofTrackTds)->errt0();
    Double_t errz    = (*recTofTrackTds)->errz();
    Double_t phi     = (*recTofTrackTds)->phi();
    Double_t errphi  = (*recTofTrackTds)->errphi();
    Double_t energy    = (*recTofTrackTds)->energy();
    Double_t errenergy = (*recTofTrackTds)->errenergy();
    
    if( TofHitStatus::is_mrpc( status ) ) { tofID = tofID*12 + strip; }

    TRecTofTrack *recTofTrackRoot = new TRecTofTrack();
    //m_common.m_RecTofTrackMap[(*recTofTrackTds)] = recTofTrackRoot;

    recTofTrackRoot->setTofTrackID(tofTrackID);
    recTofTrackRoot->setTrackID(trackID);
    recTofTrackRoot->setTofID(tofID);
    recTofTrackRoot->setStatus(status);
    recTofTrackRoot->setPath(path);
    recTofTrackRoot->setZrHit(zrhit);
    recTofTrackRoot->setPh(ph);
    recTofTrackRoot->setTof(tof);
    recTofTrackRoot->setErrTof(errtof);
    recTofTrackRoot->setBeta(beta);
    recTofTrackRoot->setTexpElectron(texpElectron);
    recTofTrackRoot->setTexpMuon(texpMuon);
    recTofTrackRoot->setTexpPion(texpPion);
    recTofTrackRoot->setTexpKaon(texpKaon);
    recTofTrackRoot->setTexpProton(texpProton);
    recTofTrackRoot->setToffsetElectron(toffsetElectron);
    recTofTrackRoot->setToffsetMuon(toffsetMuon);
    recTofTrackRoot->setToffsetPion(toffsetPion);
    recTofTrackRoot->setToffsetKaon(toffsetKaon);
    recTofTrackRoot->setToffsetProton(toffsetProton);
    recTofTrackRoot->setToffsetAntiProton(toffsetAntiProton);
    recTofTrackRoot->setSigmaElectron(sigmaElectron);
    recTofTrackRoot->setSigmaMuon(sigmaMuon);
    recTofTrackRoot->setSigmaPion(sigmaPion);
    recTofTrackRoot->setSigmaKaon(sigmaKaon);
    recTofTrackRoot->setSigmaProton(sigmaProton);
    recTofTrackRoot->setSigmaAntiProton(sigmaAntiProton);
    recTofTrackRoot->setQuality(quality);
    recTofTrackRoot->setT0(t0);
    recTofTrackRoot->setErrT0(errt0);
    recTofTrackRoot->setErrZ(errz);
    recTofTrackRoot->setPhi(phi);
    recTofTrackRoot->setErrPhi(errphi);
    recTofTrackRoot->setEnergy(energy);
    recTofTrackRoot->setErrEnergy(errenergy);

    recEvt->addTofTrack(recTofTrackRoot);

  }

  return StatusCode::SUCCESS;
}
#endif
