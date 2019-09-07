#ifndef DATABASESVC_H
#define DATABASESVC_H

#ifndef BEAN
#include "GaudiKernel/Service.h"
#include "DatabaseSvc/IDatabaseSvc.h"

#include <mysql.h>

class DbInterface;

template <class TYPE> class SvcFactory;

class DatabaseSvc : virtual public IDatabaseSvc,
  virtual public Service
{

  // Allow SvcFactory to instantiate the service.
  friend class SvcFactory<DatabaseSvc>;

 //maqm protected:
	public:
  // Constructor
  DatabaseSvc( const std::string& name, ISvcLocator* sl );
  
  // Destructor
  virtual ~DatabaseSvc();
  
 public:

  // IInterface: query
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  
  // Service: initialize
  virtual StatusCode initialize();

  // Service: finalize
  virtual StatusCode finalize();

#else
// -------------------------- BEAN ------------------------------------
#include "DatabaseSvc/DatabaseRecord.h"
#include "DatabaseSvc/DbInterface.h"

class DatabaseSvc
{
 public:
  static DatabaseSvc*   instance() {
    return (m_db) ? m_db : (m_db = new DatabaseSvc());
  }

 private:
                        DatabaseSvc();
  virtual              ~DatabaseSvc();

  bool                  initialize();
  void                  finalize();

 public:
  void                  SetDBFilePath(std::string dbFilePath);
  std::string           GetDBFilePath() const   {return m_dbFilePath;}
#endif

  // make query 
  int query(const std::string& dbName, const std::string& sql);
  int query(const std::string& dbName, const std::string& sql, 
                                        DatabaseRecordVector& res);
  
 private:

  DbInterface * dbInterface;  
  std::string m_dbName;
  
  std::string m_dbHost;
  int         m_dbPort;
  std::string m_dbUser;
  std::string m_dbPasswd;
  std::string m_dbFilePath;
  std::string m_dbType;
  bool m_dbReuseConnection;

#ifdef BEAN
  static DatabaseSvc* m_db;
#endif
};

#endif
