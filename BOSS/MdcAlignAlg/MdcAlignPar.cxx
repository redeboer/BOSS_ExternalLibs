#include "MdcAlignAlg/MdcAlignPar.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;
using namespace Alignment;

MdcAlignPar::MdcAlignPar(){
     for(int iEP=0; iEP<NEP; iEP++){
	  m_delDx[iEP] = 0.0;
	  m_delDy[iEP] = 0.0;
	  m_delDz[iEP] = 0.0;
	  m_delRx[iEP] = 0.0;
	  m_delRy[iEP] = 0.0;
	  m_delRz[iEP] = 0.0;
     }
}

MdcAlignPar::~MdcAlignPar(){
}
void MdcAlignPar::initAlignPar(){
     for(int iEP=0; iEP<NEP; iEP++){
	  m_dx[iEP] = 0.0;
	  m_dy[iEP] = 0.0;
	  m_dz[iEP] = 0.0;
	  m_rx[iEP] = 0.0;
	  m_ry[iEP] = 0.0;
	  m_rz[iEP] = 0.0;

	  m_errDx[iEP] = 0.0;
	  m_errDy[iEP] = 0.0;
	  m_errDz[iEP] = 0.0;
	  m_errRx[iEP] = 0.0;
	  m_errRy[iEP] = 0.0;
	  m_errRz[iEP] = 0.0;
     }
}

bool MdcAlignPar::rdAlignPar(std::string alignFile){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcAlignPar");
     log << MSG::WARNING << "read MdcAlignPar data directly from run directory"
	 << endreq;

     ifstream fpar(alignFile.c_str());
     log << MSG::INFO << "open file" << endreq;
     if( ! fpar.is_open() ){
	  log << MSG::FATAL << "can not open alignment file " << alignFile.c_str() << endreq;
	  return false;
     }

     int i;
     string strtmp;
     for(i=0; i<7; i++) fpar >> strtmp;
     for(int iEP=0; iEP<NEP; iEP++){
	  fpar >> strtmp >> m_dx[iEP] >> m_dy[iEP] >> m_dz[iEP]
	       >> m_rx[iEP] >> m_ry[iEP] >> m_rz[iEP];
     }
     fpar.close();
     return true;
}

void MdcAlignPar::wrtAlignPar(){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcAlignPar");
     log << MSG::INFO << "MdcAlignPar::wrtAlignPar()" << endreq;

     int i;
     int iEnd;
     string str[] = {"Inner_east", "Step0_east", "Step1_east",
		     "Step2_east", "Step3_east", "Step4_east",
		     "Step5_east", "Outer_east", "Inner_west",
		     "Step0_west", "Step1_west", "Step2_west",
		     "Step3_west", "Step4_west", "Step5_west",
		     "Outer_west"};

     string name[] = {"Elements", "DeltaX(mm)", "DeltaY(mm)",
		      "DeltaZ(mm)", "RX(rad)", "RY(rad)", "RZ(rad)"};

     ofstream fout("alignPar_new.txt");
     fout << setw(14) << name[0];
     for(i=1; i<7; i++) fout << setw(13) << name[i];
     fout << endl;
     for (iEnd=0; iEnd<NEP; iEnd++){
	  fout << setw(14) << str[iEnd]
	       << setw(13) << m_dx[iEnd] + m_delDx[iEnd]
	       << setw(13) << m_dy[iEnd] + m_delDy[iEnd]
	       << setw(13) << m_dz[iEnd] + m_delDz[iEnd]
	       << setw(13) << m_rx[iEnd] + m_delRx[iEnd]
	       << setw(13) << m_ry[iEnd] + m_delRy[iEnd]
	       << setw(13) << m_rz[iEnd] + m_delRz[iEnd] << endl;
	  if(7 == iEnd) fout << endl;
     }
     fout.close();

     ofstream fdel("delAlign_new.txt");
     fdel << setw(14) << name[0];
     for(i=1; i<7; i++) fdel << setw(13) << name[i];
     fdel << endl;
     for (iEnd=0; iEnd<NEP; iEnd++){
	  fdel << setw(14) << str[iEnd] << setw(13) << m_delDx[iEnd]
	       << setw(13) << m_delDy[iEnd] << setw(13) << m_delDz[iEnd]
	       << setw(13) << m_delRx[iEnd] << setw(13) << m_delRy[iEnd]
	       << setw(13) << m_delRz[iEnd] << endl;
	  if(7 == iEnd) fdel << endl;
     }

     fdel << endl << "Fit error:" << endl;
     for (iEnd=0; iEnd<NEP; iEnd++){
	  fdel << setw(14) << str[iEnd] << setw(13) << m_errDx[iEnd]
	       << setw(13) << m_errDy[iEnd] << setw(13) << m_errDz[iEnd]
	       << setw(13) << m_errRx[iEnd] << setw(13) << m_errRy[iEnd]
	       << setw(13) << m_errRz[iEnd] << endl;
	  if(7 == iEnd) fdel << endl;
     }
     fdel.close();
}
