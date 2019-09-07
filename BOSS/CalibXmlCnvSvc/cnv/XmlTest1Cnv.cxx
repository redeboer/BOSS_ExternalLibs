// $Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibXmlCnvSvc/src/cnv/XmlTest1Cnv.cxx,v 1.1.1.1 2006/04/03 03:04:32 maqm Exp $

#include <string>
#include "XmlTest1Cnv.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GenericAddress.h"

#include "CalibSvc/ICalibXmlSvc.h"
#include "CalibSvc/ICalibMetaCnvSvc.h"

#include "CalibData/CalibTest1.h"
#include "CalibData/CalibTime.h"
#include "xmlBase/Dom.h"

// Temporary.  Hope to find a better way to do this
#include "CalibData/CalibModel.h"

static CnvFactory<XmlTest1Cnv> s_factory;
const  ICnvFactory& XmlTest1CnvFactory = s_factory;



XmlTest1Cnv::XmlTest1Cnv( ISvcLocator* svc) :
  XmlBaseCnv(svc, CLID_Calib_CalibTest1) { 
}


const CLID& XmlTest1Cnv::objType() const {
  return CLID_Calib_CalibTest1;
}

const CLID& XmlTest1Cnv::classID() {
  return CLID_Calib_CalibTest1;
}

// Don't need to override in this case
/*
StatusCode XmlBaseCnv::i_processObj(DataObject*, // pObject,
                                    IOpaqueAddress*)  {  //pAddress
  return StatusCode::SUCCESS;
}
*/
 

// Create our specific object
StatusCode XmlTest1Cnv::i_createObj(const DOMElement* element, 
                                    DataObject*& refpObject)
{
  using xmlBase::Dom;

  // Fetch quantities we need: name, value
  DOMElement* child = Dom::findFirstChildByName(element, "data");
  if (child == 0) return StatusCode::FAILURE;
  child = Dom::findFirstChildByName(child, "leaf");
  if (child == 0) return StatusCode::FAILURE;

  std::string name = Dom::getAttribute(child, "name");

  //  std::string valueString = Dom::getAttribute(child, "value");

  //  int value = atoi(valueString.c_str());
  int value;
  try {
    value = Dom::getIntAttribute(child, "value");
  }
  catch (xmlBase::DomException ex) {
    std::cerr << "From CalibSvc::XmlTest1Cnv::i_crateObj " << std::endl;
    std::cerr << ex.getMsg() << std::endl;
  }

  refpObject = new  
    CalibData::CalibTest1(name, value, *m_vstart, *m_vend, m_serNo);

  return StatusCode::SUCCESS;
}
