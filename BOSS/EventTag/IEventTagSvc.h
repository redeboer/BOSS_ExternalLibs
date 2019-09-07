#ifndef IEVENTTAGSVC_H
#define IEVENTTAGSVC_H
#ifndef GAUDIKERNEL_IINTERFACE_H
 #include "GaudiKernel/IInterface.h"
#endif

#include <string>

#include "McTruth/McParticle.h"
//#include "McTruth/MdcMcHit.h"
#include "EventModel/Event.h"

//using namespace std;
//<<<<<< CLASS DECLARATIONS                                             >>>>>>
class IEventTagSvc : virtual public IInterface {
public:
  virtual int getDecayCode(Event::McParticle* part)=0;
  virtual unsigned long long int getChainCode(Event::McParticle* part)=0;
  virtual unsigned int getCharmDecayType(Event::McParticle* part)=0; 
  virtual std::string pdg2name(long int pdg)=0;
  virtual long int name2pdg(std::string pname)=0;

  static const InterfaceID& interfaceID();
  virtual ~IEventTagSvc() {}
};
inline const InterfaceID&  IEventTagSvc::interfaceID() {
  static const InterfaceID _IID("IEventtagsvc", 1, 0);
  //  static const InterfaceID _IID(7091, 1, 0);
  return _IID;
}

#endif // BESKERNEL_IEVENTTAGSVC_H
