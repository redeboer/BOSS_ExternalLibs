#ifndef MDCUTILITYSVC_H
#define MDCUTILITYSVC_H

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"

#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcGeomSvc/MdcGeomSvc.h"
#include "MagneticField/IMagneticFieldSvc.h"
#include "MdcUtilitySvc/IMdcUtilitySvc.h"

#include <vector>


class MdcUtilitySvc: public Service, virtual public IMdcUtilitySvc{
  public:
    MdcUtilitySvc( const std::string& name, ISvcLocator* svcloc );
    ~MdcUtilitySvc();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);

    int nLayerTrackPassed(const HepVector helix) const;
    int nLayerTrackPassed(const double helix[5]) const;

    HepVector patPar2BesPar(const HepVector& helixPar) const;
    HepSymMatrix patErr2BesErr(const HepSymMatrix& err) const;
    HepVector besPar2PatPar(const HepVector& helixPar) const;
    HepSymMatrix besErr2PatErr(const HepSymMatrix& err) const;

    double doca(int layer, int cell, const HepVector helix, const HepSymMatrix errMat, bool passCellRequired = true, bool doSag = true) const;
    double doca(int layer, int cell, HepPoint3D eastP, HepPoint3D westP, const HepVector helixBes,const HepSymMatrix errMatBes, bool passCellRequired = true, bool doSag = true) const;
    double doca(int layer, int cell, const MdcSWire* sWire, const HepVector helixPat, const HepSymMatrix errMatPat, bool passCellRequired = true) const;
    double docaPatPar(int layer, int cell, const HepVector helixPat, const HepSymMatrix errMatPat, bool passCellRequired = true, bool doSag = true) const;
    double docaPatPar(int layer, int cell, HepPoint3D eastP, HepPoint3D westP, const HepVector helixBes,const HepSymMatrix errMatBes, bool passCellRequired = true, bool doSag = true) const;
    double docaPatPar(int layer, int cell, const MdcSWire* sWire, const HepVector helixPat, const HepSymMatrix errMatPat, bool passCellRequired = true) const;

    HepPoint3D pointOnHelix(const HepVector helixPar, int lay, int innerOrOuter) const;
    HepPoint3D pointOnHelixPatPar(const HepVector helixPat, int lay, int innerOrOuter) const;
    bool cellTrackPassedByPhi(const HepVector helix,int layer, int& cellId_in, int& cellId_out) const;
    bool cellTrackPassedByPhiPatPar(const HepVector helix,int layer, int& cellId_in, int& cellId_out) const;
    bool cellTrackPassed(const HepVector helix,int layer, int& cellId_in, int& cellId_out) const;
    bool cellTrackPassedPatPar(const HepVector helix,int layer,int& cellId_in,int& cellId_out) const;

    HepPoint3D Hel(HepPoint3D piv, double dr,double phi0,double Alpha_L,double kappa,double dz,double dphi,double tanl) const;
    double p_cms(HepVector helix, int runNo, double mass) const;
    Hep3Vector momentum(const RecMdcTrack* trk) const;
    double probab(const int& ndof, const double& chisq) const;

  private:

    double Bz() const { return m_pIMF->getReferField()*1000.; };

    IMdcGeomSvc* 	         m_mdcGeomSvc;
    IMagneticFieldSvc*    	 m_pIMF;
    int                    	 m_debug;
    bool			 m_doSag;

};
#endif /* MDCUTILITYSVC_H*/
