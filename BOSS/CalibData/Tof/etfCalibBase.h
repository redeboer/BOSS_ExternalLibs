// $Header: /bes/bes/BossCvs/Calibration/CalibData/CalibData/Tof/etfCalibBase.h,v 1.3 2015/08/06 09:32:53 sunss Exp $
#ifndef CalibData_etfCalibBase_h
#define CalibData_etfCalibBase_h

#include "GaudiKernel/DataObject.h"
#include "CalibData/CalibModel.h"
#include <vector>


namespace CalibData {
  class etfCalibBase{
   public:
     etfCalibBase(){};

    virtual ~etfCalibBase() {
      }

    //set the Calibdata of ETF
     void setSpeed(const double* Speed);
     void setP(const double* etfP);
     void setP1(const double* etfP1);
     void setP2(const double* etfP2);

     //get the CalibData of Tof
     double  getSpeed(int No)   const { return m_Speed[No]; }
     double  getP(int No)       const { return m_P[No];     }
     double  getP1(int No)      const { return m_P1[No];    }
     double  getP2(int No)      const { return m_P2[No];    }

  private:
     double m_Speed[4];
     double m_P[20], m_P1[20], m_P2[20];
  };
}


#endif
