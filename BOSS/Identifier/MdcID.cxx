#include "Identifier/MdcID.h"
#include <assert.h>
#include <iostream>

MdcID::MdcID(void) {
}

MdcID::~MdcID(void) {
}

//----------------------------------------------------------------------------
bool MdcID::values_ok ( const unsigned int wireType,  
                        const unsigned int layer, 
			const unsigned int wire 			   
		       ) {
  // Check values
  if ( wireType != AXIAL_WIRE &&  wireType != STEREO_WIRE ) return false;

  if ( wireType == STEREO_WIRE ) {
    if (    layer < INNER_STEREO_LAYER_MAX 
	 || layer < (  INNER_STEREO_LAYER_MAX 
                          + INNER_AXIAL_LAYER_MAX 
                          + OUTER_STEREO_LAYER_MAX )
	    && layer >= (INNER_STEREO_LAYER_MAX + INNER_AXIAL_LAYER_MAX )) 
    return true;
  }
 
  if ( wireType == AXIAL_WIRE ) {
    if (    layer >= INNER_STEREO_LAYER_MAX 
	 && layer < ( INNER_STEREO_LAYER_MAX + INNER_AXIAL_LAYER_MAX )
	 || layer >= ( INNER_STEREO_LAYER_MAX + INNER_AXIAL_LAYER_MAX 
		       + OUTER_STEREO_LAYER_MAX ) && layer < LAYER_MAX )
    
    return true;
  }

  std::cout << " ++ MdcID::values_ok++ wiretype = " << wireType << " layer = "  << layer << " wire = " << wire  << std::endl;
 
  return false;
}

//----------------------------------------------------------------------------
bool  MdcID::is_axial (const Identifier& id) {
  unsigned int type = (id.get_value() & MdcID::WIRETYPE_MASK) >>  MdcID::WIRETYPE_INDEX;
  return (type == AXIAL_WIRE) ? true : false;
}

//----------------------------------------------------------------------------
int MdcID::layer (const Identifier& id) {
  return (id.get_value() & MdcID::LAYER_MASK) >>  MdcID::LAYER_INDEX;  
}

//----------------------------------------------------------------------------
int MdcID::wire (const Identifier& id) {
  return (id.get_value() & MdcID::WIRE_MASK) >>  MdcID::WIRE_INDEX;  
}

//----------------------------------------------------------------------------
int MdcID::layer_max  (const Identifier& id) {
   if (is_axial(id)) {
    return AXIAL_LAYER_MAX;
  } else {
    return STEREO_LAYER_MAX;
  }  
}

//----------------------------------------------------------------------------
int MdcID::wire_max    (const Identifier& id) {
  if (is_axial(id)) {
    return AXIAL_WIRE_MAX;
  } else {
    return STEREO_WIRE_MAX;
  } 
}

//---------------------------------------------------------------------------- 
Identifier  MdcID::wire_id ( int wireType,
			     int layer,   
			     int wire 
			   ) {
  assert ( values_ok(wireType, layer, wire) );
  int value = (BesDetectorID::MDC_ID << MDC_INDEX) | 
     (wireType << WIRETYPE_INDEX) | 
     (layer <<  LAYER_INDEX) | 
     (wire << WIRE_INDEX);
  return Identifier(value); 
}

//---------------------------------------------------------------------------- 
Identifier  MdcID::wire_id ( int layer,   
			     int wire 
			   ) {
  if ( layer < INNER_STEREO_LAYER_MAX 
       || layer < (INNER_STEREO_LAYER_MAX + INNER_AXIAL_LAYER_MAX + OUTER_STEREO_LAYER_MAX)
       && layer >= (INNER_STEREO_LAYER_MAX + INNER_AXIAL_LAYER_MAX ) 
     )
   return MdcID::wire_id ( STEREO_WIRE, layer, wire );
  else  return MdcID::wire_id (AXIAL_WIRE, layer, wire );  
 
}

unsigned int MdcID::getIntID(unsigned int layer,
                           unsigned int wire)
{
  unsigned int value;
  unsigned int wireType;
 
  if ( layer < INNER_STEREO_LAYER_MAX
       || layer < (INNER_STEREO_LAYER_MAX + INNER_AXIAL_LAYER_MAX + OUTER_STEREO_LAYER_MAX)
       && layer >= (INNER_STEREO_LAYER_MAX + INNER_AXIAL_LAYER_MAX )
     )
     wireType = STEREO_WIRE;
  else  wireType = AXIAL_WIRE;
 
  value = ((BesDetectorID::MDC_ID << MDC_INDEX)&MDC_MASK) |
     ((wireType << WIRETYPE_INDEX)&WIRETYPE_MASK) |
     ((layer <<  LAYER_INDEX)&LAYER_MASK) |
     ((wire << WIRE_INDEX)&WIRE_MASK);
 
  return value;
 
}

unsigned int MdcID::getIntID(unsigned int wireType,
                           unsigned int layer,
                           unsigned int wire)
{
   unsigned int value = ((BesDetectorID::MDC_ID << MDC_INDEX)&MDC_MASK) |
     ((wireType << WIRETYPE_INDEX)&WIRETYPE_MASK) |
     ((layer <<  LAYER_INDEX)&LAYER_MASK) |
     ((wire << WIRE_INDEX)&WIRE_MASK); 
   return value;
   
}



unsigned int MdcID::getAXIAL_LAYER_MAX()
{
    return AXIAL_LAYER_MAX;
}
unsigned int MdcID::getSTEREO_LAYER_MAX()
{
    return STEREO_LAYER_MAX;
}
unsigned int MdcID::getAXIAL_WIRE_MAX()
{
    return AXIAL_WIRE_MAX;
}
unsigned int MdcID::getSTEREO_WIRE_MAX()
{
    return STEREO_WIRE_MAX;
}

unsigned int MdcID::getAXIAL_WIRE()
{
    return AXIAL_WIRE;
}

unsigned int MdcID::getSTEREO_WIRE()
{
    return STEREO_WIRE;
}

