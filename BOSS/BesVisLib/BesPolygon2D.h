//
// BesPolygon2D.h
//
// $Author: longpx $
// 2005/7/16
// Modified from zevis 2D shape

#ifndef BESPOLYGON2D_H
#define BESPOLYGON2D_H

#ifndef ROOT_TNamed
#include <TNamed.h>
#endif
#ifndef ROOT_TAttLine
#include <TAttLine.h>
#endif
#ifndef ROOT_TAttFill
#include <TAttFill.h>
#endif

class TPad;
class TString;
class TView;
class TGeometry;
class TPaveText;

class BesPolygon2D : public TNamed, public TAttLine, public TAttFill {

    static int num;
protected:
    Double_t *f_xx;
    Double_t *f_yy;
    Int_t     fN;        // No. of points
    Double_t *fP;        // [3*fN] Points in world coordinates
    Double_t *fPBackUp;  // [3*fN] for fP BackUp when rotate;
    Double_t  fCenter [3];
    Bool_t    fRotatable;

    TPaveText *fInfoBox; //! info box

public:
    BesPolygon2D();
    BesPolygon2D(const char* name, const char* title, Int_t N, Double_t *P);
    virtual ~BesPolygon2D();

    virtual void  Draw(Option_t *option = "");
    virtual void  Paint(Option_t *option = "");
    virtual char *GetObjectInfo(Int_t px, Int_t py) const;

    virtual void  SetZRSign(Int_t sign);
    virtual void  Resize(Double_t ScaleFactor);
    virtual Int_t DistancetoPrimitive(Int_t px, Int_t py);
    virtual void  ExecuteEvent(Int_t event, Int_t px, Int_t py);
    virtual void  SetInfoBox(); //*MENU*
    virtual void  GetCenter(Double_t *Center) const;
    virtual void  RotatePhi(Double_t phi);
    virtual void  Restore();
    virtual void  SetSize(Double_t size);
    //Long Peixun's update: Stretch polygon along (sx, sy, sz)
    virtual void  Stretch(Double_t sx, Double_t sy, Double_t sz, Double_t factor);

    inline Bool_t IsRotatable() {
        return fRotatable;
    }
    inline void SetRotatable(Bool_t input) {
        fRotatable = input;
    }
    inline void SetName(const char* name) {
        TNamed::SetName(name);
    }
    inline void SetTitle(const char* name) {
        TNamed::SetTitle(name);
    }
    inline void Delete(Option_t *option) {
        TObject::Delete(option);
    }
    inline void DrawClass() {
        TObject::DrawClass();
    }
    inline void DrawClone(Option_t *option) {
        TObject::DrawClone(option);
    }
    inline void SetDrawOption(Option_t *option) {
        TObject::SetDrawOption(option);
    }
    inline void SetPoints(Double_t *P) {
        for ( Int_t i = 0; i < fN*3; i++ ) fP[i] = P[i];
    }

    ClassDef(BesPolygon2D,2) // Polygon in 2D
};

#endif
