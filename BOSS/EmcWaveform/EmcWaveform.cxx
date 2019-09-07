//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Hemiao
//Created: Oct 25, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
// $Id: EmcWaveform.cxx

#include "EmcWaveform/EmcWaveform.h"
#include <math.h>

// Constructors

EmcWaveform::EmcWaveform()
{
  array_size = 60;
  m_tau = 1000.;
  m_sampleTime = 50.;
  m_peakTime = 4000.;
  m_timeOffset = 2500.;
  emcWave=new double[array_size];

  for (int i=0; i<array_size;i++)
    emcWave[i]=0;
}

EmcWaveform::EmcWaveform(int size, double tau, double sampleTime)
  :m_tau(tau),m_sampleTime(sampleTime)
{
  if(size>0){
    array_size=size;
    emcWave=new double[array_size];
    double *init=emcWave+array_size;
    while(init!=emcWave) *--init=0.0;
  }
}

// Destructors
EmcWaveform::~EmcWaveform(){
  delete [] emcWave;
  emcWave=0;
}

// Operators
EmcWaveform &EmcWaveform::operator*=(const double scale)
{
  for (int i=0; i<array_size;i++) emcWave[i]*=scale;
  return *this;
}

EmcWaveform &EmcWaveform::operator/=(const double scale)
{
  for (int i=0; i<array_size;i++) emcWave[i]/=scale;
  return *this;
}

EmcWaveform &EmcWaveform::operator+=(const EmcWaveform &assign)
{
  for (int i=0; i<array_size;i++) emcWave[i]+=assign[i];
  return *this;
}
  
EmcWaveform &EmcWaveform::operator=(const EmcWaveform &assign)
{
  if (this != &assign) {
    if (emcWave!=0) delete [] emcWave;
    emcWave=new double[assign.array_size];
    array_size=assign.array_size;
    for (int i=0;i<array_size;i++) emcWave[i]=assign[i];
  }
  return *this;
}

double EmcWaveform::max(int &binOfMax) const
{
  double maxi=emcWave[0];
  binOfMax = 0;
  for (int i=1;i<array_size;i++)
  {
    if (emcWave[i]>maxi) 
    {
      maxi=emcWave[i];
      binOfMax = i;
    }
  }
  return maxi;
}

double EmcWaveform::getADCTrg(int time)
{
  double adc = emcWave[0];
  for(int i = 0; i < array_size; i++) {
    if(time >= i*m_sampleTime && time < (i+1)*m_sampleTime) adc = emcWave[i];
  } 
  return adc;
}   
    
int EmcWaveform::frontEdgeTrg(double thres)
{
  int binOfMax = -1;
  for (int i=0;i<array_size;i++)
  {
    if (emcWave[i]>thres)
    {
      binOfMax = i;
      break;
    }
  }
  return binOfMax;
}

void EmcWaveform::makeWaveformTrg(double energy, double time)
{ 
  double time0 = time*m_sampleTime+m_timeOffset-m_peakTime; //start time
  double tempTime = 0;  //time for each bin
  
  double peak = m_peakTime*m_peakTime*m_peakTime*m_peakTime*exp(-m_peakTime/m_tau)/24;
  
  for (int i=0;i<array_size;i++)
  { 
    tempTime = i*m_sampleTime + m_timeOffset - time0;
    if(tempTime>0)
      emcWave[i] +=
        energy*tempTime*tempTime*tempTime*tempTime*exp(-tempTime/m_tau)/(24*peak);
  }
}

void EmcWaveform::makeWaveform(double energy, double time)
{
  double time0 = time*m_sampleTime+m_timeOffset-m_peakTime; //start time
  double tempTime = 0;  //time for each bin

  double peak = m_peakTime*m_peakTime*m_peakTime*m_peakTime*exp(-m_peakTime/m_tau)/24;

  for (int i=0;i<array_size;i++)
  {
    tempTime = i*m_sampleTime + m_timeOffset - time0;
    if(tempTime>0)
      emcWave[i] += 
        energy*tempTime*tempTime*tempTime*tempTime*exp(-tempTime/m_tau)/(24*peak);
  }
}

void EmcWaveform::print()
{
  cout<<"New Wave!"<<endl;
  for(int i=0;i<array_size;i++)
    cout<<emcWave[i]<<"\t";
  cout<<endl;
}
