// $Header: /bes/bes/BossCvs/Calibration/rdbModel/src/test/test_build.cxx,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $
// Test program for rdbModel primitive buiding blocks

#include <iostream>
#include <string>
#include <cstdlib>
#include "rdbModel/Rdb.h"
#include "rdbModel/RdbException.h"
#include "rdbModel/Management/Manager.h"
#include "rdbModel/Management/XercesBuilder.h"
#include "rdbModel/Db/MysqlConnection.h"
#include "rdbModel/Db/MysqlResults.h"
#include "rdbModel/Tables/Table.h"
#include "rdbModel/Tables/Column.h"
#include "rdbModel/Tables/Datatype.h"
#include "rdbModel/Tables/Assertion.h"
#include "facilities/Util.h"

#define TEST_INSERT

int doInsert(rdbModel::Rdb* con);
int doSmartInsert(rdbModel::Rdb* rdb);
int doUpdate(rdbModel::Rdb*, int serial);
void tryQuick(rdbModel::Table* t, const std::string& colname);
int   doSupersedes(rdbModel::Rdb* rdb, int serial, int* newSerial);
int   doSupersede(rdbModel::Rdb* rdb, rdbModel::Row& row, 
                  int serial, int* newSerial);
int main(int, char**) {
  using rdbModel::FieldVal;

  std::string infile("$(RDBMODELROOT)/xml/calib_test.xml");

  rdbModel::Manager* man = rdbModel::Manager::getManager();

  man->setBuilder(new rdbModel::XercesBuilder);
  man->setInputSource(infile);

  // good errcode is 0
  int errcode = man->build();

  if (errcode) {
    std::cerr << "Build failed with error code " << errcode << std::endl;
    return errcode;
  }
  rdbModel::Rdb* rdb = man->getRdb();

  rdbModel::Table* t = rdb->getTable("metadata_v2r1");

  tryQuick(t, "ser_no");
  tryQuick(t, "calib_type");
  tryQuick(t, "notes");
  tryQuick(t, "prod_start");
  tryQuick(t, "vstart");

  rdbModel::Assertion* a = t->getAssertionByName("maySupersede");
  std::vector<FieldVal> oldFields;
  oldFields.reserve(10);
  oldFields.push_back(FieldVal("proc_level", "PROD", false));
  oldFields.push_back(FieldVal("calib_type", "CAL_Ped", false));
  oldFields.push_back(FieldVal("ser_no", "17", false));
  oldFields.push_back(FieldVal("completion", "OK", false));
  oldFields.push_back(FieldVal("prod_start","", true));

  rdbModel::Row oldRow1(oldFields);
  rdbModel::Row toBeRow;

  bool checkOld = a->verify(oldRow1, toBeRow);
  
  std::cout << "Result of verifying 'maySupersede' against oldRow1: " 
            << checkOld << std::endl << std::endl;

  oldFields.clear();
  oldFields.push_back(FieldVal("proc_level", "DEV", false));
  oldFields.push_back(FieldVal("calib_type", "CAL_Ped", false));
  oldFields.push_back(FieldVal("ser_no", "17", false));
  oldFields.push_back(FieldVal("completion", "OK", false));
  
  rdbModel::Row oldRow2(oldFields);

  checkOld = a->verify(oldRow2, toBeRow);
  
  std::cout << "Result of verifying 'maySupersede' against oldRow2: " 
            << checkOld << std::endl << std::endl;

  std::string colMin;
  std::string colMax;
  
  rdbModel::Column* serCol = rdb->getColumn("metadata_v2r1", "ser_no");
  if (serCol) {
    rdbModel::Datatype* serType = serCol->getDatatype();

    std::cout << "Value of isPrimaryKey() for column ser_no is: " 
              << serCol->isPrimaryKey() << std::endl;

    if (serType->getInterval(colMin, colMax) ) {
      std::cout << "Min and max for ser_no are " << colMin 
                << ", " << colMax << std::endl;
    }  else {
      std::cout << "ser_no has no min, max " << std::endl;
    }
  }
  else std::cout << "no such column as 'ser_no' " << std::endl;

  rdbModel::Column* vstartCol = rdb->getColumn("metadata_v2r1", "vstart");
  if (vstartCol) {
    rdbModel::Datatype* vstartType = vstartCol->getDatatype();

    if (vstartType->getInterval(colMin, colMax) ) {
      std::cout << "Min and max for vstart are " << colMin 
                << ", " << colMax << std::endl;
    }  else {
      std::cout << "vstart has no min, max " << std::endl;
    }
  }
  else std::cout << "no such column as 'vstart' " << std::endl;

  // mostly don't want to run code doing an insert.  For times
  // when we do, must connect as user with INSERT priv.
#ifdef TEST_INSERT
  std::string connectfileT("$(RDBMODELROOT)/xml/connect/mysqlTester.xml");
#else
  std::string connectfileT("$(RDBMODELROOT)/xml/connect/mysqlSlacT.xml");
#endif
  
  // Connect to production database, read only
  rdbModel::MysqlConnection* con = new rdbModel::MysqlConnection();

  std::string connectfile("$(RDBMODELROOT)/xml/connect/mysqlSlac.xml");

  if (!(con->open(connectfile)) ) {
    std::cerr << "Unable to connect to MySQL database" << std::endl;
    return -1;
  }


  rdbModel::MATCH match = con->matchSchema(rdb, false);

  switch (match) {
  case rdbModel::MATCHequivalent:
    std::cout << "XML schema and MySQL database are equivalent!" << std::endl;
    break;
  case rdbModel::MATCHcompatible:
    std::cout << "XML schema and MySQL database are compatible" << std::endl;
    break;
  case rdbModel::MATCHfail:
    std::cout << "XML schema and MySQL database are NOT compatible" 
              << std::endl;
    return -2;
  case rdbModel::MATCHnoConnection:
    std::cout << "Connection failed while attempting match" << std::endl;
    return -1;
  }


  // Make a query
  std::string rq[3];
  rq[0] ="select * from metadata_v2r1";
  rq[1] ="select calib_type from metadata_v2r1";
  rq[2] ="select garbage from metadata_v2r1";
  for (int i = 0; i < 3; i++) {
    try {
      rdbModel::ResultHandle* res = 
        con->dbRequest(rq[i]);
      if (res) {
        std::cout << "dbRequest '" << rq[i] << "'" << std::endl; 
        std::cout << "succeeded, returned " << res->getNRows() 
                  << " rows" << std::endl;
      }
      else {
        std::cout << "dbRequest '" << rq[i] << "'" << std::endl; 
        std::cout << "succeeded, no returned data expected" << std::endl;
      }
    }
    catch (rdbModel::RdbException ex) {
      std::cerr << "dbRequest '" <<  rq[i] << "'" << std::endl; 
      std::cerr <<" failed with error: " << ex.getMsg() << std::endl;
      std::cerr << "Code " << ex.getCode() << std::endl;
    }
  }


  // Now try to close connection, then reopen.
  con->close();

  if (!(con->open(connectfile)) ) {
    std::cerr << "Unable to connect to MySQL database" << std::endl;
    return -1;
  }
  con->close();

  // Now open with alternate connection file
  if (!(con->open(connectfileT)) ) {
    std::cerr << "Unable to connect to MySQL database" << std::endl;
    return -1;
  }
  match = con->matchSchema(rdb);

  // Following will do an insert.  To keep from cluttering up the
  // database, mostly don't execute
  //  
#ifdef TEST_INSERT
  bool disable = true;
  con->disableModify(disable);     // so don't really change db
  int serial = doInsert(rdb);
  if (serial) {
    std::cout << "Hallelujah!  Inserted new row, serial# " 
              << serial  << std::endl;
    // Try to supersede.  Should fail since flavor != "vanilla"
    /*
    int newSerial;
    bool didSup = doSupersedes(rdb, serial, &newSerial);
    if (didSup) {
      std::cout << "Supersede of " << serial << " successful." << std::endl;
      std::cout << "New row is " << newSerial << std::endl;
    }
    else {
      std::cout << "Supersede of " << serial << " failed" << std::endl;
    }
    */
  } else if (disable) {  // pick random serial# to check out disabled update
    serial = 17;
  }
  if (serial) {
    // Now try update
    /*    int nUpdates = doUpdate(con, serial);  */
    int nUpdates = doUpdate(rdb, serial);

    if (nUpdates) {
      std::cout << "Did " << nUpdates << " on row " << serial
                << std::endl;
    }
    else std::cout << "Failed to update row " << serial << std::endl;
  }
  else {
    std::cout << "Bah, humbug.  Insert failed. " << std::endl;
  }

  serial = doSmartInsert(rdb);
  if (serial) {
    std::cout << "Did insertLatest, inserted new row with ser_no = " 
              << serial << std::endl;
    // Try to supersede.  Should fail since flavor != "vanilla"
    int newSerial;
    bool didSup = doSupersedes(rdb, serial, &newSerial);
    if (didSup) {
      std::cout << "Supersede of " << serial << " successful." << std::endl;
      std::cout << "New row is " << newSerial << std::endl;
    }
    else {
      std::cout << "Supersede of " << serial << " failed" << std::endl;
    }

  }
  else if (!disable) {
    std::cout << "Bah, humbug.  insertLatest failed. " << std::endl;
  }
#else


  // Check that we can really do something with this connection

  switch (match) {
  case rdbModel::MATCHequivalent:
    std::cout << "XML schema and MySQL database are equivalent!" << std::endl;
    break;
  case rdbModel::MATCHcompatible:
    std::cout << "XML schema and MySQL database are compatible" << std::endl;
    break;
  case rdbModel::MATCHfail:
    std::cout << "XML schema and MySQL database are NOT compatible" 
              << std::endl;
    break;
    //    return -2;
  case rdbModel::MATCHnoConnection:
    std::cout << "Connection failed while attempting match" << std::endl;
    return -1;
  }

  if (match == rdbModel::MATCHfail) { // try again without dbname match
    match = con->matchSchema(rdb, false);

    switch (match) {
    case rdbModel::MATCHequivalent:
      std::cout << "XML schema and MySQL database are equivalent!" 
                << std::endl;
      break;
    case rdbModel::MATCHcompatible:
      std::cout << "XML schema and MySQL database are compatible" << std::endl;
      break;
    case rdbModel::MATCHfail:
      std::cout << "XML schema and MySQL database are NOT compatible" 
                << std::endl;
      //    return -2;
    case rdbModel::MATCHnoConnection:
      std::cout << "Connection failed while attempting match" << std::endl;
      return -1;
    }

  }
#endif

  return 0;
}

