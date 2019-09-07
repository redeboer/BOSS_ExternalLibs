#include "tofcalgsec/TofCalibManager.h"
#include "TTree.h"
#include "TFile.h"

TofCalibManager* TofCalibManager::m_manager=0;
TofCalibManager* TofCalibManager::getManager() {
  if( m_manager == 0 ) {
    m_manager = new TofCalibManager;
  }
  return m_manager;
}


TofCalibManager::TofCalibManager() {
  m_dataset = new TofDataSet;
  return;
}


TofCalibManager::~TofCalibManager() {
  if( !calib_barrel_item.empty() ) {
    std::vector<TofCalib*>::iterator iter = calib_barrel_item.begin();
    for( ; iter != calib_barrel_item.end(); iter++ ) {
      if( (*iter) ) delete (*iter);
    }
    calib_barrel_item.clear();
  }
  if( !calib_endcap_item.empty() ) {
    std::vector<TofCalib*>::iterator iter = calib_endcap_item.begin();
    for( ; iter != calib_endcap_item.end(); iter++ ) {
      if( (*iter) ) delete (*iter);
    }
    calib_endcap_item.clear();
  }
  if( !calib_etf_item.empty() ) {
    std::vector<TofCalib*>::iterator iter = calib_etf_item.begin();
    for( ; iter != calib_etf_item.end(); iter++ ) {
      if( (*iter) ) delete (*iter);
    }
    calib_etf_item.clear();
  }
  if( !calibration_barrel_item.empty() ) {
    std::vector<TofCalibFit*>::iterator iter = calibration_barrel_item.begin();
    for( ; iter != calibration_barrel_item.end(); iter++ ) {
      if( (*iter) ) delete (*iter);
    }
    calibration_barrel_item.clear();
  }
  if( !calibration_barrel_item2.empty() ) {
    std::vector<TofCalibFit*>::iterator iter = calibration_barrel_item2.begin();
    for( ; iter != calibration_barrel_item2.end(); iter++ ) {
      if( (*iter) ) delete (*iter);
    }
    calibration_barrel_item2.clear();
  }
  if( !calibration_endcap_item.empty() ) {
    std::vector<TofCalibFit*>::iterator iter = calibration_endcap_item.begin();
    for( ; iter != calibration_endcap_item.end(); iter++ ) {
      if( (*iter) ) delete (*iter);
    }
    calibration_endcap_item.clear();
  }

  delete m_dataset;
  return;
}


void TofCalibManager::addCalib( TofCalib* cal_item1, int isbarrel ) {
  if( isbarrel==1 ) {
    calib_barrel_item.push_back(cal_item1);
  }
  else if( isbarrel==0 ) {
    calib_endcap_item.push_back(cal_item1);
  }
  else if( isbarrel==2 ) {
    calib_etf_item.push_back(cal_item1);
  }
  else {
    cout << "Tof::tofcalgsec::TofCalibManaer: addCalib: isbarrel is OUT of range!" << endl;
  }
  return;
}


void TofCalibManager::addCalib( TofCalibFit* cal_item2, bool isbarrel ) {
  if( isbarrel ) {
    if( cal_item2->name()=="calib_barrel_sigma" ) {
      calibration_barrel_item2.push_back(cal_item2);
    }
    else {
      calibration_barrel_item.push_back(cal_item2);
    }
  }
  else {
    //    calibration_endcap_item.push_back(cal_item2);
    calibration_etf_item.push_back(cal_item2);
  }
  return;
}


