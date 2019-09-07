#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "CalibData/Mdc/MdcDataConst.h"

using namespace std;
namespace CalibData {

  StatusCode MdcDataConst::update(CalibBase1& other, MsgStream* log)
  {
    MdcDataConst& other1 = dynamic_cast<MdcDataConst& >(other);
    cout<<"\n"<<"here is the update in the MdcDataConst in calibration"<<std::endl;
    CalibBase1::update(other, log);
    for(int i=0;i<NMDCWIRE;i++){
      wireEff[i]=other1.wireEff[i];
    }

    return StatusCode::SUCCESS;
  }


  void MdcDataConst::readPar(char* wireEffPointer){
    istringstream  fWireEff;
    string aa1 = wireEffPointer;
    fWireEff.str(aa1);   

    //read notes line
    string tempString;
    fWireEff >> tempString >> tempString >> tempString >> tempString;

    //read wire efficiency
    string tempLayer,tempCell;
    int tempGlobalWire;
    double tempWireEff;
    for(int i=0; i<NMDCWIRE; i++){
      fWireEff >> tempGlobalWire >> tempLayer >> tempCell >> tempWireEff;
      wireEff[tempGlobalWire] = tempWireEff;
    }

  }
}
