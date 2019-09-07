#include "VertexFit/GammaShape.h"
#include <iostream>
#include <cmath>


GammaShape::GammaShape() {
  m_tail = 0;
  m_width = 0;
  m_peak = 0;
  m_e = 0;
  m_de = 0;
  m_dphi = 0;
  m_dtheta = 0;
}

GammaShape::GammaShape(const GammaShape &gammashape) {
  m_tail = gammashape.m_tail;
  m_width = gammashape.m_width;
  m_peak = gammashape.m_peak;
  m_e = gammashape.m_e;
  m_de = gammashape.m_de;
  m_dphi = gammashape.m_dphi;
  m_dtheta = gammashape.m_dtheta;
}
 
GammaShape & GammaShape :: operator = (const GammaShape &gammashape) {
  m_tail = gammashape.m_tail;
  m_width = gammashape.m_width;
  m_peak = gammashape.m_peak;
  m_e = gammashape.m_e;
  m_de = gammashape.m_de;
  m_dphi = gammashape.m_dphi;
  m_dtheta = gammashape.m_dtheta;
  return (*this);
}

GammaShape::GammaShape(const double eorigin, const double peak) {
  m_tail = tail(peak);
  m_width = width(peak);
  m_peak = peak;
  m_e = eorigin;
  m_de = de(eorigin, peak);
  m_dphi = 0;
  m_dtheta = 0;
}


GammaShape::GammaShape(const HepLorentzVector p4, const double dphi, const double dthe, const double de) {
  m_tail = 0;
  m_width = 0;
  m_peak = 0;
  m_e = 0;
  m_de = 0;
  m_dphi = dphi;
  m_dtheta = dthe;
}


//double GammaShape::peak(const double fitvalue){
//   if(fitvalue>0.2) return 0.008488 + 0.9971*fitvalue + 0.006237*fitvalue*fitvalue;
//   else return 0.00409 + 0.9561*fitvalue + 0.1232*fitvalue;
//}

//double GammaShape::tail(const double peak){
//   if(peak>0.2){
//        return -0.8446+0.486*peak-0.1574*peak*peak-0.003065*peak*peak*peak;
//   }else return -0.4676-6.217*peak+30.45*peak*peak-53.39*peak*peak*peak;
//// return -0.5253-0.7995*peak+0.9823*peak*peak-0.2942*peak*peak*peak;
//}


//double GammaShape::width(const double peak){
//   if(peak>0.2){
//     return -0.000736/sqrt(peak) + 0.00423 + 0.02175*peak; //boss 6.5.0 absolute resolution
//   
////   return 0.001822/sqrt(peak)+0.02483-0.001221*peak;   //boss 6.5.0
//   }else return -2.886*0.00001/sqrt(peak) + 0.000832 + 0.0217*peak;
//// return 0.002123/sqrt(peak)+0.01877+0.0005207*peak;
//}

double GammaShape::tail(const double peak){
    return -0.7931 + 0.8026*peak - 0.6694*peak*peak + 0.1476*peak*peak*peak;
}

double GammaShape::width(const double peak){
     return 0.002496/sqrt(peak) + 0.01861 + 0.0001788*peak;
}

double GammaShape::mean(const double peak) {
     return -0.7984 + 0.7759*peak - 0.6356*peak*peak + 0.1476*peak*peak*peak; 
}

double GammaShape::de(double eorigin, double peak) {
  double f1;
  double tail1 = tail(peak);
  double width1 = width(peak);
  double mean1 = mean(peak);
  double qa = tail1*sqrt(log(4.));
  double qb = sinh(qa)/qa;
  double qx = (eorigin-peak)/width1*qb*tail1;
//cout<<"qx ="<<qx<<endl;
//  f1 = (eorigin-peak)/fitm(eorigin,peak, width1, tail1);

  if(fabs(qx)>0.001){
    f1 = (eorigin-peak)/fitm(eorigin,peak, width1, tail1, mean1);
//    cout<<"dynamic"<<endl;
  }
  else { 
//    f1=width1/qb;
    f1 = width1/(qb*tail1);
//    cout<<"gaus"<<endl;
  }

 // else f1 = 0;
//  return f1*eorigin;
  return f1;
}
 


//for this funtion, becaus I get the parameters from the relative value of the energy,
//I can the x to x/pk
double GammaShape::fitm(double x, double pk, double width, double tail, double mean) {
    
  double qa = tail*sqrt(log(4.));
  double qb = sinh(qa)/qa;
  double qx = ((x-pk)/pk)/width*qb;
  double qy = 1 + tail * qx;
  double qc;
  if(qy < 1.e-7) qc = 15;
  else
    qc = log(qy)/tail;
  double val = qc;
  return val-mean;
 }



