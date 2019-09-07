// $Header: /bes/bes/BossCvs/Calibration/calibUtil/src/test/test_meta.cxx,v 1.1.1.1 2005/10/17 06:12:26 maqm Exp $
/** @file test_meta.cxx
    Sample program to exercise calibration metadata database services
*/

#include <string>
#include <iostream>
#include "calibUtil/Metadata.h"

void gotIt(unsigned int ser, const std::string& dataFmt,
           const std::string& fmtVersion, const std::string& dataIdent);

calibUtil::Metadata::eRet lookup(const std::string& ctype,
                                 const facilities::Timestamp& ts,
                                 unsigned int levelMask,
                                 const std::string& inst);

calibUtil::Metadata::eRet soonest(const std::string& ctype,
                                  const facilities::Timestamp& update_start,
                                  const facilities::Timestamp& update_end,
                                  unsigned int levelMask,
                                  const std::string& inst,
                                  const std::string& flavor
                                  = std::string("VANILLA"));

void compatibility();

int main(int, char**) {
  using calibUtil::Metadata;
  using facilities::Timestamp;

  // just for exercise..
  //  compatibility();

  Timestamp t_ok("2001-11-10 08:00");
  Timestamp t_none("2000-09-08 10:00");
  Timestamp t_late("2010-12-19");

  Metadata::eRet ret = lookup("TKR_DeadChan", t_ok, 
                              Metadata::LEVELDev | Metadata::LEVELTest, 
                              "BTEM");

  ret = soonest("TKR_DeadChan", t_none, t_late, 
                              Metadata::LEVELDev | Metadata::LEVELTest, 
                              "BTEM");

  ret = soonest("TKR_DeadChan", t_none, t_late, 
                              Metadata::LEVELDev | Metadata::LEVELTest, 
                              "BTEM", "invisible");

  ret = lookup("TKR_DeadChan", t_ok, 
               Metadata::LEVELProd | Metadata::LEVELDev, "BTEM");

  ret = lookup("ACD_Eff", t_ok, 
               Metadata::LEVELProd | Metadata::LEVELDev, 
               "BTEM");
  ret = lookup("TKR_HotChan", t_none, Metadata::LEVELDev,
               "BTEM");

  // Try to insert a record
  /*
  ret = meta.openRecord(Metadata::INSTEm, Metadata::CTYPE_TKRHotChan,
                   Metadata::FMTXml, "1.0",
                   "$CALIBUTILROOT/xml/test/testHot-2002-05-02.xml",
                   Metadata::CMPLOk);
  if (ret) {
    std::cerr << "openRecord failed with return value " << (int) ret 
              << std::endl;
    return ret;
  }
  ret = 
    meta.addInputDesc("This is the standard invented hot strips file");
  if (ret) {
    std::cerr << "Bad return from addInputDesc: " << (int) ret << std::endl;
    return ret;
  }
  ret = meta.addNotes("Fake record, added from test_meta");
  if (ret) {
    std::cerr << "Bad return from addNotes: " << (int) ret << std::endl;
    return ret;
  }
  ret = meta.addValidInterval(Timestamp(2000, 8, 2), Timestamp());
  if (ret) {
    std::cerr << "Bad return from addValidInterval: " << (int) ret 
              << std::endl;
    return ret;
  }
  unsigned int newSerial;
  ret = meta.insertRecord(&newSerial);
  if (ret) {
    std::cerr << "Bad return from insertRecord: " << (int) ret << std::endl;
  }
  else {
    std::cout << "Successfully inserted new record, serial number " 
              << newSerial << std::endl;
  }
  */
  return(ret);

}


void gotIt(unsigned int ser, const std::string& dataFmt,
           const std::string& fmtVersion, const std::string& dataIdent) {

  std::cout << "Success reading info for record #" << ser << std::endl;
  
  std::cout << "Data format = " <<  dataFmt << std::endl;
  std::cout << "Format version = " << fmtVersion << std::endl;
  std::cout << "Data ident = " << dataIdent << std::endl;
}

calibUtil::Metadata::eRet lookup(const std::string& ctype,
                                 const facilities::Timestamp& ts,
                                 unsigned int levelMask,
                                 const std::string& inst) {
  using calibUtil::Metadata;
  unsigned int ser;
  
  std::cout << std::endl;
  std::cout << "lookup called with input " << std::endl;
  std::cout << "   calibType = " <<  ctype <<std::endl;
  std::cout << "   timestamp = " << ts.getString() << std::endl;
  std::cout << "   levelMask = " << levelMask << std::endl;
  std::cout << "   instrument = " << inst << std::endl;

  Metadata       meta("*", "*", "calib_test");
  Metadata::eRet ret = meta.findBest(&ser, ctype, ts, levelMask, inst);

  if (ret != Metadata::RETOk) {
    std::cout << "findBest failed with status" << (int) ret << std::endl;
  }
  else if (!ser) {
    std::cout << "Query succeeded; no rows found." << std::endl;
  }
  else {
    std::string  fmtVersion;
    std::string dataFmt;
    std::string dataIdent;

    ret = meta.getReadInfo(ser, dataFmt, fmtVersion, dataIdent);
    
    if (ret == Metadata::RETOk) { 
      gotIt(ser, dataFmt, fmtVersion, dataIdent);
    }

    else {
      std::cout << "Failed reading info for record #" << ser;
      std::cout << " with code " << (int) ret << std::endl;
    }
  }

  return ret;
}

calibUtil::Metadata::eRet soonest(const std::string& ctype,
                                  const facilities::Timestamp& update_start,
                                  const facilities::Timestamp& update_end,
                                  unsigned int levelMask,
                                  const std::string& inst,
                                  const std::string& flavor) {
  using calibUtil::Metadata;
  unsigned int ser;

  
  std::cout << std::endl;
  std::cout << "soonest called with input " << std::endl;
  std::cout << "   calibType = " <<  ctype <<std::endl;
  std::cout << "   update_start = " << update_start.getString() << std::endl;
  std::cout << "   update_end = " << update_end.getString() << std::endl;
  std::cout << "   levelMask = " << levelMask << std::endl;
  std::cout << "   instrument = " << inst << std::endl;
  std::cout << "   flavor     = " << flavor << std::endl;

  Metadata       meta("*", "*", "calib_test");
  Metadata::eRet ret = meta.findSoonAfter(&ser, ctype, &update_start, 
                                          &update_end, levelMask, inst, flavor);

  if (ret != Metadata::RETOk) {
    std::cout << "findSoonAfter failed with status" << (int) ret << std::endl;
  }
  else if (!ser) {
    std::cout << "Query succeeded; no rows found." << std::endl;
  }
  else {
    std::string  fmtVersion;
    std::string dataFmt;
    std::string dataIdent;

    ret = meta.getReadInfo(ser, dataFmt, fmtVersion, dataIdent);
    
    if (ret == Metadata::RETOk) { 
      gotIt(ser, dataFmt, fmtVersion, dataIdent);
    }

    else {
      std::cout << "Failed reading info for record #" << ser;
      std::cout << " with code " << (int) ret << std::endl;
    }
  }

  return ret;
}
