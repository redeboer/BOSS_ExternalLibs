// -*- C++ -*-
//
// Package:     MdcFastTrkAlg
// Module:      FTTrack
// 
// Description:  track class for MdcFastTrkAlg
//
//

#include <stdlib.h>
#include "MdcFastTrkAlg/FTTrack.h"
#include "MdcFastTrkAlg/FTSegment.h"
#include "MdcFastTrkAlg/FTFinder.h"
#include "MdcFastTrkAlg/FTWire.h"
#include "MdcFastTrkAlg/MdcParameter.h"

#ifndef OnlineMode
#include "AIDA/IAxis.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram3D.h"
#include "AIDA/IHistogramFactory.h"

extern IHistogram1D*   g_sigmaxy;
extern IHistogram1D*   g_sigmaz;
extern IHistogram1D*   g_chi2xy;
extern IHistogram1D*   g_chi2sz;

#endif

//const HepPoint3D
//FTTrack::m_ftFinder->pivot(0,0,0);

MdcParameter* FTTrack::param = MdcParameter::instance();

int
FTTrack::r_phiFit(void)
{
  IMessageSvc *msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);

  MsgStream log(msgSvc, "FTFinder");

  // static const float alpha(333.564095);
  if (fabs(_kappa) > 1.2/param->_minPt) return 0;

  _la = new Lpav;
  int n = _axial_segments.length();
  for (int i = 0; i^n; i++){
    FTList<FTWire *> & hits = _axial_segments[i]->wireHits();
    int m = hits.length();
    for (int j = 0; j^m; j++){
      FTWire & h = * hits[j];
      if (h.stateAND(FTWireFittingInvalid)){
	hits.remove(j);
	continue;
      }
      //      log << MSG::DEBUG << "layer: "<< h.layer().layerId() << " phi: " << h.phi() << " df_distance: "<< h.distance() << endreq;
      double par = h.distance()/(0.25*h.layer().csize());
      _la->add_point((double)h.x(),(double)h.y(),exp(-par*par));
    }
  }

  double chi2 = _la->fit();
  HepVector a = _la->Hpar(m_ftFinder->pivot);
  log << MSG::DEBUG << " chi2/_la cut(1): " << chi2 << " / " << _la->nc() 
      <<" a1(" << param->_minDr << "): " << a(1) 
      <<" a3(" << 1.05/param->_minPt << "):" << a(3) <<endreq;
  if (chi2/_la->nc() > 1.) return 0;
  if (fabs(a(3))>(1.05/param->_minPt)) return 0; 
  if (fabs(a(1))>param->_minDr) return 0;
  _la->clear();
  log << MSG::DEBUG << " passed chi2/_la, a(3) and a(1) cut" <<endreq;
  int layer0 = 0;
  for (int i = 0; i^n; i++){
    FTList<FTWire *> & hits = _axial_segments[i]->wireHits();
    if (!_axial_segments[i]->superLayer().superLayerId()) layer0 = 1;
    int m = hits.length();
    for (int j = 0; j^m; j++){
      FTWire & h = * hits[j];
      const float x = h.x();
      const float y = h.y();
      double d0 = _la->d((double)x,(double)y);
      float delta = h.distance()/h.layer().r();
      if (fabs(d0) > 0.7*h.layer().csize()) continue; // remove bad hits
      if (d0>0){			// left or right
	d0 = -d0;
      }else{
	delta = -delta;
      }
      _la->add_point(x - delta*y, y + delta*x, 1);
    }
  }
  if (!layer0){		 // salvage hits from complecated segments in layer0
    FTList<FTSegment *> & salvage = 
      m_ftFinder->superLayer(0)->complecated_segments();
    HepVector center = _la->center();
    const float xc = center(1);
    const float yc = center(2);
    const float rc = a(1)+(-1. / 2.99792458 /m_pmgnIMF->getReferField())/a(3); // rho+drho(signed)
    int nn = salvage.length();
    for (int i = 0; i^nn; i++){
      int salvaged = 0;
      FTList<FTWire *> & hits = salvage[i]->wireHits();
      int m = hits.length();
      for (int j = 0; j^m; j++){
	FTWire & h = * hits[j];
	float x = h.x();
	float y = h.y();
	float r = h.layer().r();
	if ((y*xc-x*yc)/(r*rc)<0.707) break;
	double d0 = _la->d((double)x,(double)y);
	float delta = h.distance()/r;
	if (fabs(d0) > 0.7*h.layer().csize()) continue; // remove bad hits
	if (d0>0){			// left or right
	  h.stateOR(FTWireHitRight);
	  d0 = -d0;
	}else{
	  h.stateOR(FTWireHitLeft);
	  delta = -delta;
	}
	_la->add_point(x - delta*y, y + delta*x, 1);
	salvaged = 1;
      }
      if (salvaged){
	_axial_segments.append(salvage[i]);
	break;
      }
    }
  }
  chi2 = _la->fit();		// refit using drift distance
  _stereo_segments = new FTList<FTSegment *>(6);  
  _stereo_segments_cache = new FTList<FTSegment *>(3);
  _stereo_segments_by_superLayer = new FTList<FTList<FTSegment *> *>(6);
  _za = new zav;
  _SigmaS = 0.;
  _SigmaZ = 0.;
  _SigmaSS = 0.;
  _SigmaSZ = 0.;
  return 1;
}

