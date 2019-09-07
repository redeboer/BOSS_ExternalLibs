#ifndef __MucID_h__
#define __MucID_h__

#include "Identifier/BesDetectorID.h"
#include <string>
#include <assert.h>

class MucID : public BesDetectorID
{
public:
  typedef Identifier::size_type size_type; 
  typedef Identifier::value_type value_type; 

  /// constructor
  MucID();

  /// destructor 
  ~MucID();
 
  /// For a single crystal
 static Identifier channel_id ( int barrel_ec,
                                int segment,
                                int layer,   
			        int channel 
			      );

  static value_type getIntID ( int barrel_ec,
                              int segment,
                              int layer,
                              int channel
                              );
  static value_type getSEGMENT_BARREL_MAX();
  static value_type getSEGMENT_ENDCAP_MAX();
  static value_type getLAYER_BARREL_MAX();   
  static value_type getLAYER_ENDCAP_MAX();
  static value_type getCHANNEL_BARREL_MAX();
  static value_type getCHANNEL_ENDCAP_MAX();
  static value_type getBARREL_EC_MAX();
  static value_type getBARREL_EC_MIN();

  static value_type getPartNum();
  static value_type getSegNum(int part);
  static value_type getGapNum(int part);   
  static value_type getStripNum(int part, int seg, int gap);
  static value_type getSegMax();
  static value_type getGapMax();   
  static value_type getStripMax(); 

  static bool values_ok ( const unsigned int barrel_ec,
                          const unsigned int segment,  
                          const unsigned int layer,
		          const unsigned int channel 		   
		        ) ;

  /// Test for barrel
  static bool is_barrel (const Identifier& id) ;  

  /// Values of different levels
  static int barrel_ec       (const Identifier& id) ;
  static int segment         (const Identifier& id) ; 
  static int layer           (const Identifier& id) ; 
  static int channel         (const Identifier& id) ;  

  static int part            (const Identifier& id) ;
  static int seg             (const Identifier& id) ; 
  static int gap             (const Identifier& id) ; 
  static int strip           (const Identifier& id) ;  

  /// Max/Min values for each field
  int segment_max  (const Identifier& id) ; 
  int layer_max    (const Identifier& id) ;
  int channel_max  (const Identifier& id) ;  

  int getSegNum       (const Identifier& id) ; 
  int getGapNum       (const Identifier& id) ;
  int getStripNum     (const Identifier& id) ;  

private:

  typedef std::vector<Identifier>     id_vec;
  typedef id_vec::const_iterator      id_vec_it;

  static const unsigned int BARREL_EC_INDEX        = 16;
  static const unsigned int BARREL_EC_MASK         = 0x000F0000;

  static const unsigned int SEGMENT_INDEX          = 12;
  static const unsigned int SEGMENT_MASK           = 0x0000F000;

  static const unsigned int LAYER_INDEX            = 8;
  static const unsigned int LAYER_MASK             = 0x00000F00;
 
  static const unsigned int CHANNEL_INDEX          = 0;
  static const unsigned int CHANNEL_MASK           = 0x000000FF;

  static const unsigned int BARREL_EC_NUM          = 3;  

  static const unsigned int LAYER_BARREL_NUM       = 9; 
  static const unsigned int LAYER_ENDCAP_NUM       = 8;   
  
  static const unsigned int SEGMENT_BARREL_NUM     = 8; 
  static const unsigned int SEGMENT_ENDCAP_NUM     = 4;

  static const unsigned int CHANNEL_BARREL_NUM_MAX = 112; 
  static const unsigned int CHANNEL_ENDCAP_NUM_MAX = 64;

  static const unsigned int CHANNEL_BARREL_LAYER_NUM[LAYER_BARREL_NUM];
    
  static const unsigned int BARREL_EC_MAX          = 2;  
  static const unsigned int BARREL_EC_MIN          = 0;  
  
};

#endif
