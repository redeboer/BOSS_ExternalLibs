#include <iostream>
#include <fstream>
#include "Identifier/EmcID.h"
#include "RawDataCnv/EventManagement/EmcMcHitBuilder.h"
using namespace std;

EmcMcHitBuilder::EmcMcHitBuilder()
   :Builder()
{
  initialize(Builder::m_confFile);
}

void EmcMcHitBuilder::unPack(vector<uint32_t>::const_iterator& iter,
               vector<uint32_t>::const_iterator& eiter, EmcTruth_t& et)
{
  uint32_t helpVal = *(iter++); assert(iter != eiter);
  et.x       = *(iter++); assert(iter != eiter);
  et.y       = *(iter++); assert(iter != eiter);
  et.z       = *(iter++); assert(iter != eiter);
  et.px         = *(iter++); assert(iter != eiter);
  et.py         = *(iter++); assert(iter != eiter);
  et.pz         = *(iter++); assert(iter != eiter);
  et.totalEdep  = *(iter++);

  et.trackIndex = (helpVal&m_trackIndexMask) >> m_trackIndexIndex;
  et.partId     = (helpVal&m_partIdMask) >> m_partIdIndex;
  et.numTheta   = (helpVal&m_numThetaMask) >> m_numThetaIndex;
  et.numPhi     = (helpVal&m_numPhiMask) >> m_numPhiIndex;

  return;
}


StatusCode EmcMcHitBuilder::pack(EmcMcHitCol* emcMcHitCol, WriteRawEvent*& re)
{
  /*
  if (emcMcHitCol == NULL) {
    cout << "EmcMcHitBuilder::pack cant get EmcMcHitCol" << endl;
    return StatusCode::FAILURE;
  }

  vector<uint32_t> *emcReMcHitVec = new vector<uint32_t>;
  EmcTruth_t m_EmcTruth;

  EmcMcHitCol::const_iterator pEmcMcHit = emcMcHitCol->begin();
  for ( ; pEmcMcHit != emcMcHitCol->end(); pEmcMcHit++) {
    // Make the EmcTruth Data
    makeEmcTruth(pEmcMcHit, m_EmcTruth);
    // pack
    uint32_t helpVal = (m_EmcTruth.trackIndex<<m_trackIndexIndex) & m_trackIndexMask;
    helpVal |= ((m_EmcTruth.partId<<m_partIdIndex) & m_partIdMask);
    helpVal |= ((m_EmcTruth.numTheta<<m_numThetaIndex) & m_numThetaMask);
    helpVal |= ((m_EmcTruth.numPhi<<m_numPhiIndex) & m_numPhiMask);
    // fill the McHit vector
    emcReMcHitVec->push_back(helpVal);
    emcReMcHitVec->push_back(m_EmcTruth.x);
    emcReMcHitVec->push_back(m_EmcTruth.y);
    emcReMcHitVec->push_back(m_EmcTruth.z);
    emcReMcHitVec->push_back(m_EmcTruth.px);
    emcReMcHitVec->push_back(m_EmcTruth.py);
    emcReMcHitVec->push_back(m_EmcTruth.pz);
    emcReMcHitVec->push_back(m_EmcTruth.totalEdep);
  }

  OfflineEventFormat::SubDetectorHeader sh(OfflineEventFormat::EMCTRUTH);
  SubRawEvent *sub = new SubRawEvent(sh, emcReMcHitVec);
  re->append(sub);
  */

  return StatusCode::SUCCESS;
}

// initialize re2te tables

StatusCode  EmcMcHitBuilder::initialize(string& initFile)
{
  ifstream f;

  //read init file
  f.open(initFile.c_str());

  if ( f.bad() ) {
    cerr   << "Error: could not open file " << initFile << endl;
    return StatusCode::FAILURE;
  }

  if (!Builder::find( f, "##EmcTruthConf", initFile)) {
    cerr << "Error: could not find '##EmcTruthConf' in file " << initFile << endl;
    return StatusCode::FAILURE;
  }

  if ( !Builder::expect( f, "#EmcTruthShift", initFile) ||
       !Builder::expectInt( f, "trackIndex", initFile, m_trackIndexIndex, m_trackIndexMask) ||
       !Builder::expectInt( f, "partId", initFile, m_partIdIndex, m_partIdMask) ||
       !Builder::expectInt( f, "numTheta", initFile, m_numThetaIndex, m_numThetaMask) ||
       !Builder::expectInt( f, "numPhi", initFile, m_numPhiIndex, m_numPhiMask) ||
       !Builder::expect( f, "#EmcTruthCoeff", initFile) ||
       !Builder::expectLong( f, "x", initFile, m_xCoeff) ||
       !Builder::expectLong( f, "y", initFile, m_yCoeff) ||
       !Builder::expectLong( f, "z", initFile, m_zCoeff) ||
       !Builder::expectLong( f, "px", initFile, m_pxCoeff) ||
       !Builder::expectLong( f, "py", initFile, m_pyCoeff) ||
       !Builder::expectLong( f, "pz", initFile, m_pzCoeff) ||
       !Builder::expectLong( f, "totalEdep", initFile, m_totalEdepCoeff)  )
    return StatusCode::FAILURE;

  f.close();

  return StatusCode::SUCCESS;
}


uint32_t EmcMcHitBuilder::getTEID(uint32_t reid)
{
  return 0;
}

 
uint32_t EmcMcHitBuilder::getREID(uint32_t teid)
{
  return 0;
}

void EmcMcHitBuilder::makeEmcTruth
(EmcMcHitCol::const_iterator& pEmcMcHit, EmcTruth_t& et)
{
  Identifier ident = (*pEmcMcHit)->identify();

  et.trackIndex = (*pEmcMcHit)->getTrackIndex();
  et.partId     = EmcID::barrel_ec(ident);
  et.numTheta   = EmcID::theta_module(ident);
  et.numPhi     = EmcID::phi_module(ident);
  et.x          = int((*pEmcMcHit)->getPositionX() * m_xCoeff);
  et.y          = int((*pEmcMcHit)->getPositionY() * m_yCoeff);
  et.z          = int((*pEmcMcHit)->getPositionZ() * m_zCoeff);
  et.px         = int((*pEmcMcHit)->getPx() * m_pxCoeff);
  et.py         = int((*pEmcMcHit)->getPy() * m_pyCoeff);
  et.pz         = int((*pEmcMcHit)->getPz() * m_pzCoeff);
  et.totalEdep  = int((*pEmcMcHit)->getDepositEnergy() * m_totalEdepCoeff);

  return;
}

