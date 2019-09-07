//------------------------------------------------------------------------------|
//      [File  ]:                       MucCalibConst.h                         |
//      [Brief ]:       Head file of MUC structure constants for calibration    |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Aug 22, 2006                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_STRUCT_CONST_H
#define MUC_STRUCT_CONST_H

//#include<iostream>

//using namespace std;

static const double DEFAULT_EFF   = 0.95;
static const double DEFAULT_NOS   = 0.01;
static const double DEFAULT_CLST  = 4;

static const double LIMIT_CUT     = 0.000001;

static const int    CLST_CUT      = 1;
static const int    CLST_MAX      = 20;

static const double DEFAULT_MHIT_PRO[MULTI_HIT_MAX]  = {0.9, 0.95, 0.98, 0.99, 1.0};

// Globle structure
static const int PART_MAX         = 3;
static const int SEGMENT_MAX      = 8;
static const int LAYER_MAX        = 9;
static const int BOX_MAX          = 136;
static const int STRIP_MAX        = 9152;
static const int STRIP_INBOX_MAX  = 112;

static const int BOX_PER_PART[PART_MAX] = {32, 72, 32};	
static const int BOX_PER_SEG[PART_MAX]  = {8, 9, 8};
static const int BOX_SUM[PART_MAX]      = {32, 104, 136};

static const int STR_PER_PART[3]  = { 2048, 5056, 2048 };
static const int B_STR_PER_SEG[2] = { 624, 688 };
static const int STR_SUM[3]       = { 2048, 7104, 9152 };

static const int EEID   = 0; // east endcap id
static const int BRID   = 1; // berral      id
static const int EWID   = 2; // west endcap id
static const int XSTR   = 0; // strip read out from x direction
static const int YSTR   = 1; // strip read out from y direction
static const int ZSTR   = 2; // strip read out from z direction 
static const int PHISTR = 3; // strip read out from phi direction 

// Endcap( only one part, not total, east and  west is the same) 
static const int E_SEG_NUM  = 4;  // segment number
static const int E_LAY_NUM  = 8;  // layer number ( gap layer number )
static const int E_STR_NUM  = 64; // strip number persegment per layer 

// Barrel
static const int B_SEG_NUM  = 8; // segment number
static const int B_LAY_NUM  = 9; // layer   number
static const int B_TOP      = 2; // top segment id

static const int B_ZSTR_NUM     = 48;   // strip number in Z-readout box 
static const int B_PHISTR_NUM   = 96;   // strip number in Phi-readout(Tangent) box
static const int B_TOPSTR_NUM   = 112;  // strip number in Phi-readout(Tangent) box of TOP segment

#endif

//END
