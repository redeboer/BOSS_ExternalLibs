// $Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibROOTCnv/src/cnv/RootCalBaseCnv.cxx,v 1.3 2011/02/17 05:12:53 maqm Exp $
/**
            @file  RootCalBaseCnv.cxx

   Implementation file for Root calibration converter base class
*/

#include "RootCalBaseCnv.h"

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "facilities/Util.h"   // for translating env variables
#include "CalibDataSvc/ICalibRootSvc.h" 
#include "CalibDataSvc/ICalibMetaCnvSvc.h"
#include "CalibDataSvc/IInstrumentName.h"
#include "CalibData/CalibBase.h"
#include "CalibData/CalibBase1.h"
// Guessing at needed Root includes
#include "TROOT.h"    // need this for cd??
#include "TFile.h"
#include "TTree.h"
#include "TObject.h"


RootCalBaseCnv::~RootCalBaseCnv() {
  // release TFile, TTree if they need releasing.  With normal 
  // termination they should already have been released.

  // doClean();

}

// static CnvFactory<RootCalBaseCnv> s_factory;
// const ICnvFactory& RootCalBaseCnvFactory = s_factory;
RootCalBaseCnv::RootCalBaseCnv( ISvcLocator* svc, const CLID& clid) :
  Converter (CALIBROOT_StorageType, clid, svc),
  m_rootSvc (0), m_metaSvc(0), m_instrSvc(0), m_vstart(0), m_vend(0),
  m_outFile(0), m_ttree(0), m_inFile(0), m_saveDir(0) {}

StatusCode RootCalBaseCnv::initialize() {
  StatusCode status = Converter::initialize();

  IDataProviderSvc* dp;

  // I guess the service names are assigned in jobOptions?

  /*serviceLocator()->getService ("CalibDataSvc",
                                IID_IDataProviderSvc,
                                (IInterface*&)dp);*/
  serviceLocator()->getService ("CalibDataSvc",
                                IDataProviderSvc::interfaceID(),
                                (IInterface*&)dp);
  setDataProvider(dp);
  
  // Locate the Root Conversion Service
  serviceLocator()->getService ("CalibRootCnvSvc",
                                IID_ICalibRootSvc,
                                (IInterface*&) m_rootSvc);

  // Locate meta conversion service
  // Will anything need to be changed here to accommodate possibility
  // of two concrete implementations of ICalibMetaCnvSvc?  Would
  // have different storage types.  Could specify type desired
  // as job option.  Ditto for name of class?
  serviceLocator()->getService("CalibMySQLCnvSvc", 
                               IID_ICalibMetaCnvSvc,
                                (IInterface*&)m_metaSvc);

  serviceLocator()->getService ("CalibDataSvc",
                                IID_IInstrumentName,
                                (IInterface*&)m_instrSvc);

  return status;
}

StatusCode RootCalBaseCnv::finalize() {
  return Converter::finalize();
}


            /******      ROOT services     *****/

StatusCode RootCalBaseCnv::createRoot(const std::string& /* fname */, 
                                   CalibData::CalibBase1* /* pTDSObj */) {
  MsgStream log(msgSvc(), "RootCalBaseCnv");
  log << MSG::ERROR 
      << "createRoot method not implemented for this calibration type" 
      << endreq;
  return StatusCode::FAILURE;
}

StatusCode RootCalBaseCnv::openRead(const std::string& fname) { 

  MsgStream log(msgSvc(), "RootCalBaseCnv");

  // Check fname isn't empty
  if (fname == std::string("")) return StatusCode::FAILURE;

  if (doClean() ) {
    log << MSG::WARNING << "Previous operation didn't clean up! " << endreq;
  }
  m_saveDir = gDirectory;

  std::string ourName(fname);
  facilities::Util::expandEnvVar(&ourName);

  m_inFile = new TFile(ourName.c_str());
  
  if (!m_inFile->IsOpen() ) {
    log << MSG::ERROR << "ROOT file " << ourName 
        << "could not be opened for reading " << endreq;
    delete m_inFile;
    m_inFile = 0;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::INFO
        << "Successfully opened ROOT file " << fname << " aka " << ourName
        << " for reading " << endreq;
  }


  m_inFile->cd();             //    Maybe will need this


  return StatusCode::SUCCESS;
}

StatusCode RootCalBaseCnv::closeRead() {
  m_inFile->Close();

  delete m_inFile;
  m_inFile = 0;

  if (m_saveDir) {
    m_saveDir->cd();
    m_saveDir = 0;
  }
  return StatusCode::SUCCESS;
}
  
StatusCode RootCalBaseCnv::openWrite(const std::string& fname) {

  MsgStream log(msgSvc(), "RootCalBaseCnv");

  // Check fname isn't empty
  if (fname == std::string("")) return StatusCode::FAILURE;

  std::string ourName(fname);
  facilities::Util::expandEnvVar(&ourName);

  if (doClean() ) {
    log << MSG::WARNING << "Previous operation didn't clean up! " << endreq;
  }

  m_saveDir = gDirectory;

 
  m_outFile = new TFile(ourName.c_str(), "RECREATE");
  if (!m_outFile->IsOpen()) {
    log << MSG::ERROR << "ROOT file " << fname << " aka " << ourName 
        << " could not be opened for writing" << endreq;
    delete m_outFile;
    m_outFile = 0;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::INFO
        << "Successfully opened ROOT file " << fname << " aka " << ourName
        << " for writing " << endreq;
  }
   m_outFile->cd();
  return StatusCode::SUCCESS;
}

