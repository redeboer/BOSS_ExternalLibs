#ifndef TOFRAWDATAPROVIDER_H
#define TOFRAWDATAPROVIDER_H

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "RawDataProviderSvc/RawDataProviderBase.h"
#include "RawDataProviderSvc/TofData.h"


const double timeDiff = 20.0;
const double tClockDiff = 1.0;
const double timeForward  = 120.0;
const double timeBackward = 24.0;
const double tClockForward  = 5.0;
const double tClockBackward = 1.0;

const double timeDiffMRPC = 5.0;
const double totForward   = 0.0;
const double totBackward  = 50.0;

// RawDataProvider for TOF
// author: Sun Shengsen
class TofRawDataProvider : public RawDataProviderBase{ 
 public:
  TofRawDataProvider();
  TofRawDataProvider( const char* name );
  ~TofRawDataProvider();

  // initialize service point
  StatusCode initialize( bool mode= false, ISvcLocator* svcLoc=0, IMessageSvc* pMsg=0 );

  // Incident handler
  void handle(const Incident&); 

  // get multimap or vector of the TofData
  void tofDataMapFull();

  // online mode
  TofDataMap&    tofDataMapOnlineMode(uint32_t control=1);
  TofDataVector& tofDataVectorOnlineMode(uint32_t control=1);

  // event start time
  TofDataMap&    tofDataMapEstime();
  TofDataVector& tofDataVectorEstime();

  // tof reconstruction
  TofDataMap&    tofDataMapTof( double estime = 0.0 );
  TofDataVector& tofDataVectorTof( double estime = 0.0 );

  // emc reconstruction
  TofDataMap&    tofDataMapEmc( double estime = 0.0 );
  TofDataVector& tofDataVectorEmc( double estime = 0.0 );

 private:
  int  m_run,  m_event;
  bool m_hasFilled;
  TofDataMap     m_tofDataMap;  // storage of result
  // OnlineMode
  TofDataMap     m_tofDataMapOnline;
  TofDataVector  m_tofDataVectorOnline;
  // Event Start Time
  TofDataMap     m_tofDataMapEstime;
  TofDataVector  m_tofDataVectorEstime;
  // TOF Reconstruction
  TofDataMap     m_tofDataMapTof;
  TofDataVector  m_tofDataVectorTof;
  // EMC Reconstruction using energy deposit in TOF
  TofDataMap     m_tofDataMapEmc;
  TofDataVector  m_tofDataVectorEmc;

};
#endif
