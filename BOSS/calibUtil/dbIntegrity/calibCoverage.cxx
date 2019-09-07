/**

       @file calibCoverage

   Determine whether, for a given calibration type (or set of calibration
   types), instrument, flavor, proc_level there is precisely one 
   matching calibration in db.  In all cases, supplying * for
   an argument will give you the default value for it

   Call with arguments

       calibtype    Specific type, e.g. "TKR_splits", or class.  Allowable
                    classes are "CAL", "TKR", and "*"   (all). 
       instrument   Defaults to LAT   
       flavor       Defaults to "vanilla"
       level        (i.e., proc_level) defaults to "PROD"
       start        Timestamp for beginning of time period to be covered.
                    Defaults to vstart for first calibration found
       db           Defaults to "calib" (production db)

       For most of the work, see Coverage class
       
*/

#include "calibUtil/Metadata.h"
#include "Coverage.h"
#include <iostream>
#include <cstdio>

#include "string.h"



void printHelp() {
  std::cout << "Invoke as follows: " << std::endl;
  std::cout << "calibCoverage  calibtype instrument flavor level start db" 
            << std::endl;
  std::cout << "All arguments but the first are optional.  Defaults are: " << std::endl;
  std::cout << "instrument = 'LAT'" << std::endl;
  std::cout << "flavor = 'vanilla'" << std::endl;
  std::cout << "level = 'PROD'" << std::endl;
  std::cout << "start = '1970-1-1 00:00'" << std::endl;
  std::cout << "db = 'calib'" << std::endl;
}
  

namespace rdbModel {
  class Rdb;
  class Connection;
}

int main(int argc, char* argv[]) {

  using calibUtil::Metadata;
  using facilities::Timestamp;

  if (argc < 2) {
    printHelp();
    exit(0);
  }


  //  rdbModel::MysqlConnection* conn = new rdbModel::MysqlConnection();
  // First do read connection to db, see if xml schema is compatible with db
  // Then use it to check other arguments
  std::string dbname = "calib";   // the default
  // Gives us whatever is in requirements for host and table
  std::string defValue = "*"; 

  if (argc > 6) {
    if ((argv[6]) != "*") dbname = std::string(argv[6]);
  }
  Metadata*  meta = new Metadata(defValue, defValue, dbname);

  if (!meta) {
    std::cerr << "Unable to construct calibUtil::Metadata object "<< std::endl;
    std::cerr.flush();
    exit(1);
  }
  Metadata::eRet  ret;
  bool ok = meta->connectRead(ret);

  if (!ok) {
    std::cerr << "Connection to metadata dbs failed with return code "
              << ret << std::endl;
    std::cerr.flush();
    exit(1);
  }
  rdbModel::Rdb* rdb = meta->getRdb();

  std::string instr("LAT");
  std::string flavor("vanilla");
  std::string level("PROD");
  
  Timestamp ts;
  // Sort out instr, flavor, level, ts arguments.  Update local
  // variables if values other than defaults supplied
  if (argc > 2) {
    if (!strcmp(argv[2], "*")) instr = std::string(argv[2]);
    if (argc > 3) {
      if (!strcmp(argv[3], "*")) flavor = std::string(argv[3]);
      if (argc > 4) {
        if (!strcmp(argv[4], "*")) level = std::string(argv[4]);
      }
    }
  }
  if (argc > 5) {
    try {
      ts = Timestamp(std::string(argv[5]));
    }
    catch (facilities::BadTimeInput  ex) {
      std::cerr << "Caught facilities::BadTimeInput exception with complaint "
                << ex.complaint << std::endl << "Exiting..." << std::endl;
      std::cerr.flush();
      exit(1);
    }
  }
  else ts = Timestamp(0,0); 

  // Check instrument, level against standard list;
  //  put out warning (but don't exit) if not found
  if (rdb) {
    rdbModel::StringVector cols;
    rdbModel::StringVector vals;
    cols.reserve(2);
    vals.reserve(2);

    cols.push_back(std::string("instrument"));
    vals.push_back(instr);
    cols.push_back(std::string("proc_level"));
    vals.push_back(level);
    if (!(meta->checkValues(cols, vals))) {
      std::cout << "Non-standard value for instrument or level. " << std::endl;
      std::cout << "Supplied values were " << instr <<", " << level 
                << ", respectively." << std::endl;
    }
  }

  Coverage cov(meta, instr, flavor, level, ts);
  // If calibtype arg is a class, generate list

  std::vector<std::string> calibTypes;

  // Following fails only if arg was a calibration class (TKR, CAL or *  
  // meaning "all" and we don't have a schema.
  // ..except for now we haven't implemented classes at all.
  std::string arg1(argv[1]);
  bool expanded = cov.expandTypes(arg1, calibTypes);
  if (!expanded) {
    exit(1);
  }
  // For each calibtype, do the work
  for (unsigned i = 0; i < calibTypes.size(); i++) {
    unsigned ret = cov.checkType(calibTypes[i]);
    if (ret > 0) {
      std::cerr << "Type " << calibTypes[i] << " failed with return code "
                << ret << std::endl;
    }
    else {
      std::cout << "Type " << calibTypes[i] << " ok in metadata database "
                << std::endl;
    }
  }
  return 0;
}
