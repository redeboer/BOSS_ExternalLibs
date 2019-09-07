//
// BesAboutHelp.h
//

#ifndef BESABOUTHELP_h
#define BESABOUTHELP_h

#include <TGFrame.h>

class TGHorizontalFrame;
class TGVerticalFrame;
class TGCompositeFrame;
class TGLabel;
class TList;
class TEnv;
class TVirtualX;
class TGClient;
class TGTab;
class TGLayoutHints;
class TGListBox;
class TGTextButton;
class TString;
class TGIcon;
class TGPicture;

class BesAboutHelp : public TGTransientFrame {

 private:
  TGHorizontalFrame   *fTitleFrame;   // Title frame
  TGLabel             *fTitle;        // Title
  TGIcon              *fIcon;         // Icon
  TGTab               *fMain;         // Main tab frame
  TGVerticalFrame     *fAbout;        // 1st tab
  TGListBox           *fVersion;      // 2nd tab
  TGLabel             *fDescription1; // Line 1 of description
  TGLabel             *fDescription2; // Line 2 of description
  TGLabel             *fDescription3; // Line 3 of description
  TGLabel             *fDescription4; // Line 4 of description
  TGHorizontalFrame   *fBottomFrame;  // Bottom frame
  TGTextButton        *fCloseBtn;     // Close button
  TList               *fWidgets;      // list for widgets to be destroyed by destructor
  TGLayoutHints       *fLayout;       // dummy for TGlayouts to add to fWidgets
  TGVerticalFrame     *fMdcPaletteFrame;        // 1st tab

 public:
  BesAboutHelp(const TGWindow *p, const TGWindow *main);
  virtual ~BesAboutHelp();

  virtual Bool_t ProcessMessage(Long_t msg, Long_t parm, Long_t);  
  virtual Pixmap_t GetPic(const char *file);

  ClassDef(BesAboutHelp,0) // Info box about the Bes Vis client
};

#endif
