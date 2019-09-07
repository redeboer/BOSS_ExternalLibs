
#include "BesVisLib/BesGMenuBar.h"

ClassImp(BesGMenuBar)

//______________________________________________________________________________
BesGMenuBar::BesGMenuBar(const TGWindow *p, UInt_t w, UInt_t h, UInt_t options)
        : TGMenuBar(p, w, h, options)
{

}

//______________________________________________________________________________
void BesGMenuBar::AddPopup(TGHotString *s, TGPopupMenu *menu, TGLayoutHints *l,
                           TGPopupMenu *before)
{
    // Add popup menu to menu bar. The hot string will be adopted by the
    // menubar (actually the menu title) and deleted when possible.
    // If before is not 0 the menu will be added before it.

    BesGMenuTitle *t;
    Int_t keycode;

    AddFrameBefore(t = new BesGMenuTitle(this, s, menu), l, before);
    fTitles->Add(t);  // keep track of menu titles for later cleanup in dtor

    if ((keycode = t->GetHotKeyCode()) != 0) {
        BindHotKey(keycode, kTRUE);
    }
}

//______________________________________________________________________________
void BesGMenuBar::AddPopup(const char *s, TGPopupMenu *menu, TGLayoutHints *l,
                           TGPopupMenu *before)
{
    // Add popup menu to menu bar. If before is not 0 the menu will be
    // added before it.

    AddPopup(new TGHotString(s), menu, l, before);
}

//______________________________________________________________________________
void BesGMenuBar::AddFrameBefore(TGFrame *f, TGLayoutHints *l,
                                 TGPopupMenu *before)
{
    // Private version of AddFrame for menubar, to make sure that we
    // indeed only add BesGMenuTitle objects to it. If before is not 0
    // the menu will be added before it.

    if (f->IsA() != BesGMenuTitle::Class()) {
        Error("AddFrameBefore", "may only add BesGMenuTitle objects to a menu bar");
        return;
    }

    if (!before) {
        AddFrame(f, l);
        return;
    }

    TGFrameElement *nw;

    nw = new TGFrameElement;
    nw->fFrame  = f;
    nw->fLayout = l ? l : fgDefaultHints;
    nw->fState  = 1;

    TGFrameElement *el;
    TIter next(fList);
    while ((el = (TGFrameElement *) next())) {
        BesGMenuTitle *t = (BesGMenuTitle *) el->fFrame;
        if (t->GetMenu() == before) {
            fList->AddBefore(el, nw);
            return;
        }
    }
    fList->Add(nw);
}

//______________________________________________________________________________
BesGMenuTitle *BesGMenuBar::GetMenuTitle(Int_t i)
{
    if (i >= 0 && i < fTitles->GetSize()) return (BesGMenuTitle*)fTitles->At(i);
    else return 0;
}