void TofCalibManager::doCalibration() {

  if( !calib_barrel_item.empty() || !calibration_barrel_item.empty() ) {
    std::cout << "Begin barrel counters calibration..." << std::endl;
    std::cout << setiosflags(ios::left) << setw(10) << "TOF ID" << setw(8) << "Entries" << setw(30) << "What Calibration" << std::endl;
    HepVector tcorrelation;
    for( unsigned int i=0; i<NBarrel; i++ ) {
      RecordSet* barrelData = m_dataset->getBarrelData(i);
      if( !calib_barrel_item.empty() ) {
	std::vector<TofCalib*>::iterator iter1 = calib_barrel_item.begin();
	for( ; iter1!=calib_barrel_item.end(); iter1++ ) {
	  (*iter1)->reset();
	  (*iter1)->calculate( barrelData, i );
	}
      }
      if( !calibration_barrel_item.empty() ) { 
	std::vector<TofCalibFit*>::iterator iter2 = calibration_barrel_item.begin();
	for( ; iter2!=calibration_barrel_item.end(); iter2++ ) {
	  (*iter2)->calculate( barrelData, i );
	  if( i==(NBarrel-1) ) {
	    if( (*iter2)->name()=="calib_barrel_common" ) {
	      tcorrelation = (*iter2)->tcorrelation();
	    }
	  }
	}
      }
    }

    for( unsigned int i=0; i<NBarrel; i++ ) {
      RecordSet* barrelData = m_dataset->getBarrelData(i);
      if( !calibration_barrel_item2.empty() ) { 
	std::vector<TofCalibFit*>::iterator iter2 = calibration_barrel_item2.begin();
	for( ; iter2!=calibration_barrel_item2.end(); iter2++ ) {
	  if( i==0 ) {
	    if( (*iter2)->name()=="calib_barrel_sigma" ) {
	      (*iter2)->setTCorrelation( tcorrelation );
	    }
	  }
	  (*iter2)->calculate( barrelData, i );
	}
      }
    }
  }

  if( !calib_endcap_item.empty() || !calibration_endcap_item.empty() ) {
    std::cout << "Begin endcap counters calibration..." << std::endl;
    std::cout << setiosflags(ios::left) << setw(10)<<"TOF ID" << setw(8) << "Entries" << setw(30) << "What Calibration" << std::endl;
    for( unsigned int i=0; i<NEndcap; i++ ) {
      RecordSet* endcapData = m_dataset->getEndcapData(i);
      if( !calib_endcap_item.empty() ) {
	std::vector<TofCalib*>::iterator iter1 = calib_endcap_item.begin();
	for( ; iter1!=calib_endcap_item.end(); iter1++ ) {
	  (*iter1)->reset();
	  (*iter1)->calculate( endcapData, i );
	}
      }
      if( !calibration_endcap_item.empty() ) {
	std::vector<TofCalibFit*>::iterator iter2 = calibration_endcap_item.begin();
	for( ; iter2!=calibration_endcap_item.end(); iter2++ ) {
	  (*iter2)->calculate( endcapData, i );
	}
      }
    }
  }

  if( !calib_etf_item.empty() || !calibration_etf_item.empty() ) {
    std::cout << "Begin ETF(MRPC) calibration..." << std::endl;
    std::cout << setiosflags(ios::left) << setw(10) << "TOF ID *12 + STRIP" << setw(8) << "Entries" << setw(30) << "What Calibration" << std::endl;
    HepVector tcorrelation;
    for( unsigned int i=0; i<NEtf*NStrip; i++ ) {
      RecordSet* etfData = m_dataset->getEtfData(i);
      if( !calib_etf_item.empty() ) {
	std::vector<TofCalib*>::iterator iter1 = calib_etf_item.begin();
	for( ; iter1!=calib_etf_item.end(); iter1++ ) {
	  (*iter1)->reset();
	  (*iter1)->calculate( etfData, i );
	}
      }
      if( !calibration_etf_item.empty() ) {
	std::vector<TofCalibFit*>::iterator iter2 = calibration_etf_item.begin();
	for( ; iter2!=calibration_etf_item.end(); iter2++ ) {
	  (*iter2)->calculate( etfData, i );
	}
      }
    }
  }

  return;
}


