//------------------------------------------------------------------------------|
//      [File  ]:                       MucGeoCreateAlg.h                       |
//      [Brief ]:       Header file of MUC geometry creating algorithem class   |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Mar 28, 2006                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_GEO_CREATE_ALG_H
#define MUC_GEO_CREATE_ALG_H

#include <iostream>
#include <string>

#include "GaudiKernel/Algorithm.h"

#include "MucGeoCreateAlg/MucGeoMgr.h"

class MucGeoCreateAlg : public Algorithm
{
        public:
                MucGeoCreateAlg(const std::string& name, ISvcLocator* pSvcLocator);
                ~MucGeoCreateAlg(){};

                StatusCode initialize();
                StatusCode execute();
                StatusCode finalize();

        private:

                       bool	m_bAlignFlag;
	    	std::string 	m_sCreateFlag;
		std::string	m_sAlignFile;

		MucGeoMgr	*m_pMucGeoMgr;
};

#endif
