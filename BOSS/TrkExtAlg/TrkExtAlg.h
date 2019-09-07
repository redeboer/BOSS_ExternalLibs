//
// File: TrkExtAlg.h
// Author: Wang Liangliang
// Date: 2005.4.4
// Descreption: Algorithm for track extrapotation whose kernel bases on GEANT4.
//



#include "GaudiKernel/Algorithm.h"

//----test----
//#include "GaudiKernel/NTuple.h"
//----end test----

#include "TrkExtAlg/Ext_track.h"

using namespace std;

class TrkExtAlg:public Algorithm 
{
 public:
   TrkExtAlg (const std::string& name, ISvcLocator* pSvcLocator);
   ~TrkExtAlg();
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();
         
 private:

   Ext_track *myExtTrack;
  
//   ofstream *myFile;

   string myParticleName;
   string myInputTrk;
   
   bool msgFlag;//message on/off  true/false   
   bool myGeomOptimization;
   bool myResultFlag;
   bool myBFieldOn;
   int m_detVer;
   bool myUseMucKal;
   int  myMucWindow;
   bool m_setSeed;
/*   
   //----For Ext Test----
   NTuple::Tuple*  myNtuple;
   NTuple::Item<double> myCharge;
   NTuple::Item<double> myEmcHitFlag;
   NTuple::Item<double> myEmcHitTheta;
   NTuple::Item<double> myEmcHitPhi;
   NTuple::Item<double> myEmcVolNum;
   NTuple::Item<double> myEmcExtTheta;
   NTuple::Item<double> myEmcExtPhi;
   NTuple::Item<double> myDTheta;
   NTuple::Item<double> myDPhi;
   //----end test----
*/   
};            
