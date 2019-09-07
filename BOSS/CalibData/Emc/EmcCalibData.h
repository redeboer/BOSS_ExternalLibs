#ifndef CalibData_EmcCalibData_h
#define CalibData_EmcCalibData_h

/* @class CalibData_EmcCalibData_h

  calibration data class  
  calibration infrastructure

  @author ChunXiu Liu
*/

#include "CalibData/CalibModel.h"
#include "CalibData/CalibBase1.h"
#include <vector>

using namespace std;

namespace CalibData {
  class EmcCalibData:  public CalibBase1{
    
  public:
    EmcCalibData();
    virtual ~EmcCalibData(){ 
      if(m_DigiCalibConst)  delete m_DigiCalibConst;
      if(m_IxtalNumber) delete  m_IxtalNumber;
    }

    
    // Re-implemented from DataObject
    inline virtual const CLID& clID() const { return classID(); } 
    
    inline static  const CLID& classID() { return CLID_Calib_EmcCal; }
    
    virtual StatusCode   update(CalibBase1& other, MsgStream* log);
     
    //set the Calibdata of Emc
    void setDigiCalibConst(const vector<double>* DigiCalibConst);

    //set the Calibdata of Emc
    void setIxtalNumber(const vector<int>* IxtalNumber);
    
    //get the CalibData of Emc 
    // const  vector<double>*  getDigiCalibConst() const { return m_DigiCalibConst;}
    // const  vector<double>*  getEnCoeff() const { return m_EnCoeff;} 
    // const  vector<double>*  getPosCoeff() const  { return m_PosCoeff;}
    
    double getDigiCalibConst(int No) const { return *((m_DigiCalibConst->begin())+No);}

    double getIxtalNumber(int No) const { return *((m_IxtalNumber->begin())+No);}

    //get the size of the vectors
    int getDigiCalibConstNo() { return m_DigiCalibConst -> size();}

    //get the size of the vectors
    int getIxtalNumberNo() { return m_IxtalNumber -> size();}

  private:
    vector<double>* m_DigiCalibConst;
    vector<int>* m_IxtalNumber;    
  };
}


#endif //CalibData_EmcCalibData_h