StatusCode RootCalBaseCnv::closeWrite() {

  MsgStream log(msgSvc(), "RootCalBaseCnv");

  StatusCode ret = StatusCode::SUCCESS;

  m_outFile->cd();
  m_outFile->Close();
  delete m_outFile;
  m_outFile = 0;
  if (m_saveDir) m_saveDir->cd();
  m_saveDir = 0;
  return ret;
}


StatusCode RootCalBaseCnv::readRootObj(const std::string& treename, 
                                    const std::string& branch,
                                    TObject*& pObj, unsigned ix){
  TTree* pTree = (TTree*)m_inFile->Get(treename.c_str());

  return readRootObj(pTree, branch, pObj, ix);
 }

StatusCode RootCalBaseCnv::readRootObj(TTree* pTree,
                                    const std::string& branch,
                                    TObject*& pObj, unsigned ix){
  TBranch* pBranch=pTree->GetBranch(branch.c_str());
  pBranch->SetAddress(&pObj);
  int nBytes = pBranch->GetEntry(ix);
  return (nBytes > 0) ? StatusCode::SUCCESS : StatusCode::FAILURE;
 }

bool RootCalBaseCnv::doClean() {
  bool ret = false;
  
  if (m_outFile) {
    m_outFile->Close();
    delete m_outFile;
    m_outFile = 0;
    ret = true;
  }
  
  if (m_inFile) {
    m_inFile->Close();
    delete m_inFile;
    m_inFile = 0;
    ret = true;
    }
  m_ttree = 0;
  if (m_saveDir) {
    m_saveDir->cd();
    m_saveDir = 0;
  }
  return ret;
}


// Do our part to write out object -- which is nothing
StatusCode RootCalBaseCnv::fillRoot(CalibData::CalibBase* /*pTDSObj */, 
                                 TObject* /* pRootObj */) {

  // Get instrument name from InstrumentName service  Now handled by 
  // RootCalBaseCnv
  //  TString instr = TString((m_instrSvc->getInstrumentName()).c_str());
  //  pRootObj->setInstrument(instr);
  return StatusCode::SUCCESS;
}

// (To TDS) Conversion stuff
StatusCode RootCalBaseCnv::createObj(IOpaqueAddress* addr,
                                  DataObject*& refpObject) {
  //  StatusCode ret;

  // first do the things we always need:
  //   First string parameter of opaque address is file ident
  MsgStream log(msgSvc(), "RootCalBaseCnv");
   log << MSG::DEBUG<<"RootCalBaseCnv::createObj( starting ...."<<endreq;
   const std::string* par = addr->par();

  std::string par0 = par[0];
   
  return internalCreateObj(par0, refpObject, addr);

}

StatusCode RootCalBaseCnv::internalCreateObj(const std::string& fname,
                                          DataObject*& refpObject,
                                          IOpaqueAddress* address) {
  MsgStream log(msgSvc(), "RootCalBaseCnv");
   log << MSG::DEBUG<<"RootCalBaseCnv::internalCreateObj( starting ..... "<<endreq;
   RootCalBaseCnv* converter = this;
  CLID classId = address->clID();

  IConverter* conv = this->conversionSvc()->converter(classId);
  if (0 == conv) {
    log << MSG::WARNING
        << "No proper converter found for classID " << classId
            << ", the default converter"
            << " will be used. " << endreq;
  } else {
    converter = dynamic_cast <RootCalBaseCnv*> (conv);
    if (0 == converter) {
      log << MSG::ERROR
          << "The converter found for classID " << classId
              << " was not a descendent of RootCalBaseCnv as it should be "
              << "( was of type " << typeid (*converter).name() << "). "
              << "The default converter will be used" << endreq;
      converter = this;
    }
  }

    m_runfrm =*( address->ipar());
    m_runto =*( address->ipar()+1);
   // creates an object for the node found
   StatusCode sc = converter->i_createObj(fname, refpObject);
     if (sc.isFailure()) {
    return sc;
  } 
     CalibData::CalibBase1* tmpObject =  dynamic_cast <CalibData::CalibBase1*> (refpObject);
     setBaseInfo(tmpObject);
  // ends up the object construction
  sc = converter->i_processObj(refpObject, address);
  if (sc.isSuccess()) {
    log << MSG::DEBUG << "Successfully created calib. object " << endreq;
  }
  closeRead();
  return sc;
}

/* 
   Base class version of this routine shouldn't really be called
   since it doesn't correspond to a TDS object.
*/
StatusCode RootCalBaseCnv::i_createObj (const std::string& /* fname */,
                                     DataObject*& /* refpObject */) {
  return StatusCode::FAILURE;  // shouldn't ever get here
}
  
// Default is to do nothing.  Derived classes may override.
StatusCode RootCalBaseCnv::i_processObj(DataObject*, // pObject,
                                     IOpaqueAddress* ) /* address */  {
  return StatusCode::SUCCESS;
}

/// Another utility for derived classes to use
void RootCalBaseCnv::setBaseInfo(CalibData::CalibBase1* pObj) {
   MsgStream log(msgSvc(), "RootCalBaseCnv");
   log << MSG::DEBUG<<"set the runfrm and runto Numbers in the converter"<<endreq;
  pObj->setrunfrm(m_runfrm);
  pObj->setrunto(m_runto);
}
