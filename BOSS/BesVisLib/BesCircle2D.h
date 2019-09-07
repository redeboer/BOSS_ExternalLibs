//
// BesCircle2D.h
//
// $Author: longpx $
// 2005/7/16
// Modified from zevis 2D shape

#ifndef BESCIRCLE2D_H
#define BESCIRCLE2D_H

#ifndef ROOT_TNamed
#include <TNamed.h>
#endif
#ifndef ROOT_TAttLine
#include <TAttLine.h>
#endif
#ifndef ROOT_TAttFill
#include <TAttFill.h>
#endif

class TString;
class TView;
class TPad;

class BesCircle2D : public TNamed, public TAttLine, public TAttFill {

private:
    Double_t *f_innerCircleX;
    Double_t *f_innerCircleY;
    Double_t *f_outerCircleX;
    Double_t *f_outerCircleY;
    Double_t *f_areaX;
    Double_t *f_areaY;
    //Long Peixun's update: fInnerRadius, fOuterRadius are single variables, not arrays
    Double_t fInnerRadius; //inner Radius of circle
    Double_t fOuterRadius; //outer Radius of circle
    Double_t *fCenter;      //[3] center of circle
    Int_t fNSegment;

public:
    BesCircle2D();
    BesCircle2D(const char* name, const char* title, Double_t innerRadius, Double_t outerRadius, Double_t *center);
    virtual ~BesCircle2D();

    virtual Int_t DistancetoPrimitive(Int_t px, Int_t py);
    virtual void  ExecuteEvent(Int_t event, Int_t px, Int_t py);
    virtual void  Draw(Option_t *option = "");
    virtual void  Paint(Option_t *option = "");
    virtual char *GetObjectInfo(Int_t px, Int_t py) const;

    virtual void SetCenter(Double_t x, Double_t y, Double_t z);
    virtual void GetCenter(Double_t *center);
    virtual void SetNSegment(Int_t np) {
        fNSegment = np;
    }

    ClassDef(BesCircle2D,2) //  Circle in 2D
};

#endif
