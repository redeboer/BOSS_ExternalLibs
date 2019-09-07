#ifndef SqliteInterface_h
#define SqliteInterface_h

#include "DbInterface.h"
#include <sqlite3.h>
#include <string>

class SqliteInterface : public DbInterface
{
 public:
  SqliteInterface();
  ~SqliteInterface();

  int connect();
  int select_db(std::string dbname);
  int query(std::string dbname, std::string query);
  int query(std::string dbname, std::string query, DatabaseRecordVector& records);
  int disconnect();

 protected:
  int connect(std::string fname);

 private:
  sqlite3 *m_conn;
}; 

#endif
