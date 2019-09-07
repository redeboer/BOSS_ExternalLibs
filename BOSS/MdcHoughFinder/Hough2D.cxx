#include "MdcHoughFinder/Hough2D.h"

int Hough2D::m_debug=0;
TrkContextEv* Hough2D::_context=NULL;
const MdcCalibFunSvc* Hough2D::_mdcCalibFunSvc=NULL;
int Hough2D::m_qualityFactor=99999;
double Hough2D::m_dropTrkDrCut=1;
//double Hough2D::m_dropTrkDrCut=999;      //no limit of dr
double Hough2D::m_dropTrkDzCut=10;
double Hough2D::m_dropTrkNhitCut=5;
double Hough2D::m_dropTrkChi2Cut=99999.;
double Hough2D::m_dropTrkChi2NdfCut=99999.;

Hough2D::Hough2D(){
  //_m_gm = MdcDetector::instance(0);
  //_m_gm = MdcDetector::instance(0);
}

//Hough2D::~Hough2D(){
//  for(int i =0;i<_MdcHitCol.size();i++){
//	cout<<" destruct " <<i<<endl;
//	delete _MdcHitCol[i];
//  }
//}

Hough2D::Hough2D(const Hough2D& other){
  _circleR=other._circleR;
  _circleX=other._circleX;
  _circleY=other._circleY;
  _d0=other._d0;
  _phi0=other._phi0;
  _omega=other._omega;
  _charge=other._charge;
  _pt=other._pt;
  _nfit=other._nfit;
  _chi2_aver2D=other._chi2_aver2D;
  _bunchT0=other._bunchT0;
  //_m_gm = other._m_gm;
//  _recHitVec=other._recHitVec;
  _recHitVec=other._recHitVec;

//  for(int i =0;i<other._MdcHitCol.size();i++){
//	MdcHit* p_hit= new MdcHit( *(other._MdcHitCol[i]));
//	_MdcHitCol.push_back(p_hit);
//  }
}

Hough2D::Hough2D(recHitCol hitCol, double bunchtime):_recHitVec(hitCol),_bunchT0(bunchtime){
  //_m_gm = MdcDetector::instance(0);
}

//void Hough2D::print(){
//  cout<<"Print Hough2D "<<endl;
//  cout<<"d0: "<<_d0<<endl;
//  cout<<"phi0: "<<_phi0<<endl;
//  cout<<"omega: "<<_omega<<endl;
//  cout<<"charge: "<<_charge<<endl;
//
//  int size=_recHitVec.size();
//  cout<<"Hit Candi size: "<<size<<endl;
//  for(int i=0;i<size;i++){
//	int layer= _recHitVec[i].getLayerId();
//	int wire = _recHitVec[i].getWireId();
//	std::cout<<"("<<layer<<","<<wire<<") "<<std::endl;
//  }
//}

