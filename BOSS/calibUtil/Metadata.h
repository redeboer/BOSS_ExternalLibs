// $Header: /bes/bes/BossCvs/Calibration/calibUtil/calibUtil/Metadata.h,v 1.20 2012/01/05 03:53:49 zhangy Exp $
#ifndef CALIBUTIL_METADATA_H
#define CALIBUTIL_METADATA_H

#include "facilities/Timestamp.h"
#include "rdbModel/Db/Connection.h"
#include "rdbModel/Tables/Assertion.h"
//#include "DatabaseSvc/DatabaseRecord.h"
#include "DatabaseSvc/IDatabaseSvc.h"

namespace rdbModel {
  class Rdb;
  class Manager;
}

namespace calibUtil {
  /** Provide interface between calibration clients and the
   MySQL database for calibration metadata.  Supported operations
   include writing a new record, looking for the serial number of
   a "best match" record (findBest), and a method for retrieving 
   the information necessary to read the data set corresponding to
   a particular metadata record (getReadInfo). Writing a record 
   is done in stages:  open the record (openRecord), add certain 
   fields (addValidInterval, addNotes, etc.), and finally to write
   the full record to the database (insertRecord).

   The class contains definitions for several enumerated types, 
   corresponding to possible entries for columns in a metadata record.
   Within the MySQL database the values are kept as strings.  The
   enumerated types are provided to insulate clients from these 
   details; enumerations are easier to document and to check for validity.
  */
  class Metadata {
  public:
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


    /* Someday provide option of reading in description of table. In case
       it is used, can check that it matches dbs we're connected to, and
       can check column values.
    */
      
    /// Constructor keeps track of table of interest
    Metadata(const std::string& host="bes3db2.ihep.ac.cn",
             const std::string& table="*",
             const std::string& dbName="calib");

    ~Metadata();

    /** Return serial number for calibration which is best match to
        criteria, using strings for calibType and instrument arguments.
        This method may be useful for development when a particular
        instrument or calibration type is not officially supported.
                 @param ser          serial number of best match 
                                     as integer or zero if no matches
                                     (output)
                 @param calibType    type of data, must match
                 @param timestamp    must be within validity interval; 
                                     closer to center is better
                 @param levelMask    acceptable levels ("production"
                                     better than "dev" better than "test"
                                     better than "superseded")
                 @param instrument   e.g. LAT, EM, CU,...
                 @param flavor       optionally specify non-standard
                                     calibration flavor 
                 @return             status. Should be RETOk.
                                     

       If there are multiple calibrations which are not distinguished
       by the above, pick the one most recently written.
    */
/*maqm    eRet findBest(unsigned int *ser,
                  const std::string& calibType, 
                  const facilities::Timestamp& timestamp,
                  unsigned int levelMask, 
                  const std::string& instrument,
                  const std::string& flavor="VANILLA");
maqm*/
     //maqm modify
     eRet findBest(unsigned int *ser,
		   const std::string& calibType,
		   const std::string& sft_ver,
                   const std::string& cal_ver,
		   const std::string& cal_par,
                   const std::string& rec_alg,
                   const std::string& rec_par,
                   const std::string& machine,
		   const std::string& flavor="VANILLA");
     
    /** Similar to findBest above, but here caller constrains the update_time
        of the calibration by specifying an earliest and (optional) latest
        time for it.  The serial number of the calibration meeting all 
        other conditions, and with the earliest update_time, will be returned.
             @param ser          serial number of best match 
                                 as integer or zero if no matches
                                 (output)
             @param calibType    type of data, must match
             @param update_start  ptr to time; early bound for update_time 
             @param update_end    ptr to late bound for update_time (may be 0)
             @param levelMask    acceptable levels ("production"
                                 better than "dev" better than "test"
                                 better than "superseded")
             @param instrument   e.g. LAT, EM, CU,...
             @param flavor       optionally specify non-standard
                                 calibration flavor 
             @return             status. Should be RETOk.

     */
    eRet findSoonAfter(unsigned int *ser,
                       const std::string& calibType, 
                       const std::string& sft_ver,
		       const std::string& cal_ver,
		       const std::string& cal_par,
                       const std::string& rec_alg,
                       const std::string& rec_par,
                       const std::string& machine,
		       const std::string& flavor="VANILLA");

