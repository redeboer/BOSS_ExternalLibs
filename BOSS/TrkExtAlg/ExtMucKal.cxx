#include "TrkExtAlg/ExtMucKal.h"
#include "MucCalibAlg/MucIdTransform.h"
//#include "MucRawEvent/MucDigi.h"
static const double     Small( 0.01 );
static const double     Infinite( 1.0e10 );
static const double kRad = 57.2958;
//static const int n_sigm = 6;
static const int fix_ = 1;
static const int chi2lim_ = 100;
ExtMucKal::ExtMucKal():FilterOK(false),Chi2_sub(-1),HitExist(false),m_jcb(6,6,0),m_samestrip(false),n_sigm(6)
{;}

ExtMucKal::~ExtMucKal()
{;}

bool ExtMucKal::ExtMucFilter()
{
     
    if(!HitExist) {FilterOK = false;return FilterOK;}
    m_samestrip = m_gapid[0]==m_nearesthit->Part()&& m_gapid[1]==m_nearesthit->Seg()&&m_gapid[2]==m_nearesthit->Gap()&&m_iStrip==m_nearesthit->Strip();
//    cout<<"---Guess Hit---"<<"Part: "<<m_gapid[0]<<" Seg: "<<m_gapid[1]<<" Gap: "<<m_gapid[2]<<" Strip: "<<m_iStrip<<endl;    
//    cout<<"---Near  Hit---"<<"Part: "<<m_nearesthit->Part()<<" Seg: "<<m_nearesthit->Seg()<<" Gap: "<<m_nearesthit->Gap()<<" Strip: "<<m_nearesthit->Strip()<<endl;
    Hep3Vector mom_unit = m_CurrentMomentum;
    mom_unit.setMag(1.0);
    Hep3Vector  IntrPos_loc = m_nearesthit->GetGap()->TransformToGap(m_CurrentInsct);  
    Hep3Vector  mom_loc = m_nearesthit->GetGap()->RotateToGap(m_CurrentMomentum);
    MucGeoGap* nearestGap = m_nearesthit->GetGap();
    //get the orient and sigma of the nearest hit
    m_orient = nearestGap->Orient(); 
    float sx,sy,sz;
    m_nearesthit->GetStrip()->GetCenterSigma(sx, sy, sz);
    if(m_orient==1) m_sigma = sx/sqrt(12.);
    if(m_orient==0) m_sigma = sy/sqrt(12.);

    HepMatrix rotation = GetRoationMatrix(nearestGap);
    HepSymMatrix IntrErr_loc(6,0); 
    IntrErr_loc.assign(rotation*m_CurrentInsctXPErr*rotation.T());
    Hep3Vector modpos_loc;
    Hep3Vector modmom_loc;
    HepSymMatrix moderr_loc(6,0);
    //fitting
    double chisq =Fit(IntrPos_loc,mom_loc,IntrErr_loc);
    modpos_loc.setX(m_bm[0]);modpos_loc.setY(m_bm[1]);modpos_loc.setZ(m_bm[2]);
    modmom_loc.setX(m_bm[3]);modmom_loc.setY(m_bm[4]);modmom_loc.setZ(m_bm[5]);
    moderr_loc = m_Ebm;
bool direction = false;
double dirchange = (modmom_loc.theta()-1.5707963)*(mom_loc.theta()-1.5707963);
if(dirchange<0.) {direction = true; //cout<<"------direction has been changed "<<endl;//disdir++;
}

Hep3Vector modpos_glob = m_nearesthit->GetGap()->TransformToGlobal(modpos_loc);
Hep3Vector modmom_glob = m_nearesthit->GetGap()->RotateToGlobal(modmom_loc);
HepSymMatrix moderr_glob(6,0); 
moderr_glob.assign(rotation.T()*moderr_loc*rotation);

if(chisq>0.&&chisq<100.&&!direction)
{
	//cout<<"filter is OK "<<endl;
	FilterOK=true;
	Chi2_sub = chisq;
	m_err_mod = moderr_glob;
	m_pos_mod = modpos_glob;
	m_mom_mod = modmom_glob;

}                  
return FilterOK;
}



void ExtMucKal::XPmod(Hep3Vector &pos,Hep3Vector &mom,HepSymMatrix &err)
{
	pos = m_pos_mod;
	mom = m_mom_mod;
	err = m_err_mod;
}

