#include <iostream>
#include <fstream>
#include "RawDataCnv/EventManagement/McParticleBuilder.h"
using namespace std;

McParticleBuilder::McParticleBuilder()
   :Builder()
{
  //initialize(Builder::m_confFile);
}

void McParticleBuilder::unPack(uint32_t* buf, double* vPointer, McParticle* mcPar)
{
  uint32_t trkIndex = (buf[0]>>16);
  uint32_t vIndex0  = (buf[0]>>8) & 0xFF;
  uint32_t vIndex1  = buf[0] & 0xFF;

  double*  ptmp      = (double*)(buf+3);
  double*  v0Pointer = vPointer + 4*vIndex0;
  HepLorentzVector initMomentum(ptmp[0], ptmp[1], ptmp[2], ptmp[3]);
  HepLorentzVector initPosition(v0Pointer[0], v0Pointer[1], v0Pointer[2], v0Pointer[3]);

  mcPar->initialize(buf[1], buf[2], initMomentum, initPosition, "");
  mcPar->setTrackIndex(trkIndex);
  mcPar->setVertexIndex0(vIndex0);

  if (vIndex1 == 0xFF) {
    mcPar->setVertexIndex1(-99);
  }
  else {
    mcPar->setVertexIndex1(vIndex1);

    double* v1Pointer = vPointer + 4*vIndex1;
    HepLorentzVector finalPosition(v1Pointer[0], v1Pointer[1], v1Pointer[2], v1Pointer[3]);
    mcPar->finalize(finalPosition);
  }

  return;
}

StatusCode McParticleBuilder::pack(McParticleCol* mcParticleCol, WriteRawEvent*& re)
{
  if (mcParticleCol == 0 ) {
    cerr << "McParticleBuilder::pack can't get mcParticleCol" << endl;
    return StatusCode::FAILURE;
  }

  uint32_t nParticle = mcParticleCol->size();
  uint32_t nVertex = 0;
  vector<bool> vFlag(nParticle+1, false);
  double*  vPointer = (double*)(m_buf + nParticle*11 + 1);

  uint32_t index = 0;
  int tIndex, vIndex0, vIndex1;
  int parId;
  uint32_t statusFlag;

  m_buf[index++] = (nParticle<<16);

  McParticleCol::const_iterator itPar = mcParticleCol->begin();
  for( ; itPar != mcParticleCol->end(); itPar++) {
    tIndex  = (*itPar)->trackIndex();
    vIndex0 = (*itPar)->vertexIndex0();
    vIndex1 = (*itPar)->vertexIndex1();
    if (vIndex1 == -99) vIndex1 = 0xFF;
    parId   = (*itPar)->particleProperty();
    statusFlag = (*itPar)->statusFlags();
    const HepLorentzVector& p4vec    = (*itPar)->initialFourMomentum();
    m_buf[index++] = ((tIndex<<16) | (vIndex0<<8) | (vIndex1));
    m_buf[index++] = (uint32_t)parId;
    m_buf[index++] = statusFlag;
    double* ptmp = (double*)(m_buf+index);
    ptmp[0] = p4vec.x();
    ptmp[1] = p4vec.y();
    ptmp[2] = p4vec.z();
    ptmp[3] = p4vec.t();
    index += 8;

    if ( !vFlag[vIndex0] ) {
      const HepLorentzVector& initPos  = (*itPar)->initialPosition();
      vPointer[vIndex0*4 + 0] = initPos.px();
      vPointer[vIndex0*4 + 1] = initPos.py();
      vPointer[vIndex0*4 + 2] = initPos.pz();
      vPointer[vIndex0*4 + 3] = initPos.e();
      vFlag[vIndex0] = true;
      nVertex++;
    }

    if ( (vIndex1 != 0xFF) && (!vFlag[vIndex1]) ) {
      const HepLorentzVector& finalPos = (*itPar)->finalPosition();
      vPointer[vIndex1*4 + 0] = finalPos.px();
      vPointer[vIndex1*4 + 1] = finalPos.py();
      vPointer[vIndex1*4 + 2] = finalPos.pz();
      vPointer[vIndex1*4 + 3] = finalPos.e();
      vFlag[vIndex1] = true;
      nVertex++;
    }
  }

  m_buf[0] |= nVertex;

  append2event(re, 0xf10000, (nParticle*11 + nVertex*8 + 1));

  return StatusCode::SUCCESS;
}

// initialize re2te tables

StatusCode  McParticleBuilder::initialize(string& initFile)
{
  ifstream f;

  //read init file
  f.open(initFile.c_str());

  if ( f.bad() ) {
    cerr   << "Error: could not open file " << initFile << endl;
    return StatusCode::FAILURE;
  }

  if (!Builder::find( f, "##McParticleConf", initFile)) {
    cerr << "Error: could not find '##McParticleConf' in file " << initFile << endl;
    return StatusCode::FAILURE;
  }

  f.close();                    
                                  
  return StatusCode::SUCCESS;     
}
                                  
                                
uint32_t McParticleBuilder::getTEID(uint32_t /*reid*/)
{
  return 0;
}

 
uint32_t McParticleBuilder::getREID(uint32_t /*teid*/)
{
  return 0;
}
