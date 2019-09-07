#include "DatabaseSvc/MysqlInterface.h"

#include <iostream>
#include <mysql.h>
#include <cstring>

using namespace std;

MysqlInterface::MysqlInterface(){}

MysqlInterface::~MysqlInterface(){}

int MysqlInterface::connect()
{
  if( m_reuseConnection )
    connect(m_dbHost, m_dbUser, m_dbPasswd, m_dbPort);
  return 0;
}

int MysqlInterface::connect(std::string host, std::string user, std::string passwd, int port)
{
  try {
    m_conn = new MYSQL;
    mysql_init(m_conn);
    MYSQL* ret = 0;
    int iattempt = 0;
    for(iattempt=0;iattempt<3;iattempt++)
    {
      ret = mysql_real_connect(m_conn, host.c_str(), user.c_str(), passwd.c_str(),m_dbName.c_str(),port,NULL,0);
      if (ret != 0) {  // Everything is fine.  Put out an info message
       std::cout << "DatabaseSvc: Connected to MySQL database" << std::endl;
       break;
      }   
      else {      
       std::cout << "Couldn't connect to MySQL database. Trying again." << std::endl;
       sleep(1);
      }
   }

   if(ret == 0 && iattempt == 2)
	throw (char*)mysql_error(m_conn);

  } catch ( std::exception &e ) {

    cerr << "Error in MySQL session initialization!" << endl;
    cerr << "*** std::exception caught:"              << endl;
    cerr << "***  error message:" << e.what()         << endl;
    return -1;
  
  } catch (...) { return -1; }

  m_isConnected = true;
  return 0;
}

int MysqlInterface::select_db(std::string dbname)
{
  int ret = mysql_select_db(m_conn, dbname.c_str());
  if ( ret != 0 ) {
      disconnect();
      // Try to re-connect
      sleep(5);
      ret = connect(m_dbHost, m_dbUser, m_dbPasswd, m_dbPort);
      if ( ret == 0 ) {
          ret = mysql_select_db(m_conn, dbname.c_str());
      }
      if (ret != 0) {
          throw std::exception();
      }
  }
  return 0;
}

int MysqlInterface::query(std::string dbname, std::string sql)
{
  if(! m_reuseConnection)
    connect(m_dbHost, m_dbUser, m_dbPasswd, m_dbPort);

  try{
    // check database name
    if(m_dbName != dbname)
      {
        m_dbName = dbname;
        select_db(m_dbName);
      }
    
    int status = mysql_real_query(m_conn, sql.c_str(), sql.length());
    if (status)
      {
	//        if(mysql_errno(m_conn)==2006){
	//  std::cerr << "MySQL error 2006: MySQL server has gone away"<< std::endl;
	std::cerr << "MySQL error: MySQL server has gone away"<< std::endl;
	disconnect();
	// Try to re-connect
	sleep(5);
	int ret = connect(m_dbHost, m_dbUser, m_dbPasswd, m_dbPort);
	if (ret == 0) {
	  std::cout << "Connected to MySQL database " << std::endl;
	  select_db(m_dbName);
	  status = mysql_real_query(m_conn, sql.c_str(), sql.length());
	}
	//  }

        if(status)
        {
          cerr << "Query " << sql << " failed: " << mysql_error(m_conn) << endl;
          return -1;
        }
      }
  } 
  catch(...)
    {
      cerr << "Could not execute query: " << mysql_error(m_conn)  << endl;
      return -1;
    }  

  if(! m_reuseConnection)
    disconnect();

  return 0;
}
 
int MysqlInterface::query(std::string dbname, std::string sql, DatabaseRecordVector& records)
{
  if(! m_reuseConnection)
    connect(m_dbHost, m_dbUser, m_dbPasswd, m_dbPort);


  records.clear();

  try{
    // check database name
    if(m_dbName != dbname)
      {
        m_dbName = dbname;
        select_db(m_dbName);
      }
    
    int status = mysql_real_query(m_conn, sql.c_str(), sql.length());
    if (status)
      {
//	if(mysql_errno(m_conn)==2006){ 
//         std::cerr << "MySQL error 2006: MySQL server has gone away"<< std::endl;
         std::cerr << "MySQL error: MySQL server has gone away"<< std::endl;
         disconnect();
	 // Try to re-connect
 	 sleep(5);
         int ret = connect(m_dbHost, m_dbUser, m_dbPasswd, m_dbPort);
         if (ret == 0) { 
             std::cout << "Connected to MySQL database " << std::endl;
             select_db(m_dbName);
             status = mysql_real_query(m_conn, sql.c_str(), sql.length());
         }
       // }
	
	if(status)
	{
          cerr << "Query " << sql << " failed: " << mysql_error(m_conn) << endl;
	  return -1;
        }
      }     
 
    MYSQL_RES* result = mysql_store_result(m_conn);

    if(result)
      {
        int num_fields = mysql_num_fields(result);
    
        if(num_fields>0)
          {
            MYSQL_FIELD *fields;
            fields = mysql_fetch_fields(result);
            
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result)))
              {
		unsigned long* lengths;
		lengths = mysql_fetch_lengths(result);
                DatabaseRecord* dbrec = new DatabaseRecord;
                int field;
                for(field = 0; field<num_fields; field++)
                  {
                    if(row[field]!=0)
                      {
                        unsigned long field_len = lengths[field];
                        char* new_record;
			if(fields[field].type==FIELD_TYPE_BLOB)
                        { 
                          new_record = new char[field_len];
                          memcpy(new_record, row[field], field_len);
			}
			else // strings
			{
	  		  new_record = new char[field_len+1];
           		  strcpy(new_record, row[field]);	
			}
			
			(*dbrec)[fields[field].name]=new_record;
		      }
                    else 
		      {
                        (*dbrec)[fields[field].name]='\0';
                      }
                  }
                records.push_back(dbrec);
              }
          }
        mysql_free_result(result);
        if(! m_reuseConnection)
	  disconnect();
	
        return records.size();
      }
  } 
  catch(...)
    {
      cerr << "Could not execute query: " << mysql_error(m_conn)  << endl;
      if(! m_reuseConnection)
        disconnect();
      return -1;
    }  

  if(! m_reuseConnection)
    disconnect();

  return 0;
}

int MysqlInterface::disconnect()
{
  if (m_conn)
    {
      mysql_close(m_conn);
      delete m_conn;
      m_conn = NULL;
    }
  m_isConnected = false;
  return 0;
}
