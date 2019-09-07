#include "MdcHoughFinder/HoughZsFit.h"
#include "TTree.h"
#include "TH2D.h"
#include "TF1.h"
#include "TGraph.h"
#include <algorithm>
#include <assert.h>

int HoughZsFit::m_debug=0;

HoughZsFit::HoughZsFit(vector<HoughRecHit>* recHitCol)
{
  vector<HoughRecHit>::iterator  iter=recHitCol->begin();
  for(;iter!=recHitCol->end();iter++){
	//if( fabs((*iter).getzAmb(0))>10 && fabs((*iter).getzAmb(1))>10 )  continue;
	if( (*iter).getSlayerType() !=0 && (*iter).getflag()==0 && (*iter).getLayerId()<8  ) _recStereoHit.push_back(&(*iter));
  }
  _hitSize=_recStereoHit.size();
  _pro_correct =0;
  sortHit();
  if(m_debug>0) print();
  if(m_debug>0) cout<<"size of rechit in zs fit "<<_hitSize<<endl;

//  _vecPoint = new Point*[_hitSize];
//  for(int i=0;i<_hitSize;i++){
//	_vecPoint[i] = new Point[2];
//  }
//  initPoint();
  if(_hitSize>=3) doit();
  else {_tanl=0;_z0=0;}
}

HoughZsFit::~HoughZsFit(){
}

