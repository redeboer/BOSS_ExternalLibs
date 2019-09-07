//Lianjin WU <wulj@ihep.ac.cn> 2015.11.28
#ifndef Utilities_IMeasuredEcmsSvc_H
#define Utilities_IMeasuredEcmsSvc_H
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "EventModel/EventModel.h"
#include "EventModel/EventHeader.h"
#include "DatabaseSvc/IDatabaseSvc.h"

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <mysql.h>
#include <cstdlib>

static const InterfaceID  IID_IMeasuredEcmsSvc("IMeasuredEcmsSvc", 1, 0);
class IMeasuredEcmsSvc : virtual public IService{
  public:
    static const InterfaceID& interfaceID() { return IID_IMeasuredEcmsSvc; }
//    static const std::string& serviceInUse() { return g_serviceInUse;}
    virtual ~IMeasuredEcmsSvc(){}

    virtual void readMeasuredEcms(int runNo)=0;
		virtual void readRunParams(int runNo)=0;
		virtual void readDB(int runNo)=0;
    virtual int getSampleName(int runNo)=0;
    virtual int getRunFrom(int runNo)=0;
    virtual int getRunTo(int runNo)=0;
    virtual double getEcmsErr(int runNo)=0;
    virtual double getAveEcms(int runNo)=0;
    virtual double getAveEcmsErr(int runNo)=0;
		virtual double getPx(int runNo)=0;
    virtual double getPy(int runNo)=0;
    virtual double getPz(int runNo)=0;
    virtual double getEcms(int runNo)=0;
    virtual bool isRunNoValid(int runNo)=0;
    virtual bool isGetEcmsValid(int runNo)=0;

		virtual void readInfo(int runNo, char *tab, char *col, char *min_col, char *max_col)=0;
		virtual double getInfo(int runNo, char *tab, char *col, char *min_col, char *max_col)=0;
		virtual bool isGetInfoValid(int runNo, char *tab, char *col, char *min_col, char *max_col)=0;

		virtual void readInfo(int runNo)=0;
    virtual double getInfo(int runNo)=0;
		virtual bool isGetInfoValid(int runNo)=0;

		virtual void readRecord(int runNo, char *tab, char *col, char *min_col, char *max_col)=0;
    virtual std::string getRecord(int runNo, char *tab, char *col, char *min_col, char *max_col)=0;
    virtual bool isGetRecordValid(int runNo, char *tab, char *col, char *min_col, char *max_col)=0;

    virtual void readRecord(int runNo)=0;
    virtual std::string getRecord(int runNo)=0;
    virtual bool isGetRecordValid(int runNo)=0;

		virtual void readMeasuredEcms2(int runNo)=0;
    virtual void readRunParams2(int runNo)=0;
		virtual int getSampleName()=0;
    virtual int getRunFrom()=0;
    virtual int getRunTo()=0;
    virtual bool isRunNoValid()=0;
    virtual double getEcmsErr()=0;
    virtual double getAveEcms()=0;
    virtual double getAveEcmsErr()=0;
    virtual double getPx()=0;
    virtual double getPy()=0;
    virtual double getPz()=0;
		virtual double getEcms()=0;
		virtual bool isReadDBValid()=0;
		virtual bool isGetEcmsValid()=0;
//  protected:
//    static std::string g_serviceInUse;
};
#endif
