//Dear emacs, this is -*- c++ -*-

/**
 * @file factory.h
 * @author <a href="mailto:dosil@ifae.es">Mireia Dosil</a> 
 * $Revision: 1.1.1.1 $
 * $Date: 2007/04/26 12:43:06 $
 *
 * @brief Creates an Interface object of the concrete type efpsc::Psc
 */

#ifndef ESC_FACTORY_H
#define ESC_FACTORY_H

#include "efhlt/Interface.h"

extern "C" {
efhlt::Interface* factory (void);
}

#endif /* ESC_FACTORY_H */
