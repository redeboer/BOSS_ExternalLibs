/*
 * @class : RecMdcKalTrack
 *
 *  this class models "Kalman Track" used in Mdc Reconstruction
 *
 *  by wangjk 2006.9
 * 
 * ********************************************************/

#ifndef RECMDCKALTRACK_H
#define RECMDCKALTRACK_H
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h" 
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/ObjectVector.h"
#include "EventModel/EventModel.h"
#include "DstEvent/DstMdcKalTrack.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "RecMdcKalHelixSeg.h"


extern const CLID& CLID_RecMdcKalTrack;  

typedef SmartRefVector<RecMdcKalHelixSeg>  HelixSegRefVec;


class RecMdcKalTrack : public DstMdcKalTrack {

	public:
		virtual const CLID& clID() const   {
			return RecMdcKalTrack::classID();
		}

		static const CLID& classID()       {
			return CLID_RecMdcKalTrack;
		}                     
		//constructor and destructor
		RecMdcKalTrack();
		RecMdcKalTrack(const RecMdcKalTrack& track);
		RecMdcKalTrack(const DstMdcKalTrack& track);
		RecMdcKalTrack& operator=(const RecMdcKalTrack&);
		RecMdcKalTrack& operator=(const DstMdcKalTrack&);
		~RecMdcKalTrack();

		//extractors
		HelixSegRefVec getVecHelixSegs(int pid=-1) const;//2011-05-06

		int getTrackId( void )                        const { return m_trackId;            }
		double getMass( int pid )                     const { return m_mass[pid];          }
		double getLength( int pid )                   const { return m_length[pid];        }
		double getTof( int pid )                      const { return m_tof[pid];           }
		int getNhits( int pid )                       const { return m_nhits[pid];         }
		int getStat( int i, int pid )                 const { return m_stat[i][pid];       }
		int getNster(const int i, const int pid)      const { return m_nster[i][pid];      }
		int getFirstLayer(const int i, const int pid) const { return m_firstLayer[i][pid]; }
		int getLastLayer(const int i, const int pid)  const { return m_lastLayer[i][pid];  } 
		double getChisq( int i, int pid )             const { return m_chisq[i][pid];      }
		int    getNdf( int i, int pid )               const { return m_ndf[i][pid];        }
		double getPathl( int i )                      const { return m_pathl[i];           }
		double getPathSM( int pid)                    const { return m_pathSM[pid];        }


		const HepVector&    getZHelix() const { return m_zhelixs[2]; }
		const HepSymMatrix& getZError() const { return m_zerrors[2]; }
		const HepVector&    getFHelix() const { return m_fhelixs[2]; }
		const HepSymMatrix& getFError() const { return m_ferrors[2]; }
		const HepVector&    getLHelix() const { return m_lhelixs[2]; }
		const HepSymMatrix& getLError() const { return m_lerrors[2]; }   
		const double    getTHelix(const int i) const { return m_thelix[i]; }
		const double    getTError(const int i) const { return m_terror[i]; }
		const HepVector getTHelix()  const { return m_ta; }
		const HepSymMatrix getTError() const { return m_tEa; }

		HepVector&     getZHelixE() { return m_zhelixs[0]; }
		HepSymMatrix&  getZErrorE() { return m_zerrors[0]; }
		HepVector&     getFHelixE() { return m_fhelixs[0]; }
		HepSymMatrix&  getFErrorE() { return m_ferrors[0]; }   
		HepVector&     getLHelixE() { return m_lhelixs[0]; }
		HepSymMatrix&  getLErrorE() { return m_lerrors[0]; }   

		HepVector&    getZHelixMu() { return m_zhelixs[1]; }
		HepSymMatrix& getZErrorMu() { return m_zerrors[1]; }
		HepVector&    getFHelixMu() { return m_fhelixs[1]; }
		HepSymMatrix& getFErrorMu() { return m_ferrors[1]; }   
		HepVector&    getLHelixMu() { return m_lhelixs[1]; }
		HepSymMatrix& getLErrorMu() { return m_lerrors[1]; } 

		HepVector&     getZHelixK() { return m_zhelixs[3]; }
		HepSymMatrix&  getZErrorK() { return m_zerrors[3]; }
		HepVector&     getFHelixK() { return m_fhelixs[3]; }
		HepSymMatrix&  getFErrorK() { return m_ferrors[3]; }   
		HepVector&     getLHelixK() { return m_lhelixs[3]; }
		HepSymMatrix&  getLErrorK() { return m_lerrors[3]; } 

		HepVector&     getZHelixP() { return m_zhelixs[4]; }
		HepSymMatrix&  getZErrorP() { return m_zerrors[4]; }
		HepVector&     getFHelixP() { return m_fhelixs[4]; }
		HepSymMatrix&  getFErrorP() { return m_ferrors[4]; }   
		HepVector&     getLHelixP() { return m_lhelixs[4]; }
		HepSymMatrix&  getLErrorP() { return m_lerrors[4]; }   

