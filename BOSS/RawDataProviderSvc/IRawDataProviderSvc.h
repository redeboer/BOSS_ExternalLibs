#ifndef IRAWDATAPROVIDERSVC_H
#define IRAWDATAPROVIDERSVC_H

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ObjectVector.h"
#include "MdcRawEvent/MdcDigi.h"
#include "RawDataProviderSvc/TofData.h"
#include "RawDataProviderSvc/MdcRawDataProvider.h"
#include "RawDataProviderSvc/TofRawDataProvider.h"
#include "RawDataProviderSvc/EmcRawDataProvider.h"

/* Declaration of the interface ID */
static const InterfaceID IID_IRawDataProviderSvc("IRawDataProviderSvc",1,0);

class IRawDataProviderSvc: virtual public IInterface{

public:
  static const InterfaceID& interfaceID() { return IID_IRawDataProviderSvc; };
  
  virtual MdcDigiVec& getMdcDigiVec(uint32_t control = 0) =0;
  virtual MdcRawDataProvider* getMdcProvider()=0;

  virtual TofDataMap& tofDataMapOnlineMode(uint32_t control = 1) =0;
  virtual TofDataVector& tofDataVectorOnlineMode(uint32_t control = 1) =0;
  virtual TofDataMap& tofDataMapEstime() =0;
  virtual TofDataVector& tofDataVectorEstime() =0;
  virtual TofDataMap& tofDataMapTof(double estime=0) =0;
  virtual TofDataVector& tofDataVectorTof(double estime=0) =0;
  virtual TofDataMap& tofDataMapEmc(double estime=0) =0;
  virtual TofDataVector& tofDataVectorEmc(double estime=0) =0;
  virtual TofRawDataProvider* getTofProvider()=0;

  virtual EmcDigiCol& getEmcDigiVec(uint32_t control=0) =0;
  virtual EmcRawDataProvider* getEmcProvider()=0;
  virtual bool       isOnlineMode()=0;
};

#endif 
