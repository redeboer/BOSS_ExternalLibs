// -*- C++ -*-
//
// Package:     MdcFastTrkAlg
// Module:      FTSegment
// 
// Description:  segment class for MdcFastTrkAlg
//

#include <math.h>
#include "MdcFastTrkAlg/FTSegment.h"
#include "MdcFastTrkAlg/FTTrack.h"
#include "MdcFastTrkAlg/FTWire.h"
#include "MdcFastTrkAlg/FTSuperLayer.h"
#include "MdcFastTrkAlg/MdcFastTrkAlg.h"
#include "MdcFastTrkAlg/MdcParameter.h"
MdcParameter* FTSegment::param = MdcParameter::instance();


int
FTSegment::examine(){
  int stat = -1;
  FTList<FTWire *> wireHitsByLocalLayer[4] = {4,4,4,4};

  register FTWire ** hptr = _wireHits.firstPtr();
  FTWire ** const last = _wireHits.lastPtr();
  do{
    wireHitsByLocalLayer[(**hptr).layer().localLayerId()].append(*hptr);
  }while((hptr++)!=last);
  float length_phi = 0.;
  int innerBoundId = 0;
  int outerBoundId = 0;
  int AlreadyFound = 0;
  //int LocalLayerLength; 
  //int HalfLocalLayerLength;
  const int nLayer = _superLayer.nLayer();
  // to look for the innerBound and outBand of the hits
  for (int i = 0; i^nLayer; i++){
    int N_wireHits = wireHitsByLocalLayer[i].length();
    if(N_wireHits){
      float localPhi_max = 0.;
      float localPhi_min = 7.;
      for (int j = 0; j^N_wireHits; j++){
	float localPhi = wireHitsByLocalLayer[i][j]->phi();
        //LocalLayerLength = wireHitsByLocalLayer[i][j]->layer().NWire();
        //HalfLocalLayerLength = LocalLayerLength >> 1;     
	if (localPhi > localPhi_max) localPhi_max = localPhi;
	if (localPhi < localPhi_min) localPhi_min = localPhi;
      }
      float local_length_phi = localPhi_max - localPhi_min;
      if (local_length_phi > M_PI)
	local_length_phi = 2*M_PI - local_length_phi;
      if ( local_length_phi > length_phi ) length_phi = local_length_phi;
      outerBoundId = i;
      AlreadyFound = 1;
    } else if(!AlreadyFound){
      innerBoundId++;
    } else {
      break;
    }
  }
  //length_phi needed to decide more carefully 
  //  if (length_phi>0.09){
  //  cout<<length_phi<<" : "<<FTSuperLayer::_maxDphi[_superLayer.superLayerId()]*2*M_PI/180<<endl;
  if (length_phi>FTSuperLayer::_maxDphi[_superLayer.superLayerId()]*2*M_PI/180){ //by X.-R. Lu
    stat = 3;			// to be divided
  }else{
    if (innerBoundId > 0){
      stat = 2;			// outer short
    }else{
      if (outerBoundId < nLayer - 1){
	stat = 1;		// inner short
      }else{
	stat = 0;		// long simple
      }
    }
  }
  _innerBoundHits.append(wireHitsByLocalLayer[innerBoundId]);
  _outerBoundHits.append(wireHitsByLocalLayer[outerBoundId]);
  return stat;
}

void
FTSegment::update(void){
  //static const float alpha(333.564095);
  //float N_kappa;
  int n = _innerBoundHits.length();
  int m = _outerBoundHits.length();

  if (n^1) {
    _incoming_x = 0.5*(_innerBoundHits[0]->x()+_innerBoundHits[1]->x());
    _incoming_y = 0.5*(_innerBoundHits[0]->y()+_innerBoundHits[1]->y());
  } else {
    _incoming_x = _innerBoundHits[0]->x();
    _incoming_y = _innerBoundHits[0]->y();
  }
  if (m^1) {
    _outgoing_x = 0.5*(_outerBoundHits[0]->x()+_outerBoundHits[1]->x());
    _outgoing_y = 0.5*(_outerBoundHits[0]->y()+_outerBoundHits[1]->y());
  } else {
    _outgoing_x = _outerBoundHits[0]->x();
    _outgoing_y = _outerBoundHits[0]->y();
  }
  float in_r = _innerBoundHits.first()->layer().r();
  float out_r = _outerBoundHits.first()->layer().r();
  float sbX = _incoming_x - _outgoing_x;
  float sbY = _incoming_y - _outgoing_y;
  // kappa = -2. * alpha * ((Vin X Vout)_z / |Vin|*|Vout|) / |Vin-Vout|
  _kappa = 2.*(-1. / 2.99792458 /m_pmgnIMF->getReferField())*(_outgoing_x*_incoming_y - _outgoing_y*_incoming_x) /
    (in_r*out_r*sqrt(sbX*sbX+sbY*sbY));
  _r = 0.5*(in_r + out_r);
}


