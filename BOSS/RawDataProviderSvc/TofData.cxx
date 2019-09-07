#include "RawEvent/RawDataUtil.h"
#include "RawDataProviderSvc/TofData.h"
#include "TofQCorrSvc/ITofQCorrSvc.h"
#include "TofQElecSvc/ITofQElecSvc.h"
#include "Identifier/Identifier.h"
#include "Identifier/TofID.h"
#include <iostream>

extern ITofQCorrSvc* tofQCorrSvc;
extern ITofQElecSvc* tofQElecSvc;

TofValue::TofValue() {
  m_channel = -999;
  m_value  = -999.0;
  m_times  = -1;
  m_number = 0;
  m_clock  = -999;
  m_used   = false;
  return;
}


TofValue::~TofValue() {}


TofValue& TofValue::operator=(const TofValue& source){
  m_channel = source.m_channel;
  m_value  = source.m_value;
  m_times  = source.m_times;
  m_number = source.m_number;
  m_clock  = source.m_clock;
  m_used   = source.m_used;
  return *this;
}


void TofValue::timespp() {
  m_times = m_times + 1;
  return;
}


void TofValue::timesmm() {
  m_times = m_times - 1;
  return;
}


void Tdc::setValue( int value ) {
  m_clock = ( ( value & 0x7e000 ) >> 13 );
  if( value == 0x7fffffff ) { 
    m_clock   = 100;
    m_channel = 10000;
    m_value   = 10000.0;
  }
  else if( value == -999 ) {
    m_channel = -999;
    m_value   = -999.0;
  }
  else { 
    m_channel = value;
    m_value = RawDataUtil::TofTime( value );
  }

  return;
}


Adc::Adc() {
  m_corr = false;
  m_elec = false;
  m_mrpc = false;
  m_qtc  = -999.0;
}


void Adc::setValue( Identifier identify, int value ) {
  m_clock = ( ( value & 0x7e000 ) >> 13 );
  if( value == 0x7fffffff ) {
    m_clock      = 100;
    m_channel    = 10000;
    m_qtc        = 10000.0;
    m_value      = 10000.0;
  }
  else if( value == -999 ) {
    m_channel    = -999;
    m_qtc        = -999.0;
    m_value      = -999.0;
  }
  else {

    m_mrpc = TofID::is_mrpc( identify );
    if( m_mrpc ) {
      m_channel = value;
      m_qtc = RawDataUtil::TofTime( m_channel );
    }
    else {
      if( !( TofID::is_scin( identify ) ) ) {
	std::cout << "Event/RawDataProviderSvc:: ERROR! ETF(MRPC) data is treated as scintillator data !" << std::endl;
      }

      m_channel = ( value  & 0x1fff );
      if( ( value & 0x80000 ) != 0 ) {
	if( m_corr ) { 
	  if( m_channel < 4000 ) {
	    m_channel += 0x2000;
	  }
	}
	else {
	  m_channel += 0x2000;
	}
      }

      int barrel = TofID::barrel_ec( identify );
      int tofid  = TofID::phi_module( identify );
      int layer  = TofID::layer( identify );
      if( barrel==1 ) {
	if( layer==1 ) {
	  tofid = tofid + 88;
	}
      }
      else if( barrel==2 ) {
	tofid = tofid + 48;
      }
      int east = TofID::end( identify );

      if( m_corr ) {
	if( barrel==1 ) {
	  if( east==0 ) {
	    m_qtc = tofQCorrSvc->BQRaw1( tofid, m_channel*1.0 );
	  }
	  else {
	    m_qtc = tofQCorrSvc->BQRaw2( tofid, m_channel*1.0 );
	  }
	}
	else {
	  m_qtc = tofQCorrSvc->EQRaw( tofid, m_channel*1.0 );
	}
      }
      else {
	m_qtc = m_channel*1.0;
      }

      if( m_elec ) {
	if( barrel==1 ) {
	  if( east==0 ) {
	    m_value = tofQElecSvc->BQTC1( tofid, m_qtc );
	  }
	  else {
	    m_value = tofQElecSvc->BQTC2( tofid, m_qtc );
	  }
	}
	else {
	  m_value = tofQElecSvc->EQTC( tofid, m_qtc );
	}
      }
      else {
	m_value = m_qtc*1.0;
      }
    }

  }
  return;
}

