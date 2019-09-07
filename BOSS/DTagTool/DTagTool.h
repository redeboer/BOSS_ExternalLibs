
#ifndef CLASS_DTAGTOOL_H
#define CLASS_DTAGTOOL_H

#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EvtRecEvent/EvtRecDTag.h"
#include "EvtRecEvent/EvtRecVeeVertex.h"
#include "EvtRecEvent/EvtRecPi0.h"
#include "EvtRecEvent/EvtRecEtaToGG.h"

#include "SimplePIDSvc/ISimplePIDSvc.h"


#include <iostream>

using namespace std;



class DTagTool{

 public:
  
  DTagTool  ();

  ~DTagTool ();

  //DTag operations
  bool isDTagListEmpty() { return m_isdtaglistempty; }
  void setPID(bool pid) {m_pid = pid;}


  
  EvtRecDTagCol::iterator modes_begin(){ return m_iterbegin;}
  EvtRecDTagCol::iterator modes_end(){ return m_iterend;}
  EvtRecPi0Col::iterator pi0_begin(){ return m_pi0iterbegin;}
  EvtRecPi0Col::iterator pi0_end(){ return m_pi0iterend;}
  EvtRecEtaToGGCol::iterator eta_begin(){ return m_etaiterbegin;}
  EvtRecEtaToGGCol::iterator eta_end(){ return m_etaiterend;}
  EvtRecVeeVertexCol::iterator ks_begin(){ return m_ksiterbegin;}
  EvtRecVeeVertexCol::iterator ks_end(){ return m_ksiterend;}

 
  vector<int>  D0modes() { return m_d0modes;}
  vector<int>  Dpmodes() { return m_dpmodes;}
  vector<int>  Dsmodes() { return m_dsmodes;}

  vector<int> mode(EvtRecDTag::DecayMode decaymode);
  vector<int> mode(int decaymode);


  bool findSTag(EvtRecDTag::DecayMode mode, int tagcharm);
  bool findSTag(EvtRecDTag::DecayMode mode);
  bool findDTag(EvtRecDTag::DecayMode mode1, EvtRecDTag::DecayMode mode2,string smass="mbc");
  bool findDTag(EvtRecDTag::DecayMode mode1, int tagcharm1,EvtRecDTag::DecayMode mode2, int tagcharm2,string smass="mbc" );
  bool findADTag(EvtRecDTag::DecayMode mode1, EvtRecDTag::DecayMode mode2);
  bool findADTag(EvtRecDTag::DecayMode mode1, int tagcharm1,EvtRecDTag::DecayMode mode2, int tagcharm2 );

  bool findSTag(int mode, int tagcharm);
  bool findSTag(int mode);
  bool findDTag(int mode1, int mode2, string smass="mbc");
  bool findDTag(int mode1, int tagcharm1,int mode2, int tagcharm2,string smass="mbc" );
  bool findADTag(int mode1, int mode2);
  bool findADTag(int mode1, int tagcharm1,int mode2, int tagcharm2);


  
  EvtRecDTagCol::iterator stag(){ return m_iterstag;}
  EvtRecDTagCol::iterator dtag1(){ return m_iterdtag1;}
  EvtRecDTagCol::iterator dtag2(){ return m_iterdtag2;}
  vector<EvtRecDTagCol::iterator> vdtag1(){ return m_viterdtag1;}
  vector<EvtRecDTagCol::iterator> vdtag2(){ return m_viterdtag2;}
  void setdtagDE(double sigma1,double sigma2){ m_tag1desigma=sigma1;m_tag2desigma=sigma2;}
  bool compare(EvtRecDTagCol::iterator pair1_iter1,EvtRecDTagCol::iterator pair1_iter2,EvtRecDTagCol::iterator pair2_iter1,EvtRecDTagCol::iterator pair2_iter2, double mD, string smass);

  void operator<< ( EvtRecDTagCol::iterator iter);


  //other utilities
  HepLorentzVector pi0p4(EvtRecPi0Col::iterator pi0Itr, bool isconstrain=true);
  HepLorentzVector etap4(EvtRecEtaToGGCol::iterator etaItr, bool isconstrain=true);
  vector<int> pi0Id(EvtRecDTagCol::iterator iter, int numpi0=1);
  vector<int> etaId(EvtRecDTagCol::iterator iter, int numeta=1);
  vector<int> ksId(EvtRecDTagCol::iterator iter, int numks=1);
  HepLorentzVector p4shower(RecEmcShower* shower);
  HepLorentzVector p4(RecMdcKalTrack* mdcKalTrack, int pid);
  bool isPion( EvtRecTrack* trk);
  bool isKaon( EvtRecTrack* trk);
  bool isElectron( EvtRecTrack* trk);
  bool isMuon( EvtRecTrack* trk);
  bool isGoodTrack( EvtRecTrack* trk);
  bool isGoodShower( EvtRecTrack* trk);
  bool cosmicandleptonVeto(bool emc=true);
  int  numofShowerAboveThreshold();
  int  numofShowerAboveThreshold(int threshold);
  bool shareTracks(EvtRecDTagCol::iterator iter1, EvtRecDTagCol::iterator iter2);
  double angleShowerwithTrack(EvtRecTrack *shower, EvtRecTrack *track);

  void clear();
  void name(){ cout<<" I am the dtag tool"<<endl;}
  
 protected:
  
  IDataProviderSvc* eventSvc();
  
  
 private:
  
  bool m_isdtaglistempty;

  IDataProviderSvc* m_evtSvc;
  ISimplePIDSvc*    m_simplePIDSvc;



  EvtRecDTagCol::iterator m_iterbegin;
  EvtRecDTagCol::iterator m_iterend;
  EvtRecPi0Col::iterator m_pi0iterbegin;
  EvtRecPi0Col::iterator m_pi0iterend;
  EvtRecEtaToGGCol::iterator m_etaiterbegin;
  EvtRecEtaToGGCol::iterator m_etaiterend;
  EvtRecVeeVertexCol::iterator m_ksiterbegin;
  EvtRecVeeVertexCol::iterator m_ksiterend;
  

  bool m_pid;

  EvtRecDTagCol::iterator m_iterstag;
  EvtRecDTagCol::iterator m_iterdtag1;
  EvtRecDTagCol::iterator m_iterdtag2;
  vector<EvtRecDTagCol::iterator> m_viterdtag1;
  vector<EvtRecDTagCol::iterator> m_viterdtag2;
  double m_tag1desigma;
  double m_tag2desigma;

  vector<int> m_d0modes;
  vector<int> m_dpmodes;
  vector<int> m_dsmodes;

  EvtRecTrackIterator m_chargebegin;
  EvtRecTrackIterator m_chargeend;
  EvtRecTrackIterator m_showerbegin;
  EvtRecTrackIterator m_showerend;

  
};

typedef EvtRecDTagCol::iterator DTagToolIterator;
typedef EvtRecPi0Col::iterator pi0Iterator;
typedef EvtRecEtaToGGCol::iterator etaIterator;
typedef EvtRecVeeVertexCol::iterator ksIterator;



#endif