vector<MucRecHit*> ExtMucKal::TrackHit()
{       
	vector<MucRecHit*> hitvec;
	vector<MucRecHit*> HitInGap = GapHit();
	int hitsize = HitInGap.size();
	double dist_nearest = 9999.;
	for ( int h = 0;h<hitsize;h++ )
	{       
		
		MucRecHit* myhit = HitInGap[h];
		//cout<<"Gap Hit: "<<"part: "<<myhit->Part()<<"  seg: "<<myhit->Seg()<<"  layer: "<<myhit->Gap()<<"  strip: "<<myhit->Strip()<<endl;
		MucGeoGap* mygap = myhit->GetGap();
		HepMatrix rotation = GetRoationMatrix(mygap); 	
		HepSymMatrix IntrErr_loc(6,0);
		IntrErr_loc.assign(rotation*m_CurrentInsctXPErr*rotation.T()); 
		float sx,sy,sz;
		myhit->GetStrip()->GetCenterSigma(sx,sy,sz);
		double distance = GetDistance(myhit); 
		double window = -9999.;
		int myorient = myhit->GetGap()->Orient();
		if(myorient==1)   //strip parallel y axis 
			window = n_sigm*sqrt(IntrErr_loc(1,1)+sx*sx/12.);
			//window = 4.*sx;
			//{window = n_sigm*sqrt(IntrErr_loc(1,1)+sx*sx/12.);cout<<"sx: "<<sx<<endl;cout<<"window: "<<window<<endl;cout<<"window/strip:  "<<floor(window/sx)<<endl;}
		if(myorient==0)
			window = n_sigm*sqrt(IntrErr_loc(2,2)+sy*sy/12.);
			//window = 4.*sy;
			//{window = n_sigm*sqrt(IntrErr_loc(2,2)+sy*sy/12.);cout<<"sy: "<<sy<<endl;cout<<"window: "<<window<<endl;cout<<"window/strip:  "<<floor(window/sy)<<endl;}
		
		//cout<<"window/strip:  "<<floor(window/sx)<<endl;
		//cout<<"distance: "<<distance<<endl;
		if(fabs(distance)<window)
		{
			hitvec.push_back(myhit);
			if(dist_nearest>fabs(distance)) 
			{
				//cout<<"Hit exist"<<endl;
				HitExist =true;
				dist_nearest=fabs(distance);
				m_nearesthit = HitInGap[h]; 
			}
		}
	}

	return hitvec;
}

double ExtMucKal::GetDistance(const MucRecHit* hit)
{
	if (!hit) {
		cout << "RecMucTrack:GetHitDistance-E1  null hit pointer!" << endl;
		return kInfinity;
	}

	int part = hit->Part();
	int gap  = hit->Gap();
	if ( (gap < 0) || (gap >= (int)MucID::getGapNum(part)) ) {
		cout << "RecMucTrack::GetHitDistance()  bad gap number = " << gap << endl;
		return kInfinity;
	}
	HepPoint3D posHit        = hit->GetCenterPos();
	HepPoint3D posHitLocal   = hit->GetGap()->TransformToGap(posHit);
	HepPoint3D posInsctLocal = hit->GetGap()->TransformToGap(m_CurrentInsct);
	int orient = hit->GetGap()->Orient();
 
	float distance = -9990;
	if(orient == 1) distance = (posHitLocal.x()-posInsctLocal.x());
	if(orient == 0) distance = (posHitLocal.y()-posInsctLocal.y());
        //cout<<"gap hit pos: "<<"x: "<<posHitLocal.x()<<" y: "<<posHitLocal.y()<<" z: "<<posHitLocal.z()<<endl;
	return distance;
}

bool ExtMucKal::MucKalIniti()

