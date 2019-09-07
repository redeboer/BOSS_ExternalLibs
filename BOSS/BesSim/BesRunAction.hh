//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: class for BesRunAction (simple)
//Author: Liuhm
//Created: May 25, 2003
//Modified:
//Comment:
//---------------------------------------------------------------------------//

#ifndef BesRunAction_h
#define BesRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <vector>
class G4Run;
class BesRunActionMessenger;
class BesAsciiIO;
class BesRootIO;
class BesTDSWriter;
class BesTuningIO;

class BesRunAction : public G4UserRunAction
{
  public:
    BesRunAction();
   ~BesRunAction();

  public:
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);

    G4int GetRunId() {return m_runId;}
    G4int GetMCTruthFlag() {return m_MCTruthFlag;}
    BesAsciiIO* GetAsciiIO() {return m_asciiIO;}
    BesRootIO* GetRootIO() {return m_rootIO;}
    BesTuningIO* GetTuningIO() {return m_tuningIO;}
   
    G4bool GetTDSFlag() {return m_TDSFlag;}
    BesTDSWriter* GetTDSWriter() {return m_TDSWriter;}
    
    void SetMCTruthFlag(G4int flag) {m_MCTruthFlag = flag;}
    
    /*void SetMdcTruFlag(G4int flag)  {m_mdcTruFlag = flag;}
    void SetTofTruFlag(G4int flag)  {m_tofTruFlag = flag;}
    void SetEmcTruFlag(G4int flag)  {m_emcTruFlag = flag;}
    void SetMucTruFlag(G4int flag)  {m_mucTruFlag = flag;}
    
    void SetMdcDigiFlag(G4int flag) {m_mdcDigiFlag = flag;}
    void SetTofDigiFlag(G4int flag) {m_tofDigiFlag = flag;}
    void SetEmcDigiFlag(G4int flag) {m_emcDigiFlag = flag;}
    void SetMucDigiFlag(G4int flag) {m_mucDigiFlag = flag;}
    */
    void SetAsciiFlag( std::string flag) {m_asciiFlag = flag;}
    void SetAsciiFile( std::string file) {m_asciiFile = file;}
    void SetRootFlag( G4bool flag)  {m_rootFlag = flag;}
    void SetRootFile( std::string file) {m_rootFile = file;}
    void SetTuningFile(std::vector<std::string> file) {m_tuningFile = file;}

    void SetTDSFlag(G4bool flag) {m_TDSFlag = flag;}
    
  private:
    G4int m_runId;
    G4int m_MCTruthFlag;
    
    /*G4int m_mdcTruFlag;
    G4int m_tofTruFlag;
    G4int m_emcTruFlag;
    G4int m_mucTruFlag;
    
    G4int m_mdcDigiFlag;
    G4int m_tofDigiFlag;
    G4int m_emcDigiFlag;
    G4int m_mucDigiFlag;
    */
    std::string m_asciiFlag;
    std::string m_asciiFile;
    G4bool m_rootFlag;
    std::string m_rootFile;
    std::vector<std::string> m_tuningFile;
    
    BesAsciiIO* m_asciiIO;
    BesRootIO* m_rootIO;
    BesTuningIO* m_tuningIO;
    
    G4bool m_TDSFlag;
    BesTDSWriter* m_TDSWriter;
    
    BesRunActionMessenger* m_runMessenger;
};

#endif

