#ifndef __MdcID_h__
#define __MdcID_h__

#include "Identifier/BesDetectorID.h"
#include <string>
#include <assert.h>

class MdcID : public BesDetectorID
{
public:
  typedef Identifier::size_type size_type; 
  typedef Identifier::value_type value_type; 

  /// constructor
  MdcID();

  /// destructor 
  ~MdcID();
 
  /// For a single wire
 static Identifier wire_id ( int wireType,
                             int layer,   
			     int wire 
			    ) ;

 static Identifier wire_id ( int layer, int wire );


 static value_type getIntID( unsigned  int wireType,
                             unsigned  int layer,
                             unsigned  int wire
                           );

 static value_type getIntID( unsigned  int layer,
                             unsigned  int wire
                           );

 static value_type getAXIAL_LAYER_MAX();
 static value_type getSTEREO_LAYER_MAX();
 static value_type getAXIAL_WIRE_MAX();
 static value_type getSTEREO_WIRE_MAX();
 static value_type getAXIAL_WIRE();
 static value_type getSTEREO_WIRE();
 
 
 static bool values_ok ( const unsigned int wireType,
			 const unsigned int layer,
			 const unsigned int wire 		   
		       ) ;
 /// Test for axial and stereo wire
// bool  MdcID::is_axial (const Identifier& id);
   bool  is_axial (const Identifier& id);

 /// Values of different levels (failure returns 0)
 static int layer    (const Identifier& id) ; 
 static int wire     (const Identifier& id) ; 
 

 /// Max/Min values for each field (error returns -999)
 int layer_max  (const Identifier& id) ;
 int wire_max    (const Identifier& id) ;

private:
 
 typedef std::vector<Identifier>     id_vec;
 typedef id_vec::const_iterator      id_vec_it;

 static const unsigned int AXIAL_WIRE          = 0;
 static const unsigned int STEREO_WIRE         = 1;

 static const unsigned int WIRETYPE_INDEX          = 15;
 static const unsigned int WIRETYPE_MASK           = 0x00008000;

 //LAYER numbering not seperating AXIAL and STEREO
 static const unsigned int LAYER_INDEX        = 9;
 static const unsigned int LAYER_MASK         = 0x00007E00;
 

 static const unsigned int WIRE_INDEX          = 0;
 static const unsigned int WIRE_MASK           = 0x000001FF;

 static const unsigned int AXIAL_LAYER_MAX      = 18;
 static const unsigned int INNER_AXIAL_LAYER_MAX      = 12;
 static const unsigned int OUTER_AXIAL_LAYER_MAX      = 7;

 static const unsigned int LAYER_MAX = 43;
 
   
 static const unsigned int STEREO_LAYER_MAX     = 23; 
 static const unsigned int INNER_STEREO_LAYER_MAX     = 8; 
 static const unsigned int OUTER_STEREO_LAYER_MAX     =16; 
 
 static const unsigned int  AXIAL_WIRE_MAX      = 287; 
 static const unsigned int  STEREO_WIRE_MAX     = 239; 
};

#endif
