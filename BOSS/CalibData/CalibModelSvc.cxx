// $Header: /bes/bes/BossCvs/Calibration/CalibData/src/CalibModelSvc.cxx,v 1.2 2006/11/17 02:37:06 huangb Exp $

#include "CalibData/CalibModelSvc.h"
#include "CalibData/CalibModel.h"

// extern const CLID& CLID_Calib_CalibCLIDNode;

namespace CalibData {
  const std::vector<CalibModelSvc::CalibPair>& CalibModelSvc::getPairs() 
    const {
    return pairs;
  }

  CLID CalibModelSvc::getCLIDNodeCLID() const {
    return CLID_Calib_CalibCLIDNode;
  }


  std::string CalibModelSvc::getCalibType(const std::string& fullpath) {
      unsigned lastSlash = fullpath.rfind("/");
    return fullpath.substr(lastSlash+1, fullpath.size() - lastSlash);

  }


}
