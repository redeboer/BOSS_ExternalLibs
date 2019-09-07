#include <iostream>
#include <fstream>
#include "Identifier/MdcID.h"
#include "RawDataCnv/EventManagement/MdcMcHitBuilder.h"
using namespace std;

MdcMcHitBuilder::MdcMcHitBuilder()
   :Builder()
{
  initialize (Builder::m_confFile);
}

void MdcMcHitBuilder::unPack(vector<uint32_t>::const_iterator& iter,
             vector<uint32_t>::const_iterator& eiter, MdcTruth_t& mt)
{
    uint32_t helpVal = *(iter++); assert(iter != eiter);
    mt.edep       = *(iter++); assert(iter != eiter);
    mt.driftD     = *(iter++); assert(iter != eiter);
    mt.x          = *(iter++); assert(iter != eiter);
    mt.y          = *(iter++); assert(iter != eiter);
    mt.z          = *(iter++);

    mt.trackIndex = (helpVal & m_trackIndexMask) >> m_trackIndexIndex;
    mt.layerId    = (helpVal & m_layerIdMask) >> m_layerIdIndex;
    mt.cellId     = (helpVal & m_cellIdMask) >> m_cellIdIndex;
    mt.posFlag    = (helpVal & m_posFlagMask) ? 1 : 0 ;

    return;
}

StatusCode MdcMcHitBuilder::pack(MdcMcHitCol* mdcMcHitCol, WriteRawEvent*& re)
{
/*
  if (mdcMcHitCol == NULL) {
    cout << "MdcMcHitBuilder::pack cant get MdcMcHitCol" << endl;
    return StatusCode::FAILURE;
  }

  MdcTruth_t m_MdcTruth;
  vector<uint32_t> *mdcReMcHitVec = new vector<uint32_t>;

  MdcMcHitCol::const_iterator pMdcMcHit = mdcMcHitCol->begin();
  for ( ; pMdcMcHit != mdcMcHitCol->end(); pMdcMcHit++) {
    // Make MdcTruth data
    makeMdcTruth(pMdcMcHit, m_MdcTruth);
    // pack the trackIndex, layerId and cellId.
    uint32_t helpVal = (m_MdcTruth.trackIndex<<m_trackIndexIndex) & m_trackIndexMask;
    helpVal |= ((m_MdcTruth.layerId<<m_layerIdIndex) & m_layerIdMask);
    helpVal |= ((m_MdcTruth.cellId<<m_cellIdIndex) & m_cellIdMask);
    if (m_MdcTruth.posFlag) helpVal |= ((0x1<<m_posFlagIndex) & m_posFlagMask);
    //fill the McHit Vector
    mdcReMcHitVec->push_back(helpVal);
    mdcReMcHitVec->push_back(m_MdcTruth.edep);
    mdcReMcHitVec->push_back(m_MdcTruth.driftD);
    mdcReMcHitVec->push_back(m_MdcTruth.x);
    mdcReMcHitVec->push_back(m_MdcTruth.y);
    mdcReMcHitVec->push_back(m_MdcTruth.z);
  }

  OfflineEventFormat::SubDetectorHeader sh(OfflineEventFormat::MDCTRUTH);
  SubRawEvent *sub = new SubRawEvent(sh, mdcReMcHitVec);
  re->append(sub);
  */

  return StatusCode::SUCCESS;
}

// initialize re2te tables

StatusCode  MdcMcHitBuilder::initialize(string& initFile)
{
  ifstream f;

  //read init file
  f.open(initFile.c_str());

  if ( f.bad() ) {
    cerr   << "Error: could not open file " << initFile << endl;
    return StatusCode::FAILURE;
  }

  if (!Builder::find( f, "##MdcTruthConf", initFile)) {
    cerr << "Error: could not find '##MdcTruthConf' in file " << initFile << endl;
    return StatusCode::FAILURE;
  }

  if ( !Builder::expect( f, "#MdcTruthShift", initFile) ||
       !Builder::expectInt( f, "trackIndex", initFile, m_trackIndexIndex, m_trackIndexMask) ||
       !Builder::expectInt( f, "layerId", initFile, m_layerIdIndex, m_layerIdMask) ||
       !Builder::expectInt( f, "cellId", initFile, m_cellIdIndex, m_cellIdMask) ||
       !Builder::expectInt( f, "posFlag", initFile, m_posFlagIndex, m_posFlagMask) ||
       !Builder::expect( f, "#MdcTruthCoeff", initFile) ||
       !Builder::expectLong( f, "edep", initFile, m_edepCoeff) ||
       !Builder::expectLong( f, "driftD", initFile, m_driftDCoeff) ||
       !Builder::expectLong( f, "x", initFile, m_xCoeff) ||
       !Builder::expectLong( f, "y", initFile, m_yCoeff) ||
       !Builder::expectLong( f, "z", initFile, m_zCoeff)    )
    return StatusCode::FAILURE;

  f.close();

  return StatusCode::SUCCESS;
}


uint32_t MdcMcHitBuilder::getTEID(uint32_t reid)
{
  return 0;
}

 
uint32_t MdcMcHitBuilder::getREID(uint32_t teid)
{
  return 0;
}

void MdcMcHitBuilder::makeMdcTruth
(MdcMcHitCol::const_iterator& iter, MdcTruth_t& mt)
{
  Identifier ident = (*iter)->identify();

  mt.trackIndex = (*iter)->getTrackIndex();
  mt.layerId    = MdcID::layer(ident);
  mt.cellId     = MdcID::wire(ident);
  mt.edep       = int((*iter)->getDepositEnergy() * m_edepCoeff);
  mt.driftD     = int((*iter)->getDriftDistance() * m_driftDCoeff);
  mt.x          = int((*iter)->getPositionX() * m_xCoeff);
  mt.y          = int((*iter)->getPositionY() * m_yCoeff);
  mt.z          = int((*iter)->getPositionZ() * m_zCoeff);
  mt.posFlag    = (*iter)->getPositionFlag();

  return;
}

