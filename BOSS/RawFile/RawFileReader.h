#ifndef BESIII_RAW_FILE_READER_H
#define BESIII_RAW_FILE_READER_H

#include "RawFile/RawFileUtil.h"
#include "IRawFile/IRawReader.h"
#include <vector>
#include <string>

//forward declaration
class raw_ifstream;
class EvtIdxHandler;
class RawFileException;

typedef std::vector<std::string> VFileNames_t;

class RawFileReader : public IRawReader
{
   public :

      typedef const VFileNames_t ReaderArgType;

      static std::vector<int> getEventNumber(const VFileNames_t& idxfnames);

      // thread safe
      RawFileReader(const std::string& fname);
      RawFileReader(const VFileNames_t& fnames);
      RawFileReader(const std::string& fname, const std::string& idxfname);
      RawFileReader(const VFileNames_t& fnames, const VFileNames_t& idxfnames);
      virtual ~RawFileReader();

      // thread safe
      const uint32_t*  nextEvent();
      const uint32_t*  currentEvent() const { return m_buffer; }

      // following interface are not thread safe !!!
      const uint32_t*  nextEvent(int nIgnore);
      const uint32_t*  findEventById(uint32_t evtId);
      const uint32_t*  roughlyNextEvent(int nIgnore, int evtByte = 0);

      uint32_t         runNo();

      std::string      currentFile();
      uint32_t         tellg();
      uint32_t         stat();


   private :

      const uint32_t* notSafeNextEvent();

      void            read_one_event();
      RawFileReader&  nextFile(RawFileException& e);

      RawFileReader();  //stop default


   private :

      uint32_t              m_bufferSize;
      uint32_t*             m_buffer;

      raw_ifstream*         m_rfs;
      EvtIdxHandler*        m_idxHandler;

      DataSeparatorRecord   m_dataSeparatorRecord;
};

#endif
