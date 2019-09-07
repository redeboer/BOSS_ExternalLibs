// this is -*- c++ -*-
// $Id: BesTimer.h,v 1.3 2011/02/18 04:23:58 maqm Exp $
// $Name: BesTimerSvc-00-00-12 $
#ifndef BesTIMER_H_
#define BesTIMER_H_
#include <sys/time.h>
#include <iostream>
#include <string>

#include   <complex>


  class BesTimer {
  public:
    // constructor
    BesTimer (const std::string name);
    
  public:
    
    // retrieve timer name
    std::string& name() { return m_name; }
    // retrieve elapsed time
    float elapsed( void ) const { return (m_started? 0.0 : m_elapsed);}
    // retrieve mean and rms time
    double mean( void ) const { return m_mean;}
    double rms( void ) const { 
      double var = m_ms - m_mean*m_mean;
      return (var > 0 ? sqrt(var) : 0.0);
    }
    // retrieve number of measurements 
    int number_of_measurements( void ) const { return m_numberOfMeasurements;}

    // set and retrieve property name
    void propName(std::string name) { m_propName = name; }
    std::string& propName() { return m_propName; }
    // set and retrieve property value
    inline unsigned int propVal() { return m_propVal;}
    inline void propVal(unsigned int val) { m_propVal = val; } 

    double meanPropVal() { return m_meanVal;}
    double meanTimePerObject() { return m_meanTimePerObject;}

    // define less than operator based on name
    bool operator < ( BesTimer &it) { return (this->name() < it.name());}

    // methods to control clock
    void start( void );    
    void stop ( void ); 
    void pause ( void );
    void resume( void );
    void reset ();


  private:
    std::string m_name;
    struct timeval m_startTime;
    float m_elapsed;
    double m_mean; // mean time
    double m_ms;   // mean squared time
    int m_numberOfMeasurements;
    bool m_started;
    bool m_paused;
    std::string m_propName;
    unsigned int m_propVal; 
    double m_meanVal;
    double m_meanTimePerObject;
    int  m_NmeanTimePerObject;

  };

#endif // BesTIMER_H