int Hough2D::fitLeast(){
  double x_sum=0;
  double y_sum=0;
  double x2_sum=0;
  double y2_sum=0;
  double x3_sum=0;
  double y3_sum=0;
  double x2y_sum=0;
  double xy2_sum=0;
  double xy_sum=0;
  double a1=0;
  double a2=0;
  double b1=0;
  double b2=0;
  double c1=0;
  double c2=0;
  //  double x_aver,y_aver,r2;
  int hitCandiSize=_recHitVec.size();
  if(hitCandiSize>=3){
	for(int i=0;i<hitCandiSize;i++){
	  //	if( _recHitVec[i].getflag()!=0 ) continue;
	  //cout<<"size of hit in least2d "<<hitCandiSize<<endl;
	  //	if(_recHitVec[i]->getSlayerType() !=0 ) continue;
	  x_sum=x_sum+( _recHitVec[i].getMidX() );
	  y_sum=y_sum+( _recHitVec[i].getMidY() );
	  x2_sum=x2_sum+( _recHitVec[i].getMidX() )*( _recHitVec[i].getMidX() );
	  y2_sum=y2_sum+( _recHitVec[i].getMidY() )*( _recHitVec[i].getMidY() );
	  x3_sum=x3_sum+( _recHitVec[i].getMidX() )*( _recHitVec[i].getMidX() )*( _recHitVec[i].getMidX() );
	  y3_sum=y3_sum+( _recHitVec[i].getMidY() )*( _recHitVec[i].getMidY() )*( _recHitVec[i].getMidY() );
	  x2y_sum=x2y_sum+( _recHitVec[i].getMidX() )*( _recHitVec[i].getMidX() )*( _recHitVec[i].getMidY() );
	  xy2_sum=xy2_sum+( _recHitVec[i].getMidX() )*( _recHitVec[i].getMidY() )*( _recHitVec[i].getMidY() );
	  xy_sum=xy_sum+( _recHitVec[i].getMidX() )*( _recHitVec[i].getMidY() );
	}
	//hitCandiSize+=1000;   //0 constrain   
	a1=x2_sum-x_sum*x_sum/hitCandiSize;
	a2=xy_sum-x_sum*y_sum/hitCandiSize;
	b1=a2;
	b2=y2_sum-y_sum*y_sum/hitCandiSize;
	c1=(x3_sum+xy2_sum-x_sum*(x2_sum+y2_sum)/hitCandiSize)/2.;
	c2=(y3_sum+x2y_sum-y_sum*(x2_sum+y2_sum)/hitCandiSize)/2.;
	//cout<<"a1 a2 b1 b2 c1 c2 "<<a1<<" "<<a2<<" "<<b1<<" "<<b2<<" "<<c1<<" "<<c2<<endl;

	//x_aver=x_sum/hitCandiSize;
	//y_aver=y_sum/hitCandiSize;

	_circleX =(b1*c2-b2*c1)/(b1*b1-a1*b2);
	_circleY =(b1*c1-a1*c2)/(b1*b1-a1*b2);
	_circleR=sqrt( (x2_sum+y2_sum-2*_circleX*x_sum-2*_circleY*y_sum)/hitCandiSize+_circleX*_circleX+_circleY*_circleY ); 
	//double r_temp=sqrt(r2);
	// cout<<"circle: ("<<_circleX<<","<<_circleY<<","<<_circleR<<endl;

	_d0=sqrt(_circleX*_circleX+_circleY*_circleY)-_circleR;
	_phi0=atan2(_circleY,_circleX)+M_PI/2.;          
	_omega=1/_circleR;               //according with Q
	double  pt_temp=_circleR/333.567;
	if(m_debug>0) cout<<" pt: "<<pt_temp<<endl;
	if(m_debug>0)  cout<<"par: ("<<_d0<<","<<_phi0<<","<<_omega<<")"<<endl;
	_pt=pt_temp;

	_circleX_least =(b1*c2-b2*c1)/(b1*b1-a1*b2);
	_circleY_least =(b1*c1-a1*c2)/(b1*b1-a1*b2);
	_circleR_least=sqrt( (x2_sum+y2_sum-2*_circleX*x_sum-2*_circleY*y_sum)/hitCandiSize+_circleX*_circleX+_circleY*_circleY ); 
	_pt_least=pt_temp;
	return 1;
  }
  else {return 0;}
}

