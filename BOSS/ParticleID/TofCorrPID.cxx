#include <cmath>
#include "TMath.h"

#include "ParticleID/TofCorrPID.h"

#ifndef BEAN
#include "EvtRecEvent/EvtRecTrack.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "ExtEvent/RecExtTrack.h"
#include "TofRecEvent/RecTofTrack.h"
#include "DstEvent/TofHitStatus.h"
#else
#include "TofHitStatus.h"
#endif
#include <fstream>


TofCorrPID * TofCorrPID::m_pointer = 0;
TofCorrPID * TofCorrPID::instance() {
   if(!m_pointer) m_pointer = new TofCorrPID();
   return m_pointer;
}

TofCorrPID::TofCorrPID():ParticleIDBase() {
   m_runBegin = 0;
   m_runEnd   = 0;
}

void TofCorrPID::init() {

   for(int i = 0; i < 5; i++) {
      m_chi[i]    = -100.0;
      m_prob[i]   = -1.0;
      m_sigma[i]  = 10.0;
      m_offset[i] = -1000.0;
   }
   m_chimin = -99.0;
   m_chimax = 99.0;
   m_pdfmin = 99.;
   m_ndof = 0;

   m_ipmt   = -1;
   for( unsigned int i=0; i<5; i++ ) {
     for( unsigned int j=0; j<7; j++ ) {
       m_dt[i][j]      = -1000.0;
       m_dtCorr[i][j]  = -1000.0;
       m_sigCorr[i][j] = 10.0;
       m_chiCorr[i][j] = 100.0;
     }
   }

   int run = getRunNo();
   if( !( abs(run)>=m_runBegin && abs(run)<=m_runEnd ) ) {
     inputParameter( getRunNo() );
   }

   return;
}


void TofCorrPID::calculate() {
  if(particleIDCalculation() == 0) m_ndof=1;
}


