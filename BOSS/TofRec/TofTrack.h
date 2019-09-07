#ifndef TOFTRACK_H
#define TOFTRACK_H

#include "ExtEvent/RecExtTrack.h"
#include "RawDataProviderSvc/TofData.h"
#include "TofRecEvent/RecTofTrack.h"
#include "TofRecEvent/RecBTofCalHit.h"
#include "TofRecEvent/RecETofCalHit.h"

const double adc_Cut = 10000.0;
const double tdc_Cut = 100000.0;
const double ztdc_Cut = 15.0;
const double ztdc_Cut1 = 1.65*4.0;
const double ztdc_Cut2 = 1.73*4.0;
const double zadc_Cut = 25.0;
const double ztdc_EtfCut = 15.0;
const double mass[5] = { 0.00051100, 0.10566, 0.13957, 0.49368, 0.93827 };
const double stripLength[12] = { 8.6, 9.1, 9.6, 10.1, 10.6, 11.1, 11.6, 12.1, 12.6, 13.1, 13.6, 14.1};

enum ExtTrackCase { InnerLayer,OuterLayer,DoubleLayer,EastEndcap,WestEndcap,EastEndcapMRPC, WestEndcapMRPC, NoHit};

class TofTrack{
 public:
  TofTrack( int run, int event );
  ~TofTrack();

  int trackId()                      const { return  m_trackId;    }
  int tofTrackId()                   const { return  m_tofTrackId; }
  int id1()                          const { return  m_id1;        }
  int id2()                          const { return  m_id2;        }
  int istrip1()                      const { return  m_istrip1;    }
  int istrip2()                      const { return  m_istrip2;    }
  int dstrip1()                      const { return  m_delStrip1;  }
  int dstrip2()                      const { return  m_delStrip2;  }
  unsigned int barrel()              const { return  m_barrel;     }
  ExtTrackCase hitCase()             const { return  m_hitCase;    }
  double p()                         const { return  m_momentum;   }
  double path()                      const { return  m_path;       }
  double path1()                     const { return  m_path1;      }
  double path2()                     const { return  m_path2;      }
  double zrhit1()                    const { return  m_zrhit1;     }
  double zrhit2()                    const { return  m_zrhit2;     }
  double errzrhit1()                 const { return  m_errzr1;     }
  double errzrhit2()                 const { return  m_errzr2;     }
  double xhit1()                     const { return  m_xhit1;      }
  double yhit1()                     const { return  m_yhit1;      }
  double xhit2()                     const { return  m_xhit2;      }
  double yhit2()                     const { return  m_yhit2;      }
  int kal(unsigned int i)            const { return  m_kal[i];     }
  double zr1(unsigned int i)         const { return  m_zr1[i];     }
  double zr2(unsigned int i)         const { return  m_zr2[i];     }
  double errzr1(unsigned int i)      const { return  m_ezr1[i];    }
  double errzr2(unsigned int i)      const { return  m_ezr2[i];    }
  std::vector<TofData*> tofData1()   const { return  m_tofData1;   }  
  std::vector<TofData*> tofData2()   const { return  m_tofData2;   }  
  int size1()                        const { return  m_tofData1.size(); }
  int size2()                        const { return  m_tofData2.size(); }
  int size3()                        const { return  m_tofData3.size(); }
  int size4()                        const { return  m_tofData4.size(); }
  double theta1()                    const { return  m_theta1;     }
  double theta2()                    const { return  m_theta2;     }
  double phi1()                      const { return  m_phi1;       }
  double phi2()                      const { return  m_phi2;       }
  unsigned int quality1()            const { return  m_quality1;   }
  unsigned int quality2()            const { return  m_quality2;   }
  unsigned int quality()             const { return  m_quality;    }

  int tofId1()                       const { return  m_tofId1;    }
  int tofId2()                       const { return  m_tofId2;    }
  int strip1()                       const { return  m_strip1;    }
  int strip2()                       const { return  m_strip2;    }

