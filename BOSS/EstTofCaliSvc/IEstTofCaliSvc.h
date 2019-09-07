#ifndef IESTTOFCALISVC_H_
#define IESTTOFCALISVC_H_
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "EstTofCaliSvc/BTofCal.h"
#include "EstTofCaliSvc/ETofCal.h"
#include "EstTofCaliSvc/EtfCal.h"
#include "EstTofCaliSvc/BTofCommonCal.h"

// Declaration of the interface ID 
static const InterfaceID  IID_IEstTofCaliSvc("IEstTofCaliSvc",1,0);

class BTofCal;
class ETofCal;

class IEstTofCaliSvc : virtual public IInterface  {  
public: 
   static const InterfaceID& interfaceID() { return IID_IEstTofCaliSvc; }
   virtual BTofCal* BTof(unsigned id) const= 0;
   virtual BTofCommonCal* BTofCommon() const= 0;
   virtual ETofCal* ETof(unsigned id) const= 0;   
   virtual EtfCal* EtfTof(unsigned id) const= 0;   
   virtual const double  BTCorr1(double ADC,double zHit,unsigned int id) = 0;
   virtual const double  BTCorr2(double ADC,double zHit,unsigned int id) = 0;
   virtual const double  TOffset() = 0;
   virtual const double  BTime1(double ADC,double TDC,double zHit,unsigned id) = 0;
   virtual const double  BTime2(double ADC,double TDC,double zHit,unsigned id) = 0;
   virtual const double  ETime(double ADC,double TDC,double rHit,unsigned id) = 0;
   virtual const double  EtfTime(double ADC1,double ADC2, double TDC1, double TDC2,unsigned int id, unsigned int strip) = 0;
   virtual const double  EtfTime1(double ADC,double TDC,double zHit,unsigned id,unsigned strip) = 0;
   virtual const double  EtfTime2(double ADC,double TDC,double zHit,unsigned id,unsigned strip) = 0;
   virtual const double  EtfTime(double TDC1, double TDC2,unsigned int id, unsigned int strip) = 0;
   virtual const bool  ValidInfo()=0;
   virtual void Dump() = 0;
};
 
#endif // ITOFCALISVC_H