int Hough2D::fit(){
 // replace in houghTrack
 // if(_charge==-1){                                                                      
 //   _d0=_d0;                                                                               
 //   _omega=-1.*fabs(_omega);                                                                   
 // }                                                                                       
 // if(_charge==1){                                                                       
 //   _d0=-_d0;
 //   _omega=1.*fabs(_omega);
 //   _phi0=_phi0-M_PI;                                                                       
 // } 
  TrkExchangePar tt(_d0,_phi0,_omega,0,0);
  if (m_debug>0) cout<<"q d0 phi0 omega: "<<_charge<<" "<<_d0<<" "<<_phi0<<" "<<_omega<<endl;
  TrkCircleMaker circlefactory;
  float chisum =0.;
  newTrack = circlefactory.makeTrack(tt, chisum, *_context, _bunchT0);
  bool permitFlips = true;
  bool lPickHits = true;
  circlefactory.setFlipAndDrop(*newTrack, permitFlips, lPickHits);
  //int nDigi = digiToHots(newTrack);
  TrkHitList* trkHitList = newTrack->hits();
  int digiId=0;
  vector<MdcHit*> t_hitCol;
  std::vector<HoughRecHit>::iterator iter = _recHitVec.begin();
  for (;iter != _recHitVec.end(); iter++, digiId++) {
	if( (*iter).getflag()!=0) continue;
	//if( (*iter).getOuter()==1) continue;
//	const MdcDetector* mgm = Global::m_gm; 
	const MdcDigi* aDigi = (*iter).digi();
	//MdcHit *hit = new MdcHit(aDigi, _m_gm);
	//MdcHit *hit = new MdcHit(aDigi, mgm);
	MdcHit *hit = new MdcHit(aDigi, Global::m_gm);
//	MdcHit mdcHit(aDigi,_m_gm);
//	MdcHit *hit = &mdcHit;
	t_hitCol.push_back(hit);
	Identifier id = aDigi->identify();
	int layer = MdcID::layer(id);
	int wire  = MdcID::wire(id);
	//if(layer>=12) continue;
	//if ((layer>=0&&layer<=7)||(layer>=20&&layer<=35)) {continue;}	
	hit->setCalibSvc(_mdcCalibFunSvc);
	hit->setCountPropTime(true); 
	// new fltLen and ambig
	int newAmbig = 0;
	// calc. position of this point
	MdcRecoHitOnTrack temp(*hit, newAmbig, _bunchT0*1.e9);//m_bunchT0 nano second here
	MdcHitOnTrack* newhot = &temp;
	newhot->setFltLen( (*iter).getRet().second);             //caculate by mc
	double distoTrack= (*iter).getDisToTrack();

	//double ddCut=1.0;
	double ddCut=1.0;
	int use_in2d=1;
	if(hit->driftTime(_bunchT0,0)>1000) use_in2d=0;   //   >1000or800?
	if(hit->driftDist(_bunchT0,0)>ddCut)  use_in2d=0; 
	//if(m_debug>0) cout<<"flt : "<<(*iter).getRet().second <<endl;
	if(m_debug>0) std::cout<<" ("<<layer<<","<<wire<<",rt "<<hit->rawTime()<<",dt "<<hit->driftTime(_bunchT0,0)<<",dd "<<hit->driftDist(_bunchT0,0)<<") T0 " << _mdcCalibFunSvc->getT0(layer,wire) <<" timewalk "<<  _mdcCalibFunSvc->getTimeWalk(layer, aDigi->getChargeChannel())<< "dist: "<<hit->driftDist(_bunchT0,0)<<" disToTrk "<< distoTrack<<"  use?: "<<use_in2d<<endl;
	if(use_in2d==0) continue;
	trkHitList->appendHot(newhot);
  }
  //newTrack->printAll(std::cout);

  TrkHitList* qhits = newTrack->hits();
  TrkErrCode err=qhits->fit();
  const TrkFit* newFit = newTrack->fitResult();
  int nActiveHit = newTrack->hots()->nActive();
  int fit_stat=false;
  double chi2=-999.;
  if (!newFit || (newFit->nActive()<3)||err.failure()!=0) {
	if(m_debug>0){
	  cout << " global 2d fit failed ";
	  if(newFit) cout <<" nAct "<<newFit->nActive();
	  cout<<"ERR1 failure ="<<err.failure()<<endl;
	  fit_stat=0;
	}
  }else{
	TrkExchangePar par = newFit->helix(0.);
	if( abs( 1/(par.omega()) )>0.03) {
	  fit_stat = 1;
	  chi2=newFit->chisq();
	  if(m_debug>0) cout<<"chi2 "<<chi2<<endl;
	}
	else {
	  fit_stat = 0;
	  chi2=-999;
	}

	bool badQuality = false;
	if(fabs(chi2)>m_qualityFactor*m_dropTrkChi2Cut ){
	  if(m_debug>0){
		std::cout<<__FILE__<<"   "<<__LINE__<<" drop track by chi2  "<<chi2<<" > "<<m_qualityFactor<<" * "<<m_dropTrkChi2Cut <<std::endl;
	  }
	  badQuality=1;
	}
	if( fabs(par.d0())>m_qualityFactor*m_dropTrkDrCut) {
	  if(m_debug>0){
		std::cout<<__FILE__<<"   "<<__LINE__<<" drop track by d0 "<<par.d0()<<" > "<<m_qualityFactor<<" * "<<m_dropTrkDrCut <<std::endl;
	  }
	  badQuality=1;
	}
	if( (fabs(chi2)/qhits->nHit()) > m_qualityFactor*m_dropTrkChi2NdfCut) {
	  if(m_debug>0){
		std::cout<<__FILE__<<"   "<<__LINE__<<" drop track by chi2/ndf"<<(fabs(chi2)/qhits->nHit()) <<" > "<<m_qualityFactor<<" * "<<m_dropTrkChi2NdfCut<<std::endl;
	  }
	  badQuality=1;
	}
	if( nActiveHit <= m_dropTrkNhitCut) {
	  if(m_debug>0){
		std::cout<<__FILE__<<"   "<<__LINE__<<" drop track by nhit"<<nActiveHit <<" <= "<<m_dropTrkNhitCut<<std::endl;
	  }
	  badQuality=1;
	}
	if( badQuality) fit_stat=0;
  }
  if( fit_stat==1 ){
	if(m_debug>0){
	  cout << " global 2d fit success"<<endl;
	  cout<<__FILE__<<__LINE__<<"AFTER fit 2d "<<endl;
	  newTrack->print(std::cout);
	}
	TrkExchangePar par = newFit->helix(0.);
	double d0=par.d0();
	double phi0=par.phi0();
	double omega=par.omega();
	double r_temp=-1./par.omega();
	double x_cirtemp = sin(par.phi0()) *(par.d0()+1/par.omega()) * -1.; //z axis verse,x_babar = -x_belle
	double y_cirtemp = -1.*cos(par.phi0()) *(par.d0()+1/par.omega()) * -1;//???
	if(m_debug>0){
	  cout<<" circle after globle 2d:  "<<"("<<x_cirtemp<<","<<y_cirtemp<<","<<r_temp<<")"<<endl;
	  cout<<"pt_rec:   "<<1./omega/333.567<<endl;
	}
	_pt=1./omega/333.567; 
	_circleX=x_cirtemp;
	_circleY=y_cirtemp;
	_circleR=_charge/omega;
	_d0=par.d0();
	_phi0=par.phi0();
	_omega=par.omega();

	int nfit2d=0;
	if(m_debug>1) cout<<" hot list:"<<endl;
	TrkHotList::hot_iterator hotIter= qhits->hotList().begin();
	int lay=((MdcHit*)(hotIter->hit()))->layernumber();
	int wir=((MdcHit*)(hotIter->hit()))->wirenumber();
	int hittemp=lay*1000+wir;
	while (hotIter!=qhits->hotList().end()) {
	  if(m_debug>1){ cout <<"(" <<((MdcHit*)(hotIter->hit()))->layernumber()
		<<","<<((MdcHit*)(hotIter->hit()))->wirenumber() 
		  <<":"<<hotIter->isActive()<<")  ";
	  }
	  if (hotIter->isActive()==1) nfit2d++;
	  hotIter++;
	}
	_nfit=nfit2d;
  }
  _chi2_aver2D=chi2/_nfit;

  for(int i=0;i<t_hitCol.size();i++){
	delete t_hitCol[i];
  }
  delete newTrack;

  if(m_debug>0) cout<<" in 2D fit number of Active hits : "<<_nfit<<endl;
  return fit_stat;
}

