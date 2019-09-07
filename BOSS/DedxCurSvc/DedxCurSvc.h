// $Id: DedxCurSvc.h,v 1.11 2013/04/18 03:10:52 dengzy Exp $ // -*-c++-*-
// header file for a class called "DedxCurSvc"
#ifndef DEDXCURSVC_H
#define DEDXCURSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "DedxCurSvc/IDedxCurSvc.h"
#include <mysql.h>
#include <vector>
#include "DatabaseSvc/IDatabaseSvc.h"

class DedxCurSvc : public Service, virtual public IDedxCurSvc,virtual public IIncidentListener
{
public:
  // Constructors and destructors
  DedxCurSvc( const std::string& name, ISvcLocator* svcloc );
  ~DedxCurSvc();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
  virtual StatusCode initialize ( );
  virtual StatusCode finalize ( );

  void handle(const Incident&);
  void getDedxCurveInfo();


  const double getCurve(int i){return m_curve[i];}
  const double getSigma(int i){return m_sigma[i];}
  const int getCurveSize(){return m_curve_size;}
  const int getSigmaSize(){return m_sigma_size;}

private:

   double m_curve[50];
   double m_sigma[50];
   int m_curve_size;
   int m_sigma_size;
   std::string m_bossRelease;
   std::string m_calParVer;
   std::string m_sftver;
   std::string m_dbStatus;
   std::string m_type; // "Sim" will reverse the sign of RunNo because we want to use the curve from data 

   MYSQL *conn;
   IDataProviderSvc* m_eventSvc;
   IDatabaseSvc* m_dbsvc;

   int m_runFromMax;
   int m_runToMin;
};

#endif