int
FTTrack::r_phiReFit(float vx, float vy, int vtx_flag)
{
  if (vtx_flag) _la->fit(vx, vy, 20*_la->chisq()/_la->nc());
  int n = _axial_segments.length();
  _la->clear();
  for (int i = 0; i^n; i++){
    FTList<FTWire *> & hits = _axial_segments[i]->wireHits();
    int m = hits.length();
    for (int j = 0; j^m; j++){
      FTWire & h = * hits[j];
      const float x = h.x();
      const float y = h.y();
      double d0 = _la->d((double)x,(double)y);
      float cellsize = h.layer().csize();
      if (m_ftFinder->evtTiming){
	float time = h.time() + m_ftFinder->evtTiming;
	if(time<-100) continue; // discard the bad TDC time
	h.distance(m_ftFinder->t2x(h.layer(),time));
	//h.distance(time*40*0.0001);
      }
      float delta = h.distance()/h.layer().r();
      if (fabs(d0) > 0.5*cellsize) continue; // remove bad hits
      if (d0>0){			// left or right
	h.stateOR(FTWireHitRight);
	d0 = -d0;
      }else{
	h.stateOR(FTWireHitLeft);
	delta = -delta;
      }
      h.setChi2(h.distance()+d0);
#ifndef OnlineMode
      //fill histogram 
      g_sigmaxy->fill(h.distance()+d0, 1.0);
#endif
      _la->add_point(x - delta*y, y + delta*x, 1);
    }
  }
  HepVector b = _la->Hpar(m_ftFinder->pivot);
  double  chi2 = _la->fit();            // refit using drift distance
#ifndef OnlineMode
  g_chi2xy->fill(chi2/_la->nc(), 1.0);
#endif
	  
  return 1;
} 

int 
FTTrack::r_phi2Fit(float vx, float vy, int vtx_flag)
{
  if (vtx_flag) _la->fit(vx, vy, 20*_la->chisq()/_la->nc());
  int n = _axial_segments.length();
  _la->clear();
  int k=0,l=0;
  float temp=0;
  for (int i = 0; i^n; i++){
    FTList<FTWire *> & hits = _axial_segments[i]->wireHits();
     int m = hits.length();
     for (int j = 0; j^m; j++){
      k++;
      FTWire & h = * hits[j];
      const float x = h.x();
      const float y = h.y();
      double d0 = _la->d((double)x,(double)y);
      float cellsize = h.layer().csize();
      if (m_ftFinder->evtTiming){
	float time = h.time() + m_ftFinder->evtTiming ;
	if(time<-100) continue;
	h.distance(m_ftFinder->t2x(h.layer(),time));
	//	h.distance(time*40*0.0001);
      }
      float delta = h.distance()/h.layer().r();
      if (fabs(d0) > 0.5*cellsize) continue; // remove bad hits
      if (h.distance()<0.1 || h.distance()>0.6) continue;
      if (d0>0){			// left or right
	h.stateOR(FTWireHitRight);
	d0 = -d0;
      }else{
	h.stateOR(FTWireHitLeft);
	delta = -delta;
      }
#ifndef OnlineMode
      //fill histogram 
       //g_sigmaxy->fill(h.distance()+d0, 1.0);
#endif
      _la->add_point(x - delta*y, y + delta*x, 1);
      if(temp<fabs(h.distance()+d0)){
        temp=fabs(h.distance()+d0);
	l=k;
      }
    }
  }
  HepVector b = _la->Hpar(m_ftFinder->pivot);
  double  chi2 = _la->fit();            // refit using drift distance
#ifndef OnlineMode
  // g_chi2xy->fill(chi2/_la->nc(), 1.0);
#endif
  if(k>5){	  
    return  l;
  }
  else{
    return -99;
  }
}

