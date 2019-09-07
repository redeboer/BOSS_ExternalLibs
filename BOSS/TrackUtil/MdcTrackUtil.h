#ifndef MDCTRACKUTIL_H
#define MDCTRACKUTIL_H
#include "MdcGeomSvc/MdcGeomSvc.h"
#include "MagneticField/IMagneticFieldSvc.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"

class MdcTrackUtil{

public:
  MdcTrackUtil();
  ~MdcTrackUtil(){};
  static MdcTrackUtil* instance();

  int nLayerTrackPassed(const HepVector helix);
  int nLayerTrackPassed(const double helix[5]);
  HepVector patRecPar2BesPar(const HepVector& helixPar);
  HepSymMatrix patRecErr2BesErr(const HepSymMatrix& err);
private:
  static MdcTrackUtil* _myself;

  MdcGeomSvc* 	         m_mdcGeomSvc;
  IMagneticFieldSvc*     m_pIMF;
  double Bz;

};
#endif
