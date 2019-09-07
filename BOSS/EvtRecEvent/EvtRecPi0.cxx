#include "EvtRecEvent/EvtRecPi0.h"

EvtRecPi0::EvtRecPi0() :
   m_unconMass(0),
   m_chisq(0)
{
   m_hi = 0;
   m_lo = 0;
}

EvtRecPi0::EvtRecPi0(const EvtRecPi0& aPi0) :
   m_unconMass( aPi0.m_unconMass ),
   m_chisq( aPi0.m_chisq ),
   m_hiPfit( aPi0.m_hiPfit ),
   m_loPfit( aPi0.m_loPfit ),
   m_hi( aPi0.m_hi ),
   m_lo( aPi0.m_lo )
{
}

EvtRecPi0& EvtRecPi0::operator=(const EvtRecPi0& aPi0) {

   m_unconMass = aPi0.m_unconMass;
   m_chisq   = aPi0.m_chisq;
   m_hiPfit  = aPi0.m_hiPfit;
   m_loPfit  = aPi0.m_loPfit;
   m_hi      = aPi0.m_hi;
   m_lo      = aPi0.m_lo;

   return *this;
}
