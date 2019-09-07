//
// Do not edit! Automatically generated file
//
#ifndef MdcRecoUtilGEANT_H
#define MdcRecoUtilGEANT_H
#include "MdcRecoUtil/PdtLund.h"

class PdtGeant
{
public:
  enum GeantType
  {
    gamma = 1 ,
    e_plus = 2 ,
    e_minus = 3 ,
    nu_e = 4 ,
    mu_plus = 5 ,
    mu_minus = 6 ,
    pi0 = 7 ,
    pi_plus = 8 ,
    pi_minus = 9 ,
    K_L0 = 10 ,
    K_plus = 11 ,
    K_minus = 12 ,
    n0 = 13 ,
    p_plus = 14 ,
    anti_p_minus = 15 ,
    K_S0 = 16 ,
    eta = 17 ,
    Lambda0 = 18 ,
    Sigma_plus = 19 ,
    Sigma0 = 20 ,
    Sigma_minus = 21 ,
    Xi0 = 22 ,
    Xi_minus = 23 ,
    Omega_minus = 24 ,
    anti_n0 = 25 ,
    anti_Lambda0 = 26 ,
    anti_Sigma_minus = 27 ,
    anti_Sigma0 = 28 ,
    anti_Sigma_plus = 29 ,
    anti_Xi0 = 30 ,
    anti_Xi_plus = 31 ,
    anti_Omega_plus = 32 ,
    null_33 = 33 ,
    null_34 = 34 ,
    null_35 = 35 ,
    null_36 = 36 ,
    null_37 = 37 ,
    null_38 = 38 ,
    null_39 = 39 ,
    null_40 = 40 ,
    null_41 = 41 ,
    null_42 = 42 ,
    null_43 = 43 ,
    null_44 = 44 ,
    deuteron = 45 ,
    tritium = 46 ,
    alpha = 47 ,
    geantino = 48 ,
    He3 = 49 ,
    Cerenkov = 50 ,
    chargedgeantino = 51 ,
    anti_nu_e = 4,
    nu_mu = 4,
    anti_nu_mu = 4,
    nu_tau = 4,
    anti_nu_tau = 4,
    null = 0
  };
protected:
  static const GeantType _firstGeantId;
  static const int _nGeantId;
  static const PdtLund::LundType _lundId[52];
  
  friend class Pdt;
};

#endif
