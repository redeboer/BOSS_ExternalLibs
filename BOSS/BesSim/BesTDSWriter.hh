//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
////---------------------------------------------------------------------------//
////Description: 
////Author : Dengzy

////Created:  Mar, 2004
////Modified:
////Comment:
////---------------------------------------------------------------------------//
//// $Id:BesTDSWriter.hh

#ifndef BesTDSWriter_h
#define BesTDSWriter_h 1

class G4Event;
class BesRawDataWriter;
class BesMcTruthWriter;


class BesTDSWriter
{
  public:
    BesTDSWriter() ;
    ~BesTDSWriter();

  public:

    //save event head, McTruth, and raw data
    void SaveAll(const G4Event*, int);
    
  private:

    BesRawDataWriter* m_rawWriter;
    BesMcTruthWriter* m_mcWriter;
    
};

#endif
