//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Dengzy
//Created: Mar, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
// $Id: ScintSingle.hh

#ifndef ScintSingle_h
#define ScintSingle_h 1

#include <vector>
using namespace std;

class ScintSingle
{
  public:
 
    ScintSingle()
    {
      hitIndexes = new vector<G4int>;
    }
    ScintSingle(const ScintSingle& right)
    {
      partId = right.partId;
      scinNb = right.scinNb;
      hitIndexes =right.hitIndexes;
    }
    const ScintSingle& operator=(const ScintSingle& right)
    {
      partId = right.partId;
      scinNb = right.scinNb;
      hitIndexes =right.hitIndexes;
      return *this;
    }
    ~ScintSingle() 
    {
      delete hitIndexes;
    } 
  
    G4int GetPartId() {return partId;}  
    G4int GetScinNb() {return scinNb;}
    G4double GetEdep() {return edep;}
    vector<G4int>* GetHitIndexes() {return hitIndexes;}
    vector<G4int>* GetHitIndexes_mrpc() {return hitIndexes;}
    G4int GetModule_mrpc() {return scinNb;}
  
    void SetPartId(G4int id) {partId = id;}
    void SetScinNb(G4int nb) {scinNb = nb;}
    void SetEdep(G4double e) {edep = e;}
    void AddEdep(G4double e) {edep += e;}
  private:

    G4int partId;
    G4int scinNb;
    G4double edep;
    vector<G4int>* hitIndexes;
};

#endif

