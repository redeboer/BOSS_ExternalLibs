// $Header: /bes/bes/BossCvs/Calibration/CalibData/CalibData/Dedx/DedxSimData.h,v 1.5 2016/09/14 05:26:18 zhaog Exp $
#ifndef CalibData_DedxSimData_h
#define CalibData_DedxSimData_h

/* @class CalibData_DedxSimData_h

  calibration data class  
  calibration infrastructure

  @author huangb
*/

#include "CalibData/CalibModel.h"
#include "CalibData/CalibBase1.h"
#include "TH1F.h"
#include <vector>
using namespace std;

namespace CalibData {
  class DedxSimData:  public CalibBase1{
 
     public:
    DedxSimData(){
     m_hist=0;
     m_hRange=0;
    };

    virtual ~DedxSimData() {}

    // Re-implemented from DataObject
    inline virtual const CLID& clID() const { return classID(); } 
    
    inline static  const CLID& classID() { return CLID_Dedx_Sim; }

    virtual StatusCode   update(CalibBase1& other, MsgStream* log);

    int getVersion() const { return m_ver; }
    int getRangeNo() const { return m_bgNo; }
    double getRange(int i) const{return *((m_hRange->begin())+i);}
    int getThetaNo() const { return m_thetaNo; }
    int    gethistNo() const{return m_histNo;}
    TH1F  getHist(int i) const{return *((m_hist->begin())+i);}
 
    void setVersion(const int ver) { m_ver = ver; }
    void setHistNo(const int No) {m_histNo=No;}
    void setHist(const vector<TH1F>* hist); 
   // void setHist(TH1F hist){m_hist->push_back(hist);}
    void setRange(const vector<double>* hRange);
    void setRangeNo(const int No) { m_bgNo = No; }
    void setThetaNo(const int No) { m_thetaNo = No; }

  private:
   int m_ver;
   int m_histNo;
   int m_bgNo;
   int m_thetaNo;
   vector<TH1F>* m_hist;
   vector<double>* m_hRange;
  };
}


#endif
