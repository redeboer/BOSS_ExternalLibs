#ifndef HLTBUILDER_H
#define HLTBUILDER_H

#include "RawDataCnv/EventManagement/Builder.h"
#include "HltEvent/HltRaw.h"   

using namespace std;

class HltBuilder: public Builder
{                                           

public:
   HltBuilder ();
   virtual ~HltBuilder();
   StatusCode  initialize(string &initFile);
   uint32_t getTEID(uint32_t reid) { return m_re2te[reid]; }
   uint32_t getREID(uint32_t teid);
   void unPack (uint32_t reDigi, uint32_t n, 
		uint32_t &REId, uint32_t &TEData1, uint32_t &TEData2);
   StatusCode pack(HltRawCol* digiCol, WriteRawEvent*& re);

private:
  //string initFile;
  uint32_t m_idIndex;
  uint32_t m_idMask;
  uint32_t m_algIndex;
  uint32_t m_algMask;
  uint32_t m_criIndex;
  uint32_t m_criMask;
  uint32_t m_varIndex;
  uint32_t m_varMask;

  uint32_t*  m_re2te;
  TE2REMAP   m_te2reMap;

  PropertyMgr m_propMgr;
};
#endif
