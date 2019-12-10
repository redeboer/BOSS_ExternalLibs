/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: EvtAmpPdf.hh,v 1.1.1.2 2007/10/26 05:03:14 pingrg Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

#ifndef EVT_AMP_PDF_HH
#define EVT_AMP_PDF_HH

#include "EvtGenBase/EvtMacros.hh"
#include "EvtGenBase/EvtAmplitude.hh"
#include "EvtGenBase/EvtPdf.hh"

template <class T>

class EvtAmpPdf : public EvtPdf<T> {  
public:

  EvtAmpPdf() {}
  EvtAmpPdf(const EvtAmplitude<T>& amp) : EvtPdf<T>(), _amp(amp.clone()) {}
  EvtAmpPdf(const EvtAmpPdf<T>& other) : EvtPdf<T>(other), COPY_PTR(_amp) {}
  virtual ~EvtAmpPdf() { delete _amp; }
  
  virtual EvtAmpPdf<T>* clone() const { return new EvtAmpPdf(*this); }
  
  virtual double pdf(const T& p) const
  {
    EvtComplex amp = _amp->evaluate(p);
    return real(amp)*real(amp) + imag(amp)*imag(amp);
  }
  
private:
  
  EvtAmplitude<T>* _amp;
};

#endif

