#ifndef CalibData_ETofSimBase_h
#define CalibData_ETofSimBase_h

#include "GaudiKernel/DataObject.h"
#include "CalibData/CalibModel.h"

namespace CalibData {
  class ETofSimBase{
  public:
    ETofSimBase(){};
    virtual ~ETofSimBase() {}

    //set the Simulation data of Tof
    void setGain( double gain )                 { m_gain = gain;     }
    void setAttenLength( double length )        { m_length = length; }
    void setNoiseSmear( double noisesmear )     { m_noisesmear = noisesmear; }

    //get the Simulation Data of Tof
    double getGain()                      const { return m_gain;     }
    double getAttenLength()               const { return m_length;   }
    double getNoiseSmear()                const { return m_noisesmear;   }

  private:
   double m_gain;
   double m_length;
   double m_noisesmear;
  };
}

#endif
