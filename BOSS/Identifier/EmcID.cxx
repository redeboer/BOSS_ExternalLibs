#include "Identifier/EmcID.h"
#include <assert.h>

EmcID::EmcID(void) {
}

EmcID::~EmcID(void) {
}

//----------------------------------------------------------------------------
bool EmcID::values_ok (const unsigned int barrel_ec,
		       const unsigned int theta_module,
		       const unsigned int phi_module
		       ) {
   // Check values
   // unsigned int >= 0, this is definitely true.
   // Omit the compare with MIN(0) to avoid warnings in compile.
   if (barrel_ec  > BARREL_EC_MAX ) return false;
   
   if ( barrel_ec == BARREL ) {
      if ( phi_module    > PHI_BARREL_MAX )     return false;
      if ( theta_module  > THETA_BARREL_MAX )   return false;
   }
   else {  // for endcap
      if ( theta_module  > THETA_ENDCAP_MAX  )  return false; 
      if ( phi_module    > getPHI_ENDCAP_MAX(theta_module)  )  return false;
   }
   return true;
}

//----------------------------------------------------------------------------
bool  EmcID::is_barrel (const Identifier& id) {
   unsigned int pos = (id.get_value() & EmcID::BARREL_EC_MASK) >>  EmcID::BARREL_EC_INDEX;
   return (pos == BARREL) ? true : false ;
}

//----------------------------------------------------------------------------
unsigned int EmcID::barrel_ec (const Identifier& id) {
   return ((id.get_value() & EmcID::BARREL_EC_MASK) >>  EmcID::BARREL_EC_INDEX);  
}

//----------------------------------------------------------------------------
unsigned int EmcID::theta_module (const Identifier& id) {
  return (id.get_value() & EmcID::THETA_MASK) >>  EmcID::THETA_INDEX;  
}

//----------------------------------------------------------------------------
unsigned int EmcID::phi_module (const Identifier& id) {
  return (id.get_value() & EmcID::PHI_MASK) >>  EmcID::PHI_INDEX;  
}

//----------------------------------------------------------------------------
unsigned int EmcID::theta_module_max  (const Identifier& id) {
   if (is_barrel(id)) {
      return THETA_BARREL_MAX;
   } else {
      return THETA_ENDCAP_MAX;
   }  
}

//----------------------------------------------------------------------------
unsigned int EmcID::phi_module_max    (const Identifier& id) {
   if (is_barrel(id)) {
      return PHI_BARREL_MAX;
   } else {
      return getPHI_ENDCAP_MAX(theta_module(id));
   } 
}

//----------------------------------------------------------------------------
Identifier EmcID::crystal_id (const unsigned int barrel_ec,
			      const unsigned int theta_module,   
			      const unsigned int phi_module 
			      ) {
   assert ( values_ok(barrel_ec, theta_module, phi_module) );
   unsigned int value = (EMC_ID << EMC_INDEX) | (barrel_ec << BARREL_EC_INDEX)|
     (theta_module << THETA_INDEX) | (phi_module << PHI_INDEX);
   return Identifier(value); 
}

unsigned int EmcID::getIntID (const unsigned int barrel_ec,
			    const unsigned int theta_module,
			    const unsigned int phi_module
			    ) {
   assert ( values_ok(barrel_ec, theta_module, phi_module) ); 
   unsigned int value = (EMC_ID << EMC_INDEX) | (barrel_ec << BARREL_EC_INDEX)|
                      (theta_module << THETA_INDEX) | (phi_module << PHI_INDEX);
   return value;
}
 
unsigned int EmcID::getTHETA_BARREL_MAX()
{
   return THETA_BARREL_MAX;   
}
unsigned int EmcID::getTHETA_BARREL_MIN()
{
   return THETA_BARREL_MIN;
}
unsigned int EmcID::getTHETA_ENDCAP_MAX()
{
   return THETA_ENDCAP_MAX;
}
unsigned int EmcID::getTHETA_ENDCAP_MIN()
{
   return THETA_ENDCAP_MIN;
}
unsigned int EmcID::getPHI_BARREL_MAX()
{
   return PHI_BARREL_MAX;
}
unsigned int EmcID::getPHI_BARREL_MIN()
{
   return PHI_BARREL_MIN;
}
unsigned int EmcID::getPHI_ENDCAP_MAX(const unsigned int theta)
{
   if(theta==0 || theta==1) { 
      return PHI_ENDCAP_MAX1; 
   }
   if(theta==2 || theta==3) {
      return PHI_ENDCAP_MAX2;
   }
   if(theta==4 || theta==5) {
      return PHI_ENDCAP_MAX3;
   }
   return 0; //no available number
}
unsigned int EmcID::getPHI_ENDCAP_MIN()
{
   return PHI_ENDCAP_MIN;
}
unsigned int EmcID::getBARREL_EC_MAX()
{
   return BARREL_EC_MAX;
}
unsigned int EmcID::getBARREL_EC_MIN()
{
   return BARREL_EC_MIN;
}

unsigned int EmcID::getENDCAP_EAST()
{
   return ENDCAP_EAST;
}

unsigned int EmcID::getBARREL()
{
   return BARREL;
}

unsigned int EmcID::getENDCAP_WEST()
{
      return ENDCAP_WEST;
}
