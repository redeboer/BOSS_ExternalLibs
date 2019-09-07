#ifndef EVENT_TRIG_DATA_H
#define EVENT_TRIG_DATA_H

#include <stdint.h> //uint32_t
#include <string>

#include "GaudiKernel/DataObject.h"

extern const CLID& CLID_TrigData;

//Trigger data struction for physics analysis
class TrigData :  public DataObject  {
public:
  //constructor 1
  TrigData() {}
  //constructor 2
  TrigData(int window, int timing, const int* trigCond, const int* trigChan, bool preScale);
  //destructor
  virtual ~TrigData() {}

  virtual const CLID& clID()                const   { return TrigData::classID(); }
  static const CLID& classID()                      { return CLID_TrigData; }

  void setTrigCondition(int i, int j)                { m_trigcond[i]    = j;    }
  void setTrigCondition(const int* cond)             { for(int i = 0; i < 48; i++) m_trigcond[i] = cond[i]; }
  void setTrigChannel(  int i, int j)                { m_trigChan[i] = j;       }
  void setTrigChannel(const int* chan)               { for(int i = 0; i < 16; i++) m_trigChan[i] = chan[i]; }
  void setTimeWindow( int i )                        { m_timeWindow    = i;     }
  void setTimingType( int i )                        { m_Timing        = i;     } 
  void setPreScale(   bool i )                       { m_preScale      = i;     }

  const std::string  getTrigCondName(int i)  const   { return s_CONDITIONS[i];  }
  const int          getTrigCondition(int i) const   { return m_trigcond[i];    }
  const int*         getTrigCondition()      const   { return m_trigcond;       }
  const int          getTrigChannel(int i)   const   { return m_trigChan[i];    }
  const int*         getTrigChannel()        const   { return m_trigChan;       }
  const int          getTimeWindow()         const   { return m_timeWindow;     }
  const int          getTimingType()         const   { return m_Timing;         }
  const bool         getPreScale()           const   { return m_preScale;       }

  void print() const;

private:
  int  m_trigcond[48];     //Trigger conditions from 0 to 47
  int  m_trigChan[16];  //Trigger channel from 0 to 15
  int  m_timeWindow;      //The time window of trigger system
  int  m_Timing;          //Time type, 0: No timing 1: TOF 2: MDC 3: EMC
  bool m_preScale;            //PreScale for bhabha events

  static const std::string s_CONDITIONS[48];  //The name of each trigger condition
};

#endif  // EVENT_TRIG_DATA_H
