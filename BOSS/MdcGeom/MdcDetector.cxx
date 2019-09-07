#include "MdcGeom/MdcDetector.h"
#include "Identifier/MdcID.h"
#include "MdcGeomSvc/MdcGeomSvc.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"



static const int _layPerSL = 4;
MdcDetector * MdcDetector::_myself = 0;
bool MdcDetector::_doSag = false;


MdcDetector * MdcDetector::instance(bool doSag){
  _doSag = doSag;
  return instance();
}

MdcDetector * MdcDetector::instance()
{
  if( 0 == _myself ) {
    _myself = new MdcDetector();
  }
  return _myself;
} 

MdcDetector::MdcDetector() 
{

  std::cout << " +---------------------------------------------------+\n"
    << " |  Creating Mdc Geometry information                |\n"
    << " +---------------------------------------------------+\n";    

  IService* ser;
  StatusCode sc = Gaudi::svcLocator()->getService("MdcGeomSvc",ser);
  if (!sc.isSuccess())
    std::cout <<" MdcDetector::Could not open Geometry Service"<<std::endl;
  MdcGeomSvc *mdcsvc = dynamic_cast<MdcGeomSvc*> (ser);
  if(!mdcsvc) std::cout <<"MdcDetector::Could not open Geometry Service"<<std::endl;

  _nSWire = mdcsvc->getWireSize();
  _nLayer = mdcsvc->getLayerSize();
  _nSlay  = mdcsvc->getSuperLayerSize();

  _senseWire = new MdcSWire* [_nSWire];
  _dclayer   = new MdcLayer* [_nLayer];
  _slayList  = new MdcSuperLayer* [_nSlay];

  // build the sense wires
  for( int iwire = 0; iwire < _nSWire; iwire++ ) {
    const MdcGeoWire *geowir = mdcsvc->Wire(iwire);
    HepPoint3D eastP = geowir->Backward()/10.0;
    HepPoint3D westP = geowir->Forward() /10.0;
    //std::cout<< "wire ("<<geowir->Layer()<<","<<geowir->Cell()<<") east "<<eastP <<",west " <<westP << std::endl;//yzhang debug
    double sag=0.;
    if(_doSag) sag = geowir->Sag()/10.;//mm->cm

    //std::cout<<"sag = "<<sag<<" "<< geowir->Sag()<<std::endl;
    _senseWire[iwire] = new MdcSWire(eastP, westP, sag, geowir->Id(), geowir->Cell());


  }

  // build sense wire layers
  for( int ilay = 0; ilay < _nLayer; ilay++ ) {
    const MdcGeoLayer *geolay = mdcsvc->Layer(ilay);
    int nwir = geolay->NCell();
    int firstwir = geolay->Wirst();
    _dclayer[ilay] = new MdcLayer(ilay, nwir, &_senseWire[firstwir],*this);
    _wires_in_layer[ilay] = nwir;
  }

  // build pointers to make navigation faster
  buildpointers();

  // do superlayers
  // --------------
  buildSuperLayers();
  // set nominal cell height for each layer!!!! check!!!!
  double rOther;
  for(int ilay = 0; ilay < _nLayer; ilay++) {
    if( ilay == 0) {
      rOther = Layer(ilay + 1)->rMid();
    } else {
      rOther = Layer(ilay - 1)->rMid();
    }
    double height = fabs(Layer(ilay)->rMid() - rOther);
    const_cast<MdcLayer*>(Layer(ilay))->setCellHeight(height);
  }
}



MdcDetector::~MdcDetector()
{
  for( int iwire = 0; iwire < _nSWire; iwire++ ) {
    delete _senseWire[iwire];
  }
  for( int ilay = 0; ilay < _nLayer; ilay++ ) {
    delete _dclayer[ilay];
  }
  for( int islay =0; islay < _nSlay; islay++ ) {
    _slayList[islay]->print(std::cout);
    delete _slayList[islay];
  }

  delete [] _senseWire;
  delete [] _dclayer;
  delete [] _slayList;
}


  void 
