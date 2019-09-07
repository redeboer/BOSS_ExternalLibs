#ifndef IBesGlobalTrigSvc_h
#define IBesGlobalTrigSvc_h 1

#include "GaudiKernel/IInterface.h"
//#include "GaudiKernel/IService.h"
#include <vector>

static const InterfaceID IID_IBesGlobalTrigSvc ("IBesGlobalTrigSvc", 1, 0);

class IBesGlobalTrigSvc : virtual public IInterface
{

public:
 // Interface ID of this class
  static const InterfaceID& interfaceID() { return IID_IBesGlobalTrigSvc; }
public:
  virtual StatusCode GlobalTrig() = 0;

  virtual bool getIfpass() = 0;
  virtual const int getTrigCond(int i) = 0;
  virtual std::vector<int> getMdcStrkId() = 0;
  virtual std::vector<int> getMdcLtrkId() = 0;
  virtual std::vector<int> getBTofHitMap() = 0;
  virtual std::vector<int> getETofHitMap() = 0;
  virtual std::vector<double> getEmcClusE() = 0;
  virtual double getEmcTotE() = 0;
  virtual double getEmcBTotE() = 0;
  virtual double getEmcETotE() = 0;
  virtual double getEmcLTotE() = 0;
  virtual double getEmcRTotE() = 0;
  virtual void setGoodEvent(bool) = 0;
  virtual bool getGoodEvent() = 0;
};
#endif
