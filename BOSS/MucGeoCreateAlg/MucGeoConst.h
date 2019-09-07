//------------------------------------------------------------------------------|
//      [File  ]:                       MucGeoConst.h  	                        |
//      [Brief ]:       Head file of MUC geometry constants according to the    |
//			construction and production geometry	        	|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Nov 22, 2005                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_GEO_CONST_H
#define MUC_GEO_CONST_H

#include<iostream>
#include<cmath>

using namespace std;

//----------------------       Grobal    Constant     ---------------------------
static const double MUC_PI		= 3.141592653589793;	// for geometry calculation	
static const double BETA		= MUC_PI/8.0;	// structure angle
static const double VALUE 		= tan(BETA);  // for geometry calculation
static const double ERR_LIMIT		= 0.01;		// for precision cut 	

static const double MAX_FRACTION        = 0.8;          // if offset out range, set offset to MAX_FRACTION*MAX
static const double E_OFFSET_MAX[3]	= {45.0, 5.0, 5.0 }; // for endcap box x alignment offset checking 	
static const double B_X_MAX[3] 		= { 8.0, 	// Layer 0
				    	    7.0, 	// Layer 1,3,5,7
				    	    6.0  	// Layer 2,4,6,8 
				 	  };    	// for barrel box x alignment offset checking
static const double B_Y_MAX             = 5.0;          // for box y alignment offset checking
static const double B_Z_MAX             = 5.0;          // for box z alignment offset checking

static const double STR_OFFSET_MAX[3] 	= {5.0, 5.0, 2.0}; // for strip plane alignment, 1: X_MAX, 2: Y_MAX, 3: Z_MAX
 

//----------------------      Structure Constant      ----------------------------
// Globle structure
static const int PART_MAX	      = 3; // part number, 0-east endcap, 1-berral, 2-west endcap
static const int BOX_MAX        = 136; // maximum of box number
static const int STRIP_MAX      = 9152;
static const int STRIP_INBOX_MAX= 112;

static const int EEID           = 0; // east endcap id
static const int BRID           = 1; // berral      id
static const int EWID		= 2; // west endcap id
static const int SL_NUM         = 2; // rpc super layer number in one box(module)
static const int SL_UP          = 0; // up   super layer id
static const int SL_DOWN        = 1; // down super layer id
static const int ROTA_FLAG      = 1; // need rotation
static const int XSTR		= 0; // strip read out from x direction
static const int YSTR		= 1; // strip read out from y direction
static const int ZSTR		= 2; // strip read out from z direction 
static const int PHISTR	      	= 3; // strip read out from phi direction 
static const int BKLT_NUM	= 2; // bakelite number of each rpc chamber
static const double BOX_TH      = 28.0; // aluminum box thickness
static const double RPC_TH      = 6.0;  // rpc chamber thickness
static const double GAS_TH	= 2.0;	// gas mixture thickness
static const double BKLT_TH	= 2.0;	// bakelite thickness
static const double BOX_COVER_TH = 1.5; // box cover thickness
static const double STR_TH      = 1.0;  // strip thickness
static const double STR_GAP	= 2.0;  // the gap between two strips
static const double AS_GAP	= 40.0; // the gap between two absorber	
static const double RPC_SL_OFFSET    = 5.0;  // offsets of UP RPCs relative to DOWN RPCs 		
//static const double RPC_DEATH_EDGE   = 10.0; // the death edge of rpc chamber
static const double RPC_DEATH_EDGE   = 22.0; // the death edge of rpc chamber
static const double OVERLAP_WIDTH    = 4.0; // avoid space between panels when apply "AND" operation 

static const int BOX_PER_PART[PART_MAX] = {32, 72, 32};
static const int BOX_PER_SEG[PART_MAX]  = {8, 9, 8};
static const int BOX_SUM[PART_MAX]      = {32, 104, 136};
static const int STR_PER_PART[3]  = { 2048, 5056, 2048 };
static const int B_STR_PER_SEG[2] = { 624, 688 }; 
static const int STR_SUM[3]       = { 2048, 7104, 9152 };

// Endcap( only one part, not total, east and  west is the same) 
static const int E_SEG_NUM      = 4;  // segment number
static const int E_LAY_NUM      = 8;  // layer number ( gap layer number )
static const int E_ASLAY_NUM    = 9;  // absorber layer number
static const int E_PANEL_NUM    = 3;  // absorber layer number
//static const int E_AS_NUM    	= 3;  // absorber number per segment per layer
//static const int E_GP_NUM     = 3;  // gap number per segment per laye
static const int E_STR_NUM      = 64; // strip number persegment per layer 
static const double E_STR_DST 	= 35; // distance between two strip center line  
static const int E_RPC_NUM[SL_NUM]	= {3,5};  // RPC number per segment per layer, up super layer is 3, down is 5 

static const double E_XSTR_OFFSET     	 = 97.5; // the offset of first x-readout strip from x axis
static const double E_YSTR_OFFSET     	 = 87.5; // the offset of fisrst y-readout strip from y axis
static const double E_STR_WT          	 = E_STR_DST - STR_GAP; // Endcap strip width

