#ifndef ReadBoostRoot_H
#define ReadBoostRoot_H 1

#include "G4String.hh"

class ReadBoostRoot
{
	public:
          ReadBoostRoot(G4int,G4int,G4int,G4int,G4int,G4int,G4int,G4int);
	  ~ReadBoostRoot();
	public:
	  static G4String GetBoostRoot() {return BoostRoot;} 
          static G4int GetMdc() {return m_mdc;}
          static G4int GetTof() {return m_tof;}
          static G4int GetEmc() {return m_emc;}
          static G4int GetMuc() {return m_muc;}
          static G4int GetField() {return m_field;}
          static G4int GetFormatAR() {return m_formatAR;}
	  static G4int GetTuning() {return m_tuning;}
          static G4int GetHitOut() {return m_hitOut;}
	private:
		static G4String BoostRoot;
    static G4int m_mdc;
    static G4int m_tof;
    static G4int m_emc;
    static G4int m_muc;
    static G4int m_field;
    static G4int m_formatAR;
    static G4int m_tuning;
    static G4int m_hitOut;
};

#endif