int TofCorrPID::particleIDCalculation() {
   int irc=-1;

   EvtRecTrack* recTrk = PidTrk();
   if(!(recTrk->isMdcTrackValid())) return irc;
   if(!(recTrk->isMdcKalTrackValid())) return irc;
   if(!(recTrk->isExtTrackValid())) return irc;
   if(!(recTrk->isTofTrackValid())) return irc;

#ifndef BEAN
   SmartRefVector<RecTofTrack> tofTrk = recTrk->tofTrack();
   SmartRefVector<RecTofTrack>::iterator it;
#else
   const std::vector<TTofTrack* >& tofTrk = recTrk->tofTrack();
   std::vector<TTofTrack* >::const_iterator it;
#endif

   RecMdcTrack* mdcTrk = recTrk->mdcTrack();
   int charge = mdcTrk->charge();

   double p[5], betaGamma[5];
   RecMdcKalTrack* kalTrk = recTrk->mdcKalTrack();
   for( int i=0; i<5; i++ ) {
     if( i==0 ) {
       kalTrk->setPidType(RecMdcKalTrack::electron);
     }
     else if( i==1 ) {
       kalTrk->setPidType(RecMdcKalTrack::muon);
     }
     else if( i==2 ) {
       kalTrk->setPidType(RecMdcKalTrack::pion);
     }
     else if( i==3 ) {
       kalTrk->setPidType(RecMdcKalTrack::kaon);
     }
     else if( i==4 ) {
       kalTrk->setPidType(RecMdcKalTrack::proton);
     }
#ifndef BEAN
     HepLorentzVector ptrk = kalTrk->p4();
#else
     HepLorentzVector ptrk = kalTrk->p4( xmass(i) );
#endif
     p[i] = ptrk.rho();
     betaGamma[i] = p[i]/xmass(i);
   }

   double zrhit[2];
   RecExtTrack* extTrk = recTrk->extTrack();
   zrhit[0] = extTrk->tof1Position().z();
   zrhit[1] = extTrk->tof2Position().z();

   int tofid[2] = { -9, -9 };

   m_ipmt = -1;
   bool readFile = false;
   bool signal[2] = { false, false };
   TofHitStatus *hitst = new TofHitStatus;
   for( it = tofTrk.begin(); it!=tofTrk.end(); it++ ) {
     unsigned int st = (*it)->status();
     hitst->setStatus(st);
     if( hitst->is_raw() ) return irc;  // TOF no hit
     bool barrel  = hitst->is_barrel();
     bool ismrpc  = hitst->is_mrpc();
     if( !barrel && !ismrpc ) { zrhit[0] = extTrk->tof1Position().rho(); }
     bool readout = hitst->is_readout();
     bool counter = hitst->is_counter();
     bool cluster = hitst->is_cluster();
     int  layer   = hitst->layer();
     tofid[layer-1] = (*it)->tofID();
     bool east      = hitst->is_east();
     double tof     = (*it)->tof();
     unsigned int ipmt = 0;
     if( readout ) {
       // barrel: 0:inner east, 1:inner west, 2:outer east, 3: outer west
       // endcap: 7:east endcap, 8:west endcap
       if( barrel ) { ipmt = ( ( st & 0xC0 ) >> 5 ) + ( ( ( st ^ 0x20 ) & 0x20 ) >> 5 ) - 2; }
       else {
	 if( !ismrpc ) {
	   if( tofid[0]<=47 ) { ipmt = 7; }
	   else { ipmt = 8; }
	 }
	 else {
	   if( tofid[0]<=35 ) { ipmt = 7; }
	   else { ipmt = 8; }
	 }
       }

       for( unsigned int i=0; i<5; i++ ) {
	 double offset = (*it)->toffset(i);
	 double texp   = (*it)->texp(i);
	 if( texp<0.0 ) continue;
	 double dt = tof - offset - texp;
	 if( barrel ) {
	   m_dt[i][ipmt]      = dt;
	   m_dtCorr[i][ipmt]  = offsetTof( i, barrel, ipmt, betaGamma[i], charge, zrhit[layer-1], dt );
	   m_sigCorr[i][ipmt] = sigmaTof( i, charge, barrel, ipmt, &tofid[0], &zrhit[0], betaGamma[i] );
	   m_chiCorr[i][ipmt] = m_dtCorr[i][ipmt]/m_sigCorr[i][ipmt];
	 }
	 else {
	   if( !ismrpc ) {
	     m_dt[i][0]      = dt;
	     m_dtCorr[i][0]  = offsetTof( i, barrel, ipmt, betaGamma[i], charge, zrhit[layer-1], dt );
	     m_sigCorr[i][0] = sigmaTof( i, charge, barrel, ipmt, &tofid[0], &zrhit[0], betaGamma[i] );
	     m_chiCorr[i][0] = m_dtCorr[i][ipmt]/m_sigCorr[i][ipmt];
	   }
	   else {
	     m_dt[i][0]      = dt;
	     m_dtCorr[i][0]  = dt;
	     //	     m_sigCorr[i][0] = 0.065;
	     int strip = (*it)->strip();
	     double p0, p1;
	     if( strip<0 || strip>11 ) { m_sigCorr[i][0] = 0.065; }
	     else {
	       if( strip==0 )       { p0=0.16;     p1=0.0;      }
	       else if( strip==1 )  { p0=0.051094; p1=0.019467; }
	       else if( strip==2 )  { p0=0.056019; p1=0.012964; }
	       else if( strip==3 )  { p0=0.043901; p1=0.015933; }
	       else if( strip==4 )  { p0=0.049750; p1=0.010473; }
	       else if( strip==5 )  { p0=0.048345; p1=0.008545; }
	       else if( strip==6 )  { p0=0.046518; p1=0.009038; }
	       else if( strip==7 )  { p0=0.048803; p1=0.007251; }
	       else if( strip==8 )  { p0=0.047523; p1=0.008434; }
	       else if( strip==9 )  { p0=0.042187; p1=0.010307; }
	       else if( strip==10 ) { p0=0.050337; p1=0.005951; }
	       else if( strip==11 ) { p0=0.054740; p1=0.005629; }
	       if( p[i]<0.05 ) { m_sigCorr[i][0] = p0+p1/0.05; }
	       else { m_sigCorr[i][0] = p0+p1/p[i]; }
	     }
	     if( i==4 ) { m_sigCorr[i][0] = 1.5*m_sigCorr[i][0]; }
	     m_chiCorr[i][0] = m_dtCorr[i][0]/m_sigCorr[i][0];
	   }
	 }
       }
       if( counter && cluster ) {
	 m_ipmt = ipmt;
	 for( unsigned int i=0; i<5; i++ ) {
	   if( ((*it)->texp(i))>0.0 ) {
	     if( barrel ) {
	       m_offset[i] = m_dtCorr[i][ipmt];
	       m_sigma[i]  = m_sigCorr[i][ipmt];
	     }
	     else {
	       m_offset[i] = m_dtCorr[i][0];
	       m_sigma[i]  = m_sigCorr[i][0];
	     }
	   }
	 }
       }
     }
     else {
       if( counter ) {
	 ipmt = layer+3;
	 for( unsigned int i=0; i<5; i++ ) {
	   double offset = (*it)->toffset(i);
	   double texp   = (*it)->texp(i);
	   if( texp<0.0 ) continue;
	   double dt     = tof - offset - texp;
	   if( barrel ) {
	     m_dt[i][ipmt]      = dt;
	     m_dtCorr[i][ipmt]  = offsetTof( i, tofid[layer-1], zrhit[layer-1], betaGamma[i], charge, dt );
	     m_sigCorr[i][ipmt] = sigmaTof( i, charge, barrel, layer+3, &tofid[0], &zrhit[0], betaGamma[i] );
	     m_chiCorr[i][ipmt] = m_dtCorr[i][ipmt]/m_sigCorr[i][ipmt];
	   }
	   else {
	     if( ismrpc ) {
	       m_dt[i][0]      = dt;
	       m_dtCorr[i][0]  = dt;
	       //	       m_sigCorr[i][0] = 0.065;
	       int strip = (*it)->strip();
	       double p0, p1;
	       if( strip<0 || strip>11 ) { m_sigCorr[i][0] = 0.065; }
	       else {
		 if( strip==0 )       { p0=0.16;     p1=0.0;      }
		 else if( strip==1 )  { p0=0.051094; p1=0.019467; }
		 else if( strip==2 )  { p0=0.056019; p1=0.012964; }
		 else if( strip==3 )  { p0=0.043901; p1=0.015933; }
		 else if( strip==4 )  { p0=0.049750; p1=0.010473; }
		 else if( strip==5 )  { p0=0.048345; p1=0.008545; }
		 else if( strip==6 )  { p0=0.046518; p1=0.009038; }
		 else if( strip==7 )  { p0=0.048803; p1=0.007251; }
		 else if( strip==8 )  { p0=0.047523; p1=0.008434; }
		 else if( strip==9 )  { p0=0.042187; p1=0.010307; }
		 else if( strip==10 ) { p0=0.050337; p1=0.005951; }
		 else if( strip==11 ) { p0=0.054740; p1=0.005629; }
		 if( p[i]<0.05 ) { m_sigCorr[i][0] = p0+p1/0.05; }
		 else { m_sigCorr[i][0] = p0+p1/p[i]; }
	       }
	       if( i==4 ) { m_sigCorr[i][0] = 1.5*m_sigCorr[i][0]; }
	       m_chiCorr[i][0] = m_dtCorr[i][0]/m_sigCorr[i][0];
	     }
	     else {
	       cout << "ParticleID: TofCorr::particleIDCalculation: Endcap Scintillator TOF have more than one end!!!" << endl;
	     }
	   }
	 }
	 if( cluster ) {
	   m_ipmt = ipmt;
	   for( unsigned int i=0; i<5; i++ ) {
	     if( ((*it)->texp(i))>0.0 ) {
	       if( barrel ) {
		 m_offset[i] = m_dtCorr[i][ipmt];
		 m_sigma[i]  = m_sigCorr[i][ipmt];
	       }
	       else {
		 m_offset[i] = m_dtCorr[i][0];
		 m_sigma[i]  = m_sigCorr[i][0];
	       }
	     }
	   }
	 }
	 else {
	   signal[layer-1] = correlationCheck( ipmt );
	 }
       }
       else {
	 if( cluster ) {
	   ipmt = 6;
	   for( unsigned int i=0; i<5; i++ ) {
	     double offset = (*it)->toffset(i);
	     double texp   = (*it)->texp(i);
	     if( texp<0.0 ) continue;
	     double dt     = tof - offset - texp;
	     m_dt[i][ipmt]      = dt;
	     m_dtCorr[i][ipmt]  = offsetTof( i, tofid[0], tofid[1], zrhit[0], zrhit[1], betaGamma[i], charge, dt );
	     m_sigCorr[i][ipmt] =  sigmaTof( i, charge, barrel, ipmt, &tofid[0], &zrhit[0], betaGamma[i] );
	     m_chiCorr[i][ipmt] = m_dtCorr[i][ipmt]/m_sigCorr[i][ipmt];
	   }
	   if( signal[0] && signal[1] ) {
	     m_ipmt = 6;
	     for( unsigned int i=0; i<5; i++ ) {
	       m_offset[i] = m_dtCorr[i][ipmt];
	       m_sigma[i]  = m_sigCorr[i][ipmt];
	     }
	   }
	   else if( signal[0] && !signal[1] ) {
	     m_ipmt = 4;
	     for( unsigned int i=0; i<5; i++ ) {
	       m_offset[i] = m_dtCorr[i][4];
	       m_sigma[i]  = m_sigCorr[i][4];
	     }
	   }
	   else if( !signal[0] && signal[1] ) {
	     m_ipmt = 5;
	     for( unsigned int i=0; i<5; i++ ) {
	       m_offset[i] = m_dtCorr[i][5];
	       m_sigma[i]  = m_sigCorr[i][5];
	     }
	   }
	   else return irc;
	 }
       }
     }
   }
   delete hitst;

   double pdftemp = 0;
   for( unsigned int i=0; i<5; i++ ) {
     m_chi[i] = m_offset[i]/m_sigma[i];
     if( m_chi[i]<0. && m_chi[i]>m_chimin ) { m_chimin = m_chi[i]; }
     if( m_chi[i]>0. && m_chi[i]<m_chimax ) { m_chimax = m_chi[i]; }
     double ppp = pdfCalculate(m_chi[i],1);
     if( fabs(ppp) > pdftemp) { pdftemp = fabs(ppp); }
   }

   m_pdfmin = pdftemp;
   if( pdftemp < pdfCalculate(pdfMinSigmaCut(),1) ) return irc;
   if( ( m_chimin > -90.0 && (0-m_chimin)>chiMinCut() ) && ( m_chimax < 90.0 && m_chimax>chiMaxCut() ) ) return irc;
   for(int i = 0; i < 5; i++) {
      m_prob[i] = probCalculate(m_chi[i]*m_chi[i], 1);
   }

   irc = 0;
   return irc;
}


