#ifndef TOFDATASET_H
#define TOFDATASET_H

#include "TofRecEvent/RecBTofCalHit.h"
#include "TofRecEvent/RecETofCalHit.h"
#include "TChain.h"
#include "TTree.h"
#include <string>

using namespace std;

const unsigned int NBarrel = 176;
const unsigned int NEndcap = 96;
const unsigned int NEtf    = 72;
const unsigned int NStrip  = 12;

typedef struct tagItem{
  Int_t    run;
  Int_t    event;
  Int_t    tofid;
  Int_t    strip;
  Double_t qleft;
  Double_t qright;
  Double_t tleft;
  Double_t tright;
  Double_t zrhit;
  Double_t dt;
  Double_t texp;
  Double_t path;
  Double_t phi;
  Double_t theta;
  Double_t p;
  Double_t t0;
  Double_t q0;
  Int_t    hitcase;
} rootRecord;


class Record{
 public:
  Record();
  ~Record();
 public:
  Record( RecBTofCalHit* hit );
  Record( RecETofCalHit* hit );
  Record( const rootRecord& one );

  void initial();
  bool cutBarrel();
  bool cutEndcap();
  bool cutEtf();

  int run()         const { return  m_run;     }
  int event()       const { return  m_event;   }
  int tofid()       const { return  m_tofid;   }
  int strip()       const { return  m_strip;   }
  double qleft()    const { return  m_qleft;   }
  double qright()   const { return  m_qright;  }
  double tleft()    const { return  m_tleft;   }
  double tright()   const { return  m_tright;  }
  double zrhit()    const { return  m_zrhit;   }
  double dt()       const { return  m_dt;      }
  double texp()     const { return  m_texp;    }
  double path()     const { return  m_path;    }
  double phi()      const { return  m_phi;     }
  double theta()    const { return  m_theta;   }
  double p()        const { return  m_p;       }
  double t0()       const { return  m_t0;      }
  double q0()       const { return  m_q0;      }
  int hitcase()     const { return  m_hitcase; }


  void settleft( double tleft )   { m_tleft  = tleft;  }
  void settright( double tright ) { m_tright = tright; }
  void setT0( double t0 )         { m_t0   = t0;       }
  void setQ0( double q0 )         { m_q0   = q0;       }

 private:
  int    m_run;
  int    m_event;
  int    m_tofid;
  int    m_strip;
  double m_qleft;
  double m_qright;
  double m_tleft;
  double m_tright;
  double m_zrhit;
  double m_dt;
  double m_texp;
  double m_path;
  double m_phi;
  double m_theta;
  double m_p;
  double m_t0;
  double m_q0;
  int    m_hitcase;
};
typedef std::vector<Record*> RecordSet;


class TofDataSet{
 public:
  TofDataSet();
  ~TofDataSet();

 public:
  RecordSet* getBarrelData( unsigned int i )   const { return barrelData[i]; }
  RecordSet* getEndcapData( unsigned int i )   const { return endcapData[i]; }
  RecordSet* getEtfData( unsigned int i )      const { return etfData[i];    }

 public:
  void setData( TTree*, unsigned int );
  void setBarrelDataFiles( std::vector<std::string>& );
  void setEndcapDataFiles( std::vector<std::string>& );
  void setEtfDataFiles( std::vector<std::string>& );

  void setBarrelData( RecBTofCalHitCol& );
  void setEndcapData( RecETofCalHitCol& );
  void setEtfData( RecBTofCalHitCol& );

 private:
  RecordSet* barrelData[NBarrel];
  RecordSet* endcapData[NEndcap];
  RecordSet* etfData[NEtf*NStrip];
};

#endif