// int doInsert(rdbModel::Connection* con) {
int doInsert(rdbModel::Rdb* rdb) {
  
  using rdbModel::FieldVal;
  using rdbModel::Row;

  std::vector<FieldVal> fields;
  fields.reserve(15);

  fields.push_back(FieldVal("instrument", "LAT"));
  fields.push_back(FieldVal("calib_type","Test_Gen"));
  fields.push_back(FieldVal("flavor","berry"));
  fields.push_back(FieldVal("data_fmt","nonsense"));
  fields.push_back(FieldVal("vstart","2003-02-01"));
  fields.push_back(FieldVal("data_size","0"));
  fields.push_back(FieldVal("locale","phobos"));
  fields.push_back(FieldVal("completion","ABORT"));
  fields.push_back(FieldVal("data_ident","$(mycalibs)/test/moreJunk.xml"));
  fields.push_back(FieldVal("notes", 
                            "Absurd test item, setting input_desc to NULL"));
  fields.push_back(FieldVal("input_desc","", true));

  int  serial = 0;
  
  Row row(fields);

  rdb->insertRow("metadata_v2r1", row, &serial);
    
  return serial;
}
/*            start here */
int doSmartInsert(rdbModel::Rdb* rdb) {
  using rdbModel::FieldVal;

  std::vector<FieldVal> fields;
  fields.reserve(15);

  fields.push_back(FieldVal("instrument", "LAT"));
  fields.push_back(FieldVal("calib_type", "CAL_Ped"));
  fields.push_back(FieldVal("flavor", "vanilla"));
  fields.push_back(FieldVal("proc_level", "PROD"));
  fields.push_back(FieldVal("completion", "OK"));
  fields.push_back(FieldVal("data_fmt", "XML"));
  fields.push_back(FieldVal("fmt_version", "1.1"));
  fields.push_back(FieldVal("data_ident", "nofile.xml"));
  fields.push_back(FieldVal("vstart", "2004-01-04"));
  fields.push_back(FieldVal("vend", "2030-01-01"));
  fields.push_back(FieldVal("locale", "Oz"));
  fields.push_back(FieldVal("input_desc", "none"));
  fields.push_back(FieldVal("notes", "trying out insertLatest"));
  fields.push_back(FieldVal("prod_end","",true));
  fields.push_back(FieldVal("input_start","",true));
  fields.push_back(FieldVal("input_end","",true));

  rdbModel::Row row(fields);

  int  serial = 0;
  try {
    rdb->insertLatest("metadata_v2r1", row, &serial);
  }
  catch (rdbModel::RdbException ex) {
    std::cerr << "insertLatest failed with message" << ex.getMsg();
  }
  return serial;
}

