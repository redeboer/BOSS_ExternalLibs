// Count the number of bits set in a word; return that number.
// Use a small lookup table. Size of lookup table is tradeoff between 
// # of iterations needed and cache space used. 

inline int countbits(unsigned word) {
   const int nbit[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };
   int nset=0;
   for (unsigned i=0;i<2*sizeof(unsigned);i++) { // assume char = 8 bits, i.e. two
       nset+=nbit[word&0xf];                // iterations per 'char' size object
       word>>=4;
   }
   return nset;
}
