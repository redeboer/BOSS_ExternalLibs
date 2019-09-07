//
#include "HltAlgorithms/EFTofHitsCol.h"

EFTofHitsCol::EFTofHitsCol
(unsigned int iib_e,unsigned int iilayer,unsigned int iiphi,double tof):
  barrel_ec(iib_e),layer(iilayer),phi(iiphi),time(tof),num(1){
}

EFTofHitsCol::~EFTofHitsCol(){
}

void EFTofHitsCol::SetT(double val){
 if(val>0&&val<3000){
  time= (time*num+val)/(num+1);
  num++;
 }	
}
