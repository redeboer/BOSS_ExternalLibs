#include "tofcalgsec/TofCalibCheck.h"
#include <cmath>

TofCalibCheck::TofCalibCheck( NTuple::Tuple*& tuple ):m_tuple(tuple) {
  if( !m_tuple ) {
    std::cerr << " Error: invalid pionter in TofCalibCheck(NTuple::Tuple*) for m_tuple " << std::endl;
  }
  else {
    m_tuple->addItem("run",     m_run      );
    m_tuple->addItem("event",   m_event    );
    m_tuple->addItem("tofid",   m_tofid    );
    m_tuple->addItem("strip",   m_strip    );
    m_tuple->addItem("qleft",   m_q1       );
    m_tuple->addItem("qright",  m_q2       );
    m_tuple->addItem("tleft",   m_t1       );
    m_tuple->addItem("tright",  m_t2       );
    m_tuple->addItem("zrhit",   m_zrhit    );
    m_tuple->addItem("dt",      m_dt       );
    m_tuple->addItem("texp",    m_texp     );
    m_tuple->addItem("path",    m_path     );
    m_tuple->addItem("phi",     m_phi      );
    m_tuple->addItem("theta",   m_theta    );
    m_tuple->addItem("p",       m_p        );
    m_tuple->addItem("t0",      m_t0       );
    m_tuple->addItem("q0",      m_q0       );
    m_tuple->addItem("hitcase", m_hitcase  );
  }
}


void TofCalibCheck::FillBarrelCol( RecBTofCalHitCol& bhitcol ) {
  if( m_tuple ) {
    RecBTofCalHitCol::iterator iter = bhitcol.begin();
    for( ; iter!=bhitcol.end(); iter++ ) {
      if( fabs( (*iter)->dzHit() - 1.0 )>1.0e-6 ) continue;
      m_run   = (*iter)->run();
      m_event = (*iter)->event();
      m_tofid = (*iter)->mod();
      m_strip = -1;
      m_q1    = (*iter)->adc1();
      m_q2    = (*iter)->adc2();
      m_t1    = (*iter)->tdc1();
      m_t2    = (*iter)->tdc2();
      m_zrhit = (*iter)->zHit();
      m_dt    = -99.0;
      m_texp  = (*iter)->tpred();
      m_path  = (*iter)->path();
      m_phi   = (*iter)->deltaPhi();
      m_theta = (*iter)->sinTheta();
      m_p     = (*iter)->p();
      m_t0    = -99.0;
      m_q0    = -99.0;
      m_hitcase = (*iter)->qual();
      m_tuple->write();
    }
  }
  return;
}


void TofCalibCheck::FillEndcapCol( RecETofCalHitCol& ehitcol ) {
  if( m_tuple ) {
    RecETofCalHitCol::iterator iter = ehitcol.begin();
    for( ; iter!=ehitcol.end(); iter++ ) {
      m_run   = (*iter)->run();
      m_event = (*iter)->event();
      m_tofid = (*iter)->mod();
      m_strip = -1;
      m_q1    = (*iter)->adc();
      m_q2    = -99.0;
      m_t1    = (*iter)->tdc();
      m_t2    = -99.0;
      m_zrhit = (*iter)->rHit();
      m_dt    = -99.0;
      m_texp  = (*iter)->tpred();
      m_path  = (*iter)->path();
      m_phi   = (*iter)->deltaPhi();
      m_theta = (*iter)->cosTheta();
      m_p     = (*iter)->p();
      m_t0    = -99.0;
      m_q0    = -99.0;
      m_hitcase = (*iter)->qual();
      m_tuple->write();
    }
  }
  return;
}


void TofCalibCheck::FillEtfCol( RecBTofCalHitCol& bhitcol ) {
  if( m_tuple ) {
    RecBTofCalHitCol::iterator iter = bhitcol.begin();
    for( ; iter!=bhitcol.end(); iter++ ) {
      if( fabs( (*iter)->dzHit() )>1.0e-6 ) continue;
      m_run   = (*iter)->run();
      m_event = (*iter)->event();
      m_tofid = (*iter)->mod();
      m_strip = (*iter)->sinTheta();
      m_q1    = (*iter)->adc1();
      m_q2    = (*iter)->adc2();
      m_t1    = (*iter)->tdc1();
      m_t2    = (*iter)->tdc2();
      m_zrhit = (*iter)->zHit();
      m_dt    = ( (*iter)->tdc2() - (*iter)->tdc1() )/2.0;
      m_texp  = (*iter)->tpred();
      m_path  = (*iter)->path();
      m_phi   = (*iter)->deltaPhi();
      m_theta = (*iter)->sinTheta();
      m_p     = (*iter)->p();
      m_t0    = -99.0;
      m_q0    = -99.0;
      m_hitcase = (*iter)->qual();
      m_tuple->write();
    }
  }
  return;
}
