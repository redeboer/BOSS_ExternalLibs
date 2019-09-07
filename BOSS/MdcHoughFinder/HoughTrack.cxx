#include "MdcHoughFinder/HoughTrack.h"
#include "MdcHoughFinder/HoughMap.h"
#include "MdcHoughFinder/HoughStereo.h"
#include <vector>
#include <map>
#include "TF1.h" 
#include "TGraph.h" 
using namespace std;

bool HoughTrack::m_debug=0;

HoughTrack::HoughTrack(){
  p_trk=NULL;
  p_trk2D=NULL;
}
HoughTrack::~HoughTrack(){
}

HoughTrack& HoughTrack::operator=(const HoughTrack& other){
  _dist=(other._dist);
  _charge=(other._charge);
  _ptLeast=(other._ptLeast);
  _pt2D=(other._pt2D);
  _pt3D=(other._pt3D);
  _pz=(other._pz);
  _p=(other._p);

  _d0=other._d0;
  _omega=other._omega;
  _phi0=other._phi0;
  _tanl=(other._tanl);
  _z0=(other._z0);
  _tanl_zs=(other._tanl_zs);
  _z0_zs=(other._z0_zs);

  _centerPeak=(other._centerPeak);
  _Hough2D=(other._Hough2D);
  _Hough3D=(other._Hough3D);
  _bunchTime=(other._bunchTime);
  _centerX=(other._centerX);
  _centerY=(other._centerY);
  _centerR=(other._centerR);
  _chi2_aver=(other._chi2_aver);
  _nfit=(other._nfit);
  _chi2_aver2D=(other._chi2_aver2D);
  _nfit2D=(other._nfit2D);
  _recHitVec=(other._recHitVec);
  _stat2D=(other._stat2D);
  _stat3D=(other._stat3D);
  p_trk=other.p_trk;
  p_trk2D=other.p_trk2D;
  _maprho = other._maprho;
  _maptheta= other._maptheta;
  t_pro_correct= other.t_pro_correct;
  _houghList = other._houghList;
  vec_mdcHit = other.vec_mdcHit;
  //  if(_recHitVec.size() != 0 ){
  //	for(int i =0;i<_recHitVec.size();i++){
  //	  delete _recHitVec[i];
  //	  _recHitVec[i]=NULL;
  //	}
  //	_recHitVec.clear();
  //  }
  // for(int i =0;i<other._recHitVec.size();i++){
  //   //const HoughHit* p_hit= new HoughHit( *(other._recHitVec[i]));
  //   HoughRecHit p_hit ( (trackHitList[i])->digi(),0.,0. );
  //   _recHitVec.push_back(p_hit);
  // }
}

HoughTrack::HoughTrack(const HoughTrack& other):
  _dist(other._dist),
  _charge(other._charge),
  _ptLeast(other._ptLeast),
  _pt2D(other._pt2D),
  _pt3D(other._pt3D),
  _pz(other._pz),
  _p(other._p),

  _d0(other._d0),
  _omega(other._omega),
  _phi0(other._phi0),
  _z0(other._z0),
  _tanl(other._tanl),
  _z0_zs(other._z0_zs),
  _tanl_zs(other._tanl_zs),

  _centerPeak(other._centerPeak),
  _Hough2D(other._Hough2D),
  _Hough3D(other._Hough3D),
  _bunchTime(other._bunchTime),
  _centerX(other._centerX),
  _centerY(other._centerY),
  _centerR(other._centerR),
  _chi2_aver(other._chi2_aver),
  _nfit(other._nfit),
  _chi2_aver2D(other._chi2_aver2D),
  _nfit2D(other._nfit2D),
  _stat2D(other._stat2D),
  _stat3D(other._stat3D),
  _recHitVec(other._recHitVec),
  p_trk(other.p_trk),
  p_trk2D(other.p_trk2D),
  _maprho(other._maprho),
  _maptheta(other._maptheta),
  t_pro_correct(other.t_pro_correct),
  _houghList(other._houghList),
  vec_mdcHit ( other.vec_mdcHit)
{
  // _recHitVec.clear();
  //if(_recHitVec.size() != 0 ){
  //  //cout<<"clear first "<<endl;
  //  for(int i =0;i<_recHitVec.size();i++){
  //    delete _recHitVec[i];
  //    _recHitVec[i]=NULL;
  //  }
  //  _recHitVec.clear();
  //}
  //for(int i =0;i<other._recHitVec.size();i++){
  //  //const HoughHit* p_hit= new HoughHit( *(other._recHitVec[i]));
  //  HoughRecHit p_hit ( (trackHitList[i])->digi(),0.,0. );
  //  _recHitVec.push_back(p_hit);
  //}
}

HoughTrack& HoughTrack::add(const HoughTrack& other)
{
  for(int i =0;i<other._recHitVec.size();i++){
	int same=0;
	for(int j =0;j<_recHitVec.size();j++){
	  if( _recHitVec[j].digi() == other._recHitVec[i].digi() )  { same=1;}
	}
	if( same==0 ) {
	  //const HoughHit* p_hit= new HoughHit( *(other._recHitVec[i]));
	  //HoughRecHit p_hit ( (other._recHitVec[i]).digi(),0.,1);
	  HoughRecHit p_hit ( (other._recHitVec[i]));
	  _recHitVec.push_back(p_hit);
	}
	//cout<<" after copy: "<<i<<"  new: "<<p_hit<<" old:  "<<other._recHitVec[i]<<endl;
  }
  return *this;
}

