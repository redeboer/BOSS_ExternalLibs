#ifndef MDCPRINTSVC_H
#define MDCPRINTSVC_H

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataSvc.h"
#include "RawDataProviderSvc/RawDataProviderSvc.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcPrintSvc/IMdcPrintSvc.h"

#include <vector>


class MdcPrintSvc: public Service, virtual public IMdcPrintSvc{
  public:
    MdcPrintSvc( const std::string& name, ISvcLocator* svcloc );
    ~MdcPrintSvc();

    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);

    void printRecMdcTrackCol()const;
    void printRecMdcTrack(const RecMdcTrack* tk) const;
    void printDigi(uint32_t getDigiFlag=0) const;
    void printMdcMcHitCol()const;

  private:
    
    IDataProviderSvc* 		eventSvc;
    RawDataProviderSvc*        	m_rawDataProviderSvc; 
    bool			m_printMcTk;
    bool			m_printHit;
    bool			m_printTrackDetail;
    bool			m_printErrMat;
    bool			m_printTdcAdc;
};
#endif /* MDCPRINTSVC_H*/
