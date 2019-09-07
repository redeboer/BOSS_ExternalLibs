#ifndef MDCCELLADDR_H
#define MDCCELLADDR_H

//--------------------------------------------------------------------------
//
// Environment:
//      This software was developed for the BaBar collaboration.  If you
//      use all or part of it, please give an appropriate acknowledgement.
//
// Copyright Information:
//      Copyright (C) 1999      <INFN>
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

//		---------------------
// 		-- Class Interface --
//		---------------------


/**
 *  C++ source file code MdcCellAddr. 
 *  This class is only a container for three static functions to map a Mdc 
 *  cell address to its layer/wire numbers. Needed to break some circular
 *  dependency within the Mdc code
 *
 *  This software was developed for the BaBar collaboration.  If you
 *  use all or part of it, please give an appropriate acknowledgement.
 *
 *  Copyright (C) 1999 [INFN & Padova University]
// 
// History:
//	Migration for BESIII MDC
 *
 *  @see MdcCellAddrMdcCellAddr
 *
 *  @version $Id: MdcCellAddr.h,v 1.2 2009/12/17 00:38:40 zhangy Exp $ 
 *
 *  @author (R. Stroili)	(originator);
 *  
 */

class MdcCellAddr {

//--------------------
// Instance Members --
//--------------------

public:

  // Constructors
  MdcCellAddr( void );

  // Destructor
  virtual ~MdcCellAddr( ) {;}

  // Operators
    
//------------------
// Static Members --
//------------------

public:

  // Selectors (const)
  static int wireIs(const int &cell)  { return cell%1000; }
  static int layerIs(const int &cell) { return cell/1000; }
  static int cellIs(const int &wire, const int &layer) { return 
							   layer*1000+wire; }

};

#endif // MDCCELLADDR_H
