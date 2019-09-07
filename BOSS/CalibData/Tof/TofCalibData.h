// $Header: /bes/bes/BossCvs/Calibration/CalibData/CalibData/Tof/TofCalibData.h,v 1.17 2016/05/20 13:32:27 sunss Exp $
#ifndef CalibData_TofCalibData_h
#define CalibData_TofCalibData_h

#include "GaudiKernel/DataObject.h"
#include "CalibData/CalibModel.h"
#include "CalibData/CalibBase1.h"
#include "CalibData/Tof/bTofCalibBase.h"
#include "CalibData/Tof/bTofCommonCalibBase.h"
#include "CalibData/Tof/eTofCalibBase.h"
#include "CalibData/Tof/etfCalibBase.h"
#include "CalibData/Tof/etfBunchCalibBase.h"
#include "CalibData/Tof/tofCalibInfoBase.h"

namespace CalibData {
  class TofCalibData:  public CalibBase1{
 
     public:
    TofCalibData(){};
    TofCalibData(std::vector<bTofCalibBase>* bTofCal,std::vector<bTofCommonCalibBase>* bTofComCal,std::vector<eTofCalibBase>* eTofCal,std::vector<etfCalibBase>* etfCal,std::vector<etfBunchCalibBase>* etfBunchCal,std::vector<tofCalibInfoBase>* tofInfoCal);
    TofCalibData(std::vector<bTofCalibBase>* bTofCal,std::vector<bTofCommonCalibBase>* bTofComCal,std::vector<eTofCalibBase>* eTofCal,std::vector<etfCalibBase>* etfCal,std::vector<tofCalibInfoBase>* tofInfoCal);
    TofCalibData(std::vector<bTofCalibBase>* bTofCal,std::vector<bTofCommonCalibBase>* bTofComCal,std::vector<eTofCalibBase>* eTofCal,std::vector<tofCalibInfoBase>* tofInfoCal);

    virtual ~TofCalibData() {
      delete m_bTofCalibData;
      delete m_bTofCommonCalibData;
      delete m_eTofCalibData;
      if( m_etfCalibData )      { delete m_etfCalibData;      }
      if( m_etfBunchCalibData ) { delete m_etfBunchCalibData; }
      delete m_tofCalibInfoData;
    }

    // Re-implemented from DataObject
    inline virtual const CLID& clID() const { return classID();         }
    
    inline static  const CLID& classID()    { return CLID_Calib_TofCal; }

    virtual StatusCode   update(CalibBase1& other, MsgStream* log);

    bool hasEtf()                           { return m_hasEtf;          }

    //get the Calibration Data
    bTofCalibBase getBTof(int cnt) const;
    double getBTofAtten(int index,int pardex);
    double getBTofSpeed(int index,int pardex);

    double getBTofPleft(int index,int pardex);
    double getBTofPright(int index,int pardex);

    double getBTofPoffleft_bunch0(int index,int pardex);
    double getBTofPoffright_bunch0(int index,int pardex);
    double getBTofPoffleft_bunch1(int index,int pardex);
    double getBTofPoffright_bunch1(int index,int pardex);
    double getBTofPoffleft_bunch2(int index,int pardex);
    double getBTofPoffright_bunch2(int index,int pardex);
    double getBTofPoffleft_bunch3(int index,int pardex);
    double getBTofPoffright_bunch3(int index,int pardex);

    double getBTofFleft(int index,int pardex);
    double getBTofFright(int index,int pardex);
    double getBTofFcounter(int index,int pardex);

    bTofCommonCalibBase getBTofCom() const;
    double getBTofOffset(int pardex);
    double getBTofSigmaCorr(int pardex);

    eTofCalibBase getETof(int cnt) const;
    double getETofAtten(int index,int pardex);
    double getETofSpeed(int index,int pardex);
    double getETofP(int index,int pardex);
    double getETofFP(int index,int pardex);

    etfCalibBase getEtf(int cnt, int strip) const;
    double getEtfSpeed(int index,int strip,int pardex);
    double getEtfPcombine(int index,int strip,int pardex);
    double getEtfPleft(int index,int strip,int pardex);
    double getEtfPright(int index,int strip,int pardex);

    etfBunchCalibBase getEtfBunch() const;
    double getEtfPBunch(int pardex);

    tofCalibInfoBase getTofInfo() const;
    int getRunBegin();
    int getRunEnd();
    int getVersion();

    int getQCorr();
    int getQElec();
    int getMisLable();

    int getBrEast(int index);
    int getBrWest(int index);
    int getEndcap(int index);
    int getEtf(int index);

  private:
    bool m_hasEtf, m_hasEtfBunch;
    std::vector<bTofCalibBase>*       m_bTofCalibData;
    std::vector<bTofCommonCalibBase>* m_bTofCommonCalibData;
    std::vector<eTofCalibBase>*       m_eTofCalibData;
    std::vector<etfCalibBase>*        m_etfCalibData;
    std::vector<etfBunchCalibBase>*   m_etfBunchCalibData;
    std::vector<tofCalibInfoBase>*    m_tofCalibInfoData;
  };
}


#endif
