// a template header file for a class called "MdcGeoEnd"
#ifndef MDC_GEO_End_H
#define MDC_GEO_End_H

#include <iostream>
#include <string>
using namespace std;

class MdcGeoEnd {
//class used for construct tube and endplate in MC
public:
  MdcGeoEnd(void){};
  ~MdcGeoEnd(void){};

public:
  int    Id(void)     const { return fId;    }
  double Length(void) const { return fLength;} //Full length In Z direction
  double InnerR(void) const { return fInnerR;} //From Z axes to bottom surface of those segments.
  double OutR(void)   const { return fOutR;  }//From Z axes to upper surface of those segments.
  double Z(void)      const { return fZ;     }  //Offset value of segment center from Z=0 
  string Name(void)   const { return fName;  } //name of segment

public:
  void Id(int x) {fId=x;}
  void Length(double x) {fLength=x;}
  void InnerR(double x) {fInnerR=x;}
  void OutR(double x)  {fOutR=x;}
  void Z(double x)  {fZ=x;}
  void Name(string x) {fName=x;}

private:
  int fId;
  double fLength, fInnerR, fOutR, fZ;
  string fName; 
};

#endif /* MdcGeoEnd_CLASS */


