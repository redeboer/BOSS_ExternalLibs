
#ifndef MUC_REC_ROAD_FINDER_PARAMETER_H_
#define MUC_REC_ROAD_FINDER_PARAMETER_H_

#include "Identifier/MucID.h"

const int kNSeedLoops = 3;

//Number of gaps to search in each orient.
const int kSearchLength[kNSeedLoops][3][2] =
{ { {4,4}, {4,5}, {4,4} },
  { {4,4}, {4,5}, {4,4} },
  { {4,4}, {4,5}, {4,4} }
};     
//Searching order of gaps in each orient[iLoop][iPart][iOrient][iGap]
const int kSearchOrder[kNSeedLoops][3][2][5] = 
{ { { {0,4,2,6,0}, {1,5,3,7,1} },
    { {1,3,5,7,1}, {0,2,4,6,8} },
    { {0,4,2,6,0}, {1,5,3,7,1} } },
  { { {2,4,0,6,2}, {3,5,1,7,3} },
    { {3,5,1,7,3}, {2,4,0,6,8} },
    { {2,4,0,6,2}, {3,5,1,7,3} } },
  { { {0,2,4,6,0}, {1,3,5,7,1} },
    { {1,5,3,7,1}, {0,4,2,6,8} },
    { {0,2,4,6,0}, {1,3,5,7,1} } }, 
};

//const int kSearchOrder[kNSeedLoops][3][2][5] =
//{ { { {0,4,2,6,1}, {1,5,3,7,0} },
//  { {1,5,3,7,0}, {0,4,2,6,1} },
//  { {0,4,2,6,1}, {1,5,3,7,0} } },
//{ { {2,4,0,6,1}, {3,5,1,7,0} },
//  { {3,5,1,7,0}, {2,4,0,6,1} },
//  { {2,4,0,6,1}, {3,5,1,7,0} } } };



//  const float  WeightPar2D[5] = {0.0,0.0,0.0,0.0,0.0};
//  const float  WindowParAlpha[5] = {0.0,0.0,0.0,0.0,0.0};
//  const float  WindowParBeta[5] = {0.0,0.0,0.0,0.0,0.0};
//  const float  WindowParGamma[5] = {10.0,10.0,10.0,10.0,10.0};

// Parameters for 2D road quality check
const int kMinLastGap2D = 2;
const int kMinFiredGaps = 2;
const int kMaxSkippedGaps = 1;
const int kMinSharedHits2D = 2;

// Parameters for 3D road quality check
const int kMinLastGap3D = 3;          // Last gap a 3D road should arrive at least
const int kMaxDeltaLastGap = 5;       // Max tolerance of the difference between the last gap of the two 2D roads
//const int maxDelHitsPerGap = 1;
const int kMaxDeltaTotalHits = 10;    // Max tolerance of the difference between total hits of the two 2D roads
//const float WeightPar3D[5]   = {0.0,0.0,0.0,0.0,0.0};
const float kMaxXChisq = 1000.0;
const float kMaxYChisq = 1000.0;
const int   kMinSharedHits3D = 4;
//const int   kmvdvertex  = 0;
//const float MutrWindow = 20;
//const float MuidWindow = 30;
//const float MutrZNorth = 620;
//const float MutrZSouth = -470;  

const float kRpcEfficiency = 1.0;
const int   kPartSeq[3] = {1, 0, 2}; // search on barrel first.
const float kWindowSize[3][9] = { {200.0, 200.0, 200.0, 240.0, 240.0, 280.0, 280.0, 280.0, 280.0},
				  {200.0, 200.0, 120.0, 120.0, 120.0, 240.0, 240.0, 240.0, 240.0},
				  {200.0, 200.0, 200.0, 240.0, 240.0, 280.0, 280.0, 280.0, 280.0} };
const int kNSegSearch = 3;
const int kDeltaSeg[kNSegSearch] = {0, -1, 1};


#endif // MUC_REC_ROAD_FINDER_PARAMETER_H_
