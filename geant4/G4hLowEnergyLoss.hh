//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: G4hLowEnergyLoss.hh,v 1.17 2006/06/29 19:37:58 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// $Id: 
// ------------------------------------------------------------
//      GEANT 4 class header file 
//
//      History: first implementation, based on object model of
//      2nd December 1995, G.Cosmo
//      ---------- G4hEnergyLoss physics process -----------
//                by Laszlo Urban, 30 May 1997 
//
// ************************************************************             
// It is the first implementation of the NEW UNIFIED ENERGY LOSS PROCESS.
// It calculates the continuous energy loss for charged hadrons.
// Processes giving contribution to the continuous loss :
//   ionisation (= cont.ion.loss + delta ray production)
//   can be added more easily ..........
// This class creates static proton/antiproton dE/dx and range tables ,
// which tables can be used by other processes.
// The energy loss for other charged hadrons is calculated from the p/pbar
// tables with scaled kinetic energy.
//
// 7/10/98  L.Urban some bugs fixed + some cleanup 
// 22/10/98 L.Urban cleanup
// 02/02/99 L.Urban several bugs fixed
// 31/03/00 V.Ivanchenko rename to lowenergy as G4hLowEnergyLoss.hh 
// 09/08/00 V.Ivanchenko remove GetContinuousStepLimit and IsApplicable
// 23/11/01 V.Ivanchenko Move static member-functions from header to source
// 22/01/03 V.Ivanchenko Cuts per region
// 18/04/03 V.Ivanchenko Make dRoverRange protected
//
// Class description:
// Class for Low Energy electromagnetic energy loss of hadrons
// Further documentation available from http://www.ge.infn.it/geant4/lowE

// ****************************************************************************

#ifndef G4hLowEnergyLoss_h
#define G4hLowEnergyLoss_h 1

#include "G4ios.hh"
#include "globals.hh"
#include "Randomize.hh"
#include "G4VContinuousDiscreteProcess.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Proton.hh"
#include "G4AntiProton.hh"
#include "G4Electron.hh"
#include "G4VParticleChange.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4PhysicsLogVector.hh"
#include "G4PhysicsLinearVector.hh"

class G4EnergyLossMessenger;

class G4hLowEnergyLoss : public G4VContinuousDiscreteProcess

{
  public:

    G4hLowEnergyLoss(const G4String& );

    ~G4hLowEnergyLoss();

    virtual G4double GetMeanFreePath(
                                const G4Track& track,
                                      G4double previousStepSize,
                                enum  G4ForceCondition* condition
                                                         ) = 0 ;

    virtual G4VParticleChange* PostStepDoIt(const G4Track& track,
                                            const G4Step& Step) = 0 ;


  protected:

    G4bool CutsWhereModified();

  private:

  // hide  assignment operator

    G4hLowEnergyLoss(G4hLowEnergyLoss &);
    G4hLowEnergyLoss & operator=(const G4hLowEnergyLoss &right);


// =====================================================================

  public:

  private:

    // variables for the integration routines
     static G4double Mass,taulow,tauhigh,ltaulow,ltauhigh;

  protected:
//  G4Material *lastMaterial ;
  const G4double MaxExcitationNumber ;
  const G4double probLimFluct ;
  const long nmaxDirectFluct,nmaxCont1,nmaxCont2 ;

// ====================================================================
//  static part of the class

  public:

    //  get the number of processes contributing to the cont.energy loss
    static G4int GetNumberOfProcesses();

    //  set the number of processes contributing to the cont.energy loss
    static void SetNumberOfProcesses(G4int number);

    //  Increment the number of processes contributing to the cont.energy loss
    static void PlusNumberOfProcesses();

    //  decrement the number of processes contributing to the cont.energy loss
    static void MinusNumberOfProcesses();

    static void SetdRoverRange(G4double value);
    static void SetRndmStep     (G4bool   value);
    static void SetEnlossFluc   (G4bool   value);
    static void SetStepFunction (G4double c1, G4double c2);

  protected:

    static void BuildDEDXTable(const G4ParticleDefinition& aParticleType);

  private:

    static void BuildRangeTable(const G4ParticleDefinition& aParticleType);

    static void BuildInverseRangeTable(
                                const G4ParticleDefinition& aParticleType);

