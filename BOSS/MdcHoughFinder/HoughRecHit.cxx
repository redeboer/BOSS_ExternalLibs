#include "MdcHoughFinder/HoughRecHit.h"
//HoughRecHit::HoughRecHit():HoughHit(digi){
//}
//HoughRecHit::HoughRecHit(const MdcDigi*& digi):HoughHit(digi){
////  _flagLS=true;
//}
//HoughRecHit::HoughRecHit(const MdcDigi*& digi,double disToTrack):HoughHit(digi),_disToTrack(disToTrack) {
// // _flagLS=true;
//}
HoughRecHit::HoughRecHit():HoughHit(){
  _p_hough2D=NULL;
}

HoughRecHit& HoughRecHit::operator=(const HoughRecHit& other) {
  HoughHit::operator=(other);
  if(this==&other) return *this;
  _p_hough2D=other._p_hough2D; 
  _disToTrack=(other._disToTrack);
  _disToCir=(other._disToCir);
  _theta_l=(other._theta_l);
  _ambig=(other._ambig);
  _zPos=(other._zPos); _sPos=(other._sPos);
  _zAmb[0]=(other._zAmb[0]); _zAmb[1]=(other._zAmb[1]);
  _sAmb[0]=(other._sAmb[0]); _sAmb[1]=(other._sAmb[1]);
  _flagLS=(other._flagLS);
  _n_sol=(other._n_sol);
 // cout<<"in rechit ambig truth copy ("<<this->getLayerId()<<","<<this->getWireId()<<") " <<this->getLrTruth()<<endl;
  return *this;
}

HoughRecHit::HoughRecHit(const HoughRecHit& other):HoughHit( other) {
  _p_hough2D=other._p_hough2D; 
  _disToTrack=(other._disToTrack);
  _disToCir=(other._disToCir);
  _theta_l=(other._theta_l);
  _ambig=(other._ambig);
  _zPos=(other._zPos); _sPos=(other._sPos);
  _zAmb[0]=(other._zAmb[0]); _zAmb[1]=(other._zAmb[1]);
  _sAmb[0]=(other._sAmb[0]); _sAmb[1]=(other._sAmb[1]);
  _flagLS=(other._flagLS);
  _n_sol=(other._n_sol);
//  cout<<"in rechit ambig truth = ("<<this->getLayerId()<<","<<this->getWireId()<<") " <<this->getLrTruth()<<endl;
}

HoughRecHit::HoughRecHit(const MdcDigi* const digi,double disToTrack,double disToCir,int flagLS):HoughHit(digi),_disToTrack(disToTrack),_disToCir(disToCir),_flagLS(flagLS) {
  _p_hough2D=NULL;
}

HoughRecHit::HoughRecHit(const HoughHit& houghhit,double disToTrack,double disToCir,int flagLS):HoughHit(houghhit),_disToTrack(disToTrack),_disToCir(disToCir),_flagLS(flagLS) {
  //this->_cirlist= houghhit.getCirList();
  //this->_style= houghhit.getStyle();
  _p_hough2D=NULL;
  _ambig=-999;
  _zPos=-999;
  _sPos=-999;
}
void HoughRecHit::setAmb(int i){
  _ambig=i;
  if( 0 == i) {
	_zPos=_zAmb[0];
	_sPos=_sAmb[0];
  }
  if( 1 == i) {
	_zPos=_zAmb[1];
	_sPos=_sAmb[1];
  }
  if( -999 == i) {
	_zPos=-999;
	_sPos=-999;
  }
}
