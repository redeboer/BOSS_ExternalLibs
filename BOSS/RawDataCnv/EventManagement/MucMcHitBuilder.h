#ifndef MUC_MCHIT_BUILDER_H
#define MUC_MCHIT_BUILDER_H

#include "RawDataCnv/EventManagement/Builder.h"
#include "McTruth/MucMcHit.h" 
#include "RawDataCnv/EventManagement/McTruthDataStructure.h"

using namespace std;
using Event::MucMcHit;
using Event::MucMcHitCol;

class MucMcHitBuilder: public Builder
{                                           

public:
   MucMcHitBuilder ();
   virtual ~MucMcHitBuilder(){};
   virtual StatusCode  initialize(string &initFile);
   virtual uint32_t getTEID(uint32_t teid);
   virtual uint32_t getREID(uint32_t reid);
   
   virtual void unPack(vector<uint32_t>::const_iterator&,
                       vector<uint32_t>::const_iterator&,
		       MucTruth_t&);
   virtual StatusCode pack(MucMcHitCol* mucMcHitCol, WriteRawEvent*& re);

private:
   
  void makeMucTruth(MucMcHitCol::const_iterator& pMucMcHit, MucTruth_t& mt);

  TE2REMAP m_te2reMap;
  //string initFile;
  vector <int> re2te;
  PropertyMgr m_propMgr;
 
  uint32_t m_trackIndexIndex, m_partIdIndex, m_segIdIndex, m_gapIdIndex, m_stripIdIndex;
  uint32_t m_trackIndexMask, m_partIdMask, m_segIdMask, m_gapIdMask, m_stripIdMask;

public:
  uint64_t m_xCoeff;
  uint64_t m_yCoeff;
  uint64_t m_zCoeff;
  uint64_t m_pxCoeff;
  uint64_t m_pyCoeff;
  uint64_t m_pzCoeff;
};
#endif