/*
int Hough2D::digiToHots(TrkRecoTrk* newTrack){
  TrkHitList* trkHitList = newTrack->hits();
  int digiId=0;
  std::vector<HoughRecHit>::iterator iter = _recHitVec.begin();
  for (;iter != _recHitVec.end(); iter++, digiId++) {
	if( (*iter).getflag()!=0) continue;
	//if( (*iter).getOuter()==1) continue;
	const MdcDigi* aDigi = (*iter).digi();
	//cout<<"Digi in hot "<<aDigi<<endl;
	MdcHit *hit = new MdcHit(aDigi, _m_gm);
	_MdcHitCol.push_back(hit);
	Identifier id = aDigi->identify();
	int layer = MdcID::layer(id);
	int wire  = MdcID::wire(id);
	//if ((layer>=0&&layer<=7)||(layer>=20&&layer<=35)) {continue;}	
	hit->setCalibSvc(_mdcCalibFunSvc);
	hit->setCountPropTime(true); 
	// new fltLen and ambig
	int newAmbig = 0;
	// calc. position of this point
	MdcRecoHitOnTrack temp(*hit, newAmbig, _bunchT0*1.e9);//m_bunchT0 nano second here
	MdcHitOnTrack* newhot = &temp;
	newhot->setFltLen( (*iter).getRet().second);             //caculate by mc

	double ddCut;
	if(layer<8) ddCut=1.0;
	else        ddCut=1.0;
	int use_in2d=1;
	if(hit->driftTime(_bunchT0,0)>800) use_in2d=0;; 
	if(hit->driftDist(_bunchT0,0)>ddCut)  use_in2d=0; 
	//if(m_debug>0) cout<<"flt : "<<(*iter).getRet().second <<endl;
	if(m_debug>0) std::cout<<" ("<<layer<<","<<wire<<",rt "<<hit->rawTime()<<",dt "<<hit->driftTime(_bunchT0,0)<<") T0 " << _mdcCalibFunSvc->getT0(layer,wire) <<" timewalk "<<  _mdcCalibFunSvc->getTimeWalk(layer, aDigi->getChargeChannel())<< "dist: "<<hit->driftDist(_bunchT0,0)<<"  use?: "<<use_in2d<<endl;
	if(use_in2d==0) continue;
	trkHitList->appendHot(newhot);
  }
  return trkHitList->nHit();
}
*/
void Hough2D::print(){
  cout<<" print hough2d "<<endl;
  cout<<"par: "<<_d0<<" "<<_phi0<<" "<<_omega<<endl;
}

