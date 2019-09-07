#ifndef TOFHITSTATUS_H
#define TOFHITSTATUS_H

#include "DstEvent/DstTofTrack.h"

class TofHitStatus {

 public:

  TofHitStatus();
  virtual ~TofHitStatus() {}

  void init();
  TofHitStatus& operator=(const DstTofTrack& source);
  TofHitStatus& operator=(const TofHitStatus& source);
  //  TofHitStatus& operator+(const TofHitStatus& source1, const TofHitStatus& source2 )

 public:

  unsigned int value()          const { return  m_status;   }
  //  void status( const bool raw, bool readout, bool counter, bool cluster,bool barrel, bool east, const unsigned int layer, unsigned int ncounter, unsigned int neast, unsigned int nwest ) const;
  bool  is_raw()                const { return  m_raw;      }
  bool  is_readout()            const { return  m_readout;  }
  bool  is_counter()            const { return  m_counter;  }
  bool  is_cluster()            const { return  m_cluster;  }
  bool  is_barrel()             const { return  m_barrel;   }
  bool  is_east()               const { return  m_east;     }
  unsigned int  layer()         const { return  m_layer;    }
  bool  is_overflow()           const { return  m_overflow; }
  bool  is_multihit()           const { return  m_multihit; }
  unsigned int  ncounter()      const { return  m_ncounter; }
  unsigned int  neast()         const { return  m_neast;    }
  unsigned int  nwest()         const { return  m_nwest;    }
  bool  is_mrpc()               const { return  m_mrpc;     }

  static bool  is_raw( unsigned int status );
  static bool  is_readout( unsigned int status );
  static bool  is_counter( unsigned int status );
  static bool  is_cluster( unsigned int status );
  static bool  is_barrel( unsigned int status );
  static bool  is_east( unsigned int status );
  static unsigned int  layer( unsigned int status );
  static bool  is_overflow( unsigned int status );
  static bool  is_multihit( unsigned int status );
  static unsigned int  ncounter( unsigned int status );
  static unsigned int  neast( unsigned int status );
  static unsigned int  nwest( unsigned int status );
  static unsigned int  part( unsigned int status );
  static bool  is_mrpc( unsigned int status );

 public:

  void setStatus( unsigned int status );
  void setRaw( bool raw );
  void setReadout( bool readout );
  void setCounter( bool counter );
  void setCluster( bool cluster );
  void setBarrel( bool barrel );
  void setEast( bool east );
  void setLayer( unsigned int layer );
  void setOverflow( bool overflow );
  void setMultihit( bool multihit );
  void setNCounter( unsigned int ncounter );
  void setNEast( unsigned int neast );
  void setNWest( unsigned int nwest );
  void setMRPC( bool mrpc );

  void setBarrelCluster( unsigned int layer );
  void setBarrelCounter( unsigned int layer );
  void setBarrelReadout( unsigned int layer, bool east );
  void setBarrelRaw( unsigned int layer, bool east );
  void setEndcapCluster( bool east );
  void setEndcapRaw( bool east );
  void setMRPCReadout( bool east );
  void setMRPCCluster( bool cluster, bool east );
  void setNoHit();

 private:

  unsigned int m_status;
  bool m_raw;
  bool m_readout;
  bool m_counter;
  bool m_cluster;
  bool m_barrel;
  bool m_east;
  unsigned int m_layer;
  bool m_overflow;
  bool m_multihit;
  unsigned int m_ncounter;
  unsigned int m_neast;
  unsigned int m_nwest;
  bool m_mrpc;

  static const unsigned int Raw_Index      = 0;
  static const unsigned int Raw_Mask       = 0x00000001;
  static const unsigned int Readout_Index  = 1;
  static const unsigned int Readout_Mask   = 0x00000002;
  static const unsigned int Counter_Index  = 2;
  static const unsigned int Counter_Mask   = 0x00000004;
  static const unsigned int Cluster_Index  = 3;
  static const unsigned int Cluster_Mask   = 0x00000008;
  static const unsigned int Barrel_Index   = 4;
  static const unsigned int Barrel_Mask    = 0x00000010;
  static const unsigned int East_Index     = 5;
  static const unsigned int East_Mask      = 0x00000020;
  static const unsigned int Layer_Index    = 6;
  static const unsigned int Layer_Mask     = 0x000000C0;
  static const unsigned int Overflow_Index = 8;
  static const unsigned int Overflow_Mask  = 0x00000100;
  static const unsigned int Multihit_Index = 9;
  static const unsigned int Multihit_Mask  = 0x00000200;
  static const unsigned int Ncounter_Index = 12;
  static const unsigned int Ncounter_Mask  = 0x0000F000;
  static const unsigned int Neast_Index    = 16;
  static const unsigned int Neast_Mask     = 0x000F0000;
  static const unsigned int Nwest_Index    = 20;
  static const unsigned int Nwest_Mask     = 0x00F00000;
  static const unsigned int N_Mask         = 0x0000000F;
  static const unsigned int MRPC_Index     = 24;
  static const unsigned int MRPC_Mask      = 0x01000000;
  static const unsigned int N_MRPC         = 0x00000001;

};
#endif    //TOFHITSTATUS_H