// int doUpdate(rdbModel::Connection* con, int serial) {
int doUpdate(rdbModel::Rdb* rdb, int serial) {
  using rdbModel::Assertion;
  using rdbModel::Column;
  using facilities::Util;
  using rdbModel::FieldVal;
  using rdbModel::Row;

  // Set up WHERE clause, always the same
  std::string serialStr;
  Util::itoa(serial, serialStr);
  Assertion::Operator* serEquals = 
    new Assertion::Operator(rdbModel::OPTYPEequal, "ser_no",
                            serialStr, rdbModel::FIELDTYPEold, 
                            rdbModel::FIELDTYPElit);

  Assertion* whereSer = new Assertion(serEquals);
  //  Assertion* whereSer = new Assertion(Assertion::WHENwhere, serEquals);

  // First call an update without any null columns; change notes field
  // and set data_size to something.
  /*  std::vector<std::string> colNames, values, nullCols; */
  std::vector<FieldVal> fields;
  
  fields.push_back(FieldVal("notes", "1st update: set data_size to non-zero value"));
  fields.push_back(FieldVal("data_size", "883"));
  
  Row row(fields);

  std::string table("metadata_v2r1");

  unsigned nChange = rdb->updateRows(table, row, whereSer);

  // Now null out data_size
  fields.clear();
  fields.push_back(FieldVal("data_size", "", true));
  fields.push_back(FieldVal("notes", "2nd update: data_size set to NULL"));
  Row row2(fields);

  nChange += rdb->updateRows(table, row2, whereSer);
  return nChange;
}

