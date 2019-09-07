


#include "MdcGeomSvc/MdcGeoWire.h"
#include "MdcGeomSvc/MdcGeomSvc.h"


// according to the mathematicial formula:
// Sg = L^2*g*rhol/8*T
// rhol is the linear density 
// rhol is fixed at 9.47E-3 g/m
const double MdcGeoWire::Sag(void) const{

    if(MdcGeomSvc::getSagFlag()){
    // unit of wire length is mm
    const double L = (fForward - fBackward).mag(); 
    // sag unit is mm
    return (L*L*1.184E-6/fTension);
    }
    // do not consider wire sagita
    else return 0.;
}


// calculate the sag at length z
// attention, the z is not the z-coordinate of the point
// but the length from this point to the backward point 
const double MdcGeoWire::Sagz(const double z) const{
  return z*0.; //FIXME
}




