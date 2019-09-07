#ifndef IMDCTUNNINGSVC_H_
#define IMDCTUNNINGSVC_H_

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include <string>
#include "CLHEP/Matrix/Vector.h"
using CLHEP::HepVector;

/* Declaration of the interface ID */
static const InterfaceID IID_IVertexDbSvc("IVertexDbSvc",1,0);

class IVertexDbSvc: virtual public IInterface{
public:
     static const InterfaceID& interfaceID() { return IID_IVertexDbSvc; }

    virtual  double* PrimaryVertex()=0;
  virtual double* SigmaPrimaryVertex()=0;
  virtual bool isVertexValid() =0;

};

#endif /* IMDCCALIBFUNSVC_H_ */
