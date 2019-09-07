#ifndef IMDCPRINTSVC_H
#define IMDCPRINTSVC_H

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "MdcRecEvent/RecMdcTrack.h"
//#include "McTruth/McParticle.h"
//#include "McTruth/MdcMcHit.h"
//#include "EventModel/EventModel.h"

//using namespace Event;

/* Declaration of the interface ID */
static const InterfaceID IID_IMdcPrintSvc("IMdcPrintSvc",1,0);

class IMdcPrintSvc: virtual public IInterface{
  public:
    static const InterfaceID& interfaceID() { return IID_IMdcPrintSvc; }

    virtual void printRecMdcTrack(const RecMdcTrack* tk) const =0;
    virtual void printDigi(uint32_t getDigiFlag=0) const =0;
    virtual void printMdcMcHitCol()const=0;

};

#endif /* IMDCPRINTSVC_H */
