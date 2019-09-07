#ifndef DST_TOFTRACK_H
#define DST_TOFTRACK_H
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ContainedObject.h"
#include "EventModel/EventModel.h"
#include  <vector>

using namespace std;
using namespace EventModel;

extern const CLID &CLID_DstTofTrack;

/*
class RecBTofTrack ;
class RecETofTrack ;
*/

class DstTofTrack : virtual public ContainedObject {

 public:
  DstTofTrack();
  virtual ~DstTofTrack() {}

  // Assignment constructor.
  /*
  DstTofTrack& operator=(const RecBTofTrack& source);
  void copy(const RecBTofTrack* orig);
  DstTofTrack(const RecBTofTrack& source);

  DstTofTrack& operator=(const RecETofTrack& source);
  void copy(const RecETofTrack* orig);
  DstTofTrack(const RecETofTrack& source);
  */

  virtual const CLID& clID()  const { return  DstTofTrack::classID();}
  static  const CLID& classID() { return CLID_DstTofTrack; }

 public:

  int     tofTrackID()        const { return  m_tofTrackID; }
  int     trackID()           const { return  m_trackID;    }
  int     tofID();
  int     strip();

  unsigned int status()       const { return  m_status;     }

  double  path()              const { return  m_path;       }
  double  zrhit()             const { return  m_zrhit;      }
  double  ph()                const { return  m_ph;         }
  double  tof()               const { return  m_tof;        }
  double  errtof()            const { return  m_sigma[0];   }
  double  beta()              const { return  m_beta;       }

  double  texp(int i)         const { return  m_texp[i];    }
  double  texpElectron()      const { return  m_texp[0];    }
  double  texpMuon()          const { return  m_texp[1];    }
  double  texpPion()          const { return  m_texp[2];    }
  double  texpKaon()          const { return  m_texp[3];    }
  double  texpProton()        const { return  m_texp[4];    }

  double  toffset(int i)      const { return  m_toffset[i]; }
  double  toffsetElectron()   const { return  m_toffset[0]; }
  double  toffsetMuon()       const { return  m_toffset[1]; }
  double  toffsetPion()       const { return  m_toffset[2]; }
  double  toffsetKaon()       const { return  m_toffset[3]; }
  double  toffsetProton()     const { return  m_toffset[4]; }
  double  toffsetAntiProton() const { return  m_toffset[5]; }

  double  sigma(int i)        const { return  m_sigma[i];   }
  double  sigmaElectron()     const { return  m_sigma[0];   }
  double  sigmaMuon()         const { return  m_sigma[1];   }
  double  sigmaPion()         const { return  m_sigma[2];   }
  double  sigmaKaon()         const { return  m_sigma[3];   }
  double  sigmaProton()       const { return  m_sigma[4];   }
  double  sigmaAntiProton()   const { return  m_sigma[5];   }

  int     quality()           const { return  m_quality;    }

  double  t0()                const { return  m_t0;         }
  double  errt0()             const { return  m_errt0;      }

  double  errz()              const { return  m_errz;       }
  double  phi()               const { return  m_phi;        }
  double  errphi()            const { return  m_errphi;     }
  double  energy()            const { return  m_energy;     }
  double  errenergy()         const { return  m_errenergy;  }

 public:

  void  setTofTrackID(int tofTrackID)   { m_tofTrackID = tofTrackID; }
  void  setTrackID(int trackID)         { m_trackID    = trackID;    }
  void  setTofID(int tofID)             { m_tofID      = tofID;      }
  void  setStatus(unsigned int status)  { m_status     = status;     }

  void  setPath(double path)            { m_path       = path;       }
  void  setZrHit(double zrhit)          { m_zrhit      = zrhit;      }
  void  setPh(double ph)                { m_ph         = ph;         }
  void  setTof(double tof)              { m_tof        = tof;        }
  void  setErrTof(double etof)          { m_sigma[0]   = etof;       }
  void  setBeta(double beta)            { m_beta       = beta;       }

  void  setTexp(double texp[5])         { for(int i=0; i<5; i++) { m_texp[i]=texp[i];} }
  void  setTexpElectron(double texpe)   { m_texp[0] = texpe;         }
  void  setTexpMuon(double texpmu)      { m_texp[1] = texpmu;        }
  void  setTexpPion(double texppi)      { m_texp[2] = texppi;        }
  void  setTexpKaon(double texpk)       { m_texp[3] = texpk;         }
  void  setTexpProton(double texpp)     { m_texp[4] = texpp;         }

