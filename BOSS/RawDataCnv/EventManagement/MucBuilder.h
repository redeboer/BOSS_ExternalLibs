#ifndef MUCBUILDER_H
#define MUCBUILDER_H

#include "RawDataCnv/EventManagement/Builder.h"
#include "MucRawEvent/MucDigi.h" 


using namespace std;

class MucBuilder: public Builder
{                                           

public:
   MucBuilder ();
   virtual ~MucBuilder();
   virtual StatusCode  initialize(string &initFile);
   virtual uint32_t getTEID(uint32_t reid) {
      if ( reid > 1023 ) return 0xFFFFFFFF;
      return  m_re2te[reid];
   }
   virtual uint32_t getREID(uint32_t teid);
   virtual void unPack (uint32_t reDigi, uint32_t &REId, uint32_t &FecData, uint32_t &StrSqc);
   virtual StatusCode pack(MucDigiCol* digiCol, WriteRawEvent*& re);

private:
  //string initFile;
  uint32_t m_idIndex;
  uint32_t m_idMask;
  uint32_t m_overflowIndex;
  uint32_t m_overflowMask;
  uint32_t m_TQIndex;
  uint32_t m_TQMask;

  uint32_t*  m_re2te;
  uint32_t*  strSqcVec;
  TE2REMAP   m_te2reMap;

  PropertyMgr m_propMgr;
  string   m_fec2idFile;
};
#endif
