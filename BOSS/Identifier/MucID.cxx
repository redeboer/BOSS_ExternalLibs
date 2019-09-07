#include "Identifier/MucID.h"
#include <assert.h>

const unsigned int MucID::CHANNEL_BARREL_LAYER_NUM[MucID::LAYER_BARREL_NUM] = 
  {64,112,64,112,64,112,64,112,64};   // change 96 to 112. 2006.10.20

MucID::MucID(void) {
}

MucID::~MucID(void) {
}

//----------------------------------------------------------------------------
bool MucID::values_ok (  const unsigned int barrel_ec,  
                          const unsigned int segment,
                          const unsigned int layer,
		          const unsigned int channel
			) {
  // Check values
  if ( barrel_ec >= BARREL_EC_NUM )     return false;
  if ( barrel_ec == 0 || barrel_ec  == BARREL_EC_NUM - 1 ) {
    if ( segment >= SEGMENT_ENDCAP_NUM ) return false;
    if ( layer   >= LAYER_ENDCAP_NUM   ) return false;
    if ( channel >= CHANNEL_ENDCAP_NUM_MAX ) return false;
  }
  else {
    if ( segment >= SEGMENT_BARREL_NUM )       return false;
    if ( layer   >= LAYER_BARREL_NUM   )       return false;
    if ( channel >= CHANNEL_BARREL_LAYER_NUM[layer] )     return false;
  }
  return true;
}

//----------------------------------------------------------------------------
bool  MucID::is_barrel (const Identifier& id) {
  unsigned int pos = (id.get_value() & MucID::BARREL_EC_MASK) >>  MucID::BARREL_EC_INDEX;
  return (pos == 0 || pos == BARREL_EC_NUM - 1) ? false : true;
}

//----------------------------------------------------------------------------
int MucID::barrel_ec (const Identifier& id) {
  return ((id.get_value() & MucID::BARREL_EC_MASK) >>  MucID::BARREL_EC_INDEX);  
}

//----------------------------------------------------------------------------
int MucID::part (const Identifier& id) {
  return ((id.get_value() & MucID::BARREL_EC_MASK) >>  MucID::BARREL_EC_INDEX);  
}

//----------------------------------------------------------------------------
int MucID::segment (const Identifier& id) {
  return (id.get_value() & MucID::SEGMENT_MASK) >>  MucID::SEGMENT_INDEX;  
}

//----------------------------------------------------------------------------
int MucID::seg (const Identifier& id) {
  return (id.get_value() & MucID::SEGMENT_MASK) >>  MucID::SEGMENT_INDEX;  
}

//----------------------------------------------------------------------------
int MucID::layer (const Identifier& id) {
  return (id.get_value() & MucID::LAYER_MASK) >>  MucID::LAYER_INDEX;  
}

//----------------------------------------------------------------------------
int MucID::gap (const Identifier& id) {
  return (id.get_value() & MucID::LAYER_MASK) >>  MucID::LAYER_INDEX;  
}

//----------------------------------------------------------------------------
int MucID::channel (const Identifier& id) {
  return (id.get_value() & MucID::CHANNEL_MASK) >>  MucID::CHANNEL_INDEX;  
}

//----------------------------------------------------------------------------
int MucID::strip (const Identifier& id) {
  return (id.get_value() & MucID::CHANNEL_MASK) >>  MucID::CHANNEL_INDEX;  
}

//----------------------------------------------------------------------------
int MucID::segment_max  (const Identifier& id) {
   if (is_barrel(id)) {
    return SEGMENT_BARREL_NUM - 1;
  } else {
    return SEGMENT_ENDCAP_NUM - 1;
  }  
}

//----------------------------------------------------------------------------
int MucID::getSegNum  (const Identifier& id) {
   if (is_barrel(id)) {
    return SEGMENT_BARREL_NUM;
  } else {
    return SEGMENT_ENDCAP_NUM;
  }  
}

//----------------------------------------------------------------------------
int MucID::layer_max  (const Identifier& id) {
  if (this->is_barrel(id)) {
    return LAYER_BARREL_NUM - 1;
  } else {
    return LAYER_ENDCAP_NUM - 1;
  } 
}

//----------------------------------------------------------------------------
int MucID::getGapNum  (const Identifier& id) {
  if (this->is_barrel(id)) {
    return LAYER_BARREL_NUM;
  } else {
    return LAYER_ENDCAP_NUM;
  } 
}

