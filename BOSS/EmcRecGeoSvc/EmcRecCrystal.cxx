#include "EmcRecGeoSvc/EmcRecCrystal.h"

ostream& operator<<(ostream & os,const EmcRecCrystal& aCrystal)
{
   int i;
   if(aCrystal.Type()==EmcRecCrystal::SixPlane()) {
      for(i=0;i<8;i++){
	 if(i<7) {
	    os<<aCrystal.Get(i)<<endl;
	 } else {
	    os<<aCrystal.Get(i);
	 }
      }
   }
   if(aCrystal.Type()==EmcRecCrystal::SevenPlane()) {
      for(i=0;i<10;i++){
	 if(i<9) {
	    os<<aCrystal.Get(i)<<endl;
	 } else {
	    os<<aCrystal.Get(i);
	 }
      }
   }
   return os;
}