void TofCorrPID::inputParameter( int run ) {

  std::string filePath = path + "/share/TofCorrPID/";

  filePath = filePath + "jpsi2012";
  m_runBegin = 0;
  m_runEnd   = 80000;

  if( run>0 ) {
    filePath = filePath + "/data/";
  }
  else {
    filePath = filePath + "/mc/";
  }


  // weight from tof calibration
  std::string fileWeight = filePath + "calib_barrel_sigma.txt";
  ifstream inputWeight( fileWeight.c_str(), std::ios_base::in );
  if( !inputWeight ) {
    cout << "ParticleID::TofCorrPID: Can NOT open file: " << fileWeight << endl;
    exit(1);
  }

  for( unsigned int tofid=0; tofid<176; tofid++ ) {
    for( unsigned int readout=0; readout<3; readout++ ) {
      for( unsigned int p_i=0; p_i<5; p_i++ ) {
	inputWeight >> m_p_weight[tofid][readout][p_i];
      }
    }
  }
  //  cout << "finish read " << fileWeight << endl;

  // common item, from bunch size and bunch time
  std::string fileCommon = filePath + "calib_barrel_common.txt";
  ifstream inputCommon( fileCommon.c_str(), std::ios_base::in );
  if( !inputCommon ) {
    cout << "ParticleID::TofCorrPID: Can NOT open file: " << fileCommon << endl;
    exit(1);
  }
  inputCommon >> m_p_common;
  //  cout << "finish read " << fileCommon << endl;

  // endcap sigma
  std::string fileEcSigma = filePath + "calib_endcap_sigma.txt";
  ifstream inputEcSigma( fileEcSigma.c_str(), std::ios_base::in );
  if( !inputEcSigma ) {
    cout << "ParticleID::TofCorrPID: Can NOT open file: " << fileEcSigma << endl;
    exit(1);
  }

  for( unsigned int tofid=0; tofid<96; tofid++ ) {
    for( unsigned int p_i=0; p_i<3; p_i++ ) {
      inputEcSigma >> m_ec_sigma[tofid][p_i];
    }
  }
  //  cout << "finish read " << fileEcSigma << endl;

  // curve of betaGamma versus Q0
  std::string fileQ0BetaGamma = filePath + "curve_Q0_BetaGamma.txt";
  ifstream inputQ0BetaGamma( fileQ0BetaGamma.c_str(), std::ios_base::in );
  if( !inputQ0BetaGamma ) {
    cout << "ParticleID::TofCorrPID: Can NOT open file: " << fileQ0BetaGamma << endl;
    exit(1);
  }
  // barrel layer1 layer2 and endcap
  for( unsigned int layer=0; layer<3; layer++ ) {
    for( unsigned int ipar=0; ipar<5; ipar++ ) {
      inputQ0BetaGamma >> m_q0_bg[layer][ipar];
    }
  }
  //  cout << "finish read " << fileQ0BetaGamma << endl;

  // paramter of A and B
  std::string fileParAB = filePath + "parameter_A_B.txt";
  ifstream inputParAB( fileParAB.c_str(), std::ios_base::in );
  if( !inputParAB ) {
    cout << "ParticleID::TofCorrPID: Can NOT open file: " << fileParAB << endl;
    exit(1);
  }

  // Jpsi2012
  // 0: pion/kaon,  1: proton/anti-proton
  // 0: inner east, 1: inner west, 2: outer east, 3: outer west, 4: west endcap 
  // 0: plus,       1: minus
  // 0: parameter A, 1: parameter B
  for( unsigned int ispecies=0; ispecies<2; ispecies++ ) {
    for( unsigned int ipmt=0; ipmt<5; ipmt++ ) {
      for( unsigned int icharge=0; icharge<2; icharge++ ) {
	for( unsigned int iab=0; iab<2; iab++ ) {
	  for( unsigned int ipar=0; ipar<11; ipar++ ) {
	    inputParAB >> m_par_ab_12[ispecies][ipmt][icharge][iab][ipar];
	  }
	}
      }
    }
  }

  // sigma for pion, kaon and proton
  std::string fileSigma = filePath + "parameter_sigma.txt";
  ifstream inputSigma( fileSigma.c_str(), std::ios_base::in );
  if( !inputSigma ) {
    cout << "ParticleID::TofCorrPID: Can NOT open file: " << fileSigma << endl;
    exit(1);
  }
  // Jpsi2009  &  Jpsi2012
  // 0: pion,  1: kaon,  2: proton,  3: anti-proton
  // 0: inner east, 1: inner west, 2: outer east, 3: outer west
  // 4: inner layer, 5: outer layer, 6: double layer
  // 7: west endcap
  for( unsigned int ispecies=0; ispecies<4; ispecies++ ) {
    for( unsigned int ipmt=0; ipmt<8; ipmt++ ) {
      for( unsigned int ipar=0; ipar<12; ipar++ ) {
	inputSigma >> m_par_sigma[ispecies][ipmt][ipar];
      }
    }
  }

  // offset for low momentum proton and anti-proton
  std::string fileProtonOffset = filePath + "parameter_offset_proton.txt";
  ifstream inputProtonOffset( fileProtonOffset.c_str(), std::ios_base::in );
  if( !inputProtonOffset ) {
    cout << "ParticleID::TofCorrPID: Can NOT open file: " << fileProtonOffset << endl;
    exit(1);
  }

  // Jpsi2012
  // 0: proton,  1: anti-proton
  // 0: inner east, 1: inner west, 2: outer east, 3: outer west
  // 4: inner layer, 5: outer layer, 6: double layer
  // 7: west endcap
  for( unsigned int ispecies=0; ispecies<2; ispecies++ ) {
    for( unsigned int ipmt=0; ipmt<8; ipmt++ ) {
      for( unsigned int ipar=0; ipar<20; ipar++ ) {
	if( ipmt!=7 ) {
	  for( unsigned int jpar=0; jpar<46; jpar++ ) {
	    inputProtonOffset >> m_p_offset_12[ispecies][ipmt][ipar][jpar];
	  }
	}
	else {
	  for( unsigned int jpar=0; jpar<7; jpar++ ) {
	    inputProtonOffset >> m_p_offset_ec12[ispecies][ipar][jpar];
	  }
	}
      }
    }
  }
  //  cout << "finish read " << fileProtonOffset << endl;

  // sigma for low momentum proton and anti-proton
  std::string fileProtonSigma = filePath + "parameter_sigma_proton.txt";
  ifstream inputProtonSigma( fileProtonSigma.c_str(), std::ios_base::in );
  if( !inputProtonSigma ) {
    cout << "ParticleID::TofCorrPID: Can NOT open file: " << fileProtonSigma << endl;
    exit(1);
  }

  // Jpsi2012
  // 0: proton,  1: anti-proton
  // 0: inner east, 1: inner west, 2: outer east, 3: outer west
  // 4: inner layer, 5: outer layer, 6: double layer
  // 7: west endcap
  for( unsigned int ispecies=0; ispecies<2; ispecies++ ) {
    for( unsigned int ipmt=0; ipmt<8; ipmt++ ) {
      for( unsigned int ipar=0; ipar<20; ipar++ ) {
	if( ipmt!=7 ) {
	  for( unsigned int jpar=0; jpar<46; jpar++ ) {
	    inputProtonSigma >> m_p_sigma_12[ispecies][ipmt][ipar][jpar];
	  }
	}
	else {
	  for( unsigned int jpar=0; jpar<7; jpar++ ) {
	    inputProtonSigma >> m_p_sigma_ec12[ispecies][ipar][jpar];
	  }
	}
      }
    }
  }
  //  cout << "finish read " << fileProtonSigma << endl;

  return;
}


