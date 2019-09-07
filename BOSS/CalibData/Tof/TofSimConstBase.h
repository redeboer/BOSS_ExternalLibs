#ifndef CalibData_TofSimConstBase_h
#define CalibData_TofSimConstBase_h

#include "GaudiKernel/DataObject.h"
#include "CalibData/CalibModel.h"
#include <vector>

namespace CalibData {
  class TofSimConstBase{
  public:
    TofSimConstBase(){};
    virtual ~TofSimConstBase() {}

    //set the Simulation data of Tof
   double getBarLowThres()        const { return m_blth;   }
   double getBarHighThres()       const { return m_bhth;   }
   double getEndLowThres()        const { return m_elth;   }
   double getEndHighThres()       const { return m_ehth;   }
   double getBarPMTGain()         const { return m_bgain; }
   double getEndPMTGain()         const { return m_egain; }
   double getBarConstant()        const { return m_bconst; }
   double getEndConstant()        const { return m_econst; }
   double getEndNoiseSwitch()     const { return m_eswitch; }

   // get the Simulation data of Tof
   void setBarLowThres( const double blth )   { m_blth = blth;   }
   void setBarHighThres( const double bhth )  { m_bhth = bhth;   }
   void setEndLowThres( const double elth )   { m_elth = elth;   }
   void setEndHighThres( const double ehth )  { m_ehth = ehth;   }
   void setBarPMTGain( const double bg )      { m_bgain = bg;   }
   void setEndPMTGain( const double eg )      { m_egain = eg;   }
   void setBarConstant( const double bc )     { m_bconst = bc;   }
   void setEndConstant( const double ec )     { m_econst = ec;   }
   void setEndNoiseSwitch( const double eswitch )     { m_eswitch = eswitch;   }

 private:
   double m_blth;
   double m_bhth;
   double m_elth;
   double m_ehth;
   double m_bgain;
   double m_egain;
   double m_bconst;
   double m_econst;
   double m_eswitch;
  };
}

#endif