/*
void Adc::calculate( Identifier identify ) {

  if( m_calculated ) return;

  m_mrpc = TofID::is_mrpc( identify );
  if( m_mrpc ) {
    m_qtc = RawDataUtil::TofTime( m_channel );
  }
  else {
    if( !( TofID::is_scin( identify ) ) ) {
      std::cout << "Event/RawDataProviderSvc:: ERROR! ETF(MRPC) data is treated as scintillator data !" << std::endl;
    }

    int channel = ( m_channel  & 0x1fff );
    if( ( m_channel & 0x80000 ) != 0 ) {
      if( m_corr ) { 
	if( channel < 4000 ) {
	  channel += 0x2000;
	}
      }
      else {
	channel += 0x2000;
      }
    }
    m_channel = channel;

    int barrel = TofID::barrel_ec( identify );
    int tofid  = TofID::phi_module( identify );
    int layer  = TofID::layer( identify );
    if( barrel==1 ) {
      if( layer==1 ) {
	tofid = tofid + 88;
      }
    }
    else if( barrel==2 ) {
      tofid = tofid + 48;
    }
    int east   = TofID::end( identify );

    if( m_corr ) {
      if( barrel==1 ) {
	if( east==0 ) {
	  m_qtc = tofQCorrSvc->BQRaw1( tofid, m_channel*1.0 );
	}
	else {
	  m_qtc = tofQCorrSvc->BQRaw2( tofid, m_channel*1.0 );
	}
      }
      else {
	m_qtc = tofQCorrSvc->EQRaw( tofid, m_channel*1.0 );
      }
    }
    else {
      m_qtc = m_channel*1.0;
    }

    if( m_elec ) {
      if( barrel==1 ) {
	if( east==0 ) {
	  m_value = tofQElecSvc->BQTC1( tofid, m_qtc );
	}
	else {
	  m_value = tofQElecSvc->BQTC2( tofid, m_qtc );
	}
      }
      else {
	m_value = tofQElecSvc->EQTC( tofid, m_qtc );
      }
    }
    else {
      m_value = m_qtc*1.0;
    }
  }

  m_calculated = true;
  return;
}
*/

PmtData::PmtData() {
  m_iden_value = 0;
  m_times      = 0;
  m_quality    = 0;
  m_adc        = 0;
  m_tdc        = 0;
}


PmtData::~PmtData() {
  PmtData::clear();
}


PmtData& PmtData::operator=(const PmtData& source){
  m_iden_value = source.m_iden_value;
  m_times      = source.m_times;
  m_quality    = source.m_quality;
  m_adc        = source.m_adc;
  m_tdc        = source.m_tdc;
  return *this;
}


int PmtData::adcChannel() {
  int channel = -999;
  if( m_adc ) {
    channel = m_adc->channel();
  }
  return channel;
}


double PmtData::qtc() {
  double value = -999.0;
  if( m_adc ) {
    value = m_adc->qtc();
  }
  else {
    if( m_tdc ) {
      value = 10000.0;
    }
  }
  return value;
}


double PmtData::adc() {
  double value = -999.0;
  if( m_adc ) {

    if( TofID::is_mrpc( TofID::cell_id( m_iden_value ) ) ) {
      if( m_tdc ) {
	double tleading = m_tdc->value();
	if( tleading>0.0 ) {
	  int qchannel = m_adc->channel();
	  if( qchannel!=10000 && qchannel!=-999 ) {
	    double ttrailing = m_adc->qtc();
	    value = ttrailing - tleading;
	  }
	}
      }
    }
    else {
      value = m_adc->value();
    }
  }
  else {
    if( m_tdc ) {
      value = 10000.0;
    }
  }
  return value;
}


int PmtData::qclock() {
  int clock = -999;
  if( m_adc ) {
    clock = m_adc->clock();
  }
  return clock;
}


int PmtData::tdcChannel() {
  int channel = -999;
  if( m_tdc ) {
    channel = m_tdc->channel();
  }
  return channel;
}


double PmtData::tdc() {
  double value = -999.;
  if( m_tdc ) {
    value = m_tdc->value();
  }
  return value;
}


int PmtData::tclock() {
  int clock = -999;
  if( m_tdc ) {
    clock = m_tdc->clock();
  }
  return clock;
}


void PmtData::timespp() {
  m_times = m_times + 1;
  return;
}


