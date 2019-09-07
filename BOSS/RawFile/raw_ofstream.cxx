#include "RawFile/raw_ofstream.h"
#include "RawFile/RawFileTools.h"
#include <cstdlib>

#define  MAX_RAWFILE_SiZE  2000000000  //an approximate value, not exactly

//static data members
int             raw_ofstream::_nHandler = 0;
raw_ofstream*   raw_ofstream::_instance = 0;
pthread_mutex_t raw_ofstream::_pthread_lock = PTHREAD_MUTEX_INITIALIZER;


raw_ofstream* raw_ofstream::instance(const std::string& fname)
{
   lock();

   if ( _instance == 0 ) {
      _instance = new raw_ofstream(fname);
   }

   ++_nHandler;

   unlock();

   return _instance;
}

void raw_ofstream::release()
{
   lock();

   if ( _nHandler > 0 && --_nHandler == 0 ) {
      delete _instance;
      _instance = 0;
   }

   unlock();
}

raw_ofstream::raw_ofstream(const std::string& fname)
   : m_nevt(0),
     m_nfile(0),
     m_fname(fname)
{
   init_fstream();
}

raw_ofstream::~raw_ofstream()
{
   this->close();
}

int raw_ofstream::write_event(const char* pbuf, int size)
{
   uint32_t fsize = tellp();
   if ( fsize >= MAX_RAWFILE_SiZE ) {
      this->close();
      init_fstream();
   }

   m_dataSeparatorRecord.setDataBlockNumber(++m_nevt);
   m_dataSeparatorRecord.setDataBlockSize(size);

   (*this) << m_dataSeparatorRecord;
   std::ofstream::write(pbuf, size);

   return m_nfile;
}

void raw_ofstream::close()
{
   if ( is_open() ) {
      m_fileEndRecord.setEventsInFile(m_nevt);
      m_nevt = 0;

      (*this) << m_fileEndRecord;
      std::ofstream::close();

      std::cout << "[RawFile] Finished writing file: " << real_fname() << std::endl;
   }
}

void raw_ofstream::init_fstream()
{
   ++m_nfile;

   std::string fname = real_fname();

   if ( access( fname.c_str(), F_OK ) == 0 ) {
      std::cerr << "[RawFile] Attempt to create an exist file: " << fname << std::endl;
      exit(1);
   }

   std::cout << "[RawFile] Creating a new file: " << real_fname() << std::endl;
   open( fname.c_str(), std::ios::binary );

   (*this) << m_fileStartRecord << m_fileNameStrings << m_runParametersRecord;
}

std::string raw_ofstream::real_fname()
{
   std::string fname = m_fname;

   if ( m_nfile > 1 ) {
      fname += ".part" + RawFileTools::itoa( m_nfile );
   }

   return fname;
}
