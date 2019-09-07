
#include "BesVisLib/BesGMenuTitle.h"

ClassImp(BesGMenuTitle)

//______________________________________________________________________________
BesGMenuTitle::BesGMenuTitle(const TGWindow *p, TGHotString *s, TGPopupMenu *menu,
                             GContext_t norm, FontStruct_t font, UInt_t options)
        : TGMenuTitle(p, s, menu, norm, font, options)
{
    fPic   = 0;
    fPicHL = 0;
}

//______________________________________________________________________________
void BesGMenuTitle::DoRedraw()
{
    // Draw a menu title.

    TGFrame::DoRedraw();

    int x, y, max_ascent, max_descent;
    x = y = 4;

    gVirtualX->GetFontProperties(fFontStruct, max_ascent, max_descent);

    if (fState) {
        if (fPicHL) SetBackgroundPixmap(fPicHL);
        else SetBackgroundColor(GetDefaultSelectedBackground());
        gVirtualX->ClearWindow(fId);
        fLabel->Draw(fId, fSelGC, x, y + max_ascent);
    } else {
        if (fPic) SetBackgroundPixmap(fPic);
        else SetBackgroundColor(GetDefaultFrameBackground());
        gVirtualX->ClearWindow(fId);
        fLabel->Draw(fId, fNormGC, x, y + max_ascent);
    }
}
