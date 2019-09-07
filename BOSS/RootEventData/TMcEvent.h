
#ifndef ROOT_TMcEvent_H
#define ROOT_TMcEvent_H

#include "TObject.h"
#include "TClonesArray.h"
#include "TObjArray.h"

#include "TMdcMc.h"
#include "TEmcMc.h"
#include "TTofMc.h"
#include "TMucMc.h"
#include "TMcParticle.h"
//#include "TDecayMode.h"

class TMcEvent: public TObject {
public:

    TMcEvent();
    virtual ~TMcEvent();

    void initialize();

    void Clear(Option_t *option="");
 
    void Print(Option_t *option="") const;


  //  inline Bool_t getFromMc() { return m_fromMc; };


    const TObjArray* getMdcMcHitCol() const { return m_mdcMcHitCol; }
    void addMdcMc(TMdcMc *mcHit);
    const TMdcMc* getMdcMc(Int_t i) const;
    void clearMdcMcHitCol() { m_mdcMcHitCol->Clear(); }


   const TObjArray* getEmcMcHitCol() const { return m_emcMcHitCol; }
   void addEmcMc(TEmcMc *mcHit);
    const TEmcMc* getEmcMc(Int_t i) const;
    void clearEmcMcHitCol() { m_emcMcHitCol->Clear(); }
  

   const TObjArray* getTofMcHitCol() const { return m_tofMcHitCol; }
   void addTofMc(TTofMc * mcHit);
   const  TTofMc*  getTofMc(Int_t i) const;
   void  clearTofMcHitCol() { m_tofMcHitCol->Clear();}
   
   const TObjArray* getMucMcHitCol() const { return m_mucMcHitCol; }
   void addMucMc(TMucMc * mcHit);
   const  TMucMc*  getMucMc(Int_t i) const;
   void  clearMucMcHitCol() { m_mucMcHitCol->Clear();}

   const TObjArray* getMcParticleCol() const { return m_mcParticleCol; }
   void addMcParticle(TMcParticle * mcHit);
   const  TMcParticle*  getMcParticle(Int_t i) const;
   void  clearMcParticleCol() { m_mcParticleCol->Clear();}
/*
   //DecayMode
   void   addDecayMode(TDecayMode * decayMode);
   const  TDecayMode*  getDecayMode() const;
   void  clearDecayMode() { m_decayMode->Clear();}
*/
private:
 
    /// data members to store Mdc data
    static TObjArray *s_staticMdcMcHitCol;
    TObjArray* m_mdcMcHitCol; //-> 

    /// data members to store Emc data
    static TObjArray *s_staticEmcMcHitCol;
    TObjArray* m_emcMcHitCol; //->

    /// data members to store Tof data
    static TObjArray * s_staticTofMcHitCol;
    TObjArray* m_tofMcHitCol; //->

   /// data members to store Muc data
    static TObjArray * s_staticMucMcHitCol;
    TObjArray* m_mucMcHitCol; //->

   /// data members to store McParticle data
    static TObjArray * s_staticMcParticleCol;
    TObjArray* m_mcParticleCol; //->
/*
   /// data members to store DecayMode data
    static TObject* s_staticDecayMode;
    TObject* m_decayMode;
 */  


    ClassDef(TMcEvent,2) // Storage for McTruth event and subsystem data
}; 
 
#endif





