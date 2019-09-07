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
#ifndef G4FTFParameters_h
#define G4FTFParameters_h 1
//
// $Id: G4FTFParameters.hh,v 1.7 2009/10/25 10:50:54 vuzhinsk Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
#include "G4Proton.hh"
class G4FTFParameters
{

  public:
  	G4FTFParameters(const G4ParticleDefinition * , G4double theA, 
                                                       G4double theZ, 
                                                       G4double s);

	~G4FTFParameters();

// --------- Set geometrical parameteres -----------------------------
        void SethNcmsEnergy(const G4double s);
        void SetTotalCrossSection(const G4double Xtotal);
        void SetElastisCrossSection(const G4double Xelastic);
        void SetInelasticCrossSection(const G4double Xinelastic);
        void SetProbabilityOfElasticScatt(const G4double Xtotal, const G4double Xelastic);
        void SetProbabilityOfElasticScatt(const G4double aValue);

        void SetRadiusOfHNinteractions2(const G4double Radius2);
        void SetSlope(const G4double Slope);
        void SetGamma0(const G4double Gamma0);

        G4double GammaElastic(const G4double impactsquare) 
        {return (FTFGamma0 * std::exp(-FTFSlope * impactsquare));};

// --------- Set parameters of elastic scattering --------------------
        void SetAvaragePt2ofElasticScattering(const G4double aPt2);

// --------- Set parameters of excitations ---------------------------
        void SetMagQuarkExchange(const G4double aValue);
        void SetSlopeQuarkExchange(const G4double aValue);
        void SetDeltaProbAtQuarkExchange(const G4double aValue);

        void SetProjMinDiffMass(const G4double aValue);
        void SetProjMinNonDiffMass(const G4double aValue);
        void SetProbabilityOfProjDiff(const G4double aValue);

        void SetTarMinDiffMass(const G4double aValue); 
        void SetTarMinNonDiffMass(const G4double aValue);
        void SetProbabilityOfTarDiff(const G4double aValue);

        void SetAveragePt2(const G4double aValue);

// --------- Set parameters of a string kink --------------------------------
        void SetPt2Kink(const G4double aValue);
        void SetQuarkProbabilitiesAtGluonSplitUp(const G4double Puubar,
                                                 const G4double Pddbar,
                                                 const G4double Pssbar );

// --------- Set parameters of nuclear destruction--------------------
        void SetCofNuclearDestruction(const G4double aValue);
        void SetR2ofNuclearDestruction(const G4double aValue);

        void SetExcitationEnergyPerWoundedNucleon(const G4double aValue);

        void SetDofNuclearDestruction(const G4double aValue);
        void SetPt2ofNuclearDestruction(const G4double aValue);
        void SetMaxPt2ofNuclearDestruction(const G4double aValue);

//--------------------------------------------------------------------
// --------- Get geometrical parameteres -----------------------------
	G4double GetTotalCrossSection();
	G4double GetElasticCrossSection();
	G4double GetInelasticCrossSection();
        G4double GetProbabilityOfElasticScatt();

        G4double GetSlope();

	G4double GetProbabilityOfInteraction(const G4double impactsquare);
	G4double GetInelasticProbability(const G4double impactsquare);

// --------- Get parameters of elastic scattering --------------------
        G4double GetAvaragePt2ofElasticScattering();

// --------- Get parameters of excitations ---------------------------
        G4double GetMagQuarkExchange();
        G4double GetSlopeQuarkExchange();
        G4double GetDeltaProbAtQuarkExchange();

        G4double GetProjMinDiffMass();
        G4double GetProjMinNonDiffMass();
        G4double GetProbabilityOfProjDiff();

        G4double GetTarMinDiffMass();
        G4double GetTarMinNonDiffMass();
        G4double GetProbabilityOfTarDiff();

        G4double GetAveragePt2();

// --------- Get parameters of a string kink --------------------------------
        G4double GetPt2Kink();
        std::vector<G4double>  GetQuarkProbabilitiesAtGluonSplitUp();

// --------- Get parameters of nuclear destruction---------------------
        G4double GetCofNuclearDestruction();
        G4double GetR2ofNuclearDestruction();

        G4double GetExcitationEnergyPerWoundedNucleon();

        G4double GetDofNuclearDestruction();
        G4double GetPt2ofNuclearDestruction();
        G4double GetMaxPt2ofNuclearDestruction();

//  private: 

	G4FTFParameters();

// ------------ Initial energy of hN interactions --------------------	
        G4double FTFhNcmsEnergy;                // Initial hN CMS energy

// ------------ Geometrical parameteres ------------------------------
        G4double FTFXtotal;                     // Total X in mb
        G4double FTFXelastic;                   // Elastic X in mb
        G4double FTFXinelastic;                 // Inelastic X in mb
        G4double ProbabilityOfElasticScatt;     // Xel/Xtot
        G4double RadiusOfHNinteractions2;       // Xtot/pi, in fn^2
        G4double FTFSlope;                      // in fm^-1
        G4double AvaragePt2ofElasticScattering; // in MeV^2
        G4double FTFGamma0;

// --------- Parameters of excitations -------------------------------
        G4double MagQuarkExchange;
        G4double SlopeQuarkExchange;
        G4double DeltaProbAtQuarkExchange;

