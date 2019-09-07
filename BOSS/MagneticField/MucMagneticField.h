#ifndef MUCMAGNETICFIELD_H 
#define MUCMAGNETICFIELD_H 1

#include <string>

#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatibility will be enabled ONLY in CLHEP 1.9
typedef HepGeom::Point3D<double> HepPoint3D;
typedef HepGeom::Vector3D<double> HepVector3D;
#endif

class MucMagneticField
{
public:
#ifndef BEAN
  MucMagneticField();
#else
  MucMagneticField(const std::string Path);
#endif
  ~MucMagneticField();

  void getMucField(int part,int layer,int mat, HepPoint3D& r,HepVector3D& b);
  void readPar();
  
  std::string getPath()         {return path;}

private:
  static const int npar = 18;
  double bipx0[npar],bipy0[npar],bipz0[npar];//barrel iron par bx,by,bz in each layer
  double bipx1[npar],bipy1[npar],bipz1[npar];
  double bipx2[npar],bipy2[npar],bipz2[npar];
  double bipx3[npar],bipy3[npar],bipz3[npar];
  double bipx4[npar],bipy4[npar],bipz4[npar];
  double bipx5[npar],bipy5[npar],bipz5[npar];
  double bipx6[npar],bipy6[npar],bipz6[npar];
  double bipx7[npar],bipy7[npar],bipz7[npar];
  double bipx8[npar],bipy8[npar],bipz8[npar];

  double bapx0[npar],bapy0[npar],bapz0[npar];//barrel air par bx,by,bz in each layer 
  double bapx1[npar],bapy1[npar],bapz1[npar];
  double bapx2[npar],bapy2[npar],bapz2[npar];
  double bapx3[npar],bapy3[npar],bapz3[npar];
  double bapx4[npar],bapy4[npar],bapz4[npar];
  double bapx5[npar],bapy5[npar],bapz5[npar];
  double bapx6[npar],bapy6[npar],bapz6[npar];
  double bapx7[npar],bapy7[npar],bapz7[npar];

  static const int npar1 = 17;
  double aipx0[npar1],aipy0[npar1],aipz0[npar1];//endcap iron par bx,by,bz in each layer
  double aipx1[npar1],aipy1[npar1],aipz1[npar1];
  double aipx2[npar1],aipy2[npar1],aipz2[npar1];
  double aipx3[npar1],aipy3[npar1],aipz3[npar1];
  double aipx4[npar1],aipy4[npar1],aipz4[npar1];
  double aipx5[npar1],aipy5[npar1],aipz5[npar1];
  double aipx6[npar1],aipy6[npar1],aipz6[npar1];
  double aipx7[npar1],aipy7[npar1],aipz7[npar1];
  double aipx8[npar1],aipy8[npar1],aipz8[npar1];

  double aapx0[npar1],aapy0[npar1],aapz0[npar1];//endcap air par bx,by,bz in each layer
  double aapx1[npar1],aapy1[npar1],aapz1[npar1];
  double aapx2[npar1],aapy2[npar1],aapz2[npar1];
  double aapx3[npar1],aapy3[npar1],aapz3[npar1];
  double aapx4[npar1],aapy4[npar1],aapz4[npar1];
  double aapx5[npar1],aapy5[npar1],aapz5[npar1];
  double aapx6[npar1],aapy6[npar1],aapz6[npar1];
  double aapx7[npar1],aapy7[npar1],aapz7[npar1];

  double bp[npar];//barrel par
  double ep[npar1];//endcap par

  std::string filename;
  std::string path;             // path to data files
};
#endif
