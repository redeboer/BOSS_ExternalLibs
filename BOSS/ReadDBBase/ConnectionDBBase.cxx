#include "ReadDBBase/ConnectionDBBase.h"
#include "facilities/Util.h"
#include "rdbModel/Management/Manager.h"
#include "rdbModel/Management/XercesBuilder.h"

#include "rdbModel/Db/MysqlConnection.h"
#include "rdbModel/Db/MysqlResults.h"
#include "rdbModel/Rdb.h"
#include "rdbModel/RdbException.h"
#include "rdbModel/Tables/Assertion.h"
#include "rdbModel/Tables/Table.h"
#include "rdbModel/Tables/Column.h"
#include <iostream>
#include <cstdio>


//namespace RealDBUtil {


  ConnectionDBBase::ConnectionDBBase(const std::string& host, const std::string& table, const std::string& dbName)  
    : m_readCxt(0), m_writeCxt(0),
      m_host(host), m_table(table), m_dbName(dbName), m_userName("maqm"), m_password("12345"), m_man(0), m_rdb(0),
      m_match(false) {
   // if (table.compare("*") == 0) m_table = std::string("$(MYSQL_METATABLE)");
   // if (host.compare("*") == 0) m_host = std::string("$(MYSQL_HOST)");

  }
  ConnectionDBBase::ConnectionDBBase(const std::string& host, const std::string& dbName, const std::string& userName, const std::string& password)
	: m_readCxt(0), m_writeCxt(0),
    	m_host(host), m_dbName(dbName), m_userName(userName), m_password(password),
	  m_man(0), m_rdb(0), m_match(false) {
  }

  ConnectionDBBase::~ConnectionDBBase() {
    disconnectRead();
    disconnectWrite();
    if (m_man) delete m_man;
  }



  // The next 5 methods concern connection to the server
  bool ConnectionDBBase::connect(rdbModel::Connection* cxt, std::string& host,
                         const std::string& user, 
                         const std::string& pw, eRet& err,
                         const std::string& dbName)  {


    bool connected = cxt->open(host, user, pw, dbName);
    if (connected) {
      err = RETOk;
      return true;
    }
    else {
      err = RETNoConnect;
      return false;
    }
  }

  bool ConnectionDBBase::connectRead(eRet& err) {
    if (m_readCxt == 0) {
      // for now use std::cout, std::cerr 
      m_readCxt = new rdbModel::MysqlConnection();
      bool ok = connect(m_readCxt, m_host, m_userName, 
                        m_password,  err, m_dbName);
      if (!ok) {
        delete m_readCxt;
        m_readCxt = 0;
      }  else {
      }
      return ok;
    }
    else return true;
  }

  bool ConnectionDBBase::connectWrite(eRet& err) {
    if (m_writeCxt == 0) {
      m_writeCxt = new rdbModel::MysqlConnection();
      bool ok;
     
      ok = connect(m_writeCxt, m_host, m_userName, 
                     m_password, err, m_dbName);

      if (!ok) {
        delete m_readCxt;
        m_readCxt = 0;
      }     else { 
      	
      }

      return ok;
    }
    else return true;
  }

  void ConnectionDBBase::disconnectRead() {
    if (m_readCxt) {
      m_readCxt->close();
      delete m_readCxt;
      m_readCxt = 0;
    }
  }

  void ConnectionDBBase::disconnectWrite() {
    if (m_writeCxt) {
      m_writeCxt->close();
      delete m_writeCxt;
      m_writeCxt = 0;
    }
  }
//}
