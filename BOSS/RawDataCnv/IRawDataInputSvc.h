//===============================================================
//     RawDataInputSvc.h
//===============================================================
//
// Description: Interface class for RawData Input
//              
//              The concrete class can be provide Raw event from 
//              a file, transient store, or through network. 
//---------------------------------------------------------------
#ifndef RAWDATACNVSVC_RAWDATAINPUTSVC_H
#define RAWDATACNVSVC_RAWDATAINPUTSVC_H

 
#include "GaudiKernel/IInterface.h"
#include <string>
#include <stdint.h>

class RAWEVENT;

class IRawDataInputSvc :  virtual public IInterface
{
  public:

     StatusCode initialize() { return StatusCode::SUCCESS; };
     StatusCode finalize() { return StatusCode::SUCCESS; };

     virtual RAWEVENT * nextEvent() = 0;
     virtual RAWEVENT * currentEvent() = 0;
     virtual bool setCurrentEvent(RAWEVENT*) = 0;

     virtual std::string currentFile() const = 0;

     virtual int runMode() = 0;

     virtual uint32_t runNo() = 0;
  
  protected:
 
};
 
#endif 
