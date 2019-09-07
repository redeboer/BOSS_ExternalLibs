// $Header: /bes/bes/BossCvs/Calibration/calibUtil/src/test/test_strips.cxx,v 1.1.1.1 2005/10/17 06:12:26 maqm Exp $
/**  @file test_strips.cxx
   Sample program to exercise low-level calibration strip services. 
   Optional argument is path to file using badStrips.dtd.  If omitted,
     use file $CALIBUTILROOT/xml/test/testHot.xml
*/
#include "calibUtil/StripSrv.h"
#include "facilities/Util.h"

#include <string>
#include <iostream>
#include <fstream>

using calibUtil::StripCol;

class MyObject : public calibUtil::ClientObject {

  virtual calibUtil::eVisitorRet badTower(unsigned int row, unsigned int col, 
                                          int badness);
  virtual calibUtil::eVisitorRet badPlane(unsigned int row, unsigned int col, 
                                          unsigned int tray, bool top,
                                          int badness, bool allBad,
                                          const StripCol& strips);
};

int main(int argc, char* argv[]) {
  using calibUtil::StripSrv;

  std::string name = "$(CALIBUTILROOT)/xml/test/testHot.xml";
  
  if (argc > 1) {
    name = std::string((argv[1]));
  }
  facilities::Util::expandEnvVar(&name);

  StripSrv ssObj(name);

  std::vector<StripSrv::towerRC> trc;
  ssObj.getBadTowers(trc);   
  for (unsigned int i = 0; i < trc.size(); i++ ) {
    std::cout << "Tower id is (" << trc[i].row << ", " << trc[i].col;
    std::cout << ")" << std::endl;

  }
  std::cout << std::endl;

  std::cout<< "Bad type is " << (int) ssObj.getBadType() << std::endl;

  std::cout << "calType name is " << ssObj.getCalType() << std::endl;

  MyObject cli;
  ssObj.traverseInfo(&cli);

  return(0);

} /* end of main */


calibUtil::eVisitorRet MyObject::badTower(unsigned int row, unsigned int col,
                                          int badness) {
  
  std::cout << "MyObject::badTower called back for tower (" << row
           << ", " << col << ") badness = " << badness << std::endl;
  return calibUtil::CONT;
}

calibUtil::eVisitorRet MyObject::badPlane(unsigned int row, unsigned int col, 
                                          unsigned int tray, bool top,
                                          int badness, bool allBad,
                                          const calibUtil::StripCol& strips) {

  std::cout << "MyObject::badPlane called back for tower (" << row
            << ", " << col << ") tray = " << tray << " top = " 
            << top <<std::endl;
  std::cout << "badness = " << badness << " allBad = " << allBad << std::endl;
  std::cout << "#strips in strip collection = " << strips.size() << std::endl;

  return calibUtil::CONT;
}
