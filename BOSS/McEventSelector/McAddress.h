//====================================================================
//	McAddress.h
//--------------------------------------------------------------------
//
//	Package    : McEventSelector 
//
//  Description: Definition of Mc address object
//
//	Author     : P. Calafiura
//====================================================================
#ifndef MCEVENTSELECTOR_MCADDRESS_H
#define MCEVENTSELECTOR_MCADDRESS_H

// C/C++ include files
#include <string>

// Framework include files
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/GenericAddress.h"


class McAddress   : public GenericAddress   {
public:
  /// Standard Destructor
  virtual ~McAddress() {}; //doesn't own event

  /// Standard Constructor
  McAddress(const CLID& clid, const std::string& fname,
		  const std::string& cname);

  McAddress(const CLID& clid, const std::string& fname,
	    const int& run, const int& event, const std::string& cname);

  ///Event accessors
  inline void setRunEvt(int run, unsigned int evt) {
    m_runNo = run;
    m_eventNo = evt;
  }

  int runNumber() const { return m_runNo; }
  unsigned int eventNumber() const { return m_eventNo; }

private:
  int m_runNo;
  unsigned int m_eventNo;
};
#endif // MCEVENTSELECTOR_MCADDRESS_H

