#include "Identifier/Identifier.h"
#include "Identifier/TofID.h"
#include "TofCaliSvc/ITofCaliSvc.h"
#include "DstEvent/TofHitStatus.h"
#include "TofRec/TofConstants.h"
#include "TofRec/TofTrack.h"

extern ITofCaliSvc*  tofCaliSvc;

TofTrack::TofTrack( int run, int event ) {
  m_run = run;
  m_event = event;
  m_trackId = -1;
  m_tofTrackId = -1;
  m_id1 = -9;
  m_id2 = -9;
  m_istrip1 = -9;
  m_istrip2 = -9;
  m_hitCase = NoHit;
  m_momentum = -99.0;
  m_path  = 0.0;
  m_path1 = 0.0;
  m_path2 = 0.0;
  m_zrhit1 = 99.0;
  m_errzr1 = 99.0;
  m_zrhit2 = 99.0;
  m_errzr2 = 99.0;
  m_xhit1  = 99.0;
  m_yhit1  = 99.0;
  m_xhit2  = 99.0;
  m_yhit2  = 99.0;
  for( unsigned int i=0; i<5; i++ ) {
    m_kal[i]  = -1;
    m_zr1[i]  = 99.0;
    m_zr2[i]  = 99.0;
    m_ezr1[i] = 99.0;
    m_ezr2[i] = 99.0;
  }
  if( m_tofData1.size()>0 ) {
    m_tofData1.clear();
  }
  if( m_tofData2.size()>0 ) {
    m_tofData2.clear();
  }
  if( m_tofData3.size()>0 ) {
    m_tofData3.clear();
  }
  if( m_tofData4.size()>0 ) {
    m_tofData4.clear();
  }
  if( m_tofData5.size()>0 ) {
    m_tofData5.clear();
  }
  if( m_tofData6.size()>0 ) {
    m_tofData6.clear();
  }
  m_quality1 = 0;
  m_quality2 = 0;
  m_quality  = 10;

  m_delStrip1 = 20;
  m_delStrip2 = 20;

  m_tofId1 = -9;
  m_tofId2 = -9;
  m_strip1 = -9;
  m_strip2 = -9;

  m_ph11  = -99.0;
  m_ph12  = -99.0;
  m_ph21  = -99.0;
  m_ph22  = -99.0;
  m_ph1   = -99.0;
  m_ph2   = -99.0;
  m_ph    = -99.0;

  for( unsigned int i=0; i<5; i++ ) {
    m_tof11[i]  = 0.0;
    m_tof12[i]  = 0.0;
    m_tof21[i]  = 0.0;
    m_tof22[i]  = 0.0;
    m_tof1[i]   = 0.0;
    m_tof2[i]   = 0.0;
    m_tof[i]    = 0.0;
  }

  m_sigma11 = -99.0;
  m_sigma12 = -99.0;
  m_sigma21 = -99.0;
  m_sigma22 = -99.0;
  m_sigma1  = -99.0;
  m_sigma2  = -99.0;
  m_sigma   = -99.0;

  m_qch1  = -99.0;
  m_qch2  = -99.0;
  m_qch3  = -99.0;
  m_qch4  = -99.0;
  m_adc1  = -99.0;
  m_adc2  = -99.0;
  m_adc3  = -99.0;
  m_adc4  = -99.0;
  m_tdc1  = -99.0;
  m_tdc2  = -99.0;
  m_tdc3  = -99.0;
  m_tdc4  = -99.0;

  for( unsigned int i=0; i<5; i++ ) {
    m_texpInner[i] = -99.0;
    m_texpOuter[i] = -99.0;
    m_texp[i] = -99.0;
  }

  m_ztdc1 = -99.0;
  m_ztdc2 = -99.0;
  m_zadc1 = -99.0;
  m_zadc2 = -99.0;

  m_estime = -99.0;
  m_tdiff1  = -99.0;
  m_tdiff2  = -99.0;

  m_flag  = 0;

  return;
}


TofTrack::~TofTrack() {
  if( m_tofData1.size()>0 ) {
    m_tofData1.clear();
  }
  if( m_tofData2.size()>0 ) {
    m_tofData2.clear();
  }
  if( m_tofData3.size()>0 ) {
    m_tofData3.clear();
  }
  if( m_tofData4.size()>0 ) {
    m_tofData4.clear();
  }
  if( m_tofData5.size()>0 ) {
    m_tofData5.clear();
  }
  if( m_tofData6.size()>0 ) {
    m_tofData6.clear();
  }
  return;
}

//------- setExtTrack ----------------------------------------------
//  get information from RecExtTrackCol
//  tofId1(), tofId2(), hitCase(),
//  p(), path1(), path2(), zrhit1(), zrhit2(), errz1(), errz2()
//  quality() = 3 ( no hit )
//
void TofTrack::setExtTrack( RecExtTrack* extTrack, double costheta, double p[5], int kal[5], double t0, int t0Stat ) {

  m_estime = t0;
  m_t0Stat = t0Stat;

  int tofId1 = extTrack->tof1VolumeNumber();
  int tofId2 = extTrack->tof2VolumeNumber();

  int iExist1 = -1;
  int iExist2 = -1;
  if( tofId1<0 ) {
    if( extTrack->tof1VolumeNumber(3)>-1 ) {
      iExist1 = 3;
    }
    else if( extTrack->tof1VolumeNumber(1)>-1 ) {
      iExist1 = 1;
    }
    else if( extTrack->tof1VolumeNumber(4)>-1 ) {
      iExist1 = 4;
    }
    else if( extTrack->tof1VolumeNumber(0)>-1 ) {
      iExist1 = 0;
    }
    if( iExist1!=-1 ) {
      tofId1 = extTrack->tof1VolumeNumber(iExist1);
    }
  }
  if( tofId2<0 ) {
    if( extTrack->tof2VolumeNumber(3)>-1 ) {
      iExist2 = 3;
    }
    else if( extTrack->tof2VolumeNumber(1)>-1 ) {
      iExist2 = 1;
    }
    else if( extTrack->tof2VolumeNumber(4)>-1 ) {
      iExist2 = 4;
    }
    else if( extTrack->tof2VolumeNumber(0)>-1 ) {
      iExist2 = 0;
    }
    if( iExist2!=-1 ) {
      tofId2 = extTrack->tof2VolumeNumber(iExist2);
    }
  }
  if( iExist1 == -1 ) { iExist1 = 2; }
  if( iExist2 == -1 ) { iExist2 = 2; }
  
  m_barrel = 3;
  if( tofId1>=0 && tofId1<=87 ) {
    m_id1 = tofId1;
    m_barrel = 1;
    m_hitCase = InnerLayer;
  }
  else if( tofId1>=176 && tofId1<=223 ) {
    m_id1 = tofId1 - 176 + 48;
    m_barrel = 2;
    m_hitCase = WestEndcap;
    if( costheta>0.0 ) {
      m_id1 = -9;
      m_barrel = 3;
      m_hitCase = NoHit;
    }
  }
  else if( tofId1>=224 && tofId1<=271 ) { 
    m_id1 = tofId1 - 176 - 48;
    m_barrel = 0;
    m_hitCase = EastEndcap;
    if( costheta<0.0 ) {
      m_id1 = -9;
      m_barrel = 3;
      m_hitCase = NoHit;
    }
  }
  else if( tofId1>=272 && tofId1<=1135 ) { 
    m_id1 = tofId1 - 176 - 96;
    m_istrip1 = m_id1%12;
    m_id1 = m_id1/12;
    if( tofId1>=272 && tofId1<=703 ) {
      if( costheta>0.0 ) {
	m_barrel = 4;
	m_hitCase = EastEndcapMRPC;
      }
      else {
	m_id1 = -9;
	m_istrip1 = -9;
      }
    }
    else if( tofId1>=704 && tofId1<=1135 ) { 
      if( costheta<0.0 ) {
	m_barrel = 5;
	m_hitCase = WestEndcapMRPC;
      }
      else {
	m_id1 = -9;
	m_istrip1 = -9;
      }
    }
  }
  else {
    m_barrel = 3;
    m_hitCase = NoHit;
  }

  if( tofId2>=88 && tofId2<=175 ) {
    m_id2 = tofId2;
    m_barrel = 1;
    if( m_hitCase == InnerLayer ) {
      m_hitCase = DoubleLayer;
    }
    else if( m_hitCase==NoHit ) {
      m_hitCase = OuterLayer;
    }
  }
  else if( tofId2>=272 && tofId2<=1135 ) { 
    m_id2 = tofId2 - 176 - 96;
    m_istrip2 = m_id2%12;
    m_id2 = m_id2/12;
    if( m_hitCase==EastEndcapMRPC || m_hitCase==WestEndcapMRPC || m_hitCase==NoHit ) {
      if( tofId2>=272 && tofId2<=703 ) {
	if( costheta>0.0 ) {
	  m_barrel = 4;
	  m_hitCase = EastEndcapMRPC;
	}
	else {
	  m_id2 = -9;
	  m_istrip2 = -9;
	}
      }
      else if( tofId2>=704 && tofId2<=1135 ) { 
	if( costheta<0.0 ) {
	  m_barrel = 5;
	  m_hitCase = WestEndcapMRPC;
	}
	else {
	  m_id2 = -9;
	  m_istrip2 = -9;
	}
      }
    }
  }

  m_trackId = extTrack->trackId();

  m_momentum = extTrack->tof1Momentum().r();
  if( !( tofId1>=272 && tofId1<=1135 ) && ( m_hitCase == EastEndcapMRPC || m_hitCase == WestEndcapMRPC )  ){
    m_momentum = extTrack->tof2Momentum().r();
  }

  if( m_hitCase == InnerLayer || m_hitCase == DoubleLayer || m_hitCase == EastEndcap || m_hitCase == WestEndcap || ( ( m_hitCase == EastEndcapMRPC || m_hitCase == WestEndcapMRPC ) && ( tofId1>=272 && tofId1<=1135 ) ) ) {
    m_path1  = extTrack->tof1Path(iExist1);
    m_theta1 = extTrack->tof1Momentum(iExist1).rho()/extTrack->tof1Momentum(iExist1).r();
    m_phi1   = extTrack->tof1Position(iExist1).phi();
    for( unsigned int i=0; i<5; i++ ) {
      m_texpInner[i] = extTrack->tof1(i);
      if( fabs(m_texpInner[i]+99.0)<1.0e-6 ) {
	double beta = p[iExist1]/sqrt(p[iExist1]*p[iExist1]+mass[iExist1]*mass[iExist1]);
	double betaNew = p[i]/sqrt(p[i]*p[i]+mass[i]*mass[i]);
	m_texpInner[i] = beta*extTrack->tof1(iExist1)/betaNew;
      }
    }
    if( m_hitCase == InnerLayer || m_hitCase == DoubleLayer ) {
      m_xhit1  = extTrack->tof1Position(iExist1).x();
      m_yhit1  = extTrack->tof1Position(iExist1).y();
      m_zrhit1 = extTrack->tof1Position(iExist1).z();
      m_errzr1 = extTrack->tof1PosSigmaAlongZ(iExist1);
      for( unsigned int i=0; i<5; i++ ) {
	m_zr1[i]  = extTrack->tof1Position(i).z();
	m_ezr1[i] = extTrack->tof1PosSigmaAlongZ(i); 
	if( fabs(m_zr1[i]+99.0)<1.0e-6 ) {
	  m_zr1[i]  = m_zrhit1;
	  m_ezr1[i] = m_errzr1;
	}
      }
    }
    else if( m_hitCase == EastEndcap || m_hitCase == WestEndcap ) {
      m_xhit1  = extTrack->tof1Position(iExist1).x();
      m_yhit1  = extTrack->tof1Position(iExist1).y();
      m_zrhit1 = extTrack->tof1Position(iExist1).rho();
      m_errzr1 = sqrt( extTrack->tof1PosSigmaAlongX(iExist1)*extTrack->tof1PosSigmaAlongX(iExist1) + extTrack->tof1PosSigmaAlongY(iExist1)*extTrack->tof1PosSigmaAlongY(iExist1) );
      for( unsigned int i=0; i<5; i++ ) {
	m_zr1[i]  = extTrack->tof1Position(i).rho();
	m_ezr1[i] = sqrt( extTrack->tof1PosSigmaAlongX(i)*extTrack->tof1PosSigmaAlongX(i) + extTrack->tof1PosSigmaAlongY(i)*extTrack->tof1PosSigmaAlongY(i) ); 
	if( fabs(m_zr1[i]+99.0)<1.0e-6 ) {
	  m_zr1[i]  = m_zrhit1;
	  m_ezr1[i] = m_errzr1;
	}
      }
    }
    else if( ( m_hitCase == EastEndcapMRPC || m_hitCase == WestEndcapMRPC ) && ( tofId1>=272 && tofId1<=1135 ) ) { 
      m_xhit1  = extTrack->tof1Position(iExist1).x();
      m_yhit1  = extTrack->tof1Position(iExist1).z();
      m_zrhit1 = extTrack->tof1Position(iExist1).x();
      m_errzr1 = extTrack->tof1PosSigmaAlongX(iExist1);
      for( unsigned int i=0; i<5; i++ ) {
	m_zr1[i]  = extTrack->tof1Position(i).x();
	m_ezr1[i] = extTrack->tof1PosSigmaAlongX(i);
	if( fabs(m_zr1[i]+99.0)<1.0e-6 ) {
	  m_zr1[i]  = m_zrhit1;
	  m_ezr1[i] = m_errzr1;
	}
      }
      if( !( tofId2>=272 && tofId2<=1135 ) ) {
	for( unsigned int i=0; i<5; i++ ) {
	  m_texpOuter[i] = m_texpInner[i] * 136.573/133.673;
	}
      }
    }
  }

  if( m_hitCase == OuterLayer || m_hitCase == DoubleLayer || ( ( m_hitCase == EastEndcapMRPC || m_hitCase == WestEndcapMRPC ) && ( tofId2>=272 && tofId2<=1135 ) ) ) {
    m_path2  = extTrack->tof2Path(iExist2);
    m_theta2 = extTrack->tof2Momentum(iExist2).rho()/extTrack->tof2Momentum(iExist2).r();
    m_phi2   = extTrack->tof2Position(iExist2).phi();
    for( unsigned int i=0; i<5; i++ ) {
      m_texpOuter[i] = extTrack->tof2(i);
      if( fabs(m_texpOuter[i]+99.0)<1.0e-6 ) {
	double beta = p[iExist2]/sqrt(p[iExist2]*p[iExist2]+mass[iExist2]*mass[iExist2]);
	double betaNew = p[i]/sqrt(p[i]*p[i]+mass[i]*mass[i]);
	m_texpOuter[i] = beta*extTrack->tof2(iExist2)/betaNew;
      }
    }
    if( m_hitCase == OuterLayer || m_hitCase == DoubleLayer ) {
      m_xhit2  = extTrack->tof2Position(iExist2).x();
      m_yhit2  = extTrack->tof2Position(iExist2).y();
      m_zrhit2 = extTrack->tof2Position(iExist2).z();
      m_errzr2 = extTrack->tof2PosSigmaAlongZ(iExist2);
      for( unsigned int i=0; i<5; i++ ) {
	m_zr2[i]  = extTrack->tof2Position(i).z();
	m_ezr2[i] = extTrack->tof2PosSigmaAlongZ(i); 
	if( fabs(m_zr2[i]+99.0)<1.0e-6 ) {
	  m_zr2[i]  = m_zrhit2;
	  m_ezr2[i] = m_errzr2;
	}
      }
    }
    else if( ( m_hitCase == EastEndcapMRPC || m_hitCase == WestEndcapMRPC ) && ( tofId2>=272 && tofId2<=1135 ) ) { 
      m_xhit2  = extTrack->tof2Position(iExist2).x();
      m_yhit2  = extTrack->tof2Position(iExist2).z();
      m_zrhit2 = extTrack->tof2Position(iExist2).x();
      m_errzr2 = extTrack->tof2PosSigmaAlongX(iExist2);
      for( unsigned int i=0; i<5; i++ ) {
	m_zr2[i]  = extTrack->tof2Position(i).x();
	m_ezr2[i] = extTrack->tof2PosSigmaAlongX(i);
	if( fabs(m_zr2[i]+99.0)<1.0e-6 ) {
	  m_zr2[i]  = m_zrhit2;
	  m_ezr2[i] = m_errzr2;
	}
      }
      if( !( tofId1>=272 && tofId1<=1135 ) ) {
	for( unsigned int i=0; i<5; i++ ) {
	  m_texpInner[i] = m_texpOuter[i] * 133.673/136.573;
	}
      }
    }
  }

  if( m_hitCase == NoHit ) { m_quality = 11; }

  for( unsigned int i=0; i<5; i++ ) {
    m_kal[i] = kal[i];
  }

  return;
}


