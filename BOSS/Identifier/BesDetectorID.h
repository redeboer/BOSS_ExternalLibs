#ifndef BESDETECTORID_H
#define BESDETECTORID_H

#include "Identifier/Identifier.h"
#include <string>

/**
 *   class BesDetectorID
 *   
 *   This class provides an interface to generate or decode an
 *   identifier for the upper levels of the detector element hierarchy,
 *   i.e. BES III, the detector systems. 
 **/

class BesDetectorID {

public:    
  BesDetectorID(void);
  ~BesDetectorID(void);
    
  // Detector systems:
  Identifier  mdc(void) const;
  Identifier  tof(void) const;
  Identifier  emc(void) const;
  Identifier  muc(void) const;
  Identifier  hlt(void) const;  
  Identifier  mrpc(void) const;



  // Short print out of any identifier (optionally provide
  // separation character - default is '.'):
  // void show(const Identifier& id, char sep = '.' ) const;

  // or provide the printout in string form
  // std::string 	show_to_string	(const Identifier& id, char sep = '.'  ) const;

  // Expanded print out of any identifier
  // void print(const Identifier& id) const;

  // or provide the printout in string form
  // std::string 	print_to_string	(const Identifier& id) const;

  // Test of an Identifier to see if it belongs to a particular
  // detector system:
  bool is_mdc   (const Identifier& id) const;
  bool is_tof 	(const Identifier& id) const;
  bool is_emc	(const Identifier& id) const;
  bool is_muc	(const Identifier& id) const;
  bool is_hlt   (const Identifier& id) const;
  bool is_mrpc  (const Identifier& id) const;


protected:
  /// Provide efficient access to individual field values
  int  mdc_field_value  () const;     
  int  tof_field_value  () const;       
  int  emc_field_value  () const;       
  int  muc_field_value  () const;       
  int  hlt_field_value  () const;
  int  mrpc_field_value () const;

  // extract detector id information
  int get_detectorID (const Identifier& id) const;  
    
  static const unsigned int MDC_ID       = 0x10;
  static const unsigned int MDC_INDEX    = 24;
  static const unsigned int MDC_MASK     = 0xFF000000;

  static const unsigned int TOF_ID      = 0x20;
  static const unsigned int TOF_INDEX   = 24;
  static const unsigned int TOF_MASK    = 0xFF000000;

  static const unsigned int EMC_ID    = 0x30;
  static const unsigned int EMC_INDEX = 24;
  static const unsigned int EMC_MASK  = 0xFF000000;

  static const unsigned int MUC_ID     = 0x40;
  static const unsigned int MUC_INDEX  = 24;
  static const unsigned int MUC_MASK   = 0xFF000000;

  static const unsigned int HLT_ID     = 0x50;
  static const unsigned int HLT_INDEX  = 24;
  static const unsigned int HLT_MASK   = 0xFF000000;

  static const unsigned int MRPC_ID     = 0x70;
  static const unsigned int MRPC_INDEX  = 24;
  static const unsigned int MRPC_MASK   = 0xFF000000;




private:
  int  m_MdcId;     
  int  m_TofId;      	
  int  m_EmcId; 	
  int  m_MucId; 
  int  m_HltId;    	 
  int  m_MrpcId;
};

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
/////////////////////////////////////////////////////////////////////////////
inline int                 
BesDetectorID::mdc_field_value() const {return (m_MdcId);}     

inline int                 
BesDetectorID::tof_field_value() const {return (m_TofId);}

inline int                 
BesDetectorID::emc_field_value() const {return (m_EmcId);}       

inline int                 
BesDetectorID::muc_field_value() const {return (m_MucId);}       

inline int
BesDetectorID::hlt_field_value() const {return (m_HltId);}

inline int
BesDetectorID::mrpc_field_value() const {return (m_MrpcId);}

#endif // BESDETECTORID_H
