#include "Identifier/BesDetectorID.h"
#include <iostream>
#include <stdio.h>
#include <assert.h>

BesDetectorID::BesDetectorID(void) :
  m_MdcId(BesDetectorID::MDC_ID),     
  m_TofId(BesDetectorID::TOF_ID),      	
  m_EmcId(BesDetectorID::EMC_ID), 	
  m_MucId(BesDetectorID::MUC_ID),
  m_HltId(BesDetectorID::HLT_ID),
  m_MrpcId(BesDetectorID::MRPC_ID)
{
}

BesDetectorID::~BesDetectorID(void)
{
}

bool BesDetectorID::is_mdc (const Identifier& id) const {
  Identifier::value_type value = id.get_value(); 
  return ((value  &  MDC_MASK) >> MDC_INDEX) == MDC_ID ? true : false;    
}

bool BesDetectorID::is_tof (const Identifier& id) const {
  Identifier::value_type value = id.get_value(); 
  return ((value  &  TOF_MASK) >> TOF_INDEX) == TOF_ID ? true : false;
}

bool BesDetectorID::is_emc (const Identifier& id) const {
  Identifier::value_type value = id.get_value(); 
  return ((value  &  EMC_MASK) >> EMC_INDEX) == EMC_ID ? true : false;   
}
 
bool BesDetectorID::is_muc (const Identifier& id) const {
  Identifier::value_type value = id.get_value(); 
  return ((value  &  MUC_MASK) >> MUC_INDEX) == MUC_ID ? true : false;
}

bool BesDetectorID::is_hlt (const Identifier& id) const {
  Identifier::value_type value = id.get_value();
  return ((value  &  HLT_MASK) >> HLT_INDEX) == HLT_ID ? true : false;
}


bool BesDetectorID::is_mrpc (const Identifier& id) const {
  Identifier::value_type value = id.get_value();
  return ((value  &  MRPC_MASK) >> MRPC_INDEX) == MRPC_ID ? true : false;
}



Identifier BesDetectorID::mdc(void) const {
  Identifier id = Identifier(  m_MdcId << MDC_INDEX );
  return id; 
}

Identifier BesDetectorID::tof(void) const{
  Identifier id = Identifier(  m_TofId << TOF_INDEX );
  return id; 
}
  
Identifier BesDetectorID::emc(void) const {
  Identifier id = Identifier(  m_EmcId << EMC_INDEX );
  return id; 
}

Identifier BesDetectorID::muc(void) const {
  Identifier id = Identifier(  m_MucId << MUC_INDEX );
  return id; 
}

Identifier BesDetectorID::hlt(void) const {
  Identifier id = Identifier(  m_HltId << HLT_INDEX );
  return id;
}

Identifier BesDetectorID::mrpc(void) const {
  Identifier id = Identifier(  m_MrpcId << MRPC_INDEX );
  return id;
}
