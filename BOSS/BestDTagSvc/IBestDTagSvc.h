#ifndef IBEST_DTAG_SVC_H
#define IBEST_DTAG_SVC_H

#include "GaudiKernel/IService.h"

/* Decaration of the interface ID */
static const InterfaceID IID_IBestDTagSvc("IBestDTagSvc", 1, 0);

class EvtRecDTag;

class IBestDTagSvc : virtual public IService
{
   public :
      virtual ~IBestDTagSvc() {}

      static const InterfaceID& interfaceID() { return IID_IBestDTagSvc; }

      virtual EvtRecDTag*  getSingleTag(int modeid = -1, int charm = 0) = 0;
      virtual EvtRecDTag** getDoubleTag(int modeid1, int modeid2, int charm = 0) = 0;

      virtual bool isCosmicOrLepton() = 0;
};

#endif
