#ifndef TOFCALMANAGER_H
#define TOFCALMANAGER_H
#include "tofcalgsec/TofDataSet.h"
#include "tofcalgsec/TofCalib.h"
#include "tofcalgsec/TofCalibFit.h"

class TofCalibManager{
 protected:
  TofCalibManager();

 public:
  ~TofCalibManager();

  static TofCalibManager* getManager();

  TofDataSet* dataSet() { return m_dataset; }

  void addCalib( TofCalib*    cal_item1, int isbarrel );
  void addCalib( TofCalibFit* cal_item2, bool isbarrel );

  void doCalibration();

  void fillTxt( string& dir );
  void fillRoot( string& dir );

 private:
  static TofCalibManager* m_manager;

  TofDataSet* m_dataset;

  std::vector<TofCalib*> calib_barrel_item;
  std::vector<TofCalib*> calib_endcap_item;
  std::vector<TofCalib*> calib_etf_item;

  std::vector<TofCalibFit*> calibration_barrel_item;
  std::vector<TofCalibFit*> calibration_barrel_item2;
  std::vector<TofCalibFit*> calibration_endcap_item;
  std::vector<TofCalibFit*> calibration_etf_item;

};

#endif
