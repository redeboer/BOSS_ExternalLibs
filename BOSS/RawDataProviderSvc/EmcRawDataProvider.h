#ifndef EMCRAWDATAPROVIDER_H
#define EMCRAWDATAPROVIDER_H

//#include "GaudiKernel/ISvcLocator.h"
//#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "RawDataProviderSvc/RawDataProviderBase.h"
#include "EmcCalibConstSvc/EmcCalibConstSvc.h"
#include "EmcRawEvent/EmcDigi.h"
#include "Rtypes.h"
#include <vector>

//typedef std::vector<EmcDigi*> EmcDigiVec;

// RawDataProvider for EMC
// @author: C.D.Fu
class EmcRawDataProvider:public RawDataProviderBase{ 
 public:
  EmcRawDataProvider();
  EmcRawDataProvider(const char* name, uint32_t adclow=0,uint32_t tlow=0,uint32_t thigh=60);
  EmcRawDataProvider(const char* name, std::vector<uint32_t>& hot,std::vector<uint32_t>& dead,
		     uint32_t elow=0,uint32_t tlow=0, uint32_t thigh=60);
  virtual ~EmcRawDataProvider(){m_emcDigiVec=EmcDigiCol();};

  void handle(const Incident&);

  // initialize service point
  virtual StatusCode initialize(bool mode=0, ISvcLocator* svcLoc=0, IMessageSvc* pMsg=0);
  // Provider's mode
  enum EmcDataMode{
    DropLow  = BIT(0),
    DropFull = BIT(1),
    DropHot  = BIT(2),
    DropDead = BIT(3),
    CutTime  = BIT(4),
    DoCalib  = BIT(5),
    DoOther  = BIT(6),
    Redo     = BIT(7)
  };

  // get vector of point of EmcDigi after drop or cut or calib, etc.  
  EmcDigiCol& getEmcDigiVec(uint32_t control);
  // decide whether it is a good emc hit
  bool isGoodEmcDigi(uint32_t control, uint32_t id, uint32_t measure, uint32_t adc, uint32_t tdc); 
  bool isGoodEmcDigi(uint32_t control, EmcDigi* emcDigi);
  // calibiration
  void doCalib(Identifier& id, uint32_t& measure, uint32_t& adc);
  void doCalib(EmcDigi* emcDigi);

  // set
  void setELowThreshold(const uint32_t low) { m_adcLowThreshold = low; };
  void setTLowThreshold(const uint32_t low) { m_tLowThreshold = low; };
  void setTHighThreshold(const uint32_t high) { m_tHighThreshold = high; };
  void setHotCrystals(const std::vector<uint32_t>& idVec) { m_hotCrystals = idVec; };
  void addHotCrystal(const uint32_t id);
  void setDeadCrystals(const std::vector<uint32_t>& idVec) { m_deadCrystals = idVec; };
  void addDeadCrystal(const uint32_t id);
  bool removeHotCrystal(const uint32_t id);
  bool removeDeadCrystal(const uint32_t id);

  // get
  uint32_t getAdcLowThreshold() { return m_adcLowThreshold; };
  uint32_t getTdcLowThreshold() { return m_tLowThreshold; };
  uint32_t getTdcHighThreshold() { return m_tHighThreshold; };
  std::vector<uint32_t>& getHotCrystals() { return m_hotCrystals; };
  std::vector<uint32_t>& getDeadCrystals() { return m_deadCrystals; };

 private:
  IEmcCalibConstSvc* m_emcCalibConstSvc;

  uint32_t m_adcLowThreshold;          // adc threshold when measure==0
  uint32_t m_tLowThreshold;            // tdc threshold
  uint32_t m_tHighThreshold;
  std::vector<uint32_t> m_hotCrystals;   // hot channels' id
  std::vector<uint32_t> m_deadCrystals;  // dead channels' id
  
  EmcDigiCol   m_emcDigiVec;  // storage of result
};
#endif
