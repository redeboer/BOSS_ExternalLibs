
#include <TPad.h>
#include "BesVisLib/BesPaveText.h"
#include <iostream>

using namespace std;

#ifndef __CINT__
ClassImp(BesPaveText)
#endif

//______________________________________________________________________________
BesPaveText::BesPaveText(): TPaveText()
{
}

//______________________________________________________________________________
BesPaveText::BesPaveText(Double_t x1, Double_t y1,Double_t x2, Double_t  y2, Option_t *option)
        :TPaveText(x1,y1,x2,y2,option)
{
    SetFillColor(1001);
    SetBorderSize(1);
    SetCornerRadius(0.2);

    fTextSize = 0.025;
}

//______________________________________________________________________________
BesPaveText::BesPaveText(const BesPaveText &pavetext) : TPaveText()
{

}

//______________________________________________________________________________
void BesPaveText::SetText(vector<TString> infoCon)
{
    Int_t n = infoCon.size();
    Double_t margin = GetMargin();
    Double_t dy = (1.0 - margin * 2) / n;

    //Long Peixun's update: If the number of lines is 2, add an empty line to avoid text out of box.
    if (n == 2) dy = (1.0 - margin * 2) / 3;

    Double_t coordx = 0.0;
    Double_t coordy = 0.0;
    for (Int_t i = 0; i < n; i++) {
        coordy = 1.0 - (i + 1) * dy;
        if (n == 1) coordy = 1.0;

        TText* tdata = AddText(coordx, coordy, infoCon[i].Data());
        tdata->SetTextSize(fTextSize/1.2);
        tdata->SetTextFont(42);
        if (i == 0 && n > 4) {
            tdata->SetTextSize(fTextSize);
            tdata->SetTextFont(62);
        }
        tdata->SetTextColor(1);
        tdata->SetTextAlign(12);

        if (infoCon[i] == TString("Fired")) {
            tdata->SetTextColor(kRed);
            //tdata->SetTextSize(fTextSize);
            tdata->SetTextFont(72);
        }
    }
    //Long Peixun's update: If the number of lines is 2, add an empty line to avoid text out of box.
    if (infoCon.size() == 2)
    {
        coordy = 1.0 - 3 * dy;
        TText* tdata = AddText(0.0, coordy, "");
        ++n;
    }

    fdxNDC = fTextSize * fLongest / 2.2;
    fdyNDC = fTextSize * n * 1.2; //fLines->GetSize() * 1.2;

    ConvertNDCtoPad();
    // This is necessary, or when fTip was drawn the first time, it will in center, as its fX1 fX2 fY1 fY2 have not been changed.
    SetX1NDC(0.0);
    SetY1NDC(0.0);
    SetX2NDC(fdxNDC);
    SetY2NDC(fdyNDC);
}

//______________________________________________________________________________
void BesPaveText::SetPos(Int_t px, Int_t py)
{
    if (!gPad) {
        cout << "BesPaveText::SetPos, gPad does not exist" << endl;
        return;
    }

    Double_t x = gPad->AbsPixeltoX(px+10);
    Double_t y = gPad->AbsPixeltoY(py-10);
    Double_t x1, y1, x2, y2, xNDC, yNDC;
    gPad->GetRange(x1, y1, x2, y2);
    xNDC = (x-x1)/(x2-x1);
    yNDC = (y-y1)/(y2-y1);
    //cout << xNDC << " " << yNDC << endl;

    x1 = xNDC;
    y1 = yNDC;
    x2 = xNDC + fdxNDC;
    y2 = yNDC + fdyNDC;

    Double_t delta = 0.02;
    if (x2 > 1.0 && y2 > 1.0) {
        x1 -= (fdxNDC+delta);
        x2 -= (fdxNDC+delta);
        y1 -= (fdyNDC+delta);
        y2 -= (fdyNDC+delta);
    }
    else if (x2 > 1.0) {
        x1 -= fdxNDC;
        x2 -= fdxNDC;
    }
    if (y2 > 1.0) {
        y1 -= fdyNDC;
        y2 -= fdyNDC;
    }

    SetX1NDC(x1);
    SetY1NDC(y1);
    SetX2NDC(x2);
    SetY2NDC(y2);
    ConvertNDCtoPad();
}
