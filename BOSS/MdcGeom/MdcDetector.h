// $Id: MdcDetector.h,v 1.2 2009/05/31 11:50:46 zhangy Exp $ // -*-c++-*-
// header file for a class called "MdcDetector"
#ifndef MDCDETECTOR_H
#define MDCDETECTOR_H

#include <CLHEP/Units/PhysicalConstants.h>                                      
#include <CLHEP/Geometry/Point3D.h> 
#include <vector>
#include <iostream>

#include "MdcGeom/MdcSWire.h"
#include "MdcGeom/MdcLayer.h"
#include "MdcGeom/MdcSuperLayer.h"
#include "Identifier/Identifier.h"


using namespace std;

class MdcDetector
{
  public:
    // Constructors and destructors
    MdcDetector();
    ~MdcDetector();
    static MdcDetector * instance(bool doSag);
    static MdcDetector * instance();

    const MdcSWire    * Wire(unsigned id) const { return _senseWire[id];}
    const MdcSWire    * Wire(const Identifier&  id) const;
    const MdcSWire    * Wire(unsigned lyrid, unsigned wirid) const 
    { return getMdcLayer(lyrid)->getWire(wirid); }  

    const MdcLayer    * Layer(unsigned id) const { return _dclayer[id]; }
    const MdcLayer    * Layer(const Identifier&  id) const;
    MdcLayer* getMdcLayer(int laynum) const { return _dclayer[laynum]; }
    const MdcLayer *firstLayer() const { return Layer(0); }
    const MdcLayer *lastLayer () const { return Layer(_nLayer - 1); }
    const MdcLayer* nextLayer(int lay) const { return _nextlay[lay]; }
    const MdcLayer* prevLayer(int lay) const { return _prevlay[lay]; }
    const MdcLayer* nextLayer(const MdcLayer* layer) const
    { return _nextlay[layer->layNum()]; } 
    const MdcLayer* prevLayer(const MdcLayer* layer) const
    { return _prevlay[layer->layNum()]; } 

    const MdcSuperLayer * SuperLayer(unsigned id) const { return _slayList[id];}
    const MdcSuperLayer* firstSlay(void) const             {return _firstSlay;}
    const MdcSuperLayer* lastSlay(void) const               {return _lastSlay;}
    const MdcSuperLayer* firstSlayInView(int iview) const
    {return _firstSlayInView[iview+1];}
    const MdcSuperLayer* lastSlayInView(int iview) const
    {return _lastSlayInView[iview+1];}

    int nSuper()              const { return _nSlay; }
    int nLayer()              const { return _nLayer;}
    int nAxialSuper(void) const {return _nAxSlay;}
    int nStereoSuper(int iview) const 
    { return iview == 0 ? 0 : _nSterSlay[(iview+1)/2]; }

    void Dump();


  private:
    static MdcDetector * _myself;
    static bool _doSag;
    void buildpointers(void);          	// make the Layer & sLayer pointers
    void buildSuperLayers(void);       	// build super-layers

    MdcSWire** _senseWire; 		// pointer to all the sense wires
    MdcLayer** _dclayer;        	// 40 layers for chamber                                       
    int    _wires_in_layer[43]; 	// # of wires per layer
    const MdcLayer* _nextlay[43];      	// pointer to next layer
    const MdcLayer* _nextlayinvw[43];  	// pointer to next layer in same view
    const MdcLayer* _prevlay[43];      	// pointer to previous layer in view
    const MdcLayer* _prevlayinvw[43];  	// pointer to previous layer in same view
    MdcSuperLayer** _slayList;    	// array of pointers to slayers
    const MdcSuperLayer* _firstSlayInView[3];
    const MdcSuperLayer* _lastSlayInView[3];
    const MdcSuperLayer* _firstSlay;
    const MdcSuperLayer* _lastSlay;

    // description
    int  _version;
    bool _debug;
    int  _firstLayer;
    int  _nSWire;   	// total number of sense wires             
    int  _nLayer;   	// total number of layers 
    int  _firstSlayNum;
    int  _lastSlayNum;
    int  _nSlay;         // number of super layers
    int  _nAxSlay;       // number of axial super layers
    int  _nSterSlay[2];  // number of stero super layers(U V)
};

#endif /* MDCDETECTOR_H */


