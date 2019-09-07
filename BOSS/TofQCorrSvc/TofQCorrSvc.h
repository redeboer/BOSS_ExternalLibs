// $Id: TofQCorrSvc.h,v 1.4 2011/02/18 04:20:02 maqm Exp $ // -*-c++-*-
// header file for a class called "TofQCorrSvc"
#ifndef TOFQCORRSVC_H
#define TOFQCORRSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "TofQCorrSvc/ITofQCorrSvc.h"
#include <mysql.h>
#include <vector>
#include "DatabaseSvc/IDatabaseSvc.h"

class TofQCorrSvc : public Service, virtual public ITofQCorrSvc,virtual public IIncidentListener
{
public:
  // Constructors and destructors
  TofQCorrSvc( const std::string& name, ISvcLocator* svcloc );
  ~TofQCorrSvc();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
  virtual StatusCode initialize ( );
  virtual StatusCode finalize ( );

  void handle(const Incident&);
  void getTofQCorrInfo();

  const double  getBarEast(int layer,int no); 
  const double  getBarWest(int layer,int no); 
  const double  getEndcap(int layer,int no); 

  const double BQRaw1(int id, double q);
  const double BQRaw2(int id, double q);
  const double EQRaw(int id, double q);

private:

   double m_Bar_East[176][3];
   double m_Bar_West[176][3];
   double m_Endcap[96][3];

   MYSQL *conn;
   IDataProviderSvc* m_eventSvc;
   int m_runNo;
   IDatabaseSvc* m_dbsvc;
};

#endif