        G4double ProjMinDiffMass;
        G4double ProjMinNonDiffMass;
        G4double ProbabilityOfProjDiff;

        G4double TarMinDiffMass;  
        G4double TarMinNonDiffMass;
        G4double ProbabilityOfTarDiff;

        G4double AveragePt2;

// ---------- Parameters of kink -------------------------------------
        G4double Pt2kink;
        std::vector<G4double> QuarkProbabilitiesAtGluonSplitUp;

// --------- Parameters of nuclear destruction------------------------
        G4double CofNuclearDestruction;         // Cnd of nuclear destruction
        G4double R2ofNuclearDestruction;        // R2nd

        G4double ExcitationEnergyPerWoundedNucleon;

        G4double DofNuclearDestruction;         // D for momentum sampling
        G4double Pt2ofNuclearDestruction;       // Pt2
        G4double MaxPt2ofNuclearDestruction;    // Max Pt2

};

// --------------------------------------------------------------------
inline  void G4FTFParameters::SethNcmsEnergy(const G4double s)
             {FTFhNcmsEnergy = s;}

// --------- Set geometrical parameteres ------------------------------
inline  void G4FTFParameters::SetTotalCrossSection(const G4double Xtotal)
             {FTFXtotal = Xtotal;}

inline  void G4FTFParameters::SetElastisCrossSection(const G4double Xelastic)
             {FTFXelastic = Xelastic;}

inline  void G4FTFParameters::SetInelasticCrossSection(const G4double Xinelastic)
             {FTFXinelastic = Xinelastic;}

inline  void G4FTFParameters::SetProbabilityOfElasticScatt(const G4double Xtotal, 
                                                           const G4double Xelastic)
                 { 
                  if(Xtotal==0.) {ProbabilityOfElasticScatt = 0.;}
                  else           {ProbabilityOfElasticScatt = Xelastic/Xtotal;};
                 } 

inline  void G4FTFParameters::SetProbabilityOfElasticScatt(const G4double aValue)
             {ProbabilityOfElasticScatt = aValue;}

inline  void G4FTFParameters::SetRadiusOfHNinteractions2(const G4double Radius2)
             {RadiusOfHNinteractions2 = Radius2;}

inline  void G4FTFParameters::SetSlope(const G4double Slope)
             {FTFSlope = 12.84/Slope;} // Slope is in GeV^-2, FTFSlope in fm^-2

inline  void G4FTFParameters::SetGamma0(const G4double Gamma0)
             {FTFGamma0 = Gamma0;}

// --------- Set parameters of elastic scattering ---------------------
inline  void G4FTFParameters::SetAvaragePt2ofElasticScattering(const G4double aPt2)
                 {
AvaragePt2ofElasticScattering = aPt2;}

// --------- Set parameters of excitations ----------------------------
inline  void G4FTFParameters::SetMagQuarkExchange(const G4double aValue)
             {MagQuarkExchange = aValue;}
inline  void G4FTFParameters::SetSlopeQuarkExchange(const G4double aValue)
             {SlopeQuarkExchange = aValue;}
inline  void G4FTFParameters::SetDeltaProbAtQuarkExchange(const G4double aValue)
             {DeltaProbAtQuarkExchange = aValue;}

inline  void G4FTFParameters::SetProjMinDiffMass(const G4double aValue)
             {ProjMinDiffMass = aValue*GeV;}
inline  void G4FTFParameters::SetProjMinNonDiffMass(const G4double aValue)
             {ProjMinNonDiffMass = aValue*GeV;}
inline  void G4FTFParameters::SetProbabilityOfProjDiff(const G4double aValue)
             {ProbabilityOfProjDiff = aValue;}

inline  void G4FTFParameters::SetTarMinDiffMass(const G4double aValue)
             {TarMinDiffMass = aValue*GeV;}
inline  void G4FTFParameters::SetTarMinNonDiffMass(const G4double aValue)
             {TarMinNonDiffMass = aValue*GeV;}
inline  void G4FTFParameters::SetProbabilityOfTarDiff(const G4double aValue)
             {ProbabilityOfTarDiff = aValue;}

inline  void G4FTFParameters::SetAveragePt2(const G4double aValue)
             {AveragePt2 = aValue*GeV*GeV;}

// --------- Set parameters of a string kink --------------------------------
inline  void G4FTFParameters::SetPt2Kink(const G4double aValue) 
             {Pt2kink = aValue;}

