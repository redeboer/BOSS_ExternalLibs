#ifndef MDCALIGNPAR_H
#define MDCALIGNPAR_H

#include "Alignment.h"

#include <fstream>
#include <string>
#include <cstring>

using namespace Alignment;

class MdcAlignPar {
public:
     MdcAlignPar();
     ~MdcAlignPar();

     void initAlignPar();
     bool rdAlignPar(std::string alignFile);
     void wrtAlignPar();

     void setDx(int iEP, double val) { m_dx[iEP] = val; }
     void setDy(int iEP, double val) { m_dy[iEP] = val; }
     void setDz(int iEP, double val) { m_dz[iEP] = val; }
     void setRx(int iEP, double val) { m_rx[iEP] = val; }
     void setRy(int iEP, double val) { m_ry[iEP] = val; }
     void setRz(int iEP, double val) { m_rz[iEP] = val; }

     void setDelDx(int iEP, double val) { m_delDx[iEP] = val; }
     void setDelDy(int iEP, double val) { m_delDy[iEP] = val; }
     void setDelDz(int iEP, double val) { m_delDz[iEP] = val; }
     void setDelRx(int iEP, double val) { m_delRx[iEP] = val; }
     void setDelRy(int iEP, double val) { m_delRy[iEP] = val; }
     void setDelRz(int iEP, double val) { m_delRz[iEP] = val; }

     void setErrDx(int iEP, double val) { m_errDx[iEP] = val; }
     void setErrDy(int iEP, double val) { m_errDy[iEP] = val; }
     void setErrDz(int iEP, double val) { m_errDz[iEP] = val; }
     void setErrRx(int iEP, double val) { m_errRx[iEP] = val; }
     void setErrRy(int iEP, double val) { m_errRy[iEP] = val; }
     void setErrRz(int iEP, double val) { m_errRz[iEP] = val; }
     
private:
     double m_dx[NEP];
     double m_dy[NEP];
     double m_dz[NEP];
     double m_rx[NEP];
     double m_ry[NEP];
     double m_rz[NEP];

     double m_delDx[NEP];
     double m_delDy[NEP];
     double m_delDz[NEP];
     double m_delRx[NEP];
     double m_delRy[NEP];
     double m_delRz[NEP];

     double m_errDx[NEP];
     double m_errDy[NEP];
     double m_errDz[NEP];
     double m_errRx[NEP];
     double m_errRy[NEP];
     double m_errRz[NEP];
};

#endif/*MDCALIGNPAR_H*/
