#include <iostream>
#include "RawFile/EvtIdxHandler.h"
#include <cstring>
#include <cstdlib>

#define DefaultIdxBlockSize 512*1024

int            EvtIdxHandler::_nHandler = 0;
EvtIdxHandler* EvtIdxHandler::_instance = 0;

EvtIdxHandler* EvtIdxHandler::instance(const std::vector<std::string>& idxfnames)
{
   if ( _instance == 0 ) {
      _instance = new EvtIdxHandler(idxfnames);
   }

   ++_nHandler;

   return _instance;
}

void EvtIdxHandler::release()
{
   if ( _nHandler > 0 && --_nHandler == 0 ) {
      delete _instance;
      _instance = 0;
   }
}

EvtIdxHandler::EvtIdxHandler(const std::vector<std::string>& idxfnames)
   : m_idxPos(0),
     m_blockSize(DefaultIdxBlockSize),
     m_fnames(idxfnames)
{
   m_EIdBlock = new uint32_t[m_blockSize];
   m_PosBlock = new uint32_t[m_blockSize];

   m_curFile = m_fnames.begin();
   init_idx();
}

EvtIdxHandler::EvtIdxHandler()
   : m_totEvt(0),
     m_idxPos(-1),
     m_blockSize(DefaultIdxBlockSize)
{
   m_EIdBlock = new uint32_t[m_blockSize];
   m_PosBlock = new uint32_t[m_blockSize];
}

EvtIdxHandler::~EvtIdxHandler()
{
   delete[] m_EIdBlock;
   delete[] m_PosBlock;
}

void EvtIdxHandler::next_file()
{
   ++m_curFile;

   m_idxPos = 0;
   init_idx();
}

uint32_t EvtIdxHandler::nextPos(int nIgnore)
{
   m_idxPos += nIgnore;
   return m_PosBlock[m_idxPos++];
}

uint32_t EvtIdxHandler::findPosById(uint32_t evtId)
{
   while ( m_totEvt > m_idxPos ) {
      if ( m_EIdBlock[m_idxPos] == evtId ) {
	 return m_PosBlock[m_idxPos++];
      }
      ++m_idxPos;
   }
   return 0;
}

void EvtIdxHandler::addPos(uint32_t evtId, uint32_t pos)
{
   ++m_totEvt;

   if ( m_totEvt > m_blockSize ) enlarge_block( m_totEvt );

   m_EIdBlock[m_totEvt-1] = evtId;
   m_PosBlock[m_totEvt-1] = pos;
}

void EvtIdxHandler::write(std::string fname)
{
   if ( m_idxPos >= 0 ) {
      std::cerr << "[RawFile] Writing an EvtIdxHandler for reading !!!" << std::endl;
      exit(1);
   }

   m_fs.open( fname.c_str(), std::ios::out | std::ios::binary );

   uint32_t marker = EvtIdxHandler::IdxFileStartMarker();
   m_fs.write( (char*)&marker, sizeof(marker) );
   m_fs.write( (char*)&m_totEvt, sizeof(m_totEvt) );

   marker = EvtIdxHandler::IdxIdBlockMarker();
   m_fs.write( (char*)&marker, sizeof(marker) );
   m_fs.write( (char*)m_EIdBlock, m_totEvt*sizeof(uint32_t) );

   marker = EvtIdxHandler::IdxPosBlockMarker();
   m_fs.write( (char*)&marker, sizeof(marker) );
   m_fs.write( (char*)m_PosBlock, m_totEvt*sizeof(uint32_t) );

   if ( ! m_fs.good() ) {
      std::cerr << "[RawFile] Error writing IDX file: " << fname << std::endl;
      exit(1);
   }

   m_fs.close();

   std::cout << "[RawFile] Written IDX file: " << fname << std::endl;

   m_totEvt = 0;
}

void EvtIdxHandler::init_idx()
{
   if ( access( m_curFile->c_str(), F_OK ) < 0 ) {
      std::cerr << "[RawFile] Invalid IDX file: " << *m_curFile << std::endl;
      exit(1);
   }

   std::cout << "[RawFile] Initialize IDX with file: " << *m_curFile << std::endl;

   m_fs.open( m_curFile->c_str(), std::ios::in | std::ios::binary );

   uint32_t marker;

   m_fs.read( (char*)&marker, sizeof(marker) );
   if ( marker != EvtIdxHandler::IdxFileStartMarker() ) {
      std::cerr << "[RawFile] Wrong IdxFileStartMarker!" << std::endl;
      exit(1);
   }

   m_fs.read( (char*)&m_totEvt, sizeof(m_totEvt) );

   if ( m_totEvt > m_blockSize ) enlarge_block( m_totEvt );

   m_fs.read( (char*)&marker, sizeof(marker) );
   if ( marker != EvtIdxHandler::IdxIdBlockMarker() ) {
      std::cerr << "[RawFile] Wrong IdxIdBlockMarker!" << std::endl;
      exit(1);
   }

   m_fs.read( (char*)m_EIdBlock, m_totEvt*sizeof(uint32_t) );

   m_fs.read( (char*)&marker, sizeof(marker) );
   if ( marker != EvtIdxHandler::IdxPosBlockMarker() ) {
      std::cerr << "[RawFile] Wrong IdxPosBlockMarker!" << std::endl;
      exit(1);
   }

   m_fs.read( (char*)m_PosBlock, m_totEvt*sizeof(uint32_t) );

   if ( ! m_fs.good() ) {
      std::cerr << "[RawFile] Error occured while initialize the IDX file!" << std::endl;
      exit(1);
   }

   m_fs.close();
}

void EvtIdxHandler::enlarge_block(int min_size)
{
   int _initSize = m_blockSize;
   while ( min_size > m_blockSize ) {
      m_blockSize *= 2;
   }

   uint32_t* _EIdBlock = new uint32_t[m_blockSize];
   uint32_t* _PosBlock = new uint32_t[m_blockSize];

   memcpy((void*)_EIdBlock, (const void*)m_EIdBlock, sizeof(uint32_t)*_initSize);
   memcpy((void*)_PosBlock, (const void*)m_PosBlock, sizeof(uint32_t)*_initSize);

   delete[] m_EIdBlock;
   delete[] m_PosBlock;

   m_EIdBlock = _EIdBlock;
   m_PosBlock = _PosBlock;
}