double TofCorrPID::offsetTof( unsigned int ispecies, bool barrel, unsigned int ipmt, double betaGamma, int charge, double zrhit, double dt ) {
  if( ispecies==0 || ispecies==1 ) { return dt; }

  double deltaT = -1000.0;
  if( ( ipmt>= 4 && barrel ) || ( ipmt!=7 && ipmt!=8 && !barrel ) || betaGamma<0.0 || abs(charge)!=1 || fabs(zrhit)>120.0 ) {
    cout << "Particle::TofCorrPID: offsetTof for single end: the input parameter are NOT correct! Please check them!" << endl;
    return deltaT;
  }
  unsigned int layer=0;
  if( barrel ) {
    if( ipmt==0 || ipmt==1 ) { layer=0; }
    else if( ipmt==2 || ipmt==3 ) { layer=1; }
  }
  else { layer=2; }
  double q0 = qCurveFunc( layer, betaGamma );

  unsigned int inumber=ipmt;
  if( !barrel ) { inumber=4; }

  double func[9];
  for( unsigned int i=0; i<9; i++ ) {
    func[i]=0.0;
  }

  double parA = 0.0;
  double parB = 0.0;

  // Jpsi2012
  func[0] = 1.0;
  for( unsigned int i=1; i<8; i++ ) {
    func[i] = func[i-1]*zrhit;
  }
  
  unsigned int iparticle=0;
  if( ispecies==2 || ispecies==3 ) { iparticle=0; }
  else if( ispecies==4 )           { iparticle=1; }
  unsigned int icharge=0;
  if( charge==1 )       { icharge=0; }
  else if( charge==-1 ) { icharge=1; }
  
  if( ispecies!=4 || betaGamma*xmass(4)>0.8 ) {
    for( unsigned int i=0; i<8; i++ ) {
      if( i<5 ) {
	parA += m_par_ab_12[iparticle][inumber][icharge][0][i]*func[i];
	parB += m_par_ab_12[iparticle][inumber][icharge][1][i]*func[i];
      }
      else if( i>=5 ) {
	parA += m_par_ab_12[iparticle][inumber][icharge][0][i+3]*func[i];
	parB += m_par_ab_12[iparticle][inumber][icharge][1][i+3]*func[i];
      }
    }
    for( unsigned int iab=0; iab<2; iab++ ) {
      func[8] = m_par_ab_12[iparticle][inumber][icharge][iab][5]*TMath::Gaus(zrhit,m_par_ab_12[iparticle][inumber][icharge][iab][6], m_par_ab_12[iparticle][inumber][icharge][iab][7]);
      if( iab==0 ) {
	parA += func[8];
      }
      else if( iab==1 ) {
	parB += func[8];
      }
    }  
  }

  double tcorr = parA + parB/sqrt(q0);

  // barrel TOF low momentum proton and anti-proton
  // Jpsi2012
  if( barrel && ispecies==4 && betaGamma*xmass(4)<0.8 ) {
    int    nzbin  = 46;
    double zbegin = -115.0;
    double zend   = 115.0;
    double zstep  = (zend-zbegin)/nzbin;
    
    double nbgbin  = 20.0;
    double bgbegin = 0.3;
    double bgend   = 0.8;
    double bgstep;
    bgstep = (bgend-bgbegin)/nbgbin;
    
    int izbin = static_cast<int>((zrhit-zbegin)/zstep);
    if( izbin<0 )           { izbin=0;       }
    else if( izbin>=nzbin ) { izbin=nzbin-1; }
    int ibgbin = static_cast<int>((betaGamma*xmass(4)-bgbegin)/bgstep);
    if( ibgbin<0 )            { ibgbin=0;        }
    else if( ibgbin>=nbgbin ) { ibgbin=nbgbin-1; }
    
    if( charge==1 ) {
      tcorr = m_p_offset_12[0][ipmt][ibgbin][izbin];
    }
    else {
      tcorr = m_p_offset_12[1][ipmt][ibgbin][izbin];
    }
  }
  else if( !barrel && ispecies==4 && betaGamma*xmass(4)<0.8 ) {
    int    nrbin  = 7;
    double rbegin = 55.0;
    double rend   = 83.0;
    double rstep  = (rend-rbegin)/nrbin;
    
    double nbgbin  = 20.0;
    double bgbegin = 0.3;
    double bgend   = 0.8;
    double bgstep;
    bgstep = (bgend-bgbegin)/nbgbin;
    
    int irbin = static_cast<int>((zrhit-rbegin)/rstep);
    if( irbin<0 )           { irbin=0;       }
    else if( irbin>=nrbin ) { irbin=nrbin-1; }
    int ibgbin = static_cast<int>((betaGamma*xmass(4)-bgbegin)/bgstep);
    if( ibgbin<0 )            { ibgbin=0;        }
    else if( ibgbin>=nbgbin ) { ibgbin=nbgbin-1; }
    
    if( charge==1 ) {
      tcorr = m_p_offset_ec12[0][ibgbin][irbin];
    }
    else {
      tcorr = m_p_offset_ec12[1][ibgbin][irbin];
    }
  }

  deltaT = dt - tcorr;

  return deltaT;
}


