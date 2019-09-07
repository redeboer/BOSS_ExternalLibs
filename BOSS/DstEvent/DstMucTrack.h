#ifndef DST_MUCTRACK_H
#define DST_MUCTRACK_H
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ContainedObject.h"
#include "EventModel/EventModel.h"
#include <vector>

using namespace std;

using namespace EventModel;
extern const CLID &CLID_DstMucTrack;

//class MucTrack;
class DstMucTrack : virtual public ContainedObject {

public:
   DstMucTrack() {}

   /// Assignment constructor.
   //DstMucTrack& operator=(const MucTrack& source);

   //void copy(const MucTrack* orig);

   /// Copy constructor.
   //DstMucTrack(const MucTrack& source);

   virtual ~DstMucTrack() {}

  virtual const CLID& clID()  const { return  DstMucTrack::classID();}
  static  const CLID& classID() { return CLID_DstMucTrack; }
   
  int  trackId()   const     { return   m_trackId ; }
  int  id()        const     { return   m_id; }
  int  status()    const     { return   m_status; } 
  int  type()      const     { return   m_type;  }

  int  startPart()         const     { return   m_startPart; }    // start position of track locates in which part
  int  endPart()           const     { return   m_endPart; }      // end   position of track locates in which part
  int  brLastLayer()       const     { return   m_brLastLayer;  } // last layer with hits in barrel 
  int  ecLastLayer()       const     { return   m_ecLastLayer;  } // last layer with hits in end cap
  int  numHits()           const     { return   m_numHits;      } // total hits on the track   
  int  numLayers()         const     { return   m_numLayers;    } // number of layers with hits 
  int  maxHitsInLayer()    const     { return   m_maxHitsInLayer; }
  
  double  depth()   const   { return   m_depth;  }    // depth of the track transport in iron
  double  chi2()    const   { return   m_chi2;   }
  int     dof()     const   { return   m_dof;    }
  double  rms()     const   { return   m_rms;    }

  double  xPos()  const { return m_xPos; }   // position on the start of track in muc (vertex) 
  double  yPos()  const { return m_yPos; }
  double  zPos()  const { return m_zPos; }

  double  xPosSigma()  const { return m_xPosSigma; } 
  double  yPosSigma()  const { return m_yPosSigma; }
  double  zPosSigma()  const { return m_zPosSigma; }
  
  double  px()    const { return m_px; }   // momentum on the start of track in muc
  double  py()    const { return m_py; }
  double  pz()    const { return m_pz; }

  double  distance() const { return m_distance;}
  double  deltaPhi() const { return m_deltaPhi;}
  
  vector<int> vecHits() const { return  m_vecHits ;}
  
  //added by LI Chunhua 2013/02/01
  double kalRechi2() const {return m_kalrechi2;}
  int    kaldof()    const {return m_kaldof;}
  double kaldepth()  const {return m_kaldepth;}
  int    kalbrLastLayer() const {return m_kalbrLastLayer;}
  int    kalecLastLayer() const {return m_kalecLastLayer;}
  //**********************
  // Set
  void  setTrackId(int trackId) { m_trackId = trackId ;}
  void  setId(int id)      { m_id = id ; }
  void  setStatus(int st)  { m_status = st;  }
  void  setType(int type)  { m_type = type; }

  void  setStartPart(int part) {m_startPart = part; }
  void  setEndPart(int part)   {m_endPart = part;   }
  void  setBrLastLayer(int layer) {m_brLastLayer = layer; }
  void  setEcLastLayer(int layer) {m_ecLastLayer = layer; }
  void  setNumHits( int numHits)          { m_numHits  = numHits; }
  void  setNumLayers(int numLayers)       { m_numLayers = numLayers; }
  void  setMaxHitsInLayer(int maxHits)      { m_maxHitsInLayer = maxHits; }

  void  setDepth( double depth) { m_depth  = depth; }
  void  setChi2(double chi2)    { m_chi2   = chi2; }
  void  setDof(int dof)         { m_dof    = dof ; }
  void  setRms(double rms)      { m_rms    = rms ; }

  void  setXPos(double x)  { m_xPos = x; }
  void  setYPos(double y)  { m_yPos = y; }
  void  setZPos(double z)  { m_zPos = z; }
 
  void  setXPosSigma(double xsigma)  { m_xPosSigma = xsigma; }
  void  setYPosSigma(double ysigma)  { m_yPosSigma = ysigma; }
  void  setZPosSigma(double zsigma)  { m_zPosSigma = zsigma; }
  
  void  setPx( double px)  { m_px = px;  }
  void  setPy( double py)  { m_py = py;  }
  void  setPz( double pz)  { m_pz = pz;  }

  void  setDistance( double dist){m_distance = dist; }
  void  setDeltaPhi( double dphi){m_deltaPhi = dphi; }
  
  void  setVecHits(vector<int>& vecHits) { m_vecHits = vecHits; }
  //added by LI Chunhua
  void  setkalRechi2(double ch){m_kalrechi2=ch;}
  void  setkalDof(int f){m_kaldof = f;}
  void  setkalDepth(double de){m_kaldepth = de;}
  void  setkalbrLastLayer(int br){m_kalbrLastLayer = br;}
  void  setkalecLastLayer(int ec){m_kalecLastLayer = ec;}
  //******************

protected:
    int m_trackId;    // Track ID wensp Add 2005-10-19
    int m_id;              // Muc Identifier
    int m_status;          // Muc Status
    int m_type;            // Method of judge?

    int m_startPart;       // In which part does the start position of track locate
    int m_endPart;         // In which part does the end   position of track locate
    int m_brLastLayer;     // Last layer with hits in barrel 
    int m_ecLastLayer;     // Last layer with hits in endcap 
    int m_numHits;         // Number of hits on track
    int m_numLayers;       // Number of layer with  hits
    int m_maxHitsInLayer;  // Max number of hits in a layer
             
    double m_depth;        // Depth of iron penetrated
    double m_chi2;         // Fit parameters
    int    m_dof; 
    double m_rms;
    
    double m_xPos;       // x position of muc track vertex
    double m_yPos;       // y position of muc track vertex
    double m_zPos;       // z position of muc track vertex
 
    double m_xPosSigma;  // sigma of x position of muc track vertex in first gap local coordinate.
    double m_yPosSigma;  // sigma of y ...
    double m_zPosSigma;  // sigma of z ...   ... so it' 0;
    
    double m_px;         // px on muc track vertex
    double m_py;         // py on muc track vertex
    double m_pz;         // pz on muc track vertex
  
    double m_distance;
    double m_deltaPhi;
    
    vector<int> m_vecHits; //// hit list of the track
    
    //added by LI Chunhua
    double m_kalrechi2;
    int m_kaldof;
    double m_kaldepth;
    int m_kalbrLastLayer;
    int m_kalecLastLayer;
    //***************
};

typedef ObjectVector<DstMucTrack> DstMucTrackCol;
#endif //NDST_DstMucTrack_H

