#include "CalibDataSvc/CalibCLIDNode.h"
/**  @file CalibCLIDNode.cxx

     $Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibDataSvc/src/CalibCLIDNode.cxx,v 1.2 2006/04/06 02:16:44 maqm Exp $

     @author Joanne Bogart
*/
#include "CalibData/CalibModelSvc.h"
using namespace CalibData;
// CLID_Calib_CalibCLIDNode gets a value in CalibModel.cxx, but
// this is in a different component; doubt I can pick it up correctly here.
 extern const CLID& CLID_Calib_CalibCLIDNode;
const CLID& CLID_Calib_CalibCLIDNode = 6000;
//maqmCLID CalibCLIDNode::m_myClassID = 0;

const CLID& CalibCLIDNode::classID() {
    return CLID_Calib_CalibCLIDNode;
/*//maqm add
  if (m_myClassID == 0) {
    CalibData::CalibModelSvc svc;
    m_myClassID = svc.getCLIDNodeCLID();
  }
  return m_myClassID;
  */
}

std::ostream& CalibCLIDNode::fillStream(std::ostream& s) const {
  return s <<  "Child class ID = " << m_childClassID << std::endl;
}
