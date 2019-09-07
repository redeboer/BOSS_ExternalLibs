#include "EvtRecEvent/EvtRecEtaToGG.h"

EvtRecEtaToGG::EvtRecEtaToGG() :
   m_unconMass(0),
   m_chisq(0)
{
   m_hi = 0;
   m_lo = 0;
}

EvtRecEtaToGG::EvtRecEtaToGG(const EvtRecEtaToGG& aEtaToGG) :
   m_unconMass( aEtaToGG.m_unconMass ),
   m_chisq( aEtaToGG.m_chisq ),
   m_hiPfit( aEtaToGG.m_hiPfit ),
   m_loPfit( aEtaToGG.m_loPfit ),
   m_hi( aEtaToGG.m_hi ),
   m_lo( aEtaToGG.m_lo )
{
}

EvtRecEtaToGG& EvtRecEtaToGG::operator=(const EvtRecEtaToGG& aEtaToGG) {

   m_unconMass = aEtaToGG.m_unconMass;
   m_chisq   = aEtaToGG.m_chisq;
   m_hiPfit  = aEtaToGG.m_hiPfit;
   m_loPfit  = aEtaToGG.m_loPfit;
   m_hi      = aEtaToGG.m_hi;
   m_lo      = aEtaToGG.m_lo;

   return *this;
}
