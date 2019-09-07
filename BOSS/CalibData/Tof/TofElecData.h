// $Header: /bes/bes/BossCvs/Calibration/CalibData/CalibData/Tof/TofElecData.h,v 1.1 2009/09/22 07:28:05 huangb Exp $
#ifndef CalibData_TofElecData_h
#define CalibData_TofElecData_h
/* @class CalibData_TofElecData_h

  calibration data class  
  calibration infrastructure

  @author huangb
*/
#include "GaudiKernel/DataObject.h"
#include "CalibData/CalibModel.h"
#include "CalibData/CalibBase1.h"
#include "BarrelElec.h"
#include "EndcapElec.h"

namespace CalibData {
  class TofElecData:  public CalibBase1{
 
     public:
    TofElecData(){m_bTofElecData=0;
                 m_eTofElecData=0;};
    TofElecData(std::vector<BarrelElec>* bTofCal,std::vector<EndcapElec>* eTofCal);
    //    TofElecData(const TofElecData& other);
   void sim(TofElecData* other1);
    virtual ~TofElecData() { 
      delete m_bTofElecData;
      delete m_eTofElecData;
	}

    // Re-implemented from DataObject
    inline virtual const CLID& clID() const { return classID(); } 
    
    inline static  const CLID& classID() { return CLID_TofQ_Elec; }

    virtual StatusCode   update(CalibBase1& other, MsgStream* log);
        
    //get the Calibration Data
    BarrelElec getBTof(int cnt) const;

    EndcapElec getETof(int cnt) const;

  private:
    std::vector<BarrelElec>* m_bTofElecData;
    std::vector<EndcapElec>* m_eTofElecData;
  };
}


#endif
