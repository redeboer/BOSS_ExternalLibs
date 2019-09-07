//
//   EmcRecCrystal
//
//   Dec 18, 2003, Created by Zhe Wang
//
//   unit: mm, radian
//   
//   Only six planes tower-like crystal in barrel EMC.
//   Two kinds of cryscal exist in endcap EMC:
//       six planes and some seven planes cystals
//   Be careful when using them.
//   
#ifndef EMC_REC_CRYSTAL_H
#define EMC_REC_CRYSTAL_H

#include <iostream> 

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
  typedef HepGeom::Point3D<double> HepPoint3D;
#endif

using namespace std;
using namespace CLHEP;

class EmcRecCrystal
{
 public:
   // Constructors and destructors
   EmcRecCrystal() { 
      fType=INVALID_CRYSTAL;
      HepPoint3D O(0,0,0);
      for(int i=0;i<10;i++){
	 fV[i]=O;
      }
   }
   
   ~EmcRecCrystal() {}

   // Copy and Assignment
   EmcRecCrystal(const EmcRecCrystal& aCry) {
      fType=aCry.Type();
      for(int i=0;i<10;i++){
	 fV[i]=aCry.Get(i);
      }
   }
   
   EmcRecCrystal& operator=(const EmcRecCrystal& aCry) {
      if(this!=&aCry)
	{
	   fType=aCry.Type();
	   for(int i=0;i<10;i++){
	      fV[i]=aCry.Get(i);
	   }
	}
      return *this; 
   }

   // Accessment
   // Crystal type mask
   static int InvalidCrystal()
     { return INVALID_CRYSTAL; }
   
   static int SixPlane()
     { return SIX_PLANE; }
   
   static int SevenPlane()
     { return SEVEN_PLANE; }
   
   // Type read and write
   int Type() const
     { return fType; }
   
   int Type(int typ)
     {  fType=typ;
	if( (fType!=SIX_PLANE) && (fType!=SEVEN_PLANE) )
	  { fType=INVALID_CRYSTAL; }
	return fType; 
     }
   
   // Read Vertex
   HepPoint3D Get(int index) const {
      return fV[index];
   }
   
   // Get Crystal center
   HepPoint3D Center() const {
      if(fType==SIX_PLANE) {
	 return (fV[0]+fV[1]+fV[2]+fV[3]+fV[4]+fV[5]+fV[6]+fV[7])/8;
      }
      if(fType==SEVEN_PLANE) {
	 return (fV[0]+fV[1]+fV[2]+fV[3]+fV[4]+fV[5]+fV[6]+fV[7]+fV[8]+fV[9])/10;
      }
      HepPoint3D O(0,0,0);
      return O;
   }

   // Get Crystal front center
   HepPoint3D FrontCenter() const {
      if(fType==SIX_PLANE) {
   return (fV[0]+fV[1]+fV[2]+fV[3])/4;
      }
      if(fType==SEVEN_PLANE) {
   return (fV[0]+fV[1]+fV[2]+fV[3]+fV[4])/5;
      }
      HepPoint3D O(0,0,0);
      return O;
   }
   
   // Write Vertex
   void Set(int index, const HepPoint3D& aPoint) {
      fV[index]=aPoint;
   }
    
   void SetX(int index, double value) {
      fV[index].setX(value);
   }
   
   void SetY(int index, double value) {
      fV[index].setY(value);
   }

   void SetZ(int index, double value) {
      fV[index].setZ(value);
   }

   
   void BarrelCheckout() const {
      HepPoint3D t1,t2;
      cout<<"H="<<fV[4].distance(fV[7])<<endl;
      cout<<"h="<<fV[0].distance(fV[3])<<endl;
      t1=fV[3]-fV[7];
      t2=fV[4]-fV[7];
      cout<<"t="<<acos(t1*t2/(t1.mag()*t2.mag()))*180/pi<<endl;
      cout<<"B="<<fV[4].distance(fV[5])<<endl;
      cout<<"b="<<fV[0].distance(fV[1])<<endl;
      cout<<"A="<<fV[7].distance(fV[6])<<endl;
      cout<<"a="<<fV[3].distance(fV[2])<<endl;
      cout<<"L="<<fV[4].distance(fV[0])<<endl;
   }
   
