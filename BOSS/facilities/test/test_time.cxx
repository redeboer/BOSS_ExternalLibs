// $Header: /bes/bes/BossCvs/Calibration/facilities/src/test/test_time.cxx,v 1.1.1.1 2005/10/17 06:11:40 maqm Exp $
/** @file test_meta.cxx
    Sample program to exercise calibration metadata database services
*/

#include <string>
#include <iostream>
#include "facilities/Timestamp.h"


int main(int, char**) {
  using facilities::Timestamp;
  using facilities::BadTimeInput;


  try {
    //    Timestamp t_ok("2001-11-10 08:00");
    //    Timestamp t_none("2000-09-08 10:00");
    long int       zero = 0;
    //    long int       negValue = -2000;
    Timestamp unixCreation(zero);
    //    Timestamp gmttest("1970-01-01 00:00");
    long int        aTime = 4000;

    Timestamp aTimestamp(aTime);
    facilities::Timestamp   cur;

    std::cout << "unix creation time is " << unixCreation.getString()
              << std::endl;


    std::cout << "aTimestamp is " << aTimestamp.getString()
              << std::endl;

    std::cout << "cur time (GMT)  is " << cur.getString()
              << std::endl;

    std::string missionStartString("2001-1-1 00:00");

    Timestamp   missionStart(missionStartString);

    std::cout << "Supplied string: " << missionStartString << std::endl;
    std::cout << "Retrieved: " << missionStart.getString() << std::endl;

    std::string PDTString("2005-4-4 12:25");
    Timestamp   PDTTime(PDTString, 25200);

    std::cout << "Supplied PDT string: " << PDTString << std::endl;
    std::cout << "Retrieved: " << PDTTime.getString() << std::endl;
  }
  catch (const BadTimeInput e) {
    std::cout << "Exception message:  " << e.complaint << std::endl;
  }

  return 0;

}

