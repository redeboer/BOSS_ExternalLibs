#ifndef RAWDATAOUTPUTSVC_H
#define RAWDATAOUTPUTSVC_H
 
#include "GaudiKernel/ISvcLocator.h"

#include "RawFile/RawFileWriter.h" 
#include "RawDataCnv/IRawDataOutputSvc.h" 
#include "RawDataCnv/EventManagement/RawEvent.h" 

 
//class RawDataOutputSvc :  public Service, virtual public IRawDataOutputSvc 
class RawDataOutputSvc :  virtual public IRawDataOutputSvc 
{
 public:

    RawDataOutputSvc(const std::string& name, ISvcLocator* svcloc);
    ~RawDataOutputSvc(void);
    
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual bool putEvent(WriteRawEvent* re); 

 private:

    uint32_t*       m_buffer;
    RawFileWriter*  m_fw;
    std::string     m_outputFile;
};
 
#endif 
