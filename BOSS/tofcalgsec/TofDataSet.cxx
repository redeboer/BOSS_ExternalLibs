#include "tofcalgsec/TofDataSet.h"
#include <cmath>

Record::Record() {
  initial();
  return;
}


Record::~Record() {}


void Record::initial() {
  m_run     = 0;
  m_event   = -1;
  m_tofid   = -1;
  m_strip   = -1;
  m_qleft   = -99.0;
  m_qright  = -99.0;
  m_tleft   = -99.0;
  m_tright  = -99.0;
  m_zrhit   = -999.0;
  m_dt      = -999.0;
  m_texp    = -99.0;
  m_path    = -99.0;
  m_phi     = -99.0;
  m_theta   = -99.0;
  m_p       = -9.0;
  m_t0      = -99.0;
  m_q0      = -99.0;
  m_hitcase = -1;
  return;
}


Record::Record( RecBTofCalHit* hit ) {
  initial();
  if( hit ) {
    m_run     = hit->run();
    m_event   = hit->event();
    m_tofid   = hit->mod();
    m_strip   = int( hit->sinTheta() );
    m_qleft   = hit->adc1();
    m_qright  = hit->adc2();
    m_tleft   = hit->tdc1();
    m_tright  = hit->tdc2();
    m_zrhit   = hit->zHit();
    m_dt      = ( hit->tdc2() - hit->tdc1() )/2.0;
    m_texp    = hit->tpred();
    m_path    = hit->path();
    m_phi     = hit->deltaPhi();
    m_theta   = hit->sinTheta();
    m_p       = hit->p();
    m_hitcase = hit->qual();
  }
  return;
}


Record::Record( RecETofCalHit* hit ) {
  initial();
  if( hit ) {
    m_run     = hit->run();
    m_event   = hit->event();
    m_tofid   = hit->mod();
    m_qleft   = hit->adc();
    m_tleft   = hit->tdc();
    m_zrhit   = hit->rHit();
    m_texp    = hit->tpred();
    m_path    = hit->path();
    m_phi     = hit->deltaPhi();
    m_theta   = hit->cosTheta();
    m_p       = hit->p();
    m_hitcase = hit->qual();
  }
  return;
}


Record::Record( const rootRecord& one ) {
  initial();
  if( &one ) {
    m_run     = one.run;
    m_event   = one.event;
    m_tofid   = one.tofid;
    m_strip   = one.strip;
    m_qleft   = one.qleft;
    m_tleft   = one.tleft;
    if( ( one.hitcase >= 0 && one.hitcase <=2 ) || ( one.hitcase==5 || one.hitcase==6 ) ) {
      m_qright  = one.qright;
      m_tright  = one.tright;
    }
    m_zrhit   = one.zrhit;
    m_dt      = one.dt;
    m_texp    = one.texp;
    m_path    = one.path;
    m_phi     = one.phi;
    m_theta   = one.theta;
    m_p       = one.p;
    m_hitcase = one.hitcase;
  }
  return;
}


void TofDataSet::setData( TTree* t, unsigned int isBarrel ) {
  if( t && t->GetEntries()>0 ) {
    rootRecord item;
    t->SetBranchAddress( "run",     &item.run     );
    t->SetBranchAddress( "event",   &item.event   );
    t->SetBranchAddress( "tofid",   &item.tofid   );
    if( NULL != t->FindBranch( "strip" ) ) {
      t->SetBranchAddress( "strip",   &item.strip   );
    }
    t->SetBranchAddress( "qleft",   &item.qleft   );
    t->SetBranchAddress( "qright",  &item.qright  );
    t->SetBranchAddress( "tleft",   &item.tleft   );
    t->SetBranchAddress( "tright",  &item.tright  );
    t->SetBranchAddress( "zrhit",   &item.zrhit   );
    if( NULL != t->FindBranch( "dt" ) ) {
      t->SetBranchAddress( "dt",      &item.dt      );
    }
    t->SetBranchAddress( "texp",    &item.texp    );
    t->SetBranchAddress( "path",    &item.path    );
    t->SetBranchAddress( "phi",     &item.phi     );
    t->SetBranchAddress( "theta",   &item.theta        );
    t->SetBranchAddress( "p",       &item.p       );
    t->SetBranchAddress( "hitcase", &item.hitcase );

    for( unsigned int i=0; i<t->GetEntries(); i++ ) {
      t->GetEntry(i);
      if( isBarrel==1 && item.hitcase>=0 && item.hitcase<=2 ) {
	Record *r = new Record( item );
	if( r->cutBarrel() ) {
	  unsigned int tofID = item.tofid;
	  barrelData[tofID]->push_back(r);
	}
	else {
	  delete r;
	}
      }
      else if( isBarrel==0 && ( item.hitcase==3 || item.hitcase==4 ) ) {
	Record *r = new Record( item );
	if( r->cutEndcap() ) {
	  unsigned int tofID = item.tofid;
	  endcapData[tofID]->push_back(r);
	}
	else {
	  delete r;
	}
      }
      else if( isBarrel==2 && item.hitcase>=5 && item.hitcase<=6 ) {
	Record *r = new Record( item );
	if( r->cutEtf() ) {
	  unsigned int tofID = item.tofid;
	  unsigned int strip = item.strip;
	  unsigned int id = tofID*12 + strip;
	  etfData[id]->push_back(r);
	}
	else {
	  delete r;
	}
      }
    }
  }
  else {
    std::cerr << "Error: a invalid tree or a blank tree, When converting a tree to TofDataSet,exit" << std::endl;
  }

  return;
}


