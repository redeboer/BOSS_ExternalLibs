#include "DstEvent/DstTofTrack.h"
#include "DstEvent/TofHitStatus.h"
/* 
   the following code maybe cause 
   DstEvent and RecEvent use each other
   to avoid conflict, comment it

#include "TofRecEvent/RecBTofTrack.h"
#include "TofRecEvent/RecETofTrack.h"

class RecBTofTrack ;
class RecETofTrack ;

*/

DstTofTrack::DstTofTrack() {

  m_tofTrackID = -1;
  m_trackID = -1;
  m_tofID   = -1;
  m_status  = -1;
  m_path    = 9999.0;
  m_zrhit   = 9999.0;
  m_ph      = 9999.0;
  m_tof     = 9999.0;
  m_beta    = 9999.0;
  for( int i=0; i<6; i++ ) {
    m_toffset[i] = 9999.0;
    m_sigma[i]   = 9999.0;
  }
  m_quality = 10;
  m_t0      = 0.0;
  m_errt0   = 0.0;

  m_errz    = 9999.0;
  m_phi     = 9999.0;
  m_errphi  = 9999.0;
  m_energy  = 9999.0;
  m_errenergy = 9999.0;

}


int DstTofTrack::tofID() {
  if( m_tofID < 0 ) return m_tofID;
  if( TofHitStatus::is_mrpc( m_status ) ) {
    return static_cast<int>( m_tofID/12 );
  }
  else {
    return m_tofID;
  }
  return -1;
}


int DstTofTrack::strip() {
  if( m_tofID<0 ) return -1;
  if( TofHitStatus::is_mrpc( m_status ) ) {
    return static_cast<int>( m_tofID%12 );
  }
  return -1;
}


/*
DstTofTrack& 
DstTofTrack::operator=(const RecBTofTrack& source){

  m_tofTrackID = source.m_tofTrackID;
  m_trackID = source.m_trackID;
  m_tofID   = source.m_tofID;
  m_status  = source.m_status;
  m_path    = source.m_path;
  m_zrhit   = source.m_zrhit;
  m_ph      = source.m_ph;
  m_tof     = source.m_tof;
  m_beta    = source.m_beta;
  for( int i=0; i<6; i++ ) {
    m_toffset[i] = source.m_toffset[i];
    m_sigma[i]   = source.m_sigma[i];
  }
  m_quality = source.m_quality;
  m_t0      = source.m_t0;
  m_errt0   = source.m_errt0;

  m_errz    = source.m_errz;
  m_phi     = source.m_phi;
  m_errphi  = source.m_errphi;
  m_errenergy = source.m_errenergy;

  return *this;
}


void
DstTofTrack::copy(const RecBTofTrack* orig) {

  m_tofTrackID = orig->m_tofTrackID;
  m_trackID = orig->m_trackID;
  m_tofID   = orig->m_tofID;
  m_status  = orig->m_status;
  m_path    = orig->m_path;
  m_zrhit   = orig->m_zrhit;
  m_ph      = orig->m_ph;
  m_tof     = orig->m_tof;
  m_beta    = orig->m_beta;
  for( int i=0; i<6; i++ ) {
    m_toffset[i] = orig->m_toffset[i];
    m_sigma[i]   = orig->m_sigma[i];
  }
  m_quality = orig->m_quality;
  m_t0      = orig->m_t0;
  m_errt0   = orig->m_errt0;

  m_errz    = orig->m_errz;
  m_phi     = orig->m_phi;
  m_errphi  = orig->m_errphi;
  m_errenergy = orig->m_errenergy;
}


DstTofTrack::DstTofTrack(const RecBTofTrack& source) {

  m_tofTrackID = source.m_tofTrackID;
  m_trackID = source.m_trackID;
  m_tofID   = source.m_tofID;
  m_status  = source.m_status;
  m_path    = source.m_path;
  m_zrhit   = source.m_zrhit;
  m_ph      = source.m_ph;
  m_tof     = source.m_tof;
  m_beta    = source.m_beta;
  for( int i=0; i<6; i++ ) {
    m_toffset[i] = source.m_toffset[i];
    m_sigma[i]   = source.m_sigma[i];
  }
  m_quality = source.m_quality;
  m_t0      = source.m_t0;
  m_errt0   = source.m_errt0;

  m_errz    = source.m_errz;
  m_phi     = source.m_phi;
  m_errphi  = source.m_errphi;
  m_errenergy = source.m_errenergy;
}


DstTofTrack& 
DstTofTrack::operator=(const RecETofTrack& source){

  m_tofTrackID = source.m_tofTrackID;
  m_trackID = source.m_trackID;
  m_tofID   = source.m_tofID;
  m_status  = source.m_status;
  m_path    = source.m_path;
  m_zrhit   = source.m_zrhit;
  m_ph      = source.m_ph;
  m_tof     = source.m_tof;
  m_beta    = source.m_beta;
  for( int i=0; i<6; i++ ) {
    m_toffset[i] = source.m_toffset[i];
    m_sigma[i]   = source.m_sigma[i];
  }
  m_quality = source.m_quality;
  m_t0      = source.m_t0;
  m_errt0   = source.m_errt0;

  m_errz    = source.m_errz;
  m_phi     = source.m_phi;
  m_errphi  = source.m_errphi;
  m_errenergy = source.m_errenergy;

  return *this;
}


void
DstTofTrack::copy(const RecETofTrack* orig) {

  m_tofTrackID = orig->m_tofTrackID;
  m_trackID = orig->m_trackID;
  m_tofID   = orig->m_tofID;
  m_status  = orig->m_status;
  m_path    = orig->m_path;
  m_zrhit   = orig->m_zrhit;
  m_ph      = orig->m_ph;
  m_tof     = orig->m_tof;
  m_beta    = orig->m_beta;
  for( int i=0; i<6; i++ ) {
    m_toffset[i] = orig->m_toffset[i];
    m_sigma[i]   = orig->m_sigma[i];
  }
  m_quality = orig->m_quality;
  m_t0      = orig->m_t0;
  m_errt0   = orig->m_errt0;

  m_errz    = orig->m_errz;
  m_phi     = orig->m_phi;
  m_errphi  = orig->m_errphi;
  m_errenergy = orig->m_errenergy;
}


DstTofTrack::DstTofTrack(const RecETofTrack& source) {

  m_tofTrackID = source.m_tofTrackID;
  m_trackID = source.m_trackID;
  m_tofID   = source.m_tofID;
  m_status  = source.m_status;
  m_path    = source.m_path;
  m_zrhit   = source.m_zrhit;
  m_ph      = source.m_ph;
  m_tof     = source.m_tof;
  m_beta    = source.m_beta;
  for( int i=0; i<6; i++ ) {
    m_toffset[i] = source.m_toffset[i];
    m_sigma[i]   = source.m_sigma[i];
  }
  m_quality = source.m_quality;
  m_t0      = source.m_t0;
  m_errt0   = source.m_errt0;

  m_errz    = source.m_errz;
  m_phi     = source.m_phi;
  m_errphi  = source.m_errphi;
  m_errenergy = source.m_errenergy;
}
*/
