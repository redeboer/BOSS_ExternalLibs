//
// BesCursor.h
//

#ifndef BES_CURSOR
#define BES_CURSOR

#include <TObject.h>

#ifndef BES_CURSORType
#define BES_CURSORType

enum EBESCursorType {
  kBesPick   = BIT(0),
  kBesHand   = BIT(1)
};
#endif

class BesCursor : public TObject {

 private:
  EBESCursorType fCursorType;
  Bool_t fShowInfo;
  
 public:
  
  BesCursor();
  ~BesCursor();

  
  void SetType(EBESCursorType cursorType) { fCursorType = cursorType; }
  EBESCursorType GetType() { return fCursorType; } 
  
  void SetShowInfo(Bool_t show) { fShowInfo = show; }
  Bool_t GetShowInfo() { return fShowInfo; }

  ClassDef(BesCursor,0) // BesCursor
};

R__EXTERN BesCursor *gBesCursor;
#endif