//------- get Multi hit  ----------------------------------------------
//  get Multi-hit of one TOF Counter
//
void TofTrack::getMultiHit( TofTrack*& track ) {
  if( m_hitCase == InnerLayer || m_hitCase == OuterLayer || m_hitCase == DoubleLayer ) {

    if( ( m_hitCase==InnerLayer || m_hitCase==DoubleLayer ) && ( track->hitCase()==InnerLayer || track->hitCase()==DoubleLayer ) ) {
      if( ( abs(m_id1-track->id1())<=1 ) || ( m_id1==0 && track->id1()==87 ) || ( m_id1==87 && track->id1()==0 ) ) {
	track->setQuality1( ( track->quality1() | 0x400 ) );
	m_quality1 = ( m_quality1 | 0x400 );	
      }
    }

    if( ( m_hitCase==OuterLayer || m_hitCase==DoubleLayer ) && ( track->hitCase()==OuterLayer || track->hitCase()==DoubleLayer ) ) {
      if( ( abs(m_id2-track->id2())<=1 ) || ( m_id2==88 && track->id2()==175 ) || ( m_id2==175 && track->id2()==88 ) ) {
	track->setQuality2( ( track->quality2() | 0x400 ) );
	m_quality2 = ( m_quality2 | 0x400 );	
      }
    }

  }
  else if( m_hitCase == EastEndcap ) {
    if( track->hitCase()==EastEndcap ) {
      if( ( abs(m_id1-track->id1())<=1 ) || ( m_id1==0 && track->id1()==47 ) || ( m_id1==47 && track->id1()==0 ) ) {
	track->setQuality1( ( track->quality1() | 0x400 ) );
	m_quality1 = ( m_quality1 | 0x400 );	
      } 
    }
  }
  else if( m_hitCase == WestEndcap ) {
    if( track->hitCase()==WestEndcap ) {
      if( ( abs(m_id1-track->id1())<=1 ) || ( m_id1==48 && track->id1()==95 ) || ( m_id1==95 && track->id1()==48 ) ) {
	track->setQuality1( ( track->quality1() | 0x400 ) );
	m_quality1 = ( m_quality1 | 0x400 );	
      } 
    }
  }
  if( m_hitCase == EastEndcapMRPC || m_hitCase == WestEndcapMRPC ) {
    if( ( m_hitCase==EastEndcapMRPC && track->hitCase()==EastEndcapMRPC ) || ( m_hitCase==WestEndcapMRPC && track->hitCase()==WestEndcapMRPC ) ) {
      if( m_id1>=0 ) {
	if( ( m_id1==track->id1() ) && abs(m_istrip1-track->strip1())<=1 ) {
	  track->setQuality1( ( track->quality1() | 0x400 ) );
	  m_quality1 = ( m_quality1 | 0x400 );
	}
      }
      if( m_id2>=0 ) {
	if( ( m_id2==track->id2() ) && abs(m_istrip2-track->strip2())<=1 ) {
	  track->setQuality1( ( track->quality1() | 0x400 ) );
	  m_quality1 = ( m_quality1 | 0x400 );
	}
      }
    }
  }

  return;
}


//------- setTofData  --------------------------------------------
//  do TOF hits and extrapolated track match
//  tofTrackId(), tofMod1(), tofMod2()
//  quality() = 3 ( no hit )
//
void TofTrack::setTofData( TofDataMap tofDataMap ) {

  if( m_hitCase == NoHit ) return;

  unsigned int identify[11];
  unsigned int count[11];
  for( unsigned int i=0; i<11; i++ ) {
    identify[i] = 0x0000c000;
    count[i] = 0;
  }
  unsigned int countTot1 = 0;
  unsigned int countTot2 = 0;

  if( ( ( m_hitCase == InnerLayer ) || ( m_hitCase == DoubleLayer ) ) && ( m_id1 > -1 ) ) {
    int tofid0 = m_id1;
    identify[0] = TofID::getIntID( 1, 0, tofid0, 0 );
    count[0] = tofDataMap.count( identify[0] );
    int tofid1 = tofid0 - 1;
    if( tofid1 == -1 ) { tofid1 = 87; }
    identify[1] = TofID::getIntID( 1, 0, tofid1, 0 );
    count[1] = tofDataMap.count( identify[1] );
    int tofid2 = tofid0 + 1;
    if( tofid2 == 88 ) { tofid2 = 0; }
    identify[2] = TofID::getIntID( 1, 0, tofid2, 0 );
    count[2] = tofDataMap.count( identify[2] );
  }

  if( ( ( m_hitCase == EastEndcap ) || ( m_hitCase == WestEndcap ) )&& ( m_id1 > -1 ) ) {
    unsigned int whichEndcap = 0;
    int tofid0 = m_id1;
    if( m_hitCase == WestEndcap ) {
      whichEndcap = 2;
      tofid0 = m_id1 - 48;
    }
    identify[0] = TofID::getIntID( whichEndcap, 0, tofid0, 0 );
    count[0] = tofDataMap.count( identify[0] );
    int tofid1 = tofid0 - 1;
    if( tofid1 == -1 ) { tofid1 = 47; }
    identify[1] = TofID::getIntID( whichEndcap, 0, tofid1, 0 );
    count[1] = tofDataMap.count( identify[1] );
    int tofid2 = tofid0 + 1;
    if( tofid2 == 48 ) { tofid2 = 0; }
    identify[2] = TofID::getIntID( whichEndcap, 0, tofid2, 0 );
    count[2] = tofDataMap.count( identify[2] );
  }

  if( ( ( m_hitCase == EastEndcapMRPC ) || ( m_hitCase == WestEndcapMRPC ) ) && ( ( m_id1 > -1 ) && ( m_istrip1 > -1 ) ) ) {
    IterTofDataMap iter = tofDataMap.begin();
    for( ; iter != tofDataMap.end(); iter++ ) {
      Identifier iden = TofID::cell_id( (*iter).first );
      if( TofID::is_mrpc( iden ) ) {
	TofData* tof = (*iter).second;
	if( m_id1 == tof->tofId() && abs( m_istrip1 - tof->strip() )<=abs(m_delStrip1) ) {
	  m_delStrip1 = m_istrip1 - tof->strip();
	}
	if( ( abs( m_id1 - tof->tofId() )==1 || ( m_id1==0 && tof->tofId()==35 ) || ( m_id1==35 && tof->tofId()==0 ) || ( m_id1==36 && tof->tofId()==71 ) || ( m_id1==71 && tof->tofId()==36 ) ) && abs( m_istrip1 - tof->strip() )<=abs(m_delStrip1) ) {
	  m_delStrip2 = m_istrip1 - tof->strip();
	}
      }
    }

    unsigned int whichEndcap = 0;
    int tofid0 = m_id1;
    if( m_hitCase == WestEndcapMRPC ) {
      whichEndcap = 1;
      tofid0 = m_id1 - 36;
    }
    int strip0 = m_istrip1;
    int strip1 = strip0 - 1;
    int strip2 = strip0 + 1;
    int strip3 = strip0 - 2;
    int strip4 = strip0 + 2;
    int tofid1 = tofid0 - 1;
    if( tofid1 == -1 ) { tofid1 = 35; }
    int tofid2 = tofid0 + 1;
    if( tofid2 == 36 ) { tofid2 = 0; }

    identify[0] = TofID::getIntID( 3, whichEndcap, tofid0, strip0, 0 );
    count[0]    = tofDataMap.count( identify[0] );
    identify[5] = TofID::getIntID( 3, whichEndcap, tofid1, strip0, 0 );
    count[5]    = tofDataMap.count( identify[5] );
    identify[6] = TofID::getIntID( 3, whichEndcap, tofid2, strip0, 0 );
    count[6]    = tofDataMap.count( identify[6] );

    if( strip1 == -1 ) {
      count[1] = 0;
      count[7] = 0;
      count[9] = 0;
    }
    else {
      identify[1] = TofID::getIntID( 3, whichEndcap, tofid0, strip1, 0 );
      count[1]    = tofDataMap.count( identify[1] );
      identify[7] = TofID::getIntID( 3, whichEndcap, tofid1, strip1, 0 );
      count[7]    = tofDataMap.count( identify[7] );
      identify[9] = TofID::getIntID( 3, whichEndcap, tofid2, strip1, 0 );
      count[9]    = tofDataMap.count( identify[9] );
    }

    if( strip2 == 12 ) {
      count[2] = 0;
      count[8] = 0;
      count[10] = 0;
    }
    else {
      identify[2] = TofID::getIntID( 3, whichEndcap, tofid0, strip2, 0 );
      count[2]    = tofDataMap.count( identify[2] );
      identify[8] = TofID::getIntID( 3, whichEndcap, tofid1, strip2, 0 );
      count[8]    = tofDataMap.count( identify[8] );
      identify[10] = TofID::getIntID( 3, whichEndcap, tofid2, strip2, 0 );
      count[10]    = tofDataMap.count( identify[10] );
    }
    if( strip3 == -1 || strip3 == -2 ) { count[3] = 0; }
    else {
      identify[3] = TofID::getIntID( 3, whichEndcap, tofid0, strip3, 0 );
      count[3] = tofDataMap.count( identify[3] );
    }
    if( strip4 == 12 || strip4 == 13 ) { count[4] = 0; }
    else {
      identify[4] = TofID::getIntID( 3, whichEndcap, tofid0, strip4, 0 );
      count[4] = tofDataMap.count( identify[4] );
    }
  }

  for( unsigned int i=0; i<11; i++ ) {
    if( count[i] > 0 ) {
      pair< IterTofDataMap, IterTofDataMap > range = tofDataMap.equal_range( identify[i] );
      IterTofDataMap iter = range.first;
      for( unsigned int j=0; j<count[i]; j++,iter++ ) {
	if( i==0 ) {
	  tofDataAnalysis( (*iter).second, 1 );
	}
	else if( i==1 || i==2 ) {
	  tofDataAnalysis( (*iter).second, 2 );
	}
	else {
	  tofDataAnalysis( (*iter).second, 3 );
	}
      }
    }
    countTot1 = countTot1 + count[i];
  }

  if( countTot1 == 0 ) {
    if( m_hitCase == DoubleLayer ) {
      m_hitCase = OuterLayer;
    }
    else if( ( m_hitCase == EastEndcapMRPC ) || ( m_hitCase == WestEndcapMRPC ) ) {
    }
    else {
      m_hitCase = NoHit; 
      m_quality = 12;
    }
  }


  for( unsigned int i=0; i<11; i++ ) {
    identify[i] = 0x0000c000;
    count[i] = 0;
  }
  if( ( ( m_hitCase == OuterLayer ) || ( m_hitCase == DoubleLayer ) ) && ( m_id2 > 87 ) ) {
    int tofid0 = m_id2 - 88;
    identify[0] = TofID::getIntID( 1, 1, tofid0, 0 );
    count[0] = tofDataMap.count( identify[0] );
    int tofid1 = tofid0 - 1;
    if( tofid1 == -1 ) { tofid1 = 87; }
    identify[1] = TofID::getIntID( 1, 1, tofid1, 0 );
    count[1] = tofDataMap.count( identify[1] );
    int tofid2 = tofid0 + 1;
    if( tofid2 == 88 ) { tofid2 = 0; }
    identify[2] = TofID::getIntID( 1, 1, tofid2, 0 );
    count[2] = tofDataMap.count( identify[2] );

    for( unsigned int i=0; i<3; i++ ) {
      if( count[i] > 0 ) {
	pair< IterTofDataMap, IterTofDataMap > range = tofDataMap.equal_range( identify[i] );
	IterTofDataMap iter = range.first;
	for( unsigned int j=0; j<count[i]; j++,iter++ ) {
	  if( i==0 ) {
	    tofDataAnalysis( (*iter).second, 3 );
	  }
	  else {
	    tofDataAnalysis( (*iter).second, 4 );
	  }
	}
      }
      countTot2 = countTot2 + count[i];
    }

    if( countTot2 == 0 ) {
      if( m_hitCase != DoubleLayer ) {
	m_hitCase = NoHit; 
	m_quality = 12;
      }
      else {
	m_hitCase = InnerLayer;
      }
    }
  }

  if( ( ( m_hitCase == EastEndcapMRPC ) || ( m_hitCase == WestEndcapMRPC ) ) && ( ( m_id2 > -1 ) && ( m_istrip2 > -1 ) ) ) {
    IterTofDataMap iter = tofDataMap.begin();
    for( ; iter != tofDataMap.end(); iter++ ) {
      Identifier iden = TofID::cell_id( (*iter).first );
      if( TofID::is_mrpc( iden ) ) {
	TofData* tof = (*iter).second;
	if( m_id2 == tof->tofId() && abs( m_istrip2 - tof->strip() )<=abs(m_delStrip1) ) {
	  m_delStrip1 = m_istrip2 - tof->strip();
	}
	if( ( abs( m_id2 - tof->tofId() )==1 || ( m_id2==0 && tof->tofId()==35 ) || ( m_id2==35 && tof->tofId()==0 ) || ( m_id2==36 && tof->tofId()==71 ) || ( m_id2==71 && tof->tofId()==36 ) ) && abs( m_istrip2 - tof->strip() )<=abs(m_delStrip2) ) {
	  m_delStrip2 = m_istrip2 - tof->strip();
	}
      }
    }

    unsigned int whichEndcap = 0;
    int tofid0 = m_id2;
    if( m_hitCase == WestEndcapMRPC ) {
      whichEndcap = 1;
      tofid0 = m_id2 - 36;
    }
    int strip0 = m_istrip2;
    int strip1 = strip0 - 1;
    int strip2 = strip0 + 1;
    int strip3 = strip0 - 2;
    int strip4 = strip0 + 2;
    int tofid1 = tofid0 - 1;
    if( tofid1 == -1 ) { tofid1 = 35; }
    int tofid2 = tofid0 + 1;
    if( tofid2 == 36 ) { tofid2 = 0; }

    identify[0] = TofID::getIntID( 3, whichEndcap, tofid0, strip0, 0 );
    count[0]    = tofDataMap.count( identify[0] );
    identify[5] = TofID::getIntID( 3, whichEndcap, tofid1, strip0, 0 );
    count[5]    = tofDataMap.count( identify[5] );
    identify[6] = TofID::getIntID( 3, whichEndcap, tofid2, strip0, 0 );
    count[6]    = tofDataMap.count( identify[6] );

    if( strip1 == -1 ) {
      count[1] = 0;
      count[7] = 0;
      count[9] = 0;
    }
    else {
      identify[1] = TofID::getIntID( 3, whichEndcap, tofid0, strip1, 0 );
      count[1]    = tofDataMap.count( identify[1] );
      identify[7] = TofID::getIntID( 3, whichEndcap, tofid1, strip1, 0 );
      count[7]    = tofDataMap.count( identify[7] );
      identify[9] = TofID::getIntID( 3, whichEndcap, tofid2, strip1, 0 );
      count[9]    = tofDataMap.count( identify[9] );
    }

    if( strip2 == 12 ) {
      count[2] = 0;
      count[8] = 0;
      count[10] = 0;
    }
    else {
      identify[2] = TofID::getIntID( 3, whichEndcap, tofid0, strip2, 0 );
      count[2]    = tofDataMap.count( identify[2] );
      identify[8] = TofID::getIntID( 3, whichEndcap, tofid1, strip2, 0 );
      count[8]    = tofDataMap.count( identify[8] );
      identify[10] = TofID::getIntID( 3, whichEndcap, tofid2, strip2, 0 );
      count[10]    = tofDataMap.count( identify[10] );
    }
    if( strip3 == -1 || strip3 == -2 ) { count[3] = 0; }
    else {
      identify[3] = TofID::getIntID( 3, whichEndcap, tofid0, strip3, 0 );
      count[3] = tofDataMap.count( identify[3] );
    }
    if( strip4 == 12 || strip4 == 13 ) { count[4] = 0; }
    else {
      identify[4] = TofID::getIntID( 3, whichEndcap, tofid0, strip4, 0 );
      count[4] = tofDataMap.count( identify[4] );
    }

    for( unsigned int i=0; i<11; i++ ) {
      if( count[i] > 0 ) {
	pair< IterTofDataMap, IterTofDataMap > range = tofDataMap.equal_range( identify[i] );
	IterTofDataMap iter = range.first;
	for( unsigned int j=0; j<count[i]; j++,iter++ ) {
	  if( i==0 ) {
	    tofDataAnalysis( (*iter).second, 4 );
	  }
	  else if( i==1 || i==2 ) {
	    tofDataAnalysis( (*iter).second, 5 );
	  }
	  else {
	    tofDataAnalysis( (*iter).second, 6 );
	  }
	}
      }
      countTot2 = countTot2 + count[i];
    }
  }

  if( countTot1==0 && countTot2==0 ) {
    m_hitCase = NoHit; 
    m_quality = 12;
  }

  return;
}