void HoughZsFit::doit(){

//  Point point0;
//  point0.first=0.;
//  point0.second=0.;

  Line linefit[8];

  for(int iline=0;iline<8;iline++){
  linefit[iline]._pointCol.push_back( *(_recStereoHit[0]) );
  linefit[iline]._pointCol.push_back( *(_recStereoHit[1]) );
  linefit[iline]._pointCol.push_back( *(_recStereoHit[2]) );
  }
  if(m_debug>0) {
	for(int i =0;i<3;i++){
	  cout<<" the first 3 hits ("<<i<<"  "<< _recStereoHit[i]->getLayerId()<<","<<_recStereoHit[i]->getWireId()<<")"<<endl;
	  cout<<"  left "<<(_recStereoHit[i])->getsAmb(0)<<" "<<(_recStereoHit[i])->getzAmb(0)<<endl;
	  cout<<"  right "<<(_recStereoHit[i])->getsAmb(1)<<" "<<(_recStereoHit[i])->getzAmb(1)<<endl;
	}
  }

  linefit[0]._pointCol[0].setAmb(0);
  linefit[0]._pointCol[1].setAmb(0);
  linefit[0]._pointCol[2].setAmb(0);

  linefit[1]._pointCol[0].setAmb(0);
  linefit[1]._pointCol[1].setAmb(0);
  linefit[1]._pointCol[2].setAmb(1);

  linefit[2]._pointCol[0].setAmb(0);
  linefit[2]._pointCol[1].setAmb(1);
  linefit[2]._pointCol[2].setAmb(0);

  linefit[3]._pointCol[0].setAmb(0);
  linefit[3]._pointCol[1].setAmb(1);
  linefit[3]._pointCol[2].setAmb(1);


  linefit[4]._pointCol[0].setAmb(1);
  linefit[4]._pointCol[1].setAmb(0);
  linefit[4]._pointCol[2].setAmb(0);

  linefit[5]._pointCol[0].setAmb(1);
  linefit[5]._pointCol[1].setAmb(0);
  linefit[5]._pointCol[2].setAmb(1);

  linefit[6]._pointCol[0].setAmb(1);
  linefit[6]._pointCol[1].setAmb(1);
  linefit[6]._pointCol[2].setAmb(0);

  linefit[7]._pointCol[0].setAmb(1);
  linefit[7]._pointCol[1].setAmb(1);
  linefit[7]._pointCol[2].setAmb(1);

  for(int i=0;i<8;i++){
	linefit[i]._ambig.push_back( linefit[i]._pointCol[0].getAmbig() );
	linefit[i]._ambig.push_back( linefit[i]._pointCol[1].getAmbig() );
	linefit[i]._ambig.push_back( linefit[i]._pointCol[2].getAmbig() );
	if(m_debug>0) cout<<" ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^begin line "<<i<<endl;
	leastFit(linefit[i],3); 
	//judst if there is noise in the 3 outer
	int line_size=3;
	for(int j=3;j<_hitSize;j++){
	  double chi_last=linefit[i]._chi;
	  double k_last=linefit[i]._k;
	  double b_last=linefit[i]._b;
	  if(m_debug>0) cout<<"last "<<j<<" chi k b "<<chi_last<<" "<<k_last<<" "<<b_last<<endl;
	  linefit[i]._pointCol.push_back( *(_recStereoHit[j]) );
	  //int layer =linefit[i]._pointCol.back().getLayerId();
	  //int wire=linefit[i]._pointCol.back().getWireId();
	  //cout<<"???ambig ("<<layer<<","<<wire<<") "<<linefit[i]._pointCol.back().getLrTruth()<<endl;
	  linefit[i]._pointCol[line_size].setAmb(0);
	  if(m_debug>0) cout<<"Add point left "<<"("<<linefit[i]._pointCol[line_size].getLayerId()<<","<<linefit[i]._pointCol[line_size].getWireId()<<") "<<linefit[i]._pointCol[line_size].getStyle()<<"  "<<linefit[i]._pointCol[line_size].getsPos()<<" "<<linefit[i]._pointCol[line_size].getzPos()<<endl;
	  leastFit(linefit[i],line_size+1); 
	  double chil=linefit[i]._chi;
	  double kl=linefit[i]._k;
	  double bl=linefit[i]._b;
	  if (linefit[i]._pointCol[line_size].getsPos()==-99) {
	  chil = 9999;
	  }
	  if(m_debug>0) cout<<"left "<<line_size<<" chi k b "<<chil<<" "<<kl<<" "<<bl<<endl;

	  linefit[i]._pointCol[line_size].setAmb(1);
	  if(m_debug>0) cout<<"Add point right "<<"("<<linefit[i]._pointCol[line_size].getLayerId()<<","<<linefit[i]._pointCol[line_size].getWireId()<<") "<<linefit[i]._pointCol[line_size].getStyle()<<"  "<<linefit[i]._pointCol[line_size].getsPos()<<" "<<linefit[i]._pointCol[line_size].getzPos()<<endl;
	  leastFit(linefit[i],line_size+1); 
	  double chir=linefit[i]._chi;
	  double kr=linefit[i]._k;
	  double br=linefit[i]._b;
	  if (linefit[i]._pointCol[line_size].getsPos()==-99) {
	  chir = 9999;
	  }
	  if(m_debug>0) cout<<"right "<<line_size<<" chi k b "<<chir<<" "<<kr<<" "<<br<<endl;

	  if(chil<chir) {
		linefit[i]._pointCol[line_size].setAmb(0);
		linefit[i]._chi=chil;
		linefit[i]._k=kl;
		linefit[i]._b=bl;
		linefit[i]._ambig.push_back(0);
		//_recStereoHit[j]->setAmb(0);
	  }
	  else linefit[i]._ambig.push_back(1); 
	  line_size++;

	  //test ambig with truth
	  int same_ambig = 1;
	  for(int ihit=0;ihit<line_size;ihit++){
		int ambighit = linefit[i]._pointCol[ihit].getAmbig();
		int ambigTruth =  linefit[i]._pointCol[ihit].getLrTruth();
		int layer =linefit[i]._pointCol[ihit].getLayerId();
		int wire=linefit[i]._pointCol[ihit].getWireId();
		//if (ambigTruth == -1) ambigTruth=1;
		//else if (ambigTruth == 1) ambigTruth=0;
		//else cout << "wrong in ambig Truth "<<endl;

		//if(ambighit!= ambigTruth ) same_ambig=0;
	  }
	  //if(m_debug >0 ) cout << "same ambig ? "<<same_ambig<<endl;


	  double dChi = chi_last-linefit[i]._chi;
	  double dChi_n = (linefit[i]._chi)/(line_size-1)-(chi_last/line_size);
	  if(m_debug>0) cout<<"dChi: "<<dChi<<endl;
	  if(m_debug>0) cout<<"dChi/n: "<<dChi_n<<endl;
	  if(m_debug>0)  cout<<endl;
	//  if( fabs(dChi) > 500.)     //FIXME
	  if( fabs(dChi_n) > 25.)     //FIXME
		//		linefit[i]._pointCol[j].setAmb(-999);
		//_recStereoHit[j]->setAmb(-999);
		//_recStereoHit[j]->setflag(-999);
		{
		linefit[i]._pointCol.pop_back();
		linefit[i]._chi=chi_last;
		linefit[i]._k=k_last;
		linefit[i]._b=b_last;
		linefit[i]._ambig.at(j)=-999;
		line_size--;
	  }
	}
	//reject wrong line
	if( (linefit[i]._pointCol[0].getsPos()==-99) || (linefit[i]._pointCol[1].getsPos()==-99)  || (linefit[i]._pointCol[2].getsPos()==-99) ) {
	  linefit[i]._chi=99999; 
	}
  }
  std::sort(linefit,linefit+8,compare_zsfit);
  for(int i=0;i<8;i++){
	if(m_debug>0) cout<<"Line :"<<i<<" chis: "<<linefit[i]._chi<<" k,b: "<<linefit[i]._k<<" "<<linefit[i]._b<<endl;
	int ambig_correct = 0;
	for(int j=0;j<_hitSize;j++){
	  int ambig = linefit[i]._ambig.at(j);
	  int layer= _recStereoHit.at(j)->getLayerId();
	  int wire=  _recStereoHit.at(j)->getWireId();
	  int style= _recStereoHit.at(j)->getStyle();
	  double l=-99;
	  if (ambig!=-999) l= _recStereoHit.at(j)->getsAmb(ambig);
	  double z=-99;
	  if (ambig!=-999) z= _recStereoHit.at(j)->getzAmb(ambig);
	  //  cout<<"debug ("<<layer<<" ,"<<wire<<") style "<<style<<" ambig "<<ambig<<" s "<<l<<" z "<<z<<endl;
	  if (l==-99 && z==-99)  ambig=-999;
	  if(m_debug>0) cout<<"("<<layer<<" ,"<<wire<<") style "<<style<<" ambig "<<ambig<<" s "<<l<<" z "<<z<<endl;
	  if(i==0) {
		_recStereoHit[j]->setAmb(ambig);      // when line 0 select
		if( ambig ==-999) _recStereoHit[j]->setflag(-999);      // when line 0 select
		int ambigTruth = _recStereoHit.at(j)->getLrTruth();
		if (ambigTruth == -1) ambigTruth=1;
		else if (ambigTruth == 1) ambigTruth=0;
		if(ambig ==ambigTruth) ambig_correct++;
		//cout<<"ambig : "<<ambig<<" "<<ambigTruth<<endl;
		_pro_correct = (double)ambig_correct/(double)_hitSize;
	  }
	}
  }
  _tanl=linefit[0]._k;
  _z0=linefit[0]._b;
  if(m_debug>0) cout<<"z0 tanl : "<<_z0<<" "<<_tanl<<endl;

  /*
	 linefit[0]._pointCol.push_back(_vecPoint[_hitSize-1][0]);
	 linefit[0]._pointCol.push_back(_vecPoint[_hitSize-2][0]);
	 linefit[0]._pointCol.push_back(_vecPoint[_hitSize-3][0]);

	 linefit[1]._pointCol.push_back(_vecPoint[_hitSize-1][0]);
	 linefit[1]._pointCol.push_back(_vecPoint[_hitSize-2][0]);
	 linefit[1]._pointCol.push_back(_vecPoint[_hitSize-3][1]);

	 linefit[2]._pointCol.push_back(_vecPoint[_hitSize-1][0]);
	 linefit[2]._pointCol.push_back(_vecPoint[_hitSize-2][1]);
	 linefit[2]._pointCol.push_back(_vecPoint[_hitSize-3][0]);

	 linefit[3]._pointCol.push_back(_vecPoint[_hitSize-1][0]);
	 linefit[3]._pointCol.push_back(_vecPoint[_hitSize-2][1]);
	 linefit[3]._pointCol.push_back(_vecPoint[_hitSize-3][1]);

	 linefit[4]._pointCol.push_back(_vecPoint[_hitSize-1][1]);
	 linefit[4]._pointCol.push_back(_vecPoint[_hitSize-2][0]);
	 linefit[4]._pointCol.push_back(_vecPoint[_hitSize-3][0]);

	 linefit[5]._pointCol.push_back(_vecPoint[_hitSize-1][1]);
	 linefit[5]._pointCol.push_back(_vecPoint[_hitSize-2][0]);
	 linefit[5]._pointCol.push_back(_vecPoint[_hitSize-3][1]);

	 linefit[6]._pointCol.push_back(_vecPoint[_hitSize-1][1]);
	 linefit[6]._pointCol.push_back(_vecPoint[_hitSize-2][1]);
	 linefit[6]._pointCol.push_back(_vecPoint[_hitSize-3][0]);

	 linefit[7]._pointCol.push_back(_vecPoint[_hitSize-1][1]);
	 linefit[7]._pointCol.push_back(_vecPoint[_hitSize-2][1]);
	 linefit[7]._pointCol.push_back(_vecPoint[_hitSize-3][1]);


	 for(int i=0;i<8;i++){
	 linefit[i]._amg=i*pow(10,(_hitSize-3));
	 if(m_debug>0) cout<<"Start line ooooooooooooooooooooooooooooooooooooooooooooooooo"<<i<<endl;
	 for(int j=4;j<_hitSize+1;j++){
	 if(m_debug>0) cout<<"Start add _vecPoint ****************************"<<j<<endl;
	 linefit[i]._pointCol.push_back(_vecPoint[_hitSize-j][0]);
	 leastFit(linefit[i],j); 
	 double chil=linefit[i]._chi;
	 double kl=linefit[i]._k;
	 double bl=linefit[i]._b;

	 linefit[i]._pointCol[j-1]=_vecPoint[_hitSize-j][1];
	 leastFit(linefit[i],j); 
	 double chir=linefit[i]._chi;
	 double kr=linefit[i]._k;
	 double br=linefit[i]._b;
	 if(m_debug>0){
	 cout<<chil<<" "<<kl<<" "<<bl<<endl;
	 cout<<chir<<" "<<kr<<" "<<br<<endl;
	 }

	 if(chil<chir) {
  //		  cout<<"j: "<<j<<endl;
  linefit[i]._pointCol[j-1]=_vecPoint[_hitSize-j][0];
  linefit[i]._chi=chil;
  linefit[i]._k=kl;
  linefit[i]._b=bl;
  linefit[i]._amg+=0*pow(10,(_hitSize-j));
  }
  else { linefit[i]._amg+=1*pow(10,(_hitSize-j)); }
  }
  if(m_debug>0) cout<<"Start add _vecPoint ****************************"<<"0"<<endl;
  linefit[i]._pointCol.push_back(point0);
  leastFit(linefit[i],_hitSize+1); 
  double chi0=linefit[i]._chi;
  double k0=linefit[i]._k;
  double b0=linefit[i]._b;
  if(m_debug>0) cout<<chi0<<" "<<k0<<" "<<b0<<endl;
}
std::sort(linefit,linefit+8,compare_zsfit);
for(int i=0;i<8;i++){
  if(m_debug>0) cout<<"Line :"<<i<<" chis: "<<linefit[i]._chi<<" k,b: "<<linefit[i]._k<<" "<<linefit[i]._b<<" amg: "<<linefit[i]._amg<<endl;
}
_tanl=linefit[0]._k;
_z0=linefit[0]._b;
if(m_debug>0) cout<<"z0 tanl : "<<_z0<<" "<<_tanl<<endl;
*/
}


