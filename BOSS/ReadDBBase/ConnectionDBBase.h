#ifndef REALDBUTIL_CONNECTIONBASE_H
#define REALDBUTIL_CONNECTIONBASE_H

#include "rdbModel/Db/Connection.h"
#include "rdbModel/Tables/Assertion.h"

namespace rdbModel {
  class Rdb;
  class Manager;
}

//namespace RealDBUtil {
  class ConnectionDBBase {
  public:
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
  
  
    /* Someday provide option of reading in description of table. In case
       it is used, can check that it matches dbs we're connected to, and
       can check column values.
    */
      
    /// Constructor keeps track of table of interest
    ConnectionDBBase(const std::string& host, const std::string& table, const std::string& dbName);
  
    ConnectionDBBase(const std::string& host, const std::string& dbName, const std::string& userName, const std::string& password);
  
    ~ConnectionDBBase();
        	
    bool connectRead(eRet& err);
  
    rdbModel::Connection* getReadConnection() {return m_readCxt;}
    
    // Should probably be const
    rdbModel::Rdb* getRdb() {return m_rdb;}
    const std::string& getTable() {return m_table;}
  
    // Might make these private
    void disconnectRead();
    void disconnectWrite();
  
  private:
  
    rdbModel::Connection* m_readCxt;
    rdbModel::Connection* m_writeCxt;
  
    // these could be static
    rdbModel::Assertion* m_findBest;
    rdbModel::Assertion* m_findSoonest;
  
    
  
    //    static bool connect(rdbModel::MysqlConnection* conn,
    static bool connect(rdbModel::Connection* conn,
                        std::string& host, 
                        const std::string& user, 
                        const std::string& pw,  eRet& err,
                        const std::string& dbName);
  
    bool connectWrite(eRet& err);
  
  
    std::string  m_host;
    std::string  m_table;
    std::string  m_dbName;
    std::string  m_userName;
    std::string  m_password;
  
    rdbModel::Manager* m_man;
    rdbModel::Rdb* m_rdb;
    bool     m_match; // true if succeeded.  If not attempted, m_rdb is 0
  };
//}
#endif
