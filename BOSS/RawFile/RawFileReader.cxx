#include "RawFile/RawFileReader.h"
#include "RawFile/raw_ifstream.h"
#include "RawFile/EvtIdxHandler.h"
#include "RawFile/RawFileTools.h"
#include "IRawFile/RawFileExceptions.h"
#include <cstdlib>

#define  DefaultEventBufferSize  1024*32
#define  EstimatedEventSize      1024*14

std::vector<int> RawFileReader::getEventNumber(const VFileNames_t& idxfnames)
{
   uint32_t itmp[2];
   std::vector<int> vNevt;

   const VFileNames_t newfnames = RawFileTools::wildcard_correct(idxfnames);
   VFileNames_t::const_iterator it = newfnames.begin();
   while ( it != newfnames.end() ) {
      if ( access( it->c_str(), F_OK ) < 0 ) {
	 std::cerr << "[RawFile] Invalid IDX file: " << *it << std::endl;
	 exit(1);
      }

      std::ifstream fs( it->c_str(), std::ios::binary );

      fs.read( (char*)(itmp), sizeof(uint32_t)*2 );
      if ( itmp[0] != EvtIdxHandler::IdxFileStartMarker() ) {
	 std::cerr << "[RawFile] Wrong IdxFileStartMarker!" << std::endl;
	 exit(1);
      }
      vNevt.push_back(itmp[1]);
      ++it;
   }

   return vNevt;
}

RawFileReader::RawFileReader(const std::string& fname)
   : m_bufferSize(DefaultEventBufferSize),
     m_buffer( new uint32_t[DefaultEventBufferSize] ),
     m_idxHandler(0)
{
   const VFileNames_t newfnames = RawFileTools::wildcard_correct(fname);

   m_rfs =  raw_ifstream::instance(newfnames);
}

RawFileReader::RawFileReader(const VFileNames_t& fnames)
   : m_bufferSize(DefaultEventBufferSize),
     m_buffer( new uint32_t[DefaultEventBufferSize] ),
     m_idxHandler(0)
{
   const VFileNames_t newfnames = RawFileTools::wildcard_correct(fnames);

   m_rfs =  raw_ifstream::instance(newfnames);
}

RawFileReader::RawFileReader(const std::string& fname, const std::string& idxfname)
   : m_bufferSize(DefaultEventBufferSize),
     m_buffer( new uint32_t[DefaultEventBufferSize] )
{
   const VFileNames_t newfnames    = RawFileTools::wildcard_correct(fname);
   const VFileNames_t newidxfnames = RawFileTools::wildcard_correct(idxfname);

   if ( newidxfnames.size() != newfnames.size() ) {
      std::cerr << "[RawFile] Num(IdxFiles) != Num(DataFiles)" << std::endl;
      exit(1);
   }

   m_rfs =  raw_ifstream::instance(newfnames);
   m_idxHandler = EvtIdxHandler::instance(newidxfnames);
}

RawFileReader::RawFileReader(const VFileNames_t& fnames, const VFileNames_t& idxfnames)
   : m_bufferSize(DefaultEventBufferSize),
     m_buffer( new uint32_t[DefaultEventBufferSize] )
{
   const VFileNames_t newfnames    = RawFileTools::wildcard_correct(fnames);
   const VFileNames_t newidxfnames = RawFileTools::wildcard_correct(idxfnames);

   if ( newidxfnames.size() != newfnames.size() ) {
      std::cerr << "[RawFile] Num(IdxFiles) != Num(DataFiles)" << std::endl;
      exit(1);
   }

   m_rfs =  raw_ifstream::instance(newfnames);
   m_idxHandler = EvtIdxHandler::instance(newidxfnames);
}

RawFileReader::~RawFileReader()
{
   delete[] m_buffer;
   raw_ifstream::release();
   EvtIdxHandler::release();
}

const uint32_t* RawFileReader::nextEvent()
{
   // thread safe
   raw_ifstream::lock();

   try {
      notSafeNextEvent();
   }
   catch ( ReachEndOfFileList& e) {
      raw_ifstream::unlock();
      throw e;
   }
   catch ( RawFileException& e) {
      raw_ifstream::unlock();
      e.print();
      throw e;
   }

   raw_ifstream::unlock();

   return m_buffer;
}

