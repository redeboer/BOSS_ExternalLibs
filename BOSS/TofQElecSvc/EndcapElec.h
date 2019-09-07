#ifndef ENDCAPELELEC_H
#define ENDCAPELELEC_H

class EndcapElec{
 public:
  EndcapElec()  {}
  ~EndcapElec() {}
 public:
  unsigned int getNum( unsigned int idx )  const { return  m_num[idx];  }
  double getP( unsigned int idx )          const { return  m_p[idx];    }
  double getSimP( unsigned int idx )       const { return  m_simP[idx]; }

  void  setNum( unsigned int idx, unsigned int num )   { m_num[idx] = num;}
  void  setNum( unsigned int num[4] ) {
    for( unsigned int i=0; i<4; i++ ) m_num[i] = num[i];
  }
  void  setP( unsigned int idx, double p )             { m_p[idx] = p;    }
  void  setP( double p[11] ) {
    for( unsigned int i=0; i<11; i++ ) { m_p[i] = p[i]; }
  }
  void  setSimP( unsigned int idx, double p )          { m_simP[idx] = p; }
  void  setSimP( double p[11] ) {
    for( unsigned int i=0; i<11; i++ ) { m_simP[i] = p[i]; }
  }

 private:
  unsigned int m_num[4];
  double m_p[11];
  double m_simP[11];
}

#endif
