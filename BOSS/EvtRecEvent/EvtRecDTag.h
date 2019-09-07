#ifndef Event_EvtRecDTag_H
#define Event_EvtRecDTag_H

#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"
#include "EventModel/EventModel.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include <utility>
#include <vector>

using CLHEP::HepVector;
using CLHEP::HepSymMatrix;
using namespace EventModel;
extern const CLID & CLID_EvtRecDTag;

// 
// To store D Tagged events.
// Author : Chunlei Liu   Date: 2009.1.26
//
// Author : Onur Albayrak, Jake Bennett Date: 2014.12.19
//

class EvtRecDTag : virtual public ContainedObject {
 public:
  virtual const CLID& clID() const {return EvtRecDTag::classID();}
  static  const CLID& classID() {return CLID_EvtRecDTag;}

  EvtRecDTag();
  ~EvtRecDTag() {;}

  EvtRecDTag(const EvtRecDTag&);
  EvtRecDTag & operator = (const EvtRecDTag &);


  //selection critera type defined with enum type

  enum SelType
    {
      Loose,
      Tight,
      Default
    };
  
  
  //  All decay modes are defined as enum types here
  enum DecayMode
  {
    kD0toKPi,
    kD0toKPiPi0,
    kD0toKPiPi0Pi0,
    kD0toKPiPiPi,

    kD0toKPiPiPiPi0,
    kD0toKPiEta,

    kD0toKsKPi = 50,
    kD0toKsKPiPi0,

    kD0toKsPiPi = 100,
    kD0toKsPiPiPi0,
    kD0toKsPi0,

    kD0toPiPiPi0,
    kD0toPiPi,
    kD0toKK,
    kD0toKKPi0,
    kD0toPi0Pi0,
    kD0toKsKs,
    kD0toKsKsPi0,
    kD0toKsPi0Pi0,
    kD0toKsKK,
    kD0toKsEta,
    kD0toPi0Pi0Pi0,

    kD0toKsKsKs,
    kD0toPiPiPiPi,
    kD0toPiPiPi0Pi0,
    kD0toKKPiPi,
    kD0toKKPi0Pi0,
    kD0toKsKsPiPi,
    kD0toPiPiPiPiPi0,
    kD0toKsPiPiPiPi,
    kD0toKKPiPiPi0,
    kD0toKsPi0Eta,

    kD0toKsEPPiPiEta,
    kD0toKsEPRhoGam,
    kD0toKsPi0Pi0Pi0,

    kDptoKPiPi =200,
    kDptoKPiPiPi0,
    kDptoKsPi,
    kDptoKsPiPi0,
    kDptoKsPiPiPi,
    kDptoKKPi,

    kDptoPiPi0,
    kDptoKPi0,
    kDptoKsK,
    kDptoPiPiPi,
    kDptoPiPi0Pi0,
    kDptoKsKsPi,
    kDptoKsKPi0,
    kDptoKsKsK,
    kDptoPiPiPiPi0,
    kDptoKsPiPi0Pi0,
    kDptoKsKplusPiPi,
    kDptoKsKminusPiPi,
    kDptoKKPiPi0,
    kDptoPiPiPiPiPi,
    kDptoKPiPiPiPi,
    kDptoPiEta,
    kDptoKsPiEta,

    kDstoKsK = 400,
    kDstoKKPi,
    kDstoKsKPi0,
    kDstoKsKsPi,
    kDstoKKPiPi0,
    kDstoKsKplusPiPi,
    kDstoKsKminusPiPi,
    kDstoKKPiPiPi,

    kDstoPiPi0 = 420,
    kDstoPiPiPi,
    kDstoPiPiPiPi0,
    kDstoPiPiPiPiPi,
    kDstoPiPiPiPiPiPi0,
    kDstoPiPiPiPi0Pi0, // New

    kDstoPiEta = 440,
    kDstoPiPi0Eta,
    kDstoPiPiPiEta,

    kDstoPiEtaPiPiPi0 = 450, // New
    kDstoPiPi0EtaPiPiPi0, // New
    kDstoPiPiPiEtaPiPiPi0, // New

    kDstoPiEPPiPiEta = 460,
    kDstoPiPi0EPPiPiEta,

    kDstoPiEPPiPiEtaPiPiPi0 = 470, // New
    kDstoPiPi0EPPiPiEtaPiPiPi0, // New

    kDstoPiEPRhoGam = 480,
    kDstoPiPi0EPRhoGam,

    kDstoKsPi = 500,
    kDstoKsPiPi0,
    kDstoKPiPi,
    kDstoKPiPiPi0,
    kDstoKKK,

    kUnknown = 10000

  };


