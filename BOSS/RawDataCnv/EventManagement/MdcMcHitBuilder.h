#ifndef MDC_MCHIT_BUILDER_H
#define MDC_MCHIT_BUILDER_H

#include "RawDataCnv/EventManagement/Builder.h"
#include "McTruth/MdcMcHit.h" 
#include "RawDataCnv/EventManagement/McTruthDataStructure.h"

using namespace std;
using Event::MdcMcHit;
using Event::MdcMcHitCol;

class MdcMcHitBuilder: public Builder
{                                           

public:
   MdcMcHitBuilder ();
   virtual ~MdcMcHitBuilder(){};
   virtual StatusCode  initialize(string &initFile);
   virtual uint32_t getTEID(uint32_t teid);
   virtual uint32_t getREID(uint32_t reid);
   virtual void unPack (vector<uint32_t>::const_iterator&,
                        vector<uint32_t>::const_iterator&,
			MdcTruth_t&);
   virtual StatusCode pack(MdcMcHitCol* mdcMcHitCol, WriteRawEvent*& re);

private:

  void makeMdcTruth(MdcMcHitCol::const_iterator& pMdcMcHit, MdcTruth_t& tt);

  TE2REMAP m_te2reMap;
  ///string initFile;
  vector <int> re2te;
  PropertyMgr m_propMgr;
 
  uint32_t  m_trackIndexIndex, m_layerIdIndex, m_cellIdIndex, m_posFlagIndex;
  uint32_t  m_trackIndexMask, m_layerIdMask, m_cellIdMask, m_posFlagMask;

public:
  uint64_t  m_edepCoeff;
  uint64_t  m_driftDCoeff;
  uint64_t  m_xCoeff;
  uint64_t  m_yCoeff;
  uint64_t  m_zCoeff;
};
#endif
