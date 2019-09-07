#ifndef MDCRAWDATAPROVIDER_H
#define MDCRAWDATAPROVIDER_H

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "RawDataProviderSvc/RawDataProviderBase.h"
#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"
#include "MdcRawEvent/MdcDigi.h"
#include <vector>

typedef std::vector<MdcDigi*> MdcDigiVec;

// RawDataProvider for MDC
class MdcRawDataProvider : public RawDataProviderBase{ 
 public:
  MdcRawDataProvider();
  MdcRawDataProvider( const char* name );
  ~MdcRawDataProvider();

  // initialize service point
  StatusCode initialize( ISvcLocator* svcLoc=0, IMessageSvc* pMsg=0 );

  // Incident handler
  void handle(const Incident&);

  void setSkipLayers(const std::vector<int>& skipLayers){m_skipLayers=skipLayers;}
  void setEffThreshold(float effThreshold){m_effThreshold=effThreshold;}

  MdcDigiVec& getMdcDigiVec(uint32_t control = 0);

  // Mdc Provider's control mode
  enum MdcControlMode{
    i_maxMdcDigi        = 0x1FFF,
    b_keepUnmatch   	= 0x2000,
    b_dropHot	    	= 0x4000,
    b_keepSkipped	= 0x8000,
    b_keepBadTdc    	= 0x10000,
    b_unRedo            = 0x20000,
    b_keepAllChannel    = 0x40000
  };

 private:
  MdcDigiVec digiRef;
  std::vector<int> m_skipLayers;
  float            m_effThreshold;
  IMdcCalibFunSvc* m_mdcCalibFunSvc;

};
#endif
