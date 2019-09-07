//$id$
//
//$log$

/*
 *    2003/08/30   Zhengyun You     Peking University
 * 
 *    2004/09/09   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

using namespace std;

#include "MucGeomSvc/MucGeometron.h"

MucGeometron::MucGeometron()
{
  // Constructor.
}

MucGeometron::~MucGeometron()
{
  // Destructor.
}

bool 
MucGeometron::GetIntersectionLinePlane(const HepPoint3D pLine,
				       const Hep3Vector vectLine,
				       const HepPlane3D plane,
				       HepPoint3D& cross)
{
  // Given a line by basepoint linePoint and direction lineDir,
  // find the intersection with the plane.
  HepPoint3D linePoint = pLine;
  Hep3Vector lineDir = vectLine;

  HepPoint3D planePoint = plane.point();
  Hep3Vector planeDir(plane.a(), plane.b(), plane.c());

  lineDir.setMag(1.0);
  planeDir.setMag(1.0);

  // Vector connecting the basepoint.
  HepPoint3D basePoint = planePoint - linePoint;

  double distance, denominator; // d , cos(theta).

  denominator = planeDir.dot(lineDir);

  if( denominator != 0 ) {
    // Line and gap are not parallel.
    distance = 
      (planeDir.dot(basePoint)) / denominator;
    cross = linePoint + lineDir * distance;
    return 1;
  }
  else {
    // Line and gap are parrellel.
    // cout << " Line is parrellel to plane! No intersect point found!" << endl;
    return 0;
  }
}


bool
MucGeometron::GetIntersectionLinePlaneWithSigma(const HepPoint3D pLine,
    const Hep3Vector vectLine,
    const HepPoint3D pLineSigma,
    const Hep3Vector vectLineSigma,
    const HepPlane3D plane,
    HepPoint3D& cross,
    HepPoint3D& crossSigma)
{
  HepPoint3D linePoint = pLine;
  Hep3Vector lineDir = vectLine;

  HepPoint3D planePoint = plane.point();
  Hep3Vector planeDir(plane.a(), plane.b(), plane.c());

  lineDir.setMag(1.0);
  planeDir.setMag(1.0);
  HepPoint3D basePoint = planePoint - linePoint;

  double distance, denominator; // d , cos(theta).

  denominator = planeDir.dot(lineDir);
  if( denominator != 0 ) {
    distance =
      (planeDir.dot(basePoint)) / denominator;
    cross = linePoint + lineDir * distance;

    //calculation of the uncertainty in intercept

    double x0  = pLine.x();
    double y0  = pLine.y();
    double z0  = pLine.z();

    double vx  = vectLine.x();
    double vy  = vectLine.y();
    double vz  = vectLine.z();

    double dx0 = pLineSigma.x();
    double dy0 = pLineSigma.y();
    double dz0 = pLineSigma.z();

    double dvx = vectLineSigma.x();
    double dvy = vectLineSigma.y();
    double dvz = vectLineSigma.z();

    double pa  = plane.a();
    double pb  = plane.b();
    double pc  = plane.c();

    double Const1 = planeDir.dot(planePoint);
    double Const2 = pa*vx + pb*vy + pc*vz;
    double Const3 = pa*x0 + pb*y0 + pc*z0;
    

    double sigmaX_1 =  dx0 * dx0 * ( pb * vy + pc * vz ) / Const2;
    double sigmaX_2 = (-1) * dy0 * dy0 * pb * vx / Const2;
    double sigmaX_3 = (-1) * dz0 * dz0 * pc * vx / Const2;
    double sigmaX_4 =  dvx * dvx * ( Const1 - Const3) * ( pb * vy + pc * vz) / Const2 / Const2;
    double sigmaX_5 = (-1) * dvy * dvy * vx * ( Const1 - Const3) * pb / Const2 / Const2;
    double sigmaX_6 = (-1) * dvz * dvz * vx * ( Const1 - Const3) * pc / Const2 / Const2;

    double sigmaX   = sigmaX_1 + sigmaX_2 + sigmaX_3 + sigmaX_4 + sigmaX_5 + sigmaX_6;

    double sigmaY_1 = (-1) * dx0 * dx0 * pa * vy / Const2;
    double sigmaY_2 =  dy0 * dy0 * ( 1 - pb * vy / Const2);
    double sigmaY_3 = (-1) * dz0 * dz0 * pc * vy / Const2;
    double sigmaY_4 = (-1) * dvx * dvx * ( Const1 - Const3) * pa * vy / Const2 / Const2;
    double sigmaY_5 =  dvy * dvy * ( Const1 - Const3) * ( pa * vx + pc * vz) / Const2 / Const2;
    double sigmaY_6 = (-1) * dvz * dvz * ( Const1 - Const3) * pc * vy / Const2 / Const2;

    double sigmaY   = sigmaY_1 + sigmaY_2 + sigmaY_3 + sigmaY_4 + sigmaY_5 + sigmaY_6;

    double sigmaZ_1 = (-1) * dx0 * dx0 * pa * vz / Const2;
    double sigmaZ_2 = (-1) * dy0 * dy0 * pb * vz / Const2;
    double sigmaZ_3 =  dz0 * dz0 * ( 1 - pc * vz / Const2);
    double sigmaZ_4 = (-1) * dvx * dvx * ( Const1 - Const3) * pa * vz / Const2 / Const2;
    double sigmaZ_5 = (-1) * dvy * dvy * ( Const1 - Const3) * pb * vz / Const2 / Const2;
    double sigmaZ_6 =  dvz * dvz * ( Const1 - Const3) * ( pa * vx + pb * vy) / Const2 / Const2;

    double sigmaZ   = sigmaZ_1 + sigmaZ_2 + sigmaZ_3 + sigmaZ_4 + sigmaZ_5 + sigmaZ_6;


    HepPoint3D c1(sqrt(abs(sigmaX)), sqrt(abs(sigmaY)), sqrt(abs(sigmaZ)));
    crossSigma = c1;    
    return 1;
  }
  else {
    return 0;
  }
}

bool
MucGeometron::GetIntersectionQuadPlaneLocal(const int part,  //liangyt 2009.3.12
				       const int orient,
                                       const float a,              //y = a * x * x + b * x + c;
                                       const float b,
                                       const float c,
                                       const HepPlane3D plane,
                                       HepPoint3D& cross1,
                                       HepPoint3D& cross2)
{

  if(a == 0) {
    // not quad
   return 0;

  }

  HepPoint3D planePoint = plane.point();
  Hep3Vector planeDir(plane.a(), plane.b(), plane.c());

  float A, B, C, D;
  A = plane.a(); B = plane.b(); C = plane.c();
  D = planeDir.dot(planePoint);

  //B*a*x^2 + (Bb + A)*x + Bc - D = 0

  //cout<<"in geomtron ABCD = "<<A<<" "<<B<<" "<<C<<" "<<D<<endl;

  float a1 = B*a;
  float b1 = B*b + A;
  float c1 = B*c - D;

  float b2sub4ac = b1*b1 - 4*a1*c1;

  //cout<<"in geomtron abc delta = "<<a1<<" "<<b1<<" "<<c1<<" "<<b2sub4ac<<endl;

  float x1, x2, y1, y2, z1, z2;
  if(abs(a1)>10E-10){
    if(b2sub4ac>=0)
      {
        x1 = (-b1+sqrt(b2sub4ac))/(2*a1);
        x2 = (-b1-sqrt(b2sub4ac))/(2*a1);
        y1 = a*x1*x1 + b*x1 + c;
        y2 = a*x2*x2 + b*x2 + c;
        //cout<<"in MucGeometron------ x1 y1 z1 = "<<x1<<" "<<y1<<" "<<z1<<endl;
        //cout<<"in MucGeometron------ x2 y2 z2 = "<<x2<<" "<<y2<<" "<<z2<<endl;
        cross1.set(x1, y1, z1);
        cross2.set(x2, y2, z2);
        return 1;
      }
    else{
      //cout<<"MucGeometron: no intersection!"<<endl;
      cross1.set(-9999,-9999,-9999);
      cross2.set(-9999,-9999,-9999);
      return 0;
    }
  }else{
    x1 = -c1/b1;
    y1 = a*x1*x1 + b*x1 + c;
    //cout<<"in MucGeometron------ x1 y1 z1 = "<<x1<<" "<<y1<<" "<<z1<<endl;
    cross1.set(x1, y1, z1);
    cross2.set(x1, y1, z1);
    //cout<<"in MucGeometron: abs(a1)<10E-10"<<endl;
    return 1;

  }


}



bool 
MucGeometron::GetIntersectionQuadPlane(const HepPoint3D pLine,     //liangyt 2007.4.9
				       const float vy,
				       const float y0,
				       const float a,              //y = a * x * x + b * x + c;
				       const float b,
				       const float c,
				       const HepPlane3D plane,    
				       HepPoint3D& cross1,
				       HepPoint3D& cross2)
{
  // Given a parabola  by a b c,
  // find the intersection with the plane.

  //y = vy * z + y0;
  //y = a * x * x + b * x + c;
  //plane.a() * x + plane.b() * y + plane.c() * z = planeDir.dot(planePoint)

  HepPoint3D planePoint = plane.point();
  Hep3Vector planeDir(plane.a(), plane.b(), plane.c());

  float A, B, C, D;
  A = plane.a(); B = plane.b(); C = plane.c(); 
  D = planeDir.dot(planePoint);

  //(B+Cv)*a*x^2 + ((B+Cv)b + A)*x + (B+Cv)c + Cy0 - D = 0

  float a1 = (B+C/vy)*a;
  float b1 = (B+C/vy)*b + A;
  float c1 = (B+C/vy)*c - C/y0 - D;

  float b2sub4ac = b1*b1 - 4*a1*c1;

  float x1, x2, y1, y2, z1, z2;
  if(abs(a1)>10E-10){  
    if(b2sub4ac>=0)
      {
	x1 = (-b1+sqrt(b2sub4ac))/(2*a1);
	x2 = (-b1-sqrt(b2sub4ac))/(2*a1);
	y1 = a*x1*x1 + b*x1 + c;
	y2 = a*x2*x2 + b*x2 + c;
	z1 = (y1 - y0)/vy;
	z2 = (y2 - y0)/vy;
	//cout<<"in MucGeometron------ x1 y1 z1 = "<<x1<<" "<<y1<<" "<<z1<<endl;
	//cout<<"in MucGeometron------ x2 y2 z2 = "<<x2<<" "<<y2<<" "<<z2<<endl;
	cross1.set(x1, y1, z1);
	cross2.set(x2, y2, z2);
	return 1;
      }
    else{
      //cout<<"MucGeometron: no intersection!"<<endl;
      cross1.set(-9999,-9999,-9999);
      cross2.set(-9999,-9999,-9999);
      return 0;
    }
  }else{
    x1 = -c1/b1;
    y1 = a*x1*x1 + b*x1 + c;
    z1 = (y1 - y0)/vy;
    //cout<<"in MucGeometron------ x1 y1 z1 = "<<x1<<" "<<y1<<" "<<z1<<endl;
    cross1.set(x1, y1, z1);
    cross2.set(x1, y1, z1);
    //cout<<"in MucGeometron: abs(a1)<10E-10"<<endl;
    return 1;

  }

}