    //    const std::string* const getCalibTypeStr(eCalibType cType);
    //    const std::string* const getDataFmtStr(eDataFmt fmt);
    //    const std::string* const getInstrumentStr(eInstrument inst);

    /// Get validity interval for a particular calibration
    eRet getInterval(unsigned int serialNo,
                     facilities::Timestamp*& since,
                     facilities::Timestamp*& till);

    // Might also want a "findAll" which would just return a list
    // of serial numbers, and a "getRecord" which would either
    // just return the full row as a string or parse it into 
    // its separate columns

    /** Given a calibration serial number, return information needed for 
        caller to read in the data.  
          @param  serialNo           [input]
          @param  dataFormat
          @param  fmtVersion
          @param  filename
          @return     true if serialNo exists in dbs and "filename" has
                      non-null value; else false.
    */
    eRet getReadInfo(unsigned int serialNo, 
                     int *runFrm,
		     int *runTo,
		     std::string& dataFmt,
         //            std::string& fmtVersion,
                     std::string& dataIdent);

   eRet getReadTOFInfo(unsigned int serialNo, 
                     int *runFrm,
		     int *runTo,
		     std::string& calParVer,
                     DatabaseRecordVector& res,
		     int runNo,
		     std::string& sftver);

  eRet getReadEMCInfo(unsigned int serialNo, 
                     int *runFrm,
		     int *runTo,
		     std::string& calParVer,
                     DatabaseRecordVector& res,
		     int runNo,
		     std::string& sftver);

  eRet getReadDedxInfo(unsigned int serialNo, 
                     int *runFrm,
		     int *runTo,
		     std::string& calParVer,
                     DatabaseRecordVector& res,
		     int runNo,
		     std::string& sftver);
  
  eRet getReadMDCInfo(unsigned int serialNo, 
                     int *runFrm,
		     int *runTo,
		     std::string& calParVer,
                     DatabaseRecordVector& res,
		     int runNo,
		     std::string& sftver);
  
   eRet getReadMUCInfo(unsigned int serialNo,
     		       int *runFrm,
                       int *runTo,
                       std::string& calParVer,
                       DatabaseRecordVector& res,
		       int runNo,
		       std::string& sftver);

   eRet getReadEsTimeInfo(unsigned int serialNo,   
                      int *runFrm,   
                      int *runTo,   
                      std::string& calParVer,   
                      DatabaseRecordVector& res,
                      int runNo,
		      std::string& sftver);   
  
   eRet getReadEstTofInfo(unsigned int serialNo,
                     int *runFrm,
                     int *runTo,
                     std::string& calParVer,
                     DatabaseRecordVector& res,
                     int runNo,
		     std::string& sftver);
   eRet getReadMdcAlignInfo(unsigned int serialNo,
	 int *runFrm,
	 int *runTo,
	 std::string& calParVer,
         DatabaseRecordVector& res,
	 int runNo,
	 std::string& sftver);
   eRet getReadMdcDataConstInfo(unsigned int serialNo,
	 int *runFrm,
	 int *runTo,
	 std::string& calParVer,
         DatabaseRecordVector& res,
	 int runNo,
	 std::string& sftver);
  eRet getReadTofQElecInfo(unsigned int serialNo,
         int *runFrm,
         int *runTo,
         std::string& calParVer,
         DatabaseRecordVector& res,
         int runNo,
         std::string& sftver);
  eRet getReadTofSimInfo(unsigned int serialNo,
         int *runFrm,
         int *runTo,
         std::string& calParVer,
         DatabaseRecordVector& res,
         int runNo,
         std::string& sftver);
   eRet getReadDedxSimInfo(unsigned int serialNo,
         int *runFrm,
         int *runTo,
         std::string& calParVer,
         DatabaseRecordVector& res,
         int runNo,
         std::string& sftver);



