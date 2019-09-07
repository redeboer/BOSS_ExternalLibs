#include "MdcHoughFinder/HoughMap.h"
#include "MdcHoughFinder/HoughPeak.h"
#include <vector>
#include <algorithm>
#include <cmath>

int HoughMap::m_debug=0;
int HoughMap::m_useHalfCir=0;
int HoughMap::m_N1;
int HoughMap::m_N2;

HoughMap::HoughMap(){
  //_mapHitList=NULL;
  _houghSpace=NULL;
}

HoughMap::HoughMap(int charge,HoughHitList &houghHitList,int mapHit ,int ntheta,int nrho,double rhoMin,double rhoMax,int peakWidth,int peakHigh,double hitpro){
  _hitList=houghHitList;
  _mapHit=mapHit;
  _nTheta=ntheta;
  _nRho=nrho;
  _thetaMin=0;
  _thetaMax=M_PI;
  _rhoMin=rhoMin;
  _rhoMax=rhoMax;
  _peakWidth=peakWidth;
  _peakHigh=peakHigh;
  _hitpro=hitpro;
  _charge=charge;

  //_mapHitList = new vector<const HoughHit*>*[_nTheta];
  //_houghS=  new double*[_nTheta];
  //_houghS2=  new double*[m_N2];
//  _houghR=  new double*[_nTheta];
//  _houghRL=  new double*[_nTheta];
//  _houghNL=  new double*[_nTheta];

  for(int i=0; i<_nTheta; i++){ 
	//_mapHitList[i] = new vector<const HoughHit*>[_nRho];
//	_houghS[i] = new double[_nRho];
//	_houghR[i] = new double[_nRho];
//	_houghRL[i] = new double[_nRho];
//	_houghNL[i] = new double[_nRho];
//	for(int j=0; j<_nRho; j++){ 
//	  _houghS[i][j]=0;
//	  _houghR[i][j]=0;
//	}
  }
//  for(int i=0; i<m_N2; i++){ 
//    _houghS2[i] = new double[m_N2];
//    for(int j=0; j<m_N2; j++){ 
//      _houghS2[i][j]=0;
//    }
//  }

  if(_charge==-1) _houghSpace = new TH2D("houghspace","houghspace",_nTheta,_thetaMin,_thetaMax,_nRho,_rhoMin,_rhoMax);
  if(_charge==1 ) _houghSpace = new TH2D("houghspace2","houghspace2",_nTheta,_thetaMin,_thetaMax,_nRho,_rhoMin,_rhoMax);
 //  _houghR= new TH2D("houghR","houghR",_nTheta,_thetaMin,_thetaMax,_nRho,_rhoMin,_rhoMax);

  doMap();
}
HoughMap::HoughMap(const HoughMap& other){
  _mapHit  =other._mapHit,
		   _nTheta  =other._nTheta,
		   _nRho    =other._nRho,
		   _thetaMin=other._thetaMin,
		   _thetaMax=other._thetaMax,
		   _rhoMin  =other._rhoMin,
		   _rhoMax  =other._rhoMax,
		   _hitList =other._hitList,
		   _houghPeakList=other._houghPeakList,
		   _houghTrackList=other._houghTrackList,
		   _houghSpace = other._houghSpace,
		   _charge = other._charge;
		   //_houghR= other._houghR;

		   //_mapHitList = new vector<const HoughHit*>*[_nTheta];
		   //_houghNL= new double*[_nTheta];
		   //_houghRL= new double*[_nTheta];
		   //_houghS= new double*[_nTheta];
		   //for(int i=0; i<_nTheta; i++){ 
		   //_mapHitList[i] = new vector<const HoughHit*>[_nRho];
		   //	_houghNL[i] = new double[_nRho];
		   //	_houghRL[i] = new double[_nRho];
		   //_houghS[i] = new double[_nRho];
		   //}

		   //for(int i=0; i<_nTheta; i++){ 
		   //	for(int j=0; j<_nRho; j++){ 
		   //_mapHitList[i][j]=other._mapHitList[i][j];
		   //	  _houghNL[i][j]=other._houghNL[i][j];
		   //	  _houghRL[i][j]=other._houghRL[i][j];
		   //	  _houghS[i][j]=other._houghS[i][j];
		   //	}
		   // }
}

void HoughMap::doMap(){
  buildMap();
  //mapDev();
  //cald_layer();
  //select_slant();
  //gravity();
  //findPeaks();
  //if(m_debug>0)  {cout<<" before sort "<<endl;  printPeak();}
  // {cout<<" before sort "<<endl;  printPeak();} 
  sortPeaks();
  hitFinding();
  if(m_debug>0)  { cout<<" after sort "<<endl;  printPeak();}
  trackFinder();
  //  {cout<<" after sort "<<endl;  printPeak();} 
  // candiTrack();
  if(m_debug>0) printTrack();
}

