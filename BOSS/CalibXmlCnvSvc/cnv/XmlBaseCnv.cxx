// $Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibXmlCnvSvc/src/cnv/XmlBaseCnv.cxx,v 1.1.1.1 2006/04/03 03:04:32 maqm Exp $

#include "XmlBaseCnv.h"

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "CalibSvc/ICalibXmlSvc.h"
#include "CalibSvc/ICalibMetaCnvSvc.h"
#include "CalibData/CalibTime.h"
#include "CalibData/CalibBase.h"
//#include "CalibData/Cal/Xpos.h"

// Needed for ValSig
#include "CalibData/RangeBase.h"
#include "xmlBase/Dom.h"

// A little ugly to include this here.  It's needed for 
// CAL-specific utilities involving dac collections (maybe should be
// moved to XmlCalbaseCnv ?)
//#include "idents/CalXtalId.h"

// Similarly this is needed for calibrations involving dac settings
#include "CalibData/DacCol.h"

#include "facilities/Util.h"

#include <xercesc/dom/DOMDocument.hpp>
// #include <xercesc/dom/DOM_NodeList.hpp>

using namespace CalibData;
XERCES_CPP_NAMESPACE_USE

    
XmlBaseCnv::~XmlBaseCnv() {}

// static CnvFactory<XmlBaseCnv> s_factory;
// const ICnvFactory& XmlBaseCnvFactory = s_factory;
XmlBaseCnv::XmlBaseCnv( ISvcLocator* svc, const CLID& clid) :
  Converter (XML_StorageType, clid, svc),
  m_xmlSvc (0), m_metaSvc(0), m_vstart(0), m_vend(0) /*,
  m_nRow(10000), m_nCol(10000), m_nLayer(10000), m_nXtal(10000),
  m_nFace(10000), m_nRange(10000) */  {}

StatusCode XmlBaseCnv::initialize() {
  StatusCode status = Converter::initialize();

  IDataProviderSvc* dp;

  // I guess the service names are assigned in jobOptions?

  serviceLocator()->getService ("CalibDataSvc",
                                IID_IDataProviderSvc,
                                (IInterface*&)dp);
  setDataProvider(dp);
  
  // Locate the Xml Conversion Service
  serviceLocator()->getService ("CalibXmlCnvSvc",
                                IID_ICalibXmlSvc,
                                (IInterface*&)m_xmlSvc);

  // Locate meta conversion service
  // Will anything need to be changed here to accommodate possibility
  // of two concrete implementations of ICalibMetaCnvSvc?  Would
  // have different storage types.  Could specify type desired
  // as job option.  Ditto for name of class?
  serviceLocator()->getService("CalibMySQLCnvSvc", 
                               IID_ICalibMetaCnvSvc,
                                (IInterface*&)m_metaSvc);
  return status;
}

StatusCode XmlBaseCnv::finalize() {
  return Converter::finalize();
}

// Create transient representation

StatusCode XmlBaseCnv::createObj(IOpaqueAddress* addr,
                                DataObject*&    refpObject) {

   // creates a msg stream for debug purposes
   MsgStream log( msgSvc(), "XmlBaseCnv" );
   
   if (0 == addr) {
     return StatusCode::FAILURE;
   }

  // first do the things we always need:
  //   First string parameter of opaque address is file ident
  //   Parse file into DOM representation
  const std::string* par = addr->par();

  std::string par0 = par[0];

  // Ignore trailing white space.
  facilities::Util::trimTrailing(&par0);

  // Just in case there are environment variables in the file specification
  //  int nSub = 
  facilities::Util::expandEnvVar(&par0);

  //  XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = 
  DOMDocument* doc = 
    m_xmlSvc->parse(par0.c_str());

  if (doc == 0 ) {
    log << MSG::FATAL 
        << "Unable to parse document " << par[0] << " aka " 
        << par0 << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::INFO
        << "successfully parsed document " << par[0] << " aka " 
        << par0 << endreq;
  }

  // Could conceivably write some code here to handle generic
  // parts of document.  Or, alternatively, add services to
  // CalibXmlCnvSvc for converters to invoke to do this.

  // Then do some fancy footwork in internalCreateObj to get the 
  // appropriate specific converter invoked to interpret the DOM 
  // correctly and make a new object of the correct kind.

  return internalCreateObj(doc->getDocumentElement(), refpObject, addr);
}


/** In a backhanded way, invoke the right specific converter
    for the type of the object to be created
    @param  elt      Document elt from XML document   (input)
    @param  refpObject 
*/
StatusCode XmlBaseCnv::internalCreateObj(const DOMElement* docElt,
                                         DataObject*& refpObject,
                                         IOpaqueAddress* address) {
  // creates a msg stream for debug purposes
  MsgStream log( msgSvc(), "XmlBaseCnv" );
  
  // We're the default if we can't find anything better
  XmlBaseCnv* converter = this;      

  CLID classId = address->clID();

  IConverter* conv = this->conversionSvc()->converter(classId);

  if (0 == conv) {
    log << MSG::WARNING
        << "No proper converter found for classID " << classId
            << ", the default converter"
            << " will be used. " << endreq;
  } else {
    converter = dynamic_cast <XmlBaseCnv*> (conv);
    if (0 == converter) {
      log << MSG::ERROR
          << "The converter found for classID " << classId
              << " was not a descendent of XmlBaseCnv as it should be "
              << "( was of type " << typeid (*converter).name() << "). "
              << "The default converter will be used" << endreq;
      converter = this;
    }
  }

  unsigned int serNo = *(address->ipar());
  m_serNo = serNo;
/* maqm comment for remove getValidInterval()
 StatusCode sc = m_metaSvc->getValidInterval(serNo, 
                                              &m_vstart, 
                                              &m_vend );


  // creates an object for the node found

  
   if (sc.isSuccess()) sc = converter->i_createObj (docElt, refpObject);
 */ 
   StatusCode sc = converter->i_createObj (docElt, refpObject);
   if (sc.isFailure()) {
    return sc;
  }

  // ends up the object construction
  sc = converter->i_processObj(refpObject, address);
  if (sc.isSuccess()) {
    log << MSG::DEBUG << "Successfully created calib. object " << endreq;
  }
  return sc;
} 

