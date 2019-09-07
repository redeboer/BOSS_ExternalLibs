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
// $Id: EmcWaveform.h

#ifndef EMC_WAVEFORM_H
#define EMC_WAVEFORM_H

#include <iostream>
#include <stdlib.h>
using namespace std;

class EmcWaveform{

public:
  
  // Constructors
  EmcWaveform();
  EmcWaveform(int, double, double);

  // Destructors
  ~EmcWaveform();

  // Operators
  inline  double &operator[](int ) const;
  EmcWaveform &operator*=(const double);
  EmcWaveform &operator/=(const double);
  EmcWaveform &operator+=(const EmcWaveform &);
  EmcWaveform &operator=(const EmcWaveform &);
  
  // Selectors
  inline int length() const {  return array_size;}
  inline double* GetWave() const { return emcWave; }
  inline double GetTau() const { return m_tau; }
  inline double GetTauTrg() const { return m_tauTrg; }
  inline double GetSampleTime() const { return m_sampleTime; }
  inline double GetSampleTimeTrg() const { return m_sampleTimeTrg; }
  inline double GetPeakTime() const { return m_peakTime; }
  inline double GetPeakTimeTrg() const { return m_peakTimeTrg; }
  inline double GetTimeOffset() const { return m_timeOffset; }
  inline double GetTimeOffsetTrg() const { return m_timeOffsetTrg; }
  
  double max(int &binOfMax) const; // returns the max of the array
  double getADCTrg(int time); // return ADC in a certain time
  int frontEdgeTrg(double thres); // returns the time over the threshold
  
  // Modifiers
  void makeWaveform(double energy, double time);
  void makeWaveformTrg(double energy, double time);
  
  void print();

protected:

  double *emcWave;
  int array_size;

private:

  double m_tau;             //tau_CsI=tau_CR=tau_RC=1000.*ns
  double m_tauTrg;             //tau_CsI=tau_CR=tau_RC=1000.*ns
  double m_sampleTime;      //sample time = 50.*ns
  double m_sampleTimeTrg;      //sample time = 50.*ns
  double m_peakTime;        //peak time = 4000.*ns
  double m_peakTimeTrg;        //peak time = 4000.*ns
  double m_timeOffset;      //the beginning of sample time = 2500.*ns
  double m_timeOffsetTrg;      //the beginning of sample time = 2500.*ns
  
};

inline double &EmcWaveform::operator[](int index) const
{
  if(index<0 || index>array_size-1){
    cout << "Array bounds exceeded. Index " << index << endl;
    ::abort();
  }
  return emcWave[index];
}

#endif