void HoughMap::clearMap(){

  //  for(int i=0; i<_nTheta; i++){ delete []_mapHitList[i];delete []_houghNL; delete []_houghRL;delete []_houghS;}
  //  for(int i=0; i<_nTheta; i++){ delete []_mapHitList[i]; }
  // delete []_mapHitList;
  delete _houghSpace;
  //  for(int i=0; i<_nTheta; i++){ delete []_houghS[i];}
  //  delete []_houghS;
  //  delete _houghR;
  //  for(int i=0; i<m_N2; i++)   { delete []_houghS2[i];}
  //  delete []_houghS2;
}
void HoughMap::printPeak(){
  cout<<"Print Peak in HoughMap sumPeak: "<<_houghPeakList.size()<<endl;
  vector<HoughPeak>::iterator iter =_houghPeakList.begin();
  for(int i=0;iter!=_houghPeakList.end();iter++,i++){
	cout<<"count of Peak on HoughMap: "<<(*iter).getHoughHitList().size()<<endl;
	(*iter).printAllHit();
  }
}

HoughMap::~HoughMap(){
  clearMap();
}
void HoughMap::buildMap(){
  if(m_debug>0) cout<<"in build map "<<endl;
  //loop over all hits, fill histogram of HoughRec space
  //std::vector<HoughHit>::const_iterator iter = _hitList.getList().begin();
  std::vector<HoughHit>::iterator iter = _hitList.getList().begin();
  for(; iter!= _hitList.getList().end(); iter++){
	HoughHit *hit = &(*iter);
	if( _mapHit==1 && hit->getSlayerType()!=0  ) continue;
	if( hit->driftTime()>1000 && hit->driftTime()<0 ) continue;              //to big hit ,maybe error hit
	double drift_CF = hit->getR();
	hit->makeCir(2*_nTheta,0,2*M_PI,drift_CF);   //N2
	for( int ni=0;ni<2*m_N1;ni++){   //N1
	  double binwidth = M_PI/_nTheta;
	  double binhigh = (_rhoMax-_rhoMin)/_nRho;
	  double theta = hit->getCir(ni).getTheta();
	  double rho = hit->getCir(ni).getRho();
	  if( fabs(rho) >=_rhoMax ) continue;
	  double slantOfLine = hit->getCir(ni).getSlant(); 
	  int chargeOfHitOnCir = (slantOfLine/fabs(slantOfLine)) * (rho/fabs(rho));
	  if( chargeOfHitOnCir == _charge) continue;  
	  if( fabs(slantOfLine)<0.03 ) continue;      //FIXME
	  _houghSpace->Fill(theta,rho);
	  //	  int rhobinNum = (int)( (rho-_rhoMin)/((_rhoMax-_rhoMin)/m_N1) );
	  //	  int thetabinNum = (int)( (theta-_thetaMin)/((_thetaMax-_thetaMin)/m_N1) );
	  //	  _mapHitList[thetabinNum][rhobinNum].push_back(hit) ;
	}
	//break;  //only first line
  }
  //initialize
  //  double hist[1000][1000] ={0};
  //  int max =0 ;
  double aver(0),sigma(0);
  mapDev(_houghSpace,aver,sigma);
//  cout<<"aver sigma aver+10sig "<<aver<<" "<<sigma<<" "<<aver+10*sigma<<endl;
  for (int iTheta=0; iTheta<_nTheta; iTheta++) {
	for (int iRho=0; iRho<_nRho; iRho++) {
	  int	 z=_houghSpace->GetBinContent(iTheta+1,iRho+1);
	  double thetabin = _houghSpace->GetXaxis()->GetBinCenter(iTheta+1);
	  double rhobin = _houghSpace->GetYaxis()->GetBinCenter(iRho+1);
	  double pt = (1/rhobin)/333.567;
	  int    MINCOUNT = 10;   //num at least consider exist a track
	  if(0.06<=pt&&pt<0.07 ) MINCOUNT = 6;
	  if(pt<0.06) MINCOUNT = 5;
	  double minCount_Cut=( aver+4*sigma >MINCOUNT)? (aver+4*sigma):MINCOUNT;  //  4 temp par
	  //	  _houghS[iTheta][iRho]=z;
	  //if( z>=MINCOUNT) loopPeak(thetabin,rhobin,iTheta,iRho,hist);
	  //if( z>=MINCOUNT) loopPeak(thetabin,rhobin,iTheta,iRho);
	  if( z>=minCount_Cut) loopPeak(thetabin,rhobin,iTheta,iRho);
	  //	  if( max <z ) max = z ;
	}
  }
  // MAX = max;
  //  if(m_debug>0) printMapHit();

}
/*
   void HoughMap::printMapHit(){
   double aver(0),sigma(0);
   mapDev(_houghSpace,aver,sigma);
//cout<<"AVER SIGMA "<<aver<<" "<<sigma<<endl;
//cout<<"print Map Hit : "<<_charge<<endl;
for(int i =0;i<m_N1;i++){
for(int j =0;j<m_N1;j++){
if( _mapHitList[i][j].size()>=5 ) cout<<"map :("<<i<<","<<j<<") :"<<_mapHitList[i][j].size()<<endl;
else continue;
for(int k =0;k<_mapHitList[i][j].size();k++){
cout<<"Hit: ("<<_mapHitList[i][j][k]->getLayerId()<<","<<_mapHitList[i][j][k]->getWireId()<<")"<<endl;
}
}
}
}
*/