  void  setToffset(double toffset[6])   { for(int i=0; i<6; i++) { m_toffset[i]=toffset[i];} }
  void  setToffsetElectron(double toe)  { m_toffset[0] = toe;        }
  void  setToffsetMuon(double tomu)     { m_toffset[1] = tomu;       }
  void  setToffsetPion(double topi)     { m_toffset[2] = topi;       }
  void  setToffsetKaon(double tok)      { m_toffset[3] = tok;        }
  void  setToffsetProton(double top)    { m_toffset[4] = top;        }
  void  setToffsetAntiProton(double topb){ m_toffset[5] = topb;      }

  void  setSigma(double sigma[6])       { for(int i=0; i<6; i++) { m_sigma[i]=sigma[i];} }
  void  setSigmaElectron(double se)     { m_sigma[0]   = se;         }
  void  setSigmaMuon(double smu)        { m_sigma[1]   = smu;        }
  void  setSigmaPion(double spi)        { m_sigma[2]   = spi;        }
  void  setSigmaKaon(double sk)         { m_sigma[3]   = sk;         }
  void  setSigmaProton(double sp)       { m_sigma[4]   = sp;         }
  void  setSigmaAntiProton(double spb)  { m_sigma[5]   = spb;        }

  void  setQuality(int quality)         { m_quality    = quality;    }

  void  setT0(double t0)                { m_t0         = t0;         }
  void  setErrT0(double errt0)          { m_errt0      = errt0;      }

  void  setErrZ(double errz)            { m_errz       = errz;       }
  void  setPhi(double phi)              { m_phi        = phi;        }
  void  setErrPhi(double errphi)        { m_errphi     = errphi;     }
  void  setEnergy(double energy)        { m_energy     = energy;     }
  void  setErrEnergy(double errenergy)  { m_errenergy  = errenergy;  }

 private:

  int     m_tofTrackID;  // The ID of Tof Cluster reconstruction
  int     m_trackID;     // Track ID from MDC / Shower ID from EMC.
  int     m_tofID;       // Tof Counter ID.
  unsigned int m_status; // Status.
                         // 0x0000 0000
                         //   0 0 0 0            0 0 0 0
                         //   | | | BR / 1 EC    | | | raw
                         //   | | East / 1 West  | | ReadOut Unit
                         //   No Hit             | Counter
                         //   01 1st Layer       Cluster
                         //   10 2nd Layer / 11 both Layers
                         // 0x 00 00 00 00 00(used)
                         //        |  |  counter number
                         //        |  east readout number
                         //        west readout number

  double  m_path;        // Distance of flight.
  double  m_zrhit;       // Track extrapolate Z or R Hit position.
  double  m_ph;          // Pulse height.
  double  m_tof;         // Time of flight.
  double  m_beta;        // Beta value of the track.
  double  m_texp[5];     // Expected time of flight of 5 sorts of particle.
  double  m_toffset[6];  // Time offset of e, mu, pi, k and p and pbar.
  double  m_sigma[6];    // Time resolution of 5+1 sorts of particle.
  int     m_quality;     // Data quality of reconstruction.
                         //  1: good charged track
                         //  2: neutral track with good hit
                         //  0: ZT-ZTDC didnot match
                         //  3: no hit in counter
                         //  4: two hits in counter
                         //  5: more than two hits in counter
                         //  6: only single end output of one layer
                         //  7: two hits in counter with bad match with ZTDC
                         // 10: initialize
                         //  ......
  double  m_t0;          // t0 from data set calibration.
  double  m_errt0;       // t0 sigma from tof calibration.

  double  m_errz;        // Error of zTDC(m_zrhit) for neutral track.
  double  m_phi;         // Angle of phi used for neutral track.
  double  m_errphi;      // Error of angle of phi.
  double  m_energy;      // Energy deposit for neutral track
  double  m_errenergy;   // Error of energy deposit (m_ph) for neutral track. 

};

typedef ObjectVector<DstTofTrack> DstTofTrackCol;

#endif //DST_TOFTRACK_H
