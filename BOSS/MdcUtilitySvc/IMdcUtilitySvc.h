#ifndef IMDCUTILITYSVC_H
#define IMDCUTILITYSVC_H

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcGeom/MdcSWire.h"
#include <vector>

static const InterfaceID IID_IMdcUtilitySvc("IMdcUtilitySvc",1,0);

class IMdcUtilitySvc: virtual public IInterface {
  public:
    static const InterfaceID& interfaceID() { return IID_IMdcUtilitySvc; }

    virtual int nLayerTrackPassed(const HepVector helix) const = 0;
    virtual int nLayerTrackPassed(const double helix[5]) const = 0;

    virtual HepVector patPar2BesPar(const HepVector& helixPar) const = 0;
    virtual HepSymMatrix patErr2BesErr(const HepSymMatrix& err) const = 0;
    virtual HepVector besPar2PatPar(const HepVector& helixPar) const = 0;
    virtual HepSymMatrix besErr2PatErr(const HepSymMatrix& err) const = 0;

    virtual double doca(int layer, int cell, const HepVector helix, const HepSymMatrix errMat, bool passCellRequired = true, bool doSag = true) const = 0;
    virtual double doca(int layer, int cell, HepPoint3D eastP, HepPoint3D westP, const HepVector helixBes,const HepSymMatrix errMatBes, bool passCellRequired = true, bool doSag = true) const = 0;
    virtual double doca(int layer, int cell, const MdcSWire* sWire, const HepVector helixPat, const HepSymMatrix errMatPat, bool passCellRequired = true) const = 0;
    virtual double docaPatPar(int layer, int cell, const HepVector helixPat, const HepSymMatrix errMatPat, bool passCellRequired = true, bool doSag = true) const = 0;
    virtual double docaPatPar(int layer, int cell, HepPoint3D eastP, HepPoint3D westP, const HepVector helixBes,const HepSymMatrix errMatBes, bool passCellRequired = true, bool doSag = true) const = 0;
    virtual double docaPatPar(int layer, int cell, const MdcSWire* sWire, const HepVector helixPat, const HepSymMatrix errMatPat, bool passCellRequired = true) const = 0;

    virtual HepPoint3D pointOnHelix(const HepVector helixPar, int lay, int innerOrOuter) const = 0;
    virtual HepPoint3D pointOnHelixPatPar(const HepVector helixPat, int lay, int innerOrOuter) const = 0;
    virtual bool cellTrackPassedByPhi(const HepVector helix,int layer, int& cellId_in, int& cellId_out) const = 0;
    virtual bool cellTrackPassedByPhiPatPar(const HepVector helix,int layer, int& cellId_in, int& cellId_out) const = 0;
    virtual bool cellTrackPassed(const HepVector helix,int layer,int& cellId_in,int& cellId_out) const = 0;
    virtual bool cellTrackPassedPatPar(const HepVector helix,int layer,int& cellId_in,int& cellId_out) const = 0;

    virtual HepPoint3D Hel(HepPoint3D piv, double dr,double phi0,double Alpha_L,double kappa,double dz,double dphi,double tanl) const = 0;
    virtual double p_cms(HepVector helix, int runNo, double mass) const = 0;
    virtual Hep3Vector momentum(const RecMdcTrack* trk) const = 0;
    virtual double probab(const int& ndof, const double& chisq) const = 0;
};
#endif /* IMDCUTILITYSVC_H*/