int
FTSegment::update3D(const FTTrack * track){
  IMessageSvc *msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);
  
  MsgStream log(msgSvc, "FTFinder");
  
  /*if (_superLayer.superLayerId() > 
    track->axial_segments().first()->superLayer().superLayerId() + 4) return 0;*/
  if (!_sList){
    int n = _innerBoundHits.length();
    int m = _outerBoundHits.length();

    _incoming_x = (n^1) 
      ? 0.5*(_innerBoundHits[0]->x()+_innerBoundHits[1]->x()) 
      : _innerBoundHits[0]->x();
    _incoming_y = (n^1) 
      ? 0.5*(_innerBoundHits[0]->y()+_innerBoundHits[1]->y()) 
      : _innerBoundHits[0]->y();
    _outgoing_x = (m^1) 
      ? 0.5*(_outerBoundHits[0]->x()+_outerBoundHits[1]->x()) 
      : _outerBoundHits[0]->x();
    _outgoing_y = (m^1) 
      ? 0.5*(_outerBoundHits[0]->y()+_outerBoundHits[1]->y()) 
      : _outerBoundHits[0]->y();
    _sList = new FTList<float>(5);
    _zList = new FTList<float>(5);
    _trackList = new FTList<FTTrack *>(5);
  }
  const Lpav & la = track->lpav();
  double d;
  double inS;
  const FTLayer * bound = &_innerBoundHits.first()->layer();
  log<<MSG::DEBUG<<"started inS calculation with layer limit "<< (double)bound->limit()<<endreq;
  if (!(la.sd((double)bound->r(),(double)_incoming_x,
	      (double)_incoming_y,(double)bound->limit(),  // multiplied with an factor due to bad rphi fit
	      inS,d))) return 0;
  double inZ = bound->z(d);
  double outS; 
  bound = &_outerBoundHits.first()->layer();
  log<<MSG::DEBUG<<"started outS calculation with layer limit "<< (double)bound->limit()<<endreq;
  if (!(la.sd((double)bound->r(),(double)_outgoing_x,
	      (double)_outgoing_y,(double)bound->limit(),  // multiplied with an factor due to bad rphi fit
	      outS,d))) return 0;
  double outZ = bound->z(d);
  _sList->append(inS+outS);
  _zList->append(inZ+outZ);
  _trackList->append((FTTrack *)track);
  log<<MSG::DEBUG<<"-----> coupled! DONE! "<<endreq;
  return 1;
}

void
FTSegment::linkStereoSegments(void){
  float min_D_z = 9999.;
  float S = 0.;
  float Z = 0.;
  FTTrack * selected = NULL;
  int m = _trackList->length();
  for (int j = 0; j^m; j++){
    FTTrack * t = (*_trackList)[j];
    float z_tmp = (*_zList)[j];
    float s_tmp = (*_sList)[j];
    float D_z = fabs(t->d_z(s_tmp,z_tmp));
    if (D_z < min_D_z){
      selected = t;
      min_D_z = D_z;
      S = s_tmp;
      Z = z_tmp;
    }
  }
  if (selected){
    selected->append_stereo(this,S,Z); 
    
  }
}

#ifndef OnlineMode
void 
FTSegment::printout(void){
  
  IMessageSvc *msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);
  
  MsgStream log(msgSvc, "FTSegment");
  log << MSG::DEBUG << "pintout of one segment:" << endreq;  
  int n=_wireHits.length();
  for(int i=0; i^n; i++){
    FTWire &h = *_wireHits[i];
     log << MSG::DEBUG << "hit: layer " << h.layer().layerId() << " phi "<< h.phi() << endreq;
  }
}
#endif
