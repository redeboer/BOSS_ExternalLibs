// $Header: /bes/bes/BossCvs/Calibration/calibUtil/src/dbIntegrity/Coverage.cxx,v 1.1.1.1 2005/10/17 06:12:26 maqm Exp $
/**
   @file Coverage.cxx

   Implementation of Coverage class

   Also define and implement tiny data-only helper class
*/
#include <iostream>
#include <cstdio>
#include "Coverage.h"
#include "facilities/Util.h"
#include "calibUtil/Metadata.h"
#include "rdbModel/Management/Manager.h"
#include "rdbModel/Management/XercesBuilder.h"

#include "rdbModel/Db/MysqlConnection.h"
#include "rdbModel/Db/MysqlResults.h"
#include "rdbModel/Rdb.h"
#include "rdbModel/RdbException.h"
#include "rdbModel/Tables/Assertion.h"
#include "rdbModel/Tables/Table.h"
#include "rdbModel/Tables/Column.h"

class SelectInfo {
public:
  SelectInfo(int ser, const facilities::Timestamp& start, 
             const facilities::Timestamp& end) : m_ser(ser)
  {
    using facilities::Timestamp;
    m_vstart = Timestamp(start.getClibTime());
    m_vend = Timestamp(end.getClibTime());
  }
  int m_ser;
  facilities::Timestamp m_vstart;
  facilities::Timestamp m_vend;
};

std::ostream& operator<<(std::ostream& out, const SelectInfo& info) {
  out << "Serial #" << info.m_ser << " validitiy interval [" 
      << info.m_vstart.getString() << ", " << info.m_vend.getString() << "]" 
      << std::endl;
  return out;
}

Coverage::Coverage(calibUtil::Metadata* meta,
                   const std::string& instr, const std::string& flavor, 
                   const std::string& level, const facilities::Timestamp& ts) 
  : m_instr(instr), m_flavor(flavor), m_level(level), m_overlap(300) {
  m_ts = facilities::Timestamp(ts.getClibTime());
  m_selects.reserve(3);
  m_selects.push_back("ser_no");
  m_selects.push_back("vstart");
  m_selects.push_back("vend");

  m_orderBy.reserve(1);
  m_orderBy.push_back("vstart asc");

  m_rdb = meta->getRdb();
  m_conn = meta->getReadConnection();
  m_table = meta->getTable();
}



