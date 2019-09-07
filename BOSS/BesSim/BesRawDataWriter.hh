//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
////---------------------------------------------------------------------------//
////Description: 
////Author : Dengzy

////Created:  Mar, 2004
////Modified:
////Comment:
////---------------------------------------------------------------------------//
//// $Id:BesRawDataWriter.hh

#ifndef BesRawDataWriter_h
#define BesRawDataWriter_h 1

#include "G4Event.hh"
#include "BesMdcGeoParameter.hh"
#include "RealizationSvc/RealizationSvc.h"
#include "RealizationSvc/IRealizationSvc.h"

class G4DigiManager;
class G4Svc;
class IDataProviderSvc;

class BesRawDataWriter
{
  public:
    BesRawDataWriter() ;
    ~BesRawDataWriter();

  public:

    void SaveDigits();
    void SaveMdcDigits();
    void SaveTofDigits();
    void SaveEmcDigits();
    void SaveMucDigits();
    
  private:
    
    G4DigiManager* m_DigiMan;
    BesMdcGeoParameter* mdcGeoPointer;
    IDataProviderSvc* m_evtSvc;
    G4Svc* m_G4Svc;
    RealizationSvc* m_RealizationSvc;
};

#endif
