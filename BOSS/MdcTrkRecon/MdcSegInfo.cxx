//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegInfo.cxx,v 1.2 2009/12/16 09:02:47 zhangy Exp $
//
// Description:
//     
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): 
//	Steve Schaffner
//	Zhang Yao(zhangyao@ihep.ac.cn)  Migrate to BESIII
//
//------------------------------------------------------------------------

#include "MdcTrkRecon/MdcSegInfo.h"
#include <iostream>


//------------------------------------------------------------------------
MdcSegInfo::~MdcSegInfo() {
//------------------------------------------------------------------------
}


//------------------------------------------------------------------------
void MdcSegInfo::plotSegInfo() const{
//------------------------------------------------------------------------
  std::cout<<"seginfo: "<<_par0<<" "<<_par1<<" "<<_arc<<  std::endl;
} 
