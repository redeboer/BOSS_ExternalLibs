#ifndef EVENTFILTER_TOFHITSCOL_H
#define EVENTFILTER_TOFHITSCOL_H 1
#include <vector>
class EFTofHitsCol{
  // class collect TOF information with certain character, such as top vs bottom
 public:
  EFTofHitsCol(unsigned int ib_e=0,unsigned int ilayer=0,unsigned int iphi=0,double tof=0.);
  virtual ~EFTofHitsCol();
  unsigned int ib_e(){return barrel_ec;};
  unsigned int ilayer(){return layer;};
  unsigned int iphi(){return phi;};
  double GetT(){return time;};
  unsigned int nhit(){return num;};
  void SetT(double val=0.);

 private:
  unsigned int barrel_ec;
  unsigned int layer;
  unsigned int phi;
  double time;
  unsigned int num;
};
typedef std::vector<EFTofHitsCol> VTOF;
//#include "HltAlgorithms/EFTofHitsCol.icc" 
#endif