const uint32_t* RawFileReader::nextEvent(int nIgnore)
{
   // not thread safe !!!
   int nnIgnore = nIgnore;

   try {
      if ( m_idxHandler != 0 ) {
	 int nleft = m_idxHandler->nEvtLeft(nnIgnore);
	 if ( nleft > 0 ) {
	    m_rfs->seekg( m_idxHandler->nextPos( nnIgnore ) );
	    nnIgnore = 0;
	 }
	 else {
	    nnIgnore = -nleft;
	    throw ReachEndOfFile( currentFile().c_str() );
	 }
      }
      else {
	 while ( nnIgnore > 0 ) {
	    (*m_rfs) >> m_dataSeparatorRecord;
	    uint32_t size = m_dataSeparatorRecord.getRecord().data_block_size;
	    m_rfs->seekg( size + m_rfs->tellg() );
	    --nnIgnore;
	 }
      }

      read_one_event();
   }
   catch (RawFileException& e) {
      nextFile(e).nextEvent(nnIgnore);
   }

   return m_buffer;
}

const uint32_t* RawFileReader::findEventById(uint32_t evtId)
{
   // not thread safe !!!
   // find an event by ID ( only backward !!! )
   try {
      if ( m_idxHandler == 0 ) {
	 while ( true ) {
	    read_one_event();
	    uint32_t curEvtId = m_buffer[ m_buffer[5] + 8 ];
	    if ( curEvtId == evtId ) {
	       break;
	    }
	 }
      }
      else {
	 uint32_t pos = m_idxHandler->findPosById( evtId );
	 if ( pos != 0 ) {
	    m_rfs->seekg( pos );
	    read_one_event();
	 }
	 else {
	    throw ReachEndOfFile( currentFile().c_str() );
	 }
      }
   }
   catch (RawFileException& e) {
      return nextFile(e).findEventById(evtId);
   }

   return m_buffer;
}

const uint32_t* RawFileReader::roughlyNextEvent(int nIgnore, int evtByte)
{
   // not thread safe !!!
   if ( evtByte == 0 ) evtByte = EstimatedEventSize;

   assert( (evtByte&3) == 0 );

   uint32_t prePos = m_rfs->tellg();
   m_rfs->seekg( prePos + nIgnore*evtByte);

   uint32_t halfEvtWord = evtByte / 8;
   uint32_t halfEvtByte = halfEvtWord * 4;

   while ( m_rfs->read((char*)m_buffer, halfEvtByte).good() ) {
      uint32_t i = 0;
      while ( i < halfEvtWord && m_buffer[i] != 0x1234cccc ) {
	 ++i;
      }
      if ( i < halfEvtWord ) {
	 uint32_t curPos = m_rfs->tellg();
	 m_rfs->seekg( curPos - (halfEvtWord-i)*4 );
	 read_one_event();
	 return m_buffer;
      }
   }

   // reached the end of current data file! m_rfs->eof()
   m_rfs->clear();
   m_rfs->seekg( -40, std::ios::end );  //sizeof(FileEndRecord) == 40
   uint32_t curPos = m_rfs->tellg();

   int nnIgnore = nIgnore - (curPos - prePos) / evtByte;
   if ( nnIgnore < 0 ) nnIgnore = 0;

   ReachEndOfFile e( currentFile().c_str() );
   return nextFile(e).roughlyNextEvent(nnIgnore, evtByte);
}
uint32_t RawFileReader::runNo()
{
    return m_rfs->runNo();
}

std::string RawFileReader::currentFile()
{
   return m_rfs->currentFile();
}

uint32_t RawFileReader::tellg()
{
   return m_rfs->tellg();
}

uint32_t RawFileReader::stat()
{
   uint32_t stat = 0;

   if ( m_rfs->eof() )  stat |= 1;
   if ( m_rfs->fail() ) stat |= 2;
   if ( m_rfs->bad() )  stat |= 4;

   return stat;
}

const uint32_t* RawFileReader::notSafeNextEvent()
{
   try {
      read_one_event();
   }
   catch (RawFileException& e) {
      nextFile(e).notSafeNextEvent();
   }

   return m_buffer;
}

void RawFileReader::read_one_event()
{
   (*m_rfs) >> m_dataSeparatorRecord;

   uint32_t size = m_dataSeparatorRecord.getRecord().data_block_size;
   if ( size > m_bufferSize*4 ) {
      while ( size > m_bufferSize*4 ) {
	 m_bufferSize *= 2;
      }
      delete[] m_buffer;
      m_buffer = new uint32_t[m_bufferSize];
   }

   m_rfs->read((char*)m_buffer, size);
   if ( ! m_rfs->good() ) {
      //std::cerr << "[RawFile] Failed to read FullEventFragment to buffer" << std::endl;
      throw BadInputStream("event_data_block");
   }
}

RawFileReader& RawFileReader::nextFile(RawFileException& e)
{
   e.print();

   m_rfs->clear();
   m_rfs->next_file();

   if ( m_idxHandler != 0 ) {
      m_idxHandler->next_file();
   }

   return *this;
}
