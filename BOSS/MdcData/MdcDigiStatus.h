//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Package:
//	MdcData
//      $Id: MdcDigiStatus.h,v 1.1.1.1 2005/04/21 06:04:41 zhangy Exp $
//
//  Abstract:
//      Defines the online status flags 
//      
//
//  Author:
//      R. Stroili       INFN & Padova University
//
//  Creation Date:
//	November 20, 2000
//
// ----------------------------------------------------------------------------
//
#ifndef MDCDIGISTATUS_H
#define MDCDIGISTATUS_H

class MdcDigiStatus {
public:
  enum { badWaveform = 0x200,
	 badPedestal = 0x400,
	 clippedWaveform = 0x800,
	 rawDataAppended = 0x1000,
         unstablePedestal = 0x2000,
         chargeCorrected = 0x4000 };

  enum { allFlags = 0x7E00,
	 flagShift = 9 };

  ~MdcDigiStatus(){;}		// Public to allow usage of class namespace

private:
  // Make constructor private to prevent instantiation
  MdcDigiStatus(){;}
};

#endif  /* MDCDIGISTATUS_H */
