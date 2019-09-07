#ifndef  HOUGHRECHIT_H
#define  HOUGHRECHIT_H
#include "MdcHoughFinder/HoughHit.h"
#include <utility>
class Hough2D;
class HoughRecHit : public HoughHit{
  public:
	HoughRecHit();
	HoughRecHit(const HoughRecHit& other);
	HoughRecHit& operator=(const HoughRecHit& other);
	HoughRecHit(const MdcDigi* const digi,double disToTrack,double disToCir,int flagLs);
	HoughRecHit(const HoughHit& houghhit,double disToTrack,double disToCir,int flagLs);

	Hough2D* getPtr2D()     const {return _p_hough2D;}
	double getDisToTrack()  const {return _disToTrack;}
	double getDisToCir()  const {return _disToCir;}
	std::pair<double,double> getRet()  const { return _theta_l; }
	double getzPos()   const {return _zPos;}
	double getsPos()   const {return _sPos;}
	double getzAmb(int i)   const {return _zAmb[i];}
	double getsAmb(int i)   const {return _sAmb[i];}
	double getAmbig()       const {return _ambig;}
	int getflag()        const {return _flagLS;}
	int getnsol()        const {return _n_sol;}

	void   setPtr2D( Hough2D* p_hough2D)  {_p_hough2D=p_hough2D;}
	void   setDisToTrack(double dis)  {_disToTrack= dis;}
	void   setDisToCir(double dis)  {_disToCir= dis;}
	void   setRet(std::pair<double,double> theta_l) {_theta_l=theta_l;}
	void   setzPos(double zPos) {_zPos=zPos;}
	void   setsPos(double sPos) {_sPos=sPos;}
	void   setzAmb(int i,double zPos) {_zAmb[i]=zPos;}
	void   setsAmb(int i,double sPos) {_sAmb[i]=sPos;}
	void   setflag(int i) {_flagLS=i;}
	void   setnsol(int i) {_n_sol=i;}

	void   setAmb(int i); 

  private:
	Hough2D *_p_hough2D;
	double _disToTrack;
	double _disToCir;
	std::pair<double,double> _theta_l;
	int    _ambig;
	double _zPos;
	double _sPos;
	double _zAmb[2];  //1 left 2 right
	double _sAmb[2];
	int _flagLS;
	int _n_sol;
};
#endif
	
