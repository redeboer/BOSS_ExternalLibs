#ifndef EVENT_TRIG_EACC_H
#define EVENT_TRIG_EACC_H

#include <stdint.h>
#include <string>
#include <vector>
#include <map>

class TrigEACC {
public:
  TrigEACC(const char* name);
  virtual ~TrigEACC();

  void setEACCTrigData(uint32_t boardId, uint32_t window, uint32_t datasize, const uint32_t* data);

  void initialize();

  inline const std::string& getName() const { return m_name; }
  inline unsigned int getTimeWindow() const { return m_window; }

  inline unsigned int getBBLKCharge(int id) const   { return E_blk[id];   }
  inline unsigned int getBBLKTime(int id) const     { return T_blk[id];   }
  inline unsigned int getEBLKCharge(int id) const   { return E_blkE[id];  }
  inline unsigned int getEBLKTime(int id) const     { return T_blkE[id];  }
  inline unsigned int getWBLKCharge(int id) const   { return E_blkW[id];  }
  inline unsigned int getWBLKTime(int id) const     { return T_blkW[id];  }
  inline unsigned int getBarrelCharge() const       { return E_B_Etot;    }
  inline unsigned int getBarrelTime() const         { return T_B_Etot;    }
  inline unsigned int getEndCapsCharge() const      { return E_E_Etot;    }
  inline unsigned int getEndCapsTime() const        { return T_E_Etot;    }
  inline unsigned int getEMCTotalCharge() const     { return E_blkAll;    }
  inline unsigned int getEMCTotalTime() const       { return T_blkAll;    }
  inline unsigned int getEastHalfCharge() const     { return E_eastHalf;  }
  inline unsigned int getEastHalfTime() const       { return T_eastHalf;  }
  inline unsigned int getWestHalfCharge() const     { return E_westHalf;  }
  inline unsigned int getWestHalfTime() const       { return T_westHalf;  }
  inline unsigned int getEastEndHalfCharge() const  { return E_eastEndHalf; }
  inline unsigned int getEastEndHalfTime() const    { return T_eastEndHalf; }
  inline unsigned int getWestEndHalfCharge() const  { return E_westEndHalf; }
  inline unsigned int getWestEndHalfTime() const    { return T_westEndHalf; }

  inline unsigned int getBTC(int theta, int phi, int winId) const   { return TC[theta][phi][winId]; }
  inline unsigned int getETC(int phi, int winId)            const   { return TC_E[phi][winId];      }
  inline unsigned int getWTC(int phi, int winId)            const   { return TC_W[phi][winId];      }
 
  inline unsigned int getMidCondi(int bit)                  const   { return midCondi[bit];            }
  inline unsigned int getTrigCondi(int bit, int winId)      const   { return emcTrigCondi[bit][winId]; } 

  void print();

private:
  std::string m_name;

  unsigned int m_window;

  unsigned int E_blk[12]; //charge of each barrel block Id
  unsigned int T_blk[12]; //time of each barrel block Id
  unsigned int E_blkE[2]; //charge of east block
  unsigned int T_blkE[2]; //time of east block
  unsigned int E_blkW[2]; //charge of west block
  unsigned int T_blkW[2]; //time of west block
  unsigned int E_B_Etot;  //barrel charge
  unsigned int T_B_Etot;  //barrel time
  unsigned int E_E_Etot;  //endcaps charge
  unsigned int T_E_Etot;  //endcaps time
  unsigned int E_blkAll;  //total emc charge
  unsigned int T_blkAll; //total emc time
  unsigned int E_eastHalf; //charge of east half
  unsigned int T_eastHalf; //time of east half
  unsigned int E_westHalf; //charge of west half
  unsigned int T_westHalf; //time of west half
  unsigned int E_eastEndHalf; //charge of east end cap
  unsigned int T_eastEndHalf; //time of east end cap
  unsigned int E_westEndHalf; //charge of west end cap
  unsigned int T_westEndHalf; //time of west end cap

  unsigned int TC[11][30][32]; //theta, phi, windowId
  unsigned int TC_E[32][32]; //phi, windowId
  unsigned int TC_W[32][32]; //phi, windowId

  unsigned int midCondi[16]; //conditionId
  unsigned int emcTrigCondi[16][32]; //conditionId, windowId

};

#endif  // EVENT_TRIG_EACC_H
