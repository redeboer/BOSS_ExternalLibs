#include "MdcHoughFinder/HoughRecHit.h"
#include "MdcHoughFinder/HoughStereo.h"

int HoughStereo::m_debug=0;
//HoughStereo::HoughStereo(){
//}
//HoughStereo::HoughStereo(double bunchTime):_bunchTime(bunchTime){
//}
HoughStereo::HoughStereo( double bunchTime , Hough2D *circle , HoughRecHit* rechit):_bunchTime(bunchTime),_circle(circle),_rechit(rechit){
	  _lleft=-99;
	  _zleft=-99;
	  _lright=-99;
	  _zright=-99;
	  _ambig=0;
	  _charge=circle->getCharge();
}
void HoughStereo::setAmb(int i){
  //initial 
  _ambig=i;
}


int HoughStereo::cald(){
  bool ok_ambig[2];
  bool ok[2][2];
  ok_ambig[0]=true;
  ok_ambig[1]=true;
  ok[0][0]=false;
  ok[0][1]=false;
  ok[1][0]=false;
  ok[1][1]=false;
  double xeast = _rechit->getEastPoint().x();
  double xwest = _rechit->getWestPoint().x();
  double yeast = _rechit->getEastPoint().y();
  double ywest = _rechit->getWestPoint().y();
  double zeast = _rechit->getEastPoint().z();
  double zwest = _rechit->getWestPoint().z();
//  cout<<"xeast xwest "<<xeast<<" "<<xwest<<endl;
  double k = (ywest-yeast)/(xwest-xeast);  
  double b = -k*xeast+yeast;
 // cout<<" k b "<<k<<" "<<b<<endl;
  double xc = _circle->getCirX();
  double yc = _circle->getCirY();
  double rc = _circle->getCirR();
//  cout<<"xc yc rc "<<xc<<" "<<yc<<" "<<rc<<endl;
  double drift = _rechit->getDriftDist();                   
  //double drift = _rechit->getDriftDistTruth(); 
//  cout<<"drift "<<_rechit->getDriftDist()<<endl;
//  cout<<"driftTruth "<<_rechit->getDriftDistTruth()<<endl;
  double x1(999),y1(999);
  double x2(999),y2(999);
  double a = k*k+1;
  double b1 = -2*(xc+k*yc-k*b);
  double c1 = xc*xc+(yc-b)*(yc-b)-(rc+drift)*(rc+drift);
  double c2 = xc*xc+(yc-b)*(yc-b)-(rc-drift)*(rc-drift);
  double delta1 = (b1*b1-4*a*c1);
  double delta2 = (b1*b1-4*a*c2);
  if( delta1 <0 ) ok_ambig[0] = false; 
  if( delta2 <0 ) ok_ambig[1] = false; 
  // cout<<"(b1*b1-4*a*c1) "<<(b1*b1-4*a*c1)<<endl;
  // cout<<"(b1*b1-4*a*c2) "<<(b1*b1-4*a*c2)<<endl;
  if( delta1>=0 ) {
	double x1_0 = ( -b1+sqrt(delta1) ) /(2*a);
	double x1_1 = ( -b1-sqrt(delta1) ) /(2*a);
//	cout<<"x1 0 1 "<<x1_0<<" "<<x1_1<<endl;
	if( (xeast>=x1_0 && xwest<=x1_0) or (xeast<=x1_0 && xwest>=x1_0) ) ok[0][0] = true;
	if ( (xeast>=x1_1 && xwest<=x1_1) or (xeast<=x1_1 && xwest>=x1_1) ) ok[0][1]=true;
	if( ok[0][0] == true && ok[0][1]==false )  x1 = x1_0;
	if( ok[0][0] == false && ok[0][1]==true )  x1 = x1_1;
	if( ok[0][0] == true && ok[0][1]==true )  {
	  x1 = x1_0;   //??good 
//	  cout<<" error both ok "<<endl;
	}
	if( ok[0][0] == false && ok[0][1]==false ) ok_ambig[0] = false; 
	y1 = k*x1+b;
	double theta1 = 0;
	double l1= 0;
	double z1= 0;
	if(ok_ambig[0]==true) {
	  calcu(x1,y1,xc,yc,rc,xeast,yeast,zeast,xwest,ywest,zwest,theta1,l1,z1);
	  if(m_debug>0) cout<<" theta1 l1 z1 "<<theta1<<" "<<l1<<" "<<z1<<endl;
	  _zleft = z1;
	  _lleft = l1;
	}
  }

  if( delta2>=0){
	double x2_0 = ( -b1+sqrt(delta2) ) /(2*a);
	double x2_1 = ( -b1-sqrt(delta2) ) /(2*a);
//	cout<<"x2 0 1 "<<x2_0<<" "<<x2_1<<endl;
	if( (xeast>=x2_0 && xwest<=x2_0) or (xeast<=x2_0 && xwest>=x2_0) ) ok[1][0] = true;
	if ( (xeast>=x2_1 && xwest<=x2_1) or (xeast<=x2_1 && xwest>=x2_1) ) ok[1][1]=true;
	if( ok[1][0] == true && ok[1][1]==false )  x2 = x2_0;
	if( ok[1][0] == false && ok[1][1]==true )  x2 = x2_1;
	if( ok[1][0] == true && ok[1][1]==true )  {
	  x2 = x2_0;
//	  cout<<" error both ok "<<endl;
	}
	if( ok[1][0] == false && ok[1][1]==false ) ok_ambig[1] = false; 
	y2 = k*x2+b;
	double theta2 = 0;
	double l2= 0;
	double z2= 0;
	if(ok_ambig[1]==true) {
	  calcu(x2,y2,xc,yc,rc,xeast,yeast,zeast,xwest,ywest,zwest,theta2,l2,z2);
	  if(m_debug>0) cout<<" theta2 l2 z2 "<<theta2<<" "<<l2<<" "<<z2<<endl;
	  _zright = z2;
	  _lright = l2;
	}
  }
  if( ok_ambig[0] == true && ok_ambig[1] == false ) return -1;
  if( ok_ambig[0] == false && ok_ambig[1] == true ) return 1;
  if( ok_ambig[0] == true && ok_ambig[1] == true ) return 2;
  if( ok_ambig[0] == false && ok_ambig[1] == false ) return 0;
  //cout<<" ztruth : "<<_rechit->getZTruth()<<endl;
}