HoughTrack::HoughTrack(const HoughPeak& centerPeak , std::vector<const HoughHit*> trackHitList,double rho,double theta){
  _centerPeak=centerPeak;
  _recHitVec.clear();
  _stat2D=0;
  _stat3D=0;
  _tanl=-999;
  _z0=-999;
  _ptLeast=-999;
  _pt2D=-999;
  _pt3D=-999;
  _p=-999;
  _d0=-999;
  _omega=-999;
  _phi0=-999;
  _z0=-999;
  _tanl=-999;
  _chi2_aver=-999;
  _nfit=-999;
  p_trk=NULL;
  p_trk2D=NULL;
  _charge=0;   //undeter mine
  _maprho=rho;
  _maptheta=theta;
  //if(_recHitVec.size()!= 0 ){
  //  cout<<" Mind  trackhitlist size !=0 "<<endl;
  //  for(int i =0;i<_recHitVec.size();i++){
  //    delete _recHitVec[i];
  //    _recHitVec[i]=NULL;
  //  }
  //  _recHitVec.clear();
  //}
  int t_size = trackHitList.size();
  for(int i =0;i<t_size;i++){
	HoughRecHit p_hit ( *(trackHitList[i]),0.,0.,1);
	p_hit.setPtr2D(&_Hough2D);
	p_hit.setflag(0);
	_recHitVec.push_back(p_hit);
  }
}

bool digi_in_track(const HoughRecHit& hita,const HoughRecHit& hitb){
  return hita.getLayerId() < hitb.getLayerId();
}
void HoughTrack::sortHit(){
  std::sort (_recHitVec.begin(),_recHitVec.end(),digi_in_track);
}
int HoughTrack::fit2D(double bunchtime){
  sortHit();
//  cout<<"in fit2d"<<endl;
  _bunchTime=bunchtime;
  _stat2D =  fitLeast();
//  outerHit();
  _stat2D = fit_global2D(_recHitVec); 
  //printRecHit();
  //collectAxialHit();
  //_Hough2D.print();
  //cald_layer();
  return _stat2D;
}

