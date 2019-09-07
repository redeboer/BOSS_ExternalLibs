#ifndef RAWDATACNVSVC_RAWDATAFILEINPUTSVC_H 
#define RAWDATACNVSVC_RAWDATAFILEINPUTSVC_H 
 
//===================================================================
//     RawDataFileInputSvc.h
//===================================================================
//
// Description: This class implements the interface RawDataInputSvc for 
//              event selector to read the events for Files.
//
//-------------------------------------------------------------------
 
 
// Include files.
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ISvcLocator.h"
#include "IRawFile/IRawReader.h"
#include "RawDataCnv/EventManagement/RAWEVENT.h" 
#include "RawDataCnv/IRawDataInputSvc.h"

static const InterfaceID IID_RawDataInputSvc("RawDataInputSvc", 1, 0);

class RawDataInputSvc: public Service,
		       virtual public IRawDataInputSvc
{
  public:

   static const InterfaceID& interfaceID() { return IID_RawDataInputSvc; }

   // Constructors:
   RawDataInputSvc(const std::string& name, ISvcLocator* svcloc);

   // Destructor.
   virtual ~RawDataInputSvc();
  
   // Implementation of the RawDataInputSvc interface methods.
   virtual StatusCode initialize();
   virtual StatusCode finalize();

   virtual RAWEVENT* nextEvent();
   virtual RAWEVENT* currentEvent() ;
   virtual bool setCurrentEvent(RAWEVENT* m_onlinere);//TR 

   std::string currentFile() const { return m_reader->currentFile(); }

   // 0-NormalNetMode, 1-OnlineMode, 2-OfflineMode, 3-DistBossMode
   int runMode() { return m_mode; }

   uint32_t runNo() { return m_reader->runNo(); }

  private:
     
   RAWEVENT*    m_re;
   IRawReader*  m_reader;  // used by OfflineMode and DistBossMode

   // 0-NormalNetMode, 1-OnlineMode, 2-OfflineMode, 3-DistBossMode
   int  m_mode;
   bool m_keepRdm;

   // OfflineMode parameter
   std::vector<std::string>  m_inputFiles;

   // DistBossMode parameter
   std::string  m_evtServer;

   // SubDetectorFragments & ROBs
   const uint32_t** m_sds;
   const uint32_t** m_robs;
};
 
#endif  

