#ifndef BESVISALG_H_
#define BESVISALG_H_

// something needed by Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/Tokenizer.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IPersistencySvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataStoreAgent.h"
#include "GaudiKernel/strcasecmp.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/DataStoreItem.h"


// something needed by semaphore 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <wait.h>

// something needed by root: standard root libraies
#include <TObject.h>
#include <TList.h>
#include <TRootEmbeddedCanvas.h>
#include <TTimer.h>
#include <TApplication.h>
#include <TInterpreter.h>
#include <TBranch.h>
#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>
#include <TGeoManager.h>
#include <TGButton.h>
#include <TGMenu.h>
#include <TGLayout.h>
#include <TGStatusBar.h>
#include <TGFileDialog.h>
#include <TGNumberEntry.h>
#include <TGMsgBox.h>
#include <TGProgressBar.h>
#include <TGTab.h>
#include <TGSplitter.h>
#include <TGCanvas.h>
#include <TSystem.h>
#include <TGListTree.h>
#include <TGLabel.h>
#include <TGButtonGroup.h>
#include <TGTextEntry.h>
#include <TGTextBuffer.h>
#include <TG3DLine.h>
#include <TGSlider.h>
#include <TCanvas.h>
#include <TView.h>
#include <TGeometry.h>
#include <TASImage.h>
#include <TColor.h>
//#include <TViewerX3D.h>
//#include <TViewerOpenGL.h>
//#include <TGLEditor.h>
//#include <TVirtualGL.h>
#include <TVirtualGeoPainter.h>
#include <TRootHelpDialog.h>

//// something from BesCvs Event
//#include "RootCnvSvc/RootInterface.h"
#include "RootEventData/TRecTrackEvent.h"
#include "RootEventData/TDigiEvent.h"
#include "RootCnvSvc/commonData.h"
#include "RootCnvSvc/RootInterface.h"
#include "RootCnvSvc/RootCnvSvc.h"
//
//#include "MdcRecEvent/MdcTrack.h"
//#include "MdcRecEvent/MdcDedx.h"
//#include "MdcRecEvent/MdcKalTrack.h" 
//#include "TofRecEvent/BTofTrack.h"
//#include "TofRecEvent/BTofHit.h"
//#include "TofRecEvent/ETofTrack.h"
//#include "TofRecEvent/ETofHit.h"
//#include "EmcRecEventModel/EmcRecShower.h"
//#include "EmcRecEventModel/EmcRecFraction.h"
//#include "MucRecEvent/MucTrack.h"
//#include "MucRecEvent/MucRecHit.h"
//
//#include "ExtEvent/ExtTrack.h" 
//#include "ExtEvent/ExtMucHit.h"
//
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
//
//#include "DstEvent/DstEvent.h"
//#include "DstEvent/DstMdcTrack.h"
//#include "DstEvent/DstMdcKalTrack.h" 
//#include "DstEvent/DstMdcDedx.h"
//#include "DstEvent/DstTofTrack.h"
//#include "DstEvent/DstEmcTrack.h"
//#include "DstEvent/DstMucTrack.h"
//#include "DstEvent/DstExtTrack.h" 

#include "ReconEvent/ReconEvent.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcDedx.h"
#include "TofRecEvent/RecTofTrack.h"
#include "EmcRecEventModel/RecEmcShower.h"
#include "MucRecEvent/RecMucTrack.h"
#include "RootEventData/TDisTrack.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"

// standard cpp lib
#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <map>

using namespace std;
using namespace Event;
using CLHEP::Hep3Vector;
using CLHEP::HepSymMatrix;

// testing share file
#define BUFFER "./buffer"

union semun{
	int	val;
	struct semid_ds	*buf;
	ushort	*array;
};

enum {FREE_SPACE, OUTPUT_STORE};

///////////////////////////////////////////////////////////////////////////////
class TList;
class TObject;
class TRootEmbeddedCanvas;
class TApplication;
class TInterpreter;
class TFile;
class TTimer;
class TBranch;
class TTree;
class TGPopupMenu;
class TGStatusBar;
class TGHorizontal3DLine;
class TGLayoutHints;
class TGMenuBar;
class TGNumberEntry;
class TGNumberFormat;
class TGLabel;
class TGHButtonGroup;
class TGRadioButton;
class TGCheckButton;
class TGTextButton;
class TGTextBuffer;
class TGTextEntry;
class TGHProgressBar;
class TGButton;
class TGPictureButton;
class TGFileInfo;
class TGFileDialog;
class TGMsgBox;
class TGGroupFrame;
class TGTab;
class TGVSplitter;
class TGCanvas;
class TGListTree;
class TGListTreeItem;
class TGHSlider;
class TGVSlider;
class TCanvas;
class TView;
class TGListTreeItem;


class BesVisAlg:public Algorithm {
public:
  BesVisAlg (const std::string& name, ISvcLocator* pSvcLocator);
  ~BesVisAlg();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  // gaudi frame's variables
  static int	counter;
  std::vector<std::string>	testVec;
  std::string			f_rootOutputFile;
  std::string			f_geoInputFile;
  // some variables needed by semaphore
  pid_t		c_pid;
  pid_t		m_pid;
  ushort	start_val[2];
  int		sem_value_F, sem_value_O;
  int		semid;
  struct semid_ds	sem_buf;
  union semun		arg;
  struct sembuf		acquire, release;

  // Boss service interface
  IDataManagerSvc	*m_pDataManager;
  IDataProviderSvc	*m_pDataProvider;
  IConversionSvc        *m_pConversionSvc;
  IDataStoreAgent 	*m_pAgent;
  DataStoreItem         *m_currentItem;
  RootInterface 	*m_pRootInterface;
  RootCnvSvc		*m_cnvSvc;

  typedef std::vector<DataStoreItem*> Items;
  typedef std::vector<std::string>    ItemNames;

   Items		m_itemList;		
   
   TDisTrack 	*recdis;
   TRecMdcTrack *recTrack1;
   TRecMucTrack *muctrk;
   TRecTofTrack *tofTrack;
   TRecMdcHit   *mdchit;
   TRecEmcShower *emcshower;

   int m_mode; //online mode:1 offline:0
  // private methods
  StatusCode producer(int p_sleep);
  bool hasRead();
  StatusCode hasWrite();
  void addItem(Items& itms, const std::string& descriptor);
  void clearItems(Items& items);
  DataStoreItem* findItem(const std::string& path);
  StatusCode collectObjects();
  StatusCode getSvc();
  StatusCode finishSvc();
  StatusCode write2file();
};


#endif

