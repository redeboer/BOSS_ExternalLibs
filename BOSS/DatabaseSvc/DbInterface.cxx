#include "DatabaseSvc/DbInterface.h"

DbInterface::DbInterface()
{
  m_isConnected = false;
  m_reuseConnection = false;
  m_dbName="offlinedb";
  m_dbPort = 3306;
}

DbInterface::~DbInterface()
{
}
