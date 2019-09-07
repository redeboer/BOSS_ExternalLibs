#ifndef VertexFit_SecondVertexFit_H
#define VertexFit_SecondVertexFit_H
#include "VertexFit/TrackPool.h"
#include "VertexFit/VertexFit.h"

//--------------------------------------------------------------------------------
//
// Fitting Parameters Format:  (px, py, pz, E, xd, yd, zd, xp, yp, zp)
//
// Author   Date         Comments
// K.L He   11/21/2005   original version
// Xu Min   11/30/2006   Add interface of production vertex and decay vertex
// Martin   09/02/2010   Add some kindly comments 
// 
//--------------------------------------------------------------------------------

// You can find the corresponding tutorial on the following web page:  
// http://docbes3.ihep.ac.cn/~offlinesoftware/index.php/Vertex_Fit

class SecondVertexFit : public TrackPool 
{
public:

	// constructor & deconstructor
	static SecondVertexFit *instance();
	~SecondVertexFit();

	// initializtion must be called before SecondVertexFit each time
	void init();
 
	// set iteration number and chisq cut
	void setIterNumber(const int niter = 10) {m_niter = niter;}
	void setChisqCut(const double chicut = 500, const double chiter = 1.0e-2) {m_chicut = chicut;m_chiter=chiter;}
	// set decay point
	void setVpar(const VertexParameter vpar) {m_vpar_secondary = vpar;}
	// set production point
	void setPrimaryVertex(const VertexParameter vpar) {m_vpar_primary.setVx(vpar.Vx());m_vpar_primary.setEvx(vpar.Evx());}
	// set magnet correction factor
	void setMagCorrFactor(const double factor = 1.000) {m_factor = factor;}

	// fit function
	bool Fit();

	// fit results
	double              chisq() const {return m_chisq;}
	double              decayLength() const {return m_lxyz;}
	double              decayLengthError() const {return m_lxyz_error;}
	double              ctau() const {return m_ctau;}
	double              ctauError() const {return m_ctau_error;}
	double              factor() const {return m_factor;}

	HepLorentzVector    p4par() const {return m_p4par;}
	WTrackParameter     wpar() const {return m_wtrk;}
	VertexParameter     vpar() const {return m_vpar_secondary;}
	inline HepPoint3D   crossPoint() const;
	inline Hep3Vector   crossVector() const;
  
private:

	int    m_niter;       // number of iteration for second vertex fitting
	double m_chisq;       // fitting chisquare
	double m_chicut;      // chisquare upper limit
	double m_chiter;      // precision of iteration
	double m_lxyz;        // decay length
	double m_lxyz_error;  // error of decay length
	double m_ctau;        // c times tau
	double m_ctau_error;  // error of ctau
	double m_factor;      // B Field correction factor

	HepVector          m_crxyz;          // decay vertex in HepVector format
	HepLorentzVector   m_p4par;          // HepLorentzVector of particle at decay vertex
	WTrackParameter    m_wtrk;           // WTrackParameter of particle at decay vertex 
	VertexParameter    m_vpar_primary;   // production vertex
	VertexParameter    m_vpar_secondary; // decay vertex

	// Singleton Design
	SecondVertexFit();
	static SecondVertexFit * m_pointer;
};

inline HepPoint3D SecondVertexFit::crossPoint() const 
{
	return HepPoint3D(m_crxyz[0], m_crxyz[1], m_crxyz[2]);
}

inline Hep3Vector SecondVertexFit::crossVector() const 
{
	return Hep3Vector(m_crxyz[0] - m_vpar_primary.Vx()[0], 
	                  m_crxyz[1] - m_vpar_primary.Vx()[1],
	                  m_crxyz[2] - m_vpar_primary.Vx()[2]);
}

#endif  // VertexFit_SecondVertexFit_H

