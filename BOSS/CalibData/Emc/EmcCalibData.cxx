/** @class EmcCalibData
 *    Implementation of EmcCalibData calibration TCDS class
 */

#include "CalibData/Emc/EmcCalibData.h"
#include "GaudiKernel/MsgStream.h"
namespace CalibData {
  EmcCalibData::EmcCalibData():m_DigiCalibConst(0),m_IxtalNumber(0){}

   StatusCode EmcCalibData::update(CalibBase1& other, MsgStream* log)
  {
    EmcCalibData& other1 = dynamic_cast<EmcCalibData& >(other);
    std::cout<<"========================================"<<"\n"
	     <<"here is the update in the EmcCalibData"<<"\n"
	     <<"=================================================="<<std::endl;
    CalibBase1::update(other, log);
    //set the Calibration Data
     if(m_DigiCalibConst) delete m_DigiCalibConst;
     if(m_IxtalNumber) delete m_IxtalNumber;

    m_DigiCalibConst = new vector<double>(*other1.m_DigiCalibConst);
    m_IxtalNumber = new vector<int>(*other1.m_IxtalNumber);
    return StatusCode::SUCCESS;  
  }	
  
  //set the Calibdata of Emc
  void EmcCalibData::setDigiCalibConst(const vector<double>* DigiCalibConst)
  {  if(m_DigiCalibConst) delete m_DigiCalibConst;
      m_DigiCalibConst = new vector<double>(*DigiCalibConst);
  }

  void EmcCalibData::setIxtalNumber(const vector<int>* IxtalNumber)
  {  if(m_IxtalNumber) delete m_IxtalNumber;
      m_IxtalNumber = new vector<int>(*IxtalNumber);
  }


}
