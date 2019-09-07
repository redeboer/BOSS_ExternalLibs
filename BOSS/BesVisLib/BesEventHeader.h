//
// BesEventHeader.h
//

#ifndef BES_EVENT_HEADER
#define BES_EVENT_HEADER

#include <TObject.h>
#include <TString.h>
#include <vector>

using std::vector;

class BesEventHeader : public TObject{

 private:
  Long64_t   fRun;               // Run number
  Long64_t   fEvent;             // Event number
  Bool_t  fMC;                // MC event   
  
  Int_t   fDay;               // Day of Event
  Int_t   fMonth;             // Month of Event
  Int_t   fYear;              // Year of Event
  Int_t   fHour;              // Hour of Event
  Int_t   fMin;               // Minute of Event
  Int_t   fSec;               // Second of Event

  Bool_t   fRec;              //Long Peixun's update: Reconstruction Event
  Double_t fEvTime;           // event start time 
  Int_t    fEvTimeStatus;     // event start time status
  Double_t fEvTimeQuality;    // event start time quality
  
  Double_t fP;                // Total momentum of all Mdc Tracks (in GeV/c)
  Double_t fPt;               // Transverse momentum
  Double_t fPx;   
  Double_t fPy;   
  Double_t fPz;   
  Double_t fT;   
  Double_t fE;   
  Int_t fTimeType;
  vector<Int_t> fTrigChannelVector;
  vector<Int_t> fTrigConditionVector;


 public:
  BesEventHeader();
  virtual ~BesEventHeader(); 

  virtual void SetEventHeaderGeneral(Long64_t run, Long64_t event,Int_t time1,Int_t time2);
  virtual void SetEventTrig(Int_t timeType, vector<Int_t>& trigConditionVector, vector<Int_t>& trigChannelVector);
  virtual void SetEventMC(Bool_t mc);
  virtual void SetEventRec(Bool_t rec);     //Long Peixun's update: Rec flag
  virtual void SetEventEvTime(Double_t time, Int_t status, Double_t quality);
  virtual void SetEventMdc(Double_t p, Double_t pt, Double_t px, Double_t py, Double_t pz);
  virtual void SetEventTof(Double_t t);
  virtual void SetEventEmc(Double_t e);
  //virtual void SetEventTrig(Double_t e);

  void PrintWord(TString &word,Int_t nbits, Int_t trigger,Int_t trigger2=0) const ;

  inline Long64_t GetRunNumber() { return fRun; }
  inline Long64_t GetEventNumber() { return fEvent; }
  inline Bool_t GetEventMC() { return fMC; }

  inline Int_t GetTimeType() { return fTimeType; }
  inline vector<Int_t>& GetTrigChannelVector() { return fTrigChannelVector; }
  inline vector<Int_t>& GetTrigConditionVector() { return fTrigConditionVector; }

  inline Int_t GetEventYear() { return fYear; }
  inline Int_t GetEventMonth() { return fMonth; }
  inline Int_t GetEventDay() { return fDay; }
  inline Int_t GetEventHour() { return fHour;}
  inline Int_t GetEventMin() { return fMin;}
  inline Int_t GetEventSec() { return fSec;}
 
  inline Bool_t GetEventRec() { return fRec; }    //Long Peixun's update: Rec flag
  inline Double_t GetEvTime() { return fEvTime; }
  inline Int_t    GetEvTimeStatus() { return fEvTimeStatus; }
  inline Double_t GetEvTimeQuality() { return fEvTimeQuality; }

  inline virtual Float_t GetP()  { return fP; }
  inline virtual Float_t GetPt() { return fPt; }
  inline virtual Float_t GetPx() { return fPx; }
  inline virtual Float_t GetPy() { return fPy; }
  inline virtual Float_t GetPz() { return fPz; }

  inline virtual Float_t GetT()  { return fT; }

  inline virtual Float_t GetE()  { return fE; }

  ClassDef(BesEventHeader,0) // Bes Event Header
};

#endif
