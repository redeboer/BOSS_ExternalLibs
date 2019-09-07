// $Header: /bes/bes/BossCvs/Calibration/calibUtil/calibUtil/GenericSrv.h,v 1.1.1.1 2005/10/17 06:12:26 maqm Exp $
 
#ifndef CALIBUTIL_GENERICSRV_H
#define CALIBUTIL_GENERICSRV_H

#include "xmlBase/XmlParser.h"
#include "xmlBase/Dom.h"
#include <xercesc/dom/DOMElement.hpp>

#include <string>
#include <iostream>
#include <vector>

namespace calibUtil {

  class InputSample;

  class GenericSrv {
  public: 

    /// Constructor that fills in values of generic data from XML file
    GenericSrv(const XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* docElt);

    /// Constructor to be used when creating a new calibration data set
    GenericSrv(std::string inst, std::string timestamp, 
               std::string calType, InputSample* samp) :
      m_instName(inst), m_timestamp(timestamp), m_calType(calType),
      m_fmtVer(0), m_sample(samp) {}

    /// Get instrument name
    std::string getInst(){
      return m_instName;
    }
    
    /// Get timestamp
    std::string getTimestamp(){
      return m_timestamp;
    }
    
    /// Get calibration type
    std::string getCalType(){
      return m_calType;
    }
    
    /// Get format Version
    std::string getFmtVer(){
      return m_fmtVer;
    }
    
  private:
    
    std::string m_instName;
    std::string m_timestamp;
    std::string m_calType;
    std::string m_fmtVer;

    // Corresponds to inputSample element in badStrips.dtd (except
    // that the class doesn't actually exist yet.
    InputSample *m_sample;

  };

}// end of namespace calibUtil

#endif



