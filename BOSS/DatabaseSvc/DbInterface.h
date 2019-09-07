#ifndef DbInterface_h
#define DbInterface_h

#include <string>
#include <vector>
#include <map>

#include "DatabaseSvc/DatabaseRecord.h"

class DbInterface
{
 public:
  DbInterface();
  virtual ~DbInterface();

  // Connect to db
  virtual int connect() = 0;
  // Select database to be used
  virtual int select_db(std::string dbname) = 0;
  // Query which returns data (SELECT, SHOW etc.)
  virtual int query(std::string dbname, std::string query, DatabaseRecordVector& records) = 0;
  // Query wich does not return data (INSERT, UPDATE etc.)
  virtual int query(std::string dbname, std::string query) = 0;   
  // Disconnect from db
  virtual int disconnect() = 0;

  bool is_connected(){ return m_isConnected; }  

  void set_host(std::string host) {m_dbHost=host;};
  void set_user(std::string user) {m_dbUser=user;};
  void set_passwd(std::string passwd) {m_dbPasswd=passwd;};
  void set_dbpath(std::string path) {m_dbPath=path;};
  void set_port(int port) {m_dbPort=port;};
  void set_reuse_connection(bool flag) { m_reuseConnection=flag;};

 protected:
  bool m_isConnected;
  bool m_reuseConnection;

  std::string m_dbName;

  std::string m_dbHost;
  int         m_dbPort;
  std::string m_dbUser;
  std::string m_dbPasswd;
  std::string m_dbPath;

};

#endif
