// -*- C++ -*-
//
// Package:     MdcFastTrkAlg
// Module:      FTSuperLayer
// 
// Description:  super-layer class for MdcFastTrkAlg
//
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Bootstrap.h"

#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeoLayer.h"

#include "MdcFastTrkAlg/FTList.h"
#include "MdcFastTrkAlg/FTLayer.h"
#include "MdcFastTrkAlg/FTSuperLayer.h"
#include "MdcFastTrkAlg/FTSegment.h"
#include "MdcFastTrkAlg/FTWire.h"
#include "MdcFastTrkAlg/MdcParameter.h"
#include <iostream>

MdcParameter* FTSuperLayer::param = MdcParameter::instance();

//...Globals...
/*const unsigned int
FTSuperLayer::_neighborsMask[6] = {452,420,340,172,92,60};*/
//                          [0] = 452 = FTWireHitAppended + FTWireNeighbor345
//                          [1] = 420 = FTWireHitAppended + FTWireNeighbor245
//     definitions of       [2] = 340 = FTWireHitAppended + FTWireNeighbor135
//    _neighborsMask[]      [3] = 172 = FTWireHitAppended + FTWireNeighbor024
//                          [4] = 92  = FTWireHitAppended + FTWireNeighbor013
//                          [5] = 60  = FTWireHitAppended + FTWireNeighbor012
const float FTSuperLayer::_maxDphi[11] =
  {11.9, 9.72, 6.26, 4.86, 3.81, 2.37, 2.08, 1.76, 1.53, 2.33, 1.88};

void
FTSuperLayer::clear(void)
{
  if (_wireHits.length()){
    register FTWire ** hptr = _wireHits.firstPtr();
    FTWire ** const last = _wireHits.lastPtr();
    do{(**hptr).state(FTWireHitInvalid);}while((hptr++)!=last);
    _wireHits.clear();
  }
  _singleHits.clear();
  if (_segments.length()) _segments.deleteAll();
  if (_complecated_segments) _complecated_segments->deleteAll();
}


void
FTSuperLayer::mkSegmentList(void)
{
  clustering();
  FTList<FTSegment *> inner_short(10);
  FTList<FTSegment *> outer_short(10);
  FTList<FTSegment *> mid_short(10);
  int n = _segments.length();
  for (int i = 0; i^n; i++){
    FTSegment * s = _segments[i];
#ifndef OnlineMode    
    //    s->printout();
#endif
    int stat = s->examine();
    switch(stat){
    case 0:
      // simple
      break;
    case 1:
      // inner short simple
      inner_short.append(s);
      n = _segments.remove(i);
      break;
    case 2:
      // outer short simple
      outer_short.append(s);
      n = _segments.remove(i);
      break;
    case 3:
      // to be divided
      //if (!_superLayerId) _complecated_segments->append(s);
      //else delete s;
       _complecated_segments->append(s);    
       n = _segments.remove(i);
       break;
    default:
      mid_short.append(s);
      n = _segments.remove(i);
      break;
    }
  }
  connect_short_segments(inner_short,outer_short);
  connect_singleHit(inner_short);
  connect_singleHit(outer_short);
  connect_singleHit(mid_short);
  //if (!(_superLayerId&1)){  
  n = _segments.length();
  for (int i = 0; i^n; i++){
    _segments[i]->update();
#ifndef OnlineMode
    _segments[i]->printout();
#endif
  }
}

