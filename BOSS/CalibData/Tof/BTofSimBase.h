#ifndef CalibData_BTofSimBase_h
#define CalibData_BTofSimBase_h

#include "GaudiKernel/DataObject.h"
#include "CalibData/CalibModel.h"
#include "TMath.h"

namespace CalibData {
  class BTofSimBase{
  public:
    BTofSimBase(){};
    virtual ~BTofSimBase() {}

    //set the Simulation data of Tof
    void setGain( double gain )                 { m_gain = gain;     }
    void setRatio( double ratio )               { m_ratio = ratio;   }
    void setAttenLength( double length )        { m_length = length; }

    //get the Simulation Data of Tof
    double getGain()                      const { return m_gain;     }
    double getRatio()                     const { return m_ratio;    }
    double getAttenLength()               const { return m_length;   }

  private:
   double m_gain;
   double m_ratio;
   double m_length;
  };
}

#endif
