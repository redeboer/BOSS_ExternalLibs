#ifndef VertexFit_VertexFit_H
#define VertexFit_VertexFit_H

#include <vector>
#include "VertexFit/WTrackParameter.h"
#include "VertexFit/VertexParameter.h"
#include "VertexFit/VertexConstraints.h"
#include "VertexFit/TrackPool.h"

// NOTE: if you want to update the parameters of daughter tracks after vertex fitting,
// you should add the following code after invoking the interface "Fit()".
// VertexFit->Swim(n); 
// here, n is vertex number.
//
// You can find the corresponding tutorial on the following web page: 
// http://docbes3.ihep.ac.cn/~offlinesoftware/index.php/Vertex_Fit

class VertexFit : public TrackPool 
{
public:
	// constructor & deconstructor
	static VertexFit *instance();
	~VertexFit();

	// initialization, must be called before VertexFit each time
	void init();

	// add methods
	void AddVertex(int number, VertexParameter vpar, std::vector<int> lis);
	void AddVertex(int number, VertexParameter vpar, int n1, int n2);
	void AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3);
	void AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3, int n4);
	void AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3, int n4, int n5);
	void AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3, int n4, int n5, int n6);
	void AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3, int n4, int n5, int n6, int n7);
	void AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8);
	void AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9);
	void AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10);
	void AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10, int n11);
	void AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10, int n11, int n12);
	void AddBeamFit(int number, VertexParameter vpar, int n);
	
	// build virtual particle, parameter number must be match with vertex number
	void BuildVirtualParticle(int number);

	// set iteration number and chisq cut
	void setIterNumber(const int niter = 10) { m_niter = niter; }
	void setChisqCut(const double chicut = 1000, const double chiter = 1.0e-3) { m_chicut = chicut;m_chiter = chiter; }
	void setMagCorrFactor(const double factor = 1.000) { m_factor = factor; }
	double factor() const { return m_factor; }

	// fit function
	bool Fit();
	bool Fit(int n);
	bool BeamFit(int n);
	
	// renew track parameters and convariance
	void Swim(int n){ vertexCovMatrix(n); swimVertex(n);}
	
	//
	// Fit Results
	//

	// chisq of fit
	double chisq() const {return m_chi;}
	double chisq(int n) const {return m_chisq[n];}
	
	// Pull distribution for 5-helix parameters.
	// Return true on success, the resulting pull will be stored in p;
	// Return false on failure.
	bool pull(int n, int itk, HepVector& p);
	
	// updated WTrack parameter in vertex fit
	HepLorentzVector pfit(int n) const {return wTrackInfit(n).p();}
	HepPoint3D xfit(int n) const {return wTrackInfit(n).x();}
	HepVector w(int n) const {return wTrackInfit(n).w();}
	HepSymMatrix Ew(int n) const {return wTrackInfit(n).Ew();}
	WTrackParameter wtrk(int n) const {return wTrackInfit(n);}
	
	// time counter
	HepVector cpu() const {return m_cpu;}
  
	// updated Vertex Parameter in vertex fit
	HepPoint3D vx(int n) const { return m_vpar_infit[n].vx();}
	HepVector Vx(int n) const { return m_vpar_infit[n].Vx();}
	HepSymMatrix Evx(int n) const { return m_vpar_infit[n].Evx();}
	double errorVx(int n, int i) const { return sqrt((m_vpar_infit[n].Evx())[i][i]);}
	VertexParameter vpar(int n) const {return m_vpar_infit[n];}
  
	// virtual particle from Vertex Fit
	WTrackParameter wVirtualTrack(int n) const {return m_virtual_wtrk[n];}

	// Users need not care below
