#include "DatabaseSvc/SqliteInterface.h"

#include <iostream>
#include <cstring>

#include <sqlite3.h>

using namespace std;
/*
static int callback(void *result, int argc, char **argv, char **azColName){
  int i;
  DatabaseRecordVector* recv = static_cast<DatabaseRecordVector*>(result);
  DatabaseRecord dbrec;
  for(i=0; i<argc; i++){
    //  printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    unsigned long field_len = 1;
    char* new_record = new char[];
    memcpy(new_record,argv[i],field_len);
    dbrec[azColName[i]]=new_record;
  }
  recv->push_back(dbrec);

  return 0;
}
*/
SqliteInterface::SqliteInterface(){}

SqliteInterface::~SqliteInterface(){}

int SqliteInterface::connect()
{
  return 0;
}

int SqliteInterface::connect(std::string fname)
{
  int status = sqlite3_open_v2(fname.c_str(), &m_conn, SQLITE_OPEN_READONLY,0);
  if( status ){
    cerr << "Can't open database " << fname << ": "
	 << sqlite3_errmsg(m_conn) << endl;
    cerr.flush();
    sqlite3_close(m_conn);
    return -1;
  }

  //std::cout << "Open database " << fname << std::endl;

  m_isConnected = true;

  return 0;
}

int SqliteInterface::select_db(std::string dbname)
{
  if(m_isConnected)
  {
    if(m_dbName != dbname) // need to change db
	disconnect();
  }

  m_dbName = dbname;
  string fname = m_dbPath+"/"+m_dbName+".db";

  if(! m_isConnected)
    {
      int status = connect(fname);
      if(status<0)
	return -1;
    }

  return 0;
}

int SqliteInterface::query(std::string dbname, std::string sql)
{
    DatabaseRecordVector dummy;
    int status = query(dbname, sql, dummy);
    return status;
}

int SqliteInterface::query(std::string dbname, std::string sql, DatabaseRecordVector& records)
{
  char *zErrMsg = 0;
 // char ***pazResult;
 // int pnRow;
 // int pnColumn;

  int status = select_db(dbname);
  if(status<0)
    return -1;

  records.clear();
 // status = sqlite3_exec(m_conn, sql.c_str(), callback, &records, &zErrMsg);
  sqlite3_stmt *ppStmt;
  sqlite3_prepare_v2(m_conn, sql.c_str(), -1, &ppStmt, 0);

  while ( true ) 
  {
    status = sqlite3_step(ppStmt); 
    if(status == SQLITE_DONE)
    {
      break;
    }

    if(status == SQLITE_ROW)
    {
      DatabaseRecord* dbrec = new DatabaseRecord;
	
      //loop over columns
      int ncolumns = sqlite3_column_count(ppStmt);
      int i;
      for(i=0; i<ncolumns; i++)
      {
        // create new record
        unsigned long field_len = sqlite3_column_bytes(ppStmt,i);
        char* new_record; 
	const char* col_name = sqlite3_column_name(ppStmt,i);
        char column_name[255];
	strcpy(column_name,col_name); 
	int type = sqlite3_column_type(ppStmt,i);
        if(type == SQLITE_BLOB)
	{
	  new_record = new char[field_len];
	  char* col_result = (char*)sqlite3_column_blob(ppStmt,i);
	  memcpy(new_record,col_result,field_len);
	}
	else if (type != SQLITE_NULL)
	{
	  new_record = new char[field_len+1];
	  char* col_result = (char*)sqlite3_column_text(ppStmt,i);
	  strcpy(new_record,col_result);
	}
        else 
        {
          new_record = NULL; 
        }
        (*dbrec)[column_name] = new_record;
      }
    
      records.push_back(dbrec);

      continue;
    }

   // anything else means that error happened
   cerr << "SQLITE query error: " << zErrMsg << endl;   
   return -1;
  }

  sqlite3_free(zErrMsg);
  sqlite3_finalize(ppStmt);
//  if(! m_reuseConnection)
  disconnect();

  return records.size();
}

int SqliteInterface::disconnect()
{
  int res = sqlite3_close(m_conn);
  if (res)
     cerr << "ERROR: Cannot close connection to " << m_dbName << endl;
  m_isConnected = false;
  return 0;
}