		const HepPoint3D getPocaE()  const { return m_pocas[0]; }
		const HepPoint3D getPocaMu() const { return m_pocas[1]; }
		const HepPoint3D getPoca()   const { return m_pocas[2]; }
		const HepPoint3D getPocaK()  const { return m_pocas[3]; }
		const HepPoint3D getPocaP()  const { return m_pocas[4]; }

		const HepPoint3D&  getLPointE()  const { return m_lpoints[0]; }
		const HepPoint3D&  getLPointMu() const { return m_lpoints[1]; }
		const HepPoint3D&  getLPoint()   const { return m_lpoints[2]; }
		const HepPoint3D&  getLPointK()  const { return m_lpoints[3]; }
		const HepPoint3D&  getLPointP()  const { return m_lpoints[4]; }

		const HepPoint3D&  getLPivotE()  const { return m_lpivots[0]; }
		const HepPoint3D&  getLPivotMu() const { return m_lpivots[1]; }
		const HepPoint3D&  getLPivot()   const { return m_lpivots[2]; }
		const HepPoint3D&  getLPivotK()  const { return m_lpivots[3]; }
		const HepPoint3D&  getLPivotP()  const { return m_lpivots[4]; }

		// const HepVector&    getZHelix(const int pid) const { return m_zhelixs[pid];}
		// const HepSymMatrix& getZError(const int pid) const { return m_zerrors[pid];}
		const HepVector&    getLHelix(const int pid) const { return m_lhelixs[pid];}
		const HepSymMatrix& getLError(const int pid) const { return m_lerrors[pid];}
		//const HepVector&    getFHelix(const int pid) const { return m_fhelixs[pid];}
		//const HepSymMatrix& getFError(const int pid) const { return m_ferrors[pid];}
		const HepPoint3D&   getPoca(const int pid)   const { return   m_pocas[pid];}
		const HepPoint3D&   getLPoint(const int pid) const { return m_lpoints[pid];}
		const double        getFiTerm(const int pid) const { return m_fiTerm[pid]; }
		const HepPoint3D&   getLPivot(const int pid) const { return m_lpivots[pid];}


		const double getZDr(void)   const { return m_zhelixs[2][0];   }
		const double getZFi0(void)  const { return m_zhelixs[2][1];   }
		const double getZCpa(void)  const { return m_zhelixs[2][2];   }
		const double getZDz(void)   const { return m_zhelixs[2][3];   }
		const double getZTanl(void) const { return m_zhelixs[2][4];   }

		const double getLDr(void)   const { return m_lhelixs[2][0];   }
		const double getLFi0(void)  const { return m_lhelixs[2][1];   }
		const double getLCpa(void)  const { return m_lhelixs[2][2];   }
		const double getLDz(void)   const { return m_lhelixs[2][3];   }
		const double getLTanl(void) const { return m_lhelixs[2][4];   }

		const double getFDr(void)   const { return m_fhelixs[2][0];   }
		const double getFFi0(void)  const { return m_fhelixs[2][1];   }
		const double getFCpa(void)  const { return m_fhelixs[2][2];   }
		const double getFDz(void)   const { return m_fhelixs[2][3];   }
		const double getFTanl(void) const { return m_fhelixs[2][4];   }


		// modifiers
		void setVecHelixSegs(const HelixSegRefVec& vechelixsegs, int pid=-1);// 2011-05-06


		//void setTrackId(int id)                      { m_trackId = id;           }
		void setMass(double mass, int pid)             { m_mass[pid] = mass;       }
		void setLength(double length, int pid)         { m_length[pid] = length;   }
		void setTof(double tof, int pid)               { m_tof[pid] = tof;         }   
		void setNhits(int nhits, int pid)              { m_nhits[pid] = nhits;     }
		//void setStat(int stat, int i, int pid)       { m_stat[i][pid] = stat;    }
		//void setChisq(double chisq, int i, int pid)  { m_chisq[i][pid] = chisq;  }
		//void setNdf(int ndf, int i, int pid)         { m_ndf[i][pid] = ndf;      }
		//void setNster(int nster, int i, int pid)     { m_nster[i][pid] = nster;  }
		//void setFirstLayer(int fL, int i, int pid)   { m_firstLayer[i][pid] = fL;}
		//void setLastLayer(int lL, int i, int pid)    { m_lastLayer[i][pid] = lL; }
		void setPathl(double pathl, int i)             { m_pathl[i] = pathl;       }   
		void setPathSM(double length, int pid)         { m_pathSM[pid] = length;   }


		/*
		   void setZHelix(const HepVector& helix, const int pid)  {
		   m_zhelixs[pid] = helix;
		   }
		   void setZError(const HepSymMatrix& error, const int pid) { 
		   m_zerrors[pid] = error;
		   }
		   void setZHelix(float* helix, const int pid)  {
		   for(int i=0; i<3; i++) {
		   m_zhelixs[pid][i] = helix[i]; 
		   }
		   }
		   void setZError(float* error, const int pid) {
		   int k=0;
		   HepSymMatrix mat(5);
		   for(int i=0; i<5 ; i++) {
		   for(int j=0; j<=i; j++,k++) {
		   mat[i][j] = error[k];
		   mat[j][i] = error[k];	    
		   }      
		   }
		   m_zerrors[pid] = mat;
		   }
		 */

