//
// BesHeader.h
//


#ifndef BESHEADER
#define BESHEADER

#include <TPaveText.h>
#include <TQObject.h>

class BesHeader : public TPaveText, public TQObject {
//class BesHeader : public TPave, public TQObject {

 private:

  Bool_t fMdcOn;         // show Mdc information on ?
  Bool_t fTofOn;         // show Tof information on ?
  Bool_t fEmcOn;         // show Emc information on ?

 public:
  
  BesHeader();
  BesHeader(Double_t x1,Double_t y1,Double_t x2,Double_t y2,Option_t *option);
  
  virtual ~BesHeader();

  virtual Int_t     DistancetoPrimitive(Int_t px, Int_t py);

  inline void   SetMdcOn(Bool_t mdcOn)      { fMdcOn = mdcOn; TransferMdcOn(fMdcOn);} // *TOGGLE*
  inline Bool_t GetMdcOn()                  { return fMdcOn; }
  inline void   TransferMdcOn(Bool_t input) { Emit("TransferMdcOn(Bool_t)",input); } // *SIGNAL*

  inline void   SetTofOn(Bool_t tofOn)      { fTofOn = tofOn; TransferTofOn(fTofOn);} // *TOGGLE*
  inline Bool_t GetTofOn()                  { return fTofOn; }
  inline void   TransferTofOn(Bool_t input) { Emit("TransferTofOn(Bool_t)",input); } // *SIGNAL*

  inline void   SetEmcOn(Bool_t emcOn)      { fEmcOn = emcOn; TransferEmcOn(fEmcOn);} // *TOGGLE*
  inline Bool_t GetEmcOn()                  { return fEmcOn; }
  inline void   TransferEmcOn(Bool_t input) { Emit("TransferEmcOn(Bool_t)",input); } // *SIGNAL*

  inline virtual void  Clear(Option_t *option=""){TPaveText ::Clear(option);}

  inline virtual void  DeleteText(){TPaveText::DeleteText();} 
  inline virtual void  EditText(){TPaveText::EditText();}
  inline virtual void  InsertLine(){TPaveText::InsertLine();} 
  inline virtual void  InsertText(const char *label){TPaveText::InsertText(label);}  
  inline virtual void  ReadFile(const char *filename, Option_t *option="", Int_t nlines=50, Int_t fromline=0){TPaveText::ReadFile(filename, option, nlines, fromline);} 
  inline virtual void  SetAllWith(const char *text, Option_t *option, Double_t value){TPaveText::SetAllWith(text, option, value);} 
  inline virtual void  SetLabel(const char *label){TPaveText::SetLabel(label);} 
  inline virtual void  SetMargin(Float_t margin=0.05){TPaveText::SetMargin(margin);} 

  inline virtual void  SetBorderSize(Int_t bordersize=4){TPave::SetBorderSize(bordersize);}  
  inline virtual void  SetCornerRadius(Double_t rad = 0.2){TPave::SetCornerRadius(rad);}  
  inline virtual void  SetName(const char *name=""){TPave::SetName(name);}  

  inline virtual void     Delete(Option_t *option="") {TObject::Delete(option);} 
  inline virtual void     DrawClass()   {TObject::DrawClass()  ;} 
  inline virtual TObject    *DrawClone(Option_t *option="")   {return TObject::DrawClone(option) ;} 
  inline virtual void     Dump()   {TObject::Dump()  ;} 
  inline virtual void     Inspect()  {TObject :: Inspect()  ;} 
  inline virtual void     SetDrawOption(Option_t *option="")  {TObject::SetDrawOption(option);}  
  inline virtual void     SetLineAttributes()  {TAttLine::SetLineAttributes();} 
  inline virtual void     SetFillAttributes()  {TAttFill::SetFillAttributes();} 
  inline virtual void     SetTextAttributes()   {TAttText::SetTextAttributes();} 
  inline virtual void     SetTextAngle(Float_t tangle=0)  {TAttText::SetTextAngle(tangle) ;} 

  ClassDef(BesHeader,0) // BesVis Header
};

#endif