void TofCalibManager::fillTxt( string& dir ) {

  if( !calib_barrel_item.empty() ) {
    std::vector<TofCalib*>::iterator iter = calib_barrel_item.begin();
    for( ; iter != calib_barrel_item.end(); iter++ ) {
      string fname = dir + string("/") + (*iter)->name() + string(".txt");
      (*iter)->fillTxt( fname.c_str() );
    }
  }

  if( !calib_endcap_item.empty() ) {
    std::vector<TofCalib*>::iterator iter = calib_endcap_item.begin();
    for( ; iter != calib_endcap_item.end(); iter++ ) {
      string fname = dir + string("/") + (*iter)->name() + string(".txt");
      (*iter)->fillTxt( fname.c_str() );
    }
  }

  if( !calib_etf_item.empty() ) {
    std::vector<TofCalib*>::iterator iter = calib_etf_item.begin();
    for( ; iter != calib_etf_item.end(); iter++ ) {
      string fname = dir + string("/") + (*iter)->name() + string(".txt");
      (*iter)->fillTxt( fname.c_str() );
    }
  }

  if( !calibration_barrel_item.empty() ) {
    std::vector<TofCalibFit*>::iterator iter = calibration_barrel_item.begin();
    for( ; iter != calibration_barrel_item.end(); iter++ ) {
      string fname = dir + string("/") + (*iter)->name() + string(".txt");
      (*iter)->fillTxt( fname.c_str() );
    }
  }
  if( !calibration_barrel_item2.empty() ) {
    std::vector<TofCalibFit*>::iterator iter = calibration_barrel_item2.begin();
    for( ; iter != calibration_barrel_item2.end(); iter++ ) {
      string fname = dir + string("/") + (*iter)->name() + string(".txt");
      (*iter)->fillTxt( fname.c_str() );
    }
  }

  if( !calibration_endcap_item.empty() ) {
    std::vector<TofCalibFit*>::iterator iter = calibration_endcap_item.begin();
    for( ; iter != calibration_endcap_item.end(); iter++ ) {
      string fname = dir + string("/") + (*iter)->name() + string(".txt");
      (*iter)->fillTxt( fname.c_str() );
    }
  }

  if( !calibration_etf_item.empty() ) {
    std::vector<TofCalibFit*>::iterator iter = calibration_etf_item.begin();
    for( ; iter != calibration_etf_item.end(); iter++ ) {
      string fname = dir + string("/") + (*iter)->name() + string(".txt");
      (*iter)->fillTxt( fname.c_str() );
    }
  }

  return;
}