void PmtData::timesmm() {
  m_times = m_times - 1;
  return;
}


void PmtData::qtimespp() {
  if( m_adc ) {
    m_adc->timespp();
  }
  return;
}


void PmtData::qtimesmm() {
  if( m_adc ) {
    m_adc->timesmm();
  }
  return;
}


void PmtData::ttimespp() {
  if( m_tdc ) {
    m_tdc->timespp();
  }
  return;
}


void PmtData::ttimesmm() {
  if( m_tdc ) {
    m_tdc->timesmm();
  }
  return;
}


int PmtData::qtimes() {
  int times = -1;
  if( m_adc ) {
    times = m_adc->times();
  }
  return times;
}


int PmtData::ttimes() {
  int times = -1;
  if( m_tdc ) {
    times = m_tdc->times();
  }
  return times;
}


int PmtData::qnumber() {
  int number = 0;
  if( m_adc ) {
    number = m_adc->number();
  }
  return number;
}


int PmtData::tnumber() {
  int number = 0;
  if( m_tdc ) {
    number = m_tdc->number();
  }
  return number;
}


bool PmtData::qused() {
  bool used = false;
  if( m_adc ) {
    used = m_adc->used();
  }
  return used;
}


bool PmtData::tused() {
  bool used = false;
  if( m_tdc ) {
    used = m_tdc->used();
  }
  return used;
}


bool PmtData::used() {
  bool used = false;
  if( m_adc ) {
    used = ( used || m_adc->used() );
  }
  if( m_tdc ) {
    used = ( used || m_tdc->used() );
  }
  return used;
}


void PmtData::setAdc( Adc* adc ) {
  m_adc = adc;
  if( adc->channel() != -999 ) {
    if( m_adc->times() == -1 ) { m_adc->timespp(); }
    m_quality = ( m_quality | 0x2 );
  }
  return;
}


void PmtData::setTdc( Tdc* tdc ) {
  m_tdc = tdc;
  if( tdc->channel() != -999 ) {
    if( m_tdc->times() == -1 ) { m_tdc->timespp(); }
    m_quality = ( m_quality | 0x1 );
  }
  return;
}


void PmtData::setUsed() {
  if( m_adc ) {
    m_adc->setUsed( true );
  }
  if( m_tdc ) {
    m_tdc->setUsed( true );
  }
  return;
}


void PmtData::clear() {
  if( m_adc ) {
    if( m_adc->times() <= 1 ) {
      delete m_adc;
      m_adc = 0;
    }
    else {
      m_adc->timesmm();
    }
  }
  if( m_tdc ) {
    if( m_tdc->times() <= 1 ) {
      delete m_tdc;
      m_tdc = 0;
    }
    else {
      m_tdc->timesmm();
    }
  }
  return;
}


TofData::TofData() {
  m_identify = 0;
  m_mrpc     = false;
  m_barrel   = false;
  m_tofId    = -1;
  m_layer    = -1;
  m_strip    = -1;
  m_tofTrackId = -1;
  m_misLable = false;
  m_quality  = 0;
  m_tmatched = false;
  m_forward  = 0;
  m_backward = 0;
  m_ztdc     = -999;
  m_zadc     = -999;
  m_zpos     = -999;
  m_energy   = 0;
}


TofData& TofData::operator=(const TofData& source){
  m_identify = source.m_identify;
  m_mrpc     = source.m_mrpc;
  m_barrel   = source.m_barrel;
  m_tofId    = source.m_tofId;
  m_layer    = source.m_layer;
  m_strip    = source.m_strip;
  m_tofTrackId = source.m_tofTrackId;
  m_misLable = source.m_misLable;
  m_quality  = source.m_quality;
  m_tmatched = source.m_tmatched;
  m_forward  = source.m_forward;
  m_backward = source.m_backward;
  m_ztdc     = source.m_ztdc;
  m_zadc     = source.m_zadc;
  m_zpos     = source.m_zpos;
  m_energy   = source.m_energy;
  return *this;
}


TofData::~TofData() {
  TofData::clear();
}

/*
int TofData::tofId() {
  if( !m_mrpc && !m_barrel && m_misLable ) {
    if( m_tofId == 90 ) { return 91; }
    else if( m_tofId == 91 ) { return 90; }
  }
  return m_tofId;
}
*/

