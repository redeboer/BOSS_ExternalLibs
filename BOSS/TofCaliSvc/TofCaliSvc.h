// $Id: TofCaliSvc.h,v 1.34 2016/05/27 07:56:19 sunss Exp $ // -*-c++-*-
// header file for a class called "TofCaliSvc"
#ifndef TOFCALISVC_H
#define TOFCALISVC_H

#include <iostream>
#include <ios>
#include <vector>
//#include "DBBesGeoBarTOF/DB2BesGeoBarTOF.h"
//#include "DBBesGeoEndTOF/DB2BesGeoEndTOF.h"

#include "TofCaliSvc/BTofCal.h"
#include "TofCaliSvc/BTofCommonCal.h"
#include "TofCaliSvc/ETofCal.h"
#include "TofCaliSvc/EtfCal.h"
#include "TofCaliSvc/EtfBunchCal.h"
#include "TofCaliSvc/TofInfoCal.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Property.h"
#include "TofCaliSvc/ITofCaliSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"

class TofCaliSvc : public Service, virtual public ITofCaliSvc,virtual public IIncidentListener
{
public:
  // Constructors and destructors
  TofCaliSvc( const std::string& name, ISvcLocator* svcloc );
  ~TofCaliSvc();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
  virtual StatusCode initialize ( );
  virtual StatusCode finalize ( );
  void handle(const Incident&);

  // Access by others
  BTofCal*       BTof(unsigned int id) const;
  BTofCommonCal* BTofCommon() const;
  ETofCal*       ETof(unsigned int id) const;   
  EtfCal*        EtfTof(unsigned int id) const;   
  TofInfoCal*    TofInfo() const;

  const double  BTDelay1(unsigned int id);
  const double  BTDelay2(unsigned int id);
  const double  ETDelay(unsigned int id);
  const double  BTCorr1(double ADC,double zHit,unsigned int id);
  const double  BTCorr2(double ADC,double zHit,unsigned int id);
  const double  BTCorrOffset1(double zHit,unsigned int id,double t0);
  const double  BTCorrOffset2(double zHit,unsigned int id,double t0);
  const double  TOffset();
  const double  BTime1(double ADC,double TDC,double zHit,unsigned int id,double t0);
  const double  BTime2(double ADC,double TDC,double zHit,unsigned int id,double t0);
  const double  ETime(double ADC,double TDC,double rHit,unsigned int id);
  const double  EtfTime(double ADC1,double ADC2, double TDC1, double TDC2,unsigned int id, unsigned int strip, double t0);
  const double  EtfTime1(double ADC,double TDC,double zHit,unsigned int id, unsigned int strip, double t0);
  const double  EtfTime2(double ADC,double TDC,double zHit,unsigned int id, unsigned int strip, double t0);
  const double  EtfTimeMC(double ADC1,double ADC2, double TDC1, double TDC2,unsigned int id, unsigned int strip, double t0);
  const double  EtfTimeMC1(double ADC,double TDC,double zHit,unsigned int id, unsigned int strip, double t0);
  const double  EtfTimeMC2(double ADC,double TDC,double zHit,unsigned int id, unsigned int strip, double t0);
  const double  EtfTime(double tleft,double tright);
  const double  EtfBunchP(int index);
  const double  BTimeCounter(double tleft,double tright,double z,unsigned int id);
  const double  BTimeCluster(double tlayer1,double tlayer2,double z1, double z2,unsigned int id1, unsigned int id2);
  const double  BSigma1(double zHit, unsigned int id);
  const double  BSigma2(double zHit, unsigned int id);
  const double  BSigmaCounter(double zHit, unsigned int id);
  const double  BSigmaCluster(double zHit1, double zHit2, unsigned int id1, unsigned int id2);
  const double  ESigma(double rHit, unsigned int id);
  const double  BPulseHeight1(double ADC, double zHit, double sint, unsigned int id);
  const double  BPulseHeight2(double ADC, double zHit, double sint, unsigned int id);
  const double  BPulseHeight(double ADC1, double ADC2, double zHit, double sint, unsigned int id);
  const double  BPh(double ADC1, double ADC2, double zHit, unsigned int id);
  const double  EPulseHeight(double ADC, double rHit, double cost, unsigned int id);
  const double  EPh(double ADC, double rHit, unsigned int id);
  const double  BVeff(unsigned int id);
  const double  BAtten(unsigned int id);
  const double  BGainForward(unsigned int id);
  const double  BGainBackward(unsigned int id);
  const double  getQ0(unsigned int id);
  const double  EVeff(unsigned int id);
  const double  EAtten(unsigned int id);   
  const double  EGain(unsigned int id);   
  const double  EtfVeff(unsigned int id, unsigned int strip);
  const double  ZTDC(double tleft, double tright, unsigned int id);
  const double  ZTDC1(double tright, unsigned int id, double z);
  const double  ZTDC2(double  tleft, unsigned int id, double z);
  const double  ZADC(double qleft, double qright, unsigned int id);
  const double  ZADC1(double qright, unsigned int id, double z);
  const double  ZADC2(double  qleft, unsigned int id, double z);
  const double  EtfZTDC(double tleft, double tright, unsigned int id, unsigned int strip);
  const int     RunBegin();
  const int     RunEnd();
  const int     Version();
  const int     QCorr();
  const int     QElec();
  const int     MisLable();
  const int     BrEast(unsigned int No);
  const int     BrWest(unsigned int No);
  const int     Endcap(unsigned int No);
  const int     Etf(unsigned int No);
  void  Dump();

private:
  StatusCode FillfromDatabase();
  bool m_hasbeeninitialized;

private:
  IDataProviderSvc* m_pCalibDataSvc;
  IntegerProperty  m_run;
  std::vector<BTofCal*>       fBTofCal;
  std::vector<ETofCal*>       fETofCal;
  std::vector<EtfCal*>        fEtfCal;
  std::vector<EtfBunchCal*>   fEtfBunchCal;
  std::vector<BTofCommonCal*> fBTofCommonCal;
  std::vector<TofInfoCal*>    fTofInfoCal;

};
#endif /* TOF_REC_GEO_H */