//------- tofDataAnalysis ----------------------------------------
// analysis tof data
//
void TofTrack::tofDataAnalysis( TofData* tof, unsigned int iflag ) {

  unsigned int qual = tof->quality();

  if( ( qual & 0x10 ) == 0 ) {
    qual = ( qual | 0x10 );     // zadc, ztdc unmatched, and track matched
    if( tof->barrel() || tof->is_mrpc() ) {  // Barrel, Endcap has been done
      if( ( tof->quality() == 0x7 ) || ( tof->quality() == 0xd ) ) {
	qual = ( qual | 0x20 );                       // lost one Q
      }
      
      if( ( tof->quality() == 0xb ) || ( tof->quality() == 0xe ) ) {
	qual = ( qual | 0x40 );                       // lost one T
      }
      
      if( ( tof->quality() == 0x3 ) || ( tof->quality() == 0xc ) ) {
	qual = ( qual | 0x80 );                       // single end
      }
    }
    if( tof->barrel() ) {
      if( ( tof->quality() & 0x5 ) == 0x5 ) {
	double ztdc = tofCaliSvc->ZTDC( tof->tdc1(), tof->tdc2(), tof->tofId() );
	tof->setZTdc( ztdc );
      }

      if( ( tof->quality() & 0xa ) == 0xa ) {
	double zadc = tofCaliSvc->ZADC( tof->adc1(), tof->adc2(), tof->tofId() );
	tof->setZAdc( zadc );
      }
    }
    if( tof->is_mrpc() ) {
      if( ( tof->quality() & 0x5 ) == 0x5 ) {
	double ztdc = tofCaliSvc->EtfZTDC( tof->tdc1(), tof->tdc2(), tof->tofId(), tof->strip()  );
	tof->setZTdc( ztdc );
      }
    }
    tof->setQuality( qual );
  }

  if( iflag == 1 )      { m_tofData1.push_back( tof ); }
  else if( iflag == 2 ) { m_tofData2.push_back( tof ); }
  else if( iflag == 3 ) { m_tofData3.push_back( tof ); }
  else if( iflag == 4 ) { m_tofData4.push_back( tof ); }
  else if( iflag == 5 ) { m_tofData5.push_back( tof ); }
  else if( iflag == 6 ) { m_tofData6.push_back( tof ); }
  else {
    cout << "TofRec::TofTrack::TofDataAnalylsis: the Flag should be 1-4, out of the Range!" << endl;
  }

  return;
}


//------- match --------------------------------------------------
// Tof Data - Extrapolated Track Match
//
void TofTrack::match( bool forCalibration, std::vector<int> deadId, std::vector<TofTrack*>*& tofTrackVec ) {

  if( m_hitCase == NoHit ) return;

  if( m_hitCase == InnerLayer ) {
    findTofDataBarrel( m_tofData1, m_tofData2, m_zrhit1, 1, tofTrackVec );
    if( ( m_quality1 & 0x10 ) == 0 ) { m_hitCase = NoHit; }
  }
  else if( m_hitCase == OuterLayer ) {
    findTofDataBarrel( m_tofData3, m_tofData4, m_zrhit2, 2, tofTrackVec );
    if( ( m_quality2 & 0x10 ) == 0 ) { m_hitCase = NoHit; }
  }
  else if( m_hitCase == DoubleLayer ) {
    findTofDataBarrel( m_tofData1, m_tofData2, m_zrhit1, 1, tofTrackVec );
    if( ( m_quality1 & 0x10 ) == 0 ) { m_hitCase = OuterLayer; }
    findTofDataBarrel( m_tofData3, m_tofData4, m_zrhit2, 2, tofTrackVec );
    if( ( m_quality2 & 0x10 ) == 0 ) {
      if( m_hitCase == DoubleLayer ) {
	m_hitCase = InnerLayer;
      }
      else if( m_hitCase == OuterLayer ) {
	m_hitCase = NoHit;
      }
      else {
	cout << "TofRec::TofTrack::match: 2- Impossible!" << endl;
      }
    }
  }
  else if( ( m_hitCase == EastEndcap ) || ( m_hitCase == WestEndcap ) ) {
    findTofDataEndcap( m_tofData1, m_tofData2, m_zr1 );
  }
  else if( ( m_hitCase == EastEndcapMRPC ) || ( m_hitCase == WestEndcapMRPC ) ) {
    findEtfData( m_tofData1, m_tofData2, m_tofData3, m_zrhit1, 1 );
    findEtfData( m_tofData4, m_tofData5, m_tofData6, m_zrhit2, 2 );
  }
  else {
    cout << "TofRec::TofTrack::match: 1- Impossible!" << endl; 
  }

  if( forCalibration ) {
    // set Data Sample for Calibration, double layer, only one hit for counter, T and Q.
    if( m_hitCase == DoubleLayer ) {
      if( ( ( m_quality1 & 0xf ) == 0xf ) && ( ( m_quality2 & 0xf ) == 0xf ) ) {
	m_quality1 = ( m_quality1 | 0x800 );        // Calibration Sample
	m_quality2 = ( m_quality2 | 0x800 );        // Calibration Sample
      }
      else {
	std::vector<int>::iterator iter = deadId.begin();
	for( ; iter != deadId.end(); iter++ ) {
	  Identifier iden = Identifier(*iter);
	  int barrel = TofID::barrel_ec(iden);
	  int layer  = TofID::layer(iden);
	  int tofId  = TofID::phi_module(iden);
	  int east   = TofID::end(iden);
	  if( barrel == 1 ) {
	    if( layer==0 ) {
	      if( m_tofId1 == tofId ) {
		if( ( m_quality2 & 0xf ) == 0xf ) {
		  if( ( ( east == 0 ) && ( ( ( m_quality1 & 0xf ) == 0x3 ) || ( ( m_quality1 & 0xf ) == 0x7 ) || ( ( m_quality1 & 0xf ) == 0xb ) ) ) || ( ( east == 1 ) && ( ( ( m_quality1 & 0xf ) == 0xc ) || ( ( m_quality1 & 0xf ) == 0xd ) || ( ( m_quality1 & 0xf ) == 0xe ) ) ) ) {
		    m_quality1 = ( m_quality1 | 0x800 );
		    m_quality2 = ( m_quality2 | 0x800 );

		  }
		}
	      }
	    }
	    else if( layer == 1 ) {
	      if( m_tofId2 == (tofId+88) ) {
		if( ( m_quality1 & 0xf ) == 0xf ) {
		  if( ( ( east == 0 ) && ( ( ( m_quality2 & 0xf ) == 0x3 ) || ( ( m_quality2 & 0xf ) == 0x7 ) || ( ( m_quality2 & 0xf ) == 0xb ) ) ) || ( ( east == 1 ) && ( ( ( m_quality2 & 0xf ) == 0xc ) || ( ( m_quality2 & 0xf ) == 0xd ) || ( ( m_quality2 & 0xf ) == 0xe ) ) ) ) {
		    m_quality1 = ( m_quality1 | 0x800 );
		    m_quality2 = ( m_quality2 | 0x800 );
		  }
		}
	      }
	    }
	  }
	}
      }
    }
    // set Data Sample for Calibration, only one hit for counter, T and Q.
    else if( m_hitCase == InnerLayer ) {
      if( ( m_quality1 & 0xf ) == 0xf ) {
	m_quality1 = ( m_quality1 | 0x800 );        // Calibration Sample
      }
      else {
	std::vector<int>::iterator iter = deadId.begin();
	for( ; iter != deadId.end(); iter++ ) {
	  Identifier iden = Identifier(*iter);
	  int barrel = TofID::barrel_ec(iden);
	  int layer  = TofID::layer(iden);
	  int tofId  = TofID::phi_module(iden);
	  int east   = TofID::end(iden);
	  if( barrel == 1 ) {
	    if( layer==0 ) {
	      if( m_tofId1 == tofId ) {
		if( ( ( east == 0 ) && ( ( ( m_quality1 & 0xf ) == 0x3 ) || ( ( m_quality1 & 0xf ) == 0x7 ) || ( ( m_quality1 & 0xf ) == 0xb ) ) ) || ( ( east == 1 ) && ( ( ( m_quality1 & 0xf ) == 0xc ) || ( ( m_quality1 & 0xf ) == 0xd ) || ( ( m_quality1 & 0xf ) == 0xe ) ) ) ) {
		  m_quality1 = ( m_quality1 | 0x800 );
		}
	      }
	    }
	  }
	}
      }
    }

    // set Data Sample for Calibration, only one hit for counter, T and Q.
    if( ( ( m_hitCase == EastEndcap ) || ( m_hitCase == WestEndcap ) ) && ( ( m_quality1 & 0xf ) == 0xc ) ) {
      m_quality1 = ( m_quality1 | 0x800 );        // Calibration Sample
    }

    // set Data Sample for Calibration.
    if( ( m_hitCase == EastEndcapMRPC ) || ( m_hitCase == WestEndcapMRPC ) ) {
      if( ( ( m_quality1 & 0xf000 ) == 0x1000 ) || ( ( m_quality1 & 0xf000 ) == 0x2000 ) ) { 
	if( ( m_quality1 & 0xf ) == 0xf ) {
	  m_quality1 = ( m_quality1 | 0x800 );        // Calibration Sample
	}
      }
      else if( ( ( m_quality1 & 0xf000 ) == 0x4000 ) ) { 
	std::vector<int>::iterator iter = deadId.begin();
	for( ; iter != deadId.end(); iter++ ) {
	  if( (*iter)==1 ) {
	    if( ( m_tofId1==3 && ( m_strip1==1 || m_strip1==3 || ( m_strip1>=5 && m_strip1<=11 ) ) ) || ( m_tofId1==13 && m_strip1==7 ) ) {
	      if( ( m_quality1 & 0xf ) == 0xc ) {
	      m_quality1 = ( m_quality1 | 0x800 );
	      }
	    }
	    if( m_tofId1==13 && m_strip1==5 ) {
	      if( ( m_quality1 & 0xf ) == 0x3 ) {
		m_quality1 = ( m_quality1 | 0x800 );
	      }
	    }
	  }
	  else {
	    Identifier iden = Identifier(*iter);
	    int barrel = TofID::barrel_ec( iden );
	    if( barrel == 3 ) {
	      int endcap = TofID::endcap( iden );
	      int tofid  = TofID::module( iden );
	      int strip  = TofID::strip(  iden );
	      int east   = TofID::end(    iden );
	      if( ( m_tofId1 == ( endcap*36 + tofid ) ) && ( m_strip1 == strip ) ) {
		if( ( ( east == 0 ) && ( ( ( m_quality1 & 0xf ) == 0x3 ) || ( ( m_quality1 & 0xf ) == 0x7 ) || ( ( m_quality1 & 0xf ) == 0xb ) ) ) || ( ( east == 1 ) && ( ( ( m_quality1 & 0xf ) == 0xc ) || ( ( m_quality1 & 0xf ) == 0xd ) || ( ( m_quality1 & 0xf ) == 0xe ) ) ) ) {
		  m_quality1 = ( m_quality1 | 0x800 ); 
		}
	      }
	    }
	  }
	}
      }

      if( ( ( m_quality2 & 0xf000 ) == 0x1000 ) || ( ( m_quality2 & 0xf000 ) == 0x2000 ) ) { 
	if( ( m_quality2 & 0xf ) == 0xf ) {
	  m_quality2 = ( m_quality2 | 0x800 );        // Calibration Sample
	}
      }
      else if( ( m_quality2 & 0xf000 ) == 0x4000 ) {
	std::vector<int>::iterator iter = deadId.begin();
	for( ; iter != deadId.end(); iter++ ) {
	  if( (*iter)==2 ) {
	    if( m_tofId2==34 && ( m_strip2>=0 && m_strip1<=5 ) ) {
	      if( ( m_quality2 & 0xf ) == 0x3 ) {
		m_quality2 = ( m_quality2 | 0x800 );
	      }
	    }
	  }
	  else {
	    Identifier iden = Identifier(*iter);
	    int barrel = TofID::barrel_ec( iden );
	    if( barrel == 3 ) {
	      int endcap = TofID::endcap( iden );
	      int tofid  = TofID::module( iden );
	      int strip  = TofID::strip(  iden );
	      int east   = TofID::end(    iden );
	      if( ( m_tofId2 == ( endcap*36 + tofid ) ) && ( m_strip2 == strip ) ) {
		if( ( ( east == 0 ) && ( ( ( m_quality2 & 0xf ) == 0x3 ) || ( ( m_quality2 & 0xf ) == 0x7 ) || ( ( m_quality2 & 0xf ) == 0xb ) ) ) || ( ( east == 1 ) && ( ( ( m_quality2 & 0xf ) == 0xc ) || ( ( m_quality2 & 0xf ) == 0xd ) || ( ( m_quality2 & 0xf ) == 0xe ) ) ) ) {
		  m_quality2 = ( m_quality2 | 0x800 ); 
		}
	      }
	    }
	  }
	}
      }
    }
  }

  return;
}


