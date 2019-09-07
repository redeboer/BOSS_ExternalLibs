//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkDifLineTraj.h,v 1.3 2010/03/25 09:56:53 zhangy Exp $
//
// Description:
//    Trajectory subclass that implements a 3-d line, suitable for use in 
//      a track.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//------------------------------------------------------------------------
#ifndef TRKDIFLINETRAJ_H
#define TRKDIFLINETRAJ_H
#include "TrkBase/TrkSimpTraj.h"
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include "CLHEP/Vector/ThreeVector.h"

class TrkVisitor;
class TrkExchangePar;
class TrkDifLineTraj : public TrkSimpTraj
{
  public:
    //  Define the parameters 
    enum {d0Ind=0, phi0Ind, z0Ind, tanDipInd};
    enum {NLINPRM = 4};

    TrkDifLineTraj(const HepVector&,const HepSymMatrix&, double lowlim=-99999.,
	double hilim=99999., const HepPoint3D& refpoint = _theOrigin);
    TrkDifLineTraj(const TrkExchangePar&, double lowlim=-99999.,
	double hilim=99999., const HepPoint3D& refpoint = _theOrigin);
    TrkDifLineTraj( const TrkDifLineTraj&  );   // copy ctor
    TrkDifLineTraj* clone() const;
    ~TrkDifLineTraj();
    TrkDifLineTraj& operator=(const TrkDifLineTraj&);

    virtual HepPoint3D   position(double fltLen) const;
    virtual Hep3Vector direction(double fltLen) const;
    virtual Hep3Vector delDirect(double) const;
    virtual void       getInfo(double fltLen, HepPoint3D& pos,
	Hep3Vector& dir) const;
    virtual void       getInfo(double fltLen, HepPoint3D&,
	Hep3Vector& dir,
	Hep3Vector& delDir) const;
    virtual void       getDFInfo(double fltLen, DifPoint&,
	DifVector& dir,
	DifVector& delDir) const;
    virtual double     curvature( double fltLen) const;

    double cosDip()   const               {return 1./sqrt(1.+tanDip()*tanDip());}
    double d0()       const        {return parameters()->parameter()[d0Index()];}
    double z0()       const        {return parameters()->parameter()[z0Index()];}
    double tanDip()   const    {return parameters()->parameter()[tanDipIndex()];}
    double phi0()     const;
    //yzhang
    int nPar() const {return NLINPRM;}
    //zhangy
    // How far can you go using given approximation before error > tolerance, 
    //   in direction pathDir?
    virtual double distTo1stError(double flt, double tol, int pathDir) const;
    virtual double distTo2ndError(double flt, double tol, int pathDir) const;

    //  Real versions of the base class derivative functions
    HepMatrix derivDeflect(double fltlen,deflectDirection) const;
    HepMatrix derivDisplace(double fltlen,deflectDirection) const;
    HepMatrix derivPFract(double fltlen) const;
    //  PointTraj functions
    TranslateParams paramFunction() const { return TrkDifLineTraj::paramFunc; }
    // Invert the parameter.  Returns true in flags if the inversion
    //requires a change of sign in the covariance matrix.
    void invertParams(TrkParams* params, std::vector<bool>& flags) const;

    //--------------------------------------------------
    // Visitor access
    //--------------------------------------------------

    virtual void visitAccept(TrkVisitor* vis) const;

  private:

    //  Private functions (data members are part of the base class)
    double x( const double& ) const;
    double y( const double& ) const;
    double z( const double& ) const;
    int d0Index()     const                                       {return d0Ind;}
    int phi0Index()   const                                     {return phi0Ind;}
    int z0Index()     const                                       {return z0Ind;}
    int tanDipIndex() const                                   {return tanDipInd;}
    int nLinPrm() const                                         {return NLINPRM;}

    // the real point translation function
    static void paramFunc(const HepPoint3D& oldpoint,const HepPoint3D& newpoint,
	const HepVector& oldpar,const HepSymMatrix& oldcov,
	HepVector& newpar,HepSymMatrix& newcov,
	double fltlen);
};

// Inline functions:

#endif