void tryQuick(rdbModel::Table* t, const std::string& colname) {
  //  rdbModel::Column* cQuick = t->getQuick(colname);
  rdbModel::Column* col = t->getColumnByName(colname);

  if (!col) {
    std::cerr << colname << " not found by getQuick" << std::endl;
    return;
  }

  std::string name = col->getName();
  if (colname.compare(name) != 0) {
    std::cerr << "Instead of " << colname << ", getColumnByName found " 
    << name << std::endl;
  }

  else {
    std::cout << "getColumnByName found correct column with name " << colname 
              << std::endl;
  }
}

int  doSupersedes(rdbModel::Rdb* rdb, int serial, int* newSerial) {
  using rdbModel::Row;
  using rdbModel::FieldVal;

  int retVal;
  int nSuccess = 0;
  Row row;

  // First put something in row that doesn't belong there
  row.addField(FieldVal("data_ident","supFile.xml"));
  row.addField(FieldVal("notes", "this supersede is not supposed to work"));
  row.addField(FieldVal("instrument", "cello"));

  retVal = doSupersede(rdb, row, serial, newSerial);
  if (!retVal) nSuccess++;


  // Now leave out something necessary
  row.clear();
  row.addField(FieldVal("notes", "oops!  left out data_ident"));

  retVal = doSupersede(rdb, row, serial, newSerial);
  if (!retVal) nSuccess++;

  // Now try to do it right!
  row.clear();
  row.addField(FieldVal("data_ident", "supFile.xml"));
  row.addField(FieldVal("notes", "Try supersede with good row input"));
  retVal = doSupersede(rdb, row, serial, newSerial);
  if (!retVal) nSuccess++;

  std::cout << "Attempted 3 supersedes; # success = " << nSuccess
            << std::endl;
  return nSuccess;
}

int  doSupersede(rdbModel::Rdb* rdb, rdbModel::Row& row, int serial, 
                  int* newSerial) {

  std::string table("metadata_v2r1");
  int retVal = -1;

  try {
    retVal = rdb->supersedeRow(table, row, serial, newSerial);
    if (retVal) {
      std::cout << "supersede of row " << serial << " failed with code " 
                << retVal << std::endl;
    }
    else {
      std::cout << "supsersede of row " << serial << " succeeded" << std::endl;
    }
  }
  catch (rdbModel::RdbException ex) {
    std::cout << "supersede of row " << serial << " failed with exception " 
              << std::endl;
    std::cout << ex.getMsg() << std::endl;
  }
  return retVal;
}
