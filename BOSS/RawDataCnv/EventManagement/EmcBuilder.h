#ifndef EMCBUILDER_H
#define EMCBUILDER_H

#include "RawDataCnv/EventManagement/Builder.h"
#include "EmcRawEvent/EmcDigi.h"   

using namespace std;

class EmcBuilder: public Builder
{                                           

public:
   EmcBuilder ();
   virtual ~EmcBuilder();
   StatusCode  initialize(string &initFile);
   uint32_t getTEID(uint32_t reid) { return m_re2te[reid]; }
   uint32_t getREID(uint32_t teid);
   void unPack (uint32_t reDigi, uint32_t &REId, uint32_t &TETDC, uint32_t &TEADC, uint32_t &measure);
   StatusCode pack(EmcDigiCol* digiCol, WriteRawEvent*& re);

private:
  //string initFile;
  uint32_t m_idIndex;
  uint32_t m_idMask;
  uint32_t m_overflowIndex;
  uint32_t m_overflowMask;
  uint32_t m_TQIndex;
  uint32_t m_TQMask;
  uint32_t m_tdcIndex;
  uint32_t m_tdcMask;
  uint32_t m_measureIndex;
  uint32_t m_measureMask;
  uint32_t m_adcIndex;
  uint32_t m_adcMask;

  uint32_t*   m_re2te;
  TE2REMAP    m_te2reMap;

  PropertyMgr m_propMgr;
};
#endif
