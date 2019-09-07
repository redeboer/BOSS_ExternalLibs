#include "MdcHoughFinder/Hough3D.h"
int Hough3D::m_debug=0;
vector<MdcHit*> vec_for_clean;
vector<TrkRecoTrk*> vectrk_for_clean;
TrkContextEv* Hough3D::_context=NULL;
const MdcCalibFunSvc* Hough3D::_mdcCalibFunSvc=NULL;
int Hough3D::m_qualityFactor=3;
//double Hough3D::m_dropTrkDrCut=1;
//double Hough3D::m_dropTrkDzCut=10;
double Hough3D::m_dropTrkDrCut=1;        //no limit of dr dz
double Hough3D::m_dropTrkDzCut=10;
double Hough3D::m_dropTrkNhitCut=9;
double Hough3D::m_dropTrkChi2Cut=99999;
double Hough3D::m_dropTrkChi2NdfCut=99999;

Hough3D::Hough3D(){
  //_m_gm = MdcDetector::instance(0);
}
Hough3D::Hough3D(const Hough3D& other){
  _circleR=other._circleR;
  _circleX=other._circleX;
  _circleY=other._circleY;
  _charge=other._charge;
  _d0=other._d0;
  _phi0=other._phi0;
  _omega=other._omega;
  _tanl=other._tanl;
  _z0=other._z0;

  _pt=other._pt;
  _pz=other._pz;
  _p=other._p;
  _nfit=other._nfit;
  _bunchT0=other._bunchT0;
  _chi2_aver=other._chi2_aver;
  _m_gm = other._m_gm;
  _recHitVec=other._recHitVec;
  vec_mdcHit=other.vec_mdcHit;
}

Hough3D::Hough3D(const Hough2D& hough2D,recHitCol hitCol, double bunchtime,double tanl,double z0):_tanl(tanl),_z0(z0){
  //_m_gm = MdcDetector::instance(0);
  _circleR=hough2D.getCirR();
  _circleX=hough2D.getCirX();
  _circleY=hough2D.getCirY();
  _d0 = hough2D.getD0();
  _phi0 = hough2D.getPhi0();
  _omega= hough2D.getOmega();
  _charge=hough2D.getCharge();
  _bunchT0=bunchtime;
  _recHitVec=hitCol;
  _pt = 0;
  _p = 0;
  _pz = 0;
  _nfit=0;
  newTrack=NULL;
}
Hough3D::Hough3D(const Hough2D& hough2D,recHitCol hitCol, double bunchtime,double tanl,double z0,const vector<MdcHit*>* mdchit):_tanl(tanl),_z0(z0),vec_mdcHit(mdchit){
  //_m_gm = MdcDetector::instance(0);
  _circleR=hough2D.getCirR();
  _circleX=hough2D.getCirX();
  _circleY=hough2D.getCirY();
  _d0 = hough2D.getD0();
  _phi0 = hough2D.getPhi0();
  _omega= hough2D.getOmega();
  _charge=hough2D.getCharge();
  _bunchT0=bunchtime;
  _recHitVec=hitCol;
  _pt = 0;
  _p = 0;
  _pz = 0;
  _nfit=0;
  newTrack=NULL;
}

