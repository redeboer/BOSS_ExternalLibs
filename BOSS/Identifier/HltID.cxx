#include "Identifier/HltID.h"
#include <assert.h>
#include <stdint.h>

HltID::HltID(void) {
}

HltID::~HltID(void) {
}

//----------------------------------------------------------------------------
Identifier HltID::convert(const unsigned int id,const int nmdc,const int ntof,const int nemc,const int nmuc) {
  uint32_t subid=id&(HltID::DETECTOR_MASK|HltID::ID_SUB_MASK);

  if(subid<=getID_HLT_MAX()) return data_type_id(HLT,subid);

  subid = subid - getID_HLT_MAX();
  if(subid<=nmdc)  return data_type_id(MDC,subid-1);

  subid = subid - nmdc;
  if(subid<=ntof)  return data_type_id(TOF,subid-1);

  subid = subid - ntof;
  if(subid<=nemc)  return data_type_id(EMC,subid-1);

  subid = subid - nemc;
  if(subid<=nmuc)  return data_type_id(MUC,subid-1);
  
  subid = subid - nmuc;
  return data_type_id(CON,subid-1);
}

//----------------------------------------------------------------------------
bool HltID::values_ok (const unsigned int detector,const unsigned int id_in_sub) {
  // Check values
  // unsigned int >= 0, this is definitely true.
  // Omit the compare with MIN(0) to avoid warnings in compile.
  switch(detector)
    {
    case HLT:
      if ( id_in_sub  > ID_HLT_MAX  )  return false;
      break;
    case MDC:
      if ( id_in_sub  > ID_MDC_MAX  )  return false;
      break;
    case TOF:
      if ( id_in_sub  > ID_TOF_MAX  )  return false;
      break;
    case EMC:
      if ( id_in_sub  > ID_EMC_MAX  )  return false;
      break;
    case MUC:
      if ( id_in_sub  > ID_MUC_MAX  )  return false;
      break;
    case CON:
      return true;
      break;
    default:
      return false;
      break;
    }
    
  return true;
}

//----------------------------------------------------------------------------
bool  HltID::is_ef_result (const Identifier& id) {
   unsigned int pos = (id.get_value() & HltID::DETECTOR_MASK) >>  HltID::DETECTOR_INDEX;
   return (pos == HLT) ? true : false ;
}

bool  HltID::is_eventtype (const Identifier& id) {
   unsigned int pos = (id.get_value() & HltID::DETECTOR_MASK) >>  HltID::DETECTOR_INDEX;
   unsigned int no  = (id.get_value() & HltID::ID_SUB_MASK) >>  HltID::ID_SUB_INDEX;
   return ((pos == HLT)&&(no==EVENTTYPE)) ? true : false ;
}

bool  HltID::is_algorithm (const Identifier& id) {
   unsigned int pos = (id.get_value() & HltID::DETECTOR_MASK) >>  HltID::DETECTOR_INDEX;
   unsigned int no  = (id.get_value() & HltID::ID_SUB_MASK) >>  HltID::ID_SUB_INDEX;
   return ((pos == HLT)&&(no==ALGORITHM)) ? true : false ;
}

bool  HltID::is_criteria (const Identifier& id) {
   unsigned int pos = (id.get_value() & HltID::DETECTOR_MASK) >>  HltID::DETECTOR_INDEX;
   unsigned int no  = (id.get_value() & HltID::ID_SUB_MASK) >>  HltID::ID_SUB_INDEX;
   return ((pos == HLT)&&(no==CRITERIA)) ? true : false ;
}

bool  HltID::is_version (const Identifier& id) {
   unsigned int pos = (id.get_value() & HltID::DETECTOR_MASK) >>  HltID::DETECTOR_INDEX;
   unsigned int no  = (id.get_value() & HltID::ID_SUB_MASK) >>  HltID::ID_SUB_INDEX;
   return ((pos == HLT)&&(no==VERSION)) ? true : false ;
}