//void HoughZsFit::initPoint(){
//  for(int i=0;i<_hitSize;i++){
//	if(m_debug>0){
//	  //	cout<<"left s: "<<_recStereoHit[i].getsAmb(0)<<endl;
//	  //	cout<<"left z: "<<_recStereoHit[i].getzAmb(0)<<endl;
//	  //	cout<<"right s: "<<_recStereoHit[i].getsAmb(1)<<endl;
//	  //	cout<<"right z: "<<_recStereoHit[i].getzAmb(1)<<endl;
//	}
//	_vecPoint[i][0].first =_recStereoHit[i]->getsAmb(0);
//	_vecPoint[i][0].second=_recStereoHit[i]->getzAmb(0);
//	_vecPoint[i][1].first =_recStereoHit[i]->getsAmb(1);
//	_vecPoint[i][1].second=_recStereoHit[i]->getzAmb(1);
//  }
//
//}
void HoughZsFit::leastFit(Line &linefit ,int N){
  double *x=new double[N+1];
  double *y=new double[N+1];
  for(int i=0;i<N;i++){
	double a=linefit._pointCol[i].getsPos();
	double b=linefit._pointCol[i].getzPos();
	x[i]=a;
	y[i]=b;
	if(m_debug>0) cout<<" x "<<a<<" y "<<b<<endl;
  }
  x[N]=0.;
  y[N]=0.;
  double k(0),b(0),chi2(0);
  leastLine(N+1,x,y,k,b,chi2);
  linefit._k  =k;
  linefit._b  =b;
  linefit._chi=chi2;
  delete []x;
  delete []y;

}
bool compare_zsfit(const HoughZsFit::Line& a,const HoughZsFit::Line& b){
  return abs(a._chi)<abs(b._chi);
}

