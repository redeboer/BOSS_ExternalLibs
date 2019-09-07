#include "DstEvent/TofHitStatus.h"
#include "DstEvent/DstTofTrack.h"
#include <assert.h>

TofHitStatus::TofHitStatus() {
  m_status   = 0;
  m_raw      = false;
  m_readout  = false;
  m_counter  = false;
  m_cluster  = false;
  m_barrel   = false;
  m_east     = false;
  m_layer    = 0;
  m_overflow = false;
  m_multihit = false;
  m_ncounter = 0;
  m_neast    = 0;
  m_nwest    = 0;
  m_mrpc     = false;
}


void TofHitStatus::init() {
  m_status   = 0;
  m_raw      = false;
  m_readout  = false;
  m_counter  = false;
  m_cluster  = false;
  m_barrel   = false;
  m_east     = false;
  m_layer    = 0;
  m_overflow = false;
  m_multihit = false;
  m_ncounter = 0;
  m_neast    = 0;
  m_nwest    = 0;
  m_mrpc     = false;
}


TofHitStatus& 
TofHitStatus::operator=(const DstTofTrack& source){
  m_status = source.status();
  m_raw      = ((m_status & Raw_Mask) >> Raw_Index ) ? true : false;
  m_readout  = ((m_status & Readout_Mask) >> Readout_Index ) ? true : false;
  m_counter  = ((m_status & Counter_Mask) >> Counter_Index ) ? true : false;
  m_cluster  = ((m_status & Cluster_Mask) >> Cluster_Index ) ? true : false;
  m_barrel   = ((m_status & Barrel_Mask) >> Barrel_Index ) ? true : false;
  m_east     = ((m_status & East_Mask) >> East_Index ) ? true : false;
  m_layer    = (m_status  & Layer_Mask) >> Layer_Index;
  m_overflow = ((m_status & Overflow_Mask) >> Overflow_Index ) ? true : false;
  m_multihit = ((m_status & Multihit_Mask) >> Multihit_Index ) ? true : false;
  m_ncounter = (m_status >> Ncounter_Index ) & N_Mask;
  m_neast    = (m_status >> Neast_Index ) & N_Mask;
  m_nwest    = (m_status >> Nwest_Index ) & N_Mask;
  m_mrpc     = ((m_status & MRPC_Mask) >> MRPC_Index ) ? true : false;

  return *this;
}


TofHitStatus& 
TofHitStatus::operator=( const TofHitStatus& source ){
  m_status = source.m_status;
  return *this;
}


bool TofHitStatus::is_raw( unsigned int status ) {
  return ((status & Raw_Mask) >> Raw_Index ) ? true : false;
}


bool TofHitStatus::is_readout( unsigned int status ) {
  return ((status & Readout_Mask) >> Readout_Index ) ? true : false;
}


bool TofHitStatus::is_counter( unsigned int status ) {
  return ((status & Counter_Mask) >> Counter_Index ) ? true : false;
}


bool TofHitStatus::is_cluster( unsigned int status ) {
  return ((status & Cluster_Mask) >> Cluster_Index ) ? true : false;
}


bool TofHitStatus::is_barrel( unsigned int status ) {
  return ((status & Barrel_Mask) >> Barrel_Index ) ? true : false;
}


bool TofHitStatus::is_east( unsigned int status ) {
  return ((status & East_Mask) >> East_Index ) ? true : false;
}


unsigned int TofHitStatus::layer( unsigned int status ) {
  return (status  & Layer_Mask) >> Layer_Index;
}


bool TofHitStatus::is_overflow( unsigned int status ) {
  return ((status & Overflow_Mask) >> Overflow_Index ) ? true : false;
}


bool TofHitStatus::is_multihit( unsigned int status ) {
  return ((status & Multihit_Mask) >> Multihit_Index ) ? true : false;
}


unsigned int TofHitStatus::ncounter( unsigned int status ) {
  return (status >> Ncounter_Index ) & N_Mask;
}


unsigned int  TofHitStatus::neast( unsigned int status ) {
  return (status >> Neast_Index ) & N_Mask;
}


unsigned int  TofHitStatus::nwest( unsigned int status ) {
  return (status >> Nwest_Index ) & N_Mask;
}


bool TofHitStatus::is_mrpc( unsigned int status ) {
  return ((status & MRPC_Mask) >> MRPC_Index ) ? true : false;
}


