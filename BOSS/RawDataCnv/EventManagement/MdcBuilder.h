#ifndef MDCBUILDER_H
#define MDCBUILDER_H

#include "RawDataCnv/EventManagement/Builder.h"
#include "MdcRawEvent/MdcDigi.h" 

using namespace std;

class MdcBuilder: public Builder
{                                           

public:
   MdcBuilder ();
   virtual ~MdcBuilder();
   virtual StatusCode  initialize(string &initFile);
   virtual uint32_t getTEID(uint32_t reid) { return m_re2te[reid]; }
   virtual uint32_t getREID(uint32_t teid);
   virtual void unPack (uint32_t reDigi, uint32_t &REId, uint32_t &TEData, 
                        uint32_t &overFlow, uint32_t &TorQ);
   virtual StatusCode pack(MdcDigiCol* digiCol, WriteRawEvent*& re);

private:
  //string initFile;
  uint32_t    m_idIndex;
  uint32_t    m_idMask;
  uint32_t    m_dataIndex;
  uint32_t    m_dataMask;
  uint32_t    m_overflowIndex;
  uint32_t    m_overflowMask;
  uint32_t    m_TQIndex;
  uint32_t    m_TQMask;

  uint32_t*   m_re2te;
  TE2REMAP    m_te2reMap;

  PropertyMgr m_propMgr;
};
#endif