int Hough3D::fit(){
  if(_charge==-1){                                                                      
//	cout<<" charge -1 "<<endl;
//	_d0=-_d0;                                                                               
//	_omega=-1.*fabs(_omega);                                                                   
  }                                                                                       
  if(_charge==1){                                                                       
//	cout<<" charge +1 "<<endl;
//	_d0=_d0;                                                                                
//	_omega=1.*fabs(_omega);
	//_phi0=_phi0-M_PI;                                                                       
  } 

  //outerHit();                // delete 4 hit in a wire  for kalman

  TrkExchangePar tt(_d0,_phi0,_omega,_z0,_tanl);
  if (m_debug>0) cout<<"q d0 phi0 omega: "<<_charge<<" "<<_d0<<" "<<_phi0<<" "<<_omega<<" "<<_tanl<<" "<<_z0<<endl;
  TrkHelixMaker helixfactory;
  float chisum =0.;
  newTrack = helixfactory.makeTrack(tt, chisum, *_context, _bunchT0);
  //vectrk_for_clean.push_back(newTrack);
  bool permitFlips = true;
  bool lPickHits = true;
  helixfactory.setFlipAndDrop(*newTrack, permitFlips, lPickHits);
  TrkHitList* trkHitList = newTrack->hits();
  int digiId=0;
  std::vector<HoughRecHit>::iterator iter = _recHitVec.begin();
  for (;iter != _recHitVec.end(); iter++, digiId++) {
	if( (*iter).getflag()!=0 ) continue;
	//if( (*iter)->calDriftDist(_bunchT0,0)>0.8) continue; 
	//if( (*iter)->driftTime(_bunchT0,0)>800) continue;
	//cout<<"rechit dist time  "<<(*iter).calDriftDist(_bunchT0,0)<<" "<<(*iter).driftTime(_bunchT0,0)<<endl;

	const MdcDigi* aDigi = (*iter).digi();
	MdcHit* hit;
	// compare
	vector<MdcHit*>::const_iterator iter_digi = (*vec_mdcHit).begin();
	for (;iter_digi != (*vec_mdcHit).end(); iter_digi++) {
	  if( (*iter_digi)->digi() == (*iter).digi() )  {
		hit = (*iter_digi);
	  }
	}
	Identifier id = aDigi->identify();
	int layer = MdcID::layer(id);
	int wire  = MdcID::wire(id);
	hit->setCalibSvc(_mdcCalibFunSvc);
	hit->setCountPropTime(true); 
	// new fltLen and ambig
	int newAmbig = 0;
	// calc. position of this point
	MdcRecoHitOnTrack temp( *hit, newAmbig, _bunchT0*1.e9);//m_bunchT0 nano second here
	MdcHitOnTrack* newhot = &temp;
	double flight;
	double z_flight;
	//if( layer<8 ) flight = sqrt( ((*iter).getsPos())*((*iter).getsPos())+((*iter).getzPos())*((*iter).getzPos())) ;
	//	if( layer<8 ) flight =(*iter).getsPos();
	flight = (*iter).getRet().second;
	double distoTrack= (*iter).getDisToTrack();
	//	if( layer<8 ) newhot->setFltLen( (*iter).getsPos());             //caculate by mc
	//	else newhot->setFltLen( (*iter).getRet().second);             //caculate by mc
	newhot->setFltLen( flight );
	//	if(layer==18) continue;

	int use_in3d=1;
	//	if(hit->driftDist(_bunchT0,0)>1.0)  use_in3d=0; 
	if(hit->driftTime(_bunchT0,0)>1000) use_in3d=0; 
	//if(m_debug>0) cout<<"flt : "<<(*iter).getsPos()<<endl;
	if(m_debug>0) cout<<"flt : "<<flight<<endl;
	if(m_debug>0) std::cout<<" ("<<layer<<","<<wire<<",rt "<<hit->rawTime()<<",dt "<<hit->driftTime(_bunchT0,0)<<") T0 " << _mdcCalibFunSvc->getT0(layer,wire) <<" timewalk "<<  _mdcCalibFunSvc->getTimeWalk(layer, aDigi->getChargeChannel())<< "dist: "<<hit->driftDist(_bunchT0,0)<<" disToTrk "<< distoTrack<<"  use?: "<<use_in3d<<endl;
	//	if(hit->driftTime(_bunchT0,0)>800) continue; 
	if(use_in3d==0) continue;
	trkHitList->appendHot(newhot);
  }
  if( m_debug>0) newTrack->printAll(std::cout);

  TrkHitList* qhits = newTrack->hits();
  TrkErrCode err=qhits->fit();
  const TrkFit* newFit = newTrack->fitResult();
  int nActiveHit = newTrack->hots()->nActive();
  int fit_stat=false;
  double chi2=-999.;
  if (!newFit || (newFit->nActive()<5)||err.failure()!=0){    
  //if (!newFit )
	if(m_debug>0){
	  cout << " global 3d fit failed ";
	  if(newFit) cout <<" nAct "<<newFit->nActive();
	  cout<<"ERR1 failure ="<<err.failure()<<endl;
	  fit_stat=0;
	}
  }else{
	TrkExchangePar par = newFit->helix(0.);
	if( abs( 1/(par.omega()) )>0.03) {
	  fit_stat = 1;
	  chi2=newFit->chisq();
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
	if( fabs(par.z0())>m_qualityFactor*m_dropTrkDzCut) {
	  if(m_debug>0){
		std::cout<<__FILE__<<"   "<<__LINE__<<" drop track by z0 "<<par.z0()<<" > "<<m_qualityFactor<<" * "<<m_dropTrkDzCut <<std::endl;
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
	//badQuality = false;   //not delete track in this stage
	if( badQuality) fit_stat=0;
  }
  if( fit_stat!=1 ){
	delete newTrack;
	vectrk_for_clean.pop_back();
  }
  if( fit_stat==1 ){
	if(m_debug>0){
	  cout << " global 3d fit success"<<endl;
	  cout<<__FILE__<<__LINE__<<"AFTER fit 3d "<<endl;
	  newTrack->print(std::cout);
	}
	TrkExchangePar par = newFit->helix(0.);
	_d0=par.d0();
	_phi0=par.phi0();
	_omega=par.omega();
	_tanl=par.tanDip();
	_z0=par.z0();

	_circleR=_charge/par.omega();
	_circleX= sin(par.phi0()) *(par.d0()+1/par.omega()) * -1.; //z axis verse,x_babar = -x_belle
	_circleY= -1.*cos(par.phi0()) *(par.d0()+1/par.omega()) * -1;//???
	double pxy=fabs(_circleR/333.567);
	double pz=pxy*par.tanDip();
	double pxyz=_charge*sqrt(pxy*pxy+pz*pz);
	_pt=pxy; 
	_pz=pz;
	_p=pxyz;
	if(m_debug>0){
	  cout<<" circle after globle 3d:  "<<"("<<_circleX<<","<<_circleY<<","<<_circleR<<")"<<endl;
	  cout<<"pt_rec:   "<<_pt<<endl;
	  cout<<"pz_rec:   "<<_pz<<endl;
	  cout<<"p_rec:   "<<_p<<endl;
	}

	int nfit3d=0;
	if(m_debug>0) cout<<" hot list:"<<endl;
	TrkHotList::hot_iterator hotIter= qhits->hotList().begin();
	int lay=((MdcHit*)(hotIter->hit()))->layernumber();
	int wir=((MdcHit*)(hotIter->hit()))->wirenumber();
	while (hotIter!=qhits->hotList().end()) {
	  if(m_debug>0){ cout <<"(" <<((MdcHit*)(hotIter->hit()))->layernumber()
		<<","<<((MdcHit*)(hotIter->hit()))->wirenumber() 
		  <<":"<<hotIter->isActive()<<")  ";
	  }
	  //	  cout << "nuse "<<hotIter->hit()->nUsedHits()<<endl;
	  if (hotIter->isActive()==1) nfit3d++;
	  hotIter++;
	}
	_nfit=nfit3d;
  }
  _chi2_aver=chi2/_nfit;
  //  for(int i=0;i<t_hitCol.size();i++){
  //	delete t_hitCol[i];
  //  }
  if(m_debug>0) cout<<" in 3D fit number of Active hits : "<<_nfit<<endl;
  return fit_stat;
}

void Hough3D::print(){
  cout<<" print hough3d "<<endl;
  cout<<"par: "<<_d0<<" "<<_phi0<<" "<<_omega<<" "<<_tanl<<" "<<_z0<<endl;
}

bool less_layer_3D(const int& a,const int& b){
  return a < b;
}

void Hough3D::outerHit(){
  for(int ihit=0;ihit<_recHitVec.size();ihit++){
	if( _recHitVec[ihit].calDriftDist(_bunchT0,0)>0.8 || _recHitVec[ihit].driftTime(_bunchT0,0)>800 ) _recHitVec[ihit].setflag(-5);   //flag -5 time or drift
  }

  vector<int> vec_layer_num[43];
  for(int ilay=0;ilay<43;ilay++){
	for(int ihit=0;ihit<_recHitVec.size();ihit++){
	  if( _recHitVec[ihit].getLayerId()==ilay && _recHitVec[ihit].getflag()==0 ) {
		vec_layer_num[ilay].push_back( _recHitVec[ihit].getWireId() );
	  }
	}
	std::sort( vec_layer_num[ilay].begin(),vec_layer_num[ilay].end(),less_layer_3D);
	if( vec_layer_num[ilay].size() < 4 ) continue;
	for(int j=0;j<vec_layer_num[ilay].size();j++) {
	  //	  if( (vec_layer_num[ilay][j]+1 == vec_layer_num[ilay][j+1]) &&  (vec_layer_num[ilay][j+1]+1 == vec_layer_num[ilay][j+2]) &&  (vec_layer_num[ilay][j+2]+1 == vec_layer_num[ilay][j+3]) ) {
	  for(int jhit=0;jhit<_recHitVec.size();jhit++) {
		if( (ilay==_recHitVec[jhit].getLayerId()) && (vec_layer_num[ilay][j]==_recHitVec[jhit].getWireId() ) )   {_recHitVec[jhit].setflag(-1);}
	  }
	  //}
	}
  }
}