//find local maximum in hough space between 8 neighbor cells
void HoughMap::findPeaks(vector< vector<int> > vec_hist,double theta_left,double theta_right,double rho_down,double rho_up){
  int minCount_Cut;
  double rho_peak = 1./2.*(rho_down+rho_up);
  double pt_peak = (1./rho_peak)/333.567;
  if (0.06<=pt_peak&&pt_peak<0.07 )  minCount_Cut = 4; 
  if (pt_peak <0.06 )  minCount_Cut = 3; 
  else minCount_Cut = 5;
  double aver(0),sigma(0);
  //mapDev(vec_hist,aver,sigma);
  //int minCount_Cut=( aver+3*sigma> 5)?( aver+3*sigma):5;
  //cout<<"aver: "<<aver<<" "<<sigma<<endl;
  int _nTheta = m_N2;
  int _nRho= m_N2;

  // store peak property
  // 0: not a peak, 
  // 1: same hight with neighbor cells, 
  // 2: highest peak according to neighbor 8 cells
  int **hough_trans_CS_peak = new int*[_nTheta];
  for(int i=0; i<_nTheta; i++){ 
	hough_trans_CS_peak[i] = new int[_nRho];
	for(int j=0; j<_nRho; j++){ hough_trans_CS_peak[i][j]=-999;} 
  }

  //loop over hough space
  for (int iRho=0; iRho<_nRho; iRho++) {
	for (int iTheta=0; iTheta<_nTheta; iTheta++) {
	  //int height = houghSpace->GetBinContent(iTheta+1,iRho+1);
	  int height = vec_hist[iTheta][iRho];
	  if(height < minCount_Cut) {
		hough_trans_CS_peak[iTheta][iRho]=0; 
		continue;
	  }
	  //loop over around bins
	  int max_around=0;            
	  for (int ar=iTheta-1; ar<=iTheta+1; ar++) {
		for (int rx=iRho-1; rx<=iRho+1; rx++) {
		  int ax;
		  if (ar<0) { ax=ar+_nTheta; }
		  else if (ar>=_nTheta) { ax=ar-_nTheta; }
		  else { ax=ar; }
		  if ( (ax!=iTheta || rx!=iRho) && rx>=0 && rx<_nRho) {
			//		int heightThisBin =houghSpace->GetBinContent(ax+1,rx+1);
			int heightThisBin = vec_hist[ax][rx];
			if (heightThisBin>max_around) { max_around=heightThisBin; }
		  }
		}
	  }
	  if (height<max_around) { hough_trans_CS_peak[iTheta][iRho]=0; }
	  else if (height==max_around) { hough_trans_CS_peak[iTheta][iRho]=1; }
	  else if (height>max_around) { hough_trans_CS_peak[iTheta][iRho]=2; }
	}
  }
  //  mergeNeighbor(hough_trans_CS_peak,theta_left,theta_right,rho_down,rho_up);
  //merge 
  int peakNum_Merge=0;
  for (int iRho=0; iRho<_nRho; iRho++) {
	for (int iTheta=0; iTheta<_nTheta; iTheta++) {
	  if (hough_trans_CS_peak[iTheta][iRho]==1) {
		hough_trans_CS_peak[iTheta][iRho]=2;
		for (int ar=iTheta-1; ar<=iTheta+1; ar++) {
		  for (int rx=iRho-1; rx<=iRho+1; rx++) {
			int ax;
			if (ar<0) { ax=ar+_nTheta; }
			else if (ar>=_nTheta) { ax=ar-_nTheta; }
			else { ax=ar; }
			if ( (ax!=iTheta || rx!=iRho) && rx>=0 && rx<_nRho) {
			  if (hough_trans_CS_peak[ax][rx]==1) { hough_trans_CS_peak[ax][rx]=0; }
			}
		  }
		}
	  }
	  if(hough_trans_CS_peak[iTheta][iRho]==2) {
		int peak_num = _houghPeakList.size();
		//		_houghPeakList.push_back( HoughPeak(_houghSpace->GetBinContent(iTheta+1,iRho+1),iTheta,iRho,exTheta(iTheta,theta_left,theta_right,m_N2),exRho(iRho,rho_down,rho_up,m_N2),true,peak_num+1,_charge) );//store peak
		_houghPeakList.push_back( HoughPeak(vec_hist[iTheta][iRho],iTheta,iRho,exTheta(iTheta,theta_left,theta_right,m_N2),exRho(iRho,rho_down,rho_up,m_N2),true,peak_num+1,_charge) );//store peak
		//cout<<"height "<<vec_hist[iTheta][iRho]<<endl;
		peakNum_Merge++;
	  }
	}
  }

  // end of merge
  for(int i=0; i<_nTheta; i++){ delete []hough_trans_CS_peak[i]; }
  delete []hough_trans_CS_peak;
}
//merge "1" peaks, 1: same hight with neighbor peaks
int HoughMap::mergeNeighbor(int** hough_trans_CS_peak,double theta_left,double theta_right,double rho_down,double rho_up){
  //  _houghPeakList.reserve(100);
  int _nTheta = m_N2;
  int _nRho= m_N2;
  int peakNum_Merge=0;
  for (int iRho=0; iRho<_nRho; iRho++) {
	for (int iTheta=0; iTheta<_nTheta; iTheta++) {
	  if (hough_trans_CS_peak[iTheta][iRho]==1) {
		hough_trans_CS_peak[iTheta][iRho]=2;
		for (int ar=iTheta-1; ar<=iTheta+1; ar++) {
		  for (int rx=iRho-1; rx<=iRho+1; rx++) {
			int ax;
			if (ar<0) { ax=ar+_nTheta; }
			else if (ar>=_nTheta) { ax=ar-_nTheta; }
			else { ax=ar; }
			if ( (ax!=iTheta || rx!=iRho) && rx>=0 && rx<_nRho) {
			  if (hough_trans_CS_peak[ax][rx]==1) { hough_trans_CS_peak[ax][rx]=0; }
			}
		  }
		}
	  }
	  if(hough_trans_CS_peak[iTheta][iRho]==2) {
		int peak_num = _houghPeakList.size();
		//		_houghPeakList.push_back( HoughPeak(_houghSpace->GetBinContent(iTheta+1,iRho+1),iTheta,iRho,exTheta(iTheta,theta_left,theta_right,m_N2),exRho(iRho,rho_down,rho_up,m_N2),true,peak_num+1,_charge) );//store peak
		_houghPeakList.push_back( HoughPeak(_houghSpace->GetBinContent(iTheta+1,iRho+1),iTheta,iRho,exTheta(iTheta,theta_left,theta_right,m_N2),exRho(iRho,rho_down,rho_up,m_N2),true,peak_num+1,_charge) );//store peak
		peakNum_Merge++;
	  }
	}
  }
  return peakNum_Merge;
}