double TofCorrPID::offsetTof( unsigned int ispecies, int tofid, double zrhit, double betaGamma, int charge, double dt ) {
  if( ispecies==0 || ispecies==1 ) { return dt; }

  double deltaT = -1000.0;
  if( tofid<0 || tofid>=176 ) {
    cout << "Particle::TofCorrPID: offsetTof for single layer: the input parameter are NOT correct! Please check them!" << endl;
    exit(1);
  }
  int pmt[3] = { -9, -9, -9 };
  if( tofid>=0 && tofid<=87 ) {
    pmt[0] = 0;
    pmt[1] = 1;
    pmt[2] = 4;
  }
  else {
    pmt[0] = 2;
    pmt[1] = 3;
    pmt[2] = 5;
  }

  double sigmaCorr2  = m_p_common*m_p_common;
  double sigmaLeft   = bSigma( 0, tofid, zrhit );
  double sigmaLeft2  = sigmaLeft*sigmaLeft;
  double sigmaRight  = bSigma( 1, tofid, zrhit );
  double sigmaRight2 = sigmaRight*sigmaRight;

  double fraction = ( sigmaRight2 - sigmaCorr2 )/( sigmaLeft2 + sigmaRight2 - 2.0*sigmaCorr2);
  deltaT = fraction*m_dtCorr[ispecies][pmt[0]] + (1.0-fraction)*m_dtCorr[ispecies][pmt[1]];

  // Jpsi2012
  double tcorr = 0.0;
  unsigned int ipmt = 4;
  if( tofid>=88 && tofid<176 ) { ipmt = 5; }
  if( ispecies==4 && betaGamma*xmass(4)<0.8 ) {
    int    nzbin  = 46;
    double zbegin = -115.0;
    double zend   = 115.0;
    double zstep  = (zend-zbegin)/nzbin;
    
    double nbgbin  = 20.0;
    double bgbegin = 0.3;
    double bgend   = 0.8;
    double bgstep;
    bgstep = (bgend-bgbegin)/nbgbin;
    
    int izbin = static_cast<int>((zrhit-zbegin)/zstep);
    if( izbin<0 )           { izbin=0;       }
    else if( izbin>=nzbin ) { izbin=nzbin-1; }
    int ibgbin = static_cast<int>((betaGamma*xmass(4)-bgbegin)/bgstep);
    if( ibgbin<0 )            { ibgbin=0;        }
    else if( ibgbin>=nbgbin ) { ibgbin=nbgbin-1; }
    
    if( charge==1 ) {
      tcorr = m_p_offset_12[0][ipmt][ibgbin][izbin];
    }
    else {
      tcorr = m_p_offset_12[1][ipmt][ibgbin][izbin];
    }
  }
  deltaT = deltaT - tcorr;

  return deltaT;
}