//------- findTofDataBarrel ----------------------------------------
// find the right TOF information in TOF data vector 
//      of the exact TOF ID and of the neighbor
//
void TofTrack::findTofDataBarrel( std::vector<TofData*> tofDataVec1, std::vector<TofData*> tofDataVec2, double zrhit, unsigned int iflag, std::vector<TofTrack*>*& tofTrackVec ) {

  unsigned int qual = 0xf;
  TofData* tof = 0;
  if( tofDataVec2.size() == 0 ) {
    if( tofDataVec1.size() == 0 ) {
      qual = 0;
    }
    else if( tofDataVec1.size() == 1 ) {
      std::vector<TofData*>::iterator iter1 = tofDataVec1.begin();
      tof = (*iter1);
      qual = 0x1;
    }
    else if( tofDataVec1.size() > 1 ) {
      tof= chooseTofData( tofDataVec1, zrhit ); 
      qual = 0x2;
    }
    else {
      cout << "TofRec::TofTrack::findTofDataBarrel: 1- Impossible!" << endl;
    }
  }
  else if( ( tofDataVec2.size() == 1 ) ) {
    if( tofDataVec1.size() == 0 ) {
      std::vector<TofData*>::iterator iter2 = tofDataVec2.begin();
      tof = (*iter2);
      qual = 0x4;
    }
    else if( tofDataVec1.size() == 1 ) {
      std::vector<TofData*>::iterator iter1 = tofDataVec1.begin();
      if( ((*iter1)->quality()&0x1ff)==0x01f && abs((*iter1)->ztdc()-zrhit)<ztdc_Cut ) {
	tof = (*iter1);
      }
      else {
	std::vector<TofData*>::iterator iter2 = tofDataVec2.begin();
        tof = compareTofData( (*iter1), (*iter2), zrhit );
      }
      qual = 0x5;
    }
    else if( tofDataVec1.size() > 1 ) {
      TofData* tofData1 = chooseTofData( tofDataVec1, zrhit ); 
      if( (tofData1->quality()&0x1ff)==0x01f && abs(tofData1->ztdc()-zrhit)<ztdc_Cut ) {
	tof = tofData1;
      }
      else {
	std::vector<TofData*>::iterator iter2 = tofDataVec2.begin();
	tof = compareTofData( tofData1, (*iter2), zrhit );
      }
      qual = 0x6;
    }
    else {
      cout << "TofRec::TofTrack::findTofDataBarrel: 2- Impossible!" << endl;
    }
  }
  else if( ( tofDataVec2.size() > 1 ) ) {
    if( tofDataVec1.size() == 0 ) {
      tof = chooseTofData( tofDataVec2, zrhit );
      qual = 0x8;
    }
    else if( tofDataVec1.size() == 1 ) {
      std::vector<TofData*>::iterator iter1 = tofDataVec1.begin();
      if( ((*iter1)->quality()&0x1ff)==0x01f && abs((*iter1)->ztdc()-zrhit)<ztdc_Cut ) {
	tof = (*iter1);
      }
      else {
	TofData* tofData2 = chooseTofData( tofDataVec2, zrhit ); 
	tof = compareTofData( (*iter1), tofData2, zrhit );
      }
      qual = 0x9;
    }
    else if( tofDataVec1.size() > 1 ) {
      TofData* tofData1 = chooseTofData( tofDataVec1, zrhit ); 
      if( (tofData1->quality()&0x1ff)==0x01f && abs(tofData1->ztdc()-zrhit)<ztdc_Cut ) {
	tof = tofData1;
      }
      else {
	TofData* tofData2 = chooseTofData( tofDataVec2, zrhit ); 
	tof = compareTofData( tofData1, tofData2, zrhit );
      }
      qual = 0xa;
    }
    else {
      cout << "TofRec::TofTrack::findTofDataBarrel: 3- Impossible!" << endl;
    }
  }

  if( qual != 0 ) {
    if( !(tof->used()) ) {
      getTofData( tof, iflag );
    }
    else {
      bool z1=false,  z2=false;
      bool zc1=false, zc2=false;
      TofTrack* track=0;
      if( iflag==1 ) {
	z1  = ( abs( m_zrhit1 - tof->ztdc() ) < ztdc_Cut );
	zc1 = ( m_zrhit1 > tof->ztdc() );
	std::vector<TofTrack*>::iterator iter = tofTrackVec->begin(); 
	for( ; iter!=tofTrackVec->end(); iter++ ) {
	  if( (*iter)->hitCase()!=InnerLayer && (*iter)->hitCase()!=DoubleLayer ) continue;
	  if( tof->tofId()==(*iter)->tofId1() ) {
	    track = (*iter);
	    z2    = ( abs( (*iter)->zrhit1() - tof->ztdc() ) < ztdc_Cut );
	    zc2   = ( (*iter)->zrhit1() > tof->ztdc() );
	  }
	}
      }
      else if( iflag==2 ) {
	z1  = ( abs( m_zrhit2 - tof->ztdc() ) < ztdc_Cut );
	zc1 = ( m_zrhit2 > tof->ztdc() );
	std::vector<TofTrack*>::iterator iter = tofTrackVec->begin(); 
	for( ; iter!=tofTrackVec->end(); iter++ ) {
 	  if( (*iter)->hitCase()!=OuterLayer && (*iter)->hitCase()!=DoubleLayer ) continue;
	  if( tof->tofId()==(*iter)->tofId2() ) {
	    track = (*iter);
	    z2    = ( abs( (*iter)->zrhit2() - tof->ztdc() ) < ztdc_Cut );
	    zc2   = ( (*iter)->zrhit2() > tof->ztdc() );
	  }
	}
      }
      
      if( ( z1 && z2 )||( (!z1) && (!z2) ) ) {
	if( zc1 && !zc2 ) {
	  getTofDataEast( tof, iflag );
	  track->getTofDataWest( tof, iflag );
	}
	else if( !zc1 && zc2 ) {
	  getTofDataWest( tof, iflag );
	  track->getTofDataEast( tof, iflag );
	}
      }
      else if( z1 && !z2 ) {
	getTofData( tof, iflag );
	track->getTofDataNohit( iflag );
      }
      else if( !z1 && z2 ) {
	qual = 0;
      }
    }
  }

  if( qual == 0 ) {
    if( ( iflag == 1 ) || ( iflag == 3 ) ) {
      m_quality1 = ( m_quality1 | 0x300 );
    }
    else if( iflag == 2 ) {
      m_quality2 = ( m_quality2 | 0x300 );
    }
    else {
      cout << "TofRec::TofTrack::findTofDataBarrel: the 1- IFLAG is Out of Range!" << endl;
    }
  }
  else {
    qual = ( qual << 12 );
    if( ( iflag == 1 ) || ( iflag == 3 ) ) {
      m_quality1 = ( m_quality1 | qual );
    }
    else if( iflag == 2 ) {
      m_quality2 = ( m_quality2 | qual );
    }
    else {
      cout << "TofRec::TofTrack::findTofDataBarrel: the 2- IFLAG is Out of Range!" << endl;
    }
  }

  return;
}


//------- chooseTofData -------------------------------------------
// choose the most possible TofData from TofDataVector
// ZTDC, ZADC, SingleEnd/NoQ/NoT are jugded.
//
TofData* TofTrack::chooseTofData( std::vector<TofData*> tofDataVec, double zrhit ) {
  if( tofDataVec.size() == 0 ) {
    cout << "TofRec::TofTrack::ChooseTofData: Size of TofData Vector is Zero!" << endl;
    return 0;
  }
  std::vector<TofData*>::iterator igood = tofDataVec.begin();
  if( tofDataVec.size() > 1 ) {
    double deltaZ = 1000.0;
    std::vector<TofData*>::iterator iter  = tofDataVec.begin();
    // ZTDC compare
    for( ; iter != tofDataVec.end(); iter++ ) {
      if( ( (*iter)->quality() & 0x5 ) == 0x5 ) {
	if( abs( (*iter)->ztdc() - zrhit ) < deltaZ ) {
	  deltaZ = abs( (*iter)->ztdc() - zrhit );
	  igood = iter;
	}
      }
    }
    // ZADC compare
    if( deltaZ > 999.0 ) {
      iter  = tofDataVec.begin();
      for( ; iter != tofDataVec.end(); iter++ ) {
	if( ( (*iter)->quality() & 0xa ) == 0xa ) {
	  if( abs( (*iter)->zadc() - zrhit ) < deltaZ ) {
	    deltaZ = abs( (*iter)->zadc() - zrhit );
	    igood = iter;
	  }
	}
      }
    }
    // Max Q
    if( deltaZ > 999.0 ) {
      unsigned int ibad = 0xf0;
      iter  = tofDataVec.begin();
      for( ; iter != tofDataVec.end(); iter++ ) {
	if( ( (*iter)->quality() & 0xf0 ) < ibad ) {
	  igood = iter;
	  ibad = ( (*iter)->quality() & 0xf0 );
	}
	else if( ( (*iter)->quality() & 0xf0 ) == ibad ) {
	  if( ( (*iter)->adc1() + (*iter)->adc2() ) > ( (*igood)->adc1() + (*igood)->adc2() ) ) {
	    igood = iter;
	    ibad = ( (*iter)->quality() & 0xf0 );
	  }
	}
      }
    }
  }

  return (*igood);
}


//------- compareTofData -------------------------------------------
// choose the most possible TofData from TofDataVector
// ZTDC, ZADC, SingleEnd/NoQ/NoT are jugded.
//
TofData* TofTrack::compareTofData( TofData* tofData1, TofData* tofData2, double zrhit ) {
  TofData* tof = tofData1;
  // ZTDC compare
  if( abs(tofData1->ztdc() - zrhit ) > abs(tofData2->ztdc() - zrhit ) ) {
    // SingleEnd/NoT/NoQ compare
    if( ( tofData1->quality() & 0xf0 ) >= ( tofData1->quality() & 0xf0 ) ) {
      // QDC compare
      //	if( ( tofData1->adc1() + tofData1->adc2() ) < ( tofData2->adc1() + tofData2->adc2() ) ) {
      tof = tofData2;
      //	}
    }
    //    }
  }

  return tof;
}


//------- findTofDataEndcap --------------------------------------
// find the right TOF information in TOF data vector 
//      of the exact TOF ID and of the neighbor
//
void TofTrack::findTofDataEndcap( std::vector<TofData*> tofDataVec1, std::vector<TofData*> tofDataVec2, double zr1[5] ) {

  unsigned int iflag = 3;
  unsigned int qual = 0xf;

  if( tofDataVec2.size() == 0 ) {
    if( tofDataVec1.size() == 0 ) {
      qual = 0;
    }
    else if( tofDataVec1.size() == 1 ) {
      std::vector<TofData*>::iterator iter1 = tofDataVec1.begin();
      getTofData( (*iter1), iflag );
      qual = 0x1;
    }
    else if( tofDataVec1.size() > 1 ) {
      getTofData( chooseTofDataEndcap( tofDataVec1, zr1 ), iflag ); 
      qual = 0x2;
    }
    else {
      cout << "TofRec::TofTrack::findTofDataEndcap: 1- Impossible!" << endl;
    }
  }
  else if( ( tofDataVec2.size() == 1 ) ) {
    if( tofDataVec1.size() == 0 ) {
      std::vector<TofData*>::iterator iter2 = tofDataVec2.begin();
      getTofData( (*iter2), iflag );
      qual = 0x4;
    }
    else if( tofDataVec1.size() == 1 ) {
      std::vector<TofData*>::iterator iter1 = tofDataVec1.begin();
      std::vector<TofData*>::iterator iter2 = tofDataVec2.begin();
      getTofData( compareTofDataEndcap( (*iter1), (*iter2) ), iflag );
      qual = 0x5;
    }
    else if( tofDataVec1.size() > 1 ) {
      TofData* tofData1 = chooseTofDataEndcap( tofDataVec1, zr1 ); 
      std::vector<TofData*>::iterator iter2 = tofDataVec2.begin();
      getTofData( compareTofDataEndcap( tofData1, (*iter2) ), iflag );
      qual = 0x6;
    }
    else {
      cout << "TofRec::TofTrack::findTofDataBarrel: 2- Impossible!" << endl;
    }
  }
  else if( ( tofDataVec2.size() > 1 ) ) {
    if( tofDataVec1.size() == 0 ) {
      getTofData( chooseTofDataEndcap( tofDataVec2, zr1 ), iflag ); 
      qual = 0x8;
    }
    else if( tofDataVec1.size() == 1 ) {
      std::vector<TofData*>::iterator iter1 = tofDataVec1.begin();
      TofData* tofData2 = chooseTofDataEndcap( tofDataVec2, zr1 ); 
      getTofData( compareTofDataEndcap( (*iter1), tofData2 ), iflag );
      qual = 0x9;
    }
    else if( tofDataVec1.size() > 1 ) {
      TofData* tofData1 = chooseTofDataEndcap( tofDataVec1, zr1 ); 
      TofData* tofData2 = chooseTofDataEndcap( tofDataVec2, zr1 ); 
      getTofData( compareTofDataEndcap( tofData1, tofData2 ), iflag );
      qual = 0xa;
    }
    else {
      cout << "TofRec::TofTrack::findTofDataBarrel: 3- Impossible!" << endl;
    }
  }

  if( qual == 0 ) {
    m_quality1 = ( m_quality1 | 0x300 );
  }
  else {
    qual = ( qual << 12 );
    m_quality1 = ( m_quality1 | qual );
  }

  return;
}


