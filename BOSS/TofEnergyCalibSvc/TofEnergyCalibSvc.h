#ifndef TOFENERGYCALIBSVC_H_
#define TOFENERGYCALIBSVC_H_

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "TofEnergyCalibSvc/TofEnergyCalibSvc.h"
#include "TofEnergyCalibSvc/ITofEnergyCalibSvc.h"
#include <mysql.h>
#include <map>
#include <vector>
#include "DatabaseSvc/IDatabaseSvc.h"
#include "GaudiKernel/IService.h"
//#include "rdbModel/Db/Connection.h"
//#include "rdbModel/Tables/Assertion.h"
#include "CLHEP/Matrix/Vector.h"
#include "DatabaseSvc/IDatabaseSvc.h"
using CLHEP::HepVector;
#include <string>
#include "DatabaseSvc/DatabaseSvc.h"

class TofEnergyCalibSvc: public Service, virtual public ITofEnergyCalibSvc, 
		     virtual public IIncidentListener
{
	public:
		TofEnergyCalibSvc( const std::string& name, ISvcLocator* svcloc );
		//TofEnergyCalibSvc();
		~TofEnergyCalibSvc();

		virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
		virtual StatusCode initialize();
		virtual StatusCode finalize();

		// Incident handler
		void handle(const Incident&);



		IDatabaseSvc*  m_dbsvc;

		//static TofEnergyCalibSvc*  m_vdb;
		//static TofEnergyCalibSvc*   instance() {
		//	return (m_vdb) ? m_vdb : (m_vdb = new TofEnergyCalibSvc());
		//}
		//const DatabaseSvc*    GetDatabaseSvc() const          {return dynamic_cast<DatabaseSvc*>(m_dbsvc);}

		void          SetIsData(const std::string& _IsData) {m_IsData = _IsData;}
		std::string   GetIsData() const                      {return m_IsData;}
		void          SetBossRelease(const std::string& _bossrelease) {m_bossRelease = _bossrelease;}
		std::string   GetBossRelease() const                  {return m_bossRelease;}
		double getCalibConst() const { return m_CalibConst;}
		double getPara1() const{ return m_Para1;}
		double getPara2() const{ return m_Para2;}
		double getPara3() const{ return m_Para3;}
		double getPara4() const{ return m_Para4;}
		double getPara5() const{ return m_Para5;}
		void setCalibConst(double _CalibConst) {m_CalibConst=_CalibConst;}
		void setPara1(double _Para1) {m_Para1=_Para1;}
		void setPara2(double _Para2) {m_Para2=_Para2;}
		void setPara3(double _Para3) {m_Para3=_Para3;}
		void setPara4(double _Para4) {m_Para4=_Para4;}
		void setPara5(double _Para5) {m_Para5=_Para5;}


		bool          getTofEnergyCalibSvcInfo();
	private:
		// common variables for BOSS & BEAN
		double m_CalibConst;
		double m_Para1;
		double m_Para2;
		double m_Para3;
		double m_Para4;
		double m_Para5;
		std::string   dbName;
		std::string   m_bossRelease;
		std::string   m_IsData;

		std::string host;
		std::string table;
		std::string userName;
		std::string password;
		unsigned int serialNo;


		IDataProviderSvc* m_eventSvc;

};
#endif /* TOFENERGYCALIBSVC_H_ */
