/** 
* @file IRootIoSvc.h
* @brief definition of the interface for IRootIoSvc
*
*/
#ifndef _H_IRootIoSvc
#define _H_IRootIoSvc

// includes
#include "GaudiKernel/IInterface.h"
#include <TChain.h>

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IRootIoSvc("IRootIoSvc", 1 , 0); 

/** 
* \class IRootIoSvc
* \brief The RootIoSvc gaudi service interface
*
* \author Heather Kelly heather@lheapop.gsfc.nasa.gov
* 
*/
class  IRootIoSvc : virtual public IInterface {
public:
    
    virtual int getEvtMax() = 0;
    virtual void setRootEvtMax(unsigned int max) = 0;
    virtual void setRootTimeMax(unsigned int max) = 0;

    virtual int index() = 0;
    virtual bool setIndex(int i) = 0;

    virtual void registerRootTree(TChain *ch) = 0;
    virtual bool setRunEventPair(std::pair<int,int> ids) = 0;
    virtual std::pair<int,int> runEventPair() = 0;

    virtual int getAutoSaveInterval() = 0;

    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IRootIoSvc; }

};

#endif  // _H_IRootIoSvc
