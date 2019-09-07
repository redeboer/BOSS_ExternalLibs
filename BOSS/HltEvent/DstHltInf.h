#ifndef EVENT_DST_HLT_INF_H
#define EVENT_DST_HLT_INF_H

#include <stdint.h> //uint32_t
#include <iostream>
#include "GaudiKernel/DataObject.h"
#include "EventModel/EventModel.h"
#include "Identifier/Identifier.h"
#include "HltEvent/IHltInterface.h"

using namespace std;

extern const CLID& CLID_DstHltInf;

//event filter data struction for physics analysis
class DstHltInf :  public DataObject, public IHltInterface {
public:

  DstHltInf(const unsigned int type, const unsigned int alg, const unsigned int cri,
	    const unsigned int ver, const float etot);
  DstHltInf();
  DstHltInf(const DstHltInf&);
  DstHltInf& operator=(const DstHltInf&);

  virtual ~DstHltInf() {}

  virtual const CLID& clID()                const   { return DstHltInf::classID(); }
  static const CLID& classID()                      { return CLID_DstHltInf; }

  void setEventType(const unsigned int i)              { m_eventType = i;    }
  void setAlgProcess(const unsigned int i)             { m_algProcess=i; }
  void setCriteriaTable(const unsigned int i)                { m_criTable=i;       }
  void setVersion(const unsigned int i)               { m_version=i; }
  void setTotalEnergy(const float etot)               { m_eTotal = etot;     }
  void setNumber(const unsigned int i) { m_number=i; };
  void setNMDC(const unsigned int i) { m_number = (m_number&0xFFFFFF00)|i ; };
  void setNTOF(const unsigned int i) { m_number = (m_number&0xFFFF00FF)|(i<<8); };
  void setNEMC(const unsigned int i) { m_number = (m_number&0xFF00FFFF)|(i<<16); };
  void setNMUC(const unsigned int i) { m_number = (m_number&0x00FFFFFF)|(i<<24); };
  void setNCON(const unsigned int i) { m_ncon   = i; };

  uint32_t     getEventType() const   { return m_eventType;}
  uint32_t     getAlgProcess()const   { return m_algProcess;       }
  uint32_t     getCriteriaTable()const   { return m_criTable;    }
  uint32_t     getVersion()   const   { return m_version;       }
  float        getTotalEnergy()  const   { return m_eTotal;     }
  uint32_t     getNumber()  const { return m_number; };
  uint32_t     getNMDC()  const { return m_number&0xFF; };
  uint32_t     getNTOF()  const { return (m_number>>8)&0xFF; };
  uint32_t     getNEMC()  const { return (m_number>>16)&0xFF; };
  uint32_t     getNMUC()  const { return (m_number>>24)&0xFF; };
  uint32_t     getNCON()  const { return m_ncon; };

  const string& getEventName() const ;
  bool isType(string& type) const ;

  virtual void push_mdc(uint32_t){};
  virtual void push_tof(uint32_t){};
  virtual void push_emc(uint32_t){};
  virtual void push_muc(uint32_t){};
  virtual void push_con(uint32_t){};
  virtual void push_mdc(float){};
  virtual void push_tof(float){};
  virtual void push_emc(float){};
  virtual void push_muc(float){};
  virtual void push_con(float){};
  virtual bool setData(const Identifier&, uint32_t){return false;};
  virtual bool setData(const Identifier&, float){return false;};
  virtual void setMdcData(const std::vector<uint32_t>&){};
  virtual void setTofData(const std::vector<uint32_t>&){};
  virtual void setEmcData(const std::vector<uint32_t>&){};
  virtual void setMucData(const std::vector<uint32_t>&){};
  virtual void setConData(const std::vector<uint32_t>&){};
  virtual bool getData(const Identifier&, uint32_t*){ return false;};

  virtual void print() const { cout << *this; }

protected:
  uint32_t  m_eventType;     //Hlt 
  uint32_t  m_algProcess;  //Hlt
  uint32_t  m_criTable;
  uint32_t  m_version;
  float     m_eTotal;
  uint32_t  m_number;
  uint32_t  m_ncon;

  static const std::string s_chn[32];
  static const std::string s_err;
};

ostream& operator<<(ostream & os, const DstHltInf& aHlt);

#endif  // EVENT_DST_HLT_INF_H