bool less_layer(const int& a,const int& b){
  return a < b;
}
void HoughTrack::outerHit(){
  vector<int> vec_layer_num[43];
  for(int ilay=0;ilay<43;ilay++){
	for(int ihit=0;ihit<_recHitVec.size();ihit++){
	  if( _recHitVec[ihit].getLayerId()==ilay && _recHitVec[ihit].getflag()==0 ) {
		vec_layer_num[ilay].push_back( _recHitVec[ihit].getWireId() );
	  }
	}
	std::sort( vec_layer_num[ilay].begin(),vec_layer_num[ilay].end(),less_layer);
	//for(int j=0;j<vec_layer_num[ilay].size();j++) cout<<"("<<ilay<<","<<vec_layer_num[ilay][j]<<")"<<endl;
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


//int HoughTrack::collectAxialHit(){
//  for( int ilay=8;ilay<20;ilay++){
//	for( int ihit=0;ihit<_recHitVec.size();ihit++){
//	  if( _recHitVec[ihit].getLayerId()==ilay && _recHitVec[ihit].getflag()==0) {
//		_recHitVec[ihit].setflag(1);
//	  }
//	}
//  }
//}

int HoughTrack::collectAxialHit(){
  vector<HoughRecHit> hitCol;
  int lay_contain[12]; //FIXME
  for( int ilay=8;ilay<20;ilay++){
	int lay_count=0;
	for( int ihit=0;ihit<_recHitVec.size();ihit++){
	  if( _recHitVec[ihit].getLayerId()==ilay && _recHitVec[ihit].getflag()==0) {
		hitCol.push_back( _recHitVec[ihit]);
		lay_count++;
	  }
	}
	lay_contain[ilay-8]=lay_count;
  }
  //int **a;
  int **a;
  int *len;
  int p[12];
  a = (int**)malloc(sizeof(int*) * 12);
  len = (int*)malloc(sizeof(int) * 12);
  for (int i = 0; i < 12; i++) {
	int add_lay_count=0;
	for (int s = 0; s < i; s++) {
	  add_lay_count+=lay_contain[s];
	}
	int n = lay_contain[i];
	len[i] = n;
	a[i] = (int*)malloc(sizeof(int) * n);
	for (int j = 0; j < n; j++) {
	  a[i][j]= add_lay_count+j;
	  cout<<"aij "<<a[i][j]<<endl;
	  cout<<"("<<hitCol[add_lay_count+j].getLayerId()<<","<<hitCol[add_lay_count+j].getWireId()<<")"<<endl;
	}
  }
  int numall=0;
  fun(0,a,len,p,hitCol,numall);
  cout<<" num all "<<numall<<endl;
}
void HoughTrack::fun(int level,int** a,int* len,int* p,vector<HoughRecHit>& hitCol,int& numall) {
  if (level < 12) {
	cout<<"level "<<level<<" len "<<len[level]<<endl;
	for (int i = 0; i < len[level]; i++) {
	  cout<<i<<" i "<<endl;
	  p[level] = i;
	  cout<<i<<" i "<<endl;
	  cout<<"fun "<<level+1<<endl;
	  fun(level+1,a,len,p,hitCol,numall);
	}
  } else {
	vector<HoughRecHit> recHitCol;
	for (int i = 0; i < 12; i++) {
	  int num=a[i][p[i]];
	  cout<<"("<<hitCol[num].getLayerId()<<","<<hitCol[num].getWireId()<<")"<<endl;
	  recHitCol.push_back( hitCol[num] );
	}
	fit_global2D(recHitCol);
	numall++;
  }
}

int HoughTrack::fit3D(){
  sortHit();
//  cout<<"in fit3d"<<endl;
  int nhit_zs = calzs();
  if( nhit_zs >=3 ) fitzs();
  else { 
	_tanl=0.;_z0=0.; 
  }
  outerHit();
  if(m_debug>0) { cout<< " before 3d fit "<<endl; this->printRecHit();}
  _stat3D = fit_global3D(0);
  return _stat3D;
}

int HoughTrack::fit3D_inner(){      //for multi turn track
  sortHit();
  int nhit_zs = calzs();
  cutMultiCirHit();
  if( nhit_zs >=3 ) fitzs();
  else { 
//	cout<<" not enough ster hit in zs fit"<<endl;
	_tanl=0.;_z0=0.; 
  }
  cutMultiCirHit_after_zs();
  outerHit();
  if(m_debug>0) { cout<< " before 3d fit "<<endl; this->printRecHit();}
  _stat3D = fit_global3D(0);
  return _stat3D;
}

int HoughTrack::cutMultiCirHit(){
  for(int i=0;i<_recHitVec.size();i++){
  if( _recHitVec[i].getLayerId()>8 ) continue;
	if( (fabs(_recHitVec[i].getzAmb(0))>10) && (fabs(_recHitVec[i].getzAmb(1))>10) ) {
	_recHitVec[i].setflag(1);
//	cout<<"reject ("<<_recHitVec[i].getLayerId()<<","<<_recHitVec[i].getWireId()<<")"<<_recHitVec[i].getCirList()<<endl;
	}
  }
  _Hough3D.setRecHit(_recHitVec);
}

int HoughTrack::cutMultiCirHit_after_zs(){
  for(int i=0;i<_recHitVec.size();i++){
  int layer = _recHitVec[i].getLayerId();
  double zl = _recHitVec[i].getzAmb(0);
  double zr = _recHitVec[i].getzAmb(1);
  double sl = _recHitVec[i].getsAmb(0);
  double sr = _recHitVec[i].getsAmb(1);
  double dl = zl - (sl*_tanl+_z0);
  double dr = zr - (sr*_tanl+_z0);
  if (layer<=8 && fabs(dl)>10 && fabs(dr)>10 ) _recHitVec[i].setflag(1);
  if (layer>8  && fabs(dl)>20 && fabs(dr)>20 ) _recHitVec[i].setflag(1);
//  if (layer>30) _recHitVec[i].setflag(1);
  }
  _Hough3D.setRecHit(_recHitVec);
}

int HoughTrack::fitLeast(){
  _Hough2D=Hough2D(_recHitVec,_bunchTime);
  double circleR = fabs(1./(_maprho));
  double circleX = (1./_maprho)*cos(_maptheta); 
  double circleY = (1./_maprho)*sin(_maptheta); 
  double d0 = sqrt( circleX*circleX + circleY*circleY )-circleR;
  double phi0 =  atan2(circleY,circleX)+M_PI/2.;
  double omega = 1/circleR;
  if(_charge==-1){                                                                      
	d0=d0;                                                                               
	omega=-1.*fabs(omega);                                                                   
  }                                                                                       
  if(_charge==1){                                                                       
	d0=-d0;
	omega=1.*fabs(omega);
	phi0=phi0-M_PI;                                                                       
  } 
  _Hough2D.setCharge(_charge);
  _Hough2D.setCirX( circleX); 
  _Hough2D.setCirY( circleY ); 
  _Hough2D.setCirR( circleR ); 
  _Hough2D.setD0( d0 );
  _Hough2D.setPhi0( phi0 );
  _Hough2D.setOmega( omega );
  _Hough2D.setPt( circleR/333.567 );

  //fill HoughTrack
  _centerX=_Hough2D.getCirX();
  _centerY=_Hough2D.getCirY();
  _centerR=_Hough2D.getCirR();
  //_pt=_Hough2D.getPt();
  _pt2D=_Hough2D.getPt();
  _d0=_Hough2D.getD0();
  _omega=_Hough2D.getOmega();
  _phi0=_Hough2D.getPhi0();
  hitOnTrack();
  if(m_debug>0) {cout<<" after least 2d "<<endl; printRecHit();}
  return 1;

}

int HoughTrack::fit_global2D(vector<HoughRecHit>& recHitVec){
  //_Hough2D=Hough2D(_Hough2D,_recHitVec,bunchtime);
  //_Hough2D.setRecHit(_recHitVec);
  _Hough2D.setRecHit(recHitVec);
  int Stat_2d=_Hough2D.fit(); 
  p_trk2D = _Hough2D.getTrk();
  if(Stat_2d==1){
	_centerX=_Hough2D.getCirX();
	_centerY=_Hough2D.getCirY();
	_centerR=_Hough2D.getCirR();
	//_pt=_Hough2D.getPt();
	_pt2D=_Hough2D.getPt();
	_d0=_Hough2D.getD0();
	_omega=_Hough2D.getOmega();
	_phi0=_Hough2D.getPhi0();
	_chi2_aver2D = _Hough2D.getChi2_2D();
	_nfit2D= _Hough2D.getNfit();
	if(m_debug>0) cout<<"pt after global 2d: "<<_pt2D<<endl;
	if(m_debug>0) cout<<"after global 2d "<<endl;
	hitOnTrack();
	if(m_debug>0) { cout<<" after global 2d "<<endl; printRecHit();} 
  }
  else {
	if(m_debug>0) cout<<" 2d global fail"<<endl;
  }
  return Stat_2d;
}

int HoughTrack::fit_global3D(int time){
  if( time==0 ) _Hough3D=Hough3D(_Hough2D,_recHitVec,_bunchTime,_tanl,_z0,vec_mdcHit);
  if(m_debug>0) _Hough3D.print();
  //  _Hough3D.setCharge(_charge);
  int Stat_3d=_Hough3D.fit(); 
  p_trk = _Hough3D.getTrk();
  if(Stat_3d==1){
	_centerX=_Hough3D.getCirX();
	_centerY=_Hough3D.getCirY();
	_centerR=_Hough3D.getCirR();
	_pt3D=_Hough3D.getPt();
	_p=_Hough3D.getP();
	_pz=_Hough3D.getPz();
	_d0=_Hough3D.getD0();
	_omega=_Hough3D.getOmega();
	_phi0=_Hough3D.getPhi0();
	_tanl=_Hough3D.getTanl();
	_z0=_Hough3D.getZ0();
	_chi2_aver = _Hough3D.getChi2();
	_nfit= _Hough3D.getNfit();
	if(m_debug>0) cout<<"pt after global 3d: "<<_pt3D<<endl;
	//	hitOnTrack();
  }
  else {
	if(m_debug>0) cout<<" 3d global fail"<<endl;
  }
  return Stat_3d;
}

void HoughTrack::hitOnTrack(){
  if(m_debug>0) cout<<" calculate hit on track "<<endl;
  for(int ihit=0;ihit<_recHitVec.size();ihit++){
	std::pair<double,double> theta_l = calcuArcTrack( (_recHitVec[ihit]) ); 
	int flag = judge_half(_recHitVec[ihit]);
	double dist=calcuDistToTrack( (_recHitVec[ihit]));
	double distToCir=calcuDistToCir( (_recHitVec[ihit]));
	_recHitVec[ihit].setflag(flag);  //FIXME
	_recHitVec[ihit].setDisToTrack(dist);
	_recHitVec[ihit].setDisToCir(distToCir);
	_recHitVec[ihit].setRet(theta_l);
  }
  //	cut_axial_inner();
}

int HoughTrack::judge_half(const HoughRecHit& hit){
  int cir;
  double xhit =hit.getMidX();
  double yhit =hit.getMidY();
  double x_cir=_Hough2D.getCirX();
  double y_cir=_Hough2D.getCirY();
  double r_cir=_Hough2D.getCirR();
  if(_charge==-1){
	if( (x_cir*yhit - y_cir*xhit >=0) ) cir=0;
	else cir=1;
  }
  else if(_charge==1){
	if( (x_cir*yhit - y_cir*xhit <=0) ) cir=0;
	else cir=1;
  }
  else cout<<" charge is not set !!!!!!!!!!"<<endl;

  return cir;
}

double HoughTrack::calcuDistToTrack(const HoughRecHit& hit){
  double xhit =hit.getMidX();
  double yhit =hit.getMidY();
  double x_cir=_Hough2D.getCirX();
  double y_cir=_Hough2D.getCirY();
  double r_cir=_Hough2D.getCirR();
  double dist=sqrt( pow( (xhit-x_cir),2)+pow( (yhit-y_cir),2) )-r_cir;
  return dist;
}

double HoughTrack::calcuDistToCir(const HoughRecHit& hit){
  double xhit =hit.getMidX();
  double yhit =hit.getMidY();
  double x_cir=_Hough2D.getCirX();
  double y_cir=_Hough2D.getCirY();
  double r_cir=_Hough2D.getCirR();
  double dist=sqrt( pow( (xhit-x_cir),2)+pow( (yhit-y_cir),2) );
  return dist;
}
//double HoughTrack::InterSect(const HoughRecHit& hit){
//  double xeast = _rechit->getEastPoint().x();
//  double xwest = _rechit->getWestPoint().x();
//  double yeast = _rechit->getEastPoint().y();
//  double ywest = _rechit->getWestPoint().y();
//  double k = (ywest-yeast)/(xwest-xeast);  
//  double b = -k*xeast+yeast;
//  double drift = _rechit->getDriftDist();                   
//  double x_cir=_Hough2D.getCirX();
//  double y_cir=_Hough2D.getCirY();
//  double r_cir=_Hough2D.getCirR();
//}

std::pair<double,double> HoughTrack::calcuArcTrack(const HoughRecHit& hit){
  double xhit =hit.getMidX();
  double yhit =hit.getMidY();
  double x_cir=_centerX;
  double y_cir=_centerY;
  double r_cir=_centerR;
  std::pair<double,double> theta_l;

  double theta_temp;
  double l_temp;
  if(x_cir==0||xhit-x_cir==0){
	theta_temp=0;
  }
  else{
	theta_temp=M_PI-atan2(yhit-y_cir,xhit-x_cir)+atan2(y_cir,x_cir);
	if(theta_temp>2*M_PI){
	  theta_temp=theta_temp-2*M_PI;
	}
	if(theta_temp<0){
	  theta_temp=theta_temp+2*M_PI;
	}
  }
  if(_charge==-1) {
	l_temp=r_cir*theta_temp;
  }
  if(_charge==1) {
	theta_temp=2*M_PI-theta_temp;
	l_temp=r_cir*(theta_temp);
  }
  theta_l.first=theta_temp;
  theta_l.second=l_temp;
  return theta_l;
}

int HoughTrack::calzs(){
  int n_zs=0;
  for( int i =0; i<_recHitVec.size(); i++){
	if ( _recHitVec[i].getSlayerType() ==0 || _recHitVec[i].getflag()!=0 ) continue;
	//	if ( _recHitVec[i].getLayerId()>=8) continue;
	HoughStereo zs( _bunchTime, &_Hough2D, &(_recHitVec[i]) );
	//	cout<<"("<<_recHitVec[i].getLayerId()<<","<<_recHitVec[i].getWireId()<<")  stat "<<stat<<" style "<<_recHitVec[i].getStyle() <<endl;
	//	cout<<endl;

	//zs.setAmb(-1);   //right
	//int stat_left = zs.cald();

	//zs.setAmb(1);    //left
	//int stat_right= zs.cald();
	//if( stat_left==-1  &&  stat_right==-1 )  _recHitVec[i].setflag(-999);

	int stat= zs.cald();
	_recHitVec[i].setnsol(stat);
	if( stat==0 )  _recHitVec[i].setflag(-999);

	zs.setRecHit();
	_recHitVec[i].setAmb(-999); // check

	if( _recHitVec[i].getLayerId()<8 ) n_zs++;
	if(m_debug>0) zs.print();
  }
  //    int naddStero = cutNoise_inner();
  //return naddStero;
  return n_zs;
}
void HoughTrack::fitzs(){
  HoughZsFit zsfit(&_recHitVec);
  _tanl=zsfit.getTanl();
  _z0=zsfit.getZ0();
  _tanl_zs=zsfit.getTanl();
  _z0_zs=zsfit.getZ0();
  t_pro_correct = zsfit.getPro();
  if(m_debug>0) printRecHit();
}

int HoughTrack::getHitNum(int select ) const{
  int size=_recHitVec.size();
  int n0,n1,n2,n3,n4,n5,n6;
  n0=n1=n2=n3=n4=n5=n6=0;
  for(int i=0;i<size;i++){
	int cir=   _recHitVec[i].getCirList();
	int index = _recHitVec[i].digi()->getTrackIndex();
	int style=  _recHitVec[i].getStyle();
	n0++;
	if( style == 1 ) n1++;
	if( style == 2 ) n2++;
	if( index < 0 )  n3++;
	if( index >=0 && cir<=1 && style==0)  n4++;
	if( index <0 || cir>1 )   n5++;
	if( index >=0 && cir==0 && style==0)  n6++;
  }
  if( select == 0 ) return n0;
  if( select == 1 ) return n1;
  if( select == 2 ) return n2;
  if( select == 3 ) return n3;
  if( select == 4 ) return n4;
  if( select == 5 ) return n5;
  if( select == 6 ) return n6;
}
int HoughTrack::getHitNumA(int select ) const{
  int size=_recHitVec.size();
  int n0,n1,n2,n3,n4,n5,n6;
  n0=n1=n2=n3=n4=n5=n6=0;
  for(int i=0;i<size;i++){
	int cir=   _recHitVec[i].getCirList();
	int index = _recHitVec[i].digi()->getTrackIndex();
	int type =  _recHitVec[i].getSlayerType();
	int style=  _recHitVec[i].getStyle();
	if( type ==0 ) n0++;
	if( type==0 && style == 1 ) n1++;
	if( type==0 && style == 2 ) n2++;
	if( type==0 && index < 0 )  n3++;
	if( type==0 && index >=0 && cir<=1 && style==0 )  n4++;
	if( type==0 && (index <0 || cir>1) )   n5++;
	if( type==0 && index >=0 && cir==0 && style==0 )  n6++;
  }
  if( select == 0 ) return n0;
  if( select == 1 ) return n1;
  if( select == 2 ) return n2;
  if( select == 3 ) return n3;
  if( select == 4 ) return n4;
  if( select == 5 ) return n5;
  if( select == 6 ) return n6;
}
int HoughTrack::getHitNumS(int select ) const{
  int size=_recHitVec.size();
  int n0,n1,n2,n3,n4,n5,n6;
  n0=n1=n2=n3=n4=n5=n6=0;
  for(int i=0;i<size;i++){
	int cir=   _recHitVec[i].getCirList();
	int index = _recHitVec[i].digi()->getTrackIndex();
	int type =  _recHitVec[i].getSlayerType();
	int style=  _recHitVec[i].getStyle();
	if( type !=0 ) n0++;
	if( type!=0 && style == 1 ) n1++;
	if( type!=0 && style == 2 ) n2++;
	if( type!=0 && index < 0 )  n3++;
	if( type!=0 && index >=0 && cir<=1 && style==0)  n4++;
	if( type!=0 && (index <0 || cir>1) )   n5++;
	if( type!=0 && index >=0 && cir==0 && style==0)  n6++;
  }
  if( select == 0 ) return n0;
  if( select == 1 ) return n1;
  if( select == 2 ) return n2;
  if( select == 3 ) return n3;
  if( select == 4 ) return n4;
  if( select == 5 ) return n5;
  if( select == 6 ) return n6;
}

int HoughTrack::judgeTrack(){
  // hit number test 
  int axialHit=0;
  //  int stereoHit=0;
  //  int innerHit=0;
  vector<int> vec_layer;
  for( int i =0;i<_recHitVec.size();i++){
	int layer = _recHitVec[i].getLayerId();	
	int slant= _recHitVec[i].getSlayerType();	
	if( 0==slant ) axialHit++;
	//	if( 0!=slant ) stereoHit++;
	//	if( layer<8 ) innerHit++;
  }
  //  if ( axialHit <5 || stereoHit <3 || innerHit ==0 ) return 0;
  if ( axialHit <5 ) return 0;
  else return 1;
  // // continues hit test
  // bool continues=false;
  // for(int i =0;i<vec_layer.size();i++){
  //   int layer = vec_layer[i];
  //   vector<int>::iterator iter = find( vec_layer.begin(),vec_layer.end(),layer+1 );		
  //   if( iter != vec_layer.end() ){
  //     vector<int>::iterator iter2 = find( vec_layer.begin(),vec_layer.end(),layer-1 );		
  //     if( iter2 != vec_layer.end() )  continues = true;
  //   }
  // }
}

int HoughTrack::find_stereo_hit(){
  vector<HoughRecHit> vec_rec;    // store hit temp
  int naddStero=0;
  for(int ihit=0;ihit<_houghList.size();ihit++){
	const HoughHit hit= _houghList[ihit];
	if ( hit.getSlayerType()==0 ) continue;
	if ( hit.driftTime(_bunchTime,0)>800 ) continue;
	HoughRecHit p_hit ( hit,0,0,1);
	p_hit.setPtr2D(&_Hough2D);
	std::pair<double,double> theta_l = calcuArcTrack( p_hit ); 
	double dist=calcuDistToTrack( p_hit );
	double distToCir=calcuDistToCir( p_hit );
	int flag = judge_half( p_hit );
	int layer = p_hit.getLayerId();
	int wire= p_hit.getWireId();
	double disCut;
	if(layer<8) disCut=4;
	else disCut=6;
	//	double dist=sqrt( pow( (xhit-x_cir),2)+pow( (yhit-y_cir),2) )-r_cir;
	//	cout<<"find stereo hit dist?("<<layer<<" ,"<<wire<<" ) "<<dist<<endl;
	if( m_debug >0 )cout<<"("<<layer<<","<<wire<<") "<<" rec hit dist theta "<<dist<<"  "<< theta_l.first<<endl;
	if( fabs(dist) < disCut ) {
	  p_hit.setDisToTrack(dist);
	  p_hit.setDisToCir(dist);
	  p_hit.setRet(theta_l);
	  p_hit.setflag(flag);
	  //  if( p_hit.getflag()==0 ) vec_rec.push_back(p_hit); 
	  //	  vec_rec.push_back(p_hit); 
	  if( p_hit.getflag()!=0 )  continue;
	  _recHitVec.push_back(p_hit); 
	  naddStero++;
	}
  }
  return naddStero;
}

int HoughTrack::cut_axial_inner(){
  for( int ilay=0;ilay<43;ilay++){
	//if( (ilay<7&&ilay<20) || ilay>35 ) continue;
	if ((ilay>=0&&ilay<=7)||(ilay>=20&&ilay<=35)) continue;	
	//  for( int ilay=0;ilay<8;ilay++)
	double disToCir=9999;
	int max=-999;
	int count=0;
	int count_cut=0;
	for( int ihit=0;ihit<_recHitVec.size();ihit++){
	  if( _recHitVec[ihit].getLayerId()==ilay && _recHitVec[ihit].getflag()==0 ) {
		count++;
		if( (fabs(_recHitVec[ihit].getDisToCir())<disToCir) ) {
		  disToCir= fabs(_recHitVec[ihit].getDisToCir()); max=ihit;
		  count_cut=1;
		}
	  }
	}
	if( m_debug >0 ) cout<<"ilay count count_cut "<<ilay<<" "<<count<<" "<<count_cut<<endl;
	if( count_cut !=0 && count>1 ) _recHitVec[max].setflag(-999);
	if( m_debug >0 && count_cut!=0 && count>1) cout<<"delete ("<<_recHitVec[max].getLayerId()<<","<<_recHitVec[max].getWireId()<<")"<<endl;
  }
  int size_of_axial;
  for( int ihit=0;ihit<_recHitVec.size();ihit++){
	if(_recHitVec[ihit].getSlayerType()==0 && _recHitVec[ihit].getflag()==0 )  size_of_axial++;
  }
  return size_of_axial;
}

int HoughTrack::cutNoise_inner(){
  //  for( int ilay=0;ilay<43;ilay++)
  //	if( (ilay>7&&ilay<20) || ilay>35 ) continue;
  for( int ilay=0;ilay<8;ilay++){
	double z_0=0;
	double z_1=0;
	int min=-999;
	int count=0;
	int count_cut=0;
	//	cout<<" size "<<_recHitVec.size()<<endl;
	for( int ihit=0;ihit<_recHitVec.size();ihit++){
	  if( _recHitVec[ihit].getLayerId()==ilay && _recHitVec[ihit].getflag()==0) {
		count++;
		if( ((fabs(_recHitVec[ihit].getzAmb(0))>z_0) && (fabs(_recHitVec[ihit].getzAmb(1)) > z_1)) ) {
		  if( z_0!=0 && z_1!=0 )  count_cut++;
		  z_0= fabs(_recHitVec[ihit].getzAmb(0));z_1= fabs(_recHitVec[ihit].getzAmb(1)) ; min=ihit;
		}
		else if( ((fabs(_recHitVec[ihit].getzAmb(0))<z_0) && (fabs(_recHitVec[ihit].getzAmb(1)) < z_1)) ) {
		  count_cut++;
		}
	  }
	}
	if( m_debug >0 ) cout<<"ilay count count_cut "<<ilay<<" "<<count<<" "<<count_cut<<endl;
	if( count_cut>0 && count>1 ) _recHitVec[min].setflag(-999);
	if( m_debug >0 && count_cut>0 && count>1) cout<<"delete ("<<_recHitVec[min].getLayerId()<<","<<_recHitVec[min].getWireId()<<")"<<endl;
  }
  int size_of_stereo;
  for( int ihit=0;ihit<_recHitVec.size();ihit++){
	if(_recHitVec[ihit].getSlayerType()!=0 && _recHitVec[ihit].getflag()==0 )  size_of_stereo++;
  }
  return size_of_stereo;
}

void HoughTrack::printRecHit() const{
  cout<<"print rec hit"<<endl;
  double rho =  _centerPeak.getRho();
  double theta= _centerPeak.getTheta();
  int size=_recHitVec.size();
  for(int i=0;i<size;i++){
	int layer= _recHitVec[i].getLayerId();
	int wire = _recHitVec[i].getWireId();
	int slant= _recHitVec[i].getSlayerType();
	int flag= _recHitVec[i].getflag();
	int style= _recHitVec[i].getStyle();
	int cirlist= _recHitVec[i].getCirList();
	if( slant ==0 ) std::cout<<"axial hit ("<<layer<<","<<wire<<") "<<_recHitVec[i].getDisToTrack()<<" "<<_recHitVec[i].getDisToCir()<<" "<<flag<<" "<<style<<" "<<cirlist<<std::endl;
  }
  for(int i=0;i<size;i++){
	int layer= _recHitVec[i].getLayerId();
	int wire = _recHitVec[i].getWireId();
	int slant= _recHitVec[i].getSlayerType();
	int flag= _recHitVec[i].getflag();
	int style= _recHitVec[i].getStyle();
	int cirlist= _recHitVec[i].getCirList();
	if( slant !=0 ) std::cout<<"stereo hit ("<<layer<<","<<wire<<") "<<_recHitVec[i].getDisToTrack()<<" "<<flag<<" "<<style<<" "<<cirlist<<std::endl;
  }
}

int HoughTrack::trackCharge2D(){
  int n_neg(0);
  int n_pos(0);
  for(int ihit=0;ihit<_recHitVec.size();ihit++){
	double xhit =_recHitVec[ihit].getMidX();
	double yhit =_recHitVec[ihit].getMidY();
	double x_cir=_Hough2D.getCirX();
	double y_cir=_Hough2D.getCirY();
	double r_cir=_Hough2D.getCirR();
	if( (x_cir*yhit - y_cir*xhit >=0) ) n_neg++;
	if( (x_cir*yhit - y_cir*xhit <=0) ) n_pos++;
  }
  if( m_debug >0 ) cout<<" in track charge 2d "<<n_neg<<" "<<n_pos<<endl;
  if( (_charge==-1&&n_neg<3) || (_charge==1&&n_pos<3)  ) return 0;
  else if( _charge ==0 ) cout<<" wrong ! in track charge 2D not set charge "<<endl;
  else return 1;
}

int HoughTrack::trackCharge3D(){
  int n_neg(0);
  int n_pos(0);
  for(int ihit=0;ihit<_recHitVec.size();ihit++){
	if(_recHitVec[ihit].getSlayerType()==0 ) continue;
	if(_recHitVec[ihit].getLayerId()>7 ) continue;
	double xhit =_recHitVec[ihit].getMidX();
	double yhit =_recHitVec[ihit].getMidY();
	double x_cir=_Hough2D.getCirX();
	double y_cir=_Hough2D.getCirY();
	double r_cir=_Hough2D.getCirR();
	if( (x_cir*yhit - y_cir*xhit >=0) ) n_neg++;
	if( (x_cir*yhit - y_cir*xhit <=0) ) n_pos++;
  }
  if( m_debug >0 ) cout<<" in track charge 3d "<<n_neg<<" "<<n_pos<<endl;
  if( (_charge==-1&&n_neg<2) || (_charge==1&&n_pos<2)  ) return 0;
  else if( _charge ==0 ) cout<<" wrong ! in track charge 3D not set charge "<<endl;
  else return 1;
}
void HoughTrack::Leastfit(vector<double> u,vector<double> v,double& k ,double& b){
  int N = u.size();
  if (N<=2) return;
  double *x=new double[N];
  double *y=new double[N];
  for(int i=0;i<N;i++){
	x[i]=u[i];
	y[i]=v[i];
  }

  TF1 *fpol1=new TF1("fpol1","pol1",-50,50);
  TGraph *tg=new TGraph(N,x,y);
  tg->Fit("fpol1","QN");
  double ktemp  =fpol1->GetParameter(1);
  double btemp  =fpol1->GetParameter(0);
  k = ktemp;
  b = btemp;
  delete []x;
  delete []y;
  delete fpol1;
  delete tg;
}
void HoughTrack::cald_layer(){
  //in truth 
  double k,b,theta,rho,x_cross,y_cross;
  vector<double> vtemp,utemp;
  std::vector<HoughRecHit>::iterator iter = _recHitVec.begin();
  for(int iHit=0;iter!= _recHitVec.end(); iter++,iHit++){
	const HoughRecHit h = (*iter);
	//	if( h.getCirList()!=0 ) continue;
	if ( h.digi()->getTrackIndex()>=0 && h.getStyle()==0&& h.getSlayerType()==0 && h.getCirList()==0 && utemp.size()<10)    // ??use 2nd half
	{
	  utemp.push_back(h.getUTruth());
	  vtemp.push_back(h.getVTruth());
	}
  }
  Leastfit(utemp,vtemp,k,b);
  //calcu truth 
  //k,b from truth
  x_cross = -b/(k+1/k);
  y_cross = b/(1+k*k);
  rho=sqrt(x_cross*x_cross+y_cross*y_cross);
  theta=atan2(y_cross,x_cross);
  //
  //cout<<"track center position "<<_centerX<<" "<<_centerY<<endl;
  std::vector<HoughRecHit>::iterator iter0= _recHitVec.begin();
  for(; iter0!= _recHitVec.end(); iter0++){
	HoughRecHit *hit = &(*iter0);
	if( hit->getSlayerType()!=0 ) continue;
	//	if( hit->getCirList()!=0 ) continue;        // use in learn distribute
	//double cirr_hit = hit->getDriftDistTruth();
	double cirx_hit = hit->getMidX();
	double ciry_hit = hit->getMidY();
	double cirr_hit = hit->getDriftDist();
	double l1l2 = sqrt( (cirx_hit-_centerX)*(cirx_hit-_centerX)+(ciry_hit-_centerY)*(ciry_hit-_centerY) );
	double deltaD ;
	if( l1l2>_centerR )   deltaD = l1l2-_centerR-cirr_hit;
	if( l1l2<=_centerR )  deltaD = l1l2-_centerR+cirr_hit;
	hit->setDeltaD(deltaD);
	//cal flight length

	double theta_temp;
	double l_temp;
	if(_centerX==0||cirx_hit-_centerX==0){
	  theta_temp=0;
	}
	else{
	  theta_temp=M_PI-atan2(ciry_hit-_centerY,cirx_hit-_centerX)+atan2(_centerY,_centerX);
	  if(theta_temp>2*M_PI){
		theta_temp=theta_temp-2*M_PI;
	  }
	  if(theta_temp<0){
		theta_temp=theta_temp+2*M_PI;
	  }
	}
	//cout<<" charge "<<_charge <<" "<<theta_temp<<endl;
	if(_charge==-1) {
	  l_temp=_centerR*theta_temp;
	}
	if(_charge==1) {
	  theta_temp=2*M_PI-theta_temp;
	  l_temp=_centerR*(theta_temp);
	}
	//	cout<<"("<<hit->getLayerId()<<","<<hit->getWireId()<<") "<<l_temp<<endl;
	hit->setFltLen(l_temp);

	//	  cout<<"int map deltaD: ("<<hit->getLayerId()<<","<<hit->getWireId()<<")"<< hit->getDeltaD()<<endl;
  }
}
int HoughTrack::find_pair_hit(){
  int nster1=0;
  int nster2=0;
  int nster3=0;
  int naxial1=0;
  int naxial2=0;
  int naxial3=0;
  for(int ihit=0;ihit<_houghList.size();ihit++){
	const HoughHit hit= _houghList[ihit];
	if ( hit.driftTime(_bunchTime,0)>1000 ) continue;
	if ( hit.getLayerId()>=24 ) continue;
	HoughRecHit p_hit ( hit,0,0,1);
	p_hit.setPtr2D(&_Hough2D);
	std::pair<double,double> theta_l = calcuArcTrack( p_hit ); 
	double dist=calcuDistToTrack( p_hit );
	double distToCir=calcuDistToCir( p_hit );
	int flag = judge_half( p_hit ); 
	int layer = p_hit.getLayerId();
	int wire= p_hit.getWireId();
	int slayer= p_hit.getSlayerType();
	double disCut;
	if( slayer == 0 ) disCut = 6; 
	else {
	  if(layer<8) disCut=6;
	  else disCut=6;
	}
	if( m_debug >0 )cout<<"("<<layer<<","<<wire<<") "<<" pair dist flag "<<dist<<"  "<< flag<<endl;
	if( fabs(dist) < disCut ) {
	  if( flag !=1 )  continue;            //find hit in the 2nd half
	  if ( layer<4) nster1++;
	  else if ( layer<8) nster2++;
	  else if ( layer<12) naxial1++;
	  else if ( layer<16) naxial2++;
	  else if ( layer<20) naxial3++;
	  else  nster3++;
	}
  }
  if(m_debug >0){
	cout <<"naxial_1  "<<naxial1<<endl;
	cout <<"naxial_2  "<<naxial2<<endl;
	cout <<"naxial_3  "<<naxial3<<endl;
	cout <<"stereo_1  "<<nster1<<endl;
	cout <<"stereo_2  "<<nster2<<endl;
	cout <<"stereo_3  "<<nster3<<endl;
  }
  if (nster1>=2 && nster2>=2 && nster3>=2 && naxial1>=2 && naxial2>=2 && naxial3>=2 ) return 1;
  else return 0;
}
void HoughTrack::print(){
  cout<<"print HoughTrack : "<<p_trk->id()<<endl;
  cout<<"par : "<<_d0<<","<<_omega<<","<<_phi0<<","<<_z0<<","<<_tanl<<", pt: "<<_pt3D<<endl;
  TrkHitList* qhits = p_trk->hits();
  TrkHotList::hot_iterator hotIter= qhits->hotList().begin();
  int lay=((MdcHit*)(hotIter->hit()))->layernumber();
  int wir=((MdcHit*)(hotIter->hit()))->wirenumber();
  while (hotIter!=qhits->hotList().end()) {
	cout <<"(" <<((MdcHit*)(hotIter->hit()))->layernumber()
	  <<","<<((MdcHit*)(hotIter->hit()))->wirenumber() 
	  <<":"<<hotIter->isActive()<<")  ";
	cout << "nuse "<<hotIter->hit()->nUsedHits()<<endl;

	hotIter++;
  }
}
