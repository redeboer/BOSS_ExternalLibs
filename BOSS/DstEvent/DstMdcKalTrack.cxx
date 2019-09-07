/*
* @class : MdcTrack                                                            
*                                                                              
*  this class models "Kalman Track" from Mdc 
*                                                                              
* ********************************************************/

#include "DstEvent/DstMdcKalTrack.h"

enum DstMdcKalTrack::PidType DstMdcKalTrack::m_pidType = pion;

DstMdcKalTrack::DstMdcKalTrack() 
{   
  HepVector za(5,0);
  HepSymMatrix zea(5,0);
  HepPoint3D poca(0,0,0);
  m_trackId = -1;
  for(int pid=0; pid<5; pid++) {
    m_zhelixs.push_back(za);
    m_zerrors.push_back(zea);
    m_fhelixs.push_back(za);
    m_ferrors.push_back(zea);
    m_pocas.push_back(poca);   
    m_mass[pid] = 1;
    m_charge[pid] = 1;
    m_stat[0][pid] = 0;
    m_stat[1][pid] = 0;

    m_ndf[0][pid] = 0;
    m_ndf[1][pid] = 0;
    m_nster[0][pid] = 0;
    m_nster[1][pid] = 0;
    m_nlayer[pid] = 0;
    m_firstLayer[0][pid] = 0;
    m_firstLayer[1][pid] = 0;
    m_lastLayer[0][pid] = 0;
    m_lastLayer[1][pid] = 0;
    m_pxy[pid] = 0;
    m_px[pid] = 0;
    m_py[pid] = 0;
    m_pz[pid] = 0;
    m_p[pid] = 0;
    m_theta[pid] = 0;
    m_phi[pid] = 0;
    m_x[pid] = 0;
    m_y[pid] = 0;
    m_z[pid] = 0;
    m_r[pid] = 0;
    m_stat[0][pid] = 0;  	
    m_stat[1][pid] = 0;
    m_chisq[0][pid] = 0;
    m_chisq[1][pid] = 0;  
    m_ndf[0][pid] = 0;     
    m_ndf[1][pid] = 0;
    
  }
}


DstMdcKalTrack::DstMdcKalTrack( const DstMdcKalTrack& other )
  :m_trackId(other.m_trackId),
   m_pocas(other.m_pocas),   
   m_zhelixs(other.m_zhelixs),
   m_zerrors(other.m_zerrors),
   m_fhelixs(other.m_fhelixs),
   m_ferrors(other.m_ferrors)
{
  for(int i=0;i<5;i++){
       m_charge[i]        = other.m_charge[i];
       m_stat[0][i]       = other.m_stat[0][i];
       m_stat[1][i]       = other.m_stat[1][i];
       m_nster[0][i]      = other.m_nster[0][i];
       m_nster[1][i]      = other.m_nster[1][i];
       m_nlayer[i]        = other.m_nlayer[i];
       m_firstLayer[0][i] = other.m_firstLayer[0][i];
       m_firstLayer[1][i] = other.m_firstLayer[1][i];
       m_lastLayer[0][i]  = other.m_lastLayer[0][i];
       m_lastLayer[1][i]  = other.m_lastLayer[1][i]; 
       m_mass[i]       = other.m_mass[i];
 
       m_pxy[i]        = other.m_pxy[i];
       m_px[i]         = other.m_px[i];
       m_py[i]         = other.m_py[i];
       m_pz[i]         = other.m_pz[i];
       m_p[i]          = other.m_p[i];
       m_theta[i]      = other.m_theta[i];
       m_phi[i]        = other.m_phi[i];
       m_x[i]          = other.m_x[i];
       m_y[i]          = other.m_y[i];
       m_z[i]          = other.m_z[i];
       m_r[i]          = other.m_r[i];
       m_chisq[0][i]   = other.m_chisq[0][i];  
       m_chisq[1][i]   = other.m_chisq[1][i];
       m_ndf[0][i]     = other.m_ndf[0][i];   
       m_ndf[1][i]     = other.m_ndf[1][i];
       m_nhits[i]      = other.m_nhits[i];
  }
}

DstMdcKalTrack& DstMdcKalTrack::operator=(const DstMdcKalTrack& other){
  if(&other != this){
     m_trackId = other.m_trackId;
     m_pocas   = other.m_pocas;
     m_zhelixs = other.m_zhelixs;
     m_zerrors = other.m_zerrors;
     m_fhelixs = other.m_fhelixs;
     m_ferrors = other.m_ferrors;
       for(int i=0;i<5;i++){
	 m_charge[i]     = other.m_charge[i];
	 m_stat[0][i]       = other.m_stat[0][i];
         m_stat[1][i]       = other.m_stat[1][i];
	 m_nster[0][i]      = other.m_nster[0][i];
         m_nster[1][i]      = other.m_nster[1][i];
         m_nlayer[i]        = other.m_nlayer[i];
	 m_firstLayer[0][i] = other.m_firstLayer[0][i];
         m_firstLayer[1][i] = other.m_firstLayer[1][i];
	 m_lastLayer[0][i]  = other.m_lastLayer[0][i];
         m_lastLayer[1][i]  = other.m_lastLayer[1][i];
	 m_mass[i]       = other.m_mass[i];
	 m_chisq[0][i]      = other.m_chisq[0][i];
	 m_chisq[1][i]      = other.m_chisq[1][i];
	 m_pxy[i]        = other.m_pxy[i];
	 m_px[i]         = other.m_px[i];
	 m_py[i]         = other.m_py[i];
	 m_pz[i]         = other.m_pz[i];
	 m_p[i]          = other.m_p[i];
	 m_theta[i]      = other.m_theta[i];
	 m_phi[i]        = other.m_phi[i];
	 m_x[i]          = other.m_x[i];
	 m_y[i]          = other.m_y[i];
	 m_z[i]          = other.m_z[i];
	 m_r[i]          = other.m_r[i];
	 //m_chi2[i]       = other.m_chi2[i];
	 m_ndf[0][i]        = other.m_ndf[0][i];
         m_ndf[1][i]        = other.m_ndf[1][i];
	 m_nhits[i]      = other.m_nhits[i];
       }
  }
  return *this;
}


DstMdcKalTrack::~DstMdcKalTrack() {
   
   }


const HepLorentzVector DstMdcKalTrack::p4() const{
  const HepLorentzVector p_LV( px(), py(), pz(), sqrt(pow(mass(),2)+pow(p(),2)) ); 
  return p_LV; 
}


const HepLorentzVector DstMdcKalTrack::p4(double v_mass) const{
  const HepLorentzVector p_LV( px(), py(), pz(), sqrt(pow(v_mass,2)+pow(p(),2)) ); 
  return p_LV; 
}

const Hep3Vector DstMdcKalTrack::p3() const {
	const Hep3Vector p3( px(), py(), pz() ); 
	return p3; 
}

const HepPoint3D DstMdcKalTrack::x3() const {
	return HepPoint3D(m_x[m_pidType],m_y[m_pidType],m_z[m_pidType]);
}