bool less_hits_in_peak(const HoughPeak& peaka,const HoughPeak& peakb){
  return peaka.peakHeight() > peakb.peakHeight();
}
void HoughMap::sortPeaks(){
  std::sort (_houghPeakList.begin(),_houghPeakList.end(),less_hits_in_peak);
}
double HoughMap::exRho(int irho,double rhomin,double rhomax,int n){
  //double rho = _rhoMin+(irho+1/2.)*(_rhoMax-_rhoMin)/_nRho;
  double rho = rhomin+(irho+1/2.)*(rhomax-rhomin)/n;
  return rho;                                                                                 
}   
double HoughMap::exTheta(int itheta,double thetamin,double thetamax,int n){
  double theta= thetamin+(itheta+1/2.)*(thetamax-thetamin)/n;  
  return theta;
}   
//int HoughMap::exRhoBin(double rho){
//  int rhobin = ( (int)(rho-_rhoMin)/((_rhoMax-_rhoMin)/_nRho) );
//  return rhobin;
//}   
//int HoughMap::exThetaBin(double theta){
//  int thetabin = (int)( (theta-_thetaMin)/((_thetaMax-_thetaMin)/_nTheta) );
//  return thetabin;
//}   

void HoughMap::candiTrack(){
  if( _houghPeakList.size()==0 ) return;
  //  cout<<"size "<<_houghPeakList.size()<<endl;
  for(int itrack=0;itrack<_houghPeakList.size();itrack++){
	if( _houghPeakList[itrack].getisCandiTrack()==true ) combineNeighbor(itrack);    //if track is true do combine 
	else    continue;

	for(int ipeak=itrack+1;ipeak<_houghPeakList.size();ipeak++){
	  if(m_debug>0) cout<<" compare track peak : "<<itrack<<" "<<ipeak<<endl;
	  compareTrack_and_Peak(_houghTrackList.back(),_houghPeakList[ipeak]);
	}
  }
}
/*
   void HoughMap::combineNeighbor(int ipeak){
//get the hitlist in the exact peak
// cout<<" ipeak "<<ipeak<<endl;
int theta=_houghPeakList[ipeak].getThetaBin();
int rho  =_houghPeakList[ipeak].getRhoBin();
vector< const HoughHit* > centerHitList = _mapHitList[theta][rho];
for (int px=theta-_peakWidth; px<=theta+_peakWidth; px++) {
for (int py=rho-_peakHigh; py<=rho+_peakHigh; py++) {
int ax;
if (px<0) { ax=px+_nTheta; }
else if (px>=_nTheta) { ax=px-_nTheta; }
else { ax=px; }
if ( (ax!=theta|| py!=rho) && py>=0 && py<_nRho) combine_two_cells(centerHitList,ax,py);
}
}
_houghTrackList.push_back( HoughTrack(_houghPeakList[ipeak],centerHitList , Rho,Theta) );
}

void HoughMap::combine_two_cells(vector< const HoughHit* > &centerHitList,int ax,int py){
//cout<<"combine ("<<ax<<","<<py<<endl;
vector< const HoughHit* > aroundHitList_xy= _mapHitList[ax][py];
int size_of_center=centerHitList.size();
int size_of_around=aroundHitList_xy.size();

for(int i=0;i<size_of_around;i++){
int same_hit=0;
for(int j=0;j<size_of_center;j++){
if( centerHitList[j]->digi()==aroundHitList_xy[i]->digi() )    {same_hit=1;break;}
}
if(same_hit==0)  {
centerHitList.push_back(aroundHitList_xy[i]);
//	cout<<"push back hit("<<aroundHitList_xy[i]->getLayerId()<<","<<aroundHitList_xy[i]->getWireId()<<")"<<endl;
}
}
}
*/

