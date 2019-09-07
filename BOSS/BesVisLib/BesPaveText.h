
#ifndef BESPAVETEXT_H
#define BESPAVETEXT_H

#include "TPaveText.h"
#include "TString.h"
//#include <vector.h>
//2011.02.24 root update from 5.14 to 5.24
#include <vector>

class BesPaveText : public TPaveText {

 protected:
  Double_t fTextSize;
  Double_t fdxNDC, fdyNDC;
  
 public:

  BesPaveText();
  BesPaveText(Double_t x1, Double_t y1,Double_t x2 ,Double_t y2, Option_t *option="br");
  BesPaveText(const BesPaveText &pavetext);

  virtual void SetText(std::vector<TString> infoCon);
  virtual void SetPos(Int_t px, Int_t py);

  ClassDef(BesPaveText,0) //  BesPaveText
};

#endif


