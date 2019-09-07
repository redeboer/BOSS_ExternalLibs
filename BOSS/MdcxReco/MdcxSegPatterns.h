#ifndef MDCXSEGPATTERNS_H
#define MDCXSEGPATTERNS_H

class MdcxSegPatterns {
 public:
   MdcxSegPatterns() {}
   virtual ~MdcxSegPatterns() {}

   static const int patt4_size;
   static const unsigned patt4[14];

   static const int patt3_size;
   static const unsigned patt3[20];

   static const int ambPat4_size[14];
   static const int ambigPatt4[14][4];

   static const int ambPat3_size[20];
   static const int ambigPatt3[20][4];

   static const int wirePat4[14][4];
   static const int wirePat3[20][4];
};
#endif

