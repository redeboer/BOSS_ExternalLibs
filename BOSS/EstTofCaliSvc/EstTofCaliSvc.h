// $Id: EstTofCaliSvc.h,v 1.7 2015/08/20 01:41:55 sunss Exp $ // -*-c++-*-
// header file for a class called "TofCaliSvc"
#ifndef EsTTOFCALISVC_H
#define EsTTOFCALISVC_H

#include <iostream>
#include <ios>
#include <vector>

#include "EstTofCaliSvc/BTofCal.h"
#include "EstTofCaliSvc/ETofCal.h"
#include "EstTofCaliSvc/EtfCal.h"
#include "EstTofCaliSvc/BTofCommonCal.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Property.h"
#include "EstTofCaliSvc/IEstTofCaliSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"

class EstTofCaliSvc : public Service, virtual public IEstTofCaliSvc,virtual public IIncidentListener
{
public:
  // Constructors and destructors
  EstTofCaliSvc( const std::string& name, ISvcLocator* svcloc );
  ~EstTofCaliSvc();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
  virtual StatusCode initialize ( );
  virtual StatusCode finalize ( );
  void handle(const Incident&);

  // Access by others
  BTofCal*       BTof(unsigned id) const;
  BTofCommonCal* BTofCommon() const;
  ETofCal*       ETof(unsigned id) const;   
  EtfCal*        EtfTof(unsigned int id) const;   

  const double  BTCorr1(double ADC,double zHit,unsigned int id);
  const double  BTCorr2(double ADC,double zHit,unsigned int id);
  //  const double  BTCorrOffset1(double zHit,unsigned int id);
  //  const double  BTCorrOffset2(double zHit,unsigned int id);
  const double  TOffset();
  const double  BTime1(double ADC,double TDC,double zHit,unsigned id);
  const double  BTime2(double ADC,double TDC,double zHit,unsigned id);
  const double  ETime(double ADC,double TDC,double rHit,unsigned int id);
  const double  EtfTime(double ADC1,double ADC2, double TDC1, double TDC2,unsigned int id, unsigned int strip);
  const double  EtfTime1(double ADC,double TDC,double zHit,unsigned int id, unsigned int strip);
  const double  EtfTime2(double ADC,double TDC,double zHit,unsigned int id, unsigned int strip);
  const double  EtfTime(double TDC1, double TDC2,unsigned int id, unsigned int strip);
  const bool ValidInfo(){return m_st;};
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
  std::vector<BTofCommonCal*> fBTofCommonCal;

  bool m_st;
};

#endif /* TOF_REC_GEO_H */
