#ifndef MDCDATACONST_H
#define MDCDATACONST_H
#include "GaudiKernel/DataObject.h"
#include "CalibData/CalibModel.h"
#include "CalibData/CalibBase1.h"
namespace CalibData {
class MdcDataConst:public CalibBase1{

public:
     void readPar(char* p1);
     // Re-implemented from DataObject
     virtual ~MdcDataConst() {}
     inline virtual const CLID& clID() const { return classID(); }

     inline static  const CLID& classID() { return CLID_Calib_MdcDataConst; }

     virtual StatusCode update(CalibBase1& other, MsgStream* log);

     double getWireEff(int i){return wireEff[i];}  
     
 private:
     static const int NMDCWIRE = 6796;

     /* raw data wire efficiency data */

     double wireEff[NMDCWIRE];


};
}
#endif /* MDCDATACONST_H */
