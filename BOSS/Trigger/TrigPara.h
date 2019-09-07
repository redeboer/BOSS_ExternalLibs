#ifndef TrigPara_h
#define TrigPara_h 1
#include "CLHEP/Units/PhysicalConstants.h"

using namespace CLHEP;

namespace TrigConf{

class TrigPara{
public:
  TrigPara() {};
  ~TrigPara() {};
};

  static const double L1TC_GATE = 80*MeV;
  static const double L1TC_THRESH = 80*MeV;
  static const double L1ETOT_L = 200*MeV;
  static const double L1ETOT_M = 700*MeV;
  static const double L1ETOT_BR = 2300*MeV;
  static const double L1ETOT_EC = 2300*MeV;
  static const double L1EBL_BR = 800*MeV;
  static const double L1EBL_EC = 800*MeV;
  static const double L1EDIFF_BR = 600*MeV;
  static const double L1EDIFF_EC = 550*MeV;
  static const double L1BLK_GATE = 1070*MeV;
  static const double L1EBL_Z = 300*MeV;

  static const int TCTHETANO_B = 11;
  static const int TCPHINO_B = 30;
  //for trigger cell theta*phi = 1*32 in endcaps
  static const int TCTHETANO_E = 1;
  static const int TCPHINO_E = 32;
  //for trigger cell theta*phi = 2*16 in endcaps
  //static const int TCTHETANO_E = 2;
  //static const int TCPHINO_E = 16;

  //The number of track in MDC
  static const int N = 4;

}
#endif