   void EndCapCheckout() const {
      if(fType==SIX_PLANE) {
	 cout<<"A="<<fV[5].distance(fV[6])<<endl;
	 cout<<"a="<<fV[1].distance(fV[2])<<endl;
	 cout<<"B="<<fV[7].distance(fV[4])<<endl;
	 cout<<"b="<<fV[0].distance(fV[3])<<endl;
	 cout<<"C="<<fV[7].distance(fV[6])<<endl;
	 cout<<"c="<<fV[3].distance(fV[2])<<endl;
	 cout<<"D="<<fV[4].distance(fV[5])<<endl;
	 cout<<"d="<<fV[1].distance(fV[0])<<endl;
	 HepPoint3D t1,t2;
	 double t;
	 t1=fV[0]-fV[3];
	 t2=fV[2]-fV[3];
	 cout<<"alpha1="<<acos(t1*t2/(t1.mag()*t2.mag()))*180/pi<<endl;
	 t1=fV[3]-fV[2];
	 t2=fV[1]-fV[2];
	 cout<<"alpha2="<<acos(t1*t2/(t1.mag()*t2.mag()))*180/pi<<endl;
	 t1=fV[2]-fV[1];
	 t2=fV[0]-fV[1];
	 cout<<"alpha3="<<acos(t1*t2/(t1.mag()*t2.mag()))*180/pi<<endl;
	 t1=fV[3]-fV[0];
	 t2=fV[1]-fV[0];
	 cout<<"alpha4="<<acos(t1*t2/(t1.mag()*t2.mag()))*180/pi<<endl;
	 t1=fV[3]-fV[7];
	 t2=fV[4]-fV[7];
	 t=t1*t2/(t1.mag()*t2.mag());
	 t=sqrt(1-t*t);
	 cout<<"L="<<t1.mag()*t<<endl;
      } else {
	 cout<<"A="<<fV[5].distance(fV[9])<<endl;
	 cout<<"a="<<fV[0].distance(fV[4])<<endl;
	 cout<<"B="<<fV[9].distance(fV[8])<<endl;
	 cout<<"b="<<fV[4].distance(fV[3])<<endl;
	 cout<<"C="<<fV[8].distance(fV[7])<<endl;
	 cout<<"c="<<fV[3].distance(fV[2])<<endl;
	 cout<<"D="<<fV[7].distance(fV[6])<<endl;
	 cout<<"d="<<fV[2].distance(fV[1])<<endl;
	 cout<<"E="<<fV[6].distance(fV[5])<<endl;
	 cout<<"e="<<fV[1].distance(fV[0])<<endl;
	 HepPoint3D t1,t2;
	 double t;
	 t1=fV[1]-fV[0];
	 t2=fV[4]-fV[0];
	 cout<<"alpha1="<<acos(t1*t2/(t1.mag()*t2.mag()))*180/pi<<endl;
	 t1=fV[3]-fV[4];
	 t2=fV[0]-fV[4];
	 cout<<"alpha2="<<acos(t1*t2/(t1.mag()*t2.mag()))*180/pi<<endl;
	 t1=fV[2]-fV[3];
	 t2=fV[4]-fV[3];
	 cout<<"alpha3="<<acos(t1*t2/(t1.mag()*t2.mag()))*180/pi<<endl;
	 t1=fV[3]-fV[2];
	 t2=fV[1]-fV[2];
	 cout<<"alpha4="<<acos(t1*t2/(t1.mag()*t2.mag()))*180/pi<<endl;
	 t1=fV[0]-fV[1];
	 t2=fV[2]-fV[1];
	 cout<<"alpha5="<<acos(t1*t2/(t1.mag()*t2.mag()))*180/pi<<endl;
	 t1=fV[0]-fV[6];
	 t2=fV[5]-fV[6];
	 t=t1*t2/(t1.mag()*t2.mag());
	 t=sqrt(1-t*t);
	 cout<<"L="<<t1.mag()*t<<endl;
      }
   }

   // data members
 private:
   // Different type will gives different result.
   int fType;
   // Ten vertexes
   // For six planes crystal, only eight ones are in use
   HepPoint3D fV[10];
   
   // Type definition
   static const int INVALID_CRYSTAL  = -1;
   static const int SIX_PLANE        = 0;
   static const int SEVEN_PLANE      = 1;
};

ostream& operator<<(ostream & os, const EmcRecCrystal& aCrystal);
#endif /* EMC_REC_CRYSTAL_H */
