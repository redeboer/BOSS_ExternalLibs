#include "include/MdcCosGeom.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <cstdlib>

using namespace std;

MdcCosGeom::MdcCosGeom(std::string fwireconf, std::string falign):
     m_fwconf(fwireconf), m_falign(falign)
{
     // constructor
     int ilay;
     int icell;
     for(ilay=0; ilay<LAYERNMAX; ilay++){
	  for(icell=0; icell<CELLNMAX; icell++){
	       m_wiretable[ilay][icell] = -1;
	  }
     }

     for(int i=0; i<12; i++){
	  m_iTXf = 0;
	  m_iTXb = 8;
	  m_iTYf = 16;
	  m_iTYb = 24;
	  m_iTZf = 32;
	  m_iTZb = 40;

	  m_iRXf = 48;
	  m_iRXb = 56;
	  m_iRYf = 64;
	  m_iRYb = 72;
	  m_iRZf = 80;
	  m_iRZb = 88;
     }

     m_rot = 0.0;

     int ipream;
     for(ilay=0; ilay<LAYERNMAX; ilay++){
	  for(ipream=0; ipream<100; ipream++){
	       m_fan[ilay][ipream] = -999;
	  }
     }

     cout << "INFO:    MdcCosGeom created." << endl;
}

MdcCosGeom::~MdcCosGeom(){
     // destructor
     int i;
     for(i=0; i<WIRENMAX; i++){
	  delete m_pwire[i];
     }
     for(i=0; i<LAYERNMAX; i++){
	  delete m_player[i];
     }

     cout << "INFO:    MdcCosGeom has been destructed." << endl;
}

bool MdcCosGeom::initialize(double rot){
     m_rot = rot * PI / 180.0;
     if( ! initWire() ){
	  cout << "ERROR:  Intialize wire failed !!!" << endl;
	  return false;
     }
     return true;
}