void 
FTSuperLayer::reduce_noise(FTList<float> (&Estime)[10])
{
  // if(_superLayerId<5 || _superLayerId>9 ) return;
  if( _superLayerId>9 )  return; // no need for the most outer SL
 
  int n = _segments.length();
  if(n>30) return;

  float tdc[30][4]={0.},r[4]={0.},phi[4]={0.};
  int wireId[4]={0};

  IMdcGeomSvc* mdcGeomSvc;
  StatusCode sc = Gaudi::svcLocator()->service("MdcGeomSvc", mdcGeomSvc);
  // float size = mdcGeomSvc->Layer(0)->PCSiz();
  float T1=0.5*0.1*(mdcGeomSvc->Layer(4*_superLayerId+0)->PCSiz())/0.004;
  float T2=0.5*0.1*(mdcGeomSvc->Layer(4*_superLayerId+1)->PCSiz())/0.004;
  float T3=0.5*0.1*(mdcGeomSvc->Layer(4*_superLayerId+2)->PCSiz())/0.004;
  float T4=0.5*0.1*(mdcGeomSvc->Layer(4*_superLayerId+3)->PCSiz())/0.004;
              
  for (int i = 0; i^n; i++){

    float chi2=-99;
    float t0c=0;

    //float in_layerId=_segments[i]->innerBoundHits().first()->layer().layerId();
    // float out_layerId=_segments[i]->outerBoundHits().first()->layer().layerId();
    FTList<FTWire *>& hits=_segments[i]->wireHits();
    // if(hits.length()>4) continue;

    for(int j=0;j<hits.length();j++){
      FTWire & h = *hits[j];
      
      int layerId=h.layer().layerId();
      
      /* if(wireId[layerId%4] == (h.localId()-1)||wireId[layerId%4]==(h.localId()+1)) {
	 tdc[i][layerId%4]=0;
	 break;
	 }*/
      
      wireId[layerId%4]=h.localId();
      phi[layerId%4]=h.phi();
      
      // if(tdc[i][layerId%4] != (h.time())) continue;
      
      tdc[i][layerId%4]=h.time();
      r[layerId%4]=h.layer().r();
    }
    //// 4 cell
    
    if(tdc[i][0]!=0 && tdc[i][1]!=0 && tdc[i][2]!=0 && tdc[i][3]!=0 && (wireId[0]==wireId[1]&& wireId[0]==wireId[2] && wireId[0]==wireId[3] || wireId[0]==wireId[1]-1 && wireId[0]==wireId[2]&& wireId[1]==wireId[3])){
      //  if(tdc[i][0]!=0 && tdc[i][1]!=0 && tdc[i][2]!=0 && tdc[i][3]!=0 &&( phi[0]<phi[1]&&phi[1]>phi[2]&&phi[2]<phi[3]||phi[0]>phi[1] && phi[1]<phi[2] && phi[2]>phi[3])){
      
     float  r0=r[0]-r[1]-(r[2]-r[1])/2;
     float  r1=-(r[2]-r[1])/2;
     float  r2=(r[2]-r[1])/2;
     float  r3=r[3]-r[2]+(r[2]-r[1])/2;

     float  t_i = tdc[i][0]+tdc[i][2];
     float  t_j = tdc[i][1]+tdc[i][3];
     float  l_j = T2+T4;
     float  r_i = r0+r2;
     float  r_j = r1+r3;
     float  r_2k= r0*r0+r1*r1+r2*r2+r3*r3;
     float  rt_i = r0*tdc[i][0]+r2*tdc[i][2];
     float  rt_j = r1*tdc[i][1]+r3*tdc[i][3];
     float  rl_j = r1*T2+r3*T4;

     float  deno= 4*r_2k-2*(r_i*r_i+r_j*r_j);
    
     if (deno!=0){
       float  Pa=(4*(rt_i-rt_j+rl_j)-(t_i+t_j-l_j)*(r_i-r_j)-(t_i-t_j+l_j)*(r_i+r_j))/deno;
       float  Pb= 0.25*(t_i-t_j+l_j-(r_i+r_j) * Pa);
       float  Ang=fabs(90.-fabs(atan(Pa)*180./3.141593));

       t0c = -0.25*((r_i-r_j)*Pa-t_i-t_j+l_j);

       float pa1=(tdc[i][0]-tdc[i][2])/(r0-r2);
       float pa2=(T4-T2-tdc[i][3]+tdc[i][1])/(r3-r1);
  
      //// cout<<"t0c1: "<<-(tdc[i][0]-r0 *pa1  - 0.25*(t_i-t_j+l_j-(r_i+r_j)*pa1))<<endl;
      //// cout<<"t0c2: "<<T2-tdc[i][1]-r1 *pa2 - 0.25*(t_i-t_j+l_j-(r_i+r_j)*pa2)<<endl;
      //// cout<<"t0c3: "<<-(tdc[i][2]-r2 *pa1  - 0.25*(t_i-t_j+l_j-(r_i+r_j)*pa1))<<endl;
      //// cout<<"t0c4: "<<T4-tdc[i][3]-r3 *pa2 - 0.25*(t_i-t_j+l_j-(r_i+r_j)*pa2)<<endl;
     
      //// cout<<"t0c1,2,3,4: "<<-(tdc[i][0]-r0 * Pa-Pb)<<","<<(T2-tdc[i][1]-r1 * Pa-Pb)<<","<<-(tdc[i][2]-r2 * Pa-Pb)<<","<<(T4-tdc[i][3]-r3 * Pa-Pb)<<endl;

      chi2=(tdc[i][0]-t0c-r0 * Pa-Pb)*(tdc[i][0]-t0c-r0 * Pa-Pb)+(T2-tdc[i][1]+t0c-r1 * Pa-Pb)*(T2-tdc[i][1]+t0c-r1 * Pa-Pb)+(tdc[i][2]-t0c-r2 * Pa-Pb)*(tdc[i][2]-t0c-r2 * Pa-Pb) + (T4-tdc[i][3]+t0c-r3 * Pa-Pb)*(T4-tdc[i][3]+t0c-r3 * Pa-Pb);

       }
   }
    ////3 cell
      
    /*   else if(tdc[i][0]!=0 && tdc[i][1]!=0 && tdc[i][2]!=0 && (wireId[0]==wireId[1]-1||wireId[0]==wireId[1]) &&  wireId[2]==wireId[0]) {
	  
    //// else  if(tdc[i][0]!=0 && tdc[i][1]!=0 && tdc[i][2]!=0 &&(phi[0]<phi[1] && phi[2]<phi[1] ||phi[0]>phi[1] && phi[2]>phi[1])){

    float pa=-((r[0]+r[2]-2*r[1])*(tdc[i][0]+tdc[i][2])-2*(r[0]-r[1])*tdc[i][0]-2*(r[2]-r[1])*tdc[i][2])/((r[0]-r[2])*(r[0]-r[2]));
     //float pa=(tdc[i][2]-tdc[i][0])/(r[2]-r[0]);
     float pb=0.25*(tdc[i][0]-2*tdc[i][1]+tdc[i][2]+2.*T2-(r[0]+r[2]-2*r[1])*pa);
     float Ang=fabs(90.-fabs(atan(pa)*180./3.141593));
    
     t0c=-((r[0]+r[2]-2*r[1])*pa + 3.*pb - tdc[i][0]+tdc[i][1]-tdc[i][2]-T2);
     //t0c=0.5*(tdc[i][1]+tdc[i][2]-(r[1]-r[2])*pa-T2);
     chi2=(tdc[i][0]-t0c-pa*(r[0]-r[1])-pb)*(tdc[i][0]-t0c-pa*(r[0]-r[1])-pb)+(T2-tdc[i][1]+t0c-pb)*(T2-tdc[i][1]+t0c-pb)+(tdc[i][2]-t0c-pa*(r[2]-r[1])-pb)*(tdc[i][2]-t0c-pa*(r[2]-r[1])-pb);
  }

     else if(tdc[i][1]!=0 && tdc[i][2]!=0 && tdc[i][3]!=0 && (wireId[1]==wireId[2]||wireId[1]==wireId[2]+1) && wireId[3]==wireId[1]){
     //// else if(tdc[i][1]!=0 && tdc[i][2]!=0 && tdc[i][3]!=0 &&(phi[1]<phi[2] && phi[3]<phi[2] ||phi[1]>phi[2] && phi[3]>phi[2])){

       float pa=-((r[1]+r[3]-2*r[2])*(tdc[i][1]+tdc[i][3])-2*(r[1]-r[2])*tdc[i][1]-2*(r[3]-r[2])*tdc[i][3])/((r[1]-r[3])*(r[1]-r[3]));
       float pb=0.25*(tdc[i][1]-2*tdc[i][2]+tdc[i][3]+2.*T3-(r[1]+r[3]-2*r[2])*pa);

      t0c=-((r[1]+r[3]-2*r[2])*pa + 3.*pb - tdc[i][1]+tdc[i][2]-tdc[i][3]-T3);
      chi2=(tdc[i][1]-t0c-pa*(r[1]-r[2])-pb)*(tdc[i][1]-t0c-pa*(r[1]-r[2])-pb)+(T3-tdc[i][2]+t0c-pb)*(T3-tdc[i][2]+t0c-pb)+(tdc[i][3]-t0c-pa*(r[3]-r[2])-pb)*(tdc[i][3]-t0c-pa*(r[3]-r[2])-pb);

   }
   */     
   
    //    g_segmentchi2[_superLayerId][i]=chi2;
    //    g_t0c[_superLayerId][i]=t0c;

    //    if(_superLayerId>4 && chi2>0){  
    //cout<<"superLayer,t0c,chi2: "<<_superLayerId<<"["<<i<<"] : "<<t0c<<" , "<<chi2<<endl; 
      //    }
    //   if(chi2<700 && _superLayerId>4 && chi2>-1){
    if(chi2<(param->_chi2_segfit) && _superLayerId>4 && chi2>-1){
       Estime[_superLayerId].append(t0c);
    }
    //  if(chi2>0 && chi2<2000) continue;
    // if (chi2<1000) continue;
    continue;

    _complecated_segments->append(_segments[i]);
    for(int k=0;k<4;k++){
      tdc[i][k]=0;
    }
    n = _segments.remove(i);
  }
}

