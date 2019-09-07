
#ifndef MUC_REC_TRK_EXT_PARAMETER_H_
#define MUC_REC_TRK_EXT_PARAMETER_H_

const float kRpcEfficiency = 1.0;
const int   kPartSeq[3] = {1, 0, 2}; // search on barrel first.
const float kWindowSize[3][9] = { {240.0, 240.0, 240.0, 280.0, 280.0, 320.0, 320.0, 320.0, 320.0},
				  {240.0, 240.0, 240.0, 280.0, 280.0, 320.0, 320.0, 320.0, 320.0},
				  {240.0, 240.0, 240.0, 280.0, 280.0, 320.0, 320.0, 320.0, 320.0} };

// For the last 2 gaps of firts mom range, the statics is too low to retrieve sigma. 
const float kWindowSize_Mom_Gap[4][9] = {
					  {604.8, 714.3, 406.7, 377.4, 329.4, 361.5, 272.6, 320.0, 320.0},
					  {397.3, 444.8, 253.0, 269.2, 231.3, 304.1, 262.2, 338.9, 266.0},
					  {316.1, 362.1, 210.3, 214.3, 205.7, 245.2, 234.8, 288.7, 223.9},
					  {260.8, 302.8, 173.3, 184.1, 187.0, 225.1, 201.7, 268.9, 198.4}
					};

const int kNSegSearch = 3;
const int kDeltaSeg[kNSegSearch] = {0,1,-1}; // consider magnet field, 1 first, then -1
const float kFirstHitWindowRatio = 1.0;  //change 3.0 to 1.0

#endif // MUC_REC_TRK_EXT_PARAMETER_H_
