//========================================
//AUTHOR: Lianjin WU <wulj@ihep.ac.cn> 2015.11.28
//========================================
#ifndef Utilities_MeasuredEcmsSvc_H
#define Utilities_MeasuredEcmsSvc_H 
#include "MeasuredEcmsSvc/IMeasuredEcmsSvc.h"
class IDatabaseSvc;
template <class TYPE> class CnvFactory;
class MeasuredEcmsSvc:public Service, virtual public IMeasuredEcmsSvc, virtual public IIncidentListener{
  friend class CnvFactory<MeasuredEcmsSvc>;
  public:
    MeasuredEcmsSvc(const std::string& name, ISvcLocator* svcLoc);
    virtual ~MeasuredEcmsSvc();

    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
    virtual StatusCode initialize();
    virtual StatusCode finalize();

		void handle(const Incident&);

    void readMeasuredEcms(int runNo);
		void readRunParams(int runNo);
		void readDB(int runNo);
    int getSampleName(int runNo);
    int getRunFrom(int runNo);
    int getRunTo(int runNo);
    double getEcmsErr(int runNo);
    double getAveEcms(int runNo);
    double getAveEcmsErr(int runNo);
		double getPx(int runNo);
		double getPy(int runNo);
		double getPz(int runNo);
    double getEcms(int runNo);
    bool isRunNoValid(int runNo);
    bool isGetEcmsValid(int runNo);

		void readInfo(int runNo, char *tab, char *col, char *min_col, char *max_col);
		double getInfo(int runNo, char *tab, char *col, char *min_col, char *max_col);
		bool isGetInfoValid(int runNo, char *tab, char *col, char *min_col, char *max_col);

		void readInfo(int runNo);
		double getInfo(int runNo);
		bool isGetInfoValid(int runNo);

		void readRecord(int runNo, char *tab, char *col, char *min_col, char *max_col);
		std::string getRecord(int runNo, char *tab, char *col, char *min_col, char *max_col);
		bool isGetRecordValid(int runNo, char *tab, char *col, char *min_col, char *max_col);

		void readRecord(int runNo);
		std::string getRecord(int runNo);
		bool isGetRecordValid(int runNo);

    void readMeasuredEcms2(int runNo);
		void readRunParams2(int runNo);
		int getSampleName(){return m_sampleName2;}
    int getRunFrom(){return m_runFrom2;}
    int getRunTo(){return m_runTo2;}
    double getEcmsErr(){return m_ecmsErr2;}
    double getAveEcms(){return m_aveEcms2;}
    double getAveEcmsErr(){return m_aveEcmsErr2;}
    double getPx(){return m_px2;}
    double getPy(){return m_py2;}
    double getPz(){return m_pz2;}
		double getEcms(){return m_ecms2;}
		bool isReadDBValid(){return m2_runNoValid;}
		bool isGetEcmsValid(){return m2_runNoValid;}
		bool isRunNoValid(){return m2_runNoValid;}

  private:
		//RunParamsMin and RunParamsMax are used to set range for psi3773 listed in RunParams
		int RunParamsMin, RunParamsMax;
		//MeasuredEcmsMin and MeasuredEcmsMax for the data listed in MeasuredEcms2
		int MeasuredEcmsMin, MeasuredEcmsMax;

		std::string table;
		std::string column;
		std::string up_column;
		std::string down_column;

    IDatabaseSvc* m_dbSvc;
    IDataProviderSvc* m_eventSvc;

		int old_runNo;
		int old_runNo1;
		int old_runNo2;
		int old_runNo3;
		int old_runNo4;

		double m_info1;
		bool m_runNoValid1;

		double m_info2;
		bool m_runNoValid2;

    int m_sampleName;
		int m_runFrom;
		int m_runTo;
		double m_ecmsErr;
		double m_aveEcms;
		double m_aveEcmsErr;
    double m_ecms;
		double m_px, m_py, m_pz;
    bool m_runNoValid;

		int m_sampleName2;
    int m_runFrom2;
    int m_runTo2;
    double m_ecmsErr2;
    double m_aveEcms2;
    double m_aveEcmsErr2;
    double m_ecms2;
    double m_px2, m_py2, m_pz2;
    bool m2_runNoValid;

		std::string m_record1;
		bool m_record_valid1;

		std::string m_record2;
		bool m_record_valid2;
};
#endif
