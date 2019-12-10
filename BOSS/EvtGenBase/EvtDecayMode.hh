//-----------------------------------------------------------------------
// File and Version Information: 
//      $Id: EvtDecayMode.hh,v 1.1.1.2 2007/10/26 05:03:14 pingrg Exp $
// 
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations. If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information:
//      Copyright (C) 1998 Caltech, UCSB
//
// Module creator:
//      Alexei Dvoretskii, Caltech, 2001-2002.
//-----------------------------------------------------------------------

#ifndef EVT_DECAY_MODE_HH
#define EVT_DECAY_MODE_HH

#include <string>
#include <vector>
#include "EvtGenBase/EvtCyclic3.hh"

#include <iosfwd>

class EvtDecayMode {

public:

  EvtDecayMode(const char* decay);
  EvtDecayMode(const EvtDecayMode& other);
  EvtDecayMode(std::string mother,std::vector<std::string> dau);
  ~EvtDecayMode();

  const char* mother() const;
  int nD() const;
  const char* dau(int i) const; 

  std::ostream& print(std::ostream&) const;


  // Frequent name combinations

  const char* m(EvtCyclic3::Pair i) const;
  const char* q(EvtCyclic3::Pair i) const;
  const char* dal(EvtCyclic3::Pair i, EvtCyclic3::Pair j) const;
  const char* mode() const;

private:

  std::string _mother;
  std::vector<std::string> _dau;

};


std::ostream& operator<<(std::ostream&,const EvtDecayMode&);

#endif
