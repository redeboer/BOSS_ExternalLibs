#ifndef CALIBDATA_BARRELELEC_H
#define CALIBDATA_BARRELELEC_H
#include "GaudiKernel/DataObject.h"
#include "CalibData/CalibModel.h"
#include <vector>

namespace CalibData {
class BarrelElec{
 public:
  BarrelElec()  {}
  ~BarrelElec() {}
 public:
  unsigned int getNumEast( int idx )     const { return  m_num[0][idx];    }
  unsigned int getNumWest( int idx )     const { return  m_num[1][idx];    }
  double getP1( int idx )                const { return  m_pEast[idx];     }
  double getP2( int idx )                const { return  m_pWest[idx];     }
  double getSimP1( int idx )             const { return  m_simPEast[idx];  }
  double getSimP2( int idx )             const { return  m_simPWest[idx];  }

  void  setNumEast( unsigned int idx, unsigned int num )   { m_num[0][idx] = num;   }
  void  setNumEast( unsigned int num[4] ) {
    for( unsigned int i=0; i<4; i++ )  { m_num[0][i] = num[i]; }
  }
  void  setNumWest( unsigned int idx, unsigned int num )   { m_num[1][idx] = num;   }
  void  setNumWest( unsigned int num[4] ) {
    for( unsigned int i=0; i<4; i++ )  { m_num[1][i] = num[i]; }
  }
  void  setP1( unsigned int idx, double p )         { m_pEast[idx] = p;    }
  void  setP1( double p[11] ) {
    for( unsigned int i=0; i<11; i++ ) { m_pEast[i] = p[i]; }
  }
  void  setP2( unsigned int idx, double p )         { m_pWest[idx] = p;    }
  void  setP2( double p[11] ) {
    for( unsigned int i=0; i<11; i++ ) { m_pWest[i] = p[i]; }
  }
  void  setSimP1( unsigned int idx, double p )      { m_simPEast[idx] = p; }
  void  setSimP1( double p[11] ) {
    for( unsigned int i=0; i<11; i++ ) { m_simPEast[i] = p[i]; }
  }
  void  setSimP2( unsigned int idx, double p )      { m_simPWest[idx] = p; }
  void  setSimP2( double p[11] ) {
    for( unsigned int i=0; i<11; i++ ) { m_simPWest[i] = p[i]; }
  }

 private:
  unsigned int m_num[2][4];
  double m_pEast[11];
  double m_pWest[11];
  double m_simPEast[11];
  double m_simPWest[11];
};
}
#endif