// Default is to do nothing.  Derived classes may override.
StatusCode XmlBaseCnv::i_processObj(DataObject*, // pObject,
                                    IOpaqueAddress* ) /* address */  {
  return StatusCode::SUCCESS;
}
 

// Shouldn't ever really get here 
StatusCode XmlBaseCnv::i_createObj(const DOMElement*, DataObject*&) {
  return StatusCode::FAILURE;
}

/*
// Not sure yet whether this needs a real implementation or not
StatusCode XmlBaseCnv::updateObj(IOpaqueAddress* ,
                              DataObject*& ) {
  return StatusCode::FAILURE;
}

// Since we're not expecting to support writing back to persistent
// store, don't implement the converter *Rep functions.
*/

StatusCode XmlBaseCnv::readHeader(const DOMElement*) {
  return StatusCode::SUCCESS;
}

const unsigned char XmlBaseCnv::storageType() {
  return XML_StorageType;
}

/*

*/
/// Another utility for derived classes to use
void XmlBaseCnv::setBaseInfo(CalibData::CalibBase* pObj) {
  pObj->setValidity(*m_vstart, *m_vend);
  pObj->setSerNo(m_serNo);
}


DOMElement* XmlBaseCnv::findFirstDacCol(const DOMElement* docElt) {
  return  xmlBase::Dom::findFirstChildByName(docElt, "dac");
}

DOMElement* XmlBaseCnv::findNextDacCol(const DOMElement* dacElt) {
  DOMElement* next = xmlBase::Dom::getSiblingElement(dacElt);
  if (xmlBase::Dom::checkTagName(next, "dac")) return next;
  else return 0;
}

CalibData::DacCol* XmlBaseCnv::processDacCol(DOMElement* dacColElt,
                                             unsigned* range) {

  using xmlBase::Dom;
//maqm comment  using idents::CalXtalId;

  std::string att = Dom::getAttribute(dacColElt, "range");
/* maqm comment
  if (att.compare(std::string("LEX8")) == 0) *range = CalXtalId::LEX8;
  if (att.compare(std::string("LEX1")) == 0) *range = CalXtalId::LEX1;
  if (att.compare(std::string("HEX8")) == 0) *range = CalXtalId::HEX8;
  if (att.compare(std::string("HEX1")) == 0) *range = CalXtalId::HEX1; 
*/
  //  *range = Dom::getIntAttribute(dacColElt, "range");
  std::vector<int> vals;

  Dom::getIntsAttribute(dacColElt, "values", vals);

  CalibData::DacCol* pDacCol = new CalibData::DacCol(&vals);
  return pDacCol;

}

/* maqm comment
 DOMElement* XmlBaseCnv::findXpos(const DOMElement* docElt) {
  return  xmlBase::Dom::findFirstChildByName(docElt, "xpos");
}

CalibData::Xpos* XmlBaseCnv::processXpos(DOMElement* xposElt) {
  using xmlBase::Dom;

  std::vector<float> vals;

  Dom::getFloatsAttribute(xposElt, "values", vals);
  CalibData::Xpos* pXpos = new CalibData::Xpos(&vals);
  
  return pXpos;
}
*/
/// Read in what will become a CalibData::ValSig
CalibData::ValSig* XmlBaseCnv::processValSig(DOMElement* elt, 
                                             std::string valName, 
                                             std::string sigName) {
  if (elt == 0 ) return 0;
  CalibData::ValSig *pValSig = new CalibData::ValSig;
  pValSig->m_val = xmlBase::Dom::getDoubleAttribute(elt, valName);
  pValSig->m_sig = xmlBase::Dom::getDoubleAttribute(elt, sigName);
  return pValSig;
}

std::vector<CalibData::ValSig>* 
XmlBaseCnv::processValSigs(DOMElement* elt, std::string valName, 
                           std::string sigName) {

  // creates a msg stream for debug purposes
  MsgStream log( msgSvc(), "XmlBaseCnv" );

  if (elt == 0 ) return 0;
  std::vector<float> vals;
  std::vector<float> sigs;

  xmlBase::Dom::getFloatsAttribute(elt, valName, vals);
  xmlBase::Dom::getFloatsAttribute(elt, sigName, sigs);
  if (vals.size() != sigs.size() ) {
    log << MSG::ERROR << "#values <> #sigmas " << endreq;
    return 0;
  }
  unsigned n = vals.size();
  std::vector<CalibData::ValSig>* pValSigs = 
    new std::vector<CalibData::ValSig>(n);
  for (unsigned i = 0; i < n; i++) {
    (*pValSigs)[i].m_val = vals[i];
    (*pValSigs)[i].m_sig = sigs[i];
  }
  return pValSigs;
}
