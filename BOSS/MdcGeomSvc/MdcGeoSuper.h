// $Id: MdcGeoSuper.h,v 1.1.1.1 2005/02/25 09:13:20 codeman Exp $ // -*-c++-*-
// header file for a class called "MdcGeoSuper"
#ifndef MDC_GEO_SUPER_H
#define MDC_GEO_SUPER_H

#include <iostream>

class MdcGeoSuper {

  public:
    // Constructor.
     MdcGeoSuper():fId(0),fUpperR(0),fLowerR(0),fType(-1){};

    // Copy and Assignment
     MdcGeoSuper(const MdcGeoSuper& e):fId(e.Id()),fUpperR(e.UpperR()),fLowerR(e.LowerR()),fType(e.Type()){};

     MdcGeoSuper& operator=(const MdcGeoSuper& e) {
       if(this!=&e)
        {
	fId=e.Id();
        fUpperR=e.UpperR();
        fLowerR=e.LowerR();
        fType=e.Type();
        }
       return *this;
     }

    // Destructor
    ~MdcGeoSuper(){};

  public: // Extractors
    int Id(void) const {return fId; };
    double UpperR(void) const { return fUpperR; };
    double LowerR(void) const { return fLowerR; };
    int Type(void) const { return fType; };

  public: // Modifiers
    int Id(int i) { return fId=i; };
    double UpperR(double i) { return fUpperR=i; };
    double LowerR(double i) { return fLowerR=i; };
    int Type(int i) { return fType=i; };

  private:
    int fId;
    double fUpperR;
    double fLowerR;
    int fType;
};

#endif /* MdcGeoSuper_CLASS */

