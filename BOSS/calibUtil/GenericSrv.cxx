//$ Header: $
/// Module implements methods for clients to get generic services.

#include "xmlBase/Dom.h"

#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>

#include "calibUtil/GenericSrv.h"

namespace calibUtil {
 
  using XERCES_CPP_NAMESPACE_QUALIFIER DOMElement;
  /// Constructor that fills in values of generic data from XML file
  GenericSrv::GenericSrv(const DOMElement* docElt) : m_sample(0) {
    
    DOMElement* child  = xmlBase::Dom::findFirstChildByName(docElt,"generic");
    m_instName  = xmlBase::Dom::getAttribute(child,"instrument");
    m_timestamp = xmlBase::Dom::getAttribute(child,"timestamp");
    m_calType   = xmlBase::Dom::getAttribute(child,"calType");
    m_fmtVer    = xmlBase::Dom::getAttribute(child,"fmtVersion");

  }
  
}// end of namespace calibUtil






