#ifndef TOF_SIM_CONST_H
#define TOF_SIM_CONST_H 
  
class TofConst{
 public:
   TofConst()  {}
   ~TofConst() {}
 public:
   double getBarLowThres()        const { return m_blth;   }
   double getBarHighThres()       const { return m_bhth;   }
   double getEndLowThres()        const { return m_elth;   }
   double getEndHighThres()       const { return m_ehth;   }
   double getBarPMTGain()         const { return m_bgain;  }
   double getEndPMTGain()         const { return m_egain;  }
   double getBarConstant()        const { return m_bconst; }
   double getEndConstant()        const { return m_econst; }
   double getEndNoiseSwitch()     const { return m_eswitch;}

 public:
   void setBarLowThres( double blth )   { m_blth = blth;   }
   void setBarHighThres( double bhth )  { m_bhth = bhth;   }
   void setEndLowThres( double elth )   { m_elth = elth;   }
   void setEndHighThres( double ehth )  { m_ehth = ehth;   }
   void setBarPMTGain( double bg )      { m_bgain = bg;   }
   void setEndPMTGain( double eg )      { m_egain = eg;   }
   void setBarConstant( double bc )     { m_bconst = bc;   }
   void setEndConstant( double ec )     { m_econst = ec;   }
   void setEndNoiseSwitch( double eswitch )     { m_eswitch = eswitch;   }

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

#endif