void TofHitStatus::setStatus( unsigned int status ) {
  m_status   = status;
  m_raw      = ((m_status & Raw_Mask) >> Raw_Index ) ? true : false;
  m_readout  = ((m_status & Readout_Mask) >> Readout_Index ) ? true : false;
  m_counter  = ((m_status & Counter_Mask) >> Counter_Index ) ? true : false;
  m_cluster  = ((m_status & Cluster_Mask) >> Cluster_Index ) ? true : false;
  m_barrel   = ((m_status & Barrel_Mask) >> Barrel_Index ) ? true : false;
  m_east     = ((m_status & East_Mask) >> East_Index ) ? true : false;
  m_layer    = (m_status  & Layer_Mask) >> Layer_Index;
  m_overflow = ((m_status & Overflow_Mask) >> Overflow_Index ) ? true : false;
  m_multihit = ((m_status & Multihit_Mask) >> Multihit_Index ) ? true : false;
  m_ncounter = (m_status >> Ncounter_Index ) & N_Mask;
  m_neast    = (m_status >> Neast_Index ) & N_Mask;
  m_nwest    = (m_status >> Nwest_Index ) & N_Mask;
  m_mrpc     = ((m_status & MRPC_Mask) >> MRPC_Index ) ? true : false;
}


void TofHitStatus::setRaw( bool raw ) {
  m_raw = raw;
  if( raw ) { m_status |= Raw_Mask; }
  else { m_status &= (~Raw_Mask); }
}


void TofHitStatus::setReadout( bool readout ) {
  m_readout = readout;
  if( readout ) { m_status |= Readout_Mask; }
  else { m_status &= (~Readout_Mask); }
}


void TofHitStatus::setCounter( bool counter ) {
  m_counter = counter;
  if( counter ) { m_status |= Counter_Mask; }
  else { m_status &= (~Counter_Mask); }
}


void TofHitStatus::setCluster( bool cluster ) {
  m_cluster = cluster;
  if( cluster ) { m_status |= Cluster_Mask; }
  else { m_status &= (~Cluster_Mask); }
}


void TofHitStatus::setBarrel( bool barrel ) {
  m_barrel = barrel;
  if( barrel ) { m_status |= Barrel_Mask; }
  else { m_status &= (~Barrel_Mask); }
}


void TofHitStatus::setEast( bool east ) {
  m_east = east;
  if( east ) { m_status |= East_Mask; }
  else { m_status &= (~East_Mask); }
}


void TofHitStatus::setLayer( unsigned int layer ) {
  assert(layer<=3);
  m_layer = layer;
  m_status |= (layer << Layer_Index);
}


void TofHitStatus::setOverflow( bool overflow ) {
  m_overflow = overflow;
  if( overflow ) { m_status |= Overflow_Mask; }
  else { m_status &= (~Overflow_Mask); }
}


void TofHitStatus::setMultihit( bool multihit ) {
  m_multihit = multihit;
  if( multihit ) { m_status |= Multihit_Mask; }
  else { m_status &= (~Multihit_Mask); }
}


void TofHitStatus::setNCounter( unsigned int ncounter ) {
  if( m_barrel ) assert(ncounter<=176);
  else assert(ncounter<=96);
  m_ncounter = ncounter;
  m_status |= (ncounter << Ncounter_Index);
}


void TofHitStatus::setNEast( unsigned int neast ) {
  if( m_barrel ) assert(neast<=176);
  else assert(neast<=96);
  m_neast = neast;
  m_status |= (neast << Neast_Index);
}


void TofHitStatus::setNWest( unsigned int nwest ) {
  if( m_barrel ) assert(nwest<=176);
  else assert(nwest<=96);
  m_nwest = nwest;
  m_status |= (nwest << Nwest_Index);
}


void TofHitStatus::setMRPC( bool mrpc ) {
  m_mrpc = mrpc;
  if( mrpc ) { m_status |= MRPC_Mask; }
  else { m_status &= (~MRPC_Mask); }
}


