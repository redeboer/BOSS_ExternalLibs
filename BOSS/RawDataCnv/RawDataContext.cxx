//====================================================================
//	RawDataContext.cxx
//--------------------------------------------------------------------
//
//	Package    : RawDataSelector
//
//  Description: The EventSelector component is able
//               to produce a list of event references given 
//               a set of "selection criteria".
//
//
//====================================================================

// Include files
#include "RawDataCnv/RawDataSelector.h"
#include "RawDataCnv/RawDataContext.h"

//________________________________________________________________________________
RawDataContext::RawDataContext(const RawDataSelector* selector) :
                m_evtSelector(selector) {
}
//________________________________________________________________________________
RawDataContext::RawDataContext(const RawDataContext& ctxt) :
                m_evtSelector(ctxt.m_evtSelector) {
}
//________________________________________________________________________________
RawDataContext::~RawDataContext() {
}
//________________________________________________________________________________
void* RawDataContext::identifier() const {
   return((void*)(m_evtSelector));
}

