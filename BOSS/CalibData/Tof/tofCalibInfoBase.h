#ifndef CalibData_tofCalibInfoBase_h
#define CalibData_tofCalibInfoBase_h

#include "GaudiKernel/DataObject.h"
#include "CalibData/CalibModel.h"
#include <vector>

namespace CalibData {
  class tofCalibInfoBase{
  public:
    tofCalibInfoBase(){};

    virtual ~tofCalibInfoBase() {}

    //set the Calib Info of Tof
    void setRunBegin(const int run1)       {  m_run1 = run1;       }
    void setRunEnd(const int run2)         {  m_run2 = run2;       }
    void setVersion(const int version)     {  m_version = version; }

    void setQCorr(const int qCorr)         {  m_qCorr = qCorr;       }
    void setQElec(const int qElec)         {  m_qElec = qElec;       }
    void setMisLable(const int misLable)   {  m_misLable = misLable; }

    void setBrEast(const int* tofidEast);
    void setBrWest(const int* tofidWest);
    void setEndcap(const int* tofidEndcap);

    //get the Calib Info of Tof
    int getRunBegin()            const { return  m_run1;     }
    int getRunEnd()              const { return  m_run2;     }
    int getVersion()             const { return  m_version;  }

    int getQCorr()               const { return  m_qCorr;    }
    int getQElec()               const { return  m_qElec;    }
    int getMisLable()            const { return  m_misLable; }

    int getBrEast(int No)        const { return  m_tofidEast[No];   }
    int getBrWest(int No)        const { return  m_tofidWest[No];   }
    int getEndcap(int No)        const { return  m_tofidEndcap[No]; }

  private:
    int m_run1, m_run2, m_version;
    int m_qCorr, m_qElec, m_misLable;
    int m_tofidEast[5], m_tofidWest[5], m_tofidEndcap[5];

  };
}

#endif