		//  void setFHelix(const HepVector& helix, const int pid)  {
		//    m_fhelixs[pid] = helix;
		//  }
		//  void setFError(const HepSymMatrix& error, const int pid) {
		//    m_ferrors[pid] = error;
		//  }
		//  void setFHelix(double* helix, const int pid)  {
		//    for(int i=0; i<5; i++) {
		//      m_fhelixs[pid][i] = helix[i];
		//    }
		//  }
		//  void setFError(double* error, const int pid) {
		//    int k=0;
		//    HepSymMatrix mat(5);
		//    for(int i=0; i<5 ; i++) {
		//      for(int j=0; j<=i; j++,k++) {
		//        mat[i][j] = error[k];
		//        mat[j][i] = error[k];
		//      }
		//    }
		//    m_ferrors[pid] = mat;
		//  }

		void setLHelix(const HepVector& helix, const int pid)  {
			m_lhelixs[pid] = helix;
		}
		void setLError(const HepSymMatrix& error, const int pid) {
			m_lerrors[pid] = error;
		}
		void setLHelix(double* helix, const int pid)  {
			for(int i=0; i<5; i++) {
				m_lhelixs[pid][i] = helix[i];
			}
		}
		void setLError(double* error, const int pid) {
			int k=0;
			HepSymMatrix mat(5);
			for(int i=0; i<5 ; i++) {
				for(int j=0; j<=i; j++,k++) {
					mat[i][j] = error[k];
					mat[j][i] = error[k];
				}
			}
			m_lerrors[pid] = mat;
		}

		void setTHelix(const HepVector& helix)  {
			for(int i=0; i<5; i++){
				m_thelix[i] = helix[i];
			}
			m_ta = helix;
		}
		void setTError(const HepSymMatrix& error) {
			int k=0;
			for(int i=0; i<5; i++){
				for(int j=i; j<5; j++){
					m_terror[k] = error[i][j];
					k++;
				}
			}
			m_tEa = error;
		}
		void setTHelix(double helix[5])  {
			HepVector atemp(5);
			for(int i=0; i<5; i++) {
				m_thelix[i] = helix[i];
				atemp[i] = helix[i];
			}
			m_ta = atemp;
		}
		void setTError(double error[15] ) {
			int k=0;
			HepSymMatrix mat(5);
			for(int i=0; i<5 ; i++) {
				for(int j=0; j<=i; j++,k++) {
					mat[i][j] = error[k];
					mat[j][i] = mat[i][j];
				}
			}
			m_tEa = mat;

			for(int i=0; i<15 ; i++) {
				m_terror[i] = error[i];
			}
		}

		void setLPoint(const HepPoint3D& point, const int pid)  {
			m_lpoints[pid] = point;
		}


		void setFiTerm(double fi, const int pid){
			m_fiTerm[pid] = fi;
		}


		void setLPivot(const HepPoint3D& pivot, const int pid)  {
			m_lpivots[pid] = pivot;
		}


		/*
		   void setPoca(const HepPoint3D& poca, const int pid)  {
		   m_pocas[pid] = poca;
		   }
		   void setPoca(float* poca, const int pid)  {
		   for(int i=0; i<3; i++) {
		   m_pocas[pid][i] = poca[i];		      
		   }
		   }
		 */

	private:
		HelixSegRefVec m_vechelixsegs[5];// add 5 hypotheses by wangll 2011-05-06
		int pidSeg;// added by wangll 2011-05-06
		//int    m_trackId;
		double m_mass[5];     // mass assumption during the fit 
		//track path length from closest point to coord. origin to the last hit
		double m_length[5]; 
		double m_tof[5];      // time of flight correspond to length; 
		int    m_nhits[5];      // number of total hits contained 
		// status flag 0:filter 1: smoother, convention: -1:as default; 0:kalfit OK; 1: kalfit abandoned
		// int    m_stat[2][5]; 
		// int    m_nster[2][5];
		// int    m_firstLayer[2][5];
		// int    m_lastLayer[2][5];
		// double m_chisq[2][5]; // chi square of fit 0:filter 1:smoother 
		// int    m_ndf[2][5];   // degree of freedom for the fit 0:filter 1:smoother 
		double m_pathl[43];   // path length in each layer
		double m_pathSM[5];
		double m_fiTerm[5];

		// std::vector<HepPoint3D> m_pocas; 
		// std::vector<HepVector> m_zhelixs; 
		// std::vector<HepSymMatrix> m_zerrors; 

		std::vector<HepPoint3D>   m_lpivots;
		std::vector<HepPoint3D>   m_lpoints;

		//std::vector<HepVector>    m_fhelixs;
		//std::vector<HepSymMatrix> m_ferrors;
		std::vector<HepVector>    m_lhelixs;
		std::vector<HepSymMatrix> m_lerrors;
		double  m_thelix[5];
		double  m_terror[15];
		HepVector  m_ta;
		HepSymMatrix m_tEa;

};

typedef ObjectVector<RecMdcKalTrack> RecMdcKalTrackCol;

#endif