//cluster hits to segments
void
FTSuperLayer::clustering(void)
{
  //                  +---+---+
  //                  | 4 | 5 |
  //                +-+-+---+-+-+    r
  //  Neighbor ID   | 2 | * | 3 |    ^
  //                +-+-+---+-+-+    |
  //                  | 0 | 1 |      +--> -phi
  //                  +---+---+
  //
  if (!_wireHits.length()) return;
  register FTWire ** hptr = _wireHits.firstPtr();
  FTWire ** const last = _wireHits.lastPtr(); 
  // discard continuous hits
  do{(**hptr).chk_left_and_right();
    }while((hptr++)!=last);

  // clustering
  hptr = _wireHits.firstPtr();
  //FTList<FTWire *> * hits = new FTList<FTWire *>(10); zhangxm:because of 345 link
  FTList<FTWire *> * hits = new FTList<FTWire *>(16);
  do{				// loop over clusters
    if ((**hptr).stateAND(FTWireHitAppendedOrInvalid)) continue;
    int n = hits->append(*hptr);
    (**hptr).stateOR(FTWireHitAppended);
    for (int j = 0; j^n; j++){	// loop over hits in a cluster
      const unsigned int checked = (*hits)[j]->state();
      //const unsigned int * mptr = _neighborsMask;
      FTWire** nptr = (*hits)[j]->neighborPtr();
      // check 6 neighbors
      for (unsigned Mask=FTWireNeighbor0; Mask^512; Mask<<=1, /*mptr++,*/ nptr++){
	if ((**nptr).stateAND(FTWireHitAppendedOrInvalid)) continue;
	n = hits->append(*nptr);
	(**nptr).stateOR(FTWireHitAppended);
      }
    }

    if (n^1){
      _segments.append(new FTSegment(this, *hits));
      //hits = new FTList<FTWire *>(10); because of 345 link
      hits = new FTList<FTWire *>(16);
    }else{
      _singleHits.append(*hptr);
      hits->clear();
    }
  }while((hptr++)!=last);
  delete hits;
}