void TofHitStatus::setBarrelCluster( unsigned int layer ) {
  TofHitStatus::setRaw( false );
  TofHitStatus::setCluster( true );
  TofHitStatus::setBarrel( true );
  TofHitStatus::setOverflow( false );
  TofHitStatus::setMultihit( false );

  if( layer == 3 ) {
    TofHitStatus::setReadout( false );
    TofHitStatus::setCounter( false );
    TofHitStatus::setEast( false );
    TofHitStatus::setLayer( layer );
    TofHitStatus::setNCounter( 2 );
    TofHitStatus::setNEast( 2 );
    TofHitStatus::setNWest( 2 );
  }
  else if( layer == 1 || layer == 2 ) {
    TofHitStatus::setReadout( false );
    TofHitStatus::setCounter( true );
    TofHitStatus::setEast( false );
    TofHitStatus::setLayer( layer );
    TofHitStatus::setNCounter( 1 );
    TofHitStatus::setNEast( 1 );
    TofHitStatus::setNWest( 1 );
  }
  else if( layer == 11 || layer == 12 || layer == 21 || layer == 22 ) {
    TofHitStatus::setReadout( true );
    TofHitStatus::setCounter( true );
    TofHitStatus::setNCounter( 0 );
    if( layer%10 == 1 ) {
      TofHitStatus::setEast( true );
      TofHitStatus::setNEast( 1 );
      TofHitStatus::setNWest( 0 );
    }
    else {
      TofHitStatus::setEast( false );
      TofHitStatus::setNEast( 0 );
      TofHitStatus::setNWest( 1 );
    }
    if( static_cast<int>(layer/10) == 1 ) {
      TofHitStatus::setLayer( 1 );
    }
    else {
      TofHitStatus::setLayer( 2 );
    }
  }
  else{
    cout << "TofRec  TofHitStatus::setBarrelCounter  layer = " << layer << endl;
    return;
  }
  TofHitStatus::setMRPC( false );

  return;
}


void TofHitStatus::setBarrelCounter( unsigned int layer ) {

  TofHitStatus::setRaw( false );
  TofHitStatus::setCounter( true );
  TofHitStatus::setCluster( false );
  TofHitStatus::setBarrel( true );
  TofHitStatus::setOverflow( false );
  TofHitStatus::setMultihit( false );

  if( layer == 1 || layer == 2 ) {
    TofHitStatus::setReadout( false );
    TofHitStatus::setEast( false );
    TofHitStatus::setLayer( layer );
    TofHitStatus::setNCounter( 1 );
    TofHitStatus::setNEast( 1 );
    TofHitStatus::setNWest( 1 );
  }
  if( layer == 11 || layer == 12 || layer == 21 || layer == 22 ) {
    TofHitStatus::setReadout( true );
    TofHitStatus::setNCounter( 0 );
    if( layer%10 == 1 ) {
      TofHitStatus::setEast( true );
      TofHitStatus::setNEast( 1 );
      TofHitStatus::setNWest( 0 );
    }
    else {
      TofHitStatus::setEast( false );
      TofHitStatus::setNEast( 0 );
      TofHitStatus::setNWest( 1 );
    }
    if( static_cast<int>(layer/10) == 1 ) {
      TofHitStatus::setLayer( 1 );
    }
    else {
      TofHitStatus::setLayer( 2 );
    }
  }
  TofHitStatus::setMRPC( false );

  return;
}


void TofHitStatus::setBarrelReadout( unsigned int layer, bool east ) {
  TofHitStatus::setRaw( false );
  TofHitStatus::setReadout( true );
  TofHitStatus::setCounter( false );
  TofHitStatus::setCluster( false );
  TofHitStatus::setBarrel( true );
  TofHitStatus::setEast( east );
  TofHitStatus::setLayer( layer );
  TofHitStatus::setOverflow( false );
  TofHitStatus::setMultihit( false );
  TofHitStatus::setNCounter( 0 );
  if( east ){
    TofHitStatus::setNEast( 1 );
    TofHitStatus::setNWest( 0 );
  }
  else{
    TofHitStatus::setNEast( 0 );
    TofHitStatus::setNWest( 1 );
  }
  TofHitStatus::setMRPC( false );
  return;
}


