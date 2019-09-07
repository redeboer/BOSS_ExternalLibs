#ifndef __HltID_h__
#define __HltID_h__

#include "Identifier/BesDetectorID.h"
//#include <string>
//#include <assert.h>

class HltID : public BesDetectorID
{
 public:
  typedef Identifier::size_type size_type; 
  typedef Identifier::value_type value_type; 
    
  /// constructor
  HltID(void);

  /// destructor 
  ~HltID(void);

  enum Header {EVENTTYPE =0,ALGORITHM,CRITERIA,VERSION,ENERGY,NUMSUB,NUMCON};
  enum SubID {HLT =0,MDC,TOF,EMC,MUC,CON};
  enum MdcData{ NMDC =0x100, NTRK, PMAX1, COST1, VZ1, PHI1, VR1, TRKHIT1, TRKSHIT1,
		PMAX2, COST2, VZ2, PHI2, VR2, TRKHIT2, TRKSHIT2, ACOL};
  enum TofData{ NTOF =0x200, DELTAT, DELTAPHI };
  enum EmcData{ NEMC =0x300, ETOT, EBAR, EEND, EBAL, EEAST, EWEST, NSEC, NSHW,
                EMAX1, ECOST1, EPHI1, EMAX2, ECOST2, EPHI2, EACOP, EACOL, EMAX12,
                EMAX3, ECOST3, EPHI3 };
  enum MucData{ NMUC =0x400 };
  enum ConData{ NCON =0x500 };

  /// convert global id to sub-id(Identifier)
  static Identifier convert(const unsigned int id,const int nmdc=0,const int ntof=0,
			    const int nemc=0,const int nmuc=0);

  /// For a single data
  static Identifier data_type_id (const unsigned int detector,const unsigned int id_in_sub=0);
  static unsigned int getIntID (const unsigned int detector,const unsigned int id_in_sub=0);
  static bool values_ok (const unsigned int detector, const unsigned int id_in_sub=0);

  /// Test for data type
  static bool is_ef_result  (const Identifier& id);
  static bool is_eventtype  (const Identifier& id);
  static bool is_algorithm  (const Identifier& id);
  static bool is_criteria   (const Identifier& id);
  static bool is_version    (const Identifier& id);
  static bool is_energy     (const Identifier& id);
  static bool is_number     (const Identifier& id);
  static bool is_ncon       (const Identifier& id);

  static bool is_mdc_inf    (const Identifier& id);
  static bool is_tof_inf    (const Identifier& id);
  static bool is_emc_inf    (const Identifier& id);
  static bool is_muc_inf    (const Identifier& id);
  static bool is_con_inf    (const Identifier& id);
  static bool is_hlt_inf    (const Identifier& id){ return is_ef_result(id);};

  /// Values of different levels (failure returns 0)
  static unsigned int detector  (const Identifier& id) ;
  static unsigned int id_sub    (const Identifier& id) ;

  static unsigned int id_sub_max       (const Identifier& id) ; 

  static unsigned int getID_MDC_MAX (){return ID_MDC_MAX;};
  static unsigned int getID_TOF_MAX (){return ID_TOF_MAX;};
  static unsigned int getID_EMC_MAX (){return ID_EMC_MAX;};
  static unsigned int getID_MUC_MAX (){return ID_MUC_MAX;};
  static unsigned int getID_HLT_MAX (){return ID_HLT_MAX;};

  static unsigned int getDETECTOR_MIN(){return DETECTOR_MIN;};
  static unsigned int getDETECTOR_MAX(){return DETECTOR_MAX;};
  //static unsigned int getHLT() {return HLT;};
  //static unsigned int getMDC() {return MDC;};
  //static unsigned int getTOF() {return TOF;};
  //static unsigned int getEMC() {return EMC;};
  //static unsigned int getMUC() {return MUC;};
  //static unsigned int getCON() {return CON;};

 private:
  static const unsigned int DETECTOR_INDEX      = 8;
  static const unsigned int DETECTOR_MASK       = 0x00000F00;

  static const unsigned int ID_SUB_INDEX      = 0;
  static const unsigned int ID_SUB_MASK       = 0x000000FF;
  static const unsigned int ID_MDC_MAX        = 19;   //20
  static const unsigned int ID_TOF_MAX        = 4;   //5
  static const unsigned int ID_EMC_MAX        = 21;   //22
  static const unsigned int ID_MUC_MAX        = 2;   //3
  static const unsigned int ID_HLT_MAX        = 5;   //6

  static const unsigned int DETECTOR_MAX      = 5;
  static const unsigned int DETECTOR_MIN      = 0;
};

#endif
