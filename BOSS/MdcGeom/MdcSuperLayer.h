// $Id: MdcSuperLayer.h,v 1.1.1.1 2005/07/18 05:51:56 codeman Exp $ // -*-c++-*-
// header file for a class called "MdcSuperLayer"
#ifndef __MDCSUPERLAYER_H__
#define __MDCSUPERLAYER_H__

#include <iostream>
#include "MdcGeom/MdcLayer.h"

class MdcSuperLayer {

  public:
    // Constructor.
     MdcSuperLayer( int number);

    // Destructor
    ~MdcSuperLayer();

  public: // Extractors
  
    int index(void)        const  {return _slayer; }
    int whichView(void)    const {return _view;}
    bool exist(void)       const {return _exist;}
    double rEnd(void)      const {return _radius;}
    double rad0(void)      const { return 0.5 * (firstLayer()->rMid() +
                                                 lastLayer()->rMid()); }
    double zEnd(void)      const { return 0.5 * (firstLayer()->zEnd() +
                                                 lastLayer()->zEnd()); }
    double stDip(void)     const { return 0.5 * (firstLayer()->stDip() +
                                                 lastLayer()->stDip()); }
    double delPhi(void)    const {return _delphi;}
    double delPhiinv(void) const {return _delphiinv;}
    //----------------------------------------------------------
    // here |index| is the index of array of pointers to layers
    // belonging to the superlayer, so this ramges from 0 to 3
    //----------------------------------------------------------

    const MdcLayer* firstLayer(void) const { return layers[0];}
    const MdcLayer* lastLayer(void)  const { return layers[_nLayers - 1];}
    const MdcSuperLayer* next(void)       const { return _next;}
    const MdcSuperLayer* prev(void)       const { return _prev;}
    const MdcSuperLayer* nextInView(void) const { return _nextInView;}
    const MdcSuperLayer* prevInView(void) const { return _prevInView;}
    int   slayNum(void)                 const { return _slayer;}
    int   nLayers(void)                 const { return _nLayers;}
    const MdcLayer* layer(int i)     const { /*yzhang SKIP assert ( i>=0 && i < nLayer() ) ;*/
//    const MdcLayer* layer(int i)     const { assert ( i>=0 && i < _nLayers ) ;
                                                return layers[i]; }
    void setNextInView(MdcSuperLayer* sl)  {_nextInView = sl;}
    void setPrevInView(MdcSuperLayer* sl)  {_prevInView = sl;}
    // One line printout
    void print(std::ostream& o=std::cout) const ;


  private:
    friend class MdcDetector;
    void addLayer(int index, const MdcLayer * lay);
    void updateInfo(const MdcSuperLayer* prev, const MdcSuperLayer* next);

  // Data members
    bool   _exist;
    double _radius;    // mean rad.
    double _delphi;    // diff in phi between z=0 and zend (=0 for axial
    double _delphiinv;
    int   _view;       // +1, 0, -1 = U, axial, V
    int _slayer;   // superlayer number

    const MdcLayer* layers[4];
    const MdcSuperLayer* _next;
    const MdcSuperLayer* _prev;
    const MdcSuperLayer* _nextInView;
    const MdcSuperLayer* _prevInView;
    int   _nLayers;    // number of layers

};

#endif /* MdcSuperLayer_CLASS */