void HoughMap::compareTrack_and_Peak(HoughTrack &track,HoughPeak& peak){
  int sameHit=0;
  int sizeoftrack=track.getHoughHitList().size();
  int sizeofpeak =peak.getHoughHitList().size();
  for(int i=0;i<sizeoftrack;i++){
	for(int j=0;j<sizeofpeak;j++){
	  //  cout<<" debug digi ("<< ((track.getHoughHitList())[i])->getLayerId()<<" ,"<<((track.getHoughHitList())[i])->getWireId() <<")   ("<< ((peak.getHoughHitList())[j])->getLayerId()<<" ,"<<((peak.getHoughHitList())[j])->getWireId()<<" )"<<endl;
	  if( ((track.getHoughHitList())[i]).digi() == ((peak.getHoughHitList())[j])->digi() ) sameHit++;
	}
  }
  int rho_track=track.getRho();
  int theta_track=track.getTheta();
  int rho_peak=peak.getRho();
  int theta_peak=peak.getTheta();
  if(m_debug>0) cout<<"same hit : "<<(double)sameHit/peak.getHoughHitList().size()<<endl;
  //if( sameHit>_hitpro*peak.getHoughHitList().size() || (fabs(rho_track-rho_peak)< 0.3 && fabs(theta_track-theta_peak)< 0.3) )  
  if( sameHit>_hitpro*peak.getHoughHitList().size() ) {
	if(m_debug>0) cout <<"DEBUG peak is out "<<peak.getPeakNum()<<endl;
	peak.setisCandiTrack(false);
	//combineTrack_and_Peak(track,peak);
  }
  else if(m_debug>0) cout<<" DEBUG peak is reserve  "<<peak.getPeakNum()<<endl;
}

void HoughMap::printTrack(){
  cout<<"Print Track in HoughMap: "<<endl;
  vector<HoughTrack>::iterator iter =_houghTrackList.begin();
  for(int i=0;iter!=_houghTrackList.end();iter++,i++){
	cout<<"Print Track"<<i<<endl;
	(*iter).printRecHit();
  }
}