{
	MucGeoGap* box = MucGeoGeneral::Instance()->GetGap((int)m_gapid[0],(int)m_gapid[1],(int)m_gapid[2]);
	Hep3Vector mom_unit= m_CurrentMomentum;
	mom_unit.setMag(1.0);
	HepVector3D _interc = box->ProjectToGap(m_CurrentPosition,mom_unit);
	m_CurrentInsct = _interc;
	HepVector3D m_CurrentInsct_loc = box->TransformToGap(m_CurrentInsct);
	int iStrip = box->GuessStrip(m_CurrentInsct_loc.x(),m_CurrentInsct_loc.y(),m_CurrentInsct_loc.z());
	m_iStrip =iStrip;
	//Tuning-------------
	/*
	cout<<"----------------------------------------------------"<<endl;
	cout<<"Insct pos_loc: "<<"x: "<<m_CurrentInsct_loc.x()<<"  y:"<<m_CurrentInsct_loc.y()<<"  z:"<<m_CurrentInsct_loc.z()<<endl;
	cout<<"---Guess Hit---"<<"Part: "<<m_gapid[0]<<" Seg: "<<m_gapid[1]<<" Gap: "<<m_gapid[2]<<" Strip: "<<m_iStrip<<endl;
	int stripMax = box->GetStripNum()-1;
        if(iStrip < stripMax&&iStrip>0 )
        {
	MucRecHit guess_hit(m_gapid[0],m_gapid[1],m_gapid[2],m_iStrip);
        HepPoint3D posHit  = guess_hit.GetCenterPos();	
	HepPoint3D posHit_loc  = box->TransformToGap(posHit);
	cout<<"Guess Hit Pos--- "<<"x: "<<posHit_loc.x()<<" y: "<<posHit_loc.y()<<" z: "<<posHit_loc.z()<<endl;
	}
	*/
	//Tuning--------------
	bool iniOK = false;
	iniOK = JCB();
	m_CurrentInsctXPErr = m_CurrentXPErr.similarity( m_jcb );
       return iniOK;
}
/*
int ExtMucKal::GuesStrip()
{
 MucGeoGap* box = MucGeoGeneral::Instance()->GetGap((int)m_gapid[0],(int)m_gapid[1],(int)m_gapid[2]);
int iStrip = box->GuessStrip(m_CurrentInsct.x(),m_CurrentInsct.y(),m_CurrentInsct.z());

//int stripMax = box->GetStripNum()-1;
//if(iStrip > stripMax||iStrip<0 )

//MucGeoStrip *guesstrip = box->GetStrip(iStrip);

}
*/

vector<MucRecHit*> ExtMucKal::GapHit()
{
	vector<MucRecHit*> gaphit;
	int kDeltaSeg[3]={-1,0,1};
	MucDigiCol::iterator digiIter = m_MucDigiCol->begin();
	for ( ; digiIter != m_MucDigiCol->end(); digiIter++ )
	{
		Identifier mucId   = (*digiIter)->identify();
		int part    = MucID::barrel_ec(mucId);
		int segment = MucID::segment(mucId);
		int layer   = MucID::layer(mucId);
		int strip   = MucID::strip(mucId);
		int m_part =(int)m_gapid[0];
		int m_seg = (int)m_gapid[1];
		int m_gap = (int)m_gapid[2];

		//if(part==0 && segment ==2 && layer ==6 && strip ==60)continue;
		if(m_part==part&&m_gap == layer)
			for(int s=0;s<3;s++)
			{
				int iSeg = m_seg+kDeltaSeg[s];
				if(iSeg<0) iSeg += MucID::getSegNum(m_part);
				if(iSeg>((int)MucID::getSegNum(m_part) - 1))iSeg -= MucID::getSegNum(m_part);
				if(iSeg==segment)
				{
					MucRecHit* hit = new MucRecHit(part,segment,layer,strip);
					gaphit.push_back(hit);
				}
			}
	}
	return gaphit;
}

Hep3Vector ExtMucKal::GetHitGap()
{
m_hitgap.setX(m_nearesthit->Part());
m_hitgap.setY(m_nearesthit->Seg());
m_hitgap.setZ(m_nearesthit->Gap());
return m_hitgap;
}