//----------------------------------------------------------------------------
int MucID::channel_max  (const Identifier& id) {
  if (this->is_barrel(id)) {
    return CHANNEL_BARREL_NUM_MAX - 1;
  } else {
    return CHANNEL_ENDCAP_NUM_MAX - 1;
  } 
}

//----------------------------------------------------------------------------
int MucID::getStripNum   (const Identifier& id) {
  if (this->is_barrel(id)) {
    return CHANNEL_BARREL_LAYER_NUM[this->gap(id)];
  } else {
    return CHANNEL_ENDCAP_NUM_MAX;
  } 
}

//----------------------------------------------------------------------------
Identifier MucID::channel_id ( int barrel_ec,
                                int segment,
                                int layer,   
			        int channel 
				) {
  assert ( values_ok(barrel_ec, segment, layer, channel) );
  value_type value = (MUC_ID << MUC_INDEX) | (barrel_ec << BARREL_EC_INDEX)|
    (segment << SEGMENT_INDEX) | (layer << LAYER_INDEX) | (channel << CHANNEL_INDEX);
  return Identifier(value);  
}

//----------------------------------------------------------------------------
unsigned int MucID::getIntID ( int barrel_ec,
                              int segment,
                              int layer,
                              int channel
                              ) {
  value_type value = (MUC_ID << MUC_INDEX) | (barrel_ec << BARREL_EC_INDEX)|
    (segment << SEGMENT_INDEX) | (layer << LAYER_INDEX) | (channel << CHANNEL_INDEX);
  return value;

}

//---------------------------------------------------------------------------
unsigned int MucID::getPartNum(){
   return BARREL_EC_NUM;
}

///---------------------------------------------------------------------------
unsigned int MucID::getSegNum( int part ) {
   if (part == 1) return SEGMENT_BARREL_NUM;
   else if (part == 0 || part == 2) return SEGMENT_ENDCAP_NUM;
   else return 0;
}

//----------------------------------------------------------------------------
unsigned int MucID::getGapNum( int part ) {
  if (part == 1) return LAYER_BARREL_NUM;
  if (part == 0 || part == 2) return LAYER_ENDCAP_NUM;
  else return 0;
}

//----------------------------------------------------------------------------
unsigned int MucID::getStripNum   (int part, int seg, int gap) {
  if (part == 1) {
    if (seg == 2) return CHANNEL_BARREL_LAYER_NUM[gap];
    else return CHANNEL_BARREL_LAYER_NUM[gap];
  } else {
    return CHANNEL_ENDCAP_NUM_MAX;
  } 
}

///---------------------------------------------------------------------------
unsigned int MucID::getSegMax() {
  if (SEGMENT_BARREL_NUM > SEGMENT_ENDCAP_NUM) return SEGMENT_BARREL_NUM;
  else return SEGMENT_ENDCAP_NUM;
}

///---------------------------------------------------------------------------

unsigned int MucID::getGapMax() {
  if (LAYER_BARREL_NUM > LAYER_ENDCAP_NUM) return LAYER_BARREL_NUM;
  else return LAYER_ENDCAP_NUM;
}

unsigned int MucID::getStripMax() {
  if (CHANNEL_BARREL_NUM_MAX > CHANNEL_ENDCAP_NUM_MAX) return CHANNEL_BARREL_NUM_MAX;
  else return CHANNEL_ENDCAP_NUM_MAX;
}

//---------------------------------------------------------------------------
unsigned int MucID::getSEGMENT_BARREL_MAX(){
   return SEGMENT_BARREL_NUM - 1;
}

//---------------------------------------------------------------------------
unsigned int MucID::getSEGMENT_ENDCAP_MAX(){
   return SEGMENT_ENDCAP_NUM - 1;
}

//---------------------------------------------------------------------------
unsigned int MucID::getLAYER_BARREL_MAX(){
   return LAYER_BARREL_NUM - 1;
}
 
//---------------------------------------------------------------------------
unsigned int MucID::getLAYER_ENDCAP_MAX(){
   return LAYER_ENDCAP_NUM - 1;
}

//---------------------------------------------------------------------------
unsigned int MucID::getCHANNEL_BARREL_MAX(){
   return CHANNEL_BARREL_NUM_MAX;
}

//---------------------------------------------------------------------------
unsigned int MucID::getCHANNEL_ENDCAP_MAX(){
   return CHANNEL_ENDCAP_NUM_MAX;
}

//---------------------------------------------------------------------------
unsigned int MucID::getBARREL_EC_MAX(){
  return BARREL_EC_NUM - 1;
}

//---------------------------------------------------------------------------
unsigned int MucID::getBARREL_EC_MIN(){
  return 0;
}