//------- chooseTofDataEndcap --------------------------------
// choose the most possible TofData from TofDataVector
//
TofData* TofTrack::chooseTofDataEndcap( std::vector<TofData*> tofDataVec, double zr1[5] ) {
  if( tofDataVec.size() == 0 ) {
    cout << "TofRec::TofTrack::ChooseTofData: Size of TofData Vector is Zero!" << endl;
    return 0;
  }
  std::vector<TofData*>::iterator igood = tofDataVec.begin();
  if( tofDataVec.size() > 1 ) {
    bool multihit = false;
    std::vector<TofData*>::iterator iter  = tofDataVec.begin();
    for( ; iter != tofDataVec.end(); iter++ ) {
      if( (*iter)->qtimes1()>1 ) { multihit = true; }
    }
    iter = tofDataVec.begin();
    if( multihit ) {
      double tcorr     = -999.0;
      double deltaTMin = 999.0;
      for( ; iter != tofDataVec.end(); iter++ ) {
	tcorr = tofCaliSvc->ETime( (*iter)->adc(), (*iter)->tdc()-m_estime, zr1[2], (*iter)->tofId() );
	for( unsigned int i=0; i<5; i++ ) {
	  if( abs(tcorr-m_texpInner[i]) < deltaTMin ) {
	    deltaTMin = abs(tcorr-m_texpInner[i]);
	    igood     = iter;
	  }
	}
      }
    }
    else {
      double maxQ = 0.0;
      for( ; iter != tofDataVec.end(); iter++ ) {
	if( (*iter)->adc() > maxQ ) {
	  maxQ  = (*iter)->adc();
	  igood = iter;
	}
      }
    }
  }
  return (*igood);
} 


//------- compareTofDataEndcap -------------------------------
// choose the most possible TofData from TofDataVector
//
TofData* TofTrack::compareTofDataEndcap( TofData* tofData1, TofData* tofData2 ) {
  TofData* tof = tofData1;
  if( tof->adc() < tofData2->adc() ) {
    tof = tofData2;
  }
  return tof;
}


//------- findTofDataEtf --------------------------------------
// find the right TOF information in TOF data vector 
//      of the exact TOF ID and of the neighbor
//
void TofTrack::findEtfData( std::vector<TofData*> tofDataVec1, std::vector<TofData*> tofDataVec2, std::vector<TofData*> tofDataVec3, double zrhit, unsigned int iflag ) {

  TofData *tof1 = 0;
  TofData *tof2 = 0;
  TofData *tof3 = 0;

  bool findSignal = false;

  if( tofDataVec1.size()==0 && tofDataVec2.size()==0 && tofDataVec3.size()==0 ) {
    if( iflag == 1 ) {
      m_quality1 = ( m_quality1 | 0x300 );
    }
    else if( iflag == 2 ) {
      m_quality2 = ( m_quality2 | 0x300 );
    }
  }
  else {
    if( tofDataVec1.size()>0 ) {
      tof1 = chooseEtfData1( tofDataVec1, zrhit );
      if( ( tof1->quality() & 0xf ) == 0xf ) {
	getEtfData( tof1, iflag, 1 );
	findSignal = true;
      }
    }
    if( !findSignal && tofDataVec2.size() > 0 ) {
      tof2 = chooseEtfData1( tofDataVec2, zrhit );
      if( ( tof2->quality() & 0xf ) == 0xf ) {
	getEtfData( tof2, iflag, 2 );
	findSignal = true;
      }
    }
    if( !findSignal && tofDataVec3.size() > 0 ) {
      tof3 = chooseEtfData2( tofDataVec3, zrhit );
      if( ( tof3->quality() & 0xf ) == 0xf ) {
	getEtfData( tof3, iflag, 3 );
	findSignal = true;
      }
    }
    if( !findSignal && tofDataVec1.size()>0 ) {
      if( ( ( tof1->quality() & 0xf ) == 0xc ) || ( ( tof1->quality() & 0xf ) == 0x3 ) )  {
	getEtfData( tof1, iflag, 4 );
	findSignal = true;
      }
    }
    if( !findSignal && tofDataVec2.size() > 0 ) {
      if( ( ( tof2->quality() & 0xf ) == 0xc ) || ( ( tof2->quality() & 0xf ) == 0x3 ) )  {
	getEtfData( tof2, iflag, 5 );
	findSignal = true;
      }
    }
    if( !findSignal && tofDataVec3.size() > 0 ) {
      if( ( ( tof3->quality() & 0xf ) == 0xc ) || ( ( tof3->quality() & 0xf ) == 0x3 ) )  {
	getEtfData( tof3, iflag, 6 );
	findSignal = true;
      }
    }
    if( findSignal ) {
      if( iflag == 1 ) {
	m_quality1 = ( m_quality1 | 0x300 );
      }
      else if( iflag == 2 ) {
	m_quality2 = ( m_quality2 | 0x300 );
      }
    }
  }

  return;
}

//------- chooseEtfData -------------------------------
// choose the most possible TofData from TofDataVector
//
TofData* TofTrack::chooseEtfData1( std::vector<TofData*> tofDataVec, double zrhit ) {
  if( tofDataVec.size() == 0 ) {
    return 0;
  }
  std::vector<TofData*>::iterator igood = tofDataVec.begin();
  if( tofDataVec.size() == 1 ) {
    return (*igood);
  }
  else if( tofDataVec.size() > 1 ) {
    double deltaZ = 1000.0;
    std::vector<TofData*>::iterator iter  = tofDataVec.begin();
    for( ; iter != tofDataVec.end(); iter++ ) {
      if( ( (*iter)->quality() & 0xf ) == 0xf ) {
	if( abs( (*iter)->ztdc() - zrhit ) < deltaZ ) {
	  deltaZ = abs( (*iter)->ztdc() - zrhit );
	  igood = iter;
	}
      }
    }
    // Max Q
    if( deltaZ > 999.0 ) {
      double maxQ = -1;
      iter  = tofDataVec.begin();
      for( ; iter != tofDataVec.end(); iter++ ) {
	if( ( (*iter)->quality() & 0xc ) == 0xc ) {
	  if( (*iter)->adc1() > maxQ ) {
	    maxQ =  (*iter)->adc1();
	    igood = iter;
	  }
	}
	else if( ( (*iter)->quality() & 0x3 ) == 0x3 ) {
	  if( (*iter)->adc2() > maxQ ) {
	    maxQ =  (*iter)->adc2();
	    igood = iter;
	  }
	}
      }
    }
  }

  return (*igood);
}

//------- chooseEtfData -------------------------------
// choose the most possible TofData from TofDataVector
//
TofData* TofTrack::chooseEtfData2( std::vector<TofData*> tofDataVec, double zrhit ) {
  if( tofDataVec.size() == 0 ) {
    return 0;
  }
  std::vector<TofData*>::iterator igood = tofDataVec.begin();
  if( tofDataVec.size() == 1 ) {
    return (*igood);
  }
  else if( tofDataVec.size() > 1 ) {
    double maxQ = -1;
    std::vector<TofData*>::iterator iter = tofDataVec.begin();
    for( ; iter != tofDataVec.end(); iter++ ) {
      if( ( (*iter)->quality() & 0xc ) == 0xc ) {
	if( (*iter)->adc1() > maxQ ) {
	  maxQ =  (*iter)->adc1();
	  igood = iter;
	}
      }
      else if( ( (*iter)->quality() & 0x3 ) == 0x3 ) {
	if( (*iter)->adc2() > maxQ ) {
	  maxQ =  (*iter)->adc2();
	  igood = iter;
	}
      }
    }
  }

  return (*igood);
}

//------- getTofData -----------------------------------------
// set Tof Data of Inner / Outer Layer Barrel TOF and Endcap TOF
//
void TofTrack::getTofData( TofData* tof, unsigned int iflag ) {

  if( iflag == 1 ) { 
    m_tofId1   = tof->tofId();
    m_strip1   = tof->strip();
    if( tofCaliSvc->QElec() ) {
      m_qch1   = tof->qtc1();  
    }
    else {
      m_qch1   = tof->adcChannelEast();
    }
    m_adc1     = tof->adc1();
    m_tdc1     = tof->tdc1();
    if( tofCaliSvc->QElec() ) {
      m_qch2   = tof->qtc2();  
    }
    else {
      m_qch2   = tof->adcChannelWest();
    }
    m_adc2     = tof->adc2();
    m_tdc2     = tof->tdc2();
    m_ztdc1    = tof->ztdc();
    m_zadc1    = tof->zadc();
    m_quality1 = ( m_quality1 | ( 0x1f & tof->quality() ) );
    if( ( ( tof->quality() & 0x5 ) != 0x5 ) || ( ( ( tof->quality() & 0x5 ) == 0x5 ) && ( abs( tof->ztdc() - m_zrhit1 ) > ztdc_Cut ) ) ) { 
      m_quality1 = ( m_quality1 | 0x100 );
    }
    if( ( ( tof->quality() & 0xa ) != 0xa ) || ( ( ( tof->quality() & 0xa ) == 0xa ) && ( abs( tof->zadc() - m_zrhit1 ) > zadc_Cut ) ) ) { 
      m_quality1 = ( m_quality1 | 0x200 );
    }
  }
  else if( iflag == 2 ) {
    m_tofId2   = tof->tofId();
    m_strip2   = tof->strip();
    if( tofCaliSvc->QElec() ) {
      m_qch3   = tof->qtc1();  
    }
    else {
      m_qch3   = tof->adcChannelEast();
    }
    m_adc3     = tof->adc1();
    m_tdc3     = tof->tdc1();
    if( tofCaliSvc->QElec() ) {
      m_qch4   = tof->qtc2();  
    }
    else {
      m_qch4   = tof->adcChannelWest();
    }
    m_adc4     = tof->adc2();
    m_tdc4     = tof->tdc2();
    m_ztdc2    = tof->ztdc();
    m_zadc2    = tof->zadc();
    m_quality2 = ( m_quality2 | ( 0x1f & tof->quality() ) );
    if( ( ( tof->quality() & 0x5 ) != 0x5 ) || ( ( ( tof->quality() & 0x5 ) == 0x5 ) && ( abs( tof->ztdc() - m_zrhit2 ) > ztdc_Cut ) ) ) { 
      m_quality2 = ( m_quality2 | 0x100 );
    }
    if( ( ( tof->quality() & 0xa ) != 0xa ) || ( ( ( tof->quality() & 0xa ) == 0xa ) && ( abs( tof->zadc() - m_zrhit2 ) > zadc_Cut ) ) ) { 
      m_quality2 = ( m_quality2 | 0x200 );
    }
  }
  else if( iflag == 3 ) {
    m_tofId1   = tof->tofId();
    if( tofCaliSvc->QElec() ) {
      m_qch1   = tof->qtc();  
    }
    else {
      m_qch1   = tof->adcChannel();
    }
    m_adc1     = tof->adc();
    m_tdc1     = tof->tdc();
    m_quality1 = ( m_quality1 | ( 0x1f & tof->quality() ) );
    m_quality1 = ( m_quality1 | 0x300 );
  }
  else {
    cout << "TofRec::TofTrack::getTofData: Flag which sign the Barrel/Endcap or Inner/Outer is wrong! Please check it!" << endl;
  }
  tof->setUsed();
  return;
}


//------- getTofDataEast ------------------------------------
// set Tof Data of Inner / Outer Layer Barrel TOF and Endcap TOF
//
void TofTrack::getTofDataEast( TofData* tof, unsigned int iflag ) {

  if( iflag == 1 ) { 
    m_tofId1   = tof->tofId();
    m_strip1   = tof->strip();
    if( tofCaliSvc->QElec() ) {
      m_qch1   = tof->qtc1();  
    }
    else {
      m_qch1   = tof->adcChannelEast();
    }
    m_adc1     = tof->adc1();
    m_tdc1     = tof->tdc1();
    m_qch2     = -999.0;
    m_adc2     = -999.0;
    m_tdc2     = -999.0;
    m_ztdc1    = tof->ztdc();
    m_zadc1    = tof->zadc();
    m_quality1 = ( ( m_quality1 & 0xfffffff0 ) | ( 0x1c & tof->quality() ) );
    if( ( ( tof->quality() & 0x5 ) != 0x5 ) || ( ( ( tof->quality() & 0x5 ) == 0x5 ) && ( abs( tof->ztdc() - m_zrhit1 ) > ztdc_Cut ) ) ) { 
      m_quality1 = ( m_quality1 | 0x100 );
    }
    if( ( ( tof->quality() & 0xa ) != 0xa ) || ( ( ( tof->quality() & 0xa ) == 0xa ) && ( abs( tof->zadc() - m_zrhit1 ) > zadc_Cut ) ) ) { 
      m_quality1 = ( m_quality1 | 0x200 );
    }
  }
  else if( iflag == 2 ) {
    m_tofId2   = tof->tofId();
    if( tofCaliSvc->QElec() ) {
      m_qch3   = tof->qtc1();  
    }
    else {
      m_qch3   = tof->adcChannelEast();
    }
    m_adc3     = tof->adc1();
    m_tdc3     = tof->tdc1();
    m_qch4     = -999.0;
    m_adc4     = -999.0;
    m_tdc4     = -999.0;
    m_ztdc2    = tof->ztdc();
    m_zadc2    = tof->zadc();
    m_quality2 = ( ( m_quality2 & 0xfffffff0 ) | ( 0x1c & tof->quality() ) );
    if( ( ( tof->quality() & 0x5 ) != 0x5 ) || ( ( ( tof->quality() & 0x5 ) == 0x5 ) && ( abs( tof->ztdc() - m_zrhit2 ) > ztdc_Cut ) ) ) { 
      m_quality2 = ( m_quality2 | 0x100 );
    }
    if( ( ( tof->quality() & 0xa ) != 0xa ) || ( ( ( tof->quality() & 0xa ) == 0xa ) && ( abs( tof->zadc() - m_zrhit2 ) > zadc_Cut ) ) ) { 
      m_quality2 = ( m_quality2 | 0x200 );
    }
  }
  else {
    cout << "TofRec::TofTrack::getTofDataEast: Flag which sign the Barrel/Endcap or Inner/Outer is wrong! Please check it!" << endl;
  }
  tof->setUsed();
  return;
}


