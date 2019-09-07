#ifndef MDCALIGNDATA_H
#define MDCALIGNDATA_H
#include "GaudiKernel/DataObject.h"
#include "CalibData/CalibModel.h"
#include "CalibData/CalibBase1.h"
namespace CalibData {
class MdcAlignData:public CalibBase1{

public:
     void readPar(char* p1, char* p2, char* p3);
     // Re-implemented from DataObject
     virtual ~MdcAlignData() {}
     inline virtual const CLID& clID() const { return classID(); }

     inline static  const CLID& classID() { return CLID_Calib_MdcAlign; }

     virtual StatusCode   update(CalibBase1& other, MsgStream* log);

     /* alignment arrays for endplates, 0~7 for east, 8~15 for west */

     double getdxEP(int i){return dxEP[i];}
     double getdyEP(int i){return dyEP[i];}  
     double getdzEP(int i){return dzEP[i];}
     double getrxEP(int i){return rxEP[i];}
     double getryEP(int i){return ryEP[i];}
     double getrzEP(int i){return rzEP[i];} 
     double getdxWireEast(int i){return dxWireEast[i];}
     double getdyWireEast(int i){return dyWireEast[i];}
     double getdzWireEast(int i){return dzWireEast[i];}
     double getdxWireWest(int i){return dxWireWest[i];} 
     double getdyWireWest(int i){return dyWireWest[i];}
     double getdzWireWest(int i){return dzWireWest[i];}
     double gettension(int i){return tension[i];}
     
 private:
     double dxEP[16];
     double dyEP[16];
     double dzEP[16];
     double rxEP[16];
     double ryEP[16];
     double rzEP[16];

     /* Wire position calibration data */
     double dxWireEast[6796];
     double dyWireEast[6796];
     double dzWireEast[6796];
     double dxWireWest[6796];
     double dyWireWest[6796];
     double dzWireWest[6796];

     /* Wire tension data */
     double tension[6796];

};
}
#endif /* MDCALIGNDATA_H */
