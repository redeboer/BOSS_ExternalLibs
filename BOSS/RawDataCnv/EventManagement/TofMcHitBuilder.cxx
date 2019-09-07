#include <iostream>
#include <fstream>
#include "Identifier/TofID.h"
#include "RawDataCnv/EventManagement/TofMcHitBuilder.h"
using namespace std;

TofMcHitBuilder::TofMcHitBuilder()
   :Builder()
{
  initialize(Builder::m_confFile);
}

void TofMcHitBuilder::unPack(vector<uint32_t>::const_iterator& iter,
             vector<uint32_t>::const_iterator& eiter, TofTruth_t& tt)
{
  uint32_t helpVal = *(iter++); assert(iter != eiter);
  tt.x          = *(iter++); assert(iter != eiter);
  tt.y          = *(iter++); assert(iter != eiter);
  tt.z          = *(iter++); assert(iter != eiter);
  tt.px         = *(iter++); assert(iter != eiter);
  tt.py         = *(iter++); assert(iter != eiter);
  tt.pz         = *(iter++); assert(iter != eiter);
  tt.trackLength= *(iter++); assert(iter != eiter);
  tt.time       = *(iter++);

  tt.trackIndex = (helpVal&m_trackIndexMask) >> m_trackIndexIndex;
  tt.partId     = (helpVal&m_partIdMask) >> m_partIdIndex; 
  tt.scinNb     = (helpVal&m_scinNbMask) >> m_scinNbIndex;
  tt.strip      = (helpVal&m_stripMask)  >> m_stripIndex;

  return;
}


StatusCode TofMcHitBuilder::pack(TofMcHitCol* tofMcHitCol, WriteRawEvent*& re)
{
/*
  if (tofMcHitCol == NULL) {
    cout << "TofMcHitBuilder::pack cant get TofMcHitCol" << endl;
    return StatusCode::FAILURE;
  }

  vector<uint32_t> *tofReMcHitVec = new vector<uint32_t>;
  TofTruth_t m_TofTruth;

  TofMcHitCol::const_iterator pTofMcHit = tofMcHitCol->begin();
  for ( ; pTofMcHit != tofMcHitCol->end(); pTofMcHit++) {
    // make TofTruth_t data
    makeTofTruth(pTofMcHit, m_TofTruth);
    //pack the int numbers
    uint32_t helpVal = (m_TofTruth.trackIndex<<m_trackIndexIndex) & m_trackIndexMask;
    helpVal |= ((m_TofTruth.partId<<m_partIdIndex) & m_partIdMask);
    helpVal |= ((m_TofTruth.scinNb<<m_scinNbIndex) & m_scinNbMask);
    //fill the McHit vector
    tofReMcHitVec->push_back(helpVal);
    tofReMcHitVec->push_back(m_TofTruth.x);
    tofReMcHitVec->push_back(m_TofTruth.y);
    tofReMcHitVec->push_back(m_TofTruth.z);
    tofReMcHitVec->push_back(m_TofTruth.px);
    tofReMcHitVec->push_back(m_TofTruth.py);
    tofReMcHitVec->push_back(m_TofTruth.pz);
    tofReMcHitVec->push_back(m_TofTruth.trackLength);
    tofReMcHitVec->push_back(m_TofTruth.time);
  }

  OfflineEventFormat::SubDetectorHeader sh(OfflineEventFormat::TOFTRUTH);
  SubRawEvent *sub = new SubRawEvent(sh, tofReMcHitVec);
  re->append(sub);
*/

  return StatusCode::SUCCESS;
}

// initialize re2te tables

StatusCode  TofMcHitBuilder::initialize(string& initFile)
{
  ifstream f;

  //read init file
  f.open(initFile.c_str());

  if ( f.bad() ) {
    cerr   << "Error: could not open file " << initFile << endl;
    return StatusCode::FAILURE;
  }

  if (!Builder::find( f, "##TofTruthConf", initFile)) {
    cerr << "Error: could not find '##TofTruthConf' in file " << initFile << endl;
    return StatusCode::FAILURE;
  }

  if ( !Builder::expect( f, "#TofTruthShift", initFile) ||
       !Builder::expectInt( f, "trackIndex", initFile, m_trackIndexIndex, m_trackIndexMask) ||
       !Builder::expectInt( f, "partId", initFile, m_partIdIndex, m_partIdMask) ||
       !Builder::expectInt( f, "scinNb", initFile, m_scinNbIndex, m_scinNbMask) ||
       !Builder::expectInt( f, "strip",  initFile, m_stripIndex,  m_stripMask) ||
       !Builder::expect( f, "#TofTruthCoeff", initFile) ||
       !Builder::expectLong( f, "x", initFile, m_xCoeff) ||
       !Builder::expectLong( f, "y", initFile, m_yCoeff) ||
       !Builder::expectLong( f, "z", initFile, m_zCoeff) ||
       !Builder::expectLong( f, "px", initFile, m_pxCoeff) ||
       !Builder::expectLong( f, "py", initFile, m_pyCoeff) ||
       !Builder::expectLong( f, "pz", initFile, m_pzCoeff) ||
       !Builder::expectLong( f, "trackLength", initFile, m_trackLengthCoeff) ||
       !Builder::expectLong( f, "time", initFile, m_timeCoeff)   )
    return StatusCode::FAILURE;

  f.close();

  return StatusCode::SUCCESS;
}


uint32_t TofMcHitBuilder::getTEID(uint32_t reid)
{
  return 0;
}

 
uint32_t TofMcHitBuilder::getREID(uint32_t teid)
{
  return 0;
}

void TofMcHitBuilder::makeTofTruth
(TofMcHitCol::const_iterator& pTofMcHit, TofTruth_t& tt)
{
  Identifier ident = (*pTofMcHit)->identify();
  uint32_t partId  = TofID::barrel_ec(ident);
  uint32_t scinNum = 0;
  uint32_t strip   = 0;

  if( TofID::is_scin(ident) ) {
    uint32_t layer = TofID::layer(ident);
    scinNum = TofID::phi_module(ident);
    if (TofID::is_barrel(layer)) scinNum += TofID::getPHI_BARREL_MAX() + 1;
  }
  else if( TofID::is_mrpc(ident) ) {
    uint32_t endcap = TofID::endcap(ident);
    scinNum = TofID::module(ident);
    if( TofID::is_barrel(endcap) ) partId = 4;
    strip = TofID::strip(ident);
  }

  tt.trackIndex = (*pTofMcHit)->getTrackIndex();
  tt.partId     = partId;
  tt.scinNb     = scinNum;
  tt.strip      = strip;
  tt.x          = int((*pTofMcHit)->getPositionX() * m_xCoeff);
  tt.y          = int((*pTofMcHit)->getPositionY() * m_yCoeff);
  tt.z          = int((*pTofMcHit)->getPositionZ() * m_zCoeff);
  tt.px         = int((*pTofMcHit)->getPx() * m_pxCoeff);
  tt.py         = int((*pTofMcHit)->getPy() * m_pyCoeff);
  tt.pz         = int((*pTofMcHit)->getPz() * m_pzCoeff);
  tt.trackLength= int((*pTofMcHit)->getTrackLength() * m_trackLengthCoeff);
  tt.time       = int((*pTofMcHit)->getFlightTime() * m_timeCoeff);

  return;
}

