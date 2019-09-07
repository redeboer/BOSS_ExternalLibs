#include "DatabaseSvc/DatabaseSvc.h"
#include "DatabaseSvc/SqliteInterface.h"

#ifndef BEAN
  #include "DatabaseSvc/DbInterface.h"
  #include "GaudiKernel/MsgStream.h"
  #include "GaudiKernel/SvcFactory.h"

  #include "DatabaseSvc/MysqlInterface.h"
#else
  #include <iostream>
  #include <cstdlib>
#endif

#include <algorithm>

using namespace std;

#ifndef BEAN
//----------------------------------------------------------------------------

DatabaseSvc::DatabaseSvc( const std::string& name, ISvcLocator* sl ) : Service( name, sl )
{
  // Set the name of this service
  if ( IDatabaseSvc::g_serviceInUse != "" ) {
    std::ostringstream error;
    error << "There is another IDatabaseSvc registered with name "
          << IDatabaseSvc::g_serviceInUse << std::ends;
    throw "Error in DatabaseSvc: "+error.str();
  }
  IDatabaseSvc::g_serviceInUse = "DatabaseSvc";

  // declare properties
  declareProperty("Host",m_dbHost="");
  declareProperty("User",m_dbUser="guest");
  declareProperty("Passwd",m_dbPasswd="guestpass");
  declareProperty("Port", m_dbPort=3306);
  declareProperty("SqliteDbPath",m_dbFilePath="");
  declareProperty("DbType",m_dbType="SQLITE");
  declareProperty("ReuseConnection",m_dbReuseConnection=false);
}

//----------------------------------------------------------------------------

DatabaseSvc::~DatabaseSvc()
{
  if (dbInterface)
    delete dbInterface;
}

//----------------------------------------------------------------------------

StatusCode
DatabaseSvc::queryInterface( const InterfaceID& riid, void** ppvInterface )
{
  if ( IID_IDatabaseSvc == riid ) {
    *ppvInterface = static_cast< IDatabaseSvc* >( this );
    return StatusCode::SUCCESS;
  } else {
    return Service::queryInterface( riid, ppvInterface );
  }
}

//----------------------------------------------------------------------------