bool  HltID::is_energy (const Identifier& id) {
   unsigned int pos = (id.get_value() & HltID::DETECTOR_MASK) >>  HltID::DETECTOR_INDEX;
   unsigned int no  = (id.get_value() & HltID::ID_SUB_MASK) >>  HltID::ID_SUB_INDEX;
   return ((pos == HLT)&&(no==ENERGY)) ? true : false ;
}

bool  HltID::is_number (const Identifier& id) {
  unsigned int pos = (id.get_value() & HltID::DETECTOR_MASK) >>  HltID::DETECTOR_INDEX;
  unsigned int no  = (id.get_value() & HltID::ID_SUB_MASK) >>  HltID::ID_SUB_INDEX;
  return ((pos == HLT)&&(no==NUMSUB)) ? true : false ;
}

bool  HltID::is_ncon (const Identifier& id) {
  unsigned int pos = (id.get_value() & HltID::DETECTOR_MASK) >>  HltID::DETECTOR_INDEX;
  unsigned int no  = (id.get_value() & HltID::ID_SUB_MASK) >>  HltID::ID_SUB_INDEX;
  return ((pos == CON)&&(no==NUMCON)) ? true : false ;
}

bool  HltID::is_mdc_inf (const Identifier& id) {
   unsigned int pos = (id.get_value() & HltID::DETECTOR_MASK) >>  HltID::DETECTOR_INDEX;
   return (pos == MDC) ? true : false ;
}

bool  HltID::is_tof_inf (const Identifier& id) {
   unsigned int pos = (id.get_value() & HltID::DETECTOR_MASK) >>  HltID::DETECTOR_INDEX;
   return (pos == TOF) ? true : false ;
}

bool  HltID::is_emc_inf (const Identifier& id) {
   unsigned int pos = (id.get_value() & HltID::DETECTOR_MASK) >>  HltID::DETECTOR_INDEX;
   return (pos == EMC) ? true : false ;
}

bool  HltID::is_muc_inf (const Identifier& id) {
   unsigned int pos = (id.get_value() & HltID::DETECTOR_MASK) >>  HltID::DETECTOR_INDEX;
   return (pos == MUC) ? true : false ;
}

bool  HltID::is_con_inf (const Identifier& id) {
  unsigned int pos = (id.get_value() & HltID::DETECTOR_MASK) >>  HltID::DETECTOR_INDEX;
  return (pos == CON) ? true : false ;
}

//----------------------------------------------------------------------------
unsigned int HltID::detector (const Identifier& id) {
   return ((id.get_value() & HltID::DETECTOR_MASK) >>  HltID::DETECTOR_INDEX);  
}

//----------------------------------------------------------------------------
unsigned int HltID::id_sub (const Identifier& id) {
  return (id.get_value() & HltID::ID_SUB_MASK) >>  HltID::ID_SUB_INDEX;  
}

//----------------------------------------------------------------------------
unsigned int HltID::id_sub_max (const Identifier& id) {
  if (is_ef_result(id)) {
    return ID_HLT_MAX;
  }
  else if (is_mdc_inf(id)) {
    return ID_MDC_MAX;
  }
  else if (is_tof_inf(id)) {
    return ID_TOF_MAX;
  }
  else if (is_emc_inf(id)) {
    return ID_EMC_MAX;
  }
  else if (is_muc_inf(id)) {
    return ID_MUC_MAX;
  }
  else {
    return 0;
  }
}

//----------------------------------------------------------------------------
Identifier HltID::data_type_id(const unsigned int detector,const unsigned int id_in_sub){
  assert ( values_ok(detector, id_in_sub) );
  //if(!values_ok(detector, id_in_sub)) return Identifier(0);

  unsigned int value = (HLT_ID << HLT_INDEX) | (detector << DETECTOR_INDEX)|
     (id_in_sub<< ID_SUB_INDEX);
  return Identifier(value); 
}

unsigned int HltID::getIntID (const unsigned int detector,const unsigned int id_in_sub){
  assert ( values_ok(detector, id_in_sub) );
  //if(!values_ok(detector, id_in_sub)) return 0;

  unsigned int value = (HLT_ID << HLT_INDEX) | (detector << DETECTOR_INDEX)|
     (id_in_sub<< ID_SUB_INDEX);
  return value;
}

