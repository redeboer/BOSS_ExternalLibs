
#ifndef BESGMENUBAR_H
#define BESGMENUBAR_H

#include "TGMenu.h"
#include "BesGMenuTitle.h"

class BesGMenuBar : public TGMenuBar {

 protected:
   virtual void AddFrameBefore(TGFrame *f, TGLayoutHints *l = 0,
                               TGPopupMenu *before = 0);

 public:
   BesGMenuBar(const TGWindow *p = 0, UInt_t w = 60, UInt_t h = 20,
	       UInt_t options = kHorizontalFrame | kRaisedFrame);

   virtual void AddPopup(TGHotString *s, TGPopupMenu *menu, TGLayoutHints *l,
                         TGPopupMenu *before = 0);
   virtual void AddPopup(const char *s, TGPopupMenu *menu, TGLayoutHints *l,
                         TGPopupMenu *before = 0);

   BesGMenuTitle *GetMenuTitle(Int_t i);
   
   ClassDef(BesGMenuBar,0)  // Menu bar class
};

#endif