void TofData::setIdentify( Identifier identify ) {
  m_identify = identify.get_value();
  m_mrpc = TofID::is_mrpc( identify );
  if( !m_mrpc ) {
    int barrel_ec = TofID::barrel_ec(identify);
    m_layer  = TofID::layer(identify);
    m_tofId  = TofID::phi_module(identify);
    if( barrel_ec == 0 ) {
      m_barrel = false;
    }
    else if( barrel_ec == 1 ) {
      m_barrel = true;
      if( m_layer == 1 ) { m_tofId = m_tofId + 88; } 
    }
    else if( barrel_ec == 2 ) {
      m_barrel = false;
      m_tofId = m_tofId + 48;
    }
  }
  else {
    m_barrel = false;  
    int endcap = TofID::endcap(identify);
    m_tofId  = TofID::module(identify);
    if( endcap==1 ) {
      m_tofId = m_tofId + 36;
    }
    m_strip  = TofID::strip(identify);
  }

  return;
}


void TofData::data( double& adcEast, double& tdcEast, double& adcWest, double& tdcWest ) {
  adcEast = -999.;
  tdcEast = -999.;
  if( m_forward ) {
    adcEast = m_forward->adc();
    tdcEast = m_forward->tdc();
  }
  adcWest = -999.;
  tdcWest = -999.;
  if( m_backward ) {
    adcWest = m_backward->adc();
    tdcWest = m_backward->tdc();
  }
  return;
}


void TofData::data(double& adc, double& tdc ) {
  adc = -999.;
  tdc = -999.;
  if( m_forward ) {
    adc = m_forward->adc();
    tdc = m_forward->tdc();
  }
  return;
}


double TofData::adc1() {
  double value = -999.;
  if( m_forward ) {
    value = m_forward->adc();
  }
  return value;
}


double TofData::tdc1() {
  double value = -999.;
  if( m_forward ) {
    value = m_forward->tdc();
  }
  return value;
}


double TofData::adc2() {
  double value = -999.;
  if( m_backward ) {
    value = m_backward->adc();
  }
  return value;
}


double TofData::tdc2() {
  double value = -999.;
  if( m_backward ) {
    value = m_backward->tdc();
  }
  return value;
}


double TofData::adc() {
  double value = -999.;
  if( m_forward ) {
    value = m_forward->adc();
  }
  return value;
}


double TofData::tdc() {
  double value = -999.;
  if( m_forward ) {
    value = m_forward->tdc();
  }
  return value;
}


double TofData::qtc1() {
  double value = -999.;
  if( m_forward ) {
    value = m_forward->qtc();
  }
  return value;
}


double TofData::qtc2() {
  double value = -999.;
  if( m_backward ) {
    value = m_backward->qtc();
  }
  return value;
}


double TofData::qtc() {
  double value = -999.;
  if( m_forward ) {
    value = m_forward->qtc();
  }
  return value;
}


int TofData::adcChannelEast() {
  int channel = -999;
  if( m_forward ) {
    channel = m_forward->adcChannel();
  }
  return channel;
}


int TofData::tdcChannelEast() {
  int channel = -999;
  if( m_forward ) {
    channel = m_forward->tdcChannel();
  }
  return channel;
}


int TofData::adcChannelWest() {
  int channel = -999;
  if( m_backward ) {
    channel = m_backward->adcChannel();
  }
  return channel;
}


int TofData::tdcChannelWest() {
  int channel = -999;
  if( m_backward ) {
    channel = m_backward->tdcChannel();
  }
  return channel;
}


int TofData::adcChannel() {
  int channel = -999;
  if( m_forward ) {
    channel = m_forward->adcChannel();
  }
  return channel;
}


int TofData::tdcChannel() {
  int channel = -999;
  if( m_forward ) {
    channel = m_forward->tdcChannel();
  }
  return channel;
}


int TofData::qclock1() {
  int clock = -999;
  if( m_forward ) {
    clock = m_forward->qclock();
  }
  return clock;
}


int TofData::tclock1() {
  int clock = -999;
  if( m_forward ) {
    clock = m_forward->tclock();
  }
  return clock;
}


int TofData::qclock2() {
  int clock = -999;
  if( m_backward ) {
    clock = m_backward->qclock();
  }
  return clock;
}


int TofData::tclock2() {
  int clock = -999;
  if( m_backward ) {
    clock = m_backward->tclock();
  }
  return clock;
}