//to match two phi-near short segments and connect them 
void
FTSuperLayer::connect_short_segments(FTList<FTSegment *> & inner_short,
				     FTList<FTSegment *> & outer_short)
{
  int n = inner_short.length();
  int m = outer_short.length();
  for (int i = 0; i^n; i++){
    FTSegment * inner = inner_short[i];
    const FTWire & in_outerBoundHit = * inner->outerBoundHits().first();
    const FTLayer & in_outerBound = in_outerBoundHit.layer();
    float in_outerPhi = inner->outgoingPhi();

    int   min_dphi_index = -1;
    float min_dphi = 2*M_PI;
    int   dLayer_cache = -1;
    float out_innerPhi, D_phi;
    for (int j = 0; j^m; j++) {
      const int out_innerLayerId
             = (outer_short[j]->innerBoundHits().first())->layer().localLayerId();
      int dLayer_cache_tmp = out_innerLayerId - in_outerBound.localLayerId();
      if (dLayer_cache_tmp < 1) continue;
      out_innerPhi = outer_short[j]->incomingPhi();
      D_phi= fabs(in_outerPhi - out_innerPhi);
      if (D_phi > M_PI) D_phi = 2*M_PI - D_phi;
      if (D_phi < min_dphi) {
	min_dphi = D_phi;
	min_dphi_index = j;
	dLayer_cache = dLayer_cache_tmp;
      }
    }
    if (min_dphi_index < 0) continue;

    switch (dLayer_cache) {
      case 1:
	if (min_dphi > _maxDphi[_superLayerId]*M_PI/180.) continue;
	break;
      case 2:
	if (min_dphi > _maxDphi[_superLayerId]*M_PI/120.) continue;
	break;
      case 3:
	if (min_dphi > _maxDphi[_superLayerId]*M_PI/80.) continue;
	break;
      default:
	continue;
    }

    FTSegment * outer = outer_short[min_dphi_index];
    n = inner_short.remove(i);
    m = outer_short.remove(min_dphi_index);
    inner->connect_outer(outer->wireHits(), outer->outerBoundHits());
    _segments.append(inner);
    delete outer;
  }
}


