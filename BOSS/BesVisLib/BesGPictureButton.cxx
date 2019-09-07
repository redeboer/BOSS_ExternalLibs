
#include "BesVisLib/BesGPictureButton.h"

using namespace std;

#ifndef __CINT__
ClassImp(BesGPictureButton)
#endif

//______________________________________________________________________________
BesGPictureButton::BesGPictureButton(const TGWindow *p, const TGPicture *pic,
                                     Int_t id, GContext_t norm, UInt_t option) : TGPictureButton(p, pic, id, norm, option)
{
    fPicHL = 0;
    fState = false;
}

//______________________________________________________________________________
BesGPictureButton::BesGPictureButton(const TGWindow *p, const TGPicture *pic,
                                     const char *cmd, Int_t id, GContext_t norm, UInt_t option)
        : TGPictureButton(p, pic, cmd, id, norm, option)
{
    fPicHL = 0;
    fState = false;
}

//______________________________________________________________________________
BesGPictureButton::BesGPictureButton(const TGWindow *p, const char *pic,
                                     Int_t id, GContext_t norm, UInt_t option) : TGPictureButton(p, pic, id, norm, option)
{
    fPicHL = 0;
    fState = false;
}

//______________________________________________________________________________
Bool_t BesGPictureButton::HandleCrossing(Event_t *event)
{
    // Handle mouse crossing event.

    static const TGPicture *picTemp;
    static Bool_t state; // state when entering

    if (fPicHL) {
        if (event->fType == kEnterNotify) {
            picTemp = fPic;
            state = fState;
            SetPicture(fPicHL);
        }
        else {
            if (fState == state) SetPicture(picTemp); // if state unchanged, set to pic when cursor entering
            else SetPicture(fPic);                   // else, set to state pic
        }
    }

    /*
     if (fTip) {
        if (event->fType == kEnterNotify)
           fTip->Reset();
        else
           fTip->Hide();
     }

     if ((fgDbw != event->fWindow) || (fgReleaseBtn == event->fWindow)) return kTRUE;

     if (!(event->fState & (kButton1Mask | kButton2Mask | kButton3Mask)))
        return kTRUE;

     if (fState == kButtonEngaged || fState == kButtonDisabled) return kTRUE;

     if (event->fType == kEnterNotify) {
        SetState(kButtonDown, kFALSE);
     } else {
        SetState(kButtonUp, kFALSE);
     }

    */
    TGButton::HandleCrossing(event);
    return kTRUE;
}

//______________________________________________________________________________
void BesGPictureButton::SetPicture(const TGPicture *new_pic)
{
    // once you SetPicture, you show set the button state
    TGPictureButton::SetPicture(new_pic);
}
