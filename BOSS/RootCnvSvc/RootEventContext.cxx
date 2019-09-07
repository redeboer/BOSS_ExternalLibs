#include "RootCnvSvc/RootEventContext.h"

//______________________________________________________
RootEventContext::RootEventContext(const RootEvtSelector* 
selector) :
        m_evtSelector(selector) {
}

//______________________________________________________
RootEventContext::RootEventContext(const
RootEventContext& ctxt) :
        m_evtSelector(ctxt.m_evtSelector) {
}

//______________________________________________________
RootEventContext::~RootEventContext() {
}

//______________________________________________________
void* RootEventContext::identifier() const {
 return((void*)(m_evtSelector));
}