void TofCalibManager::fillRoot( string& dir ) {
  int count = 1;
  char filename1[256];
  sprintf( filename1, "%s/barrel%i.root", dir.c_str(), count );
  TFile  f1(filename1,"RECREATE");
  TTree* ntbarrel = new TTree( "btrk", "barrel data after calibration" );

  int    run;
  int    event;
  int    tofid;
  int    strip;
  double qleft;
  double qright;
  double tleft;
  double tright;
  double zrhit;
  double dt;
  double texp;
  double path;
  double phi;
  double theta;
  double p;
  double t0;
  double q0;
  int    hitcase;

  ntbarrel->Branch( "run",    &run,    "run/I"    );
  ntbarrel->Branch( "event",  &event,  "event/I"  );
  ntbarrel->Branch( "tofid",  &tofid,  "tofid/I"  );
  ntbarrel->Branch( "qleft",  &qleft,  "qleft/D"  );
  ntbarrel->Branch( "qright", &qright, "qright/D" );
  ntbarrel->Branch( "tleft",  &tleft,  "tleft/D"  );
  ntbarrel->Branch( "tright", &tright, "tright/D" );
  ntbarrel->Branch( "zrhit",  &zrhit,  "zrhit/D"  );
  ntbarrel->Branch( "texp",   &texp,   "texp/D"   );
  ntbarrel->Branch( "path",   &path,   "path/D"   );
  ntbarrel->Branch( "phi",    &phi,    "phi/D"    );
  ntbarrel->Branch( "theta",  &theta,  "theta/D"  );
  ntbarrel->Branch( "p",      &p,      "p/D"      );
  ntbarrel->Branch( "t0",     &t0,     "t0/D"     );
  ntbarrel->Branch( "q0",     &q0,     "q0/D"     );
  ntbarrel->Branch( "hitcase",&hitcase,"hitcase/I");

  int counter = 0;
  std::cout << "begin to write..." << filename1 << std::endl;
  for( unsigned int i=0; i<NBarrel; i++ ) {
    std::cout << "\r\b Writing record ... " << (int((++counter)*100./NBarrel)) << "%";
    RecordSet* barrelData = m_dataset->getBarrelData(i);
    std::vector<Record*>::iterator iter = barrelData->begin();
    for( ; iter!=barrelData->end(); iter++ ) {
      run     = (*iter)->run();
      event   = (*iter)->event();
      tofid   = (*iter)->tofid();
      qleft   = (*iter)->qleft();
      qright  = (*iter)->qright();
      tleft   = (*iter)->tleft();
      tright  = (*iter)->tright();
      zrhit   = (*iter)->zrhit();
      texp    = (*iter)->texp();
      path    = (*iter)->path();
      phi     = (*iter)->phi();
      theta   = (*iter)->theta();
      p       = (*iter)->p();
      t0      = (*iter)->t0();
      q0      = (*iter)->q0();
      hitcase = (*iter)->hitcase();
      ntbarrel->Fill();
    }
  }
  std::cout << std::endl;
  ntbarrel->Write();
  delete ntbarrel;
  f1.Close();

  char filename2[256];
  sprintf( filename2, "%s/endcap%i.root", dir.c_str(), count );
  TFile  f2(filename2,"RECREATE");
  TTree* ntendcap = new TTree( "etrk", "endcap data after calibration" );

  ntendcap->Branch( "run",    &run,    "run/I"    );
  ntendcap->Branch( "event",  &event,  "event/I"  );
  ntendcap->Branch( "tofid",  &tofid,  "tofid/I"  );
  ntendcap->Branch( "qleft",  &qleft,  "qleft/D"  );
  ntendcap->Branch( "qright", &qright, "qright/D" );
  ntendcap->Branch( "tleft",  &tleft,  "tleft/D"  );
  ntendcap->Branch( "tright", &tright, "tright/D" );
  ntendcap->Branch( "zrhit",  &zrhit,  "zrhit/D"  );
  ntendcap->Branch( "texp",   &texp,   "texp/D"   );
  ntendcap->Branch( "path",   &path,   "path/D"   );
  ntendcap->Branch( "phi",    &phi,    "phi/D"    );
  ntendcap->Branch( "theta",  &theta,  "theta/D"  );
  ntendcap->Branch( "p",      &p,      "p/D"      );
  ntendcap->Branch( "t0",     &t0,     "t0/D"     );
  ntendcap->Branch( "q0",     &q0,     "q0/D"     );
  ntendcap->Branch( "hitcase",&hitcase,"hitcase/I");

  counter = 0;
  std::cout << "begin to write..." << filename2 << std::endl;
  for( unsigned int i=0; i<NEndcap; i++ ) {
    std::cout << "\r\b Writing record ... " << (int((++counter)*100./NEndcap)) << "%";
    RecordSet* endcapData = m_dataset->getEndcapData(i);
    std::vector<Record*>::iterator iter = endcapData->begin();
    for( ; iter!=endcapData->end(); iter++ ) {
      run     = (*iter)->run();
      event   = (*iter)->event();
      tofid   = (*iter)->tofid();
      qleft   = (*iter)->qleft();
      qright  = (*iter)->qright();
      tleft   = (*iter)->tleft();
      tright  = (*iter)->tright();
      zrhit   = (*iter)->zrhit();
      texp    = (*iter)->texp();
      path    = (*iter)->path();
      phi     = (*iter)->phi();
      theta   = (*iter)->theta();
      p       = (*iter)->p();
      t0      = (*iter)->t0();
      q0      = (*iter)->q0();
      hitcase = (*iter)->hitcase();
      ntendcap->Fill();
    }
  }
  std::cout << std::endl;
  ntendcap->Write();
  delete ntendcap;
  f2.Close();

  char filename3[256];
  sprintf( filename3, "%s/etf%i.root", dir.c_str(), count );
  TFile  f3(filename3,"RECREATE");
  TTree* ntetf = new TTree( "etf", "en data after calibration" );

  ntetf->Branch( "run",    &run,    "run/I"    );
  ntetf->Branch( "event",  &event,  "event/I"  );
  ntetf->Branch( "tofid",  &tofid,  "tofid/I"  );
  ntetf->Branch( "strip",  &strip,  "strip/I"  );
  ntetf->Branch( "qleft",  &qleft,  "qleft/D"  );
  ntetf->Branch( "qright", &qright, "qright/D" );
  ntetf->Branch( "tleft",  &tleft,  "tleft/D"  );
  ntetf->Branch( "tright", &tright, "tright/D" );
  ntetf->Branch( "zrhit",  &zrhit,  "zrhit/D"  );
  ntetf->Branch( "dt",     &dt,     "dt/D"     );
  ntetf->Branch( "texp",   &texp,   "texp/D"   );
  ntetf->Branch( "path",   &path,   "path/D"   );
  ntetf->Branch( "phi",    &phi,    "phi/D"    );
  ntetf->Branch( "p",      &p,      "p/D"      );
  ntetf->Branch( "t0",     &t0,     "t0/D"     );
  ntetf->Branch( "q0",     &q0,     "q0/D"     );
  ntetf->Branch( "hitcase",&hitcase,"hitcase/I");

  counter = 0;
  std::cout << "begin to write..." << filename3 << std::endl;
  for( unsigned int i=0; i<NEtf*NStrip; i++ ) {
    std::cout << "\r\b Writing record ... " << (int((++counter)*100./(NEtf*NStrip))) << "%";
    RecordSet* etfData = m_dataset->getEtfData(i);
    std::vector<Record*>::iterator iter = etfData->begin();
    for( ; iter!=etfData->end(); iter++ ) {
      run     = (*iter)->run();
      event   = (*iter)->event();
      tofid   = (*iter)->tofid();
      strip   = (*iter)->strip();
      qleft   = (*iter)->qleft();
      qright  = (*iter)->qright();
      tleft   = (*iter)->tleft();
      tright  = (*iter)->tright();
      zrhit   = (*iter)->zrhit();
      dt      = (*iter)->dt();
      texp    = (*iter)->texp();
      path    = (*iter)->path();
      phi     = (*iter)->phi();
      p       = (*iter)->p();
      t0      = (*iter)->t0();
      q0      = (*iter)->q0();
      hitcase = (*iter)->hitcase();
      ntetf->Fill();
    }
  }
  std::cout << std::endl;
  ntetf->Write();
  delete ntetf;
  f3.Close();

  if( !calibration_barrel_item.empty() ) {
    std::vector<TofCalibFit*>::iterator iter = calibration_barrel_item.begin();
    for( ; iter != calibration_barrel_item.end(); iter++ ) {
      string fname = dir + string("/") + (*iter)->name() + string(".root");
      (*iter)->fillRoot( fname.c_str() );
    }
  }
  if( !calibration_barrel_item2.empty() ) {
    std::vector<TofCalibFit*>::iterator iter = calibration_barrel_item2.begin();
    for( ; iter != calibration_barrel_item2.end(); iter++ ) {
      string fname = dir + string("/") + (*iter)->name() + string(".root");
      (*iter)->fillRoot( fname.c_str() );
    }
  }

  if( !calibration_endcap_item.empty() ) {
    std::vector<TofCalibFit*>::iterator iter = calibration_endcap_item.begin();
    for( ; iter != calibration_endcap_item.end(); iter++ ) {
      string fname = dir + string("/") + (*iter)->name() + string(".root");
      (*iter)->fillRoot( fname.c_str() );
    }
  }

  if( !calibration_etf_item.empty() ) {
    std::vector<TofCalibFit*>::iterator iter = calibration_etf_item.begin();
    for( ; iter != calibration_etf_item.end(); iter++ ) {
      string fname = dir + string("/") + (*iter)->name() + string(".root");
      (*iter)->fillRoot( fname.c_str() );
    }
  }

  return;
}
