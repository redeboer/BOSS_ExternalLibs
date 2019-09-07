#ifndef Physics_Analysis_DSemilep_H
#define Physics_Analysis_DSemilep_H 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

#include "VertexFit/IVertexDbSvc.h"
#include "SimplePIDSvc/ISimplePIDSvc.h"
#include "DTagTool/DTagTool.h"

class DSemilepAlg : public Algorithm {

	public:
		DSemilepAlg(const std::string& name, ISvcLocator* pSvcLocator);

		StatusCode initialize();
		StatusCode execute();
		StatusCode finalize();

		bool isGoodTrack(EvtRecTrack* trk,Hep3Vector xorigin);
		void calU(DTagToolIterator sditer,RecMdcKalTrack* Etrack, RecMdcKalTrack* Ktrack,double& U, double& MM2, double& q2);
		Hep3Vector tagDP3(DTagToolIterator iter_dtag);

	private:
		ISimplePIDSvc*         m_simplePIDSvc;
		std::string      m_pdt;

		//
		// Define Ntuples here
		NTuple::Tuple* m_tuple0;
		NTuple::Item<double> m_mBC;
		NTuple::Item<double> m_deltaE;
		NTuple::Item<double> m_U;
		NTuple::Item<double> m_mode;
		NTuple::Item<double> m_MM2;
		NTuple::Item<double> m_q2;


};
#endif