int HoughZsFit::leastLine(int nhit,double x[],double y[],double &k,double &b,double& chi2){
  double x_sum=0;
  double y_sum=0;
  double x2_sum=0;
  double y2_sum=0;
  double xy_sum=0;
  for(int i=0;i<nhit;i++){
	x_sum=x_sum+x[i];
	y_sum=y_sum+y[i];
	x2_sum=x2_sum+x[i]*x[i];
	y2_sum=y2_sum+y[i]*y[i];
	xy_sum=xy_sum+x[i]*y[i];
  }
  b=(x2_sum*y_sum-xy_sum*x_sum)/(nhit*x2_sum-x_sum*x_sum);
  k=(nhit*xy_sum-x_sum*y_sum)/(nhit*x2_sum-x_sum*x_sum);

  double yE[3];
  for(int i=0;i<nhit;i++){
	yE[i]=k*x[i]+b;
	double chi2_temp;
	if(yE[i]!=0) chi2_temp=(y[i]-yE[i])*(y[i]-yE[i])/yE[i]*yE[i];
	else chi2_temp=0;
	chi2+=chi2_temp;
  }

  return 1;
}

bool layer_in_track(const HoughRecHit* hita,const HoughRecHit* hitb){
  return (*hita).getLayerId() > (*hitb).getLayerId();
}
void HoughZsFit::sortHit(){
  std::sort (_recStereoHit.begin(),_recStereoHit.end(),layer_in_track);
}
void HoughZsFit::print(){
  for(int i =0;i<_recStereoHit.size();i++){
	cout<<"("<<_recStereoHit[i]->getLayerId()<<","<<_recStereoHit[i]->getWireId()<<")"<<endl;
  }
}
