//------------------------------------------------------------------------------|
//      [File  ]:                       MucStructConst.h                        |
//      [Brief ]:       Head file of MUC structure constants for calibration    |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Aug 22, 2006                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_STRUCT_CONST_H
#define MUC_STRUCT_CONST_H

#include<iostream>
#include<cmath>

using namespace std;

static const int    CALIB_LV_MAX	= 4; // 0-layer, 1-box, 2-strip, 3-pad
static const int    EFF_WINDOW		= 4; // [-EFF_WINDOW,...,-2,-1,expected strip,+1,+2,,...,+EFF_WINDOW ] 
static const int    TRACK_SEG_MAX	= 8; // segment max of a track in MUC 
static const int    TRACK_THRESHOLD	= 100; // down limit of track number for good eff calibrated
static const int    DEFAULT_BUILD_MODE  = 1;  // default cluster building method, optimized, fatest 
static const double DEFAULT_EFF_VALUE  	= 0.95;
static const double DEFAULT_EFF_ERR     = 0.0;
static const double DEFAULT_INC_VALUE   = 0.0;
static const double DEFAULT_NOS_VALUE  	= 0.0;
static const double LIMIT_CUT          	= 0.0001; // For double variables compare
static const double TRIGGER_RATE       	= 4000.0; // Hz
static const double TRIGGER_WINDOW     	= 800e-9; // s

static const double PI          = 3.141593;		
static const double VALUE       = tan(PI/8.0);  // for geometry calculation

// Globle structure
static const int PART_MAX       = 3; // part number, 0-east endcap, 1-berral, 2-west endcap
static const int SEGMENT_MAX    = 8;
static const int LAYER_MAX      = 9;
static const int BOX_MAX        = 136;
static const int STRIP_MAX      = 9152;
static const int STRIP_INBOX_MAX= 112;
static const int SL_NUM         = 2;
static const int CLUSTER_CUT    = 0; // if zero, including cluster ==1( used: cluster > CLUSTER_CUT );
static const int CLUSTER_ALARM	= 20; // a cluster with size == 20 regarded as huge
static const int CLUSTER_RANGE  = STRIP_INBOX_MAX; // on the worst situation, generally < CLUSTER_ALARM;

static const int BOX_PER_PART[PART_MAX] = {32, 72, 32};
static const int BOX_PER_SEG[PART_MAX]  = {8, 9, 8};
static const int BOX_SUM[PART_MAX]      = {32, 104, 136};

static const int STR_PER_PART[3]  = { 2048, 5056, 2048 };
static const int B_STR_PER_SEG[2] = { 624, 688 };
static const int STR_SUM[3]       = { 2048, 7104, 9152 };

static const int EEID           = 0; // east endcap id
static const int BRID           = 1; // berral      id
static const int EWID           = 2; // west endcap id
static const int SL_UP          = 0; // up   super layer id
static const int SL_DOWN        = 1; // down super layer id
static const int XSTR           = 0; // strip read out from x direction
static const int YSTR           = 1; // strip read out from y direction
static const int ZSTR           = 2; // strip read out from z direction
static const int PHISTR         = 3; // strip read out from phi direction

static const double STR_GAP     = 2.0;  // the gap between two strips
static const double STR_TH      = 1.0;  // strip thickness
static const double BOX_TH      = 28.0; // aluminum box thickness
static const double AS_GAP      = 40.0; // the gap between two absorber

// Endcap( only for one part, not total, east and west is the same)
static const int E_SEG_NUM      = 4;  // segment number
static const int E_LAY_NUM      = 8;  // layer number ( gap layer number )
static const int E_STR_NUM      = 64; // strip number persegment per layer

static const int E_ASLAY_NUM    = 9;  // absorber layer number
static const int E_PANEL_NUM    = 3;  // absorber layer number
static const double E_STR_DST   = 35; // distance between two strip center line
static const int E_RPC_NUM[SL_NUM]       = {3,5};  // RPC number per segment per layer, up super layer is 3, down is 5

