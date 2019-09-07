#include "DQADtagAlg/util/MyUtil.h"

void fill(NTuple::Array<double>& nt_p4, const HepLorentzVector& p4) {
   nt_p4[0] = p4.x();
   nt_p4[1] = p4.y();
   nt_p4[2] = p4.z();
   nt_p4[3] = p4.e();
}

void fill(NTuple::Matrix<double>& nt_p4, const HepLorentzVector& p4, int i) {
   nt_p4[i][0] = p4.x();
   nt_p4[i][1] = p4.y();
   nt_p4[i][2] = p4.z();
   nt_p4[i][3] = p4.e();
}
HepLorentzVector getP4(RecEmcShower* gtrk) {
   double eraw = gtrk->energy();
   double phi  = gtrk->phi();
   double the  = gtrk->theta();

   double pxy  = eraw * sin(the);

   HepLorentzVector ptrk( pxy  * cos(phi),
	                  pxy  * sin(phi),
			  eraw * cos(the),
			  eraw );

   return ptrk;
}