  double ph11()                   const { return  m_ph11;       }
  double ph12()                   const { return  m_ph12;       }
  double ph21()                   const { return  m_ph21;       }
  double ph22()                   const { return  m_ph22;       }
  double ph1()                    const { return  m_ph1;        }
  double ph2()                    const { return  m_ph2;        }
  double ph()                     const { return  m_ph;         }

  double tof11(unsigned int i)    const { return  m_tof11[i];   }
  double tof12(unsigned int i)    const { return  m_tof12[i];   }
  double tof21(unsigned int i)    const { return  m_tof21[i];   }
  double tof22(unsigned int i)    const { return  m_tof22[i];   }
  double tof1(unsigned int i)     const { return  m_tof1[i];    }
  double tof2(unsigned int i)     const { return  m_tof2[i];    }
  double tof(unsigned int i)      const { return  m_tof[i];     }

  double qch1()                   const { return  m_qch1;       }
  double qch2()                   const { return  m_qch2;       }
  double qch3()                   const { return  m_qch3;       }
  double qch4()                   const { return  m_qch4;       }
  double adc1()                   const { return  m_adc1;       }
  double adc2()                   const { return  m_adc2;       }
  double adc3()                   const { return  m_adc3;       }
  double adc4()                   const { return  m_adc4;       }
  double tdc1()                   const { return  m_tdc1;       }
  double tdc2()                   const { return  m_tdc2;       }
  double tdc3()                   const { return  m_tdc3;       }
  double tdc4()                   const { return  m_tdc4;       }

  double texpInner(unsigned int i) const { return m_texpInner[i]; }
  double texpOuter(unsigned int i) const { return m_texpOuter[i]; }
  double texp(unsigned int i)      const { return m_texp[i];      }

  double ztdc1()                  const { return  m_ztdc1;      }
  double ztdc2()                  const { return  m_ztdc2;      }
  double zadc1()                  const { return  m_zadc1;      }
  double zadc2()                  const { return  m_zadc2;      }

  double estime()                 const { return  m_estime;     }
  double tdiff1()                 const { return  m_tdiff1;     }
  double tdiff2()                 const { return  m_tdiff2;     }

  int t0Stat()                    const { return  m_t0Stat;     }

  unsigned int flag()             const { return  m_flag;       }

 public:
  bool isNoHit()                  const { return  m_hitCase==NoHit; }
  void setQuality( int qual )           { m_quality  = qual;        }
  void setQuality1( int qual1 )         { m_quality1 = qual1;       }
  void setQuality2( int qual2 )         { m_quality2 = qual2;       }
  void setFlag( unsigned int flag )     { m_flag = flag;            }

  void setExtTrack( RecExtTrack* extTrack, double costheta, double p[5], int kal[5], double t0, int t0Stat );
  void getMultiHit( TofTrack*& );
  void setTofData( TofDataMap tofDataMap );
  void tofDataAnalysis( TofData* tof, unsigned int iflag );
  void getTofData( TofData* tof, unsigned int iflag );
  void getTofDataEast( TofData* tof, unsigned int iflag );
  void getTofDataWest( TofData* tof, unsigned int iflag );
  void getTofDataNohit( unsigned int iflag );
  void getEtfData( TofData* tof, unsigned int iflag, unsigned int qual );
  void match( bool forCalibration, std::vector<int> deadId, std::vector<TofTrack*>*& tofTrackVec );
  void findTofDataBarrel( std::vector<TofData*> tofDataVec1, std::vector<TofData*> tofDataVec2, double zrhit, unsigned int iflag, std::vector<TofTrack*>*& tofTrackVec );
  TofData* chooseTofData( std::vector<TofData*> tofDataVec, double zrhit );
  TofData* compareTofData( TofData* tofData1, TofData* tofData2, double zrhit );
  void findTofDataEndcap( std::vector<TofData*> tofDataVec1, std::vector<TofData*> tofDataVec2, double zr1[5] );
  TofData* chooseTofDataEndcap( std::vector<TofData*> tofDataVec, double zr1[5] );
  TofData* compareTofDataEndcap( TofData* tofData1, TofData* tofData2 );
  void findEtfData( std::vector<TofData*> tofDataVec1, std::vector<TofData*> tofDataVec2, std::vector<TofData*> tofDataVec3, double zrhit, unsigned int iflag );
  TofData* chooseEtfData1( std::vector<TofData*> tofDataVec, double zrhit );
  TofData* chooseEtfData2( std::vector<TofData*> tofDataVec, double zrhit );
  //  TofData* compareEtfData( TofData* tofData1, TofData* tofData2, double zrhit );

