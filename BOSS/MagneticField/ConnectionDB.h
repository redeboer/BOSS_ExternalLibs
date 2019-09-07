#ifndef FIELDDBUTIL_CONNECTIONDB_H
#define FIELDDBUTIL_CONNECTIONDB_H

#include <string>
#include <vector>

#ifndef BEAN
#include "DatabaseSvc/IDatabaseSvc.h"
#include "DatabaseSvc/DatabaseSvc.h"
#endif

namespace FieldDBUtil {
  class ConnectionDB {
  public:

#ifndef BEAN
    /// Constructor keeps track of table of interest
    ConnectionDB();
#else
    ConnectionDB() {}
#endif

    ~ConnectionDB() {}
	
    enum eRet {
      RETOk = 0,
      RETBadCnfFile = 1,
      RETBadHost = 2,
      RETNoConnect = 3,
      RETWrongState = 4,
      RETBadValue = 5,
      RETMySQLError = 6,
      RETNoSchemaMatch = 7
    };
    /// Used to form bit masks for dbs queries
    enum eLevel {
      LEVELProd = 1,
      LEVELDev  = 2,
      LEVELTest = 4,
      LEVELSuperseded = 8
    };
	
    ConnectionDB::eRet getReadSC_MagnetInfo(std::vector<double>& current, int runNo);
    ConnectionDB::eRet getBeamEnergy( std::vector<double>& beamE, int runNo);

#ifndef BEAN
  private:
    IDatabaseSvc* m_dbsvc;
#endif
  };
}

#endif
