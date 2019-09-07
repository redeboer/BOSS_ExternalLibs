
//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkSimpTraj.cxx,v 1.2 2005/07/18 02:56:08 zhangy Exp $
//
// Description:
//     
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------

//#include "BaBar/BaBar.h"
#include "MdcGeom/Constants.h"
#include "MdcGeom/BesAngle.h"
#include "TrkBase/TrkSimpTraj.h"
#include "TrkBase/TrkParams.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "TrkBase/TrkPocaXY.h"
//#include "ErrLogger/ErrLog.h"
using std::ostream;

// statics
HepPoint3D TrkSimpTraj::_theOrigin(0.0,0.0,0.0);
//Constructors
//----------------------------------------------------------------------------
TrkSimpTraj::TrkSimpTraj(const HepVector& params, const HepSymMatrix& cov, 
                         const double lowlim,const double hilim,
			 const HepPoint3D& refpoint) :
//----------------------------------------------------------------------------
  TrkDifTraj(lowlim, hilim),_dtparams(params, cov),_refpoint(refpoint)
{;}

TrkSimpTraj::TrkSimpTraj(const TrkParams& params,
                         const double lowlim,const double hilim,
			 const HepPoint3D& refpoint) :
//----------------------------------------------------------------------------
  TrkDifTraj(lowlim, hilim),_dtparams(params),_refpoint(refpoint)
{;}

//----------------------------------------------------------------------------
TrkSimpTraj::TrkSimpTraj(const TrkSimpTraj& other) :
//----------------------------------------------------------------------------
  TrkDifTraj(other.lowRange(),other.hiRange()),
  _dtparams(other._dtparams),
  _refpoint(other._refpoint)
{}

//----------------------------------------------------------------------------
TrkSimpTraj::~TrkSimpTraj()
//----------------------------------------------------------------------------
{ }

//----------------------------------------------------------------------------
const TrkSimpTraj*
TrkSimpTraj::localTrajectory(double fltLen, double& localFlt) const {
//----------------------------------------------------------------------------
  localFlt = fltLen;
  return this;
}

//
//  The following is the only useful function which can be implemented at this level
//
//----------------------------------------------------------------------------
void
TrkSimpTraj::changePoint(const HepPoint3D& newpoint,double& fltlen) {
//----------------------------------------------------------------------------
  if(newpoint != _refpoint){
//  find POCA to the new point
    TrkPocaXY endpoca(*this,fltlen,newpoint);
    if(endpoca.status().failure()){
      std::cout<<"ErrMsg(error)" << "poca failure changing reference point" << std::endl;
      return;
    } else {
// update flight length
      fltlen = endpoca.flt1();
// make a symmatrix from the covariance: temporary kludge
      int nrow = parameters()->covariance().num_row();
      HepSymMatrix cov(nrow);
      for(int irow=0;irow<nrow;irow++)
	for(int icol=0;icol<=irow;icol++)
	  cov.fast(irow+1,icol+1) = parameters()->covariance().fast(irow+1,icol+1);
//  Get the translation function
      TranslateParams pfunc = paramFunction();
//  Use it on the SimpTraj parameters
      pfunc(_refpoint,newpoint,
	    parameters()->parameter(),cov,
	    _dtparams.parameter(),cov,
	    fltlen);
// put back the covariance
      _dtparams.covariance() = cov;
      _refpoint = newpoint;
// update the flight range to correspond to the same range in space as before
      double newrange[2];
      newrange[0] = lowRange() - fltlen;
      newrange[1] = hiRange() - fltlen;
      setFlightRange(newrange);
    }
  }
  return;
}
//----------------------------------------------------------------------------
void 
TrkSimpTraj::printAll(ostream& os) const {
//----------------------------------------------------------------------------
    os << "Simple ";
    Trajectory::printAll(os);
    os << "SimpTraj parameter vector = "
       << _dtparams.parameter();
    os << "  and covariance matrix =  " 
       << _dtparams.covariance();
  }
//----------------------------------------------------------------------------
void 
TrkSimpTraj::print(ostream& os) const {
//----------------------------------------------------------------------------
  os << "Simple ";
  Trajectory::print(os);
}
// Inversion.  This changes the flightrange from A->B to (-B)->(-A), as well
// adjusting the parameters so that the same arc in space is described (just
// going the opposite direction).  This is now implemented fully generically.
TrkSimpTraj&
TrkSimpTraj::invert()
{
  // Invert parameters
  std::vector<bool> flags(parameters()->nPar(),false);
  invertParams(parameters(), flags);
  // loop over parameters and invert covariance matrix
  for(int iparam=0;iparam<parameters()->nPar();iparam++){
    bool iinvert = flags[iparam];
// do covariance cross-terms too
    for(int jparam=iparam+1;jparam<parameters()->nPar();jparam++){
      bool jinvert = flags[jparam];
      if( (iinvert && !jinvert) || (!iinvert && jinvert) ) {
// cross-terms change sign
        parameters()->covariance()[iparam][jparam] *= -1.0;
      }
    }
  }
// invert the flightlength
  double range[2];
  range[0] = -hiRange();
  range[1] = -lowRange();
  setFlightRange(range);
// done
  return *this;
}


bool 
TrkSimpTraj::operator==(const TrkSimpTraj& x) const
{
     if (lowRange()!=x.lowRange() || hiRange()!=x.hiRange()) return false;
     const HepVector &m=_dtparams.parameter(); 
     unsigned int mp=m.num_row();
     const HepVector &n=x._dtparams.parameter(); 
     unsigned int np=n.num_row();
     if (np!=mp) return false;
     for(unsigned i=0;i<np;++i){
	if(m[i] != n[i]) return false;
     }
     return _refpoint==x._refpoint;
}
