#include "RawFile/raw_ifstream.h"
#include "IRawFile/RawFileExceptions.h"
#include <cstdlib>

//static data members
int             raw_ifstream::_nHandler = 0;
raw_ifstream*   raw_ifstream::_instance = 0;
pthread_mutex_t raw_ifstream::_pthread_lock = PTHREAD_MUTEX_INITIALIZER;


raw_ifstream* raw_ifstream::instance(const std::vector<std::string>& fnames)
{
   lock();

   if ( _instance == 0 ) {
      _instance = new raw_ifstream(fnames);
   }

   ++_nHandler;

   unlock();

   return _instance;
}

void raw_ifstream::release()
{
   lock();

   if ( _nHandler > 0 && --_nHandler == 0 ) {
      if ( _instance->m_isOpen ) {
	 std::cout << "[RawFile] Closing: " << *(_instance->m_curFile) << std::endl;
	 _instance->close();
      }
      delete _instance;
      _instance = 0;
   }

   unlock();
}

raw_ifstream::raw_ifstream(const std::vector<std::string>& fnames)
   : m_isOpen(false), m_fnames(fnames)
{
   if ( m_fnames.empty() ) {
      throw RawExMessage("[RawFile] Empty input file list!");
   }

   m_curFile = m_fnames.begin();
   init_fstream();
}

raw_ifstream::~raw_ifstream()
{
}

void raw_ifstream::next_file()
{
   std::cout << "[RawFile] Closing: " << *m_curFile << std::endl;
   close();
   m_isOpen = false;

   if ( ++m_curFile == m_fnames.end() ) {
      --m_curFile;
      throw ReachEndOfFileList();
   }

   init_fstream();
}

uint32_t raw_ifstream::runNo()
{
    return m_runParametersRecord.getRecord().run_number;
}

void raw_ifstream::init_fstream()
{
   if ( access( m_curFile->c_str(), F_OK ) < 0 ) {
      std::cerr << "[RawFile] Invalid file: " << *m_curFile << std::endl;
      exit(1);
   }

   std::cout << "[RawFile] Prepare for reading: " << *m_curFile << std::endl;

   open( m_curFile->c_str(), std::ios::binary );
   m_isOpen = true;

   try {
      (*this) >> m_fileStartRecord >> m_fileNameStrings >> m_runParametersRecord;
   }
   catch (RawFileException& e) {
      e.print();
      exit(1);
   }
}
