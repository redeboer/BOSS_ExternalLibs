//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//
//  The shape of the ideal signal is generated from the response function of 
//  the crystal-photodiode-preAmplifier-mainAmplifier combination. The signal
//  from crystal is an exponential decay with the time constant tau_CsI. In 
//  the Laplace domain, it equals 1/(s+a), where a=1/tau_CsI. Assume that the
//  preAmplifier has a step function 1/s. The mainAmplifier has a CR-RC-RC 
//  shape. CR=s/(s+b), RC=1/(s+b), where b=1/tau_s, the shaping time constant.
//  The response time of the photodiode, and the time of deposition of energy
//  in the CsI crystal are presumed to be infinitely short, the response 
//  funtion is then
//  
//            F(s) = [1/(s+a)]*(1/s)*[s/(s+b)^3] = 1/(s+a)(s+b)^3.
//
//  In BESIII EMC, tau_CsI=tau_s=1.0e-6s. So the response fuction is 1/(s+a)^4.
//  In time domain, it is 
//  
//            f(t) = t^3*exp(-t/tau)/6
//            
//Author: Hemiao
//Created: Oct 25, 2004
//Modified:
//  May 23, 2005
//  The mainAmplifier changes to CR-RC-RC-RC, and the response funtion is then
//      F(s) = [1/(s+a)]*(1/s)*[s/(s+b)^4] = 1/(s+a)(s+b)^4.
//  In time domain, it is
//      f(t) = t^4*exp(-t/tau)/24
//Comment:
//---------------------------------------------------------------------------//
// $Id: BesEmcWaveform.hh

#ifndef BesEmcWaveform_h
#define BesEmcWaveform_h

#include "globals.hh"
#include "BesEmcHit.hh"

class BesEmcHit;
class BesEmcDigi;

class BesEmcWaveform{

public:
  
  // Constructors
  BesEmcWaveform();
  BesEmcWaveform(G4long, G4double, G4double);

  // Destructors
  virtual ~BesEmcWaveform();

  // Operators
  inline  G4double &operator[](G4long ) const;
  virtual BesEmcWaveform &operator*=(const G4double &);
  virtual BesEmcWaveform &operator/=(const G4double &);
  virtual BesEmcWaveform &operator+=(const BesEmcWaveform &);
  virtual BesEmcWaveform &operator=(const BesEmcWaveform &);
  
  // Selectors
  inline G4long length() const {  return array_size;}
  inline G4double* GetWave() const { return emcWave; }
  inline G4double GetTau() const { return m_tau; }
  inline G4double GetSampleTime() const { return m_sampleTime; }
  inline G4double GetPeakTime() const { return m_peakTime; }
  inline G4double GetTimeOffset() const { return m_timeOffset; }
  inline G4int GetBitNb() const { return m_bitNb; }
  inline G4int GetGainFlag() const { return m_flag; }
  inline G4double GetPhotonsPerMeV() const { return m_photonsPerMeV; }
  inline G4double GetNonuniformity() const { return m_nonuniformity; }
  G4double max(G4long &binOfMax) const;
    // returns the max of the array
  
  // Modifiers
  void updateWaveform(BesEmcHit *);
  void updateWaveform(BesEmcDigi *);
  void makeWaveform(G4double energy, G4double time);
  void digitize();
  void addElecNoise(G4double, G4double);  //first parameter: incoherent noise width
                                          //second parameter: coherent noise
  void print();

protected:

  G4double *emcWave;
  G4long array_size;

private:

  G4double m_tau;             //tau_CsI=tau_CR=tau_RC=1000.*ns
  G4double m_sampleTime;      //sample time = 50.*ns
  G4double m_peakTime;        //peak time = 4000.*ns
  G4double m_timeOffset;      //the beginning of sample time = 2500.*ns
  G4int m_bitNb;              //10 bit flash ADC
  G4int m_flag;         
    //flag = 0, 1, 2 correspond to three different range of measurement
  
  G4double m_photonsPerMeV;   //photons produced in crystal per MeV
  G4double m_nonuniformity;   //non-uniformity of Light Output
  
  G4double m_highRange;       //2.5GeV
  G4double m_highPrecision;   
  G4double m_midRange;        //0.625GeV
  G4double m_midPrecision;
  G4double m_lowRange;        //0.078GeV
  G4double m_lowPrecision;

};

inline G4double &BesEmcWaveform::operator[](G4long index) const
{
  if(index<0 || index>array_size-1){
    G4cout << "Array bounds exceeded. Index " << index << G4endl;
    ::abort();
  }
  return emcWave[index];
}

#endif
