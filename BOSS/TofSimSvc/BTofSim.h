#ifndef BTOF_SIM_H
#define BTOF_SIM_H 

#include "TMath.h"

class BTofSim{
 public:
   BTofSim() {}
   ~BTofSim(){}
 public:
   double getGain1()     const { return m_gain*TMath::Exp(m_ratio); }
   double getGain2()                     const { return m_gain;     }
   double getAttenLength()               const { return m_length;   }

 public:
   void setGain( double gain )                 { m_gain = gain;     }
   void setRatio( double ratio )               { m_ratio = ratio;   }
   void setAttenLength( double length )        { m_length = length; }

 private:
   double m_gain;
   double m_ratio;
   double m_length;

};

#endif
