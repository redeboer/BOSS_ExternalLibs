/** @class IBesRndmGenSvc
  * @brief manage multiple CLHEP random engines as named streams
  *
  * Based on ATLAS Software 
  */

#ifndef BESKERNEL_IBESRNDMGENSVC_H
# define BESKERNEL_IBESRNDMGENSVC_H

//<<<<<< INCLUDES                                                       >>>>>>
#ifndef GAUDIKERNEL_IINTERFACE_H
 #include "GaudiKernel/IInterface.h"
#endif
#ifndef _CPP_STRING
 #include <string>
#endif

#ifndef HepRandomEngine_h
 #include "CLHEP/Random/RandomEngine.h"
#endif


//<<<<<< CLASS DECLARATIONS                                             >>>>>>
class IBesRndmGenSvc : virtual public IInterface {
public:
  /// Interface to the CLHEP engine
  //@{
  virtual CLHEP::HepRandomEngine* GetEngine(const std::string& StreamName)=0;
  virtual void CreateStream(long seed1,  
			    const std::string& StreamName)=0;
  ///set the seeds for an engine. First param will usually be the event number
  virtual CLHEP::HepRandomEngine* setOnDefinedSeeds(int EventNumber, 
					     const std::string& StreamName)=0;
  //@}
  
  /// Print methods
  //@{
  virtual void print(const std::string& StreamName)=0;
  virtual void print()=0;
  //@}
  //random seed interface caogf
  virtual void setGenseed(long) = 0;
  virtual long getGenseed() = 0;
  /// Gaudi boilerplate
  static const InterfaceID& interfaceID();
  
  virtual ~IBesRndmGenSvc() {}
};
inline
const InterfaceID& 
IBesRndmGenSvc::interfaceID() {
  static const InterfaceID _IID("IBesRndmGenSvc", 1, 0);
  return _IID;
}

#endif // BESKERNEL_IBESRNDMGENSVC_H
