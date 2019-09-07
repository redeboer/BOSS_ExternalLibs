#include "Identifier/TofID.h"
#include <assert.h>
#include <iostream>


TofID::TofID(void) {
}

TofID::~TofID(void) {
}

//----------------------------------------------------------------------------
bool TofID::values_ok ( const unsigned int barrel_ec,  
			const unsigned int layer, 
			const unsigned int phi_module,
			const unsigned int end
			) {
  if ( barrel_ec > BARREL_EC_MAX ) return false;
  if ( BARREL_EC_MIN == barrel_ec || BARREL_EC_MIN == barrel_ec) {
    if ( layer > LAYER_ENDCAP_MAX ) return false;
  }
  else {
    if ( layer > LAYER_BARREL_MAX  ) return false;
  }
  if( end > 1 ) return false;

  return true;
}


//-----------------------------------------------------------------------------
bool TofID::values_ok ( const unsigned int barrel_ec,
			const unsigned int endcap,
			const unsigned int module,
			const unsigned int strip,
			const unsigned int end
			) {
  if( barrel_ec != BARREL_EC_MRPC ) return false;
  if( endcap > ENDCAP_MRPC_MAX ) return false;
  if( module > MODULE_MRPC_MAX ) return false;
  if( strip  > STRIP_MRPC_MAX  ) return false;
  if( end    > 1               ) return false;

  return true;
}


//----------------------------------------------------------------------------
bool  TofID::is_barrel (const Identifier& id) {
  unsigned int pos = (id.get_value() & TofID::BARREL_EC_MASK) >>  TofID::BARREL_EC_INDEX;
  return (pos == BARREL_EC_MAX || pos == BARREL_EC_MIN || pos == BARREL_EC_MRPC) ? false : true;
}


bool  TofID::is_barrel (const unsigned int barrel_ec) {
  return (barrel_ec == BARREL_EC_MAX || barrel_ec == BARREL_EC_MIN || barrel_ec == BARREL_EC_MRPC) ? false : true;
}


//----------------------------------------------------------------------------
int TofID::barrel_ec (const Identifier& id ) {
  return ((id.get_value() & TofID::BARREL_EC_MASK) >>  TofID::BARREL_EC_INDEX);
}

//----------------------------------------------------------------------------
int TofID::layer (const Identifier& id) {
  if( ( TofID::is_mrpc( id ) ) ) { return -999; }
  return ((id.get_value() & TofID::LAYER_MASK) >>  TofID::LAYER_INDEX);
}


//----------------------------------------------------------------------------
int TofID::phi_module (const Identifier& id) {
  if( ( TofID::is_mrpc( id ) ) ) { return -999; }
  return ((id.get_value() & TofID::PHI_MASK) >>  TofID::PHI_INDEX);
}

//----------------------------------------------------------------------------
int TofID::end(const Identifier& id) {
  return ((id.get_value() & TofID::END_MASK) >> TofID::END_INDEX);
}

//----------------------------------------------------------------------------
int TofID::layer_max  (const Identifier& id) {
   if (is_barrel(id)) {
    return LAYER_BARREL_MAX;
  } else {
    return LAYER_ENDCAP_MAX;
  }  
}

//----------------------------------------------------------------------------
int TofID::phi_module_max    (const Identifier& id) {
  if (is_barrel(id)) {
    return PHI_BARREL_MAX;
  } else {
    return PHI_ENDCAP_MAX;
  } 
}

//----------------------------------------------------------------------------
bool TofID::is_scin( const Identifier& id ) {
  unsigned int pos = (id.get_value() & TofID::BARREL_EC_MASK) >>  TofID::BARREL_EC_INDEX;
  return (pos == BARREL_EC_MRPC) ? false : true;
}


bool TofID::is_scin(const unsigned int barrel_ec ) {
  return (barrel_ec < BARREL_EC_MRPC) ? true : false;
}

//----------------------------------------------------------------------------
bool TofID::is_mrpc( const Identifier& id ) {
  unsigned int pos = (id.get_value() & TofID::BARREL_EC_MASK) >>  TofID::BARREL_EC_INDEX;
  return (pos == BARREL_EC_MRPC) ? true : false;
}