double TofCorrPID::offsetTof( unsigned int ispecies, int tofid1, int tofid2, double zrhit1, double zrhit2, double betaGamma, int charge, double dt ) {
  if( ispecies==0 || ispecies==1 ) { return dt; }

  double deltaT = -1000.0;
  if( tofid1<0 || tofid1>=88 || tofid2<88 || tofid2>=176 ) {
    cout << "Particle::TofCorrPID: offsetTof for double layer: the input parameter are NOT correct! Please check them!" << endl;
    exit(1);
  }

  double sigmaCorr2  = m_p_common*m_p_common;
  double sigmaInner  = bSigma( 2, tofid1, zrhit1 );
  double sigmaInner2 = sigmaInner*sigmaInner;
  double sigmaOuter  = bSigma( 2, tofid2, zrhit2 );
  double sigmaOuter2 = sigmaOuter*sigmaOuter;
  double sigma = sqrt( (sigmaInner2*sigmaOuter2-sigmaCorr2*sigmaCorr2)/(sigmaInner2+sigmaOuter2-2.0*sigmaCorr2) );

  m_sigma[0] = sigma;
  m_sigma[1] = sigma;

  double fraction = ( sigmaOuter2 - sigmaCorr2 )/( sigmaInner2 + sigmaOuter2 - 2.0*sigmaCorr2);
  deltaT = fraction*m_dtCorr[ispecies][4] + (1.0-fraction)*m_dtCorr[ispecies][5];

  // Jpsi2012
  double tcorr = 0.0;
  if( ispecies==4 && betaGamma*xmass(4)<0.8 ) {
    int    nzbin  = 46;
    double zbegin = -115.0;
    double zend   = 115.0;
    double zstep  = (zend-zbegin)/nzbin;
    
    double nbgbin  = 20.0;
    double bgbegin = 0.3;
    double bgend   = 0.8;
    double bgstep;
    bgstep = (bgend-bgbegin)/nbgbin;
    
    int izbin = static_cast<int>((zrhit1-zbegin)/zstep);
    if( izbin<0 )           { izbin=0;       }
    else if( izbin>=nzbin ) { izbin=nzbin-1; }
    int ibgbin = static_cast<int>((betaGamma*xmass(4)-bgbegin)/bgstep);
    if( ibgbin<0 )            { ibgbin=0;        }
    else if( ibgbin>=nbgbin ) { ibgbin=nbgbin-1; }
    
    if( charge==1 ) {
      tcorr = m_p_offset_12[0][6][ibgbin][izbin];
    }
    else {
      tcorr = m_p_offset_12[1][6][ibgbin][izbin];
    }
  }
  deltaT = deltaT - tcorr;

  return deltaT;
}


