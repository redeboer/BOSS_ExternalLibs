#ifndef BESIII_EVTIDX_HANDLER_H
#define BESIII_EVTIDX_HANDLER_H

#include <vector>
#include <string>
#include <fstream>
#include <stdint.h>

class EvtIdxHandler
{
   public :

      static uint32_t IdxFileStartMarker() { return 0xFFFFAAAA; }
      static uint32_t IdxIdBlockMarker()   { return 0xFFFFBBBB; }
      static uint32_t IdxPosBlockMarker()  { return 0xFFFFCCCC; }

      static EvtIdxHandler* instance(const std::vector<std::string>& fnames);
      static void release();

      EvtIdxHandler();  // writing
      virtual ~EvtIdxHandler();

      // for reading
      void next_file();

      uint32_t  nextPos(int nIgnore);
      uint32_t  findPosById(uint32_t evtId);

      int nEvtLeft(int nIgnore) const {
	 return ( m_totEvt - (m_idxPos+nIgnore) );
      }

      // for constructing and writing
      void addPos(uint32_t evtId, uint32_t pos);
      void write(std::string fname);


   private :

      EvtIdxHandler(const std::vector<std::string>& idxfnames);  // reading

      void init_idx();  //only for reading
      void enlarge_block(int min_size);


   private :

      int        m_totEvt;
      int        m_idxPos;  //only for reading, no less than 0

      int        m_blockSize;
      uint32_t*  m_EIdBlock;
      uint32_t*  m_PosBlock;

      std::vector<std::string>            m_fnames;  //only for reading
      std::vector<std::string>::iterator  m_curFile;  //only for reading
      std::fstream                        m_fs;

      static int             _nHandler;
      static EvtIdxHandler*  _instance;
};

#endif
