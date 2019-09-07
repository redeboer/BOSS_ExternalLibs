#ifndef __EmcID_h__
#define __EmcID_h__

#include "Identifier/BesDetectorID.h"
#include <string>
#include <assert.h>

class EmcID : public BesDetectorID
{
 public:
   typedef Identifier::size_type  size_type; 
   typedef Identifier::value_type value_type; 
   
   /// constructor
   EmcID();
   
   /// destructor 
   ~EmcID();
   
   /// For a single crystal
   static Identifier crystal_id (const unsigned int barrel_ec,
                                 const unsigned int theta_module,   
			         const unsigned int phi_module 
				 );

   static unsigned int getIntID (const unsigned int barrel_ec,
                               const unsigned int theta_module,
                               const unsigned int phi_module 
                              );

   static bool values_ok (const unsigned int barrel_ec,  
                          const unsigned int theta_module,
		          const unsigned int phi_module 		   
			  );
   
   /// Test for barrel
   static bool is_barrel (const Identifier& id);  
   
   /// Values of different levels (failure returns 0)
   static unsigned int barrel_ec       (const Identifier& id) ;
   static unsigned int theta_module    (const Identifier& id) ; 
   static unsigned int phi_module      (const Identifier& id) ; 
   
   
   /// Max/Min values for each field (error returns -999)
   unsigned int theta_module_max  (const Identifier& id) ;
   unsigned int phi_module_max    (const Identifier& id) ;

   static unsigned int getTHETA_BARREL_MAX();
   static unsigned int getTHETA_BARREL_MIN();
   static unsigned int getTHETA_ENDCAP_MAX();
   static unsigned int getTHETA_ENDCAP_MIN();
   static unsigned int getPHI_BARREL_MAX();
   static unsigned int getPHI_BARREL_MIN();
   static unsigned int getPHI_ENDCAP_MAX(const unsigned int theta);
   static unsigned int getPHI_ENDCAP_MIN();
   static unsigned int getBARREL_EC_MAX();
   static unsigned int getBARREL_EC_MIN();
   static unsigned int getENDCAP_EAST();
   static unsigned int getBARREL();
   static unsigned int getENDCAP_WEST();
   
 private:
   static const unsigned int BARREL_EC_INDEX    = 16;
   static const unsigned int BARREL_EC_MASK     = 0x000F0000;
   
   static const unsigned int THETA_INDEX        = 8;
   static const unsigned int THETA_MASK         = 0x00003F00;
   
   static const unsigned int PHI_INDEX          = 0;
   static const unsigned int PHI_MASK           = 0x000000FF;
   
   static const unsigned int THETA_BARREL_MAX     = 43;
   static const unsigned int THETA_BARREL_MIN     = 0;
   static const unsigned int THETA_ENDCAP_MAX     = 5;
   static const unsigned int THETA_ENDCAP_MIN     = 0;
   
   static const unsigned int PHI_BARREL_MAX       = 119;
   static const unsigned int PHI_BARREL_MIN       = 0;
   static const unsigned int PHI_ENDCAP_MAX1      = 63;
   static const unsigned int PHI_ENDCAP_MAX2      = 79;
   static const unsigned int PHI_ENDCAP_MAX3      = 95;
   static const unsigned int PHI_ENDCAP_MIN       = 0;
   
   static const unsigned int BARREL_EC_MAX      = 2;
   static const unsigned int BARREL_EC_MIN      = 0;
   static const unsigned int ENDCAP_EAST        = 0;
   static const unsigned int BARREL             = 1;
   static const unsigned int ENDCAP_WEST        = 2;
};

#endif
