#ifndef _BFIELDCORR_H_
#define _BFIELDCORR_H_

/*****************************************************************************
 * Project: B Field Correction                                               *
 * Package: BFieldCorr                                                       *
 *    File: BFieldCorr.h    $v1   2013/11/14$                                *
 * Authors:                                                                  *
 *   Tian MA,     mat@ihep.ac.cn                                             *
 *                                                                           *
 *****************************************************************************/

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "CLHEP/Matrix/SymMatrix.h"
using CLHEP::HepSymMatrix;

class BFieldCorr:public Algorithm 
{
public:
	BFieldCorr(const std::string& name, ISvcLocator* pSvcLocator);
	StatusCode initialize();
	StatusCode execute();
	StatusCode finalize();

private:
	double         m_factor; // B Field correction factor
	
	HepSymMatrix   m_Ew;     // conversion matrix
	int            RUN_BEGIN_10;
	int            RUN_END_10;
	int            RUN_BEGIN_11;
	int            RUN_END_11;
};
#endif //_BFIELDCORR_H_
