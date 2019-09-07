#include "MdcHoughFinder/HoughPeak.h"
#include<fstream>
#include<sstream>
double HoughPeak::m_dSigma_cut;
double HoughPeak::m_dSigma_cut2;
string HoughPeak::m_file;

HoughPeak::HoughPeak(){
}
HoughPeak::~HoughPeak(){
//  cout<<" HoughPeak  destruct" <<endl;
	for(int i =0;i<_houghPeakHitList.size();i++){
	//  delete _houghPeakHitList[i];
	  _houghPeakHitList[i]=NULL;
	}
	_houghPeakHitList.clear();
}
HoughPeak& HoughPeak::operator=(const HoughPeak& other)
{
  if( this == &other ) return *this;
  _height=other._height;
  _theta=other._theta;
  _rho=other._rho;
  _thetaBin=other._thetaBin;
  _rhoBin=other._rhoBin;
  _isCandiTrack=other._isCandiTrack;
  _peakNum=other._peakNum;
  _charge=other._charge;
  if(_houghPeakHitList.size() != 0 ){
	for(int i =0;i<_houghPeakHitList.size();i++){
	  _houghPeakHitList[i]=NULL;
	}
	_houghPeakHitList.clear();
  }
  _houghPeakHitList = other._houghPeakHitList;
  //for(int i =0;i<other._houghPeakHitList.size();i++){
  //  const HoughHit* p_hit= new HoughHit( *(other._houghPeakHitList[i]));
  //  _houghPeakHitList.push_back(p_hit);
  //}
  return *this;
}

HoughPeak::HoughPeak(const HoughPeak& other)
{
  if(_houghPeakHitList.size() != 0 ){
	for(int i =0;i<_houghPeakHitList.size();i++){
	  _houghPeakHitList[i]=NULL;
	}
	_houghPeakHitList.clear();
  }
  _height=other._height;
  _theta=other._theta;
  _rho=other._rho;
  _thetaBin=other._thetaBin;
  _rhoBin=other._rhoBin;
  _houghPeakHitList = other._houghPeakHitList;
  _isCandiTrack=other._isCandiTrack;
  _peakNum=other._peakNum;
  _charge=other._charge;
  //for(int i =0;i<other._houghPeakHitList.size();i++){
  //  const HoughHit* p_hit= new HoughHit( *(other._houghPeakHitList[i]));
  //  _houghPeakHitList.push_back(p_hit);
  //}
}

HoughPeak::HoughPeak(int itheta ,int irho, double theta,double rho,vector< const HoughHit* >** mapHitList,bool isCandiTrack,int peakNum){
  _thetaBin=itheta;
  _rhoBin=irho;
  _theta=theta;
  _rho=rho;
  _houghPeakHitList = mapHitList[itheta][irho];
  _isCandiTrack =isCandiTrack;
  _peakNum=peakNum;
 // int t_size=mapHitList[itheta][irho].size();
 // for(int i =0;i<t_size;i++){
 //   const HoughHit* p_hit= new HoughHit( *(mapHitList[itheta][irho][i]));
 //   _houghPeakHitList.push_back(p_hit);
 // }
}

HoughPeak::HoughPeak(int height,int itheta ,int irho, double theta,double rho,bool isCandiTrack,int peakNum,int charge){
  _height=height;
  _thetaBin=itheta;
  _rhoBin=irho;
  _theta=theta;
  _rho=rho;
//  _houghPeakHitList = mapHitList[itheta][irho];
  _isCandiTrack =isCandiTrack;
  _peakNum=peakNum;
  _charge=charge;
//  collectHits(hitlist);
}

void HoughPeak::printAllHit() const{
  int size=_houghPeakHitList.size();
  if( size ==0 ) return ;
  cout<<"Print hitlist in HoughPeak " <<endl;
  cout<<"at center ("<<_theta<<" ,"<<_rho<<"): "<<size<<endl;
  for(int i=0;i<size;i++){
	int layer= _houghPeakHitList[i]->getLayerId();
	int wire = _houghPeakHitList[i]->getWireId();
	std::cout<<"Peak   ("<<layer<<","<<wire<<") "<< _houghPeakHitList[i]<<std::endl;
  }
}

