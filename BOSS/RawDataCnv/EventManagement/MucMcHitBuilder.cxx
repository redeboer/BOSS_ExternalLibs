#include <iostream>
#include <fstream>
#include "Identifier/MucID.h"
#include "RawDataCnv/EventManagement/MucMcHitBuilder.h"
using namespace std;

MucMcHitBuilder::MucMcHitBuilder()
   :Builder()
{
  initialize (Builder::m_confFile);
}

void MucMcHitBuilder::unPack(vector<uint32_t>::const_iterator& iter,
             vector<uint32_t>::const_iterator& eiter, MucTruth_t& mt)
{
  uint32_t helpVal = *(iter++); assert(iter != eiter);
  mt.x          = *(iter++); assert(iter != eiter);
  mt.y          = *(iter++); assert(iter != eiter);
  mt.z          = *(iter++); assert(iter != eiter);
  mt.px         = *(iter++); assert(iter != eiter);
  mt.py         = *(iter++); assert(iter != eiter);
  mt.pz         = *(iter++);
  
  mt.trackIndex = (helpVal&m_trackIndexMask) >> m_trackIndexIndex;
  mt.partId     = (helpVal&m_partIdMask) >> m_partIdIndex;
  mt.segId      = (helpVal&m_segIdMask) >> m_segIdIndex;
  mt.gapId      = (helpVal&m_gapIdMask) >> m_gapIdIndex;
  mt.stripId    = (helpVal&m_stripIdMask) >> m_stripIdIndex;

  return;
}


StatusCode MucMcHitBuilder::pack(MucMcHitCol* mucMcHitCol, WriteRawEvent*& re)
{
/*

  if (mucMcHitCol == NULL) {
    cout << "MucMcHitBuilder::pack cant get MucMcHitCol" << endl;
    return StatusCode::FAILURE;
  }

  vector<uint32_t> *mucReMcHitVec = new vector<uint32_t>;
  MucTruth_t m_MucTruth;

  MucMcHitCol::const_iterator pMucMcHit = mucMcHitCol->begin();
  for ( ; pMucMcHit != mucMcHitCol->end(); pMucMcHit++) {
    //make the MucTruth data
    makeMucTruth(pMucMcHit, m_MucTruth);
    //pack integers in mucTruth
    uint32_t helpVal = (m_MucTruth.trackIndex<<m_trackIndexIndex) & m_trackIndexMask;
    helpVal |= ((m_MucTruth.partId<<m_partIdIndex) & m_partIdMask);
    helpVal |= ((m_MucTruth.segId<<m_segIdIndex) & m_segIdMask);
    helpVal |= ((m_MucTruth.gapId<<m_gapIdIndex) & m_gapIdMask);
    helpVal |= ((m_MucTruth.stripId<<m_stripIdIndex) & m_stripIdMask);
    //fill the McHit vector
    mucReMcHitVec->push_back(helpVal);
    mucReMcHitVec->push_back(m_MucTruth.x);
    mucReMcHitVec->push_back(m_MucTruth.y);
    mucReMcHitVec->push_back(m_MucTruth.z);
    mucReMcHitVec->push_back(m_MucTruth.px);
    mucReMcHitVec->push_back(m_MucTruth.py);
    mucReMcHitVec->push_back(m_MucTruth.pz);
  }

  OfflineEventFormat::SubDetectorHeader sh(OfflineEventFormat::MUCTRUTH);
  SubRawEvent *sub = new SubRawEvent(sh, mucReMcHitVec);
  re->append(sub);
  */

  return StatusCode::SUCCESS;
}

// initialize re2te tables

StatusCode  MucMcHitBuilder::initialize(string& initFile)
{
  ifstream f;

  //read init file
  f.open(initFile.c_str());

  if ( f.bad() ) {
    cerr   << "Error: could not open file " << initFile << endl;
    return StatusCode::FAILURE;
  }

  if (!Builder::find( f, "##MucTruthConf", initFile)) {
    cerr << "Error: could not find '##MucTruthConf' in file " << initFile << endl;
    return StatusCode::FAILURE;
  }

  if ( !Builder::expect( f, "#MucTruthShift", initFile) ||
       !Builder::expectInt( f, "trackIndex", initFile, m_trackIndexIndex, m_trackIndexMask) ||
       !Builder::expectInt( f, "partId", initFile, m_partIdIndex, m_partIdMask) ||
       !Builder::expectInt( f, "segId", initFile, m_segIdIndex, m_segIdMask) ||
       !Builder::expectInt( f, "gapId", initFile, m_gapIdIndex, m_gapIdMask) ||
       !Builder::expectInt( f, "stripId", initFile, m_stripIdIndex, m_stripIdMask) ||
       !Builder::expect( f, "#MucTruthCoeff", initFile) ||
       !Builder::expectLong( f, "x", initFile, m_xCoeff) ||
       !Builder::expectLong( f, "y", initFile, m_yCoeff) ||
       !Builder::expectLong( f, "z", initFile, m_zCoeff) ||
       !Builder::expectLong( f, "px", initFile, m_pxCoeff) ||
       !Builder::expectLong( f, "py", initFile, m_pyCoeff) ||
       !Builder::expectLong( f, "pz", initFile, m_pzCoeff)    )
    return StatusCode::FAILURE;

  f.close();

  return StatusCode::SUCCESS;
}


uint32_t MucMcHitBuilder::getTEID(uint32_t reid)
{
  return 0;
}

 
uint32_t MucMcHitBuilder::getREID(uint32_t teid)
{
  return 0;
}

void MucMcHitBuilder::makeMucTruth
(MucMcHitCol::const_iterator& pMucMcHit, MucTruth_t& mt)
{
  Identifier ident = (*pMucMcHit)->identify();

  mt.trackIndex = (*pMucMcHit)->getTrackIndex() % 1000;
  mt.partId     = MucID::part(ident);
  mt.segId      = MucID::seg(ident);
  mt.gapId      = MucID::gap(ident);
  mt.stripId    = MucID::strip(ident);
  mt.x          = int((*pMucMcHit)->getPositionX() * m_xCoeff);
  mt.y          = int((*pMucMcHit)->getPositionY() * m_yCoeff);
  mt.z          = int((*pMucMcHit)->getPositionZ() * m_zCoeff);
  mt.px         = int((*pMucMcHit)->getPx() * m_pxCoeff);
  mt.py         = int((*pMucMcHit)->getPy() * m_pyCoeff);
  mt.pz         = int((*pMucMcHit)->getPz() * m_pzCoeff);

  return;
}

