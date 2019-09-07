// $Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibTreeCnv/src/cnv/TreeCalBaseCnv.cxx,v 1.4 2012/01/05 03:53:29 zhangy Exp $
/**
            @file  TreeCalBaseCnv.cxx

   Implementation file for Root calibration converter base class
*/

#include "TreeCalBaseCnv.h"

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
#include "DatabaseSvc/IDatabaseSvc.h"
#include "CalibMySQLCnv/TreeAddress.h"

TreeCalBaseCnv::~TreeCalBaseCnv() {
  // release TFile, TTree if they need releasing.  With normal 
  // termination they should already have been released.

  // doClean();

}

// static CnvFactory<TreeCalBaseCnv> s_factory;
// const ICnvFactory& TreeCalBaseCnvFactory = s_factory;
TreeCalBaseCnv::TreeCalBaseCnv( ISvcLocator* svc, const CLID& clid) :
  Converter (CALIBTREE_StorageType, clid, svc),
  m_treeSvc (0), m_metaSvc(0), m_instrSvc(0),m_outFile(0), m_ttree(0), m_inFile(0), m_saveDir(0) {}

StatusCode TreeCalBaseCnv::initialize() {
  StatusCode status = Converter::initialize();

  IDataProviderSvc* dp;

  // I guess the service names are assigned in jobOptions?

/*  serviceLocator()->getService ("CalibDataSvc",
                                IID_IDataProviderSvc,
                                (IInterface*&)dp);*/
  serviceLocator()->getService ("CalibDataSvc",
                                IDataProviderSvc::interfaceID(),
                                (IInterface*&)dp);
  setDataProvider(dp);
  
  // Locate the Root Conversion Service
  serviceLocator()->getService ("CalibTreeCnvSvc",
                                IID_ICalibTreeSvc,
                                (IInterface*&) m_treeSvc);

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

StatusCode TreeCalBaseCnv::finalize() {
  return Converter::finalize();
}


            /******      ROOT services     *****/

StatusCode TreeCalBaseCnv::createRoot(const std::string& /* fname */, 
                                   CalibData::CalibBase1* /* pTDSObj */) {
  MsgStream log(msgSvc(), "TreeCalBaseCnv");
  log << MSG::ERROR 
      << "createRoot method not implemented for this calibration type" 
      << endreq;
  return StatusCode::FAILURE;
}


// Do our part to write out object -- which is nothing
StatusCode TreeCalBaseCnv::fillRoot(CalibData::CalibBase* /*pTDSObj */, 
                                 TObject* /* pRootObj */) {

  // Get instrument name from InstrumentName service  Now handled by 
  // TreeCalBaseCnv
  //  TString instr = TString((m_instrSvc->getInstrumentName()).c_str());
  //  pRootObj->setInstrument(instr);
  return StatusCode::SUCCESS;
}

// (To TDS) Conversion stuff
StatusCode TreeCalBaseCnv::createObj(IOpaqueAddress* addr,
                                  DataObject*& refpObject) {
  //  StatusCode ret;

  // first do the things we always need:
  //   First string parameter of opaque address is file ident
  MsgStream log(msgSvc(), "TreeCalBaseCnv");
   log << MSG::DEBUG<<"TreeCalBaseCnv::createObj( starting ...."<<endreq;
 //  const std::string* par = addr->par();

 // std::string par0 = par[0];
   
//  return internalCreateObj(par0, refpObject, addr);
  return internalCreateObj(refpObject, addr);

}

//StatusCode TreeCalBaseCnv::internalCreateObj(const std::string& fname,
//                                          DataObject*& refpObject,
//                                          IOpaqueAddress* address) {
  StatusCode TreeCalBaseCnv::internalCreateObj(DataObject*& refpObject,
                                          IOpaqueAddress* address) {

  MsgStream log(msgSvc(), "TreeCalBaseCnv");
   log << MSG::DEBUG<<"TreeCalBaseCnv::internalCreateObj( starting ..... "<<endreq;
   TreeCalBaseCnv* converter = this;
  CLID classId = address->clID();

  IConverter* conv = this->conversionSvc()->converter(classId);
  if (0 == conv) {
    log << MSG::WARNING
        << "No proper converter found for classID " << classId
            << ", the default converter"
            << " will be used. " << endreq;
  } else {
    converter = dynamic_cast <TreeCalBaseCnv*> (conv);
    if (0 == converter) {
      log << MSG::ERROR
          << "The converter found for classID " << classId
              << " was not a descendent of TreeCalBaseCnv as it should be "
              << "( was of type " << typeid (*converter).name() << "). "
              << "The default converter will be used" << endreq;
      converter = this;
    }
  }

  TreeAddress* treeAddress =  dynamic_cast <TreeAddress*> (address);
  m_runfrm = treeAddress->getRunFrom();
  m_runto = treeAddress->getRunTo();
    //m_runfrm =*( address->ipar());
    //m_runto =*( address->ipar()+1);
 //   m_runfrm = 100;
 //   m_runto =1000;
   // creates an object for the node found
   StatusCode sc = converter->i_createObj(address, refpObject);
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
 // closeRead();
  return sc;
}

/* 
   Base class version of this routine shouldn't really be called
   since it doesn't correspond to a TDS object.
*/
StatusCode TreeCalBaseCnv::i_createObj (IOpaqueAddress* address,
                                     DataObject*& /* refpObject */) {
  return StatusCode::FAILURE;  // shouldn't ever get here
}
  
// Default is to do nothing.  Derived classes may override.
StatusCode TreeCalBaseCnv::i_processObj(DataObject*, // pObject,
                                     IOpaqueAddress* ) /* address */  {
  return StatusCode::SUCCESS;
}

/// Another utility for derived classes to use
void TreeCalBaseCnv::setBaseInfo(CalibData::CalibBase1* pObj) {
   MsgStream log(msgSvc(), "TreeCalBaseCnv");
   log << MSG::DEBUG<<"set the runfrm and runto Numbers in the converter"<<endreq;
  pObj->setrunfrm(m_runfrm);
  pObj->setrunto(m_runto);
}
