/** @class EsTimeCalibData
 *    Implementation of EsTimeCalibData calibration TCDS class
 */

#include "CalibData/EsTime/EsTimeCalibData.h"
#include "GaudiKernel/MsgStream.h"
namespace CalibData {
  EsTimeCalibData::EsTimeCalibData():m_TestCalibConst(0){}
   StatusCode EsTimeCalibData::update(CalibBase1& other, MsgStream* log)
  {
    EsTimeCalibData& other1 = dynamic_cast<EsTimeCalibData& >(other);
    std::cout<<"========================================"<<"\n"
	     <<"here is the update in the EsTimeCalibData"<<"\n"
	     <<"=================================================="<<std::endl;
    CalibBase1::update(other, log);
    //set the Calibration Data
     if(m_TestCalibConst) delete m_TestCalibConst;
    m_TestCalibConst = new vector<double>(*other1.m_TestCalibConst);
    m_toffsetb = other1.m_toffsetb;
    m_toffsete = other1.m_toffsete;
    m_bunchtime = other1.m_bunchtime;
    return StatusCode::SUCCESS;  
  }	
  
  //set the Calibdata of EsTime
  void EsTimeCalibData::setTestCalibConst(const vector<double>* TestCalibConst)
  {  if(m_TestCalibConst) delete m_TestCalibConst;
      m_TestCalibConst = new vector<double>(*TestCalibConst);
  }

  //set time of offset
   void EsTimeCalibData::setToffsetb(const double toffset){
        m_toffsetb=toffset;
   }
  
  void EsTimeCalibData::setToffsete(const double toffset){
        m_toffsete=toffset;
   }


  //set the time interval of each bunch
  void EsTimeCalibData::setBunchTime(const int bunchtime){
      m_bunchtime=bunchtime;
   }
}