bool MdcCosGeom::initWire(){
     int iwire = 0;
     int ilay;
     int icell;
     int ampid;
     int channel;
     int iFan;

     char wirelabel[100];
     char wiretag[100];

     int    ncell;
     int    first;
     double radius;
     double twist;
     double length;
     double offset;
     string strtmp;

     double aglstart;
     double angleforwini;
     double angleforw;
     double angleback;

     int layertype;

     double xf;
     double yf;
     double zf;
     double xb;
     double yb;
     double zb;

     double slant;

     double tension[] = {15, 15, 15, 16, 16, 17, 17, 18, 14, 14,
			 19, 19, 24, 24, 31, 31, 37, 37, 45, 45,
			 46, 47, 47, 47, 47, 48, 48, 48, 48, 49,
			 49, 49, 49, 50, 50, 50, 51, 51, 51, 52,
			 52, 52, 52};

     int iEle;			// index of the element for alignment
     int iPar[12];		// index of alignment paramter

     if( ! initAlign() ){
	  return false;
     }

     if( ! initPream() ){
	  return false;
     }

     ifstream fin( m_fwconf.c_str() );
     if( ! fin.is_open() ){
	  cout << "ERROR:   can not open file " << m_fwconf << endl;
	  return false;
     }

     for(ilay=0; ilay<LAYERNMAX; ilay++){
	  fin >> strtmp >> radius >> ncell >> twist
	      >> offset >> length >> first;

	  length -= 4.0;	// wulh test on 2007-05-08
	  ncell /= 2;

	  if( fabs(twist) > 0.1 ){
	       offset = 0.0;
	  }

	  aglstart = (PI / ncell) * first + (offset * PI / 180.0);
	  if( aglstart < -0.000001 ){
	       aglstart += 2.0 * PI / ncell;
	  }

	  if( twist < -0.1 ){
	       layertype = 1;
	  } else if( twist > 0.1 ){
	       layertype = 2;
	  } else{
	       layertype = 0;
	  }

	  slant = atan( sin(PI * twist / ncell) 
			  * radius * 2.0 / length );

	  m_player[ilay] = new MdcCosLayer;
	  m_player[ilay] -> setLayerType( layertype );
	  m_player[ilay] -> setNcell( ncell );
	  m_player[ilay] -> setLayerRad( radius );
	  m_player[ilay] -> setLength( length );
	  m_player[ilay] -> setTwistCell( twist );
	  m_player[ilay] -> setSlant( slant );

	  if( ilay < INNERNMAX )  m_player[ilay] -> setFgInner( true );
	  else                    m_player[ilay] -> setFgInner( false );

	  // set the index of the element for alignment
	  if( ilay < 8 ){
	       iEle = 0;
	  } else if( ilay < 10 ){
	       iEle = 1;
	  } else if( ilay < 12 ){
	       iEle = 2;
	  } else if( ilay < 14 ){
	       iEle = 3;
	  } else if( ilay < 16 ){
	       iEle = 4;
	  } else if( ilay < 18 ){
	       iEle = 5;
	  } else if( ilay < 20 ){
	       iEle = 6;
	  } else{
	       iEle = 7;
	  }

	  iPar[0] = iEle + m_iTXf;	// TXf
	  iPar[1] = iEle + m_iTXb;	// TXb
	  iPar[2] = iEle + m_iTYf;	// TYf
	  iPar[3] = iEle + m_iTYb;	// TYb
	  iPar[4] = iEle + m_iTZf;	// TZf
	  iPar[5] = iEle + m_iTZb;	// TZb
	  iPar[6] = iEle + m_iRXf;	// RXf
	  iPar[7] = iEle + m_iRXb;	// RXb
	  iPar[8] = iEle + m_iRYf;	// RYf
	  iPar[9] = iEle + m_iRYb;	// RTb
	  iPar[10] = iEle + m_iRZf;	// RZf
	  iPar[11] = iEle + m_iRZb;	// RZb

	  m_player[ilay] -> setAlignParId(iPar);

	  for(icell=0; icell<ncell; icell++){
	       m_wiretable[ilay][icell] = iwire;
	       angleforwini = (double)icell * 2.0 * PI / ncell + aglstart;

	       // forward endpoint
	       angleforw = angleforwini + m_alignPar[iEle+m_iRZf];
	       angleforw += m_rot;

	       xf = radius * cos( angleforw );
	       yf = radius * sin( angleforw );
	       zf = 0.5 * length;

	       xf += m_alignPar[iEle+m_iTXf];
	       yf += m_alignPar[iEle+m_iTYf];
	       zf += m_alignPar[iEle+m_iTZf];

	       // backward endpoint
	       angleback = angleforwini + twist * 2.0 * PI / ncell;
	       angleback += m_alignPar[iEle+m_iRZb];
	       angleback += m_rot;

	       xb = radius * cos( angleback );
	       yb = radius * sin( angleback );
	       zb = -0.5 * length;

	       xb += m_alignPar[iEle+m_iTXb];
	       yb += m_alignPar[iEle+m_iTYb];
	       zb += m_alignPar[iEle+m_iTZb];

	       ampid = icell / 8;
	       channel = icell - 8 * ampid;
	       sprintf(wirelabel, "%02dS%02d-%d", ilay+1, ampid+1, channel+1);
	       sprintf(wiretag, "%02d_%02d_%04d_%02d", 
		       ilay, icell, iwire, ampid+1);

	       m_pwire[iwire] = new MdcCosWire;

	       m_pwire[iwire] -> setLayerPtr(m_player[ilay]);

	       m_pwire[iwire] -> setWireId( iwire );
	       m_pwire[iwire] -> setLayerId( ilay );
	       m_pwire[iwire] -> setCellId( icell );
	       m_pwire[iwire] -> setPreAmpId( ampid+1 );
	       m_pwire[iwire] -> setChannel( channel );

	       // set fanID
	       iFan = m_fan[ilay][ampid];
// 	       m_pwire[iwire] -> setFanId( abs(iFan) - 1 );
	       m_pwire[iwire] -> setFanId( abs(iFan) ); // begin from 1
	       if(iFan > 0) m_pwire[iwire] -> setEndId( 1 );
	       else         m_pwire[iwire] -> setEndId( -1 );

	       m_pwire[iwire] -> setWireLabel( wirelabel );
	       m_pwire[iwire] -> setWireTag( wiretag );

	       m_pwire[iwire] -> setXf( xf );
	       m_pwire[iwire] -> setYf( yf );
	       m_pwire[iwire] -> setZf( zf );

	       m_pwire[iwire] -> setXb( xb );
	       m_pwire[iwire] -> setYb( yb );
	       m_pwire[iwire] -> setZb( zb );

	       if( angleforw < 0 ) angleforw += 2.0 * PI;
	       m_pwire[iwire] -> setPhiForward( angleforw );

	       m_pwire[iwire] -> setTension( tension[ilay] );

	       iwire++;
	  }
     }
     fin.close();
     cout << "INFO:    Number of wires: " << iwire << endl;
     m_nWire = iwire;
     m_nLayer = LAYERNMAX;

     return true;
}

