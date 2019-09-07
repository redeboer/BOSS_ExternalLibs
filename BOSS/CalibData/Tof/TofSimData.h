// $Header: /bes/bes/BossCvs/Calibration/CalibData/CalibData/Tof/TofSimData.h,v 1.10 2010/01/08
#ifndef CalibData_TofSimData_h
#define CalibData_TofSimData_h

/* @class CalibData_TofSimData_h

  calibration data class  
  calibration infrastructure

  @author sunss
*/

#include "GaudiKernel/DataObject.h"
#include "CalibData/CalibModel.h"
#include "CalibData/CalibBase1.h"
#include "CalibData/Tof/TofSimConstBase.h"
#include "CalibData/Tof/BTofSimBase.h"
#include "CalibData/Tof/ETofSimBase.h"

namespace CalibData {
  class TofSimData:  public CalibBase1{
 
  public:
    TofSimData(){};
    TofSimData(std::vector<TofSimConstBase>* fTofConst,std::vector<BTofSimBase>* fBTofSim,std::vector<ETofSimBase>* fETofSim);

    virtual ~TofSimData() { 
      delete m_TofSimConstData;
      delete m_BTofSimData;
      delete m_ETofSimData;
    }

    // Re-implemented from DataObject
    inline virtual const CLID& clID() const { return classID(); } 
    
    inline static  const CLID& classID() { return CLID_Calib_TofSim; }

    virtual StatusCode   update(CalibBase1& other, MsgStream* log);
        
    //get the Simulation Constants
    TofSimConstBase getTofConst() const;
    double getBarLowThres();
    double getBarHighThres();
    double getEndLowThres();
    double getEndHighThres();
    double getBarPMTGain();
    double getEndPMTGain();
    double getBarConstant();
    double getEndConstant();
    double getEndNoiseSwitch();

    BTofSimBase getBTofSim(int cnt) const;
    double getBarGain(int cnt);
    double getBarRatio(int cnt);
    double getBarAttenLength(int cnt);

    ETofSimBase getETofSim(int cnt) const;
    double getEndGain(int cnt);
    double getEndAttenLength(int cnt);
    double getEndNoiseSmear(int cnt);

  private:
    std::vector<TofSimConstBase>* m_TofSimConstData;
    std::vector<BTofSimBase>* m_BTofSimData;
    std::vector<ETofSimBase>* m_ETofSimData;
  };
}


#endif
