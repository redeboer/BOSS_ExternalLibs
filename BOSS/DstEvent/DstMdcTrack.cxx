
#include "DstEvent/DstMdcTrack.h"

  DstMdcTrack::DstMdcTrack()
  : m_trackId(0),      
    m_charge(0),  
    m_poca3D(0,0,0),    
    m_a(5,0),      
    m_Ea(5,0),
    m_stat(0),
    m_chi2(0),
    m_ndof(0),    
    m_nster(0),    
    m_nlayer(0),    
    m_firstLayer(0),    
    m_lastLayer(42), 
    m_pxy(0), 
    m_px(0),
    m_py(0),
    m_pz(0),
    m_p(0),
    m_theta(0),      
    m_phi(0),   
    m_x(0),     
    m_y(0),   
    m_z(0),       
    m_r(0)
  {
    for(int i=0;i<5;i++)  m_helix[i] = 0;
    for(int j=0;j<15;j++) m_err[j]   = 999; 
    for(int k=0;k<3;k++)  m_poca[k]  = 0;    
  }

 DstMdcTrack::DstMdcTrack( const DstMdcTrack& other)
 : m_trackId(other.m_trackId),
   m_charge(other.m_charge),
   m_poca3D(other.m_poca3D),
   m_a(other.m_a),
   m_Ea(other.m_Ea),
   m_stat(other.m_stat),
   m_chi2(other.m_chi2),
   m_ndof(other.m_ndof),
   m_nster(other.m_nster),
   m_nlayer(other.m_nlayer),
   m_firstLayer(other.m_firstLayer),
   m_lastLayer(other.m_lastLayer),
   m_pxy(other.m_pxy),
   m_px(other.m_px),
   m_py(other.m_py),
   m_pz(other.m_pz),
   m_p(other.m_p),
   m_theta(other.m_theta),
   m_phi(other.m_phi),
   m_x(other.m_x),
   m_y(other.m_y),
   m_z(other.m_z),
   m_r(other.m_r)
   { 
     for(int i=0;i<5;i++)  m_helix[i] = (other.m_helix)[i];
     for(int j=0;j<15;j++) m_err[j]   = (other.m_err)[j];
     for(int k=0;k<3;k++)  m_poca[k]  = (other.m_poca)[k];
   }


  DstMdcTrack& DstMdcTrack::operator=(const DstMdcTrack& h){
    if( &h != this ){
      m_trackId    = h.m_trackId;
      m_charge     = h.m_charge;
      m_poca3D     = h.m_poca3D;
      m_a          = h.m_a;
      m_Ea         = h.m_Ea;
      m_stat       = h.m_stat;
      m_chi2       = h.m_chi2;
      m_ndof       = h.m_ndof;
      m_nster      = h.m_nster;
      m_nlayer     = h.m_nlayer;
      m_firstLayer = h.m_firstLayer;
      m_lastLayer  = h.m_lastLayer;
      m_pxy        = h.m_pxy;
      m_px         = h.m_px;
      m_py         = h.m_py;
      m_pz         = h.m_pz;
      m_p          = h.m_p;
      m_theta      = h.m_theta;
      m_phi        = h.m_phi;
      m_x          = h.m_x;
      m_y          = h.m_y;
      m_z          = h.m_z;
      m_r          = h.m_r;
    
      for(int i=0;i<5;i++)  m_helix[i] = (h.m_helix)[i];
      for(int j=0;j<15;j++) m_err[j]   = (h.m_err)[j];
      for(int k=0;k<3;k++)  m_poca[k]  = (h.m_poca)[k];
    }
    return *this;
  }

  void DstMdcTrack::setHelix(double helix[5]) {   
    for(int i=0; i<5; i++){
      m_helix[i] = helix[i];
      m_a[i] = helix[i]; 
    }
  }

  void DstMdcTrack::setPoca(double poca[3]){
    for(int i=0; i<3; i++){
      m_poca[i] = poca[i];
      m_poca3D[i] = poca[i];
    }
  }  

  void DstMdcTrack::setError( double err[15]) {	       
    for (int i1=0; i1<15; i1++) {
      m_err[i1] = err[i1];
    }
    for(int i=0, k=0; i<5; i++) {
      for(int j=i; j<5; j++) {
	m_Ea[i][j] = m_err[k++];
	m_Ea[j][i] = m_Ea[i][j];
      }
    }
  }	     


  void  DstMdcTrack::setHelix(const HepVector& helix){
    for(int i=0; i<5; i++){        
      m_helix[i] = helix[i];
    }
    m_a = helix; 
  }
  
  void  DstMdcTrack::setPoca(const HepPoint3D& poca){
    for(int i=0; i<3; i++){
      m_poca[i] = poca[i];
    }
    m_poca3D = poca;
  }
  
  void  DstMdcTrack::setError(const HepSymMatrix& err){
    int k = 0;
    for (int i=0; i<5; i++){
      for (int j=i; j<5; j++)
      {
	m_err[k] = err[i][j];
	k++;
      }
    }
    m_Ea = err;
  }

  const HepVector DstMdcTrack::helix() const{
    return m_a;
  }
  
  const HepSymMatrix DstMdcTrack::err() const{
    /*
    HepSymMatrix ea(5);
    for(int i = 0, k = 0; i < 5; i++) {
      for(int j = 0; j <= i; j++) {
	ea[i][j] = m_err[k++];
	ea[j][i] = ea[i][j];
      }
    }
    return ea;
   */
    return m_Ea;
  }
	
  const HepLorentzVector DstMdcTrack::p4(const double mass) const{
    return HepLorentzVector(p3(),sqrt(p3()*p3()+mass*mass)); 
  }
	  
  