StatusCode
DatabaseSvc::initialize()
{
  StatusCode status = Service::initialize();
  if ( !status.isSuccess() ) return status;

  MsgStream log( msgSvc(), name() );
  IDatabaseSvc::g_serviceInUse = name();
  setProperties();

  bool use_sqlite = false;
  bool use_mysql = false;

  std::transform(m_dbType.begin(), m_dbType.end(), m_dbType.begin(), ::toupper);

  if(m_dbType=="MYSQL")
    use_mysql = true;

  if(m_dbType=="SQLITE")
    use_sqlite = true;

  log << MSG::DEBUG << "Using " << m_dbType
      << " interface with options:" << endreq;

  try {
    if(use_mysql)
      {
	log << MSG::DEBUG << "	dbHost " << m_dbHost << endreq;
	log << MSG::DEBUG << "	dbPort " << m_dbPort << endreq;
	log << MSG::DEBUG << "	dbUser " << m_dbUser << endreq;
	log << MSG::DEBUG << "	dbPasswd " << m_dbPasswd << endreq;

	dbInterface = new MysqlInterface();
	dbInterface->set_host(m_dbHost);
	dbInterface->set_port(m_dbPort);
	dbInterface->set_user(m_dbUser);
	dbInterface->set_passwd(m_dbPasswd);
      }
    else if(use_sqlite)
      {
	log << MSG::DEBUG << "	dbFilepath " << m_dbFilePath << endreq;

	dbInterface = new SqliteInterface();
	dbInterface->set_dbpath(m_dbFilePath);
      }
    else
      {
	log << MSG::FATAL << "No valid database type is set. Please choose either MYSQL or SQLITE " << endreq;
	return StatusCode::FAILURE;
      }

    if(m_dbReuseConnection)
      log << MSG::DEBUG << "One connection per job is used" << endreq;
    else
      log << MSG::DEBUG << "One connection per query is used" << endreq;

    if( m_dbReuseConnection )
      {
	dbInterface->set_reuse_connection(true);
	dbInterface->connect();
      }

  } catch ( std::exception &e ) {

    log << MSG::FATAL << "Exception in DataSvc initialization:" << endreq;
    log << MSG::FATAL << "***  error message: " << e.what()         << endreq;
    return StatusCode::FAILURE;

  } catch (char* mess) {
    log << MSG::FATAL << "Exception DataSvc initialization caught: " << mess << endreq;
    return StatusCode::FAILURE;
  }
  catch (...) {
    log << MSG::FATAL << "UNKNOWN exception in DataSvc session initialization caught" << endreq;
    return StatusCode::FAILURE;
  }

  log << MSG::INFO << "DatabaseSvc initialized successfully" << endreq;
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

StatusCode
DatabaseSvc::finalize()
{
  MsgStream log( msgSvc(), name() );
  StatusCode status = Service::finalize();
  if ( ! status.isSuccess() ) {
    log << MSG::ERROR << "Unable to finalize the Service" << endreq;
    return status;
  }

  if(dbInterface)
    {
      if(dbInterface->is_connected())
	dbInterface->disconnect();
      delete dbInterface;
      dbInterface = NULL;
    }

  log << MSG::INFO << "DatabaseSvc finalized successfully" << endreq;
  return StatusCode::SUCCESS;
}

#else
// -------------------------- BEAN ------------------------------------

DatabaseSvc* DatabaseSvc::m_db = 0;

DatabaseSvc::DatabaseSvc()
{
  // m_dbFilePath="unknown";
  if( !this->initialize() ) {
    exit(0);
  }
}

//----------------------------------------------------------------------------

DatabaseSvc::~DatabaseSvc()
{
  this->finalize();
}

//----------------------------------------------------------------------------

bool
DatabaseSvc::initialize()
{
  bool exit_code = true;

  try {
    dbInterface = new SqliteInterface();
    dbInterface->set_dbpath(m_dbFilePath);
    dbInterface->set_reuse_connection(true);
    dbInterface->connect();
  }
  catch ( std::exception &e ) {
    cerr << " Exception in DatabaseSvc initialization:" << endl
         << " ***  error message: " << e.what()         << endl;
    exit_code = false;
  }
  catch (char* mess) {
    cerr << " Exception DatabaseSvc initialization caught: "
         << mess << endl;
    exit_code = false;
  }
  catch (...) {
    cerr << "UNKNOWN exception in DatabaseSvc session initialization caught"
         << endl;
    exit_code = false;
  }

  return exit_code;
}

//----------------------------------------------------------------------------

void
DatabaseSvc::finalize()
{
  if( dbInterface ) {
    if(dbInterface->is_connected())
      dbInterface->disconnect();
    delete dbInterface;
    dbInterface = 0;
  }
}

//----------------------------------------------------------------------------

void
DatabaseSvc::SetDBFilePath(std::string dbFilePath)
{
  m_dbFilePath = dbFilePath;
  dbInterface->set_dbpath(m_dbFilePath);
}

#endif
//----------------------------------------------------------------------------

int
DatabaseSvc::query(const std::string& dbName, const std::string& sql,
                                                DatabaseRecordVector& result)
{
#ifndef BEAN
  MsgStream log( msgSvc(), name() );

  //maqm log << MSG::DEBUG << "Query database " << dbName << " SQL: " << sql << endreq;
#endif

  result.clear();

  try{
    int status = dbInterface->query(dbName, sql, result);
    if (status<0)
      {
#ifndef BEAN
	log << MSG::ERROR << "Query " << sql << " failed" << endreq;
#else
        cout << "Query " << sql << " failed" << endl;
#endif
	return -1;
      }
  }
  catch(...)
    {
#ifndef BEAN
      log << MSG::ERROR << "Could not execute query " << sql << endreq;
#else
      cout << "Could not execute query " << sql << endl;
#endif
      return -1;
    }

  return result.size();
}

//----------------------------------------------------------------------------

int DatabaseSvc::query(const std::string& dbName, const std::string& sql)
{
#ifndef BEAN
  MsgStream log( msgSvc(), name() );

  log << MSG::DEBUG << "Query database " << dbName << " SQL: " << sql << endreq;
#endif

  try{
    int status = dbInterface->query(dbName, sql);
    if (status<0)
      {
#ifndef BEAN
	log << MSG::ERROR << "Query " << sql << " failed" << endreq;
#else
        cerr << "Query " << sql << " failed" << endl;
#endif
	return -1;
      }
  }
  catch(...)
    {
#ifndef BEAN
      log << MSG::ERROR << "Could not execute query " << sql << endreq;
#else
      cerr << "Could not execute query " << sql << endl;
#endif
      return -1;
    }

  return 0;
}

//----------------------------------------------------------------------------
