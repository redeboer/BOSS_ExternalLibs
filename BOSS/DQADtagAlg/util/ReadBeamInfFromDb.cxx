#include "DQADtagAlg/util/ReadBeamInfFromDb.h"
#include <string.h>
#include <cstdio>

ReadBeamInfFromDb::ReadBeamInfFromDb() :
  m_run(-1),
  m_isRunValid(false),
  m_beamE(0){
  }

MYSQL* ReadBeamInfFromDb::OpenDb() const {
  
  const char host[]     = "bes3db2.ihep.ac.cn";
  const char user[]     = "guest";
  const char passwd[]   = "guestpass";
  const char db[]       = "run";
  unsigned int port_num = 3306;

  MYSQL* mysql = mysql_init(NULL);
  mysql = mysql_real_connect(mysql, host, user, passwd, db, port_num,
                             NULL,  // socket
                             0);    // client_flag

  if (mysql == NULL) {
    fprintf(stderr, "can not open database: offlinedb\n");
  }

  return mysql;
}


void ReadBeamInfFromDb::CloseDb(MYSQL* mysql) const {
  mysql_close(mysql);
}


void ReadBeamInfFromDb::ReadDb(int run){
  
  m_run=run;
  m_isRunValid = false;
  
  MYSQL* mysql=OpenDb();
  
  char stmt[1024];
  snprintf(stmt, 1024,
           "select BER_PRB, BPR_PRB "
	  "from RunParams where run_number = %d", run);
  if (mysql_real_query(mysql, stmt, strlen(stmt))) {
    fprintf(stderr, "query error\n");
    return ;
  }
  
  
  MYSQL_RES* result_set = mysql_store_result(mysql);
  MYSQL_ROW row = mysql_fetch_row(result_set);
  if (!row) {
    fprintf(stderr, "cannot find data for RunNo %d\n", run);
    return ;
  }
  
  m_isRunValid = true;

  double E_E=0, E_P=0;
  sscanf(row[0], "%lf", &E_E);
  sscanf(row[1], "%lf", &E_P);
  
  m_beamE=(E_E+E_P)/2.0;
  
  // Free the memory to prevent memory leak!
  mysql_free_result(result_set);
  
  CloseDb(mysql);
}

bool ReadBeamInfFromDb::isRunValid(int run) {
  if (run == -1 || m_run != run) {
    ReadDb(run);
  }

  return m_isRunValid;
}


double ReadBeamInfFromDb::getbeamE(int run){
  if (!isRunValid(run)){
    fprintf(stderr, "ERROR in ReadBeamInfFromDb: runNo is invalid!\n");
  }
  return m_beamE;
}