MdcDetector::buildpointers(void) 
{
  // first layers
  //  for (int index=0; index<lastLayNum()-firstLayNum()+1; index++) {
  for (int index=0; index<_nLayer; index++) {
    // initialize
    _nextlay[index]=0; _prevlay[index]=0; 
    _nextlayinvw[index]=0; _prevlayinvw[index]=0;
    //    int layi=index+firstLayNum();

    //    if ( !existDet(layi) ) {
    //      ErrMsg(fatal) << " layer # " << layi << " does not exist!" <<endmsg;
    //    }
    //    _dclayer[index] = dchLayer(layi);

    //next and previous pointers
    //    if (existDet(layi+1)) { _nextlay[index] = dchLayer(layi+1); }
    //    if (existDet(layi-1)) { _prevlay[index] = _dclayer[index-1]; }

    if ( index + 1 < _nLayer ) { _nextlay[index] = Layer(index+1); }
    if ( index > 0                  ) { _prevlay[index] = Layer(index-1); }

    //next in view pointer
    //    int iview=dchLayer(layi)->view();
    int iview = Layer(index)->view();
    int jndex;
    //    for (jndex=index+1; jndex<lastLayNum()-firstLayNum()+1; jndex++) {
    for (jndex=index+1; jndex<_nLayer; jndex++) {
      //      int layj=jndex+firstLayNum();
      //      if ( !existDet(layj) ) {
      //	ErrMsg(fatal) << " layer # " << layj << " does not exist!" <<endmsg;
      //      }
      if ( iview != Layer(jndex)->view() ) continue;
      _nextlayinvw[index] = Layer(jndex+1);
      break;
    } //(int jndex=index+1; _dclayer[jndex].Exist(); jndex++) 

    //prev in view pointer
    for (jndex=index-1; jndex>=0 ; jndex--) {
      //      int layj=jndex+firstLayNum();
      //      if ( !existDet(layj) ) {
      //	ErrMsg(fatal) << " layer # " << layj << " does not exist!" <<endmsg;
      //      }
      if ( iview!=Layer(jndex)->view() ) continue;
      _prevlayinvw[index] = Layer(jndex);
      break;
    } //(int jndex=index+1; _dclayer[jndex].exist(); jndex++) 

  } //(int index=0; _dclayer[index].exist(); index++)
  }


  void 
    MdcDetector::buildSuperLayers(void) 
    {
      // some initializations
      _nAxSlay = 0;
      _nSterSlay[0] = _nSterSlay[1] = 0;

      //  _nSlay = _nLayer/_layPerSL;

      _firstSlayNum = 1;
      _lastSlayNum = _nSlay;
      //  _slayList = new MdcSuperLayer*[_nSlay];

      // initialize pointers 
      _firstSlayInView[0] = _firstSlayInView[1] =_firstSlayInView[2] = 0;
      _lastSlayInView[0] = _lastSlayInView[1] =_lastSlayInView[2] = 0;

      int islay;

      // build the SuperLayers
      for (islay = 0; islay < _nSlay; islay++) {
	//    MdcSuperLayer* superlay = new MdcSuperLayer("Dch SuperLayer",1001+islay);
	MdcSuperLayer* superlay = new MdcSuperLayer(islay);
	//    *_dchslayerset += *superlay;
	_slayList[islay] = superlay;
      } 
      _firstSlay = _slayList[0];
      _lastSlay = _slayList[_nSlay-1];

      // set pointers to Layers in SuperLayers
      //  for (int lay=firstLayNum(); lay<=lastLayNum(); lay++) {
      for (int lay = 0; lay < _nLayer; lay++) {
	int superlayer = lay / _layPerSL;
	int index = lay % _layPerSL;
	_slayList[superlayer]->addLayer(index,getMdcLayer(lay)); 
      }
      // update SuperLayer data members
      MdcSuperLayer* oldSlayByView[3] = {0,0,0};

      for (islay = 0; islay < _nSlay; islay++) {
	const MdcSuperLayer* prev = 0;
	const MdcSuperLayer* next = 0;
	//    const MdcSuperLayer* prevInView = 0;
	//    const MdcSuperLayer* nextInView = 0;

	// SuperLayer view
	int iview = _slayList[islay]->layer(0)->view();
	int viewIndex = iview+1;
	// count SuperLayer types
	if ( iview == 0 ) _nAxSlay++;
	else if ( iview == -1 ) _nSterSlay[0]++;
	else if ( iview == 1 ) _nSterSlay[1]++;

	// build pointer links
	if ( islay > 0 ) prev = _slayList[islay - 1];
	if ( islay < _nSlay-1 ) next = _slayList[islay + 1];

	// fill first and last SuperLayer pointers
	if (firstSlayInView(iview) == 0) _firstSlayInView[viewIndex] =
	  _slayList[islay];
	_lastSlayInView[viewIndex] = _slayList[islay];

	_slayList[islay]->updateInfo(prev, next);
	// now the poiters to SuperLayers of the same view
	if (oldSlayByView[viewIndex] != 0) {
	  oldSlayByView[viewIndex]->setNextInView(_slayList[islay]);
	  _slayList[islay]->setPrevInView(oldSlayByView[viewIndex]);
	}

	oldSlayByView[viewIndex] = _slayList[islay];
      }
    }




    const MdcSWire*
      MdcDetector::Wire(const Identifier&  id) const
      {
	int  wire = MdcID::wire(id);
	int  layer= MdcID::layer(id);
	return Wire(layer, wire);
	//  unsigned wireid= Layer(layer)->Wirst()+wire;
	//  if( wireid < fWires.size()){
	//          return fWires[wireid];
	//  }
	//  else {
	//  return 0;
	//  }
      }

    const MdcLayer*
      MdcDetector::Layer(const Identifier&  id) const
      {
	unsigned layerid = MdcID::layer(id);
	return Layer(layerid);
	//  if (layerid < fLayers.size())
	//         return fLayers[layerid];
	//
	//         return 0;
      }