void HoughMap::loopPeak(double thetabin,double rhobin,int iTheta,int iRho){
  int binx,biny,binz;
  int ntheta=m_N2;
  int nrho=m_N2;
  double theta_left = thetabin-(1/2.)*(M_PI/_nTheta);
  double theta_right= thetabin+(1/2.)*(M_PI/_nTheta);
  double rho_down=  rhobin-(1/2.)*((2*_rhoMax)/_nRho);
  double rho_up =   rhobin+(1/2.)*((2*_rhoMax)/_nRho);
  // cout<<"binx biny "<<binx<<" "<<biny<<endl;
  std::vector<HoughHit>::iterator iter = _hitList.getList().begin();
  vector< vector<int> > vec_hist(nrho,vector<int>(ntheta,0) );
  for(int itheta =0;itheta<ntheta;itheta++){
	for(int irho=0;irho<nrho;irho++){
	  vec_hist[itheta][irho]=0;
	}
  }

  for(; iter!= _hitList.getList().end(); iter++){
	HoughHit *hit = &(*iter);
	if( _mapHit==1 && hit->getSlayerType()!=0 ) continue;
	if( hit->driftTime()>1000 ) continue;              //to big hit ,maybe error hit
	double drift_CF = hit->getR();

	// from 0 to M_PI
	hit->makeCir(m_N2,theta_left,theta_right,drift_CF);   //N2
	for( int ni=0;ni<m_N2;ni++){
	  double theta = hit->getCir(ni).getTheta();
	  double rho = hit->getCir(ni).getRho();
	  double slantOfLine = hit->getCir(ni).getSlant(); 
	  int chargeOfHitOnCir = (slantOfLine/fabs(slantOfLine)) * (rho/fabs(rho));
	  if( chargeOfHitOnCir == _charge ) continue;  
	  if( fabs(slantOfLine)<0.03 ) continue;  
	  //
	  if( theta<=theta_left|| theta>=theta_right) continue;
	  if( rho<=rho_down || rho>=rho_up ) continue;
	  int rhobinNum = (int)( (rho-rho_down)/((rho_up-rho_down)/m_N2) );
	  int thetabinNum = (int)( (theta-theta_left)/((theta_right-theta_left)/m_N2) );
	  vec_hist[thetabinNum][rhobinNum]++;
	  //	  hist[thetabinNum+iTheta*ntheta][rhobinNum+iRho*nrho]++;
	}

	// from M_PI to 2*M_PI
	hit->makeCir(m_N2,theta_left+M_PI,theta_right+M_PI,drift_CF);   //N2
	for( int ni=0;ni<m_N2;ni++){
	  double theta = hit->getCir(ni).getTheta();
	  double rho = hit->getCir(ni).getRho();
	  double slantOfLine = hit->getCir(ni).getSlant(); 
	  int chargeOfHitOnCir = (slantOfLine/fabs(slantOfLine)) * (rho/fabs(rho));
	  if( chargeOfHitOnCir == _charge ) continue;  
	  if( fabs(slantOfLine)<0.03 ) continue;  
	  //
	  if( theta<=theta_left|| theta>=theta_right) continue;
	  if( rho<=rho_down || rho>=rho_up ) continue;
	  int rhobinNum = (int)( (rho-rho_down)/((rho_up-rho_down)/m_N2) );
	  int thetabinNum = (int)( (theta-theta_left)/((theta_right-theta_left)/m_N2) );
	  vec_hist[thetabinNum][rhobinNum]++;
	  //	  hist[thetabinNum+iTheta*ntheta][rhobinNum+iRho*nrho]++;
	  //cout<<"("<<thetabinNum+iTheta*ntheta<<","<<rhobinNum+iRho*nrho<<"): "<<hist[thetabinNum+iTheta*ntheta][rhobinNum+iRho*nrho]<<endl;
	}
  }  //loop over hit

  findPeaks(vec_hist,theta_left,theta_right,rho_down,rho_up);
}

