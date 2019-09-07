#ifndef DCHAIN_CHILDREN_H
#define DCHAIN_CHILDREN_H

//  File:	Children.h
//  Author: 	Simon Patton
//  Package:	Taxi - a prototype set of objects for Physics analysis
//  Contents:	Declarations of the members of `Children' class.
//
//  Class Description: This simple acts as a namespace for an enum
//		for accessing a Candidate's Children.
//
//
// $Id: children.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//

namespace dchain {
   namespace children
   {
      enum Position { First = 0 ,
		      Second ,
		      Third ,
		      Fourth ,
		      Fifth
      } ;
   }
}
#endif // DCHAIN_CHILDREN_H
