//
//Parameters for Event start time determination
//
//Create by Ma Xiang
//

#ifndef EstParameter_FLAG_
#define EstParameter_FLAG_

#include<iostream>

using namespace std;

class EstParameter{

 public:
  //Constructor and destructor
 
  EstParameter();
  ~EstParameter();
   
   double   vscint()        const;
   double   vdrift()        const;
 //  int     optCosmic()     const;
 //  int     cosmicScheme()  const;
   int     SkipMC()      const;
   int     Skip()        const;
   double   ptCut()       const ;  
   double   drCut()       const ;  
   double   dzCut()       const ;  
   double   pathlCut()    const ;  
   double   ztofCutmin()  const ;
   double   ztofCutmax()  const ;
   double   timeOfs()     const ;  
   double   MDC_Skip()    const ; 
   double   MDC_Inner()   const ;
   double   MDC_drCut()   const ;
   double   MDC_dzCut()   const ;
   double   MDC_Tof()     const ;  
   double   MDC_Prop()    const ; 
   double   MDC_Walk()    const ; 
   double   MDC_Xt()      const ; 
   int     MDC_Debug()   const ;
  
 private:
   double _vscint;
   double _vdrift;   
   //int   _optCosmic, _cosmicScheme; 
   int   _SkipMC,_Skip; 
   double _ptCut, _drCut, _dzCut;   
   double _pathlCut, _ztofCutmin, _ztofCutmax; 
   double _timeOfs; 
   int   _MDC_Skip; 
   double _MDC_ptCut, _MDC_drCut, _MDC_dzCut; 
   int   _MDC_Tof, _MDC_Prop, _MDC_Walk;  
   int   _MDC_Debug;
   int   _MDC_Xt, _MDC_Inner;
   //unsigned int _TOF_TimeChannel; 
   //double       __Trigger_cycle_coarse; 


};

#endif  /*EstParameter_FLAG_*/
