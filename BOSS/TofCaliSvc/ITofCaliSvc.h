#ifndef ITOFCALISVC_H_
#define ITOFCALISVC_H_
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "TofCaliSvc/BTofCal.h"
#include "TofCaliSvc/BTofCommonCal.h"
#include "TofCaliSvc/ETofCal.h"
#include "TofCaliSvc/EtfCal.h"
#include "TofCaliSvc/TofInfoCal.h"

// Declaration of the interface ID 
static const InterfaceID  IID_ITofCaliSvc("ITofCaliSvc",1,0);

class BTofCal;
class ETofCal;

class ITofCaliSvc : virtual public IInterface  {  
public: 
   static const InterfaceID& interfaceID() { return IID_ITofCaliSvc; }
   virtual BTofCal* BTof(unsigned id) const= 0;
   virtual BTofCommonCal* BTofCommon() const= 0;
   virtual ETofCal* ETof(unsigned id) const= 0;   
   virtual EtfCal* EtfTof(unsigned id) const= 0;   
   virtual TofInfoCal* TofInfo() const= 0;
   virtual const double  BTDelay1(unsigned id) = 0;
   virtual const double  BTDelay2(unsigned id) = 0;
   virtual const double  ETDelay(unsigned id) = 0;
   virtual const double  BTCorr1(double ADC,double zHit,unsigned int id) = 0;
   virtual const double  BTCorr2(double ADC,double zHit,unsigned int id) = 0;
   virtual const double  BTCorrOffset1(double zHit,unsigned int id,double t0) = 0;
   virtual const double  BTCorrOffset2(double zHit,unsigned int id,double t0) = 0;
   virtual const double  TOffset() = 0;
   virtual const double  BTime1(double ADC,double TDC,double zHit,unsigned id,double t0) = 0;
   virtual const double  BTime2(double ADC,double TDC,double zHit,unsigned id,double t0) = 0;
   //   virtual  const double  BTime(double tleft,double tright,double z,unsigned id)= 0;
   virtual const double  ETime(double ADC,double TDC,double rHit,unsigned id) = 0;
   virtual const double  EtfTime(double ADC1,double ADC2, double TDC1, double TDC2,unsigned int id, unsigned int strip, double t0) = 0;
   virtual const double  EtfTime1(double ADC,double TDC,double zHit,unsigned id,unsigned strip, double t0) = 0;
   virtual const double  EtfTime2(double ADC,double TDC,double zHit,unsigned id,unsigned strip, double t0) = 0;
   virtual const double  EtfTimeMC(double ADC1,double ADC2, double TDC1, double TDC2,unsigned int id, unsigned int strip, double t0) = 0;
   virtual const double  EtfTimeMC1(double ADC,double TDC,double zHit,unsigned id,unsigned strip, double t0) = 0;
   virtual const double  EtfTimeMC2(double ADC,double TDC,double zHit,unsigned id,unsigned strip, double t0) = 0;
   virtual const double  EtfTime(double tleft,double tright) = 0;
   virtual const double  EtfBunchP(int index) = 0;
   virtual const double  BTimeCounter(double tleft,double tright,double z,unsigned id) = 0;
   virtual const double  BTimeCluster(double tlayer1,double tlayer2,double z1,double z2,unsigned id1, unsigned int id2) = 0;
   virtual const double  BSigma1(double zHit, unsigned id) = 0;
   virtual const double  BSigma2(double zHit, unsigned id) = 0;
   virtual const double  BSigmaCounter(double zHit, unsigned id) = 0;
   virtual const double  BSigmaCluster(double zHit1, double zHit2, unsigned id1, unsigned id2) = 0;
   virtual const double  ESigma(double rHit, unsigned id) = 0;
   virtual const double  BPulseHeight1( double ADC, double zHit, double sint, unsigned int id ) = 0;
   virtual const double  BPulseHeight2( double ADC, double zHit, double sint, unsigned int id ) = 0;
   virtual const double  BPulseHeight( double ADC1, double ADC2, double zHit, double sint, unsigned int id ) = 0;
   virtual const double  BPh( double ADC1, double ADC2, double zHit, unsigned int id ) = 0;
   virtual const double  EPulseHeight( double ADC, double rHit, double cost, unsigned int id ) = 0;
   virtual const double  EPh( double ADC, double rHit, unsigned int id ) = 0;
   virtual const double  BVeff(unsigned id) = 0;
   virtual const double  BAtten(unsigned id) = 0;
   virtual const double  BGainForward(unsigned id) = 0;
   virtual const double  BGainBackward(unsigned id) = 0;
   virtual const double  getQ0(unsigned id)=0;
   virtual const double  EVeff(unsigned id) = 0;
   virtual const double  EAtten(unsigned id) = 0;
   virtual const double  EGain(unsigned id) = 0;
   virtual const double  EtfVeff(unsigned int id, unsigned int strip) = 0;
   virtual const double  ZTDC(double tleft, double tright, unsigned id) = 0;
   virtual const double  ZTDC1(double tright, unsigned id, double z) = 0;
   virtual const double  ZTDC2(double  tleft, unsigned id, double z) = 0;
   virtual const double  ZADC(double qleft, double qright, unsigned id) = 0;
   virtual const double  ZADC1(double qright, unsigned id, double z) = 0;
   virtual const double  ZADC2(double  qleft, unsigned id, double z) = 0;
   virtual const double  EtfZTDC(double tleft, double tright, unsigned int id, unsigned int strip) = 0;
   virtual const int  RunBegin() = 0;
   virtual const int  RunEnd() = 0;
   virtual const int  Version() = 0;
   virtual const int  QCorr() = 0;
   virtual const int  QElec() = 0;
   virtual const int  MisLable() = 0;
   virtual const int  BrEast(unsigned int No) = 0;
   virtual const int  BrWest(unsigned int No) = 0;
   virtual const int  Endcap(unsigned int No) = 0;
   virtual const int  Etf(unsigned int No) = 0;
   virtual void Dump() = 0;
};
 
#endif // ITOFCALISVC_H