/*
   void HoughMap::gravity(){
   int binx,biny,binz;
   _houghSpace->GetMaximumBin(binx,biny,binz);
   double thetabin = _houghSpace->GetXaxis()->GetBinCenter(binx);
   double rhobin = _houghSpace->GetYaxis()->GetBinCenter(biny);
   int ntheta=15;
   int nrho=10;
   double theta_left = thetabin-(ntheta+1/2.)*(M_PI/_nTheta);
   double theta_right= thetabin+(ntheta+1/2.)*(M_PI/_nTheta);
   double rho_down=  rhobin-(nrho+1/2.)*((2*_rhoMax)/_nRho);
   double rho_up =  rhobin+(nrho+1/2.)*((2*_rhoMax)/_nRho);
   int NbinRho=m_N2;
   int NbinTheta=m_N2;
   std::vector<HoughHit>::iterator iter = _hitList.getList().begin();
   TH2D *_houghSpace2 = new TH2D("houghspace2","houghspace2",NbinTheta,theta_left,theta_right,NbinRho,rho_down,rho_up);  //N2
   for(; iter!= _hitList.getList().end(); iter++){
   HoughHit *hit = &(*iter);
   if( _mapHit==1 && hit->getSlayerType()!=0 ) continue;
   double drift_CF = hit->getRTruth();
   hit->makeCir(NbinTheta,theta_left,theta_right,drift_CF);   //N2
   for( int ni=0;ni<NbinTheta;ni++){
   double theta = hit->getCir(ni).getTheta();
   double rho = hit->getCir(ni).getRho();
   double slantOfLine = hit->getCir(ni).getSlant(); 
   int chargeOfHitOnCir = (slantOfLine/fabs(slantOfLine)) * (rho/fabs(rho));
   if( chargeOfHitOnCir == _charge ) continue;  
   if( fabs(slantOfLine)<0.03 ) continue;  
   _houghSpace2->Fill(theta,rho);
   }
   }
   for (int iRho=0; iRho<NbinRho; iRho++) {
   for (int iTheta=0; iTheta<NbinTheta; iTheta++) {
   int	 z5=_houghSpace2->GetBinContent(iTheta+1,iRho+1);
   _houghS2[iTheta][iRho]=z5;
   }
   }
   int binx2,biny2,binz2;
   _houghSpace2->GetMaximumBin(binx2,biny2,binz2);
   double thetabin2 = _houghSpace2->GetXaxis()->GetBinCenter(binx2);
   double rhobin2 = _houghSpace2->GetYaxis()->GetBinCenter(biny2);
   Theta=thetabin2;
   Rho=rhobin2;
   Height=_houghSpace2->GetBinContent(binx2,biny2);
   delete _houghSpace2;
   }
   */

void HoughMap::select_slant(){
  int binx,biny,binz;
  _houghSpace->GetMaximumBin(binx,biny,binz);
  double thetabin = _houghSpace->GetXaxis()->GetBinCenter(binx);
  double rhobin = _houghSpace->GetYaxis()->GetBinCenter(biny);
  vector<int> vec_layer;
  std::vector<HoughHit>::iterator iter1= _hitList.getList().begin();
  for(; iter1!= _hitList.getList().end(); iter1++){
	if( (*iter1).getSlayerType()!=0 || (*iter1).getStyle()!=0 || (*iter1).getCirList()!=0 ) continue;
	vec_layer.push_back( (*iter1).getLayerId() );
  }

  if( vec_layer.size()==0 ) return;
  vector<int>::iterator laymax=max_element( vec_layer.begin(),vec_layer.end() );
  int maxLayer=*laymax;
  m_maxlayer=maxLayer;
  // cout<<"max layer "<<maxLayer<<endl;

  vector<int>::iterator iterlay = vec_layer.begin();
  for(; iterlay!= vec_layer.end(); iterlay++){
	if ( *iterlay==*laymax) {
	  vec_layer.erase(laymax);
	  iterlay--;
	}
  }
  vector<int>::iterator laymax2=max_element( vec_layer.begin(),vec_layer.end() );
  int maxLayer2=*laymax2;
  //  cout<<"max layer2 "<<maxLayer2<<endl;

  std::vector<HoughHit>::iterator iter = _hitList.getList().begin();
  for(; iter!= _hitList.getList().end(); iter++){
	HoughHit *hit = &(*iter);
	if(  hit->getSlayerType()!=0 || hit->getStyle()!=0 || hit->getCirList()!=0 ) continue;
	double slantOfLine= hit->getV()*cos(thetabin) - hit->getU()*sin(thetabin); 
	//cout<<"("<<hit->getLayerId()<<","<<hit->getWireId()<<") "<< slantOfLine<<endl;
	if( hit->getLayerId()==maxLayer || hit->getLayerId()==maxLayer2){
	  maxlayer_slant.push_back(slantOfLine);
	}
	else{
	  nomaxlayer_slant.push_back(slantOfLine);
	  nomaxlayerid.push_back(hit->getLayerId());
	}
  }
}