void TofHitStatus::setBarrelRaw( unsigned int layer, bool east ) {
  TofHitStatus::setRaw( true );
  TofHitStatus::setReadout( true );
  TofHitStatus::setCounter( false );
  TofHitStatus::setCluster( false );
  TofHitStatus::setBarrel( true );
  TofHitStatus::setEast( east );
  TofHitStatus::setLayer( layer );
  TofHitStatus::setOverflow( false );
  TofHitStatus::setMultihit( false );
  TofHitStatus::setNCounter( 0 );
  if( east ){
    TofHitStatus::setNEast( 1 );
    TofHitStatus::setNWest( 0 );
  }
  else{
    TofHitStatus::setNEast( 0 );
    TofHitStatus::setNWest( 1 );
  }
  TofHitStatus::setMRPC( false );
  return;
}


void TofHitStatus::setEndcapCluster( bool east ) {
  TofHitStatus::setRaw( false );
  TofHitStatus::setReadout( true );
  TofHitStatus::setCounter( true );
  TofHitStatus::setCluster( true );
  TofHitStatus::setBarrel( false );
  TofHitStatus::setEast( east );
  TofHitStatus::setLayer( 1 );
  TofHitStatus::setOverflow( false );
  TofHitStatus::setMultihit( false );
  TofHitStatus::setNCounter( 1 );
  TofHitStatus::setNEast( 1 );
  TofHitStatus::setNWest( 0 );
  TofHitStatus::setMRPC( false );
  return;
}


void TofHitStatus::setEndcapRaw( bool east ) {
  TofHitStatus::setRaw( true );
  TofHitStatus::setReadout( true );
  TofHitStatus::setCounter( false );
  TofHitStatus::setCluster( false );
  TofHitStatus::setBarrel( false );
  TofHitStatus::setEast( east );
  TofHitStatus::setLayer( 1 );
  TofHitStatus::setOverflow( false );
  TofHitStatus::setMultihit( false );
  TofHitStatus::setNCounter( 1 );
  TofHitStatus::setNEast( 1 );
  TofHitStatus::setNWest( 0 );
  TofHitStatus::setMRPC( false );
  return;
}


void TofHitStatus::setMRPCReadout( bool east ) {
  TofHitStatus::setRaw( false );
  TofHitStatus::setReadout( true );
  TofHitStatus::setCounter( false );
  TofHitStatus::setCluster( false );
  TofHitStatus::setBarrel( false );
  TofHitStatus::setEast( east );
  TofHitStatus::setLayer( 1 );
  TofHitStatus::setOverflow( false );
  TofHitStatus::setMultihit( false );
  TofHitStatus::setNCounter( 0 );
  if( east ) {
    TofHitStatus::setNEast( 1 );
    TofHitStatus::setNWest( 0 );
  }
  else {
    TofHitStatus::setNEast( 0 );
    TofHitStatus::setNWest( 1 );
  }
  TofHitStatus::setMRPC( true );
  return;
}


void TofHitStatus::setMRPCCluster( bool cluster, bool east ) {
  TofHitStatus::setRaw( false );
  if( cluster ) {
    TofHitStatus::setReadout( false );
  }
  else {
    TofHitStatus::setReadout( true );
  }
  TofHitStatus::setCounter( true );
  TofHitStatus::setCluster( true );
  TofHitStatus::setBarrel( false );
  TofHitStatus::setEast( east );
  TofHitStatus::setLayer( 1 );
  TofHitStatus::setOverflow( false );
  TofHitStatus::setMultihit( false );
  if( cluster ) {
    TofHitStatus::setNCounter( 1 );
    TofHitStatus::setNEast( 1 );
    TofHitStatus::setNWest( 1 );
  }
  else {
    TofHitStatus::setNCounter( 1 );
    if( east ) {
      TofHitStatus::setNEast( 1 );
      TofHitStatus::setNWest( 0 );
    }
    else {
      TofHitStatus::setNEast( 0 );
      TofHitStatus::setNWest( 1 );
    }
  }
  TofHitStatus::setMRPC( true );
  return;
}


void TofHitStatus::setNoHit() {
  TofHitStatus::setRaw( true );
  TofHitStatus::setReadout( false );
  TofHitStatus::setCounter( false );
  TofHitStatus::setCluster( false );
  TofHitStatus::setBarrel( false );
  TofHitStatus::setEast( false );
  TofHitStatus::setLayer( 0 );
  TofHitStatus::setOverflow( false );
  TofHitStatus::setMultihit( false );
  TofHitStatus::setNCounter( 0 );
  TofHitStatus::setNEast( 0 );
  TofHitStatus::setNWest( 0 );
  TofHitStatus::setMRPC( false );
  return;
}