TofDataSet::TofDataSet() {
  for( unsigned int i=0; i<NBarrel; i++ ) {
    barrelData[i] = new RecordSet;
  }
  for( unsigned int i=0; i<NEndcap; i++ ) {
    endcapData[i] = new RecordSet;
  }
  for( unsigned int i=0; i<NEtf*NStrip; i++ ) {
    etfData[i] = new RecordSet;
  }
  return;
}


TofDataSet::~TofDataSet() {
  for( unsigned int i=0; i<NBarrel; i++ ) {
    barrelData[i]->clear();
    delete barrelData[i];
  }
  for( unsigned int i=0; i<NEndcap; i++ ) {
    endcapData[i]->clear();
    delete endcapData[i];
  }
  for( unsigned int i=0; i<NEtf*NStrip; i++ ) {
    etfData[i]->clear();
    delete etfData[i];
  }
  return;
}


void TofDataSet::setBarrelDataFiles( std::vector<std::string>& barrelFiles ) {
  TChain* data_barrel = new TChain("btrk");
  if( !data_barrel ) {
    std::cerr << " tofcalgsec Error Msg: creating a tree[barrel] fails in TofDataSet()"<<std::endl;
    throw "Error Msg: creating a tree fails in TofDataSet() ";
  }
  std::cout<<"begin reading barrel data file ... "<<std::endl;
  try{
    for( std::vector<std::string>::iterator it=barrelFiles.begin(); it!=barrelFiles.end(); it++ ) {
      std::cout << " Add file : " << (*it) << std::endl;
      data_barrel->Add( (*it).c_str() );
    }
  }
  catch(...){
    std::cerr << "tofcalgsec Error Msg : in TofDataSet::setDataFiles(std::vector<std::string>&) " << std::endl;
    return;
  }
  setData( data_barrel, 1 );
  delete data_barrel;
  return;
}


void TofDataSet::setEndcapDataFiles( std::vector<std::string>& endcapFiles ) {
  TChain* data_endcap = new TChain("etrk");
  if( !data_endcap ) {
    std::cerr << " tofcalgsec Error Msg: creating a tree[endcap] fails in TofDataSet()"<<std::endl;
    throw "Error Msg: creating a tree fails in TofDataSet() ";
  }
  std::cout<<"begin reading endcap data file ... "<<std::endl;
  try{
    for( std::vector<std::string>::iterator it=endcapFiles.begin(); it!=endcapFiles.end(); it++ ) {
      std::cout << " Add file : " << (*it) << std::endl;
      data_endcap->Add( (*it).c_str() );
    }
  }
  catch(...){
    std::cerr << "tofcalgsec Error Msg : in TofDataSet::setDataFiles(std::vector<std::string>&) " << std::endl;
    return;
  }
  setData( data_endcap, 0 );
  delete data_endcap;
  return;
}


void TofDataSet::setEtfDataFiles( std::vector<std::string>& etfFiles ) {
  TChain* data_etf = new TChain("etf");
  if( !data_etf ) {
    std::cerr << " tofcalgsec Error Msg: creating a tree[etf] fails in TofDataSet()"<<std::endl;
    throw "Error Msg: creating a tree fails in TofDataSet() ";
  }
  std::cout<<"begin reading etf data file ... "<<std::endl;
  try{
    for( std::vector<std::string>::iterator it=etfFiles.begin(); it!=etfFiles.end(); it++ ) {
      std::cout << " Add file : " << (*it) << std::endl;
      data_etf->Add( (*it).c_str() );
    }
  }
  catch(...){
    std::cerr << "tofcalgsec Error Msg : in TofDataSet::setDataFiles(std::vector<std::string>&) " << std::endl;
    return;
  }
  setData( data_etf, 2 );
  delete data_etf;
  return;
}


void TofDataSet::setBarrelData( RecBTofCalHitCol& bhitcol ) {
  RecBTofCalHitCol::iterator iter = bhitcol.begin();
  for( ; iter!=bhitcol.end(); iter++ ) {
    int tofid = (*iter)->mod();
    if( tofid<0 || tofid>175 ) continue;
    if( fabs( (*iter)->dzHit() - 1.0 )>1.0e-6 ) continue;

    Record *r = new Record( (*iter) );
    if( r->cutBarrel() ) {
      barrelData[tofid]->push_back(r);
    }
    else {
      delete r;
    }
  }
  return;
}


void TofDataSet::setEndcapData( RecETofCalHitCol& ehitcol ) {
  RecETofCalHitCol::iterator iter = ehitcol.begin();
  for( ; iter!=ehitcol.end(); iter++ ) {
    int tofid = (*iter)->mod();
    if( tofid<0 || tofid>95 ) continue;

    Record *r = new Record( (*iter) );
    if( r->cutEndcap() ) {
      endcapData[tofid]->push_back(r);
    }
    else {
      delete r;
    }
  }
  return;
}


void TofDataSet::setEtfData( RecBTofCalHitCol& bhitcol ) {
  RecBTofCalHitCol::iterator iter = bhitcol.begin();
  for( ; iter!=bhitcol.end(); iter++ ) {
    int tofid = (*iter)->mod();
    int strip = int( (*iter)->sinTheta() );
    if( tofid<0 || tofid>71 ) continue;
    if( strip<0 || strip>11 ) continue;
    if( fabs( (*iter)->dzHit() )>1.0e-6 ) continue;
    unsigned int id = tofid*12 + strip;
    Record *r = new Record( (*iter) );
    if( r->cutEtf() ) {
      etfData[id]->push_back(r);
    }
    else {
      delete r;
    }
  }
  return;
}

