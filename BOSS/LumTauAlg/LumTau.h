#ifndef Physics_Analysis_LumTau_H
#define Physics_Analysis_LumTau_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "VertexFit/IVertexDbSvc.h"
#include "GaudiKernel/Bootstrap.h"
class LumTau : public Algorithm
{
	public:
		LumTau(const std::string& name, ISvcLocator* pSvcLocator);
		StatusCode initialize();
		StatusCode execute();
		StatusCode finalize();
		StatusCode hello();
	private:
		NTuple::Tuple*  m_tuple2;
		NTuple::Item<long>  m_run;
		NTuple::Item<long>  m_rec;
		NTuple::Item<double>  m_time;
		NTuple::Item<double>  m_etot;
		NTuple::Item<double>  m_e1;
		NTuple::Item<double>  m_e2;
		NTuple::Item<double>  m_costht1;
		NTuple::Item<double>  m_costht2;
		NTuple::Item<double>  m_dltphi;
		NTuple::Item<double>  m_dlttht;
		NTuple::Item<double>  m_phi1;
		NTuple::Item<double>  m_phi2;
};
#endif