int TofData::qclock() {
  int clock = -999;
  if( m_forward ) {
    clock = m_forward->qclock();
  }
  return clock;
}


int TofData::tclock() {
  int clock = -999;
  if( m_forward ) {
    clock = m_forward->tclock();
  }
  return clock;
}


int TofData::qtimes1() {
  int times = -1;
  if( m_forward ) {
    times = m_forward->qtimes();
  }
  return times;
}


int TofData::ttimes1() {
  int times = -1;
  if( m_forward ) {
    times = m_forward->ttimes();
  }
  return times;
}


int TofData::qtimes2() {
  int times = -1;
  if( m_backward ) {
    times = m_backward->qtimes();
  }
  return times;
}


int TofData::ttimes2() {
  int times = -1;
  if( m_backward ) {
    times = m_backward->ttimes();
  }
  return times;
}


unsigned int TofData::eastTimes() {
  int times = 0;
  if( m_forward ) {
    times = m_forward->times();
  }
  return times;
}


unsigned int TofData::westTimes() {
  int times = 0;
  if( m_backward ) {
    times = m_backward->times();
  }
  return times;
}


int TofData::qnumber1() {
  int number = 0;
  if( m_forward ) {
    number = m_forward->qnumber();
  }
  return number;
}


int TofData::tnumber1() {
  int number = 0;
  if( m_forward ) {
    number = m_forward->tnumber();
  }
  return number;
}


int TofData::qnumber2() {
  int number = 0;
  if( m_backward ) {
    number = m_backward->qnumber();
  }
  return number;
}


int TofData::tnumber2() {
  int number = 0;
  if( m_backward ) {
    number = m_backward->tnumber();
  }
  return number;
}


bool TofData::qused1() {
  bool used = false;
  if( m_forward ) {
    used = ( used || m_forward->qused() );
  }
  return used;
}


bool TofData::tused1() {
  bool used = false;
  if( m_forward ) {
    used = ( used || m_forward->tused() );
  }
  return used;
}


bool TofData::qused2() {
  bool used = false;
  if( m_backward ) {
    used = ( used || m_backward->qused() );
  }
  return used;
}


bool TofData::tused2() {
  bool used = false;
  if( m_backward ) {
    used = ( used || m_backward->tused() );
  }
  return used;
}


bool TofData::used1() {
  bool used = false;
  if( m_forward ) {
    used = ( used || m_forward->used() );
  }
  return used;
}


bool TofData::used2() {
  bool used = false;
  if( m_backward ) {
    used = ( used || m_backward->used() );
  }
  return used;
}


bool TofData::used() {
  bool used = false;
  if( m_forward ) {
    used = ( used || m_forward->used() );
  }
  if( m_backward ) {
    used = ( used || m_backward->used() );
  }
  return used;
}


void TofData::setForward( PmtData* pmt ) {
  m_forward = pmt;
  unsigned int quality = m_forward->quality();
  quality = ( quality & 0x3 );
  if( quality != 0 ) {
    m_forward->timespp();
    if( ( quality & 0x1 ) != 0 ) {
      m_forward->ttimespp();
    }
    if( ( quality & 0x2 ) != 0 ) {
      m_forward->qtimespp();
    }
  }
  m_quality = ( m_quality | ( quality << 2 ) );
  return;
}


void TofData::setBackward( PmtData* pmt ) {
  m_backward = pmt;
  unsigned int quality = m_backward->quality();
  quality = ( quality & 0x3 );
  if( quality != 0 ) {
    m_backward->timespp();
    if( ( quality & 0x1 ) != 0 ) {
      m_backward->ttimespp();
    }
    if( ( quality & 0x2 ) != 0 ) {
      m_backward->qtimespp();
    }
  }
  m_quality = ( m_quality | quality );
  return;
}


void TofData::setUsed() {
  if( m_forward ) {
    m_forward->setUsed();
  }
  if( m_backward ) {
    m_backward->setUsed();
  }
  return;
}


void TofData::clear() {
  if( m_forward ) {
    if( m_forward->times() <= 1 ) {
      delete m_forward;
      m_forward = 0;
    }
    else {
      m_forward->timesmm();
    }
  }
  if( m_backward ) {
    if( m_backward->times() <= 1 ) {
      delete m_backward;
      m_backward = 0;
    }
    else {
      m_backward->timesmm();
    }
  }
  return;
}