//------- getTofDataWest ------------------------------------
// set Tof Data of Inner / Outer Layer Barrel TOF and Endcap TOF
//
void TofTrack::getTofDataWest( TofData* tof, unsigned int iflag ) {

  if( iflag == 1 ) { 
    m_tofId1   = tof->tofId();
    m_strip1   = tof->strip();
    m_qch1     = -999.0;
    m_adc1     = -999.0;
    m_tdc1     = -999.0;
    if( tofCaliSvc->QElec() ) {
      m_qch2   = tof->qtc2();  
    }
    else {
      m_qch2   = tof->adcChannelWest();
    }
    m_adc2     = tof->adc2();
    m_tdc2     = tof->tdc2();
    m_ztdc1    = tof->ztdc();
    m_zadc1    = tof->zadc();
    m_quality1 = ( ( m_quality1 & 0xfffffff0 ) | ( 0x13 & tof->quality() ) );
    if( ( ( tof->quality() & 0x5 ) != 0x5 ) || ( ( ( tof->quality() & 0x5 ) == 0x5 ) && ( abs( tof->ztdc() - m_zrhit1 ) > ztdc_Cut ) ) ) { 
      m_quality1 = ( m_quality1 | 0x100 );
    }
    if( ( ( tof->quality() & 0xa ) != 0xa ) || ( ( ( tof->quality() & 0xa ) == 0xa ) && ( abs( tof->zadc() - m_zrhit1 ) > zadc_Cut ) ) ) { 
      m_quality1 = ( m_quality1 | 0x200 );
    }
  }
  else if( iflag == 2 ) {
    m_tofId2   = tof->tofId();
    m_qch3     = -999.0;
    m_adc3     = -999.0;
    m_tdc3     = -999.0;
    if( tofCaliSvc->QElec() ) {
      m_qch4   = tof->qtc2();  
    }
    else {
      m_qch4   = tof->adcChannelWest();
    }
    m_adc4     = tof->adc2();
    m_tdc4     = tof->tdc2();
    m_ztdc2    = tof->ztdc();
    m_zadc2    = tof->zadc();
    m_quality2 = ( ( m_quality2 & 0xfffffff0 ) | ( 0x13 & tof->quality() ) );
    if( ( ( tof->quality() & 0x5 ) != 0x5 ) || ( ( ( tof->quality() & 0x5 ) == 0x5 ) && ( abs( tof->ztdc() - m_zrhit2 ) > ztdc_Cut ) ) ) { 
      m_quality2 = ( m_quality2 | 0x100 );
    }
    if( ( ( tof->quality() & 0xa ) != 0xa ) || ( ( ( tof->quality() & 0xa ) == 0xa ) && ( abs( tof->zadc() - m_zrhit2 ) > zadc_Cut ) ) ) { 
      m_quality2 = ( m_quality2 | 0x200 );
    }
  }
  else {
    cout << "TofRec::TofTrack::getTofDataWest: Flag which sign the Barrel/Endcap or Inner/Outer is wrong! Please check it!" << endl;
  }
  tof->setUsed();
  return;
}


//------- getTofData -----------------------------------------
// set Tof Data of Inner / Outer Layer Barrel TOF and Endcap TOF
//
void TofTrack::getTofDataNohit( unsigned int iflag ) {

  if( iflag == 1 ) { 
    m_tofId1   = -99;
    m_strip1   = -99;
    m_qch1     = -999.0;
    m_adc1     = -999.0;
    m_tdc1     = -999.0;
    m_qch2     = -999.0;
    m_adc2     = -999.0;
    m_tdc2     = -999.0;
    m_ztdc1    = -999.0;
    m_zadc1    = -999.0;
    m_quality1 = ( m_quality1 & 0x700 );
    if( m_hitCase == InnerLayer ) { m_hitCase = NoHit; }
    else if( m_hitCase == DoubleLayer ) { m_hitCase = OuterLayer; }
  }
  else if( iflag == 2 ) {
    m_tofId2   = -99;
    m_qch3     = -999.0;
    m_adc3     = -999.0;
    m_tdc3     = -999.0;
    m_qch4     = -999.0;
    m_adc4     = -999.0;
    m_tdc4     = -999.0;
    m_ztdc2    = -999.0;
    m_zadc2    = -999.0;
    m_quality2 = ( m_quality2 & 0x700 );
    if( m_hitCase == OuterLayer ) { m_hitCase = NoHit; }
    else if( m_hitCase == DoubleLayer ) { m_hitCase = InnerLayer; }
  }
  else {
    cout << "TofRec::TofTrack::getTofData: Flag which sign the Barrel/Endcap or Inner/Outer is wrong! Please check it!" << endl;
  }

  return;
}


//------- getTofData -----------------------------------------
// set Tof Data of MRPC Endcap TOF
//
void TofTrack::getEtfData( TofData* tof, unsigned int iflag, unsigned int qual ) {

  if( iflag == 1 && tof->tofId() != m_id2 ) {
    m_tofId1   = tof->tofId();
    m_strip1   = tof->strip();
    m_qch1     = tof->adcChannelEast();
    m_adc1     = tof->adc1();
    m_tdc1     = tof->tdc1();
    m_qch2     = tof->adcChannelWest();
    m_adc2     = tof->adc2();
    m_tdc2     = tof->tdc2();
    m_ztdc1    = tof->ztdc();
    m_zadc1    = tof->zadc();
    m_quality1 = ( m_quality1 | ( 0x1f & tof->quality() ) );
    if( ( ( tof->quality() & 0x5 ) != 0x5 ) || ( ( ( tof->quality() & 0x5 ) == 0x5 ) && ( abs( tof->ztdc() - m_zrhit1 ) > ztdc_Cut ) ) ) { 
      m_quality1 = ( m_quality1 | 0x100 );
    }
    if( ( ( tof->quality() & 0xa ) != 0xa ) || ( ( ( tof->quality() & 0xa ) == 0xa ) && ( abs( tof->zadc() - m_zrhit1 ) > zadc_Cut ) ) ) { 
      m_quality1 = ( m_quality1 | 0x200 );
    }
    m_quality1 = ( m_quality1 | ( qual << 12 ) );
    tof->setUsed();
    if( abs(tof->tofId()-m_id1)== 1 || ( tof->tofId()==0 && m_id1==35 ) || ( tof->tofId()==35 && m_id1==0 ) || ( tof->tofId()==36 && m_id1==71 ) || ( tof->tofId()==71 && m_id1==36 ) ) {
      for( unsigned int i=0; i<5; i++ ) {
	m_texpInner[i] = m_texpOuter[i];
      }
    }
  }
  else if( iflag == 2 && tof->tofId() != m_id1 ) {
    m_tofId2   = tof->tofId();
    m_strip2   = tof->strip();
    m_qch3     = tof->adcChannelEast();
    m_adc3     = tof->adc1();
    m_tdc3     = tof->tdc1();
    m_qch4     = tof->adcChannelWest();
    m_adc4     = tof->adc2();
    m_tdc4     = tof->tdc2();
    m_ztdc2    = tof->ztdc();
    m_zadc2    = tof->zadc();
    m_quality2 = ( m_quality2 | ( 0x1f & tof->quality() ) );
    if( ( ( tof->quality() & 0x5 ) != 0x5 ) || ( ( ( tof->quality() & 0x5 ) == 0x5 ) && ( abs( tof->ztdc() - m_zrhit2 ) > ztdc_Cut ) ) ) { 
      m_quality2 = ( m_quality2 | 0x100 );
    }
    if( ( ( tof->quality() & 0xa ) != 0xa ) || ( ( ( tof->quality() & 0xa ) == 0xa ) && ( abs( tof->zadc() - m_zrhit2 ) > zadc_Cut ) ) ) { 
      m_quality2 = ( m_quality2 | 0x200 );
    }
    m_quality2 = ( m_quality2 | ( qual << 12 ) );
    tof->setUsed();
    if( abs(tof->tofId()-m_id2)== 1 || ( tof->tofId()==0 && m_id2==35 ) || ( tof->tofId()==35 && m_id2==0 ) || ( tof->tofId()==36 && m_id2==71 ) || ( tof->tofId()==71 && m_id2==36 ) ) {
      for( unsigned int i=0; i<5; i++ ) {
	m_texpOuter[i] = m_texpInner[i];
      }
    }
  }

  return;
}


//------- setCalibration() ----------------------------------------
//  get calibration constants 
//  ph11() - ph22(), tof11() -- tof22()
//  ph1() ph2() ph(), tof1() tof2() tof()
//
void TofTrack::setCalibration() {

  bool barrel = ( ( m_hitCase == InnerLayer ) || ( m_hitCase == OuterLayer ) || ( m_hitCase == DoubleLayer ) );
  bool endcap = ( ( m_hitCase == EastEndcap ) || ( m_hitCase == WestEndcap ) );
  bool endcapMRPC = ( ( m_hitCase == EastEndcapMRPC ) || ( m_hitCase == WestEndcapMRPC ) );

  bool innerEast  = ( ( m_quality1 & 0xc ) == 0xc );
  bool innerWest  = ( ( m_quality1 & 0x3 ) == 0x3 );
  bool outerEast  = ( ( m_quality2 & 0xc ) == 0xc );
  bool outerWest  = ( ( m_quality2 & 0x3 ) == 0x3 );
  bool innerLayer = ( ( m_quality1 & 0xf ) == 0xf );
  bool outerLayer = ( ( m_quality2 & 0xf ) == 0xf );

  bool endcapData = ( ( m_quality1 & 0xc ) == 0xc );

  if( m_hitCase == DoubleLayer ) {
    for( unsigned int i=0; i<5; i++ ) {
      m_texp[i] = tofCaliSvc->BTimeCluster( m_texpInner[i], m_texpOuter[i], m_zr1[i], m_zr2[i], m_tofId1, m_tofId2 );
    }
    m_path    =  tofCaliSvc->BTimeCluster( m_path1, m_path2, m_zrhit1, m_zrhit2, m_tofId1, m_tofId2 );
  }

  if( barrel ) {
    if( innerEast ) {
      for( unsigned int i=0; i<5; i++ ) {
	m_tof11[i] = tofCaliSvc->BTime1( m_adc1, m_tdc1-m_estime, m_zr1[i], m_tofId1, m_estime );
      }
      m_sigma11 = tofCaliSvc->BSigma1( m_zrhit1, m_tofId1 );
      m_ph11 = m_adc1;
    }
    
    if( innerWest ) {
      for( unsigned int i=0; i<5; i++ ) {
	m_tof12[i] = tofCaliSvc->BTime2( m_adc2, m_tdc2-m_estime, m_zr1[i], m_tofId1, m_estime );
      }
      m_sigma12 = tofCaliSvc->BSigma2( m_zrhit1, m_tofId1 );
      m_ph12 = m_adc2;
    }

    if( innerLayer ) {
      for( unsigned int i=0; i<5; i++ ) {
	m_tof1[i]  = tofCaliSvc->BTimeCounter( m_tof11[i], m_tof12[i], m_zr1[i], m_tofId1 );
      }
      m_sigma1  = tofCaliSvc->BSigmaCounter( m_zrhit1, m_tofId1 );
      m_ph1  = tofCaliSvc->BPulseHeight( m_adc1, m_adc2, m_zrhit1, m_theta1, m_tofId1 );
    }

    if( outerEast ) {
      for( unsigned int i=0; i<5; i++ ) {
	m_tof21[i] = tofCaliSvc->BTime1( m_adc3, m_tdc3-m_estime, m_zr2[i], m_tofId2, m_estime );
      }
      m_sigma21 = tofCaliSvc->BSigma1( m_zrhit2, m_tofId2 );
      m_ph21 = m_adc3;
    }

    if( outerWest ) {
      for( unsigned int i=0; i<5; i++ ) {
	m_tof22[i] = tofCaliSvc->BTime2( m_adc4, m_tdc4-m_estime, m_zr2[i], m_tofId2, m_estime );
      }
      m_sigma22 = tofCaliSvc->BSigma2( m_zrhit2, m_tofId2 );
      m_ph22 = m_adc4;
    }

    if( outerLayer ) {
      for( unsigned int i=0; i<5; i++ ) {
	m_tof2[i]  = tofCaliSvc->BTimeCounter( m_tof21[i], m_tof22[i], m_zr2[i], m_tofId2 );
      }
      m_sigma2  = tofCaliSvc->BSigmaCounter( m_zrhit2, m_tofId2 );
      m_ph2  = tofCaliSvc->BPulseHeight( m_adc3, m_adc4, m_zrhit2, m_theta2, m_tofId2 );
    }

    if( innerLayer && outerLayer ) {
      for( unsigned int i=0; i<5; i++ ) {
	m_tof[i] = tofCaliSvc->BTimeCluster( m_tof1[i], m_tof2[i], m_zr1[i], m_zr2[i], m_tofId1, m_tofId2 );
      }
      m_sigma = tofCaliSvc->BSigmaCluster( m_zrhit1, m_zrhit2, m_tofId1, m_tofId2 );
      m_ph    = tofCaliSvc->BTimeCluster( m_ph1, m_ph2, m_zrhit1, m_zrhit2, m_tofId1, m_tofId2 );
    }
  }

  if( endcap ) {
    if( endcapData ) {
      for( unsigned int i=0; i<5; i++ ) {
	m_tof11[i] = tofCaliSvc->ETime( m_adc1, m_tdc1-m_estime, m_zr1[i], m_tofId1 );
      }
      m_sigma11 = tofCaliSvc->ESigma( m_zrhit1, m_tofId1 );
      m_ph11  = tofCaliSvc->EPulseHeight( m_adc1, m_zrhit1, m_theta1, m_tofId1 );
      m_quality = 1;
      if( (m_quality1&0xa000)!=0 ) { m_quality = 4; }
    }
  }

  if( endcapMRPC ) {
    if( innerEast ) {
      if( m_tofId1>-1 ) {
	for( unsigned int i=0; i<5; i++ ) {
	  if( m_run > 0 ) {
	    m_tof11[i] = tofCaliSvc->EtfTime1( m_adc1, m_tdc1-m_estime, m_zr1[i], m_tofId1, m_strip1, m_estime );
	  }
	  else {
	    m_tof11[i] = tofCaliSvc->EtfTimeMC1( m_adc1, m_tdc1-m_estime, m_zr1[i], m_tofId1, m_strip1, m_estime );
	  }
	}
	m_ph11 = m_adc1;
      }
    }
    if( innerWest ) {
      if( m_tofId1>-1 ) {
	for( unsigned int i=0; i<5; i++ ) {
	  if( m_run > 0 ) {
	    m_tof12[i] = tofCaliSvc->EtfTime2( m_adc2, m_tdc2-m_estime, m_zr1[i], m_tofId1, m_strip1, m_estime );
	  }
	  else {
	    m_tof12[i] = tofCaliSvc->EtfTimeMC2( m_adc2, m_tdc2-m_estime, m_zr1[i], m_tofId1, m_strip1, m_estime );
	  }
	}
	m_ph12 = m_adc2;
      }
    }
    if( outerEast ) {
      if( m_tofId2>-1 ) {
	for( unsigned int i=0; i<5; i++ ) {
	  if( m_run > 0 ) {
	    m_tof21[i] = tofCaliSvc->EtfTime1( m_adc3, m_tdc3-m_estime, m_zr2[i], m_tofId2, m_strip2, m_estime );
	  }
	  else {
	    m_tof21[i] = tofCaliSvc->EtfTimeMC1( m_adc3, m_tdc3-m_estime, m_zr2[i], m_tofId2, m_strip2, m_estime );
	  }
	}
	m_ph21 = m_adc3;
      }
    }
    if( outerWest ) {
      if( m_tofId2>-1 ) {
	for( unsigned int i=0; i<5; i++ ) {
	  if( m_run > 0 ) {
	    m_tof22[i] = tofCaliSvc->EtfTime2( m_adc4, m_tdc4-m_estime, m_zr2[i], m_tofId2, m_strip2, m_estime );
	  }
	  else {
	    m_tof22[i] = tofCaliSvc->EtfTimeMC2( m_adc4, m_tdc4-m_estime, m_zr2[i], m_tofId2, m_strip2, m_estime );
	  }
	}
	m_ph22 = m_adc4;
      }
    }
    if( innerLayer ) {
      if( m_tofId1>-1 ) {
	m_tof1[0] = tofCaliSvc->EtfTime( m_tof11[0], m_tof12[0] );
	//	if( m_run > 0 ) {
	//	  m_tof1[0] = tofCaliSvc->EtfTime( m_adc1, m_adc2, m_tdc1-m_estime, m_tdc2-m_estime, m_tofId1, m_strip1, m_estime );
	//	}
	//	else {
	//	  m_tof1[0] = tofCaliSvc->EtfTimeMC( m_adc1, m_adc2, m_tdc1-m_estime, m_tdc2-m_estime, m_tofId1, m_strip1, m_estime );
	//	  m_tof1[0] = tofCaliSvc->EtfTime( m_tof11[0], m_tof12[0] );
	//	}
	for( unsigned int i=1; i<5; i++ ) {
	  m_tof1[i] = m_tof1[0];
	}
	m_ph1 = ( m_adc1 + m_adc2 )/2.0;
      }
    }
    if( outerLayer ) {
      if( m_tofId2>-1 ) {
	m_tof2[0] = tofCaliSvc->EtfTime( m_tof21[0], m_tof22[0] );
	//	if( m_run > 0 ) {
	//	  m_tof2[0] = tofCaliSvc->EtfTime( m_adc3, m_adc4, m_tdc3-m_estime, m_tdc4-m_estime, m_tofId2, m_strip2, m_estime );
	//	}
	//	else {
	//	  m_tof2[0] = tofCaliSvc->EtfTimeMC( m_adc3, m_adc4, m_tdc3-m_estime, m_tdc4-m_estime, m_tofId2, m_strip2, m_estime );
	//	}
	for( unsigned int i=1; i<5; i++ ) {
	  m_tof2[i] = m_tof2[0];
	}
	m_ph2 = ( m_adc3 + m_adc4 )/2.0;
      }
    }
  }

  // set Quality of Barrel TOF
  if( barrel ) {

    // double layer
    if( innerLayer && outerLayer ) {
      m_quality = 1;
    }
    else {
      // single layer
      if( innerLayer || outerLayer ) {
	m_quality = 2;
      }
      else {
	// single-end of one layer
        if( innerEast || innerWest || outerEast || outerWest ) {
	  m_quality = 3;
	}
      }
    }

    // multi-hit
    if( ( (m_quality1&0xa000)!=0 ) || ( (m_quality2&0xa000)!=0 ) ) {
      m_quality = m_quality + 3;
    }

    // ztdc and extrapolated zhit is not matched
    if( ( (m_quality1&0x100)==0x100 ) || ( (m_quality2&0x100)==0x100 ) ) {
      if( ( m_quality == 1 ) || ( m_quality == 4 ) ) { m_quality = 7; }
      else if( ( m_quality == 2 ) || ( m_quality == 5 ) ) { m_quality = 8; }
      else if( ( m_quality == 3 ) || ( m_quality == 6 ) ) { m_quality = 9; }
      else {
	cout << "TofRec::TofTrack::setCalibration: Impossible!" << endl;
      }
    }

  }

  return;
}