// Implement wildcards later
bool Coverage::expandTypes(std::string& nickname, 
                           std::vector<std::string>& types) {
  if ((nickname == "CAL") ||
      (nickname == "TKR") ||
      (nickname == "*")  ) {
    if (!m_rdb) {
    std::cerr << "Unable to expand wildcard argument " << nickname 
              << " without schema for metadata dbs " << std::endl;
    }
    else {
      std::cerr << 
        "Wildcard expansion of calib_type argument not yet supported";
      std::cerr << std::endl << "Have a nice day" << std::endl;
    }
    std::cerr.flush();
    exit(1);
  }
  types.push_back(nickname);
  return true;
}

 unsigned Coverage::checkType(std::string calibtype) {
   using namespace rdbModel;
   using facilities::Timestamp;
   // Make a query to find all rows matching criteria, ordered ascending
   // by vstart
   //    match calib_type, flavor, instrument, proc_level
   //    completion = "OK"
   //    vstart >= ts
   // Ask for return of ser_no, vstart, vend

   std::vector<Assertion::Operator *> conditions;
   conditions.reserve(6);

   Assertion::Operator completeOp(OPTYPEequal, "completion", "OK",
                                  FIELDTYPEold, FIELDTYPElit);
   //                                  false, true);
   Assertion::Operator instOp(OPTYPEequal, "instrument", m_instr, 
                              FIELDTYPEold, FIELDTYPElit);
                              // false, true);
   Assertion::Operator calibTypeOp(OPTYPEequal, "calib_type", calibtype,
                                  FIELDTYPEold, FIELDTYPElit);
                                   //                  false, true);
   Assertion::Operator flavorOp(OPTYPEequal, "flavor", m_flavor, 
                                  FIELDTYPEold, FIELDTYPElit);
   //    false, true);
   Assertion::Operator levelOp(OPTYPEequal, "proc_level", m_level, 
                                  FIELDTYPEold, FIELDTYPElit);
   //                        false, true);
   Assertion::Operator vstartOp(OPTYPElessThan, m_ts.getString(), 
                                "vstart", 
                                FIELDTYPElit, FIELDTYPEold);
                                //  true, false);
   conditions.push_back(&calibTypeOp);
   conditions.push_back(&instOp);
   conditions.push_back(&flavorOp);
   conditions.push_back(&levelOp);
   conditions.push_back(&vstartOp);
   conditions.push_back(&completeOp);
   Assertion::Operator* andOp = new Assertion::Operator(OPTYPEand, conditions);
   Assertion* whereClause = new Assertion(andOp);

   ResultHandle* results = 0;

   try {               // make the query
     results = m_conn->select(m_table, m_selects, m_orderBy, whereClause);
   }
   catch (RdbException ex) {
     std::cerr << ex.getMsg() << std::endl;
     std::cerr.flush();            // exit(1);
     return false;
   }

   if (!results) {   // Error. Should have ResultHandle even if 0 rows.
     std::cerr << "Error making query " << std::endl;
     std::cerr.flush();
     exit(1);
   }

   // Save returned values
   std::vector<SelectInfo> info;
   unsigned nFound = results->getNRows();
   
   if (!nFound) {
     std::cout << "No calibrations found for calib_type '" << calibtype
               << "'" << std::endl;
     return true;
   }

   info.reserve(results->getNRows() );
   
   unsigned iRow = 0;
   std::vector<std::string> fields;
   fields.reserve(3);

   bool ok = true;
   unsigned retCode = 0;

   // 1. For each row store columnes; check if vstart <  vend
   std::cout << std::endl 
             << "Checking for valid timestamps, vstart < vend .. " 
             << std::endl;
   for (iRow = 0; iRow < nFound; iRow++) {
     results->getRow(fields, iRow);
     int ser = facilities::Util::stringToInt(fields[0]);
     try {
       Timestamp vstart = Timestamp(fields[1]);
       Timestamp vend = Timestamp(fields[2]);
       info.push_back(SelectInfo(ser, vstart, vend));

       if (vend.getClibTime() < vstart.getClibTime() ) {
         std::cerr << "vend < vstart for " << info.back();
         ok = false;  // or could discard the row and try to continue
         retCode = 1;
       }
     }
     catch (facilities::BadTimeInput    ex) {
       std::cerr << "Bad vstart or vend in row " << ser << std::endl;
       ok = false;
       retCode = 1;
     }
     
   }
   if (!ok) return retCode;

   // 2. Check if vend's also increase monotonically
   std::cout << std::endl << "Checking for vends monotonic.. " << std::endl;
   for (iRow = 0; iRow < nFound - 1; iRow++) {
     if (info[iRow].m_vend.getClibTime() > info[iRow+1].m_vend.getClibTime()) {
       std::cerr << "Validity interval for row with serial #" 
                 << info[iRow+1].m_ser 
                 << " completely contained in interval for row with serial #" 
                 << info[iRow].m_ser << std::endl;
       std::cerr << info[iRow];
       std::cerr << info[iRow+1] << std::endl;
       ok = false;
       retCode = 2;
     }
   }
   if (!ok) return retCode;

   // 3. Look for gaps
   std::cout << std::endl << "Checking for gaps.. " << std::endl;
   for (iRow = 0; iRow < nFound - 1; iRow++) {
     if (info[iRow].m_vend < info[iRow+1].m_vstart) {
       std::cerr << "Validity interval gap between calibrations with serial #" 
                 << info[iRow].m_ser << " and #" << info[iRow+1].m_ser 
                 << std::endl;
       std::cerr << info[iRow];
       std::cerr << info[iRow+1] << std::endl;
       ok = false;
       retCode = 3;
     }
   }
   // 4. Look for overlaps
   std::cout << std::endl << "Checking for overlaps.. " << std::endl;
   for (iRow = 0; iRow < nFound - 1; iRow++) {
     if ((info[iRow].m_vend).getClibTime() > 
         (info[iRow+1].m_vstart).getClibTime() + m_overlap) {
       std::cerr << "Unacceptable overlap between serial #" << info[iRow].m_ser
                 << " and #" << info[iRow+1].m_ser << std::endl;
       std::cerr << info[iRow];
       std::cerr << info[iRow+1] << std::endl;
       ok = false;
       if (!retCode) retCode = 4;
     }
   }
   return retCode;

 }
