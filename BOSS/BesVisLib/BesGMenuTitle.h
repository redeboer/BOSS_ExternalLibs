#ifndef BESGMENUTITLE_H_
#define BESGMENUTITLE_H_

#include <TGMenu.h>
#include <TGPicture.h>

class TGPopupMenu;
class TGMenuBar;
class TGMenuTitle;
class TTimer;

class BesGMenuTitle : public TGMenuTitle {

protected:

  Pixmap_t  fPic;        // picture shown
  Pixmap_t  fPicHL;      // picture when selected
  
  virtual void DoRedraw();

public:

  BesGMenuTitle(const TGWindow *p = 0, TGHotString *s = 0, TGPopupMenu *menu = 0,
               GContext_t norm = GetDefaultGC()(),
               FontStruct_t font = GetDefaultFontStruct(),
               UInt_t options = 0);

  virtual void SetPicture(Pixmap_t pic)   { fPic   = pic; }
  virtual void SetPictureHL(Pixmap_t pic) { fPicHL = pic; }

   ClassDef(BesGMenuTitle,0)  // Menu title class
};
#endif
