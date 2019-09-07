#ifndef EMC_MCHIT_BUILDER_H
#define EMC_MCHIT_BUILDER_H

#include "RawDataCnv/EventManagement/Builder.h"
#include "McTruth/EmcMcHit.h" 
#include "RawDataCnv/EventManagement/McTruthDataStructure.h"

using namespace std;
using Event::EmcMcHit;
using Event::EmcMcHitCol;

class EmcMcHitBuilder: public Builder
{                                           

public:
   EmcMcHitBuilder ();
   virtual ~EmcMcHitBuilder(){};
   virtual StatusCode  initialize(string &initFile);
   virtual uint32_t getTEID(uint32_t teid);
   virtual uint32_t getREID(uint32_t reid);

   virtual void unPack (vector<uint32_t>::const_iterator&,
                        vector<uint32_t>::const_iterator&,
			EmcTruth_t&);
   virtual StatusCode pack(EmcMcHitCol* emcMcHitCol, WriteRawEvent*& re);

private:
   
  void makeEmcTruth(EmcMcHitCol::const_iterator& pEmcMcHit, EmcTruth_t& et);

  TE2REMAP m_te2reMap;
  //string initFile;
  vector <int> re2te;
  PropertyMgr m_propMgr;
 
  uint32_t m_trackIndexIndex, m_partIdIndex, m_numThetaIndex, m_numPhiIndex;
  uint32_t m_trackIndexMask, m_partIdMask, m_numThetaMask, m_numPhiMask;

public:
  uint64_t m_xCoeff;
  uint64_t m_yCoeff;
  uint64_t m_zCoeff;
  uint64_t m_pxCoeff;
  uint64_t m_pyCoeff;
  uint64_t m_pzCoeff;
  uint64_t m_totalEdepCoeff;
};
#endif