bool ExtMucKal::JCB()
{
	HepMatrix m_xp_jcb(6,6,0);
	double dx( ( m_CurrentInsct - m_CurrentPosition ).mag() );
	double dp(0.);//dp( ( pv1 - m_pv ).mag() );
	double p2( m_CurrentMomentum.mag2() );
	double p_abs( sqrt( p2 ) );

	double p_inv;
	if( p_abs > Small && m_CurrentMomentum.mag() > Small ){
		p_inv = 1.0 / p_abs;
	} else {
		p_inv = Infinite;
		return 0;
	}
	double p2inv( p_inv * p_inv );
	double p3inv( p2inv * p_inv );
	double fdx( dx * p3inv );

	double fdp( dp * p_inv );
	double px(  m_CurrentMomentum.x() );
	double py(  m_CurrentMomentum.y() );
	double pz(  m_CurrentMomentum.z() );
	double px2( px * px );
	double py2( py * py );
	double pz2( pz * pz );
	double pxy( px * py );
	double pyz( py * pz );
	double pzx( pz * px );
	m_xp_jcb( 1, 1 ) = 1.0;			// dx'/dx
	m_xp_jcb( 1, 4 ) = fdx * ( py2 + pz2 );	// dx'/dpx
	m_xp_jcb( 1, 5 ) = - fdx * pxy;		// dx'/dpy
	m_xp_jcb( 1, 6 ) = - fdx * pzx;		// dx'/dpz
	m_xp_jcb( 2, 2 ) = 1.0;			// dy'/dy
	m_xp_jcb( 2, 4 ) = - fdx * pxy;		// dy'/dpx
	m_xp_jcb( 2, 5 ) = fdx * ( pz2 + px2 );	// dy'/dpy
	m_xp_jcb( 2, 6 ) = - fdx * pyz;		// dy'/dpz

	m_xp_jcb( 3, 3 ) = 1.0;			// dz'/dz
	m_xp_jcb( 3, 4 ) = - fdx * pzx;		// dz'/dpx
	m_xp_jcb( 3, 5 ) = - fdx * pyz;		// dz'/dpy
	m_xp_jcb( 3, 6 ) = fdx * ( px2 + py2 );	// dz'/dpz

	m_xp_jcb( 4, 4 ) = 1.0 - fdp;			// dx'/dx energy loss
	m_xp_jcb( 5, 5 ) = 1.0 - fdp;			// dy'/dy energy loss
	m_xp_jcb( 6, 6 ) = 1.0 - fdp;			// dz'/dz energy loss
       m_jcb = m_xp_jcb;
       return 1;
	// m_CurrentInsctXPErr = m_CurrentInsct.similarity( m_xp_jcb );
}

HepMatrix  ExtMucKal::GetRoationMatrix(MucGeoGap *box)//from global to local
{

	//cout<<"now in ExtMucKalTransfor::GetRoationMatrix() "<<endl;
	MucGeoGap* box_ = box;
	float thetaX,phiX,thetaY,phiY,thetaZ,phiZ;
	box->GetRotationMatrix(thetaX,phiX,thetaY,phiY,thetaZ,phiZ);
	//cout<<"after get m_box->GetRotationMatrix "<<endl;
	thetaX=thetaX/kRad;
	thetaY=thetaY/kRad;
	thetaZ=thetaZ/kRad;
	phiX=phiX/kRad;
	phiY=phiY/kRad;
	phiZ=phiZ/kRad;
	Hep3Vector colX, colY,colZ;
	colX.setRThetaPhi(1.,thetaX,phiX);
	colY.setRThetaPhi(1.,thetaY,phiY);
	colZ.setRThetaPhi(1.,thetaZ,phiZ);
	//cout<<"will give value to rotation "<<endl;
	HepMatrix rotation(6,6,0);
	rotation(1,1) = colX.x();
	rotation(1,2) = colX.y();
	rotation(1,3) = colX.z();
	rotation(2,1) = colY.x();
	rotation(2,2) = colY.y();
	rotation(2,3) = colY.z();
	rotation(3,1) = colZ.x();
	rotation(3,2) = colZ.y();
	rotation(3,3) = colZ.z();

	rotation(4,4) = colX.x();
	rotation(4,5) = colX.y();
	rotation(4,6) = colX.z();
	rotation(5,4) = colY.x();
	rotation(5,5) = colY.y();
	rotation(5,6) = colY.z();
	rotation(6,4) = colZ.x();
	rotation(6,5) = colZ.y();
	rotation(6,6) = colZ.z();
	return rotation;
}


