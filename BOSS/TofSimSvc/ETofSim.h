#ifndef ETOF_SIM_H
#define ETOF_SIM_H 
  
class ETofSim{
 public:
   ETofSim() {}
   ~ETofSim(){}
 public:
   double getGain()                     const { return m_gain;         }
   double getAttenLength()              const { return m_length;       }
   double getNoiseSmear()               const { return m_noisesmear;   }

 public:
   void setGain( double gain )                { m_gain = gain;     }
   void setAttenLength( double length )       { m_length = length; }
   void setNoiseSmear( double noisesmear )    { m_noisesmear = noisesmear; }

 private:
   double m_gain;
   double m_length;
   double m_noisesmear;

};

#endif
