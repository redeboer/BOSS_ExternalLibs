//	thanks chunlei
#ifndef DQADtagAlg_ReadBeamInfFromDb_H
#define DQADtagAlg_ReadBeamInfFromDb_H

#include <iostream>
#include <mysql.h>


class ReadBeamInfFromDb {

 public:
  
  ReadBeamInfFromDb();
  ~ReadBeamInfFromDb() {}
  
  bool isRunValid(int run);
  double getbeamE(int run);
  
 private:
  MYSQL* OpenDb() const;
  void ReadDb(int run);
  void CloseDb(MYSQL* mysql) const;
  
  
  int m_run;
  bool m_isRunValid;
  double m_beamE;
  
};


#endif
