#include "EsTimeAlg/EstParameter.h"

EstParameter::EstParameter()
{

  //velocity in scintillator
      _vscint=17.6;

  //drift velocity of electron
      _vdrift=4.0e-3;


  // optCosmic = 1: employ cosmic correction, 0: don't

//      _optCosmic=0;

  // cosmicScheme to decide incoming and outgoing legs
  //              = 0: 2hits+TDCdiff  1: 2hits+Phi
  //                2: All hits+Phi 
 //     _cosmicScheme=0;

  // SkipMC = 1: Skip timing determination for MC  0: Don't
      _SkipMC=1;

  //Skip =1 : Skip timing determination for MC  0: Don't
      _Skip=1;

// ptCut,drCut,dzCut: fzisan track param cuts
      _ptCut=0.0;
      _drCut=5.0;
      _dzCut=50.0;
 
 // Cuts on path length and tof z-position on 
  // fzisan tracks extrapolated to TOF counters

      _pathlCut=500.0;
      _ztofCutmin=-140.0;
      _ztofCutmax=140.0;

// TOF time internal offset
      _timeOfs=0.0;

// Skip = 1: Skip CDCtime if TOFtime is available  0: Don't
      _MDC_Skip=1;

      _MDC_Inner=0;
 // ptCut,drCut,dzCut: fzisan track param cuts
      _MDC_drCut=10.0;
      _MDC_dzCut=50.0;
      //Tof corr. (0:off;1:on)
      _MDC_Tof=1;

      //Propagation delay corr. (0:off;1:on)
      _MDC_Prop=1;

      //Time walk corr. (0:off;1:on for data)
      _MDC_Walk=1;

      //debug (0:off;1:on)
      _MDC_Debug=1;

      //xt (1:linear x-t; 2:non-linear x-t)
      _MDC_Xt=1;

      //inner 4 layers(0:not use; 1:use)
      _MDC_Inner=0;
}

EstParameter::~EstParameter(){
}

double EstParameter::vscint() const
{
  return _vscint;
}

double EstParameter::vdrift() const
{
  return _vdrift;
}

int EstParameter::SkipMC() const
{
  return _SkipMC;
}

int EstParameter::Skip() const
{
  return _Skip;
}

double EstParameter::ptCut() const
{
  return _ptCut;
}

double EstParameter::drCut() const
{
  return _drCut;
}

double EstParameter::dzCut() const
{
  return _dzCut;
}

double EstParameter::pathlCut() const
{
  return _pathlCut;
}

double EstParameter::ztofCutmin() const
{
  return _ztofCutmin;
}

double EstParameter::ztofCutmax() const
{
  return _ztofCutmax;
}

double EstParameter::timeOfs() const
{
  return _timeOfs;
}

double EstParameter::MDC_Skip() const
{
  return _MDC_Skip;
}

double EstParameter::MDC_Inner() const
{
  return _MDC_Inner;
}

double EstParameter::MDC_drCut() const
{
  return _MDC_drCut;
}

double EstParameter::MDC_dzCut() const
{
  return _MDC_dzCut;
}

double EstParameter::MDC_Tof() const
{
  return _MDC_Tof;
}

double EstParameter::MDC_Prop() const
{
  return _MDC_Prop;
}

double EstParameter::MDC_Walk() const
{
  return _MDC_Walk;
}

int  EstParameter::MDC_Debug() const
{
  return _MDC_Debug;
} 

double EstParameter::MDC_Xt() const
{
  return _MDC_Xt;
}


