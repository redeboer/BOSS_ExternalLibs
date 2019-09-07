// $Header: /bes/bes/BossCvs/Calibration/rdbModel/src/Db/MysqlResults.cxx,v 1.6 2011/02/22 06:16:29 maqm Exp $
#ifdef  WIN32
#include <windows.h>
#endif

#include "rdbModel/Db/MysqlResults.h"
#include "mysql.h"
#include "facilities/Util.h"
#include <iostream>
#include <cstring>   

namespace rdbModel {

  MysqlResults::MysqlResults(MYSQL_RES* results) : m_myres(results) {
  }

  MysqlResults::~MysqlResults() {
    mysql_free_result(m_myres);
  }

  unsigned int MysqlResults::getNRows() const {
    return mysql_num_rows(m_myres);
  }

  bool MysqlResults::getRow(std::vector<std::string>& fields, 
                            unsigned int i, bool clear) {
    mysql_data_seek(m_myres, i);
    MYSQL_ROW myRow = mysql_fetch_row(m_myres);

    unsigned nFields = mysql_num_fields(m_myres);


    if (clear) fields.clear();

    for (unsigned int iField = 0; iField < nFields; iField++) {
      if (myRow[iField]) fields.push_back(std::string(myRow[iField]));
      else fields.push_back("");
    }

    return true;
  }

  bool MysqlResults::getRowPtrs(std::vector<std::string*>& fields, 
                                unsigned int i, bool clear) {

    mysql_data_seek(m_myres, i);
    MYSQL_ROW myRow = mysql_fetch_row(m_myres);

    unsigned nFields = mysql_num_fields(m_myres);


    if (clear) fields.clear();

    for (unsigned int iField = 0; iField < nFields; iField++) {
      if (myRow[iField]) fields.push_back(new std::string(myRow[iField]));
      else fields.push_back(0);
    }

    return true;

  }


  bool MysqlResults::getRowCon(char* par,unsigned long* treesize,unsigned int* runFrm,
		           unsigned int* runTo,unsigned int i, bool clear){
    mysql_data_seek(m_myres, i);
    MYSQL_ROW myRow = mysql_fetch_row(m_myres);
    unsigned nFields = mysql_num_fields(m_myres);
    unsigned long* lengths;
    lengths = mysql_fetch_lengths(m_myres);

    for (unsigned int iField = 0; iField <nFields; iField++) {
      if (myRow[iField]&&iField<(nFields-4)){
	memcpy(par+1024000*iField,myRow[iField],lengths[iField]);
	treesize[iField] = lengths[iField];
      }
    }

    *runFrm =facilities::Util::stringToInt(myRow[nFields-4]);
    *runTo =facilities::Util::stringToInt(myRow[nFields-3]);
  std::cout<<" CalVerSft is "<<myRow[nFields-2]<<"  File name  is "<<myRow[nFields-1]<<std::endl;

    return true;

  }


  // May also want to do 
  //  MYSQL_FIELD* fields = mysql_fetch_fields(m_myres);
  // and make these available to client, especially for select * queries
}
