#include "include/MdcCosWire.h"
#include "include/fun.h"

#include <math.h>

double MdcCosWire::getPhi(double z) const {
     double lx = m_wpos[0] - m_wpos[3];
     double ly = m_wpos[1] - m_wpos[4];
     double lz = m_wpos[2] - m_wpos[5];

     double xhit = (z - m_wpos[2]) * lx / lz + m_wpos[0];
     double yhit = (z - m_wpos[2]) * ly / lz + m_wpos[1];

     double r = sqrt( (xhit * xhit) + (yhit * yhit) );
     double phi = asin(yhit / r);

     if(xhit < 0) phi = PI - phi;
     if(phi < 0) phi += PI2;

     return phi;
}

void MdcCosWire::print(FILE* fout) const {
     fprintf(fout, "\n%10s %10s %10s %10s %10s %10s %10s\n", 
	     "WireLabel", "Wire", "Layer", "Cell", 
	     "Radius", "PhiF", "Ten");
     fprintf(fout, "%10s %10d %10d %10d\n\n", getWireLabel(),
	     getWireId(), getLayerId(), getCellId() );

     fprintf(fout, "%10s %10s %10s %10s %10s %10s %10s\n", 
	     "Xf", "Yf", "Zf", "Xb", "Yb", "Zb", "Sag");
     fprintf(fout, "%10.3f %10.3f %10.3f %10.3f %10.3f %10.3f\n\n", 
	     getXf(), getYf(), getZf(), getXb(), getYb(), getZb() );

}