    // Insert a new record; return serial number if successful
    int registerCalib(const std::string& inst, 
                      const std::string& flavor,
                      const std::string& calib_type,
                      const std::string& data_ident,
                      const std::string& data_format,
                      unsigned int& runfrm,
		      unsigned int& runto,
		      const std::string& input_desc,
                      const std::string& notes,
                      const std::string& proc_level,
                      const std::string& locale,
                      const std::string& fmt_version = "",
                      const std::string& completion = "OK");
                    

    bool connectRead(eRet& err);


    bool checkValues(const rdbModel::StringVector& cols,
                     const rdbModel::StringVector& vals) const;

    bool checkNulls(const rdbModel::StringVector& cols) const;

    rdbModel::Connection* getReadConnection() {return m_readCxt;}
    
    // Should probably be const
    rdbModel::Rdb* getRdb() {return m_rdb;}
    const std::string& getTable() {return m_table;}

    // Might make these private
    void disconnectRead();
    void disconnectWrite();

  private:

    rdbModel::Connection* m_readCxt;
    rdbModel::Connection* m_writeCxt;

    // these could be static
    rdbModel::Assertion* m_findBest;
    rdbModel::Assertion* m_findSoonest;

    // For each pre-existing row satisfying certain conditions, 
    // set its vend = vstart of new row
    unsigned adjustVend(int newSer);
    

    /*
      Set val to string representation of "next" bit in levelMask
      (one of "PROD", "DEV", "TEST" or "SUPSED"),
      and mask off that bit in levelMask.  
    */
    bool fetchLevel(std::string& val, unsigned int *levelMask);

    //    void initConditions(std::vector<rdbModel::Operator *> conditions);

    //    bool addLevel(std::string& q, unsigned int *levelMask);

    /*  Use rdbModel::MysqlConnection::open( ..  )   to do most of the work */

    //    static bool connect(rdbModel::MysqlConnection* conn,
    static bool connect(rdbModel::Connection* conn,
                        std::string& host, 
                        const std::string& user, 
                        const std::string& pw,  eRet& err,
                        const std::string& dbName);

    bool connectWrite(eRet& err);


    eRet compareSchema(rdbModel::Connection* conn,
                       const std::string& schema);
    // Find 'best' calibration, based on conditions and levelMask; store
    // serial number.
    //maqm fix 
     //eRet Metadata::doSelect(unsigned levelMask,
     eRet doSelect(std::vector<rdbModel::Assertion::Operator *>& 
                            conditions, 
                            rdbModel::StringVector& orderBy,
                            unsigned*& ser);
    
    /// Discover username and store in caller-supplied string
    eRet fetchUser(std::string& user);

    /// Compare schema described in xml file (schema argument) with
    /// actual database.  If schema="", derive file name from 
    /// database name.
    //    eRet compareSchema(const std::string& schema="");

    // Won't need followin enum or bit mask if we eliminate multi-step
    // insert procedure
    /** Keep track of which columns in row have been initialized 
        with bit mask */
    /*
    enum eRow {
      eOpened = 1,
      eValid = 2,
      eInputDesc = 4,
      eComment = 8,
      eFlavor  = 0x10,
      eLocale  = 0x20,
      eCreator = 0x40 };
    */
    /// Constant bit mask indicating all necessary fields have been set
    //    static const unsigned int s_rowReady;

    //    std::string m_row;     // place to keep row as it's being built
    //    unsigned int m_rowStatus;
    std::string  m_host;
    std::string  m_table;
    std::string  m_dbName;
    rdbModel::Manager* m_man;
    rdbModel::Rdb* m_rdb;
    IDatabaseSvc* m_dbsvc;
    bool     m_match; // true if succeeded.  If not attempted, m_rdb is 0
  };
}

#endif
