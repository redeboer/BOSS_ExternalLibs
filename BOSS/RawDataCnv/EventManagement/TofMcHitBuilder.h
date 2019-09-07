#ifndef TOF_MCHIT_BUILDER_H
#define TOF_MCHIT_BUILDER_H

#include "RawDataCnv/EventManagement/Builder.h"
#include "McTruth/TofMcHit.h" 
#include "RawDataCnv/EventManagement/McTruthDataStructure.h"

using namespace std;
using Event::TofMcHit;
using Event::TofMcHitCol;

class TofMcHitBuilder: public Builder
{                                           

public:
   TofMcHitBuilder ();
   virtual ~TofMcHitBuilder(){};
   virtual StatusCode  initialize(string &initFile);
   virtual uint32_t getTEID(uint32_t teid);
   virtual uint32_t getREID(uint32_t reid);

   virtual void unPack(vector<uint32_t>::const_iterator&,
                       vector<uint32_t>::const_iterator&,
		       TofTruth_t&);
   virtual StatusCode pack(TofMcHitCol* tofMcHitCol, WriteRawEvent*& re);

private:
   
  void makeTofTruth(TofMcHitCol::const_iterator& pTofMcHit, TofTruth_t& tt);

  TE2REMAP m_te2reMap;
  //string initFile;
  vector <int> re2te;
  PropertyMgr m_propMgr;
 
  uint32_t m_trackIndexIndex, m_partIdIndex, m_scinNbIndex, m_stripIndex;
  uint32_t m_trackIndexMask, m_partIdMask, m_scinNbMask, m_stripMask;

public:
  uint64_t m_xCoeff;
  uint64_t m_yCoeff;
  uint64_t m_zCoeff;
  uint64_t m_pxCoeff;
  uint64_t m_pyCoeff;
  uint64_t m_pzCoeff;
  uint64_t m_trackLengthCoeff;
  uint64_t m_timeCoeff;
};
#endif
