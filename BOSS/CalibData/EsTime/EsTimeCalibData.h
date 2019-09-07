#ifndef CalibData_EsTimeCalibData_h
#define CalibData_EsTimeCalibData_h

/* @class CalibData_EsTimeCalibData_h

  calibration data class  
  calibration infrastructure

*/

#include "CalibData/CalibModel.h"
#include "CalibData/CalibBase1.h"
#include <vector>

using namespace std;

namespace CalibData {
  class EsTimeCalibData:  public CalibBase1{
    
  public:
    EsTimeCalibData();
    virtual ~EsTimeCalibData(){ 
      if(m_TestCalibConst)  delete m_TestCalibConst;
                           }
    
    // Re-implemented from DataObject
    inline virtual const CLID& clID() const { return classID(); } 
    
    inline static  const CLID& classID() { return CLID_Calib_EsTimeCal; }
    
    virtual StatusCode   update(CalibBase1& other, MsgStream* log);
     
    //set the Calibdata of EsTime
    void setTestCalibConst(const vector<double>* TestCalibConst);
    void setToffsetb(const double toffsetb);
    void setToffsete(const double toffsete);
    void setBunchTime(const int bunchtime);  
    
    //get the CalibData of EsTime 
    double getTestCalibConst(int No) const { return *((m_TestCalibConst->begin())+No);}
    //get the size of the vectors
    int getTestCalibConstNo() { return m_TestCalibConst -> size();}
    //get the time of offset for barrel
    double getToffsetb()   const { return m_toffsetb;}
    //get the time of offset for endcap
    double getToffsete()   const { return m_toffsete;}
    //get the time interval of each bunch
    double getBunchTime() const { return m_bunchtime;}

  private:
    vector<double>* m_TestCalibConst;
    double m_toffsetb;
    double m_toffsete;
    int m_bunchtime;
  };
}


#endif //CalibData_EsTimeCalibData_h
