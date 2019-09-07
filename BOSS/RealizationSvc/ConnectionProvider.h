#ifndef REALDBUTIL_CONNECTIONPROVIDER_H
#define REALDBUTIL_CONNECTIONPROVIDER_H

#include "DatabaseSvc/IDatabaseSvc.h"
#include "DatabaseSvc/DatabaseSvc.h"
#include "DatabaseSvc/DatabaseRecord.h"
#include <vector>
#include <string>

namespace RealDBUtil {

  template <class type>
  std::string toString(type obj)
  {
    std::ostringstream tmp;
    tmp<<obj;
    return tmp.str();
  }

  class ConnectionProvider {
public:
    /// Constructor keeps track of table of interest
//    ConnectionProvider(const std::string& host="202.122.35.54",
//             const std::string& table="metadata_v0",
//             const std::string& dbName="calib");

    ConnectionProvider();

    ~ConnectionProvider();

    enum eRet {
      RETOk = 0,
      RETBadCnfFile = 1,
      RETBadHost = 2, 
      RETNoConnect = 3,
      RETWrongState = 4,
      RETBadValue = 5,
      RETMySQLError = 6,
      RETNoSchemaMatch = 7
    };
    /// Used to form bit masks for dbs queries
    enum eLevel {
      LEVELProd = 1,
      LEVELDev  = 2,
      LEVELTest = 4,
      LEVELSuperseded = 8
    };
		
    ConnectionProvider::eRet getReadLumInfo( std::string& Lum, int runNo, std::string SftVer, std::string ParVer, std::string BossRelease);
    ConnectionProvider::eRet getReadBunchInfo( std::vector<std::string>& bunch, int runNo,std::string SftVer, std::string ParVer, std::string BossRelease);
    ConnectionProvider::eRet getReadTrgTableInfo( std::vector<std::string>& trgTable, int runNo);
    //ConnectionDBBase::eRet getReadSC_MagnetInfo(unsigned int serialNo, std::vector<std::string>& current, int runNo);
    //ConnectionProvider::eRet getReadBackgroundInfo( std::vector<std::string>& fileInfor, std::vector<int>& vRanTrgEvtNum, int runNo);
    ConnectionProvider::eRet getReadBackgroundInfo( std::vector<std::string>& fileInfor, int runNo);
    //ConnectionProvider::eRet getReadBackgroundInfo( std::vector<std::string>& fileInfor, std::vector<int>& vRanTrgEvtNum,std::string query);
    ConnectionProvider::eRet getReadBackgroundInfo( std::vector<std::string>& fileInfor, std::string query);
    ConnectionProvider::eRet getReadTofThreshInfo( std::vector<std::string>& tofThresh, int runNo);
    ConnectionProvider::eRet getRunInfo( std::vector<std::string>& runInfo, int runNo);
    ConnectionProvider::eRet getLumCurvePar(std::string& runTotalTime, std::string& tau_value, int runNo, std::string SftVer, std::string ParVer, std::string BossRelease);
    ConnectionProvider::eRet getEmcGain(std::vector<double>& emcGain, int runNo);

    std::vector<std::string> split(const std::string& src, std::string delimit, std::string null_subst="");
    bool getcal(int runNo, std::string ids);

    ConnectionProvider::eRet getTrgConfigInfo(int runNo);

    int getEtotDataSteps() { return m_EtotDataSteps; }
    int getVthBEtotH() { return m_VthBEtotH; }
    int getVthEEtotH() { return m_VthEEtotH; }
    int getVthEtotL() { return m_VthEtotL; }
    int getVthEtotM() { return m_VthEtotM; }
    int getVthBLZ() { return m_VthBLZ; }
    int getVthDiffB() { return m_VthDiffB; }
    int getVthDiffE() { return m_VthDiffE; }
    int getVthBalBLK() { return m_VthBalBLK; }
    int getVthBalEEMC() { return m_VthBalEEMC; }
    int getVthDiffMin() { return m_VthDiffMin; }

  private:
    IDatabaseSvc* m_dbsvc;

    int m_trgTpye;
    int m_tfee_fileid;
    std::vector<std::string> m_trgTable;
    std::vector<std::string> m_tofThresh;

    int id_num;
    std::string* trgGain[6500];

    //trigger config infor.
    int m_EtotDataSteps;
    int m_VthBEtotH;
    int m_VthEEtotH;
    int m_VthEtotL;
    int m_VthEtotM;
    int m_VthBLZ;
    int m_VthDiffB;
    int m_VthDiffE;
    int m_VthBalBLK;
    int m_VthBalEEMC;
    int m_VthDiffMin;

  };
}

#endif
