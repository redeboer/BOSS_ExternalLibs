#ifndef JOBOPTIONS_MGR_H
#define JOBOPTIONS_MGR_H

#include <string>
#include <vector>

class Property;
class JobOptionsSvc;


class JobOptionsMgr
{
   public :

      JobOptionsMgr(const std::string& optname);
      virtual ~JobOptionsMgr();

      const std::string& optName() { return m_optName; }

      int inputType() { return m_iFileType; }
      int evtMax()   { return m_evtMax; }
      const std::vector<std::string>& inputFiles() { return m_inputFiles; }

      const std::vector<int>& outputTypes() { return m_oFileTypes; }
      const std::string& dstFile() { return m_dstFile; }
      const std::string& recFile() { return m_recFile; }
      const std::vector<std::string>& tupleOpts() { return m_tupleOpts; }
      const std::vector<std::string>& histOpts() { return m_histOpts; }

      void clientOptsTemplate(const std::string& svrName);


   private :

      const Property* getProperty(const std::string& client, const std::string& name);

      JobOptionsMgr();


   private :

      std::string                m_optName;

      int                        m_iFileType;
      int                        m_evtMax;
      std::vector<std::string>   m_inputFiles;

      std::vector<int>           m_oFileTypes;
      std::string                m_dstFile;
      std::string                m_recFile;
      std::vector<std::string>   m_tupleOpts;   //for users' Ntuples
      std::vector<std::string>   m_histOpts;    //for users' Hists

      JobOptionsSvc*   m_svc;
};

#endif
