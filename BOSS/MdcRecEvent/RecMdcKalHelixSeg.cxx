/*
 *  @author : wangjk 
 *
 * **********************************************/
#include "MdcRecEvent/RecMdcKalHelixSeg.h"
#include "MdcRecEvent/RecMdcKalTrack.h"

RecMdcKalHelixSeg::RecMdcKalHelixSeg()

: m_helix_incl(5,0),
  m_helix_excl(5,0),
  m_error_incl(5,0),
  m_error_excl(5,0),
  m_trackid(0),
  m_mdcid(0),
  m_flagLR(0),
  m_tdc(0),
  m_adc(0),
  m_zhit(0),
  m_tof(0),
  m_doca_incl(0),
  m_doca_excl(0),
  m_dd(0),
  m_entra(0),
  m_dt(0),
  m_res_incl(0),
  m_res_excl(0),
  m_layerId(0)

{
}

RecMdcKalHelixSeg::RecMdcKalHelixSeg( RecMdcKalHelixSeg& helixseg)

 :
  m_helix_incl(helixseg.m_helix_incl),
  m_helix_excl(helixseg.m_helix_excl),
  m_error_incl(helixseg.m_error_incl),
  m_error_excl(helixseg.m_error_excl),
  m_trackid(helixseg.m_trackid),
  m_mdcid(helixseg.m_mdcid),
  m_flagLR(helixseg.m_flagLR),
  m_tdc(helixseg.m_tdc),
  m_adc(helixseg.m_adc),
  m_zhit(helixseg.m_zhit),
  m_tof(helixseg.m_tof),
  m_doca_incl(helixseg.m_doca_incl),
  m_doca_excl(helixseg.m_doca_excl),
  m_dd(helixseg.m_dd),
  m_entra(helixseg.m_entra),
  m_dt(helixseg.m_dt),
  m_res_incl(helixseg.m_res_incl),
  m_res_excl(helixseg.m_res_excl),
  m_layerId(helixseg.m_layerId)

{   }

RecMdcKalHelixSeg::~RecMdcKalHelixSeg()
{


}