double TofCorrPID::sigmaTof( unsigned int ispecies, int charge, bool barrel, unsigned int ipmt, int tofid[2], double zrhit[2], double betaGamma ) {

  double sigma = 1.0e-6;

  if( ispecies==0 || ispecies==1 ) {
    if( barrel ) {
      if( ipmt==0 ) {
	sigma = bSigma( 0, tofid[0], zrhit[0] );
      }
      else if( ipmt==1 ) {
	sigma = bSigma( 1, tofid[0], zrhit[0] );
      }
      else if( ipmt==2 ) {
	sigma = bSigma( 0, tofid[1], zrhit[1] );
      }
      else if( ipmt==3 ) {
	sigma = bSigma( 1, tofid[1], zrhit[1] );
      }
      else if( ipmt==4 ) {
	sigma = bSigma( 2, tofid[0], zrhit[0] );
      }
      else if( ipmt==5 ) {
	sigma = bSigma( 2, tofid[1], zrhit[1] );
      }
      else if( ipmt==6 ) {
	sigma = bSigma( &tofid[0], &zrhit[0] );
      }
    }
    else {
      sigma = eSigma( tofid[0], zrhit[0] );
    }
  }
  else {
    unsigned int iz = 0;
    if( ipmt==2 || ipmt==3 || ipmt==5 ) { iz=1; }

    sigma = sigmaTof( ispecies, charge, barrel, ipmt, zrhit[iz], betaGamma );
  }

  return sigma;
}


