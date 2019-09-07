#ifndef  HoughZsFit_H
#define HoughZsFit_H
#include "MdcHoughFinder/HoughStereo.h"
class HoughZsFit
{
  public:
	HoughZsFit(vector<HoughRecHit>* recHitCol);
	~HoughZsFit();
//	typedef std::pair<double,double> Point;
	struct Line{
	  double _k;
	  double _b;
	  double _chi;
	  //std::vector<HoughRecHit> _recCol;
	  std::vector<HoughRecHit> _pointCol;
	  std::vector<int> _ambig;
	};
	void leastFit(Line &linefit, int N);
	int leastLine(int ,double x[],double y[],double &,double &,double& );
	void  doit();
	void  initPoint();
	double getTanl()  const {return _tanl;}
	double getZ0()    const {return _z0;}
	double getPro()    const {return _pro_correct;}
	void sortHit();
	void print();
	static int m_debug;

  private:
	vector<HoughRecHit*> _recStereoHit; 
//	Point**  _vecPoint;
	int    _hitSize;
	double _k;
	double _b;
	double _chi;
//	int    _ambig;
	double _z0;
	double _tanl;
	double _pro_correct;
};
bool compare_zsfit(const HoughZsFit::Line& a,const HoughZsFit::Line& b);
bool layer_in_track(const HoughRecHit* hita,const HoughRecHit* hitb);
#endif
