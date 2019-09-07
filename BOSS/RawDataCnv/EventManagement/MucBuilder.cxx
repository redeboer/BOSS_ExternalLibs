#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "Identifier/MucID.h" 
#include "RawDataCnv/EventManagement/MucBuilder.h"

using namespace std;

MucBuilder::MucBuilder()
  :Builder()
{
  if (!m_fec2idFile.empty()) return;
  m_propMgr.declareProperty("Fec2IdMap", m_fec2idFile);

  // Get a pointer to the Job Options Service
  IJobOptionsSvc* jobSvc;
  Gaudi::svcLocator()->service("JobOptionsSvc", jobSvc);
  jobSvc->setMyProperties("MucBuilder", &m_propMgr);

  m_re2te = new uint32_t[1024];
  strSqcVec = new uint32_t[1024];
  memset((void*)m_re2te, 0xFF, 1024*sizeof(uint32_t));
  memset((void*)strSqcVec, 0, 1024*sizeof(uint32_t));

  initialize (m_fec2idFile);
}

MucBuilder::~MucBuilder()
{
   delete m_re2te;
   delete strSqcVec;
}

void MucBuilder::unPack(uint32_t reDigi, uint32_t& REId, uint32_t& FecData, uint32_t& StrSqc)
{
  uint32_t fecAddr = (reDigi&0xFFFF0000) >> 16;
  uint32_t module = (fecAddr&0xF800) >> 5;
  REId = (fecAddr&0x07FF) | module;
  FecData = reDigi&0xFFFF;
  StrSqc = strSqcVec[REId];
  return;
}

StatusCode MucBuilder::pack(MucDigiCol* digiCol, WriteRawEvent*& re)
{
  if (digiCol == 0 ) {
    cerr << "MdcBuilder::pack can't get digiCol" << endl;
    return StatusCode::FAILURE;
  }

  uint32_t size = 0, index;
  uint32_t teid, reid, channel;

  MucDigiCol::const_iterator pMucDigi = digiCol->begin();
  for (pMucDigi; pMucDigi!= digiCol->end(); pMucDigi++) {
    teid = (*pMucDigi)->getIntId();
    reid = getREID(teid&0xFFFFFFF0);
    channel = (strSqcVec[reid] == 0) ? (0x8000>>(teid&0xF)) : (0x1<<(teid&0xF));
    // set the value if the current reid has been in the buffer
    for (index = 0; index < size; index++) {
      if ((m_buf[index]>>16) == reid) {
	m_buf[index] |= channel;
	break;
      }
    }
    // otherwise, append the current reid and value to the buffer
    if (index == size) m_buf[size++] = ((reid << 16) | (channel));
  }

  append2event(re, 0xa40000, size);

  return StatusCode::SUCCESS;
}

// initialize re2te tables
StatusCode  MucBuilder::initialize(string& fec2idFile)
{
  ifstream fs(fec2idFile.c_str());
  if( fs.bad() ) {
     cerr << "Error: could not open file " << fec2idFile << endl;
     return false;
  }

  fs.ignore(256, '\n');

  int fec, part, seg, layer, firstStr, strSqc;

  for (int i = 0; i < 572; i++) {
    fs >> fec >> part >> seg >> layer >> firstStr >> strSqc;
    // reid ... teid
    uint32_t reid = fec;  // ?
    uint32_t teid = MucID::getIntID(part, seg, layer, firstStr);
    //if (strSqc < 0) teid |= 0x00F00000;  // local teid

    m_re2te[reid] = teid;
    strSqcVec[reid] = strSqc + 1;
    m_te2reMap.insert(TE2REMAP::value_type(teid, reid));
    //cout << "teid " << teid << "  reid " << reid << "  strSqc " << strSqcVec[reid] << endl;
  }

  return StatusCode::SUCCESS;
}

uint32_t MucBuilder::getREID(uint32_t teid)
{
  TE2REMAP::iterator itr = m_te2reMap.find(teid);
 
  if (itr == m_te2reMap.end()) {
      cout << "wrong teid in muc " << teid << endl;
      exit(8);
  }

  return itr->second;
}

