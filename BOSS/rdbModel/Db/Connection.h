// $Header: /bes/bes/BossCvs/Calibration/rdbModel/rdbModel/Db/Connection.h,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $
#ifndef RDBMODEL_CONNECTION_H
#define RDBMODEL_CONNECTION_H
#include <vector>
#include <string>


namespace rdbModel{

  /** The following are used as return codes from the function schemaMatch.
      which checks for compatibility between the schema definition embodied
      in an Rdb object and the one at the other end of a Connection.
      <ul>
      <li>MATCHequivalent means that every comparison attempted has succeeded
      </li>
      <li>MATCHcompatible means that everything required by the local 
      schema exists in the remote db, but not necessarily the other way around.
      selects, inserts and updates attempted via the Connection should work.
      </li>
      <li>MATCHfail means the remote db is missing something described locally,
      or types are incompatible for corresponding objects.
      </li>
      </ul>
  */
  enum MATCH  {
    MATCHequivalent,
    MATCHcompatible,
    MATCHfail,
    MATCHnoConnection
  };

  class ResultHandle;
  class Assertion;
  class Rdb;
  /** 
      Class to handle connection to an SQL database, or something very like
      it.  It should be able to
        - initiate connection
        - make queries, including making returned data from queries available
        - issue SQL statements such as INSERT and UPDATE which have no
          returned data other than status
        - close down connection.
      Someday it might also have a method to create a database

      Maybe make it pure virtual?  And make a MySQL implementation
      derived from it.  

      Initial design will just use host, password, userid passed in.
      Will be up to caller to insure that the userid has the right
      privilages for the operations caller intends to do.
  */
  typedef std::vector<std::string>  StringVector;

  class Connection {
    /** Open a connection 
        Allowed operations will depend on userid, etc., specified 
        return true if successful
    */
  public:
    Connection() {};
    virtual ~Connection() {};
    virtual bool open(const std::string& host, const std::string& userid,
                      const std::string& password,
                      const std::string& dbName) = 0;
                      //,                      unsigned int       port) = 0;
    /** Close the current open connection , if any.  Return true if there
     was a connection to close and it was closed successfully */
    virtual bool close() = 0;

    /** Parameter is normally path for an xml file descrbing the 
        connection parameters */
    virtual bool open(const std::string& parms) = 0;

    /// Return true iff open has been done with no matching close
    virtual bool isConnected() = 0;

    virtual std::ostream* getOut() const = 0;
    virtual std::ostream* getErrOut() const = 0;

    /**
       Check to what degree local schema definition is compatible with
       remote db accessed via this connection.  By default check db names
       match, but this may be disabled.
     */
    virtual MATCH matchSchema(Rdb *rdb, bool matchDbName=true) = 0;

    /** Typical derived class will form a syntactically correct 
        INSERT statement from the input arguments and issue it to
        the dbms. Return true if row was inserted successfully

        Might also want to add a routine for INSERT ... SELECT
    */
    virtual bool insertRow(const std::string& tableName, 
                           const StringVector& colNames, 
                           const StringVector& values,
                           int* auto_value=0,
                           const StringVector* nullCols = 0) = 0;

    /*
       So far anticipated uses of UPDATE would just modify a single row
       identified by ser_no (or, more generally, primary key), so
       make a method for this case.  Can call the more general
       version below.

       @return true if no errors were encountered; else false.  If the 
       update requested was error-free but entailed no actual change, 
       returns ??
     */
    /*
      At this level, don't have access to column name for primary 
      key, if any.  This has to be done by caller
    virtual bool updateUnique(const std::string& tableName, 
                              const StringVector& colNames, 
                              const StringVector& values,
                              const std::string& keyValue) = 0;
    */

    /**
      Generic UPDATE. Return value is number of rows changed.
    */
    virtual unsigned int update(const std::string& tableName, 
                                const StringVector& colNames, 
                                const StringVector& values,
                                const Assertion* where=0,
                                const StringVector* nullCols = 0) = 0;

    /**
      Support only for relatively simple SELECT, involving just
      one table.  
      @param tableName
      @param getCols   vector of columns to be retrieved
      @param where     ptr to an Assertion object
      @param rowLimit  max number of rows to return
      @param rowOffset offset for first row returned among those satisfying 
                       conditions; ignored if 0.
      @return If the SELECT succeeds, a pointer to an object which 
       manages the returned data; else 0.  Caller is responsible for
       deleting the ResultHandle object.
    */
    virtual ResultHandle* select(const std::string& tableName,
                                 const StringVector& getCols,
                                 const StringVector& orderCols,
                                 const Assertion* where=0,
                                 int   rowLimit=0,
                                 int   rowOffset=0)=0;

    /**
      Turn select and update into no-ops: output SQL string for
      debugging but don't change db 
    */
    virtual void disableModify(bool disable)=0;

    /** 
      Transmit raw request of any form to database.  If it is a request
      that returns results, those results will be stored in a newly-
      allocated ResultHandle and dbRequest will return a pointer to
      it. Otherwise dbRequest will return a null pointer.
      Throw an exception if request fails for any reason.
    */
    virtual ResultHandle* dbRequest(const std::string& request)=0;

    /**
      compile method for assertions.  Use it internally, but also make
      it publicly available so assertions belonging to a table
      can save the compiled version.
    */
    virtual bool compileAssertion(const Assertion* a, std::string& sqlString)
      const = 0;

  };

}  // end namespace rdbModel
#endif