//------- convert2RecTofTrackCol() -----------------------------------
//  generate RecTofTrackCol
// 
void TofTrack::convert2RecTofTrackCol( RecTofTrackCol* recTofTrackCol ) {

  bool barrel = ( ( m_hitCase == InnerLayer ) || ( m_hitCase == OuterLayer ) || ( m_hitCase == DoubleLayer ) );

  bool innerEast = ( ( m_quality1 & 0xc ) == 0xc );
  bool innerWest = ( ( m_quality1 & 0x3 ) == 0x3 );
  bool outerEast = ( ( m_quality2 & 0xc ) == 0xc );
  bool outerWest = ( ( m_quality2 & 0x3 ) == 0x3 );

  if( barrel ) {

    if( innerEast ) {
      RecTofTrack* atrack11 = new RecTofTrack;
      buildRecTofTrack( atrack11, 11 );            // innerlayer east readout
      TofHitStatus* hitStatus11 = new TofHitStatus;
      if( innerWest ) {
	hitStatus11->setBarrelReadout( 1, true );  // innerlayer east readout
      }
      else {
	if( m_hitCase == InnerLayer ) {
	  hitStatus11->setBarrelCluster( 11 );     // innerlayer east cluster
	}
	else if( m_hitCase == DoubleLayer ) {
	  if( outerEast && outerWest ) {
	    hitStatus11->setBarrelCounter( 11 );   // innerlayer east counter
	  }
	  else {
	    hitStatus11->setBarrelCluster( 11 );   // innerlayer east cluster
	  }
	}
	else {
	  cout << "TofRec::TofTrack:convert2RecTofTrackCol: 11- Impossible!" << endl;
	}
      }
      atrack11->setStatus( hitStatus11->value() );
      delete hitStatus11;
      recTofTrackCol->push_back( atrack11 );
    }

    if( innerWest ) {
      RecTofTrack* atrack12 = new RecTofTrack;
      buildRecTofTrack( atrack12, 12 );            // innerlayer west readout
      TofHitStatus* hitStatus12 = new TofHitStatus;
      if( innerEast ) {
	hitStatus12->setBarrelReadout( 1, false ); // innerlayer west
      }
      else {
	if( m_hitCase == InnerLayer ) {
	  hitStatus12->setBarrelCluster( 12 );     // innerlayer west cluster
	}
	else if( m_hitCase == DoubleLayer ) {
	  if( outerEast && outerWest ) {
	    hitStatus12->setBarrelCounter( 12 );   // innerlayer west counter
	  }
	  else {
	    hitStatus12->setBarrelCluster( 12 );   // innerlayer west cluster
	  }
	}
	else {
	  cout << "TofRec::TofTrack:convert2RecTofTrackCol: 12- Impossible!" << endl;
	}
      }
      atrack12->setStatus( hitStatus12->value() );
      delete hitStatus12;
      recTofTrackCol->push_back( atrack12 );
    }

    if( innerEast && innerWest ) {
      RecTofTrack* atrack1  = new RecTofTrack;
      buildRecTofTrack( atrack1, 1 );             // innerlayer counter
      TofHitStatus* hitStatus1 = new TofHitStatus;
      if( m_hitCase == InnerLayer ) {
	hitStatus1->setBarrelCluster( 1 );        // innerlayer cluster and counter
      }
      else if( m_hitCase == DoubleLayer ) {
	if( outerEast && outerWest ) {
	  hitStatus1->setBarrelCounter( 1 );      // innerlayer counter
	}
	else {
	  hitStatus1->setBarrelCluster( 1 );      // innerlayer cluster and counter
	}
      }
      else {
	cout << "TofRec::TofTrack:convert2RecTofTrackCol: 1- Impossible!" << endl;
      }
      atrack1->setStatus( hitStatus1->value() );
      delete hitStatus1;
      recTofTrackCol->push_back( atrack1 );
    }

    if( outerEast ) {
      RecTofTrack* atrack21 = new RecTofTrack;
      buildRecTofTrack( atrack21, 21 );           // outerlayer east readout
      TofHitStatus* hitStatus21 = new TofHitStatus;
      if( outerWest ) {
	hitStatus21->setBarrelReadout( 2, true ); // outerlayer east readout
      }
      else {
	if( m_hitCase == OuterLayer ) {
	  hitStatus21->setBarrelCluster( 21 );    // outerlayer east cluster
	}
	else if( m_hitCase == DoubleLayer ) {
	  if( innerEast || innerWest ) {
	    hitStatus21->setBarrelCounter( 21 );  // outerlayer east counter
	  }
	  //	  else {
	  //	    hitStatus21->setBarrelCluster( 21 );  // outerlayer east cluster
	  //      }
	}
	else {
	  cout << "TofRec::TofTrack:convert2RecTofTrackCol: 21- Impossible!" << endl;
	}
      }
      atrack21->setStatus( hitStatus21->value() );
      delete hitStatus21;
      recTofTrackCol->push_back( atrack21 );
    }

    if( outerWest ) {
      RecTofTrack* atrack22 = new RecTofTrack;
      buildRecTofTrack( atrack22, 22 );            // outerlayer west readout
      TofHitStatus* hitStatus22 = new TofHitStatus;
      if( outerEast ) {
	hitStatus22->setBarrelReadout( 2, false ); // outerlayer west readout
      }
      else {
	if( m_hitCase == OuterLayer ) {
	  hitStatus22->setBarrelCluster( 22 );     // outerlayer west cluster
	}
	else if( m_hitCase == DoubleLayer ) {
	  if( innerEast || innerWest ) {
	    hitStatus22->setBarrelCounter( 22 );   // outerlayer west counter
	  }
	  //	  else {
	  //	    hitStatus22->setBarrelCluster( 22 );   // outerlayer west cluster
	  //	  }
	}
	else {
	  cout << "TofRec::TofTrack:convert2RecTofTrackCol: 22- Impossible!" << endl;
	}
      }
      atrack22->setStatus( hitStatus22->value() );
      delete hitStatus22;
      recTofTrackCol->push_back( atrack22 );
    }

    if( outerEast && outerWest ) {
      RecTofTrack* atrack2  = new RecTofTrack;
      buildRecTofTrack( atrack2, 2 );             // outerlayer counter
      TofHitStatus* hitStatus2 = new TofHitStatus;
      if( m_hitCase == OuterLayer ) {
	hitStatus2->setBarrelCluster( 2 );        // outerlayer cluster and counter
      }
      else if( m_hitCase == DoubleLayer ) {
	if( innerEast && innerWest ) {
	  hitStatus2->setBarrelCounter( 2 );      // outerlayer counter
	}
	else {
	  hitStatus2->setBarrelCluster( 2 );      // outerlayer cluster and counter
	}
      }
      else {
	cout << "TofRec::TofTrack:convert2RecTofTrackCol: 2- Impossible!" << endl;
      }
      atrack2->setStatus( hitStatus2->value() );
      delete hitStatus2;
      recTofTrackCol->push_back( atrack2 );
    }

    if( innerEast && innerWest && outerEast && outerWest ) {
      RecTofTrack* atrack  = new RecTofTrack;
      buildRecTofTrack( atrack, 0 );              // doublelayer cluster
      TofHitStatus* hitStatus = new TofHitStatus;
      hitStatus->setBarrelCluster( 3 );           // doublelayer cluster
      atrack->setStatus( hitStatus->value() );
      delete hitStatus;
      recTofTrackCol->push_back( atrack );
    }

  }

  if( ( m_hitCase == EastEndcap ) || ( m_hitCase == WestEndcap ) ) {
    RecTofTrack* atrack = new RecTofTrack;
    buildRecTofTrack( atrack, 11 );               // eastendcap counter
    TofHitStatus* hitStatus = new TofHitStatus;
    if( m_hitCase == EastEndcap ) {
      hitStatus->setEndcapCluster( true );        // east endcap cluster counter readout
    }
    else if( m_hitCase == WestEndcap ) {
      hitStatus->setEndcapCluster( false );       // west endcap cluster counter readout
    }
    else {
      cout << "TofRec::TofTrack:convert2RecTofTrackCol: endcap- Impossible!" << endl;
    }
    atrack->setStatus( hitStatus->value() );
    delete hitStatus;
    recTofTrackCol->push_back( atrack );
  }

  if( ( m_hitCase == EastEndcapMRPC ) || ( m_hitCase == WestEndcapMRPC ) ) {

    if( innerEast || innerWest ) {

      if( innerEast ) {
	RecTofTrack* atrack1 = new RecTofTrack;
	buildRecTofTrack( atrack1, 11 );            // mrpc east readout
	TofHitStatus* hitStatus1 = new TofHitStatus;
	if( innerWest ) {
	  hitStatus1->setMRPCReadout( true );       // mrpc east readout
	}
	else {
	  hitStatus1->setMRPCCluster( false, true ); // mrpc east cluster
	}
	atrack1->setStatus( hitStatus1->value() );
	delete hitStatus1;
	recTofTrackCol->push_back( atrack1 );
      }
      
      if( innerWest ) {
	RecTofTrack* atrack2 = new RecTofTrack;
	buildRecTofTrack( atrack2, 12 );            // mrpc west readout
	TofHitStatus* hitStatus2 = new TofHitStatus;
	if( innerEast ) {
	  hitStatus2->setMRPCReadout( false );      // mrpc west readout
	}
	else {
	  hitStatus2->setMRPCCluster( false, false ); // mrpc east cluster
	}
	atrack2->setStatus( hitStatus2->value() );
	delete hitStatus2;
	recTofTrackCol->push_back( atrack2 );
      }
      
      if( innerEast && innerWest ) {
	RecTofTrack* atrack = new RecTofTrack;
	buildRecTofTrack( atrack, 1 );                // mrpc east readout
	TofHitStatus* hitStatus = new TofHitStatus;
	if( m_hitCase == EastEndcapMRPC ) {
	  hitStatus->setMRPCCluster( true, true ); // mrpc east cluster
	}
	else {
	  hitStatus->setMRPCCluster( true, false );// mrpc west cluster
	}
	atrack->setStatus( hitStatus->value() );
	delete hitStatus;
	recTofTrackCol->push_back( atrack );
      }

    }
    else {

      if( outerEast ) {
	RecTofTrack* atrack1 = new RecTofTrack;
	buildRecTofTrack( atrack1, 21 );            // mrpc east readout
	TofHitStatus* hitStatus1 = new TofHitStatus;
	if( outerWest ) {
	  hitStatus1->setMRPCReadout( true );       // mrpc east readout
	}
	else {
	  hitStatus1->setMRPCCluster( false, true ); // mrpc east cluster
	}
	atrack1->setStatus( hitStatus1->value() );
	delete hitStatus1;
	recTofTrackCol->push_back( atrack1 );
      }
      
      if( outerWest ) {
	RecTofTrack* atrack2 = new RecTofTrack;
	buildRecTofTrack( atrack2, 22 );            // mrpc west readout
	TofHitStatus* hitStatus2 = new TofHitStatus;
	if( outerEast ) {
	  hitStatus2->setMRPCReadout( false );      // mrpc west readout
	}
	else {
	  hitStatus2->setMRPCCluster( false, false ); // mrpc east cluster
	}
	atrack2->setStatus( hitStatus2->value() );
	delete hitStatus2;
	recTofTrackCol->push_back( atrack2 );
      }
      
      if( outerEast && outerWest ) {
	RecTofTrack* atrack = new RecTofTrack;
	buildRecTofTrack( atrack, 2 );                // mrpc east readout
	TofHitStatus* hitStatus = new TofHitStatus;
	if( m_hitCase == EastEndcapMRPC ) {
	  hitStatus->setMRPCCluster( true, true ); // mrpc east cluster
	}
	else {
	  hitStatus->setMRPCCluster( true, false );// mrpc west cluster
	}
	atrack->setStatus( hitStatus->value() );
	delete hitStatus;
	recTofTrackCol->push_back( atrack );
      }

    }

  }    

  if( m_hitCase == NoHit ) {
    RecTofTrack* atrack = new RecTofTrack;
    buildRecTofTrack( atrack, 3 );            // no hit
    TofHitStatus* hitStatus = new TofHitStatus;
    hitStatus->setNoHit();                    // no hit
    atrack->setStatus( hitStatus->value() );
    delete hitStatus;
    recTofTrackCol->push_back( atrack );
  }

  return;
}



