//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkTimeAdjuster.h,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// Description:
//      Class TrkTimeAdjuster
//     
//      base class which can be just to 'transform' raw times from
//      one subdetector to another...
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Gerhard Raven
//
// Copyright Information:
//      Copyright (C) 1999              UC, San Diego
//
//------------------------------------------------------------------------

#ifndef TRKTIMEADJUSTER_H
#define TRKTIMEADJUSTER_H


class TrkTimeAdjuster 
{
public:
        virtual ~TrkTimeAdjuster();

        // subtract this # of seconds from the 'raw' (digi) time 
        // to get the 'BaBar' time; This corrected time can be 
        // compared directly to the bunch T0.
        virtual double deltaTimePep() const =0;
        virtual double deltaTimeMdc() const =0;
        virtual double deltaTimeDrc() const =0;
        virtual double deltaTimeEmc() const =0;
        virtual double deltaTimeIfr() const =0;

// set functions

        virtual void setDeltaTimePep(double delta) =0;
        virtual void setDeltaTimeMdc(double delta) =0;
        virtual void setDeltaTimeDrc(double delta) =0;
        virtual void setDeltaTimeEmc(double delta) =0;
        virtual void setDeltaTimeIfr(double delta) =0;

};

#endif
