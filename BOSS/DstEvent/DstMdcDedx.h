#ifndef DST_DSTMDCDEDX_H
#define DST_DSTMDCDEDX_H
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ContainedObject.h"
#include "EventModel/EventModel.h"

using namespace EventModel;
extern const  CLID  &CLID_DstMdcDedx;
enum pid_dedx{electron, muon, kaon, pion, proton};

class  DstMdcDedx : virtual public ContainedObject {
public:

// ctor and dector  
  DstMdcDedx() {}
  virtual ~DstMdcDedx() {}

  virtual const CLID& clID()  const { return  DstMdcDedx::classID(); }
  static  const CLID& classID() { return CLID_DstMdcDedx; }

// extractors 
  //int 	 getTrackId()    const {return m_trackId ;}
  //int    getParticleId()      const { return m_particleId; }
  //int    getStatus()     const { return m_status; }
  //int    geTruncAlg() const { return m_trunc_alg;  }  
  //double getNumChidedx(int i)        const { return m_numChidedx[i]; }
  //int    getNumGoodHits()      const { return m_numGoodHits; }
  //int    getNumTotalHits()     const { return m_numTotalHits; }
  //double getProbPH()           const { return m_probPH; }
  //double getNormPH()        const { return m_normPH; }
// extractors 
  int    trackId()    const {return m_trackId ;}
  int    particleId()      const { return m_particleId; } // will converge to particleType() later
  pid_dedx particleType() const{
    switch(particleId()){
    case 0:
      return electron;
      break;
    case 1:
      return muon;
      break;
    case 2:
      return kaon;
      break;
    case 3:
      return pion;
      break;
    case 4:
      return proton;
      break;
    default:
      std::cerr << "the particleType receive an incorrect input!" << std::endl;
      exit(1);
    }
  }
  int    status()     const { return m_status; }
  int    truncAlg() const { return m_trunc_alg;  }  
  double chi(int i)        const { return m_chi[i]; }
  double chiE()        const { return m_chi[0]; }
  double chiMu()        const { return m_chi[1]; }
  double chiPi()        const { return m_chi[2]; }
  double chiK()        const { return m_chi[3]; }
  double chiP()        const { return m_chi[4]; }
  int    numGoodHits()      const { return m_numGoodHits; }
  int    numTotalHits()     const { return m_numTotalHits; }
  double probPH()           const { return m_probPH; }
  double normPH()        const { return m_normPH; }
  double errorPH()       const { return m_errorPH; }                   
  double twentyPH()           const { return m_twentyPH; }
  
// modifiers 
  void   setTrackId(int trackId) {m_trackId = trackId ;} 
  void   setParticleId(int particleId)  { m_particleId = particleId; }
  void   setStatus (int status) { m_status=status; }
  void   setTruncAlg(int trunc_alg) { m_trunc_alg = trunc_alg;   }
  
  void   setChi( double* chi)  {
    for(int i =0; i<5 ; i++) m_chi[i]= chi[i];
  }
  
  void   setNumGoodHits(int  numGoodHits)      { m_numGoodHits = numGoodHits; } 
  void   setNumTotalHits(int    numTotalHits)  { m_numTotalHits = numTotalHits; } 
  void   setProbPH(double probPH)              { m_probPH = probPH; }
  void   setNormPH(double normPH)        { m_normPH = normPH; } 
  void   setErrorPH(double errorPH)      { m_errorPH = errorPH; }
  void   setTwentyPH(double twentyPH)    { m_twentyPH = twentyPH; }
 protected:	
  int    m_trackId ; //Track ID Add 2005-10-18
  int    m_particleId;    //Particle ID from De/Dx 
  int    m_status;        //Status
  int    m_trunc_alg;    //truncate method
  double m_chi[5]; //Number of Chi_dEdx for different particles
                          //0: e  1: mu   2: pi  3: K  4:p
  int    m_numGoodHits;   //No. of good de/dx hits(exclude overflow) 
  int    m_numTotalHits;  //No. of good de/dx hits(include overflow) 
  double m_probPH;        //Most probable pulse height from truncated mean
  double m_normPH;     //normalized pulse height
  double m_errorPH;      //resolution of truncated mean 
  double m_twentyPH;    //de/dx value of bitrunction rate equal to 20% 
};

typedef ObjectVector<DstMdcDedx> DstMdcDedxCol;

#endif //DST_DEDX_H