private:
	// renew vertex constraints
	void UpdateConstraints(const VertexConstraints &vc);
	// vertex fit
	void fitVertex(int n);
	// renew covariance of input track parameters
	void vertexCovMatrix(int n);
	// extrapolate input track parameters to the vertex
	void swimVertex(int n);
	
	void fitBeam(int n);
	void swimBeam(int n);
 
	// vertex parameters before fit
	std::vector<VertexParameter> m_vpar_origin;
	// vertex parameters in fit
	std::vector<VertexParameter> m_vpar_infit;
	// vertex constraints
	std::vector<VertexConstraints> m_vc;
	// chisquare counter
	std::vector<double> m_chisq;
	double m_chi;	// total chisquare
	int m_nvtrk;	// number of WTracks in VertexFit
	// virtual particle in WTrackParameter format
	std::vector<WTrackParameter> m_virtual_wtrk;
  
	// track parameters storage and access
	// origin vertex and its covariance matrix
	HepVector m_xOrigin;
	HepSymMatrix m_xcovOrigin;
	HepSymMatrix m_xcovOriginInversed;
	inline HepVector xOrigin() const {return m_xOrigin;}
	inline void setXOrigin(const HepVector &x) { m_xOrigin = x;}
	inline HepSymMatrix xcovOrigin() const {return m_xcovOrigin;}
	inline void setXCovOrigin(const HepSymMatrix &v) {m_xcovOrigin = v;}
	inline HepSymMatrix xcovOriginInversed() const {return m_xcovOriginInversed;}
	inline void setXCovOriginInversed(const HepSymMatrix &v){m_xcovOriginInversed = v;}

	// vertex and covariance matrix in fit
	HepVector m_xInfit;
	HepSymMatrix m_xcovInfit;
	HepSymMatrix m_xcovInfitInversed;
	inline HepVector xInfit() const {return m_xInfit;}
	inline void setXInfit(const HepVector &x) {m_xInfit = x;}
	inline HepSymMatrix xcovInfit() const {return m_xcovInfit;}
	void setXCovInfit(const HepSymMatrix &v) {m_xcovInfit = v;}
	inline HepSymMatrix xcovInfitInversed() const {return m_xcovInfitInversed;}
	void setXCovInfitInversed(const HepSymMatrix &v) {m_xcovInfitInversed = v;}
  
	// track parameters and covariance matrice at initial
	HepVector m_pOrigin;
	HepSymMatrix m_pcovOrigin;
	inline HepVector pOrigin(int i) const { return m_pOrigin.sub(i*NTRKPAR+1, (i+1)*NTRKPAR); }
	inline void setPOrigin(int i, const HepVector &p) { m_pOrigin.sub(i*NTRKPAR+1, p); }
	inline HepSymMatrix pcovOrigin(int i) const { return m_pcovOrigin.sub(i*NTRKPAR+1, (i+1)*NTRKPAR); }
	inline void setPCovOrigin(int i, const HepSymMatrix &v) { m_pcovOrigin.sub(i*NTRKPAR+1,v); }
 
	// track parameters and covariance matrice in fit
	HepVector m_pInfit;
	HepSymMatrix m_pcovInfit;
	inline HepVector pInfit(int i) const { return m_pInfit.sub(i*NTRKPAR+1, (i+1)*NTRKPAR); }
	inline void setPInfit(int i, const HepVector &p) { m_pInfit.sub(i*NTRKPAR+1, p); }
	inline HepSymMatrix pcovInfit(int i) const { return m_pcovInfit.sub(i*NTRKPAR+1, (i+1)*NTRKPAR); }
	inline void setPCovInfit(int i, const HepSymMatrix &v) { m_pcovInfit.sub(i*NTRKPAR+1, v); }

	// some matrices convenient for calculation
	HepMatrix m_B;	// NCONSTR x NVTXPAR -- E
	inline HepMatrix vfB(int i) const {return m_B.sub(i*NCONSTR+1, (i+1)*NCONSTR, 1, NVTXPAR);}
	inline void setB(int i, const HepMatrix &e) {m_B.sub(i*NCONSTR+1, 1, e);}
	
	HepMatrix m_BT;	// NVTXPAR x NCONSTR -- E.T()
	inline HepMatrix vfBT(int i) const {return m_BT.sub(1, NVTXPAR, i*NCONSTR+1, (i+1)*NCONSTR);}
	inline void setBT(int i, const HepMatrix &e) {m_BT.sub(1, i*NCONSTR+1, e);}
	
	HepMatrix m_A;	// NCONSTR x NTRKPAR -- D
	inline HepMatrix vfA(int i) const {return m_A.sub(i*NCONSTR+1, (i+1)*NCONSTR, i*NTRKPAR+1, (i+1)*NTRKPAR);}
	inline void setA(int i, const HepMatrix &d) {m_A.sub(i*NCONSTR+1, i*NTRKPAR+1, d);}
	
	HepMatrix m_AT;	// NTRKPAR x NCONSTR -- D.T()
	inline HepMatrix vfAT(int i) const {return m_AT.sub(i*NTRKPAR+1, (i+1)*NTRKPAR, i*NCONSTR+1, (i+1)*NCONSTR);}
	inline void setAT(int i, const HepMatrix &d) {m_AT.sub(i*NTRKPAR+1, i*NCONSTR+1, d);}
	
	HepMatrix m_KQ;	// NVTXPAR x NCONSTR -- Vx E.T() VD 
	inline HepMatrix vfKQ(int i) const {return m_KQ.sub(1, NVTXPAR, i*NCONSTR+1, (i+1)*NCONSTR);}
	inline void setKQ(int i, const HepMatrix &d) {m_KQ.sub(1, i*NCONSTR+1, d);}
	
	HepVector m_G;	// NCONSTR x 1  -- Dda + Edx + d
	inline HepVector vfG(int i) const {return m_G.sub(i*NCONSTR+1, (i+1)*NCONSTR);}
	inline void setG(int i, const HepVector &p) {m_G.sub(i*NCONSTR+1, p);}
	
	HepSymMatrix m_W;	// NCONSTR x NCONSTR -- VD
	inline HepSymMatrix vfW(int i) const {return m_W.sub(i*NCONSTR+1, (i+1)*NCONSTR);}
	inline void setW(int i, HepSymMatrix &m) {m_W.sub(i*NCONSTR+1, m);}
	
	HepMatrix m_E;	// NTRKPAR x NVTXPAR -- -Va0 D.T() VD E Vx
	inline HepMatrix vfE(int i) const {return m_E.sub(i*NTRKPAR+1, (i+1)*NTRKPAR, 1, NVTXPAR);}
	inline void setE(int i, const HepMatrix &p){m_E.sub(i*NTRKPAR+1, 1, p);}

	HepMatrix m_TRA;	// transform matrix from 7x1 to 6x1
	HepMatrix m_TRB;	// transform matrix from 6x1 to 7x1
	// convert HepVector 6x1 to 7x1
	// (px, py, pz, x, y, z) -> (px, py, pz, e, x, y, z)
	HepVector Convert67(const double &mass, const HepVector &p);	
	// convert HepVector 7x1 to 6x1
	// (px, py, pz, e, x, y, z) -> (px, py, pz, x, y, z)
	HepVector Convert76(const HepVector &p);
	
	// Singleton Design
	VertexFit();
	static VertexFit *m_pointer;

	int m_niter;      //number of iteration for vertex fitting
	double m_chicut;  //chisquare upper limit
	double m_chiter;
	double m_factor;  //B Field correction factor
	HepVector m_cpu;
  
	static const int NTRKPAR;  //number of track parameters
	static const int NVTXPAR;  //number of vertex parameters
	static const int NCONSTR;  //number of vertex constraints
};
#endif	//VertexFit_VertexFit_H