void
FTSuperLayer::connect_singleHit(FTList<FTSegment *> & short_sgmnts){
  int minLength = 0;
  if (superLayerId() == 2 || superLayerId() == 3 || superLayerId() == 4
     || superLayerId() == 9 || superLayerId() == 10 )
     minLength = 1;
  int n = short_sgmnts.length();
  int m = _singleHits.length();
  for (int i = 0; i^n; i++){
    FTSegment & s = * short_sgmnts[i];
    const FTWire & outerBoundHit = * s.outerBoundHits().first();
    const FTWire & innerBoundHit = * s.innerBoundHits().first();
    const FTLayer & outerBound = outerBoundHit.layer();
    const FTLayer & innerBound = innerBoundHit.layer();
    float outerPhi = s.outgoingPhi();
    float innerPhi = s.incomingPhi();

    int   min_dphi_index = -1;
    float min_dphi = 2*M_PI;
    int   dLayer_cache = -1;
    bool  inner_flag_cache;
    for (int j = 0; j^m; j++) {
      const FTWire * h = _singleHits[j];
      const int hLocalLayerId = h->layer().localLayerId();
      int dLayer_cache_tmp = -1;
      bool inner_flag_cache_tmp;
      float D_phi;

      if ((dLayer_cache_tmp=innerBound.localLayerId()-hLocalLayerId) > 0) {
        D_phi = fabs(innerPhi - h->phi());
	inner_flag_cache_tmp = true;
      } else if ((dLayer_cache_tmp=hLocalLayerId-outerBound.localLayerId()) > 0) {
	D_phi = fabs(outerPhi - h->phi());
	inner_flag_cache_tmp = false;
      } else {
	continue;
      }
      if (D_phi > M_PI) D_phi = 2*M_PI - D_phi;

      if (D_phi < min_dphi) {
	min_dphi = D_phi;
	min_dphi_index = j;
	dLayer_cache = dLayer_cache_tmp;
	inner_flag_cache = inner_flag_cache_tmp;
      }
    }

    int   not_append = 1;
    if (min_dphi_index >= 0) {
      switch (dLayer_cache) {
        case 1:
          if (min_dphi < _maxDphi[_superLayerId]*M_PI/180.) not_append = 0;
          break; 
        case 2:
          if (min_dphi < _maxDphi[_superLayerId]*M_PI/120.) not_append = 0;
          break;
        case 3:
          if (min_dphi < _maxDphi[_superLayerId]*M_PI/80.) not_append = 0;
          break;
        default:
	  break;
      }
    }

    if (!not_append) {
      if (inner_flag_cache) {
        s.connect_inner(_singleHits[min_dphi_index]);
      } else {
        s.connect_outer(_singleHits[min_dphi_index]);
      }
      _segments.append(&s);
      m = _singleHits.remove(min_dphi_index);
    } else {
      if (outerBound.localLayerId()-innerBound.localLayerId()>minLength){
	_segments.append(&s);
      }else{
	if (!_superLayerId) _complecated_segments->append(&s);
	else delete &s;
      }
    }
  }
}


void
FTSuperLayer::reAppendSalvage(void)
{
  int n = _segments.length();
  for (int i = 0; i^n; i++){
    if (_segments[i]->track()) continue;
    _complecated_segments->append(_segments[i]);
    n = _segments.remove(i);
  }
}


#ifdef FZISAN_DEBUG
void
FTSuperLayer::showBinary(unsigned src)
{
  unsigned mask = 512;
  while(mask >>=1){
    unsigned bit = (mask&src) ? 1 : 0;
  }
}
#endif