inline  void G4FTFParameters::SetQuarkProbabilitiesAtGluonSplitUp(
                                                 const G4double Puubar,
                                                 const G4double Pddbar,
                                                 const G4double Pssbar )
             {
              QuarkProbabilitiesAtGluonSplitUp.push_back(Puubar); 
              QuarkProbabilitiesAtGluonSplitUp.push_back(Puubar+Pddbar);
              QuarkProbabilitiesAtGluonSplitUp.push_back(Puubar+Pddbar+Pssbar);
             }

// --------- Set parameters of nuclear destruction--------------------
inline  void G4FTFParameters::SetCofNuclearDestruction(const G4double aValue)
             {CofNuclearDestruction = aValue;}
inline  void G4FTFParameters::SetR2ofNuclearDestruction(const G4double aValue)
             {R2ofNuclearDestruction = aValue;}

inline  void G4FTFParameters::SetExcitationEnergyPerWoundedNucleon(const G4double aValue)
             {ExcitationEnergyPerWoundedNucleon = aValue;}

inline  void G4FTFParameters::SetDofNuclearDestruction(const G4double aValue)
             {DofNuclearDestruction = aValue;}
inline  void G4FTFParameters::SetPt2ofNuclearDestruction(const G4double aValue)
             {Pt2ofNuclearDestruction =aValue;}
inline  void G4FTFParameters::SetMaxPt2ofNuclearDestruction(const G4double aValue)
             {MaxPt2ofNuclearDestruction = aValue;}

// --------- Get geometrical parameteres ------------------------------
inline  G4double G4FTFParameters::GetTotalCrossSection()     {return FTFXtotal;}
inline  G4double G4FTFParameters::GetElasticCrossSection()   {return FTFXelastic;}
inline  G4double G4FTFParameters::GetInelasticCrossSection() {return FTFXinelastic;}

inline  G4double G4FTFParameters::GetSlope()                 {return FTFSlope;}

inline  G4double G4FTFParameters::GetProbabilityOfInteraction(const G4double impactsquare)
                 {
                  if(RadiusOfHNinteractions2 > impactsquare) {return 1.;}
                  else                                       {return 0.;}
                 } 

inline  G4double G4FTFParameters::GetProbabilityOfElasticScatt()
                 {return ProbabilityOfElasticScatt;}

inline  G4double G4FTFParameters::GetInelasticProbability( const G4double impactsquare)
        {
         G4double Gamma = GammaElastic(impactsquare);
         return 2 * Gamma - Gamma *Gamma;
        }

// --------- Get parameters of elastic scattering ---------------------
inline  G4double G4FTFParameters::GetAvaragePt2ofElasticScattering()
                 {return AvaragePt2ofElasticScattering;}

// --------- Get parameters of excitations ---------------------------
inline  G4double G4FTFParameters::GetMagQuarkExchange()       {return MagQuarkExchange;}
inline  G4double G4FTFParameters::GetSlopeQuarkExchange()     {return SlopeQuarkExchange;}
inline  G4double G4FTFParameters::GetDeltaProbAtQuarkExchange()
                                                              {return DeltaProbAtQuarkExchange;}


inline  G4double G4FTFParameters::GetProjMinDiffMass()        {return ProjMinDiffMass;}
inline  G4double G4FTFParameters::GetProjMinNonDiffMass()     {return ProjMinNonDiffMass;}
inline  G4double G4FTFParameters::GetProbabilityOfProjDiff()  {return ProbabilityOfProjDiff;} 

inline  G4double G4FTFParameters::GetTarMinDiffMass()         {return TarMinDiffMass;}
inline  G4double G4FTFParameters::GetTarMinNonDiffMass()      {return TarMinNonDiffMass;}
inline  G4double G4FTFParameters::GetProbabilityOfTarDiff()   {return ProbabilityOfTarDiff;}

inline  G4double G4FTFParameters::GetAveragePt2()             {return AveragePt2;}

// --------- Get parameters of a string kink --------------------------
inline  G4double G4FTFParameters::GetPt2Kink()                {return Pt2kink;}
inline  std::vector<G4double> 
                 G4FTFParameters::GetQuarkProbabilitiesAtGluonSplitUp() 
                                  {return QuarkProbabilitiesAtGluonSplitUp;}

// --------- Get parameters of nuclear destruction---------------------
inline  G4double G4FTFParameters::GetCofNuclearDestruction(){return CofNuclearDestruction;}
inline  G4double G4FTFParameters::GetR2ofNuclearDestruction(){return R2ofNuclearDestruction;}

inline  G4double G4FTFParameters::GetExcitationEnergyPerWoundedNucleon()
            {return ExcitationEnergyPerWoundedNucleon;}


inline  G4double G4FTFParameters::GetDofNuclearDestruction()
                 {return DofNuclearDestruction;}
inline  G4double G4FTFParameters::GetPt2ofNuclearDestruction(){return Pt2ofNuclearDestruction;}
inline  G4double G4FTFParameters::GetMaxPt2ofNuclearDestruction()
                 {return MaxPt2ofNuclearDestruction;}
#endif
