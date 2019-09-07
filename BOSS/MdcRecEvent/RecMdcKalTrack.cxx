/*
 *                                                                              
 *  this class models "Kalman Track" used in Mdc Reconstruction        
 *
 * ********************************************************/

#include "MdcRecEvent/RecMdcKalTrack.h"
//#include "MdcRecEvent/MdcKalHelixSeg.h"

#include <cstdio>
#include <fstream>

RecMdcKalTrack::RecMdcKalTrack() 
:DstMdcKalTrack() 
{
	HepVector a(5,0);
	HepSymMatrix Ea(5,0);
	HepPoint3D poca(0,0,0);
	HepPoint3D point(0.,0.,0);
	HepPoint3D pivot(0.,0.,0);

	m_ta(5,0);
	m_tEa(5,0);
	m_trackId = -1;

	pidSeg=4;
	
	for(int pid=0; pid<5; pid++){
		m_lhelixs.push_back(a);
		m_lerrors.push_back(Ea);
		//    m_fhelixs.push_back(a);
		//    m_ferrors.push_back(Ea);
		m_lpoints.push_back(point);
		m_lpivots.push_back(pivot);

		m_length[pid] = 0.0;
		m_tof[pid] = 0.0;
		m_nhits[pid] = 0;
		m_pathSM[pid] = 0.0;
		m_fiTerm[pid] = 0.0;
		m_stat[0][pid] = -1;
		m_stat[1][pid] = -1;
		m_nster[0][pid] = -1;
		m_nster[1][pid] = -1;
		m_firstLayer[0][pid] = 0;
		m_firstLayer[1][pid] = 0;
		m_lastLayer[0][pid] = 43; 
		m_lastLayer[1][pid] = 43;
		m_chisq[0][pid] = 999.0;	
		m_chisq[1][pid] = 999.0;   
		m_ndf[0][pid] = 0;
		m_ndf[1][pid] = 0;
	}
	for( int i=0; i<43; i++) {   
		m_pathl[i] = 0.0;
	}
	for(int j=0; j<5; j++) {
		m_thelix[j] = 0.0;
	}
	for(int k=0; k<15; k++){
		m_terror[k]= 0.;
	}
}


RecMdcKalTrack::RecMdcKalTrack( const RecMdcKalTrack& trk )
:DstMdcKalTrack(trk)
{   
	for(int i=0; i<5; i++) m_vechelixsegs[i]=trk.m_vechelixsegs[i];
}

RecMdcKalTrack::RecMdcKalTrack( const DstMdcKalTrack& dstrk )
:DstMdcKalTrack(dstrk)
{
	HepVector a(5,-99);
	HepSymMatrix Ea(5,-99);
	HepPoint3D poca(-99.,-99.,-99.);
	HepPoint3D point(0.,0.,0);
	HepPoint3D pivot(0.,0.,0);

	m_ta(5,0);
	m_tEa(5,0);

	//m_trackId = -1;
	for(int pid=0; pid<5; pid++){
		//m_pocas.push_back(poca);
		//m_zhelixs.push_back(a);
		//m_zerrors.push_back(Ea);
		m_lhelixs.push_back(a);
		m_lerrors.push_back(Ea);
		//   m_fhelixs.push_back(a);
		//   m_ferrors.push_back(Ea);
		m_lpoints.push_back(point);
		m_lpivots.push_back(pivot);

		m_length[pid] = 0.0;
		m_tof[pid] = 0.0;
		m_nhits[pid] = 0;
		m_pathSM[pid] = 0.0;
		m_fiTerm[pid] = 0.0;

		m_stat[0][pid] = -1;
		m_stat[1][pid] = -1;
		m_nster[0][pid] = -1;
		m_nster[1][pid] = -1;
		m_firstLayer[0][pid] = 0;
		m_firstLayer[1][pid] = 0;
		m_lastLayer[0][pid] = 43;
		m_lastLayer[1][pid] = 43;
		m_chisq[0][pid] = 999.0;
		m_chisq[1][pid] = 999.0;
		m_ndf[0][pid] = 0;
		m_ndf[1][pid] = 0;
	}
	for(int i=0; i<43; i++) {
		m_pathl[i] = 0.0;
	}
	for(int j=0; j<5; j++) {
		m_thelix[j] = 0.0;
	}
	for(int k=0; k<15; k++){
		m_terror[k]= 0.;
	}
}

RecMdcKalTrack& RecMdcKalTrack::operator=(const DstMdcKalTrack& dstrk)
{
	if( &dstrk != this ){
		DstMdcKalTrack::operator=(dstrk);
		HepVector a(5,0);
		HepSymMatrix Ea(5,0);
		HepPoint3D poca(-99.,-99.,-99.);
		HepPoint3D point(0.,0.,0.); 
		HepPoint3D pivot(0.,0.,0.);

		//m_trackId = -1;
		//m_pocas.clear();
		//m_zhelixs.clear();
		//m_zerrors.clear();
		m_lhelixs.clear();
		m_lerrors.clear();
		//   m_fhelixs.clear();
		//   m_ferrors.clear();
		m_lpoints.clear();
		m_lpivots.clear();
		m_ta(5,0) ;  
		m_tEa(5,0);
		for(int pid=0; pid<5; pid++){
			//m_pocas.push_back(poca);
			//m_zhelixs.push_back(a);
			//m_zerrors.push_back(Ea);
			m_lhelixs.push_back(a);
			m_lerrors.push_back(Ea);
			//     m_fhelixs.push_back(a);
			//     m_ferrors.push_back(Ea);
			m_lpoints.push_back(point);


			m_length[pid] = 0.0;
			m_tof[pid] = 0.0;
			m_nhits[pid] = 0;
			m_pathSM[pid] = 0.;
			m_fiTerm[pid] = 0.;
		}

		for(int i=0; i<43; i++) {
			m_pathl[i] = 0.0;
		}
		for(int j=0; j<5; j++) {
			m_thelix[j] = 0.0;
		}
		for(int k=0; k<15; k++){
			m_terror[k]= 0.;
		}
	}
	return  *this;
}

// 2011-05-06
HelixSegRefVec RecMdcKalTrack::getVecHelixSegs(int pid) const 
{
	if(pid>=0&&pid<5) return m_vechelixsegs[pid];
	else {
		return m_vechelixsegs[pidSeg];
	}
}

// 2011-05-06
void RecMdcKalTrack::setVecHelixSegs(const HelixSegRefVec& vechelixsegs, int pid){
	if(pid>=0&&pid<5) {
		m_vechelixsegs[pid] = vechelixsegs;
		pidSeg=pid;
	}
	else std::cout<<"RecMdcKalTrack::setVecHelixSegs error, wrong particle id"<<std::endl;
}

RecMdcKalTrack::~RecMdcKalTrack() {

}