int   
FTTrack::r_phi3Fit(int l, float vx, float vy, int vtx_flag)
{
  if (vtx_flag) _la->fit(vx, vy, 20*_la->chisq()/_la->nc());
  int n = _axial_segments.length();
  _la->clear();
  int k=0;
  for (int i = 0; i^n; i++){
    FTList<FTWire *> & hits = _axial_segments[i]->wireHits();
    int m = hits.length();
    for (int j = 0; j^m; j++){
      k++;
      FTWire & h = * hits[j];
      const float x = h.x();
      const float y = h.y();
      double d0 = _la->d((double)x,(double)y);
      float cellsize = h.layer().csize();
      if (m_ftFinder->evtTiming){
	float time = h.time() + m_ftFinder->evtTiming ;
	if(time<-100) continue;
	h.distance(m_ftFinder->t2x(h.layer(),time));
	//h.distance(time*40*0.0001);
      }
      float delta = h.distance()/h.layer().r();
      if (fabs(d0) > 0.5*cellsize) continue; // remove bad hits
      if (d0>0){			// left or right
	h.stateOR(FTWireHitRight);
	d0 = -d0;
      }else{
	h.stateOR(FTWireHitLeft);
	delta = -delta;
      }
      if (k==l){
	//delete hits[j];
	m=hits.remove(j);
	continue;
      }
      _la->add_point(x - delta*y, y + delta*x, 1);
      // h.setChi2(h.distance()+d0);
    }
  }
  HepVector b = _la->Hpar(m_ftFinder->pivot);
  double chi2 = _la->fit();            // refit using drift distance
  return 1;
}

int  
FTTrack::r_phi4Fit(float vx, float vy, int vtx_flag)
{
  if (vtx_flag) _la->fit(vx, vy, 20*_la->chisq()/_la->nc());
  int n = _axial_segments.length();
  _la->clear();
  for (int i = 0; i^n; i++){
    FTList<FTWire *> & hits = _axial_segments[i]->wireHits();
    int m = hits.length();
    for (int j = 0; j^m; j++){
      FTWire & h = * hits[j];
      const float x = h.x();
      const float y = h.y();
      double d0 = _la->d((double)x,(double)y);
      float cellsize = h.layer().csize();
      if (m_ftFinder->evtTiming){
	float time = h.time() + m_ftFinder->evtTiming ;
	if(time<-100) continue;
	h.distance(m_ftFinder->t2x(h.layer(),time));
	//h.distance(time*40*0.0001);
      }
      float delta = h.distance()/h.layer().r();
      if (fabs(d0) > 0.5*cellsize) continue; // remove bad hits
      if (d0>0){			// left or right
	h.stateOR(FTWireHitRight);
	d0 = -d0;
      }else{
	h.stateOR(FTWireHitLeft);
	delta = -delta;
      }
    
      _la->add_point(x - delta*y, y + delta*x, 1);
      h.setChi2(h.distance()+d0);
#ifndef OnlineMode
      //fill histogram 
      // g_sigmaxy->fill(h.distance()+d0, 1.0);
#endif
    }
  }
  HepVector b = _la->Hpar(m_ftFinder->pivot);
  double chi2 = _la->fit();            // refit using drift distance
#ifndef OnlineMode
 // g_chi2xy->fill(chi2/_la->nc(), 1.0);
#endif
  return 1;
}

int
FTTrack::linkStereoSegments(void){
  delete _stereo_segments_cache;
  int n = _stereo_segments_by_superLayer->length();
  for (int i = 0; i^n; i++){
    FTList<FTSegment *> & segments = *(*_stereo_segments_by_superLayer)[i];
    int m = segments.length();
    float min_D_z = 9998.;
    float S = 0.;
    float Z = 0.;
    FTSegment * selected = NULL;
    for (int j = 0; j^m; j++){
      FTSegment * s = segments[j];
      float s_tmp = s->s();
      float z_tmp = s->z();
      double D_z = fabs(d_z(s_tmp,z_tmp));
      if (D_z < min_D_z){
	selected = s;
	min_D_z = D_z;
	S = s_tmp;
	Z = z_tmp;
      }
    }
    if (selected){
      _stereo_segments->append(selected);
      _SigmaS += S;
      _SigmaZ += Z;
      _SigmaSZ += S*Z;
      _SigmaSS += S*S;
    }
  }
  _stereo_segments_by_superLayer->deleteAll();
  delete _stereo_segments_by_superLayer;
  return 0;
}