  void tofDataStudy();

  void setCalibration();

  void convert2RecTofTrackCol( RecTofTrackCol* recTofTrackCol );
  void buildRecTofTrack( RecTofTrack* track, int layerorend );
  void setRecTofTrack( RecTofTrack* track, int layerorend );

  void convert2RecBTofCalHitColBarrel( int runNumber, int eventNumber, RecBTofCalHitCol* btofCalHitCol, std::string calibData );
  void convert2RecETofCalHitCol( int runNumber, int eventNumber, RecETofCalHitCol* etofCalHitCol, std::string calibData );
  void convert2RecBTofCalHitColETF( int runNumber, int eventNumber, RecBTofCalHitCol* btofCalHitCol, std::string calibData );

  void qualityAnalysis();

 private:
  int  m_run,  m_event;
  int  m_trackId;
  int  m_tofTrackId;
  int  m_id1, m_id2, m_istrip1, m_istrip2;
  int  m_tofId1, m_tofId2, m_strip1, m_strip2;
  int  m_delStrip1, m_delStrip2;
  unsigned int  m_barrel;
  ExtTrackCase  m_hitCase;
  double  m_momentum;
  double  m_path, m_path1, m_path2;
  double  m_zrhit1, m_errzr1, m_zrhit2, m_errzr2;
  double  m_xhit1, m_yhit1, m_xhit2, m_yhit2;
  int  m_kal[5];
  double  m_zr1[5], m_ezr1[5], m_zr2[5], m_ezr2[5];
  std::vector<TofData*> m_tofData1;
  std::vector<TofData*> m_tofData2;
  std::vector<TofData*> m_tofData3;
  std::vector<TofData*> m_tofData4;
  std::vector<TofData*> m_tofData5;
  std::vector<TofData*> m_tofData6;
  unsigned int m_quality1, m_quality2;
  unsigned int m_quality;
  // quality introduction
  // 17   16 15 14 13   12 11 10  9   8  7  6  5   4  3  2  1
  //      neighbor>1                  single end
  //         neighbor==1                 lost TDC
  //            hit counter>1               lost ADC
  //               hit counter==1              Raw data
  //                    calibration                q  t  q  t
  //                          zAdc not match       East  West
  //                              zTdc not match

  double m_theta1, m_theta2, m_phi1, m_phi2;

  double m_ph11, m_ph12, m_tof11[5], m_tof12[5];
  double m_ph21, m_ph22, m_tof21[5], m_tof22[5];
  double m_ph1,  m_ph2,  m_ph;
  double m_tof1[5], m_tof2[5], m_tof[5];

  double m_texp[5], m_texpInner[5], m_texpOuter[5];

  double m_sigma11, m_sigma12, m_sigma21, m_sigma22;
  double m_sigma1,  m_sigma2,  m_sigma;

  double m_qch1, m_qch2, m_qch3, m_qch4;
  double m_adc1, m_adc2, m_adc3, m_adc4;
  double m_tdc1, m_tdc2, m_tdc3, m_tdc4;

  double m_ztdc1, m_ztdc2, m_zadc1, m_zadc2;

  double m_estime, m_tdiff1, m_tdiff2;

  int m_t0Stat;

  unsigned m_flag;

};

typedef std::vector<TofTrack*> TofTrackVec;

#endif   // TOFTRACK_H
