#ifndef ValidRecMdcKalTrack_H
#define ValidRecMdcKalTrack_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "AIDA/IHistogram1D.h" 
#include "AIDA/IHistogramFactory.h" 
#include "GaudiKernel/IHistogramSvc.h"

// Specify the namespace
using AIDA::IHistogram1D;


class ValidRecMdcKalTrackAlg  : public  Algorithm {

public:

  ValidRecMdcKalTrackAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  void histDef(void);
  
private:

  int _ntuple;
  NTuple::Tuple* m_nt1;  //
  NTuple::Tuple* m_nt2;  //
  NTuple::Tuple* m_nt3;  //
  NTuple::Tuple* m_nt4;  // 
  NTuple::Tuple* m_nt5;  //
 

 
  NTuple::Matrix<double> m_ndf,m_chisq,m_stat;
  
  NTuple::Array<double> m_length,m_tof,m_nhits;
   
  NTuple::Item<double> m_zptot,m_zptote,m_zptotmu,m_zptotk,m_zptotp;
   
  NTuple::Item<double> m_zpt,m_zpte,m_zptmu,m_zptk,m_zptp;
	
  NTuple::Item<double> m_fptot,m_fptote,m_fptotmu,m_fptotk,m_fptotp;
	
  NTuple::Item<double> m_fpt,m_fpte,m_fptmu,m_fptk,m_fptp;
	
  NTuple::Item<double> m_lptot,m_lptote,m_lptotmu,m_lptotk,m_lptotp;
	
  NTuple::Item<double> m_lpt,m_lpte,m_lptmu,m_lptk,m_lptp;
	
  NTuple::Item<double> m_zsigp,m_zsigpe,m_zsigpmu,m_zsigpk,m_zsigpp;
	
  NTuple::Array<double> m_zhelix,m_zhelixe,m_zhelixmu,m_zhelixk,m_zhelixp;
	
  NTuple::Array<double> m_fhelix,m_fhelixe,m_fhelixmu,m_fhelixk,m_fhelixp;
	
  NTuple::Array<double> m_lhelix,m_lhelixe,m_lhelixmu,m_lhelixk,m_lhelixp;
	
  NTuple::Array<double> m_zerror,m_zerrore,m_zerrormu,m_zerrork,m_zerrorp;
	
  NTuple::Array<double> m_ferror,m_ferrore,m_ferrormu,m_ferrork,m_ferrorp;
	
  NTuple::Array<double> m_lerror,m_lerrore,m_lerrormu,m_lerrork,m_lerrorp;
			      
  
};


#endif

