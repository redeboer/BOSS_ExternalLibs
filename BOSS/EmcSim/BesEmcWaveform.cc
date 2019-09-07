//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Hemiao
//Created: Oct 25, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
// $Id: BesEmcWaveform.cc

#include "BesEmcWaveform.hh"
#include "Randomize.hh"
#include "CLHEP/Random/RanecuEngine.h"
#include "CLHEP/Random/RandGauss.h"
#include "BesEmcParameter.hh"
#include "BesEmcDigi.hh"
using namespace CLHEP;
// Constructors

BesEmcWaveform::BesEmcWaveform()
{
  BesEmcParameter& emcPara=BesEmcParameter::GetInstance();
  //BesEmcParameter emcPara;
  //emcPara.ReadData();
  array_size = emcPara.GetArraySize();
  m_tau = emcPara.GetTau();
  m_highRange = emcPara.GetHighRange();
  m_midRange = emcPara.GetMidRange();
  m_lowRange = emcPara.GetLowRange();
  m_sampleTime = emcPara.GetSampleTime();
  m_peakTime = emcPara.GetPeakTime();
  m_timeOffset = emcPara.GetTimeOffset();
  m_bitNb = emcPara.GetADCbit();
  m_photonsPerMeV = emcPara.GetPhotonsPerMeV();
  m_nonuniformity = emcPara.GetNonuniformity();
  m_flag = -1;
  m_highPrecision = m_highRange/((G4double)(1<<m_bitNb));
  m_midPrecision = m_midRange/((G4double)(1<<m_bitNb));
  m_lowPrecision = m_lowRange/((G4double)(1<<m_bitNb));
  emcWave=new G4double[array_size];

  for (G4long i=0; i<array_size;i++)
    emcWave[i]=0;
}

BesEmcWaveform::BesEmcWaveform(G4long size, G4double tau, G4double sampleTime)
  :m_tau(tau),m_sampleTime(sampleTime)
{
  if(size>0){
    array_size=size;
    emcWave=new G4double[array_size];
    G4double *init=emcWave+array_size;
    while(init!=emcWave) *--init=0.0;
  }
  else{
    G4Exception("Invalid size");
  }
}

// Destructors
BesEmcWaveform::~BesEmcWaveform(){
  delete []emcWave;
  emcWave=0;
}

// Operators
BesEmcWaveform &BesEmcWaveform::operator*=(const G4double &scale)
{
  for (G4long i=0; i<array_size;i++) emcWave[i]*=scale;
  return *this;
}

BesEmcWaveform &BesEmcWaveform::operator/=(const G4double &scale)
{
  for (G4long i=0; i<array_size;i++) emcWave[i]/=scale;
  return *this;
}

BesEmcWaveform &BesEmcWaveform::operator+=(const BesEmcWaveform &assign)
{
  for (G4long i=0; i<array_size;i++) emcWave[i]+=assign[i];
  return *this;
}
  
BesEmcWaveform &BesEmcWaveform::operator=(const BesEmcWaveform &assign)
{
  if (this != &assign) {
    if (emcWave!=0) delete []emcWave;
    emcWave=new G4double[assign.array_size];
    array_size=assign.array_size;
    for (G4long i=0;i<array_size;i++) emcWave[i]=assign[i];
  }
  return *this;
}

G4double BesEmcWaveform::max(G4long &binOfMax) const
{
  G4double maxi=emcWave[0];
  binOfMax = 0;
  for (G4long i=1;i<array_size;i++)
  {
    if (emcWave[i]>maxi) 
    {
      maxi=emcWave[i];
      binOfMax = i;
    }
  }
  return maxi;
}

void BesEmcWaveform::updateWaveform(BesEmcHit* hit)
{
  G4double energy = hit->GetEdepCrystal();
  G4double time = hit->GetTimeCrystal();

  makeWaveform(energy, time);
}

void BesEmcWaveform::updateWaveform(BesEmcDigi* digi)
{
  G4double energy = digi->GetEnergy();
  G4double time = digi->GetTime()*m_sampleTime+m_timeOffset-m_peakTime;

  makeWaveform(energy, time);
}

void BesEmcWaveform::makeWaveform(G4double energy, G4double time)
{
  G4double amplitude;
  if(m_photonsPerMeV==0)
  {
    amplitude = energy;
  }
  else
  {
    G4double photons = energy*m_photonsPerMeV;
    if(photons>0) {
      G4double photonStatFactor = RandGauss::shoot(photons, sqrt(photons))/photons;
      amplitude = energy*photonStatFactor;
    } else {
      amplitude = 0;
    }
  }

  G4double tempTime;
  tempTime = 0;

  G4double peak = m_peakTime*m_peakTime*m_peakTime*m_peakTime*exp(-m_peakTime/m_tau)/24;

  for (G4long i=0;i<array_size;i++)
  {
    tempTime = i*m_sampleTime + m_timeOffset - time;
    if(tempTime>0)
      emcWave[i] += amplitude*tempTime*tempTime*tempTime*tempTime*exp(-tempTime/m_tau)/(24*peak);
  }
}

void BesEmcWaveform::digitize()
{
  G4double oneBitResolution;
  oneBitResolution = m_midRange*2/((G4double)(1<<m_bitNb));   //not used now
  G4double energy;
  
  for(G4long i=0;i<array_size;i++)
  {
    energy = emcWave[i];
    if(energy > m_highRange)
      G4cout<<"---In BesEmcWaveform: Over measurement!--- energy="<<energy<<G4endl;
    else if(energy > m_midRange)
    {
      m_flag = 2;
      emcWave[i] = (G4double)((G4long)(emcWave[i]/m_highPrecision))*m_highPrecision; 
    }
    else if(energy > m_lowRange)
    {
      m_flag = 1;
      emcWave[i] = (G4double)((G4long)(emcWave[i]/m_midPrecision))*m_midPrecision;
    }
    else
    {
      m_flag = 0;
      emcWave[i] = (G4double)((G4long)(emcWave[i]/m_lowPrecision))*m_lowPrecision;
    }
  }
}  

void BesEmcWaveform::print()
{
  G4cout<<"New Wave!"<<G4endl;
  for(G4long i=0;i<array_size;i++)
    G4cout<<emcWave[i]<<"\t";
  G4cout<<G4endl;
}

void BesEmcWaveform::addElecNoise(G4double width, G4double coherentNoise)
{
  for(G4int i=0;i<array_size;i++)
  {
    emcWave[i] += RandGauss::shoot()*width;
    emcWave[i] += coherentNoise;
    emcWave[i] = emcWave[i]>0?emcWave[i]:0;
  }
}