void TofTrack::buildRecTofTrack( RecTofTrack* track, int layerorend ) {

  track->setTofTrackID( m_tofTrackId );
  track->setTrackID( m_trackId );

  track->setErrTof( 0.0 );
  track->setBeta( 0.0 );

  double sigma[6];
  for( int i=0; i<6; i++ ) {
    sigma[i]   = 0.0;
  }
  track->setSigma( sigma );
  track->setQuality( m_quality );
  track->setT0( m_estime );
  track->setErrT0( 0.0 );
  track->setPhi( 9999.0 );
  track->setErrPhi( 9999.0 );
  track->setEnergy( 9999.0 );
  track->setErrEnergy( 9999.0 );

  if( ( layerorend == 11 ) || ( layerorend == 12 ) || ( layerorend == 1 ) ) {
    if( m_strip1<0 ) {
      track->setTofID( m_tofId1 );    // scintillator
    }
    else {
      track->setTofID( 12*m_tofId1+m_strip1 );    // MRPC
    }
    track->setPath( m_path1 );
    track->setZrHit( m_zrhit1 );
    track->setErrZ( m_errzr1 );
    track->setTexp( m_texpInner );

    setRecTofTrack( track, layerorend );
  }

  if( ( layerorend==21 ) || ( layerorend==22 ) || ( layerorend==2 ) ) {
    if( m_strip2<0 ) {
      track->setTofID( m_tofId2 );    // scintillator
    }
    else {
      track->setTofID( 12*m_tofId2+m_strip2 );    // MRPC
    }
    track->setPath( m_path2 );
    track->setZrHit( m_zrhit2 );
    track->setErrZ( m_errzr2 );
    track->setTexp( m_texpOuter );

    setRecTofTrack( track, layerorend );
  }

  if( layerorend==0 ) {
    track->setTofID( m_tofId1 );
    track->setPath( m_path2 );
    track->setZrHit( m_zrhit2 );
    track->setErrZ( m_errzr2 );
    track->setTexp( m_texp );

    setRecTofTrack( track, layerorend );
  }

  if( layerorend == 3 ) {
    if( m_strip1<0 ) {
      track->setTofID( m_id1 );                // scintillator
    }
    else {
      track->setTofID( 12*m_id1 + m_strip1 );   // mrpc
    }
    track->setPath( m_path1 );
    track->setZrHit( m_zrhit1 );
    track->setErrZ( m_errzr1 );
    track->setTexp( m_texpInner );
  }

  return;
}


void TofTrack::setRecTofTrack( RecTofTrack* track, int layerorend ) {

  double toffset[6];
  for( unsigned int i=0; i<6; i++ ) {
    toffset[i] = 0.0;
  }

  if( layerorend == 0 ) {   // cluster or double layer hit
    track->setPh( m_ph );
    track->setTof( m_tof[0] );
    track->setSigmaElectron( m_sigma );
    for( unsigned int i=0; i<5; i++ ) {
      toffset[i] = m_tof[0] - m_tof[i];
    }
    track->setToffset( toffset );
    track->setBeta( m_path/m_tof[0]/30.0 );
  }
  else if( layerorend == 1 ) { // inner layer
    track->setPh( m_ph1 );
    track->setTof( m_tof1[0] );
    track->setSigmaElectron( m_sigma1 );
    for( unsigned int i=0; i<5; i++ ) {
      toffset[i] = m_tof1[0] - m_tof1[i];
    }
    track->setToffset( toffset );
    track->setBeta( m_path1/m_tof1[0]/30.0 );
  }
  else if( layerorend == 2 ) { // outer layer
    track->setPh( m_ph2 );
    track->setTof( m_tof2[0] );
    track->setSigmaElectron( m_sigma2 );
    for( unsigned int i=0; i<5; i++ ) {
      toffset[i] = m_tof2[0] - m_tof2[i];
    }
    track->setToffset( toffset );
    track->setBeta( m_path2/m_tof2[0]/30.0 );
  }
  else if( layerorend == 11 ) { // inner layer east end readout
    track->setPh( m_ph11 );
    track->setTof( m_tof11[0] );
    track->setSigmaElectron( m_sigma11 );
    for( unsigned int i=0; i<5; i++ ) {
      toffset[i] = m_tof11[0] - m_tof11[i];
    }
    track->setToffset( toffset );
    track->setBeta( m_path1/m_tof11[0]/30.0 );
  }
  else if( layerorend == 12 ) { // inner layer west end readout
    track->setPh( m_ph12 );
    track->setTof( m_tof12[0] );
    track->setSigmaElectron( m_sigma12 );
    for( unsigned int i=0; i<5; i++ ) {
      toffset[i] = m_tof12[0] - m_tof12[i];
    }
    track->setToffset( toffset );
    track->setBeta( m_path1/m_tof12[0]/30.0 );
  }
  else if( layerorend == 21 ) { // outer layer east end readout
    track->setPh( m_ph21 );
    track->setTof( m_tof21[0] );
    track->setSigmaElectron( m_sigma21 );
    for( unsigned int i=0; i<5; i++ ) {
      toffset[i] = m_tof21[0] - m_tof21[i];
    }
    track->setToffset( toffset );
    track->setBeta( m_path2/m_tof21[0]/30.0 );
  }
  else if( layerorend == 22 ) { // outer layer west end readout
    track->setPh( m_ph22 );
    track->setTof( m_tof22[0] );
    track->setSigmaElectron( m_sigma22 );
    for( unsigned int i=0; i<5; i++ ) {
      toffset[i] = m_tof22[0] - m_tof22[i];
    }
    track->setToffset( toffset );
    track->setBeta( m_path2/m_tof22[0]/30.0 );
  }
  else{
    cout << "TofRec TofTrack::SetRecTofTrack  layerorend = " << layerorend << endl;
  }
  return;
}


void TofTrack::convert2RecBTofCalHitColBarrel( int runNumber, int eventNumber, RecBTofCalHitCol* btofCalHitCol, std::string calibData ) {

  if( ( m_quality1 & 0x800 ) == 0x800 ) {

    RecBTofCalHit* ahit = new RecBTofCalHit;
    ahit->setRun( runNumber );
    ahit->setEvent( eventNumber );
    ahit->setMod( m_tofId1 );
    ahit->setQual( m_hitCase );
    ahit->setdZHit( 1 );

    for( int i=0; i<5; i++ ) {
      ahit->setTpred( i, m_texpInner[i] );
    }
    if( calibData == "Dimu" ) {
      ahit->setTpred( m_texpInner[1] );
      ahit->setZHit( m_zr1[1] );
      //      ahit->setdZHit( m_ezr1[1] );
    }
    else {
      ahit->setTpred( m_texpInner[0] );
      ahit->setZHit( m_zr1[0] );
      //      ahit->setdZHit( m_ezr1[0] );
    }

    ahit->setTdc1( m_tdc1-m_estime );
    ahit->setTdc2( m_tdc2-m_estime );
    ahit->setAdc1( m_adc1 );
    ahit->setAdc2( m_adc2 );
    //  ahit->setZHit( m_zrhit1 );
    //  ahit->setdZHit( m_errzr1 );
    ahit->setDeltaPhi( m_estime );
    ahit->setsinTheta( m_theta1 );
    ahit->setP( m_momentum );
    ahit->setQ( m_ph1 );
    ahit->setPath( m_path1 );

    btofCalHitCol->push_back( ahit );

    if( ( m_quality2 & 0x800 ) == 0x800 ) {

      RecBTofCalHit* bhit = new RecBTofCalHit;
      bhit->setRun( runNumber );
      bhit->setEvent( eventNumber );
      bhit->setMod( m_tofId2 );
      bhit->setQual( m_hitCase );
      bhit->setdZHit( 1 );

      for( int i=0; i<5; i++ ) {
	bhit->setTpred( i, m_texpOuter[i] );
      }
      if( calibData == "Dimu" ) {
	bhit->setTpred( m_texpOuter[1] );
	bhit->setZHit( m_zr2[1] );
	//	bhit->setdZHit( m_ezr2[1] );
      }
      else {
	bhit->setTpred( m_texpOuter[0] );
	bhit->setZHit( m_zr2[0] );
	//	bhit->setdZHit( m_ezr2[0] );
      }

      bhit->setTdc1( m_tdc3-m_estime );
      bhit->setTdc2( m_tdc4-m_estime );
      bhit->setAdc1( m_adc3 );
      bhit->setAdc2( m_adc4 );
      //  bhit->setZHit( m_zrhit2 );
      //  bhit->setdZHit( m_errzr2 );
      bhit->setDeltaPhi( m_estime );
      bhit->setsinTheta( m_theta2 );
      bhit->setP( m_momentum );
      bhit->setQ( m_ph2 );
      bhit->setPath( m_path2 );

      ahit->setnext(bhit);

      btofCalHitCol->push_back( bhit );

    }
  }

  return;
}


void TofTrack::convert2RecETofCalHitCol( int runNumber, int eventNumber, RecETofCalHitCol* etofCalHitCol, std::string calibData ) {

  if( ( m_quality1 & 0x800 ) != 0x800 ) return;

  RecETofCalHit* chit = new RecETofCalHit;
  chit->setRun( runNumber );
  chit->setEvent( eventNumber );
  chit->setMod( m_tofId1 );
  chit->setQual( m_hitCase );

  for( int i=0; i<5; i++ ) {
    chit->setTpred( i, m_texpInner[i] );
  }
  if( calibData == "Dimu" ) {
    chit->setTpred( m_texpInner[1] );
    chit->setRHit( m_zr1[1] );
    chit->setdRHit( m_ezr1[1] );
  }
  else {
    chit->setTpred( m_texpInner[0] );
    chit->setRHit( m_zr1[0] );
    chit->setdRHit( m_ezr1[0] );
  }

  chit->setTdc( m_tdc1-m_estime );
  chit->setAdc( m_adc1 );
  //  chit->setRHit( m_zrhit1 );
  //  chit->setdRHit( m_errzr1 );
  chit->setDeltaPhi( m_estime );
  chit->setcosTheta( m_theta1 );
  chit->setQ( m_ph1 );
  chit->setP( m_momentum );
  chit->setPath( m_path1 );

  etofCalHitCol->push_back( chit );

  return;
}


void TofTrack::convert2RecBTofCalHitColETF( int runNumber, int eventNumber, RecBTofCalHitCol* btofCalHitCol, std::string calibData ) {

  if( ( m_quality1 & 0x800 ) == 0x800 ) {

    RecBTofCalHit* ahit = new RecBTofCalHit;
    ahit->setRun( runNumber );
    ahit->setEvent( eventNumber );
    ahit->setMod( m_tofId1 );
    ahit->setQual( m_hitCase );
    ahit->setdZHit( 0 );
    
    for( int i=0; i<5; i++ ) {
      ahit->setTpred( i, m_texpInner[i] );
    }
    if( calibData == "Dimu" ) {
      ahit->setTpred( m_texpInner[1] );
      ahit->setZHit( m_zr1[1] );
    }
    else {
      ahit->setTpred( m_texpInner[0] );
      ahit->setZHit( m_zr1[0] );
    }
    
    ahit->setTdc1( m_tdc1-m_estime );
    ahit->setTdc2( m_tdc2-m_estime );
    ahit->setAdc1( m_adc1 );
    ahit->setAdc2( m_adc2 );
    ahit->setDeltaPhi( m_estime );
    ahit->setsinTheta( m_strip1 );
    ahit->setP( m_momentum );
    ahit->setQ( m_ph1 );
    ahit->setPath( m_path1 );
    
    btofCalHitCol->push_back( ahit );
  }

  if( ( m_quality2 & 0x800 ) == 0x800 ) {

    RecBTofCalHit* bhit = new RecBTofCalHit;
    bhit->setRun( runNumber );
    bhit->setEvent( eventNumber );
    bhit->setMod( m_tofId2 );
    bhit->setQual( m_hitCase );
    bhit->setdZHit( 0 );
    
    for( int i=0; i<5; i++ ) {
      bhit->setTpred( i, m_texpOuter[i] );
    }
    if( calibData == "Dimu" ) {
      bhit->setTpred( m_texpOuter[1] );
      bhit->setZHit( m_zr2[1] );
    }
    else {
      bhit->setTpred( m_texpOuter[0] );
      bhit->setZHit( m_zr2[0] );
    }
    
    bhit->setTdc1( m_tdc3-m_estime );
    bhit->setTdc2( m_tdc4-m_estime );
    bhit->setAdc1( m_adc3 );
    bhit->setAdc2( m_adc4 );
    bhit->setDeltaPhi( m_estime );
    bhit->setsinTheta( m_strip2 );
    bhit->setP( m_momentum );
    bhit->setQ( m_ph2 );
    bhit->setPath( m_path2 );
    
    btofCalHitCol->push_back( bhit );
  }

  return;
}


// set Quality using quality1 and quality2
void qualityAnalysis() {
  
  return;
}
