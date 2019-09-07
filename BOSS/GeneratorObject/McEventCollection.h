#ifndef GENERATOROBJECTSMCEVENTCOLLECTION_H
#define GENERATOROBJECTSMCEVENTCOLLECTION_H 1


// --------------------------------------------------
//
// File:  GeneratorObject/McEventCollection.h
// Description:
//   This defines the McEventCollection, which is really juat an 
//   ObjectVector of McEvent objects.
//
// AuthorList:
//         M. Shapiro:  Initial Code March 2000
//         C. Leggett:  added maps, inherits from ObjectVector 4/25/01
//         fixed for gcc Oct 26 2001: Ian Hinchliffe


// #include "GeneratorObject/McEvent.h"
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "CLHEP/HepMC/GenEvent.h"
#include "CLHEP/HepMC/GenParticle.h"
#include "CLHEP/HepMC/GenVertex.h"

class McEventCollection : public DataVector<HepMC::GenEvent>
{
 public:
  McEventCollection (void);
  ~McEventCollection (void);
};

inline
McEventCollection::McEventCollection (void)
  :     DataVector<HepMC::GenEvent>       ()
{}

inline
McEventCollection::~McEventCollection (void)
{}

CLASS_DEF(McEventCollection, 133273, 1)

#endif