void HoughStereo::calcu(double x1 ,double y1 ,double xc,double yc,double rc,double x_east,double y_east,double z_east,double x_west,double y_west,double z_west,double& theta,double& l,double& z){
  //rphi -> sz
  if(xc==0||x1-xc==0){
	theta=0;   
  }
  else{
	theta=M_PI-atan2(y1-yc,x1-xc)+atan2(yc,xc);
	if(theta>2*M_PI){
	  theta=theta-2*M_PI;
	}
	if(theta<0){
	  theta=theta+2*M_PI;
	}
  }
  if(_charge == 1 ) theta = 2*M_PI-theta;

  double d1=sqrt((x1-x_west)*(x1-x_west)+(y1-y_west)*(y1-y_west));
  double d2=sqrt((x_east-x_west)*(x_east-x_west)+(y_east-y_west)*(y_east-y_west));
//  cout<<"d1/d2 "<<d1/d2<<" "<<d1<<" "<<d2<<endl;
//  cout<<"zw ze "<<z_west<<" "<<z_east<<endl;
  z = z_west-(z_west-z_east)*d1/d2;
  l = rc*theta;
}
/*


int HoughStereo::cald(){
  HepPoint3D fp =_rechit->getWestPoint();
  HepPoint3D rp =_rechit->getEastPoint();
  HepPoint3D X=_rechit->getMidPoint();
  if(m_debug>0) std::cout<<"fp rp "<<fp<<" "<<rp<<std::endl;//yzhang debug
  if(m_debug>0) std::cout<<"Xmid  "<<X<<std::endl;//yzhang debug
  HepVector3D xx = HepVector3D(X.x(), X.y(), 0.);
  HepPoint3D  center( _circle->getCirX(),_circle->getCirY(),0 );
  HepVector3D yy = center - xx;
  HepVector3D ww = HepVector3D(yy.x(), yy.y(), 0.);
  double wwmag2 = ww.mag2();
  double wwmag = sqrt(wwmag2);
  double driftdist = fabs( _rechit->calDriftDist(_bunchTime,_ambig) );
  if(m_debug>0) cout<<"Bunch time: "<<_bunchTime<<endl;
  HepVector3D lr(driftdist/wwmag * ww.x(), driftdist/wwmag * ww.y(), 0.);

  if(m_debug>0){
	std::cout<<"xc "<< _circle->getCirX()<< " yc "<<_circle->getCirY()<< " drfitdist "<<driftdist<<std::endl;
	std::cout<<"lr "<<lr<<"     "<<" ambig "<<_ambig
	  <<"left "<<_rechit->calDriftDist(0,1)
	  <<"right "<<_rechit->calDriftDist(0,-1)<<std::endl;
  }

  HepPoint3D ORIGIN = HepPoint3D(0., 0., 0.);
  if (_ambig == 0) lr = ORIGIN;
  if (_charge> 0){ 
	if (_ambig == -1){
	  lr = - lr;
	}
  }else{
	if (_ambig == 1){
	  lr = - lr;
	}
  }
  X += lr;

  HepPoint3D tmp(-9999., -9999., 0.);
  HepVector3D x = HepVector3D(X.x(), X.y(), 0.);
  HepVector3D w = x - center;
  // //modified the next sentence because the direction are different from belle.
  HepVector3D V = _rechit->wire()->zAxis();
  HepVector3D v = HepVector3D(V.x(), V.y(), 0.);
  double vmag2 = v.mag2();
  //double vmag = sqrt(vmag2);
  //double r = _helix->curv();
  double wv = w.dot(v);
  //    wv = abs(wv);
  if(m_debug>0){
	std::cout<<"X_fix "<<X<<" center "<<center<<std::endl;
	std::cout<<"V "<<V<<std::endl;//yzhang debug
	std::cout<<"w "<<w<<" v "<<v<<std::endl;      
	std::cout<<"wv "<<wv<<endl;
  }
  double d2 = wv * wv - vmag2 * (w.mag2() - _circle->getCirR()* _circle->getCirR() );
  if ( d2<0. ) return -1;
  double d=sqrt(d2);

  double l[2];
  l[0] =-1*( (- wv + d) / vmag2 );          //multiply -1  ......?
  l[1] =-1*( (- wv - d) / vmag2 );

  double z[2];
  //z[0] = X.z() + l[0] * V.z();               
  z[0] = X.z() - l[0] * V.z();               //l *-1
  z[1] = X.z() - l[1] * V.z();
  if(m_debug>0) cout<<"z0 z1: "<<z[0]<<" "<<z[1]<<endl;


  bool ok[2];
  ok[0] = true;
  ok[1] = true;
  //...Check z position...
  if (_ambig == 0) {
	if (z[0] > rp.z()+20.  || z[0] < fp.z()-20.) {
	  ok[0] = false;
	}
	if (z[1] > rp.z()+20.  || z[1] < fp.z()-20.) {
	  ok[1] = false;
	}
  } else {
	if (fabs(z[0]/rp.z())>1.05 ) { ok[0] = false; }
	if (fabs(z[1]/rp.z())>1.05 ) { ok[1] = false; }
  }
  if ((! ok[0]) && (! ok[1])) {
	return -1;
  }
  if (( ok[0]) && ( ok[1])) {
	return 2;
  }

  unsigned best = 0;
  if (ok[1]) best = 1;          //need improve 
  HepVector3D p[2];
  p[0] = x + l[0] * v;
  p[1] = x + l[1] * v;
  double cosdPhi = - center.dot((p[best] - center).unit()) / center.mag();
  double dPhi;
  if(fabs(cosdPhi)<=1.0) {
	dPhi = acos(cosdPhi);
  } else if (cosdPhi>1.0) {
	dPhi = 0.0;
  } else {
	dPhi = pi;	
  }

  double stemp=_circle->getCirR()*dPhi;
  if(_ambig==-1){
	_zright=z[best];	
	_lright=stemp;	
  }
  if(_ambig==1){
	_zleft=z[best];	
	_lleft=stemp;	
  }
  if(m_debug>0){ 
	cout<<"in hough stereo "<<endl;
	cout<<"left: "<<_zleft<<" "<<_lleft<<endl;
	cout<<"right: "<<_zright<<" "<<_lright<<endl;
  }
  return 0;
}
*/


void HoughStereo::setRecHit( ){
  _rechit->setzAmb( 0 , _zleft);
  _rechit->setsAmb( 0 , _lleft);
  _rechit->setzAmb( 1 , _zright);
  _rechit->setsAmb( 1 , _lright);
}
void HoughStereo::print(){
  cout<<"Hit"<<"("<<_rechit->getLayerId()<<","<<_rechit->getWireId()<<") "<<_rechit->getStyle()<<endl;
  cout<<"   left: "<<_lleft<<","<<_zleft<<endl;
  cout<<"   right: "<<_lright<<","<<_zright<<endl;
}

