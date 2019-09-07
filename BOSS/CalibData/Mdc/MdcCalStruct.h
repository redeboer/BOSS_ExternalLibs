#ifndef MDCCAL_STRUCT_H
#define MDCCAL_STRUCT_H

#define MDCCAL_LAYER 43
#define MDCCAL_CELLMAX 288
/* #define MDCCAL_TOTCELL 6796 */
#define MDCCAL_TOTCELL 6860
#define MDCCAL_LR 3           // left + lrcom + right 
#define MDCCAL_XTORDER 6      // 5 order polynomial
#define MDCCAL_XTPARNUMBER 8  // 5+1(edge) order
#define MDCCAL_TXORDER 7      // 6 order polynomial
#define MDCCAL_TXPARNUMBER 9  // 6+1(edge) order
#define MDCCAL_QTORDER 2     
#define MDCCAL_SPRESORDER 5   // 4 order polynomial

/*
typedef struct{
     double xtpar[MDCCAL_LAYER][MDCCAL_LR][MDCCAL_XTPARNUMBER];
     double txpar[MDCCAL_LAYER][MDCCAL_LR][MDCCAL_TXPARNUMBER];

     double t0[MDCCAL_TOTCELL];
     double delt0[MDCCAL_TOTCELL];

     double wshift[MDCCAL_TOTCELL];
     double delwshift[MDCCAL_TOTCELL];

     double qtpar[MDCCAL_LAYER][MDCCAL_QTORDER];

     // spcaial resolution
     double spres[MDCCAL_LAYER][MDCCAL_LR][MDCCAL_SPRESORDER];
}MdcCalConst_Struct;

typedef struct{
     // number of cells in each layer
     int cellnumber[MDCCAL_LAYER];
}MdcCalHist_Struct;

// typedef struct{ 
// }MdcCalTrack_Struct; 
*/
#endif /* MDCCAL_STRUCT_H */
