#ifndef MCPARTICLE_BUILDER_H
#define MCPARTICLE_BUILDER_H

#include "RawDataCnv/EventManagement/Builder.h"
#include "McTruth/McParticle.h" 

using namespace std;
using Event::McParticle;
using Event::McParticleCol;

class McParticleBuilder: public Builder
{                                           

public:
   McParticleBuilder ();
   virtual ~McParticleBuilder(){};
   virtual StatusCode  initialize(string &initFile);
   virtual uint32_t getTEID(uint32_t teid);
   virtual uint32_t getREID(uint32_t reid);

   virtual void unPack(uint32_t* buf, double* vPointer, McParticle* mcPar);

   virtual StatusCode pack(McParticleCol* mcParticleCol, WriteRawEvent*& re);

private:

};
#endif
