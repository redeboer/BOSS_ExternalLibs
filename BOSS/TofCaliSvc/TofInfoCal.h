#ifndef TOFINFO_CAL_H
#define TOFINFO_CAL_H 

class TofInfoCal{
 public:
  TofInfoCal() {}
  ~TofInfoCal() {}
 public:
  int getRunBegin()          const { return  m_run1;     }
  int getRunEnd()            const { return  m_run2;     }
  int getVersion()           const { return  m_version;  }

  int getQCorr()             const { return  m_qcorr;    }
  int getQElec()             const { return  m_qelec;    }
  int getMisLable()          const { return  m_mislable; }

  int getBrEast(unsigned int im)   const { return m_breast[im]; }
  int getBrWest(unsigned int im)   const { return m_brwest[im]; }
  int getEndcap(unsigned int im)   const { return m_endcap[im]; }
  int getEtf(unsigned int im)      const { return m_etf[im];    }

 public:
  void setRunBegin( int run1 )     { m_run1 = run1;        }
  void setRunEnd( int run2 )       { m_run2 = run2;        }
  void setVersion( int version )   { m_version = version;  }

  void setQCorr( int qcorr )       { m_qcorr = qcorr;      }
  void setQElec( int qelec )       { m_qelec = qelec;      }
  void setMisLable( int mis )      { m_mislable = mis;     }

  void setBrEast(unsigned int i, int id1) { m_breast[i] = id1; }
  void setBrWest(unsigned int i, int id2) { m_brwest[i] = id2; }
  void setEndcap(unsigned int i, int id3) { m_endcap[i] = id3; }
  void setEtf(unsigned int i, int id4)    { m_endcap[i] = id4; }

 private:
  int  m_run1, m_run2, m_version;
  int  m_qcorr, m_qelec, m_mislable;
  int  m_breast[5], m_brwest[5], m_endcap[5], m_etf[5];

};

#endif

