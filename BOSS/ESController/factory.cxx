//Dear emacs, this is -*- c++ -*-

/**
 * @file factory.cxx
 * @author <a href="dosil@ifae.es">Mireia Dosil</a> 
 * $Revision: 1.1.1.1 $
 * $Date: 2007/04/26 12:43:06 $
 *
 * Implements the factory
 */



#include "ESController/factory.h"
#include "ESController/SC.h"

efhlt::Interface* factory (void)
{
  return new efpsc::SC();
}


