#ifndef __TofID_h__
#define __TofID_h__

#include "Identifier/BesDetectorID.h"
#include <string>
#include <assert.h>

class TofID : public BesDetectorID
{
 public:
  typedef Identifier::size_type size_type; 
  typedef Identifier::value_type value_type; 
  
  /// constructor
  TofID();
  
  /// destructor 
  ~TofID();
  
  /// For a single crystal
  static Identifier cell_id ( int barrel_ec,
			      int layer, 
			      int phi_module,   
			      int end
			      );
  static Identifier cell_id( int value );

  static value_type getIntID ( int barrel_ec,
			       int layer,
			       int phi_module,
			       int end
			       );
  
  /// For a single MRPC
  static Identifier cell_id ( int barrel_ec,
			      int endcap,
			      int module,
			      int strip,
			      int end
			      );
  static value_type getIntID ( int barrel_ec,
			       int endcap,
			       int module,
			       int strip,
			       int end
			       );
  
  static value_type getLAYER_BARREL_MAX();
  static value_type getLAYER_ENDCAP_MAX();
  static value_type getPHI_BARREL_MAX();
  static value_type getPHI_ENDCAP_MAX();
  static value_type getBARREL_EC_MAX();
  static value_type getBARREL_EC_MIN();
  
  static bool values_ok ( const unsigned int barrel_ec,  
			  const unsigned int layer, 
			  const unsigned int phi_module,   
			  const unsigned int end
			  ) ;
  
  // MRPC  sunss
  static value_type getBARREL_EC_MRPC();
  static value_type getENDCAP_MRPC_MAX();
  static value_type getMODULE_MRPC_MAX();
  static value_type getSTRIP_MRPC_MAX();
  
  static bool values_ok ( const unsigned int barrel_ec,
			  const unsigned int endcap,
			  const unsigned int module,
			  const unsigned int strip,
			  const unsigned int end
			  ) ;
  // MRPC  sunss
  
  /// Test for barrel
  static bool is_barrel (const Identifier& id) ;  
  static bool is_barrel (const unsigned int barrel_ec) ;

  /// Values of different levels (failure returns 0)
  static int barrel_ec       (const Identifier& id) ;
  static int layer           (const Identifier& id) ; 
  static int phi_module      (const Identifier& id) ; 
  static int end             (const Identifier& id) ; 
 
  /// Max/Min values for each field (error returns -999)
  int phi_module_max         (const Identifier& id) ;
  int layer_max              (const Identifier& id);

  // MRPC  sunss
  static bool is_scin        (const Identifier& id) ;
  static bool is_scin        (const unsigned int barrel_ec) ;
  static bool is_mrpc        (const Identifier& id) ;
  static bool is_mrpc        (const unsigned int barrel_ec) ;

  static int endcap          (const Identifier& id) ;
  static int module          (const Identifier& id) ;
  static int strip           (const Identifier& id) ;
  // MRPC  sunss

private:

  typedef std::vector<Identifier>     id_vec;
  typedef id_vec::const_iterator      id_vec_it;

  static const unsigned int BARREL_EC_INDEX    = 14;
  static const unsigned int BARREL_EC_MASK     = 0x0000C000;

  static const unsigned int LAYER_INDEX        = 8;
  static const unsigned int LAYER_MASK         = 0x00000100;
 
  static const unsigned int PHI_INDEX          = 1;
  static const unsigned int PHI_MASK           = 0x000000FE;

  static const unsigned int END_INDEX          = 0;
  static const unsigned int END_MASK           = 0x00000001;

  static const unsigned int LAYER_BARREL_MAX   = 1; 
  static const unsigned int LAYER_ENDCAP_MAX   = 0;   
  
  static const unsigned int PHI_BARREL_MAX     = 87; 
  static const unsigned int PHI_ENDCAP_MAX     = 47;

  static const unsigned int BARREL_EC_MAX      = 2;
  static const unsigned int BARREL_EC_MIN      = 0;

  // sunss
  static const unsigned int ENDCAP_INDEX       = 11;
  static const unsigned int ENDCAP_MASK        = 0x00000800;

  static const unsigned int MODULE_INDEX       = 5; 
  static const unsigned int MODULE_MASK        = 0x000007E0;

  static const unsigned int STRIP_INDEX        = 1;
  static const unsigned int STRIP_MASK         = 0x0000001E;

  static const unsigned int BARREL_EC_MRPC     = 3;
  static const unsigned int ENDCAP_MRPC_MAX    = 1;
  static const unsigned int MODULE_MRPC_MAX    = 35;
  static const unsigned int STRIP_MRPC_MAX     = 11;

};

#endif