bool MdcCosGeom::initAlign(){
     ifstream falign( m_falign.c_str() );
     if( ! falign.is_open() ){
	  cout << "ERROR:   can not open alignment data file " 
	       << m_falign << endl;
	  return false;
     }

     cout << "INFO:     Alignment data file is " << m_falign << endl;

     int i;
     string str;
     getline(falign, str);

     for(i=0; i<8; i++){
	  falign >> str >> m_alignPar[i+m_iTXf] >> m_alignPar[i+m_iTYf]
		 >> m_alignPar[i+m_iTZf] >> m_alignPar[i+m_iRXf]
		 >> m_alignPar[i+m_iRYf] >> m_alignPar[i+m_iRZf];

     // mrad -> rad
	  m_alignPar[i+m_iRXf] *= 0.001;
	  m_alignPar[i+m_iRYf] *= 0.001;
	  m_alignPar[i+m_iRZf] *= 0.001;
     }

     for(i=0; i<8; i++){
	  falign >> str >> m_alignPar[i+m_iTXb] >> m_alignPar[i+m_iTYb]
		 >> m_alignPar[i+m_iTZb] >> m_alignPar[i+m_iRXb]
		 >> m_alignPar[i+m_iRYb] >> m_alignPar[i+m_iRZb];

     // mrad -> rad
	  m_alignPar[i+m_iRXb] *= 0.001;
	  m_alignPar[i+m_iRYb] *= 0.001;
	  m_alignPar[i+m_iRZb] *= 0.001;
     }

     falign.close();

     return true;
}

bool MdcCosGeom::initPream(){
     int i;
     int k;
     int iEnd;
     char fname[100];

     int lay;
     int ipream;

     for(k=0; k<2; k++){
	  for(i=0; i<8; i++){
	       if(0 == k){
		    sprintf(fname, "pream/east%d.dat", i+1);
		    iEnd = 1;
	       } else{
		    sprintf(fname, "pream/west%d.dat", i+1);
		    iEnd = -1;
	       }

	       ifstream fin(fname);
	       while(fin >> lay){
		    fin >> ipream;
		    m_fan[lay-1][ipream-1] = (i+1) * iEnd;
	       }
	       fin.close();
	       fin.clear();
	  }
     }
     return true;
}

bool MdcCosGeom::setTension(){
     return true;
}

void MdcCosGeom::check(string fchk) const {
	  ofstream fout( fchk.c_str() );
	  for(int iwire=0; iwire<6796; iwire++){
	       fout << setw(4) << iwire;
	       fout << setw(6) << getWire(iwire) -> getWireId();
	       fout << setw(4) << getWire(iwire) -> getLayerId();
	       fout << setw(5) << getWire(iwire) -> getCellId();
	       fout << setw(15) << getWire(iwire) -> getXb();
	       fout << setw(15) << getWire(iwire) -> getYb();
	       fout << setw(7) << getWire(iwire) -> getZb();
	       fout << setw(15) << getWire(iwire) -> getXf();
	       fout << setw(15) << getWire(iwire) -> getYf();
	       fout << setw(7) << getWire(iwire) -> getZf();
	       fout << endl;
	  }
	  fout.close();

	  cout << fchk << " has been created for check." << endl;
}

const MdcCosWire* MdcCosGeom::getWire(int ilay, int icell) const {
     assert( ilay < LAYERNMAX );
     assert( icell < getLayer(ilay)->getNcell() );

     int iwire = m_wiretable[ilay][icell];

     return m_pwire[iwire];
}

const void MdcCosGeom::getAlignPar(double alignPar[]) const {
     for(int i=0; i<96; i++){
	  alignPar[i] = m_alignPar[i];
     }

     for(int k=0; k<8; k++){
	  alignPar[k+m_iRXf] *= 1000.0;
	  alignPar[k+m_iRYf] *= 1000.0;
	  alignPar[k+m_iRZf] *= 1000.0;
     }
}
