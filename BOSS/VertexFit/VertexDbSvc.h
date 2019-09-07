#ifndef VERTEXDBSVC_H_
#define VERTEXDBSVC_H_

#ifndef BEAN
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "VertexFit/IVertexDbSvc.h"
#include <mysql.h>
#include <map>
#include <vector>
#include "DatabaseSvc/IDatabaseSvc.h"
#include "GaudiKernel/IService.h"
//#include "rdbModel/Db/Connection.h"
//#include "rdbModel/Tables/Assertion.h"
#include "CLHEP/Matrix/Vector.h"
#include "DatabaseSvc/IDatabaseSvc.h"
using CLHEP::HepVector;
#else
#include <string>
#include "DatabaseSvc/DatabaseSvc.h"
#endif

#ifndef BEAN
class VertexDbSvc: public Service, virtual public IVertexDbSvc, 
		     virtual public IIncidentListener{
		     public:
  VertexDbSvc( const std::string& name, ISvcLocator* svcloc );
  ~VertexDbSvc();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // Incident handler
  void handle(const Incident&);

#else
// -------------------------- BEAN ------------------------------------
class VertexDbSvc
{
private:
                VertexDbSvc();
               ~VertexDbSvc(){}

public:
  static VertexDbSvc*   instance() {
    return (m_vdb) ? m_vdb : (m_vdb = new VertexDbSvc());
  }

  const DatabaseSvc*    GetDatabaseSvc() const          {return m_dbsvc;}

  void          SetDbName(const std::string& _dbName)   {dbName = _dbName;}
  std::string   GetDbName() const                       {return dbName;}
  void          SetBossVer(const std::string& _bossver) {m_bossver = _bossver;}
  std::string   GetBossVer() const                      {return m_bossver;}
  void          SetBossRelease(const std::string& _bossrelease) 
                                                {m_bossRelease = _bossrelease;}
  std::string   GetBossRelease() const                  {return m_bossRelease;}
  void          SetVerPar(const std::string& _verpar)   {m_verpar = _verpar;}
  std::string   GetVerPar() const                       {return m_verpar;}

  // New run handler
  void          handle(int new_run);
#endif

  double * PrimaryVertex();
  double * SigmaPrimaryVertex();
  bool     isVertexValid() {return m_isRunNumberValid;}

  private:
  // common variables for BOSS & BEAN
  std::string   dbName;
  std::string   m_bossver;
  std::string   m_verpar;
  std::string   m_bossRelease;
  double        m_primaryVertex[3];
  double        m_sigmaPrimaryVertex[3];
  bool          m_isRunNumberValid;  

#ifndef BEAN
  std::string host;
  std::string table;
  std::string userName;
  std::string password;
  unsigned int serialNo;
 // RealDBUtil::ConnectionProvider* m_connect_offline;
  double m_vx;
  double m_vy;
  double m_vz;
  double m_sigmax;
  double m_sigmay;
  double m_sigmaz;

  IDataProviderSvc* m_eventSvc;
  IDatabaseSvc* m_dbsvc;

  StatusCode getVertexTableInfo();
#else
  static VertexDbSvc*  m_vdb;

  DatabaseSvc*  m_dbsvc;

  void          getVertexTableInfo(int run);
#endif

  bool          getReadBunchInfo(int run);
};
#endif /* MDCCALIBFUNSVC_H_ */