int
FTTrack::s_zFit(void){
  IMessageSvc *msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);

  MsgStream log(msgSvc, "FTFinder");

  HepVector a = _la->Hpar(m_ftFinder->pivot);
  int n = _stereo_segments->length();
  log<<MSG::DEBUG<<"number of stereo segments: "<< n << endreq;
  if (n < (param->_nseg)){
    a(4) = -9999.;
    a(5) = -9999.;
    _helix = new Helix(m_ftFinder->pivot,a);
    log<<MSG::DEBUG<<"cut by _nseg" << param->_nseg << endreq;
    return 0;
  }
  FTList<double> zList(50);
  FTList<double> sList(50);
  FTList<FTWire *> hList(50);
  for (int i = 0; i^n; i++){
    FTList<FTWire *> & hits = (*_stereo_segments)[i]->wireHits();
    int m = hits.length();
    for (int j = 0; j^m; j++){
      FTWire & h = * hits[j];
      double z;
      if (!(h.z(*_la,z))) continue;
      double s = _la->s(h.layer().r());
      float cellsize = h.layer().csize();
      log<<MSG::DEBUG<<"cellsize: " << cellsize << endreq;

      if (m_ftFinder->evtTiming){
 	float time = h.time() + m_ftFinder->evtTiming;
	if(time<-100) continue; //discard the bad TDC time
	double distance = m_ftFinder->t2x(h.layer(),time);
	h.distance(distance);
 	log<<MSG::DEBUG<<"m_ftFinder->evtTiming: "<< m_ftFinder->evtTiming << " TDC time: " << h.time() << " distance: "<< distance << endreq;
      }

      double par = h.distance()/(0.25*cellsize);
      _za->add(s,z,exp(-par*par));
      sList.append(s);
      zList.append(z);
      hList.append(&h);
    }
  }
  if (hList.length() < (param->_nlength)){
    a(4) = -9999.;
    a(5) = -9999.;
    _helix = new Helix(m_ftFinder->pivot,a);
    log << MSG::DEBUG << "cut by _nlength: " << param->_nlength << endreq;
    return 0;
  }   
  double chi2 = _za->calculate();
  _za->clear();
  n = hList.length();
  for (int i = 0; i^n; i++){
    double d = _za->d(sList[i],zList[i]);
    float z_distance = hList[i]->distance_z();
    if (fabs(fabs(d)-z_distance) > (param->_z_cut1)){ 
      log<<MSG::DEBUG<<"cut by _z_cut1: "<< param->_z_cut1 << endreq;
      zList.remove2(i);
      sList.remove2(i);
      n = hList.remove(i);
      continue;
    }
    _za->add(sList[i], (d>0) ? zList[i]-z_distance : zList[i]+z_distance, 1.);
  }

  if (_za->nc() < (param->_nc)){
    a(4) = -9999.;
    a(5) = -9999.;
    _helix = new Helix(m_ftFinder->pivot,a);
    log<<MSG::DEBUG<<"cut by _nc: " << param->_nc << endreq;
    return 0;
  }    
  chi2 = _za->calculate();
  _za->clear();
  for (int i = 0; i^n; i++){
    double d = _za->d(sList[i],zList[i]);
    float z_distance = hList[i]->distance_z();
    hList[i]->setChi2(fabs(d)-z_distance);
 #ifndef OnlineMode
    //fill ntuple
    g_sigmaz->fill(fabs(d)-z_distance, 1.0);
#endif
    if (fabs(fabs(d)-z_distance) > (param->_z_cut2)) continue; 
    _za->add(sList[i], (d>0) ? zList[i]-z_distance : zList[i]+z_distance, 1.);
  }

  if (_za->nc() < (param->_nc)){
    a(4) = -9999.;
    a(5) = -9999.;
    _helix = new Helix(m_ftFinder->pivot,a);
    log<<MSG::DEBUG<<"cut by _nc" << param->_nc << endreq;
    return 0;
  }    
  chi2 = _za->calculate();
#ifndef OnlineMode
  g_chi2sz->fill(chi2/_za->nc(), 1.0);
#endif
  a(4) = _za->b();		// dz
  a(5) = _za->a();		// tanLambda
  _helix = new Helix(m_ftFinder->pivot,a);
  return 1;
}

int FTTrack::get_nhits(void)
{
  int nhits = 0;
  int n = _axial_segments.length();
  for (int i = 0; i^n; i++) {
    nhits += _axial_segments[i]->wireHits().length();
  }
  n = _stereo_segments->length();
  for (int j = 0; j^n; j++) {
    nhits += (*_stereo_segments)[j]->wireHits().length();
  }
  return nhits;
}

#ifndef OnlineMode
void FTTrack::printout(){
  int n = axial_segments().length();
  for (int i = 0; i^n; i++) axial_segments()[i]->printout();
  n = stereo_segments().length();
  for (int i = 0; i^n; i++) stereo_segments()[i]->printout();  
}
#endif
