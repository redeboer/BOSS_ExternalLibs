#ifndef ROOT_FILE_WRITER_H
#define ROOT_FILE_WRITER_H

#include "RootEventData/TBossFullEvent.h"
#include <string>

class TFile;
class TTree;
class TJobInfo;

class RootFileWriter
{
   public :

      typedef const std::string WriterArgType;

      RootFileWriter(WriterArgType& fname);
      virtual ~RootFileWriter();

      int writeEvent(void* pevt, int size);
      int close();

      int stat();


   private :

      RootFileWriter();  //stop default

      std::string getJobOptions();
      std::string getDecayOptions();


   private :

      TFile*           m_file;
      TTree*           m_tree;
      TTree*           m_jtree;
      TBossFullEvent*  m_evt;
      TJobInfo*        m_jobInfo;
      std::string      m_fname;

      bool             m_firstRound;
      int              m_stat;
};

#endif