static const double E_STR_WT             = 33;      // 35-2, Endcap strip width
static const double E_AS_RMAX            = 2500.0;  // maximal radius of absorber incircle

static const double E_GP_RMIN[E_LAY_NUM] = {1067,1100,1133,1167,1203,1241,1302,1362}; // minimal radius of gap incircle
static const double E_GP_DX              = 40.0;  // X offset of gap from X axis
static const double E_GP_DY              = 50.0;  // Y offset of gap from Y axis
static const double E_BOX_DR             = 130.0; // radius offset of box from E_AS_RMAX

static const double E_XSTR_OFFSET        = 97.5;  // the offset of first x-readout strip from x axis
static const double E_YSTR_OFFSET        = 87.5;  // the offset of fisrst y-readout strip from y axis
static const double E_STRPLN_DA          = 16.0;  // offset of strip plane from box edge close to X or Y axises
static const double E_STRPLN_DB[2]       = { 53.0,41.0 }; // offsets of strip plane from box edge far away X or Y axises
static const double E_STRPLN_DR          = 26.0;          // offsets of strip plane from box incircle edge

// Barrel
static const int B_SEG_NUM      = 8; // segment number
static const int B_LAY_NUM      = 9; // layer   number
static const int B_TOP          = 2; // top segment id

static const int B_ZSTR_NUM     = 48;   // strip number in Z-readout box
static const int B_PHISTR_NUM   = 96;   // strip number in Phi-readout(Tangent) box
static const int B_TOPSTR_NUM   = 112;  // strip number in Phi-readout(Tangent) box of TOP segment

static const int B_PHISTR_CUT_NUM          = 80;        // number of strips be cut in phi direction
static const double B_PHISTR_CUT           = 642;       // dimension of phi strips be cut
static const double B_ZSTR_CUT[2]          = {540,610}; // dimensions of z strips be cut
static const double B_STR_DST[B_LAY_NUM]   = {20,39,27,39,30,39,33,39,37};      // distance between two strips center line
static const double B_PHI[B_SEG_NUM]       = { -2, -1, 0, 1, 2, -1, 0, 1 };     // rotation angle on Z axis, times PI/4
static const int B_ZSTR_CUT_NUM[B_LAY_NUM] = {16, 0, 12, 0, 11, 0, 10, 0, 9};   // number of strips be cut in z direction

static const double B_AS_LMAX              = 3940.0; // maximal length of absorber 
static const double B_GP_DZ             = 70.0;     // offset of gap from the east or west edge of absorber
static const double B_GP_LT             = 3940.0;   // gap length
static const double B_GP_WT[B_LAY_NUM]  = {1266,1314,1372,1430,1496,1562,1652,1762,1860}; // gap width
static const double B_GP_SLOT_WT        = 422; // width of gap slot be cut in segment 2 of east-end barrel
static const double B_GP_SLOT_LT        = 542; // length of gap slot be cut in segment 2 of east-end barrel
static const double B_BOX_LT            = 3800.0;   // B_GP_LT - 2*B_GP_DZ; 
static const double B_BOX_SLOT_WT       = 560;      // width of slot be cut in segment 2 in east-end barrel
static const double B_BOX_DT[SL_NUM]    = {31,26};  // gap of chamber and box in Phi direction
static const double B_BOX_DZ[SL_NUM]    = {46,41};  // gap of chamber and box in Z direction
static const double B_BOX_WT[B_LAY_NUM] = {1050,1300,1360,1416,1484,1548,1640,1748,1848}; // box width
static const double B_AS_RMIN[B_LAY_NUM]= {1740,1810,1880,1950,2030,2110,2230,2350,2470}; // minimal r of absorber 

static const double B_TOPRPC_LTS[2]     = {540,610}; // special length of RPC in B_TOP

static const double OVERLAP_WIDTH       = 4.0;  // avoid space between panels when apply "AND" operation 

#endif
