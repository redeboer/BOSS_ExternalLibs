//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegInfo.h,v 1.2 2009/12/16 09:02:47 zhangy Exp $
//
// Description:
//   Base class for various MdcSegInfo classes, which hold values 
//    for a track segment, calculated for some particular application
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Authors: Steve Schaffner
//
// Copyright (C)  1996  The Board of Trustees of  
// 
// History:
//	Migration for BESIII MDC
// The Leland Stanford Junior University.  All Rights Reserved.
//------------------------------------------------------------------------

// Interface Dependencies ----------------------------------------------

#ifndef MDCSEGINFO_H
#define MDCSEGINFO_H

//  End Interface Dependencies -----------------------------------------

class MdcSeg;
class MdcTrack;
class MdcSegWorks;

// Class definition//
class MdcSegInfo 
{
public:
  MdcSegInfo()                                           { };
  virtual ~MdcSegInfo();

  const double *errmat() const                           {return _errmat;}
  const double *inverr() const                           {return _inverr;}
  //  double par1() const                                    {return _par1;}
  //  double par0() const                                    {return _par0;}
  double par(int i) const                   {return (0 == i) ? _par0 : _par1;}
  double arc() const                                     {return _arc;}
  virtual bool parIsAngle(int i) const = 0;
  void plotSegInfo() const;
  
protected:
  double _par0;   // origin: phi0;  stereo: z0
  double _par1;   // origin: curv;  stereo: ct
  double _errmat[3]; //error matrix: sig**2(phi0), sig**2(phi0-curv), (curv)
  double _inverr[3];
  double _arc;  // arclength @ which calc. -- for stereo
};

#endif