int HoughPeak::getHitNum(int select ) const{
  int size=_houghPeakHitList.size();
  int n0,n1,n2,n3,n4,n5,n6;
  n0=n1=n2=n3=n4=n5=n6=0;
  for(int i=0;i<size;i++){
	int cir=   _houghPeakHitList[i]->getCirList();
	int index = _houghPeakHitList[i]->digi()->getTrackIndex();
	int style=  _houghPeakHitList[i]->getStyle();
	//	cout<<"peak compare ("<<_houghPeakHitList[i]->getLayerId()<<" ," <<_houghPeakHitList[i]->getWireId()<<" )"<<endl;
	n0++;
	if( style == 1 ) n1++;
	if( style == 2 ) n2++;
	if( index < 0 )  n3++;
	if( index >=0 && cir<=1 && style==0 )  n4++;
	if( index <0 || cir>1 )   n5++;
	if( index >=0 && cir==0 && style==0 )  n6++;
  }
  if( select == 0 ) return n0;
  if( select == 1 ) return n1;
  if( select == 2 ) return n2;
  if( select == 3 ) return n3;
  if( select == 4 ) return n4;
  if( select == 5 ) return n5;
  if( select == 6 ) return n6;
}
int HoughPeak::getHitNumA(int select ) const{
  int size=_houghPeakHitList.size();
  int n0,n1,n2,n3,n4,n5,n6;
  n0=n1=n2=n3=n4=n5=n6=0;
  for(int i=0;i<size;i++){
	int cir=   _houghPeakHitList[i]->getCirList();
	int index = _houghPeakHitList[i]->digi()->getTrackIndex();
	int type =  _houghPeakHitList[i]->getSlayerType();
	int style=  _houghPeakHitList[i]->getStyle();
	if( type ==0 ) n0++;
	if( type==0 && style == 1 ) n1++;
	if( type==0 && style == 2 ) n2++;
	if( type==0 && index < 0 )  n3++;
	if( type==0 && index >=0 && cir<=1 && style==0)  n4++;
	if( type==0 && (index <0 || cir>1) )   n5++;
	if( type==0 && index >=0 && cir==0 && style==0)  n6++;
  }
  if( select == 0 ) return n0;
  if( select == 1 ) return n1;
  if( select == 2 ) return n2;
  if( select == 3 ) return n3;
  if( select == 4 ) return n4;
  if( select == 5 ) return n5;
  if( select == 6 ) return n6;
}
int HoughPeak::getHitNumS(int select ) const{
  int size=_houghPeakHitList.size();
  int n0,n1,n2,n3,n4,n5,n6;
  n0=n1=n2=n3=n4=n5=n6=0;
  for(int i=0;i<size;i++){
	int cir=   _houghPeakHitList[i]->getCirList();
	int index = _houghPeakHitList[i]->digi()->getTrackIndex();
	int type =  _houghPeakHitList[i]->getSlayerType();
	int style=  _houghPeakHitList[i]->getStyle();
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

int HoughPeak::collectHits(const HoughHitList& hitlist){
  //read in
  double par[7][12];
  ifstream input;
  input.open(m_file.c_str());
  string line;
  int i=0;
  while ( getline(input,line) ){
	istringstream stream(line);
	double sigma;
	int j=0;
	while(stream>>sigma){
		par[i][j]=sigma;
		j++;
	}
	i++;
  }

  double cirr_track = fabs(1./(_rho));
  double cirx_track = (1./_rho)*cos(_theta); 
  double ciry_track = (1./_rho)*sin(_theta); 
  std::vector<HoughHit>::const_iterator iter= hitlist.getList().begin();
  for(; iter!= hitlist.getList().end(); iter++){
	const HoughHit *hit = &(*iter);
	int layer = hit->getLayerId();
	if( hit->getSlayerType()!=0 ) continue;
	double cirx_hit = hit->getMidX();
	double ciry_hit = hit->getMidY();
	double cirr_hit = hit->getDriftDist();
	if( _charge == 1 && (cirx_track*ciry_hit - ciry_track*cirx_hit>=0) ) continue;  // suppose charge -1 FIXME
	if( _charge ==-1 && (cirx_track*ciry_hit - ciry_track*cirx_hit<=0) ) continue;  // suppose charge -1 FIXME
	double l1l2 = sqrt( (cirx_hit-cirx_track)*(cirx_hit-cirx_track)+(ciry_hit-ciry_track)*(ciry_hit-ciry_track) );
	double deltaD ;
	if( l1l2>cirr_track )   deltaD = l1l2-cirr_track-cirr_hit;
	if( l1l2<=cirr_track )  deltaD = l1l2-cirr_track+cirr_hit;

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
	double pt = fabs( (1./_rho)/333.567 );
	//if( fabs(deltaD)<0.2 && l_temp<=35 ) _houghPeakHitList.push_back(hit);  //suppose 60MeV FIXME
	//if( fabs(deltaD)<0.2 && l_temp>35 && l_temp<=45) _houghPeakHitList.push_back(hit);
	/*
	   double d_cut1 = m_dcut1;
	   if( pt<0.06 && fabs(deltaD)<d_cut1 && l_temp<=35 ) _houghPeakHitList.push_back(hit);  
	   if( 0.06<pt&&pt<0.07 && fabs(deltaD)<d_cut1 && l_temp<=35 ) _houghPeakHitList.push_back(hit);  
	   if( 0.07<pt&&pt<0.08 && fabs(deltaD)<d_cut1 && l_temp<=43 ) _houghPeakHitList.push_back(hit);  
	   if( 0.08<pt&&pt<0.09 && fabs(deltaD)<d_cut1 && l_temp<=43 ) _houghPeakHitList.push_back(hit);  
	   if( 0.09<pt&&pt<0.10 && fabs(deltaD)<d_cut1 && l_temp<=41 ) _houghPeakHitList.push_back(hit);  
	   if( 0.10<pt&&pt<0.11 && fabs(deltaD)<d_cut1 && l_temp<=41 ) _houghPeakHitList.push_back(hit);  
	   if( 0.11<pt&&pt<0.12 && fabs(deltaD)<d_cut1 && l_temp<=41 ) _houghPeakHitList.push_back(hit);  
	   if( pt>0.12 && fabs(deltaD)<d_cut1 ) _houghPeakHitList.push_back(hit);  

	   double d_cut2 = m_dcut2;
	   if( pt<0.06 && fabs(deltaD)<d_cut2 && l_temp>35&&l_temp<45 ) _houghPeakHitList.push_back(hit);  
	   if( 0.06<pt&&pt<0.07 && fabs(deltaD)<d_cut2 && l_temp>35&& l_temp<=45) _houghPeakHitList.push_back(hit);  
	   if( 0.07<pt&&pt<0.08 && fabs(deltaD)<d_cut2 && l_temp>43&& l_temp<=50) _houghPeakHitList.push_back(hit);  
	   if( 0.10<pt&&pt<0.11 && fabs(deltaD)<d_cut2 && l_temp>80&& l_temp<=105) _houghPeakHitList.push_back(hit);  
	   if( 0.11<pt&&pt<0.12 && fabs(deltaD)<d_cut2 && l_temp>80&& l_temp<=105) _houghPeakHitList.push_back(hit);  
	   */


	if( pt<0.06 && fabs(deltaD)<0.1&& l_temp<=35 ) _houghPeakHitList.push_back(hit);  
	if( 0.06<pt&&pt<0.07 && fabs(deltaD)<0.1 && l_temp<=35 ) _houghPeakHitList.push_back(hit);  
	if( 0.07<pt&&pt<0.08 && fabs(deltaD)<0.1 && l_temp<=43 ) _houghPeakHitList.push_back(hit);  
	if( 0.08<pt&&pt<0.09 && fabs(deltaD)<0.1 && l_temp<=43 ) _houghPeakHitList.push_back(hit);  
	if( 0.09<pt&&pt<0.10 && fabs(deltaD)<0.1 && l_temp<=41 ) _houghPeakHitList.push_back(hit);  
	if( 0.10<pt&&pt<0.11 && fabs(deltaD)<0.1 && l_temp<=41 ) _houghPeakHitList.push_back(hit);  
	if( 0.11<pt&&pt<0.12 && fabs(deltaD)<0.1 && l_temp<=41 ) _houghPeakHitList.push_back(hit);  

	if( pt<0.06 && fabs(deltaD)<0.1&& l_temp>35&&l_temp<45 ) _houghPeakHitList.push_back(hit);  
	if( 0.06<pt&&pt<0.07 && fabs(deltaD)<0.1 && l_temp>35&& l_temp<=45) _houghPeakHitList.push_back(hit);  
	if( 0.07<pt&&pt<0.08 && fabs(deltaD)<0.1 && l_temp>43&& l_temp<=50) _houghPeakHitList.push_back(hit);  
	if( 0.08<pt&&pt<0.09 && fabs(deltaD)<0.1 && l_temp>43&& l_temp<=50) _houghPeakHitList.push_back(hit);  
	//	if( 0.09<pt&&pt<0.10 && fabs(deltaD)<0. && l_temp>41&& l_temp<=50) _houghPeakHitList.push_back(hit);  
	//	if( 0.10<pt&&pt<0.11 && fabs(deltaD)<0. && l_temp>80&& l_temp<=105) _houghPeakHitList.push_back(hit);  
	//	if( 0.11<pt&&pt<0.12 && fabs(deltaD)<0. && l_temp>80&& l_temp<=105) _houghPeakHitList.push_back(hit);  
	if( pt>0.12 && fabs(deltaD)<0.1) _houghPeakHitList.push_back(hit);  

/*
	double n = m_dSigma_cut;
	double n2 = m_dSigma_cut2;
	if( layer<20){
	  if( 0.05<pt&&pt<0.06 ) {
		if(layer<16 && fabs(deltaD)<n*par[0][layer-8]) _houghPeakHitList.push_back(hit);
		if(layer>=16 && fabs(deltaD)<n2*par[0][layer-8]) _houghPeakHitList.push_back(hit);
	  }
	  if( 0.06<pt&&pt<0.07 ) {
		if(layer<18 && fabs(deltaD)<n*par[1][layer-8]) _houghPeakHitList.push_back(hit);
		if(layer>=18 && fabs(deltaD)<n2*par[0][layer-8]) _houghPeakHitList.push_back(hit);
	  }
	  if( 0.07<pt&&pt<0.08 ) {
		if(fabs(deltaD)<n*par[2][layer-8]) _houghPeakHitList.push_back(hit);
	  }
	  if( 0.08<pt&&pt<0.09 ) {
		if(fabs(deltaD)<n*par[3][layer-8]) _houghPeakHitList.push_back(hit);
	  }
	  if( 0.09<pt&&pt<0.10 ) {
		if(fabs(deltaD)<n*par[4][layer-8]) _houghPeakHitList.push_back(hit);
	  }
	  if( 0.10<pt&&pt<0.11 ) {
		if(fabs(deltaD)<n*par[5][layer-8]) _houghPeakHitList.push_back(hit);
	  }
	  if( 0.11<pt&&pt<0.12 ) {
		if(fabs(deltaD)<n*par[6][layer-8]) _houghPeakHitList.push_back(hit);
	  }
	  if( 0.12<pt || pt<0.05 ) {
		if(fabs(deltaD)<0.1) _houghPeakHitList.push_back(hit);
	  }
	}
	else if ( 0.12<pt || pt<0.05 ) {
	  if(fabs(deltaD)<0.1) _houghPeakHitList.push_back(hit);
	}
	*/

  }
  return _houghPeakHitList.size();

}
