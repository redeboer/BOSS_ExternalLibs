// $Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibDataSvc/CalibDataSvc/ICalibXmlSvc.h,v 1.1 2006/04/06 03:04:46 maqm Exp $

#ifndef IXMLSvc_h 
#define IXMLSvc_h

/** @class ICalibXmlSvc
   Abstract interface to be satisfied by any XML conversion implementation.
 
   Will parse a file, making its DOM rep. available

   Maybe also provide layer over "raw" access to the DOM to insulate
   converters from changes moving from one xml parser version (or even
   parser) to another.
   ...or do we just depend on things in calibUtil?

*/
#include "GaudiKernel/IInterface.h"
// #include <xercesc/dom/DOM_Document.hpp>
#include <xercesc/util/XercesDefs.hpp>
XERCES_CPP_NAMESPACE_BEGIN
class DOMDocument;
XERCES_CPP_NAMESPACE_END

static const InterfaceID IID_ICalibXmlSvc("ICalibXmlSvc", 1, 0);

class ICalibXmlSvc : virtual public IInterface 
{
public:
  // Re-implemented from IInterface
  static const InterfaceID& interfaceID() { return IID_ICalibXmlSvc; }
  
  /**
   * This method parses an xml file and produces the corresponding DOM
   * document.
   * @param fileName the name of the file to parse
   * @return the document issued from the parsing
   */
  virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* 
  parse(const char* filename) = 0;

  // Do we also want a "reset" or "clearDocument" ?  Can in any case
  // do this internally when a new document is to be parsed so might not
  // be necessary to have explicit public method.
};


#endif