void HoughMap::cald_layer(){
  //in truth 
  double k,b,theta,rho,x_cross,y_cross;
  vector<double> vtemp,utemp;
  std::vector<HoughHit>::iterator iter = _hitList.getList().begin();
  for(int iHit=0;iter!= _hitList.getList().end(); iter++,iHit++){
	const HoughHit h = (*iter);
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
  //  Rho = rho;              //use truth rho, theta
  //  Theta =theta;
  //
  double cirr_track = fabs(1./(Rho));
  double cirx_track = (1./Rho)*cos(Theta); 
  double ciry_track = (1./Rho)*sin(Theta); 
  //cout<<"track center position "<<cirx_track<<" "<<ciry_track<<endl;
  std::vector<HoughHit>::iterator iter0= _hitList.getList().begin();
  for(; iter0!= _hitList.getList().end(); iter0++){
	HoughHit *hit = &(*iter0);
	if( hit->getSlayerType()!=0 ) continue;
	//	if( hit->getCirList()!=0 ) continue;        // use in learn distribute
	//double cirr_hit = hit->getDriftDistTruth();
	double cirx_hit = hit->getMidX();
	double ciry_hit = hit->getMidY();
	double cirr_hit = hit->getDriftDist();
	double l1l2 = sqrt( (cirx_hit-cirx_track)*(cirx_hit-cirx_track)+(ciry_hit-ciry_track)*(ciry_hit-ciry_track) );
	double deltaD ;
	if( l1l2>cirr_track )   deltaD = l1l2-cirr_track-cirr_hit;
	if( l1l2<=cirr_track )  deltaD = l1l2-cirr_track+cirr_hit;
	hit->setDeltaD(deltaD);
	//cal flight length

	double theta_temp;
	double l_temp;
	if(cirx_track==0||cirx_hit-cirx_track==0){
	  theta_temp=0;
	}
	else{
	  theta_temp=M_PI-atan2(ciry_hit-ciry_track,cirx_hit-cirx_track)+atan2(ciry_track,cirx_track);
	  if(theta_temp>2*M_PI){
		theta_temp=theta_temp-2*M_PI;
	  }
	  if(theta_temp<0){
		theta_temp=theta_temp+2*M_PI;
	  }
	}
	//cout<<" charge "<<_charge <<" "<<theta_temp<<endl;
	if(_charge==-1) {
	  l_temp=cirr_track*theta_temp;
	}
	if(_charge==1) {
	  theta_temp=2*M_PI-theta_temp;
	  l_temp=cirr_track*(theta_temp);
	}
	//	cout<<"("<<hit->getLayerId()<<","<<hit->getWireId()<<") "<<l_temp<<endl;
	hit->setFltLen(l_temp);

	//	  cout<<"int map deltaD: ("<<hit->getLayerId()<<","<<hit->getWireId()<<")"<< hit->getDeltaD()<<endl;
  }
}

void HoughMap::hitFinding(){
  if(m_debug>0) cout<<"in hit finding "<<endl;
  for(int ipeak=0;ipeak<_houghPeakList.size();ipeak++){
	int hitColNum =  _houghPeakList[ipeak].collectHits(_hitList);
	if( hitColNum < 5 ) _houghPeakList[ipeak].setisCandiTrack(false);
  }
}
void HoughMap::trackFinder(){
  if(m_debug>0) cout<<"in track finder "<<endl;
  if( _houghPeakList.size()==0 ) return;
  //  cout<<"size "<<_houghPeakList.size()<<endl;
  for(int itrack=0;itrack<_houghPeakList.size();itrack++){
	if( _houghPeakList[itrack].getisCandiTrack()==true )     //if track is true do combine 
	  _houghTrackList.push_back( HoughTrack(_houghPeakList[itrack],_houghPeakList[itrack].getHoughHitList(),_houghPeakList[itrack].getRho(),_houghPeakList[itrack].getTheta() ) );
	else    continue;

	for(int ipeak=itrack+1;ipeak<_houghPeakList.size();ipeak++){
	  compareTrack_and_Peak(_houghTrackList.back(),_houghPeakList[ipeak]);
	}
  }
}
void HoughMap::Leastfit(vector<double> u,vector<double> v,double& k ,double& b){
  int N = u.size();
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
void HoughMap::mapDev(vector< vector<int> > vec_hist,double& aver ,double& sigma){
  int count_use=0;
  double sum=0;
  double sum2=0;
  for(int i=0;i<m_N2;i++){
	for(int j=0;j<m_N2;j++){
	  int count=vec_hist[i][j];
	  if(count!=0){
		count_use++;
		sum+=count;
		sum2+=count*count;
	  }
	}
  }
  aver=sum/count_use;
  sigma=sqrt(sum2/count_use-aver*aver);
}
void HoughMap::mapDev(TH2D* houghspace,double& aver ,double& sigma){
  int count_use=0;
  double sum=0;
  double sum2=0;
  for(int i=0;i<m_N1;i++){
	for(int j=0;j<m_N1;j++){
	  int count=houghspace->GetBinContent(i+1,j+1);
	  if(count!=0){
		count_use++;
		sum+=count;
		sum2+=count*count;
	  }
	}
  }
  aver=sum/count_use;
  sigma=sqrt(sum2/count_use-aver*aver);
}
