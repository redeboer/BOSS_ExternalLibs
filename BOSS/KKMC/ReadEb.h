/****************************************
 * Read CMS energy from data base for 
 * psi(3770) production
 *
 * 2012-1-05 created pingrg 
 *
 ***************************************/
#ifndef DTagAlg_ReadBeamInfFromDb_H
#define DTagAlg_ReadBeamInfFromDb_H

#include "CLHEP/Vector/ThreeVector.h"
#include "DatabaseSvc/IDatabaseSvc.h"
#include <iostream>
#include <mysql.h>


class ReadEb {

 public:
  
 ReadEb(int run):
  m_run(-1),
  m_isRunValid(false),
  m_beamE(0),
  m_beta(0.011,0,0),
  m_usecbE(true){
   if ( run != previousRun) {
    previousRun = run;
    ReadDb(run);
   }
 }

  virtual  ~ReadEb() {}
  
  double getEcms(){return m_Ecms;}
  double getXangle(){return m_xangle;}
  inline  CLHEP::Hep3Vector getbeta(){return m_beta;}
  inline bool setcalib(bool calib){m_usecbE=calib;}

 private:
  MYSQL* OpenDb() const;
  void ReadDb(int run);
  void CloseDb(MYSQL* mysql) const;
  
  IDatabaseSvc* m_dbsvc;
  int m_run;
  bool m_isRunValid;
  double m_beamE;
  static double m_Ecms;
  static double m_xangle;
  bool m_usecbE;
  CLHEP::Hep3Vector m_beta;
  static int previousRun;
};


#endif