  DecayMode decayMode() const {return m_decayMode;}
  SelType type() const {return m_type;}
  double beamE() const {return m_beamE;}
  double mass()  const {return m_mass;}
  double mBC() const {return m_mBC;}
  double deltaE() const {return m_deltaE;}
  int    charge() const {return m_charge;}
  int    charm() const {return m_charm;}
  int    numOfChildren() const{return m_numOfChildren;}
  HepLorentzVector p4() const {return m_p4;}
  vector< double > vKsMass() const {return m_ksmass;}
  vector< double > vFitChi2() const {return m_vfitchi2;}
  vector< double > vFitDecayLength() const {return m_vfitlength;}
  vector< double > vFitDecayLengthError() const {return m_vfiterror;}


  SmartRefVector<EvtRecTrack>  tracks(){ return m_tracks;}
  SmartRefVector<EvtRecTrack>  showers(){ return m_showers;}
  SmartRefVector<EvtRecTrack>  otherTracks(){ return m_otherTracks;}
  SmartRefVector<EvtRecTrack>  otherShowers(){ return m_otherShowers;}
  SmartRefVector<EvtRecTrack>  pionId(){ return m_pionId;}
  SmartRefVector<EvtRecTrack>  kaonId(){ return m_kaonId;}
  
  
  void setdecayMode(DecayMode decayMode) {m_decayMode=decayMode;}
  void settype(SelType type) {m_type=type;}
  void setbeamE(double beamE) {m_beamE=beamE;}
  void setmass(double mass) {m_mass=mass;}
  void setmBC(double mBC) {m_mBC=mBC;}  
  void setdeltaE(double deltaE) {m_deltaE=deltaE;}
  void setcharge(int charge) {m_charge=charge;}
  void setcharm(int charm) {m_charm=charm;}
  void setnumOfChildren(int numOfChildren) {m_numOfChildren=numOfChildren;}
  void setp4(HepLorentzVector p4) {m_p4=p4;}

  void addToFitInfo(double ksmass, double chi2, double length, double error) {
    m_ksmass.push_back(ksmass);
    m_vfitchi2.push_back(chi2);
    m_vfitlength.push_back(length);
    m_vfiterror.push_back(error);
  }
  
  void addTrack(const SmartRef<EvtRecTrack> track) {
    m_tracks.push_back(track);}
  void addShower(const SmartRef<EvtRecTrack> shower) {
    m_showers.push_back(shower);}
  void addOtherTrack(const SmartRef<EvtRecTrack> track) {
    m_otherTracks.push_back(track);}
  void addOtherShower(const SmartRef<EvtRecTrack> shower){
    m_otherShowers.push_back(shower);}
  void addPionId(const SmartRef<EvtRecTrack> pionId){
    m_pionId.push_back(pionId);}
  void addKaonId(const SmartRef<EvtRecTrack> kaonId){
    m_kaonId.push_back(kaonId);}
  
  
  void setTracks(const SmartRefVector<EvtRecTrack> tracks) {
    m_tracks=tracks;}
  void setShowers(const SmartRefVector<EvtRecTrack> showers) {
    m_showers=showers;}
  void setOtherTracks(const SmartRefVector<EvtRecTrack> tracks) {
    m_otherTracks=tracks;}
  void setOtherShowers(const SmartRefVector<EvtRecTrack> showers){
    m_otherShowers=showers;}
  void setPionId(const SmartRefVector<EvtRecTrack> pionId){
    m_pionId=pionId;}
  void setKaonId(const SmartRefVector<EvtRecTrack> kaonId){
    m_kaonId=kaonId;}
  

 private:
  DecayMode m_decayMode;
  SelType  m_type;
  double  m_beamE;
  double  m_mass;
  double  m_mBC;
  double  m_deltaE;
  int     m_charge;
  int     m_charm;
  unsigned int     m_numOfChildren;
  HepLorentzVector m_p4;
  vector< double > m_ksmass;
  vector< double > m_vfitchi2;
  vector< double > m_vfitlength;
  vector< double > m_vfiterror;
  SmartRefVector<EvtRecTrack>  m_tracks;
  SmartRefVector<EvtRecTrack>  m_showers;
  SmartRefVector<EvtRecTrack>  m_otherTracks;
  SmartRefVector<EvtRecTrack>  m_otherShowers;
  SmartRefVector<EvtRecTrack>  m_pionId;
  SmartRefVector<EvtRecTrack>  m_kaonId;
  
};


typedef ObjectVector<EvtRecDTag> EvtRecDTagCol;
typedef EvtRecDTagCol::iterator EvtRecDTagIterator;
#endif
  
