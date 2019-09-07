#ifndef MysqlInterface_h
#define MysqlInterface_h

#include "DatabaseSvc/DbInterface.h"
#include <mysql.h>

class MysqlInterface : public DbInterface
{
 public:
  MysqlInterface();
  ~MysqlInterface();

  int connect();
  int select_db(std::string dbname);
  int query(std::string dbname, std::string query);
  int query(std::string dbname, std::string query, DatabaseRecordVector& records);
  int disconnect();

 protected:
  int connect(std::string host, std::string user, std::string passwd, int port=3306);

 private:
  MYSQL* m_conn;
}; 

#endif