double TofCorrPID::sigmaTof( unsigned int ispecies, int charge, bool barrel, unsigned int ipmt, double zrhit, double betaGamma ) {

  int ibgbin = -1;
  int izbin = 0;
  // Jpsi2012
  if( ispecies==4 && (betaGamma*xmass(4))<0.8 ) {
    double nbgbin = 20.0;
    double bgbegin = 0.3;
    double bgend   = 0.8;
    double bgstep;
    bgstep = (bgend-bgbegin)/nbgbin;
    ibgbin = static_cast<int>((betaGamma*xmass(4)-bgbegin)/bgstep);
    
    if( ibgbin<0 )            { ibgbin=0;        }
    else if( ibgbin>=nbgbin ) { ibgbin=nbgbin-1; }
    
    if( barrel ) {
      int    nzbin  = 46;
      double zbegin = -115.0;
      double zend   = 115.0;
      double zstep  = (zend-zbegin)/nzbin;
      
      izbin = static_cast<int>((zrhit-zbegin)/zstep);
      if( izbin<0 )           { izbin=0;       }
      else if( izbin>=nzbin ) { izbin=nzbin-1; }
    }
    else {
      int    nzbin  = 7;
      double zbegin = 55.0;
      double zend   = 83.0;
      double zstep  = (zend-zbegin)/nzbin;
      
      izbin = static_cast<int>((zrhit-zbegin)/zstep);
      if( izbin<0 )           { izbin=0;       }
      else if( izbin>=nzbin ) { izbin=nzbin-1; }
    }
  }

  unsigned int numParam = 4;
  double func[12];
  for( unsigned int i=0; i<4; i++ ) {
    if( i==0 ) { func[i] = 1.0; }
    else {
      func[i] = func[i-1]*zrhit;
    }
  }
  for( unsigned int i=4; i<12; i++ ) {
    func[i] = 0.0;
  }

  // Jpsi2012
  if( barrel ) { // barrel
    if( ispecies==2 || ispecies==3 ) { // pion / kaon
      for( unsigned int i=4; i<10; i++ ) {
	func[i] = func[i-1]*zrhit;
      }
      func[10] = 1.0/(115.0-zrhit)/(115.0-zrhit);
      func[11] = 1.0/(115.0+zrhit)/(115.0+zrhit);
      numParam = 12;
    }
    else if( ispecies==4 ) {  // proton / anti-proton
      for( unsigned int i=4; i<12; i++ ) {
	func[i] = func[i-1]*zrhit;
      }
      numParam = 12;
    }
  }
  else { // endcap
    numParam = 4;
  }

  unsigned int inumber = ipmt;
  if( !barrel ) { inumber=7; }

  double sigma = 0.0;
  if( ispecies==2 || ispecies==3 ) { // pion / kaon
    for( unsigned int i=0; i<numParam; i++ ) {
      sigma += m_par_sigma[ispecies-2][inumber][i]*func[i];
    }
  }
  else if( ispecies==4 ) {
    if( ibgbin!=-1 ) {
      // Jpsi2012
      if( barrel ) {
	if( charge==1 ) {
	  sigma = m_p_sigma_12[0][inumber][ibgbin][izbin];
	}
	else {
	  sigma = m_p_sigma_12[1][inumber][ibgbin][izbin];
	}
      }
      else {
	if( charge==1 ) {
	  sigma = m_p_sigma_ec12[0][ibgbin][izbin];
	}
	else {
	  sigma = m_p_sigma_ec12[1][ibgbin][izbin];
	}
      }
      if( fabs(sigma+999.0)<1.0e-6 ) {
	sigma = 0.3;
      }
    }
    else {
      for( unsigned int i=0; i<numParam; i++ ) {
	if( charge==1 ) {
	  sigma += m_par_sigma[2][inumber][i]*func[i];
	}
	else {
	  sigma += m_par_sigma[3][inumber][i]*func[i];
	}
      }
    }
  }

  // Jpsi2012
  int run = getRunNo();
  if( run>0 ) {
    if( ispecies==2 ) {
      sigma = sigma*(TMath::Exp((betaGamma-0.356345)/(-0.767124))+0.994072);
    }
  }

  return sigma;
}


double TofCorrPID::qCurveFunc( unsigned int layer, double betaGamma ) {
  double q0 = -100.0;
  if( layer>=3 || betaGamma<0.0 ) {
    cout << "Particle::TofCorrPID::qCurveFunc: the input parameter are NOT correct! Please check them!" << endl;
    return q0;
  }

  double beta = betaGamma/sqrt(1.0+betaGamma*betaGamma);
  double logterm = log( m_q0_bg[layer][2]+pow((1.0/betaGamma), m_q0_bg[layer][4] ) );
  q0 = m_q0_bg[layer][0]*( m_q0_bg[layer][1]-pow( beta, m_q0_bg[layer][3] ) - logterm )/pow( beta, m_q0_bg[layer][3] );

  return q0;
}


double TofCorrPID::bSigma( unsigned int end, int tofid, double zrhit ) {

  double func[5];
  func[0] = 1.0;
  func[1] = zrhit;
  func[2] = zrhit*zrhit;
  func[3] = zrhit*zrhit*zrhit;
  func[4] = zrhit*zrhit*zrhit*zrhit;

  double sigma = 0.0;
  for( unsigned int i=0; i<5; i++ ) {
    sigma += m_p_weight[tofid][end][i]*func[i];
  }

  return sigma;
}


double TofCorrPID::bSigma( int tofid[2], double zrhit[2] ) {

  double sigma1 = bSigma( 2, tofid[0], zrhit[0] );
  double sigma2 = bSigma( 2, tofid[1], zrhit[1] );
  double sigmaCorr2  = m_p_common*m_p_common;
  double sigma = ( sigma1*sigma1*sigma2*sigma2 - sigmaCorr2*sigmaCorr2 )/( sigma1*sigma1 + sigma2*sigma2 - 2.0*sigmaCorr2 );
  sigma = sqrt(fabs(sigma));

  return sigma;
}


double TofCorrPID::eSigma( int tofid, double zrhit ) {

  double func[5];
  func[0] = 1.0;
  func[1] = zrhit;
  func[2] = zrhit*zrhit;

  double sigma = 0.0;
  for( unsigned int i=0; i<3; i++ ) {
    sigma += m_ec_sigma[tofid][i]*func[i];
  }

  return sigma;
}


bool TofCorrPID::correlationCheck( unsigned int ipmt ) {
  bool chiCut = false;
  bool good = false;
  double pdftemp = 0;
  for( unsigned int i=0; i<5; i++ ) {
    if( ( m_chiCorr[i][ipmt]>(0.0-chiMinCut()) && m_chiCorr[i][ipmt]<chiMaxCut() ) && !good ) { good=true; }
    double ppp = pdfCalculate(m_chiCorr[i][ipmt],1);
    if( fabs(ppp) > pdftemp) { pdftemp = fabs(ppp); }
  }
  m_pdfmin = pdftemp;
  if( pdftemp < pdfCalculate(pdfMinSigmaCut(),1) ) return chiCut;
  if( !good ) return chiCut;
  chiCut = true;
  return chiCut;
}
