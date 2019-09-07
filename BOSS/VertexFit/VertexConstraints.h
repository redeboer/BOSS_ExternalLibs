#ifndef Vertex_Constraints_H
#define Vertex_Constraints_H

//
// Vertex constraints and related calculation
// For each charged particl, there are two constraints:
// 1) 0 = px * delta_y - py*delta_x -a/2*(delta_x^2 +delta_y^2)
// 2) 0 = delta_z - pz/a*asin(a*(px*delta_x+py*delta_y)/pxy^2)
// where, delta_x = Vx - x, etc.
// a = -0.00299792458 * B * q, B in Tesla 
//
// Author: K.L He   date:10/31/2005, created
//

#include <vector>
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
using CLHEP::HepMatrix;
using CLHEP::HepSymMatrix;
using CLHEP::HepVector;
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
	typedef HepGeom::Point3D<double> HepPoint3D;
#endif

#include "VertexFit/VertexParameter.h"
#include "VertexFit/WTrackParameter.h"

class VertexConstraints 
{
public:
	// contructor & descontructir
	VertexConstraints();
	~VertexConstraints() {;}
	inline VertexConstraints(const VertexConstraints &);
	void CommonVertexConstraints(std::vector<int> tlis);
	void FixedVertexConstraints(std::vector<int> tlis);
	void UpdateConstraints(VertexParameter vpar, std::vector<WTrackParameter> wlis);
	void setCovax(const int i, const HepMatrix cov) {m_covax[i] = cov;}
	void setLambda(const int i, const HepVector L) {m_lambda[i] = L;}
	void setLambda0(const int i, const HepVector L0){m_lambda0[i] = L0;}
	void setVD(const int i, const HepSymMatrix vd) {m_VD[i] = vd;}
	void setEVDE(const int i, const HepSymMatrix evde) {m_EVDE[i] = evde;}
	void setType(const int type){m_type = type;}

	std::vector<HepMatrix> Ec() const {return m_Ec;}
	std::vector<HepMatrix> Dc() const {return m_Dc;}
	std::vector<HepVector> dc() const {return m_dc;}
	std::vector<HepSymMatrix> VD() const{return m_VD;}
	std::vector<HepSymMatrix> EVDE() const {return m_EVDE;}
	std::vector<HepMatrix> covax() const {return m_covax;}
	std::vector<HepVector> lambda0() const {return m_lambda0;}
	std::vector<HepVector> lambda() const {return m_lambda;}
	std::vector<int> Ltrk() const {return m_ltrk;}
	int type() const {return m_type;}
	int commonVertex() const {return CommonVertex;}
	int fixedVertex() const {return FixedVertex;}
	inline VertexConstraints & operator = (const VertexConstraints &);

private:
	std::vector<HepMatrix> m_Ec;
	std::vector<HepMatrix> m_Dc;
	std::vector<HepVector> m_dc;
	std::vector<HepSymMatrix> m_VD; 
	std::vector<HepSymMatrix> m_EVDE;
	std::vector<HepMatrix> m_covax;
	std::vector<HepVector> m_lambda0;
	std::vector<HepVector> m_lambda;
	std::vector<int> m_ltrk;
	int m_type;
	static const double Alpha;
	static const int CommonVertex;
	static const int FixedVertex;
};

inline VertexConstraints::VertexConstraints(const VertexConstraints & vc) 
{
	m_Ec = vc.m_Ec;
	m_Dc = vc.m_Dc;
	m_dc = vc.m_dc;
	m_VD = vc.m_VD;
	m_EVDE = vc.m_EVDE;
	m_covax = vc.m_covax;
	m_lambda0 = vc.m_lambda0;
	m_lambda = vc.m_lambda;
	m_ltrk = vc.m_ltrk;
	m_type = vc.m_type;
}

inline VertexConstraints & VertexConstraints::operator = (const VertexConstraints & vc) 
{
	m_Ec = vc.m_Ec;
	m_Dc = vc.m_Dc;
	m_dc = vc.m_dc;
	m_VD = vc.m_VD;
	m_EVDE = vc.m_EVDE;
	m_covax = vc.m_covax;
	m_lambda0 = vc.m_lambda0;
	m_lambda = vc.m_lambda;
	m_ltrk = vc.m_ltrk;
	m_type = vc.m_type;
	return (*this);
}
#endif	//Vertex_Constraints_H

