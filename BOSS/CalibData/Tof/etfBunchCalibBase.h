#ifndef CalibData_etfBunchCalibBase_h
#define CalibData_etfBunchCalibBase_h

#include "GaudiKernel/DataObject.h"
#include "CalibData/CalibModel.h"
#include <vector>

namespace CalibData {
  class etfBunchCalibBase{
  public:
    etfBunchCalibBase(){};

    virtual ~etfBunchCalibBase() {}

    //set the bunch offset of etf
    void setPBunch(const double* etfBunchP);

    //get the bunch offset of etf
    double  getPBunch(int No)       const { return m_pBunch[No];     }

  private:
    double  m_pBunch[4];

  };
}

#endif
