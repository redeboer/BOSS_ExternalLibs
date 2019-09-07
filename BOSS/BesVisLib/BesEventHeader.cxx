//
// BesEventHeader.cxx
//

#include "BesVisLib/BesEventHeader.h"
#include <iostream>

using namespace std;

#ifndef __CINT__
ClassImp(BesEventHeader)
#endif

//_____________________________________________________________
// BesEventHeader
// Bes event header class
//
// Event informations are handled in blocks with fixed order
//
// (0)  general block, handled by own access functions
// (1)  CAL information           => GetContentCAL
// (2)  FLT information           => GetContentFLT
// (3a) Sinistra DA information   => GetContentSinDA
// (3b) Sinistra EL information   => GetContentSinEL
// (3c) Sinistra JB information   => GetContentSinDA
//
//

BesEventHeader::BesEventHeader() : TObject()
{
    //
    // BesEventHeader default constructor
    if ( gDebug ) cout << "BesEventHeader ctor called" << endl;

    fRun   = 0;             // Run number
    fEvent = 0;             // Event number
    fMC    = true;          // MC event
    fRec   = false;         //Long Peixun's update: Reconstruction Event
    fDay   = 20;             // Day of Event
    fMonth = 7;            // Month of Event
    fYear  = 2008;          // Year of Event
    fHour  = 1;             // Hour of Event
    fMin   = 4;             // Minute of Event
    fSec   = 4;             // Second of Event
    fP     = 0.0;           // Total momentum of all Mdc Tracks (in GeV/c)
    fPt    = 0.0;           // Transverse momentum
}

//_____________________________________________________________

BesEventHeader::~BesEventHeader() {
    //
    // BesEventHeader default destructor
    if ( gDebug ) cout << "BesEventHeader dtor called" << endl;

}

//_____________________________________________________________
void BesEventHeader::SetEventTrig(Int_t timeType, 
    vector<Int_t>& trigConditionVector, 
    vector<Int_t>& trigChannelVector ){

   fTimeType = timeType;
   fTrigChannelVector.clear();
   fTrigConditionVector.clear();
   fTrigChannelVector = trigChannelVector;
   fTrigConditionVector = trigConditionVector;
   //************************************
   cout << "BesEventHeader::time Type: " << timeType << endl;
   vector<Int_t>::iterator pTrigVector;
   for (pTrigVector = trigConditionVector.begin(); 
       pTrigVector != trigConditionVector.end(); 
       pTrigVector++){
     cout << "BesEventHeader::trigCondition: " << *pTrigVector << endl;
   }
   for (pTrigVector = trigChannelVector.begin(); 
       pTrigVector != trigChannelVector.end(); 
       pTrigVector++){
     cout << "BesEventHeader::trigChannel: " << *pTrigVector << endl;
   }
   //************************************
}
//________________________________________________________

void BesEventHeader::SetEventHeaderGeneral(Long64_t run, Long64_t event,Int_t time1,Int_t time2) {
    //
    // Set general event header
    fRun = run;
    fEvent = event;


    // modify date
    if (time1 > 0) {
        int a,b;
        a=time1%100;
        fDay=a;

        b=(time1-a)%10000;
        fMonth=b/100;

        fYear=(time1-a-b)/10000;
    }

    //modify time
    if (time2 > 0) {
        int c,d;
        c=time2%100;
        fSec=c;

        d=(time2-c)%10000;
        fMin=d/100;

        fHour=(time2-c-d)/10000;
    }
}

//_____________________________________________________________

void BesEventHeader::SetEventMC(Bool_t mc)
{
    fMC = mc;
}

//_____________________________________________________________
//Long Peixun's update: for Rec flag
void BesEventHeader::SetEventRec(Bool_t rec)
{
    fRec = rec;
}

//_____________________________________________________________

void BesEventHeader::SetEventEvTime(Double_t time, Int_t status, Double_t quality)
{
    fEvTime  = time;
    fEvTimeStatus  = status;
    fEvTimeQuality = quality;
}

//_____________________________________________________________

void BesEventHeader::SetEventMdc(Double_t p, Double_t pt, Double_t px, Double_t py, Double_t pz)
{
    fP  = p;
    fPt = pt;
    fPx = px;
    fPy = py;
    fPz = pz;
}

//_____________________________________________________________

void BesEventHeader::SetEventTof(Double_t t)
{
    fT = t;
}

//_____________________________________________________________

void BesEventHeader::SetEventEmc(Double_t e)
{
    fE = e;
}

//_____________________________________________________________
void BesEventHeader::PrintWord(TString & word,Int_t nbits, Int_t trigger,Int_t trigger2) const {
    Int_t j;
    if (trigger!=0){
        for (j=0;j<nbits;j++){
            if ((trigger>>j)&0x1) word+=Form(" %.2d",j+1);
        }
    }
    if (trigger2!=0){
        for (j=0;j<nbits;j++){
            if ((trigger2>>j)&0x1) word+=Form(" %.2d",j+17);
        }
    }
}