double  ExtMucKal::Fit( Hep3Vector & pos,   Hep3Vector & mom, HepSymMatrix & Epm)
{
	static HepVector a(6,0);
        a[0] = pos.x();
	a[1] = pos.y();
	a[2] = pos.z();
	a[3] = mom.x();//protect from NaN
	a[4] = mom.y();
	a[5] = mom.z();
        m_bm = a;
	m_Ebm = Epm;

	HepMatrix V_meas(1,1,0);
	V_meas(1,1)= m_sigma*m_sigma;
	double mag2(m_bm[5]*m_bm[5]+m_bm[4]*m_bm[4]+m_bm[3]*m_bm[3]);
	//static HepSymMatrix ebm(6,0);
	HepSymMatrix ebm(6,0);
	ebm.assign(m_Ebm);
	//static HepVector v_bm(6,0);
	HepVector v_bm(6,0);
	//v_bm[2] = bm()[2];//because bm0 is never modified!
	v_bm = m_bm;
	v_bm[2] = 0;
	//static HepMatrix H(1,6,0);
	HepMatrix H(1,6,0);
	if(m_orient == 1) H(1,1) = 1;//;if(part==1&&gap==0)ebm(1,1)=ebm(1,1)*1.6;}// H[1][1] = 1;
	if(m_orient == 0) H(1,2) = 1;//;if(part==1&&gap==1)ebm(2,2)=ebm(2,2)*2.4;}//H[1][2] = 1;
	HepMatrix ebm_HT(6,1,0);
	ebm_HT = ebm * H.T();
        
	HepMatrix R(1,1,0);
        
	HepMatrix R2(1,1,0);
	HepMatrix Aii(1,1,0);
	Aii(1,1)=(H * ebm_HT)(1,1);
	R = H * ebm_HT + V_meas;
	//double r = (H * ebm_HT)(1,1);
	R2 = Aii + V_meas;
	double aii = Aii(1,1);
	//static HepMatrix K(6,1,0);
	HepMatrix K(6,1,0);
	int ierr;
	K = ebm_HT * R.inverse(ierr);//Kalman gain matrix
	double dist = GetDistance(m_nearesthit);
	double r = R2(1,1);
	double pull;
	if(r==0.)pull = 9999;
	else
	pull = dist/sqrt(r);
	m_pull = pull;
//	cout<<"in filter distance between hit and track is "<<dist<<endl;
	//static HepVector diff_v_bm(6, 0);
	HepVector diff_v_bm(6, 0);
	diff_v_bm = K * dist;

	//// check diff_v_bm! sometime huge diff_v_bm make serious SEGV.
	//   So, if diff is more than 2 time of window,discard it.
	if ((fabs(diff_v_bm[0]) > 2 * n_sigm * sqrt(V_meas(1,1)+fabs(m_Ebm(1,1)))&&m_orient==1) ||
			(fabs(diff_v_bm[1]) > 2 * n_sigm * sqrt(V_meas(1,1)+fabs(m_Ebm(2,2)))&&m_orient==0) ){
//#ifdef DEBUG
		cout<<"KLMK:WARNING! Huge diff_v_bm found!Discard this fit!:  "<<endl;
		return 99999;
	}////////////////////////////////////

	HepVector v_bm_mod = v_bm + diff_v_bm;

	static const HepSymMatrix Id(6,1);
	HepMatrix ebm_mod(6,6,0);
	ebm_mod = (Id - K*H) * ebm;
	double r_bm;
	r_bm = (1 - (H*K)(1,1))*dist;
	//static HepMatrix Rk(2,2);
	double Rk;
	Rk = (V_meas - H * ebm_mod * H.T())(1,1);
	double chi2;
	if(Rk==0.)
	chi2 = 99999;
	else
	chi2 = pull*pull;
	m_pull = pull;
	if (chi2>0 && chi2 < chi2lim_) {
		HepSymMatrix ebm_replace;
		ebm_replace.assign(ebm_mod);
		HepVector v_bm_replace;
		v_bm_replace = v_bm_mod;
		v_bm_replace[2] = 0;
		m_bm = v_bm_replace;
	        m_Ebm= ebm_replace;
		if(fix_)
		{
		double mag2mod(m_bm[5]*m_bm[5]+m_bm[4]*m_bm[4]+m_bm[3]*m_bm[3]);
		v_bm_replace[3] = v_bm_replace[3]*sqrt(mag2)/sqrt(mag2mod);
		v_bm_replace[4] = v_bm_replace[4]*sqrt(mag2)/sqrt(mag2mod);
		v_bm_replace[5] = v_bm_replace[5]*sqrt(mag2)/sqrt(mag2mod);
		m_bm=v_bm_replace;
		}
		
		// Keep magnitude of momentum
		//double bm52(mag2/(1+(bm()[3]*bm()[3]+bm()[4]*bm()[4]))
		//		*(1+bm()[3]*bm()[3]));
		//v_bm_replace[5] = sqrt(bm52);
		//if (fix_) bm(v_bm_replace);  //I can't confirm if it is needed or not.
	}
  
 //       cout<<" in filter chisq is "<<chiSq<<endl;	
	return chi2;
}




