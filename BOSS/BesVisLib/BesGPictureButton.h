
#ifndef BESGPICTUREBUTTON_H
#define BESGPICTUREBUTTON_H

#include "TGButton.h"

class BesGPictureButton : public TGPictureButton {

 public:

  BesGPictureButton(const TGWindow *p, const TGPicture *pic, Int_t id = -1,
		    GContext_t norm = GetDefaultGC()(),
		    UInt_t option = kRaisedFrame | kDoubleBorder);
  BesGPictureButton(const TGWindow *p, const TGPicture *pic, const char *cmd,
		    Int_t id = -1, GContext_t norm = GetDefaultGC()(),
		    UInt_t option = kRaisedFrame | kDoubleBorder);
  BesGPictureButton(const TGWindow *p = 0, const char* pic = 0, Int_t id = -1,
		    GContext_t norm = GetDefaultGC()(),
		    UInt_t option = kRaisedFrame | kDoubleBorder);

  Bool_t HandleCrossing(Event_t *event);
  Bool_t IsStateOn() { return fState; }
  virtual void SetState(Bool_t state) { fState = state; }

  virtual void SetPicture(const TGPicture *new_pic);
  virtual void SetPictureHL(const TGPicture *hl_pic) { fPicHL = hl_pic; }

 private:

protected:

  const TGPicture   *fPicHL;        // picture shown when button selected
  Bool_t fState;

  ClassDef(BesGPictureButton,0) //  BesGPictureButton
};

#endif