static const double E_AS_ZMAX	         = 2800.0; // maximum Z of endcap absorber coordinates
static const double E_AS_RMAX            = 2500.0; // maximal radius of absorber incircle
static const double E_AS_TOTAL_TH        = 750; // total length in z direction of absorber
static const double E_AS_TH[E_ASLAY_NUM] = {40,40,30,30,30,50,80,80,50}; // absorber thickness
static const double E_AS_RMIN[E_ASLAY_NUM] = {1034,1067,1100,1133,1167,1203,1241,1302,1302}; // minimal radius of absorber incircle 

static const double E_GP_RMIN[E_LAY_NUM] = {1067,1100,1133,1167,1203,1241,1302,1362}; // minimal radius of gap incircle
static const double E_GP_DX              = 40.0;  // X offset of gap from X axis 
static const double E_GP_DY              = 50.0;  // Y offset of gap from Y axis
static const double E_BOX_DR             = 130.0; // radius offset of box from E_AS_RMAX  

static const double E_STRPLN_DA		= 16.0;			// offset of strip plane from box edge close to X or Y axises
static const double E_STRPLN_DB[2]	= { 53.0,41.0 }; 	// offsets of strip plane from box edge far away X or Y axises
static const double E_STRPLN_DR	 	= 26.0;			// offsets of strip plane from box incircle edge

// Barrel
static const int B_SEG_NUM       = 8; // segment number
static const int B_LAY_NUM       = 9; // layer   number
static const int B_AS_NUM	 = 1; // absorber number per segment per layer
static const int B_GP_NUM        = 1; // gap number per segment per layer
static const int B_STR_PANEL_NUM = 3; // maximum number of stripp_lane panel per segment per layer
static const int B_RPC_NUM       = 4; // RPC number per segment per layer, except segment NO.2(TOP)
static const int B_TOP_RPC_NUM   = 5; // RPC number in TOP segment per layer
static const int B_TOP           = 2; // top segment id

static const int B_ZSTR_NUM            	   = 48; // strip number in Z-readout box 
static const int B_PHISTR_NUM              = 96; // strip number in Phi-readout(Tangent) box
static const int B_TOPSTR_NUM         	   = 112; // strip number in Phi-readout(Tangent) box of TOP segment
static const int B_ZSTR_CUT_NUM[B_LAY_NUM] = {16, 0, 12, 0, 11, 0, 10, 0, 9}; // number of strips be cut in z direction
static const int B_PHISTR_CUT_NUM          = 80; // number of strips be cut in phi direction
static const double B_ZSTR_CUT[2]          = {540,610}; // dimensions of z strips be cut 
static const double B_PHISTR_CUT           = 642; // dimension of phi strips be cut
static const double B_STR_DST[B_LAY_NUM]   = {20,39,27,39,30,39,33,39,37}; // distance between two strips center line
static const double B_PHI[B_SEG_NUM] 	   = { -2, -1, 0, 1, 2, -1, 0, 1 }; // rotation angle on Z axis, times MUC_PI/4
static const double B_AS_RMAX          	   = 2620.0; // maximal radius of absorber incircle
static const double B_AS_LMAX              = 3940.0; // maximal length of absorber 
static const double B_AS_RMIN[B_LAY_NUM]   = {1740,1810,1880,1950,2030,2110,2230,2350,2470}; // minimal radius of absorber incircle
static const double B_AS_WT[B_LAY_NUM]	   = {1293.3,1351.3,1409.3,1467.3,1533.3,1599.3,1699.3,1797.3,1919.3};
static const double B_AS_TH[B_LAY_NUM]     = {30,30,30,40,40,80,80,80,150}; // absorber thickness

static const double B_GP_DZ             = 70.0; // offset of gap from absorber edge
static const double B_GP_LT             = 3940.0; // gap length
// static const double B_GP_WT[B_LAY_NUM]  = {1263.3,1311.3,1369.3,1427.3,1493.3,1559.3,1649.3,1759.3,1857.3}; // gap width
static const double B_GP_WT[B_LAY_NUM]  = {1266,1314,1372,1430,1496,1562,1652,1762,1860}; // gap width
static const double B_BOX_LT            = B_GP_LT - 2*B_GP_DZ; // 3800.0
static const double B_BOX_WT[B_LAY_NUM] = {1050,1300,1360,1416,1484,1548,1640,1748,1848}; // box width
static const double B_BOX_DZ[SL_NUM]  	= {46,41}; // gap of chamber and box in Z direction
static const double B_BOX_DT[SL_NUM]  	= {31,26}; // gap of chamber and box in Phi direction
static const double B_BOX_SLOT_WT       = 560; // width of box slot be cut in segment 2 of east-end barrel
static const double B_GP_SLOT_WT	= 422; // width of gap slot be cut in segment 2 of east-end barrel
static const double B_GP_SLOT_LT	= 542; // length of gap slot be cut in segment 2 of east-end barrel

static const double B_RPC_LTN           = 1112; // normal rpc length except B_TOP
static const double B_RPC_LTS           = 1050; // special rpc length except B_TOP
static const double B_TOPRPC_LTN        = 1112; // nomal length of RPC in B_TOP 
static const double B_TOPRPC_LTS[2]     = {540,610}; // special length of RPC in B_TOP


#endif