    static void BuildTimeTables(const G4ParticleDefinition& aParticleType);

    static void BuildLabTimeVector(G4int materialIndex,
                          G4PhysicsLogVector* rangeVector);

    static void BuildProperTimeVector(G4int materialIndex,
                          G4PhysicsLogVector* rangeVector);

    static void InvertRangeVector(G4int materialIndex,
                          G4PhysicsLogVector* rangeVector);

    static void BuildRangeVector(G4int materialIndex,
                          G4PhysicsLogVector* rangeVector);

    static G4double LabTimeIntLog(G4PhysicsVector* physicsVector
                                                        ,G4int nbin);

    static G4double ProperTimeIntLog(G4PhysicsVector* physicsVector,
                                                         G4int nbin);

    static G4double RangeIntLin(G4PhysicsVector* physicsVector
                                                        ,G4int nbin);

    static G4double RangeIntLog(G4PhysicsVector* physicsVector
                                                        ,G4int nbin);

    static void BuildRangeCoeffATable(
                          const G4ParticleDefinition& aParticleType);
    static void BuildRangeCoeffBTable(
                          const G4ParticleDefinition& aParticleType);
    static void BuildRangeCoeffCTable(
                          const G4ParticleDefinition& aParticleType);

// ====================================================================

  public:

  protected:

    static G4PhysicsTable* theDEDXpTable ;
    static G4PhysicsTable* theDEDXpbarTable ;
    static G4PhysicsTable* theRangepTable ;
    static G4PhysicsTable* theRangepbarTable ;

    //inverse of the range tables
    static G4PhysicsTable* theInverseRangepTable ;
    static G4PhysicsTable* theInverseRangepbarTable ;

    //lab and proper time tables
    static G4PhysicsTable* theLabTimepTable ;
    static G4PhysicsTable* theLabTimepbarTable ;

    static G4PhysicsTable* theProperTimepTable ;
    static G4PhysicsTable* theProperTimepbarTable ;

    //  processes inherited from G4hLowEnergyLoss
    //   register themselves  in the static array Recorder
    static G4PhysicsTable** RecorderOfpProcess;
    static G4PhysicsTable** RecorderOfpbarProcess;
    static G4int CounterOfpProcess ;
    static G4int CounterOfpbarProcess ;

    // particle mass
    static G4double ParticleMass ;

    // cut in range
    static G4double ptableElectronCutInRange;
    static G4double pbartableElectronCutInRange;

    static G4double Charge ;


    static G4double LowestKineticEnergy;
    static G4double HighestKineticEnergy;
    static G4int TotBin; // number of bins in table,
                         // calculated in BuildPhysicsTable

    static G4double RTable,LOGRTable; // LOGRTable=std::log(HighestKineticEnergy
                                      //          /LowestKineticEnergy)/TotBin
                                      //   RTable = std::exp(LOGRTable)
  private:

    static G4PhysicsTable* theDEDXTable;

    static G4PhysicsTable* theRangeTable;
    static G4PhysicsTable* theInverseRangeTable;

    static G4PhysicsTable* theLabTimeTable;
    static G4PhysicsTable* theProperTimeTable;

    static G4PhysicsTable** RecorderOfProcess;
    static G4int CounterOfProcess;

    static G4PhysicsTable* thepRangeCoeffATable;
    static G4PhysicsTable* thepRangeCoeffBTable;
    static G4PhysicsTable* thepRangeCoeffCTable;
    static G4PhysicsTable* thepbarRangeCoeffATable;
    static G4PhysicsTable* thepbarRangeCoeffBTable;
    static G4PhysicsTable* thepbarRangeCoeffCTable;

    static G4PhysicsTable* theRangeCoeffATable;
    static G4PhysicsTable* theRangeCoeffBTable;
    static G4PhysicsTable* theRangeCoeffCTable;
    static G4int NumberOfProcesses ;

  protected:

    G4PhysicsTable* theLossTable ;

    G4double linLossLimit ;

    G4double MinKineticEnergy ;

    static G4double dRoverRange ; // maximum allowed deltarange/range
                                  //  in one step
    static G4double finalRange ;  // last step before stop
    static G4double c1lim,c2lim,c3lim ; // coeffs for computing steplimit

    static G4bool rndmStepFlag ;
    static G4bool EnlossFlucFlag ;

};

#endif



