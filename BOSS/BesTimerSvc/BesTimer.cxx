

// this is -*- c++ -*-
// $Id: BesTimer.cxx,v 1.4 2011/02/18 04:23:58 maqm Exp $
// $Name: BesTimerSvc-00-00-12 $
#include <sys/time.h>
#include   <complex>   

#include "BesTimerSvc/BesTimer.h"

#define SecsInDay 86400

    BesTimer::BesTimer (const std::string name)
      : m_name (name) {
      m_numberOfMeasurements = 0;
      m_mean = 0.0;
      m_ms = 0.0;
      m_elapsed = 0.0;
      m_started=false;
      m_paused=false;
      m_propVal = 0;
      m_meanVal = 0;
      m_meanTimePerObject = 0.0;
      m_NmeanTimePerObject = 0;
    }
    
    void BesTimer::start( void ) {
      struct timeval tv;

      m_started = true;
      m_paused = false;
      m_elapsed = 0.0;
      m_propVal = 0;
      // if already started, the start time will be over-written
      gettimeofday(&tv, 0);
      m_startTime = tv;
    }
    
    void BesTimer::stop ( void ) {
      struct timeval tv;
      
      if (!m_started) return;

      gettimeofday(&tv, 0);
      m_started = false;
      
      if (!m_paused) {
	float mtime;
	int secs, usecs;
	
	if (tv.tv_sec >= m_startTime.tv_sec) {
	  secs = tv.tv_sec - m_startTime.tv_sec;
	} else {
	  secs = tv.tv_sec - m_startTime.tv_sec + SecsInDay;
	}
	usecs = tv.tv_usec - m_startTime.tv_usec;
	mtime = static_cast<float>(secs)*1000. + static_cast<float>(usecs)/1000.;

	// elapsed time
	m_elapsed += mtime;
	
      }

      // statistics - mean, rms
      double denom = static_cast <double> (++m_numberOfMeasurements);
      double d = m_elapsed - m_mean;
      m_mean += d / denom;
      double dd = m_elapsed*m_elapsed - m_ms;
      m_ms += dd / denom;

      // mean property 
      m_meanVal += static_cast <double> (m_propVal - m_meanVal) / denom;
      if (m_propVal !=0) {
	double timePerObject = m_elapsed/static_cast <double>(m_propVal);
	m_meanTimePerObject += static_cast <double> (timePerObject - m_meanTimePerObject) / 
	  static_cast<double> (++m_NmeanTimePerObject);
      }
    }
    
    void BesTimer::pause ( void ) {
      struct timeval tv;

      // only pause if started
      // a pause while paused is ignored 
      if (m_paused || !m_started) return;
      m_paused = true;
      gettimeofday(&tv, 0);

      float mtime;
      int secs, usecs;
      
      if (tv.tv_sec >= m_startTime.tv_sec) {
	secs = tv.tv_sec - m_startTime.tv_sec;
      } else {
	secs = tv.tv_sec - m_startTime.tv_sec + SecsInDay;
      }
      usecs = tv.tv_usec - m_startTime.tv_usec;
      mtime = static_cast<float>(secs)*1000. + static_cast<float>(usecs)/1000.;
      m_elapsed += mtime; // time so far
    }

    void BesTimer::resume( void ) {
      struct timeval tv;

      if (!m_started) { 
	start(); // resume acts as start if not started
      } else if (m_paused) {
	m_paused = false;
	gettimeofday(&tv, 0);
	m_startTime = tv;
      }
    }

    void BesTimer::reset () { 
      if (!m_started) {
	m_elapsed = 0.0; 
	m_propVal = 0;
      }
    }
