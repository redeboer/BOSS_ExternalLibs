#include "EvtRecEvent/EvtRecDTag.h"

EvtRecDTag::EvtRecDTag() :
    m_decayMode(kUnknown),
    m_type(Default),
    m_beamE(0),
    m_mass(0),
    m_mBC(0),
    m_deltaE(0),
    m_charge(0),
    m_charm(0),
    m_numOfChildren(0),
    m_p4(0)
{
  m_tracks.clear();
  m_showers.clear();
  m_otherTracks.clear();
  m_otherShowers.clear();
  m_pionId.clear();
  m_kaonId.clear();
  m_ksmass.clear();
  m_vfitchi2.clear();
  m_vfitlength.clear();
  m_vfiterror.clear();
}

// copy ctor
EvtRecDTag::EvtRecDTag(const EvtRecDTag& dtag) :
    ContainedObject(dtag),
    m_decayMode(dtag.m_decayMode),
    m_type(dtag.m_type),
    m_beamE(dtag.m_beamE),
    m_mass(dtag.m_mass),
    m_mBC(dtag.m_mBC),
    m_deltaE(dtag.m_deltaE),
    m_charge(dtag.m_charge),
    m_charm(dtag.m_charm),
    m_numOfChildren(dtag.m_numOfChildren),
    m_p4(dtag.m_p4),
    m_tracks(dtag.m_tracks),
    m_showers(dtag.m_showers),
    m_otherTracks(dtag.m_otherTracks),
    m_otherShowers(dtag.m_otherShowers),
    m_pionId(dtag.m_pionId),
    m_kaonId(dtag.m_kaonId),
    m_ksmass(dtag.m_ksmass),
    m_vfitchi2(dtag.m_vfitchi2),
    m_vfitlength(dtag.m_vfitlength),
    m_vfiterror(dtag.m_vfiterror)
{
}

// assignment operator
EvtRecDTag& EvtRecDTag::operator=(const EvtRecDTag & dtag) {
  this->ContainedObject::operator=(dtag);
  m_decayMode=dtag.m_decayMode;
  m_type=dtag.m_type;
  m_beamE=dtag.m_beamE;
  m_mass=dtag.m_mass;
  m_mBC=dtag.m_mBC;
  m_deltaE=dtag.m_deltaE;
  m_charge=dtag.m_charge;
  m_charm=dtag.m_charm;
  m_numOfChildren=dtag.m_numOfChildren;
  m_p4=dtag.m_p4;
  m_tracks=dtag.m_tracks;
  m_showers=dtag.m_showers;
  m_otherTracks=dtag.m_otherTracks;
  m_otherShowers=dtag.m_otherShowers;
  m_pionId=dtag.m_pionId;
  m_kaonId=dtag.m_kaonId;
  m_ksmass=dtag.m_ksmass;
  m_vfitchi2=dtag.m_vfitchi2;
  m_vfitlength=dtag.m_vfitlength;
  m_vfiterror=dtag.m_vfiterror;
  return *this;
}
