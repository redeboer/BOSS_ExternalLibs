// $Header: /bes/bes/BossCvs/Calibration/rdbModel/rdbModel/Db/MysqlResults.h,v 1.2 2008/04/09 02:57:28 huangb Exp $
#ifndef RDBMODEL_MYSQLRESULTS_H
#define RDBMODEL_MYSQLRESULTS_H

#include "rdbModel/Db/ResultHandle.h"

typedef struct st_mysql_res MYSQL_RES;

namespace rdbModel{

  class MysqlConnection;

  /** 
      Concrete implementation of ResultHandle, to accompany MysqlConnection.
  */
  class MysqlResults : virtual public ResultHandle {
    friend class MysqlConnection;

  public:
    virtual ~MysqlResults();

    /// Return number of rows in results
    virtual unsigned int getNRows() const;

    /**  
         Get array of field values for ith row of result set
    */
    virtual bool getRow(std::vector<std::string>& fields, unsigned int i = 0,
                        bool clear = true);

    /**  
         Get array of field values for ith row of result set.  If a field 
         value is NULL, return a zero ptr for that element of the array.

         --> It is the responsibility of the caller to delete the strings
             containing the field values.  See service cleanFieldPtrs
             in base class ResultHandle.
    */
    virtual bool getRowPtrs(std::vector<std::string*>& fieldPtrs, 
                            unsigned int i = 0, bool clear=true);
 
    virtual bool getRowCon(char* par,unsigned long* treesize,unsigned int *runFrm,
		           unsigned int *runTo,unsigned int i = 0, bool clear=true);
    /*
    // Return specified row in results as a string
    virtual bool getRowString(std::string& row, unsigned int iRow=0) const;

      Return vector of rows
         @param rows     to hold returned data
         @param iRow     starting row
         @param maxRow   maximum number of rows requested.  0 means "all"
         @param clear    if true, clear @a rows before storing new data
         @return         status
    
    virtual bool getRowStrings(std::vector<std::string>& rows, 
                               unsigned int iRow=0,
                               unsigned int maxRow=0, bool clear=true) const;
    */

  private:
    // Only MysqlConnection calls constructor
    MysqlResults(MYSQL_RES* results = 0); 

    MYSQL_RES* m_myres;
  };
}
#endif
