/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: EvtAmplitude.hh,v 1.1.1.2 2007/10/26 05:03:14 pingrg Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

// Complex-valued amplitude

#ifndef EVT_AMPLITUDE_HH
#define EVT_AMPLITUDE_HH

#include "EvtGenBase/EvtComplex.hh"

template <class T>
class EvtAmplitude {
public:

  EvtAmplitude() {}
  EvtAmplitude(const EvtAmplitude&) {}
  virtual ~EvtAmplitude() {}
  virtual EvtAmplitude<T>* clone() const = 0;

  EvtComplex evaluate(const T& p) const
  {
    EvtComplex ret(0.,0.);
    if(p.isValid()) ret = amplitude(p);
    return ret;
  }

protected:

  // Derive in subclasses to define amplitude computation
  // for a fully constructed amplitude object. 
  
  virtual EvtComplex amplitude(const T&) const = 0;

}; 

#endif