bool TofID::is_mrpc(const unsigned int barrel_ec ) {
  return (barrel_ec == BARREL_EC_MRPC) ? true : false;
}

//----------------------------------------------------------------------------
int TofID::endcap( const Identifier& id ) {
  if( ( TofID::is_scin( id ) ) ) { return -999; }
  return ((id.get_value() & TofID::ENDCAP_MASK) >>  TofID::ENDCAP_INDEX);
}

//----------------------------------------------------------------------------
int TofID::module( const Identifier& id ) {
  if( ( TofID::is_scin( id ) ) ) { return -999; }
  return ((id.get_value() & TofID::MODULE_MASK) >>  TofID::MODULE_INDEX);
}

//----------------------------------------------------------------------------
int TofID::strip( const Identifier& id ) {
  if( ( TofID::is_scin( id ) ) ) { return -999; }
  return ((id.get_value() & TofID::STRIP_MASK) >>  TofID::STRIP_INDEX);
}


//----------------------------------------------------------------------------
Identifier TofID::cell_id ( int barrel_ec,
			    int layer,   
			    int phi_module,
			    int end 
			    ) {
  assert ( values_ok(barrel_ec, layer, phi_module, end) ); 
  unsigned int value = (BesDetectorID::TOF_ID << TOF_INDEX) | 
                       (barrel_ec << BARREL_EC_INDEX)|
                       (layer << LAYER_INDEX) | (phi_module << PHI_INDEX) | 
                       (end << END_INDEX);
  return Identifier(value); 
}


Identifier TofID::cell_id(int value) {
  return Identifier(value);
}


//----------------------------------------------------------------------------
Identifier TofID::cell_id ( int barrel_ec,
                            int endcap,
                            int module,
			    int strip,
                            int end
                            ) {
  assert ( values_ok(barrel_ec, endcap, module, strip, end) );
  unsigned int value = (BesDetectorID::TOF_ID << TOF_INDEX) |
                       (barrel_ec << BARREL_EC_INDEX)|
                       (endcap << ENDCAP_INDEX) | (module << MODULE_INDEX) |
                       (strip << STRIP_INDEX) | (end << END_INDEX);
  return Identifier(value);
}

//----------------------------------------------------------------------------
unsigned int TofID::getIntID ( int barrel_ec,
			       int layer,
			       int phi_module,
			       int end
			       )
{
     unsigned int value = (BesDetectorID::TOF_ID << TOF_INDEX) |
                     (barrel_ec << BARREL_EC_INDEX)|
                     (layer << LAYER_INDEX) | (phi_module << PHI_INDEX) |
                     (end << END_INDEX);
     return value;
}


//----------------------------------------------------------------------------
unsigned int TofID::getIntID ( int barrel_ec,
			       int endcap,
			       int module,
			       int strip,
			       int end
			       ) {
  unsigned int value = (BesDetectorID::TOF_ID << TOF_INDEX) |
                       (barrel_ec << BARREL_EC_INDEX)|
                       (endcap << ENDCAP_INDEX) | (module << MODULE_INDEX) |
                       (strip << STRIP_INDEX) | (end << END_INDEX);
  return value;
}


unsigned int TofID::getLAYER_BARREL_MAX()
{
     return LAYER_BARREL_MAX;
}

unsigned int TofID::getLAYER_ENDCAP_MAX()
{
     return LAYER_ENDCAP_MAX;
}

unsigned int TofID::getPHI_BARREL_MAX()
{
     return PHI_BARREL_MAX;
}

unsigned int TofID::getPHI_ENDCAP_MAX()
{
     return PHI_ENDCAP_MAX;
}
unsigned int TofID::getBARREL_EC_MAX()
{
     return BARREL_EC_MAX;
}

unsigned int TofID::getBARREL_EC_MIN()
{
     return BARREL_EC_MIN;
}

unsigned int TofID::getBARREL_EC_MRPC()
{
  return BARREL_EC_MRPC;
}

unsigned int TofID::getENDCAP_MRPC_MAX()
{
  return ENDCAP_MRPC_MAX;
}

unsigned int TofID::getMODULE_MRPC_MAX()
{
  return MODULE_MRPC_MAX;
}

unsigned int TofID::getSTRIP_MRPC_MAX()
{
  return STRIP_MRPC_MAX;
}
