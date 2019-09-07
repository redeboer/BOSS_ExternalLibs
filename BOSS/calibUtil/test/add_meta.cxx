// $Header: /bes/bes/BossCvs/Calibration/calibUtil/src/test/add_meta.cxx,v 1.1.1.1 2005/10/17 06:12:26 maqm Exp $
/** @file test_meta.cxx
    Sample program to add record(s) to the MySQL database. For now user
    only gets to specify
           instrument type
           location of file
           validity start and end times

    Ultimately need something more flexible and easier to use.

*/

#include <string>
#include <iostream>
#include "calibUtil/Metadata.h"

void gotIt(unsigned int ser, const std::string& dataFmt,
           const std::string& fmtVersion, const std::string& dataIdent);

int main(int, char**) {
  using calibUtil::Metadata;
  using facilities::Timestamp;

  Metadata  meta("*", "*", "calib_test");

  int ser = 
    meta.registerCalib("kazoo","mango", "Test_Gen", "notAFile.xml",
                       "XML", facilities::Timestamp("1990-1-5"),
                       facilities::Timestamp("2037-1-1"), "", 
                       "From add_meta test program in calibUtil package",
                       "TEST", "SLAC", "", "OK", "", "");
  if (ser) {
    std::cout << "Successfully added metadata record; returned serial no. is " 
              << ser << std::endl;
  }
  else std::cout << "Failed to register new metadata record" << std::endl;

  ser = 
    meta.registerCalib("kazoo","mango", "Test_Gen", "alsoNotAFile.xml",
                       "XML", facilities::Timestamp("1990-2-5"),
                       facilities::Timestamp("2037-1-1"), "", 
       "From add_meta test program in calibUtil package, overlaps previous",
                       "TEST", "SLAC", "", "OK", "", "");
  if (ser) {
    std::cout << "Successfully added metadata record; returned serial no. is " 
              << ser << std::endl;
  }
  else std::cout << "Failed to register new metadata record" << std::endl;

  /*
  ser = 
    meta.registerCalib("cello","rocky road", "Test_Gen", "notAnotherFile.root",
                       "ROOT", facilities::Timestamp("1993-1-5"),
                       facilities::Timestamp("2035-6-11"), "", 
                       "From add_meta test program in calibUtil package",
                       "TEST", "SLAC", "", "OK", "", "");
  if (ser) {
    std::cout << "Successfully added metadata record; returned serial no. is " 
              << ser << std::endl;
  }
  else std::cout << "Failed to register new metadata record" << std::endl;

  ser = meta.registerCalib("banjo","rocky road", "Test_Gen", 
                           "notAnotherFile.xml",
                           "XML", facilities::Timestamp("1997-1-5"), 
                           facilities::Timestamp("1998-6-11"), "", 
                           "From add_meta test program in calibUtil package",
                           "TEST", "SLAC", "", "ILLEGAL", "", "");

  if (ser) {
    std::cout << "Successfully added metadata record; returned serial no. is " 
              << ser << std::endl;
  }
  else std::cout << "Failed to register new metadata record" << std::endl;
  */

}



