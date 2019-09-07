//------------------------------------------------------------------------------|
//      [File  ]:                       MucGeoConst.h                           |
//      [Brief ]:       Header file of MUC geometry constants			|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 1, 2006                                             |
//      [Log   ]:       See ChangLog                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_GEO_CONST_H
#define MUC_GEO_CONST_H

  // Global
  const double PI = 3.141592654;
  const double ErrLimit = 0.01;

  const int PART_MAX 	 = 3;		// Part maximum		
  const int EEID     	 = 0;		// East endcap id
  const int BRID     	 = 1;		// Barrel id
  const int WEID     	 = 2;		// West endcap id
  const int BRTOP	 = 2;		// Top segment of barrel
  const int SEG_MAX  	 = 8;		// Segment maximum
  const int LAY_MAX  	 = 9;		// Layer maximum
  const int STR_TYPE_NUM = 4;		// Number of strip types

  const double GAS_GAP 	 = 40.0;	// Size of gas gap, mm
  const double STR_GAP 	 = 2.0;		// Size of strip gap, mm 



  // Barrel yoke
  const int    B_SEG_NUM 	 = 8;	
  const int    B_LAY_NUM 	 = 9;
  const double B_YK_LT 		 = 3940;
  const double B_YK_WT[B_LAY_NUM]  = {1293.3,1351.3,1409.3,1467.3,1533.3,1599.3,1699.3,1797.3,1919.3};
  const double B_YK_TH[B_LAY_NUM] = {30,30,30,40,40,80,80,80,150};
  const double B_YK_RB[B_LAY_NUM]  = {1740,1810,1880,1950,2030,2110,2230,2350,2470};
 
  // Endcap yoke
  const int    E_SEG_NUM 	 = 4;
  const int    E_LAY_NUM 	 = 8;
  const int    E_YK_NUM		 = 9;
  const double E_YK_ROUT          = 2500;
  const double E_YK_ZMAX          = 2800.0;
  const double E_YK_TOTALTH       = 750;
  const double E_YK_TH[E_YK_NUM]  ={40,40,30,30,30,50,80,80,50};
  const double E_YK_RIN[E_YK_NUM] ={1020,1067,1100,1133,1167,1203,1241,1302,1362};

  // Barrel strip
  const int B_ZSTR_NUM               = 48; // for each box modul
  const int B_TSTR_ANG_NUM            = 96; // for each box modul
  const int B_TSTR_TOP_NUM            = 112;// for each box modul
  const int B_STR_TYPE[STR_TYPE_NUM]   = {0,1}; // 0 -> Z, 1 -> Phi
  const int B_ZSTR_CUT_NUM[B_LAY_NUM]  = {16, 0, 12, 0, 11, 0, 10, 0, 9};
  const int B_TSTR_CUT_NUM            = 80;

  const double B_STR_DIST[B_LAY_NUM]  = {20,39,27,39,30,39,33,39,37};
  const double B_ZSTR_CUT[2]         = {540,610};
  const double B_TSTR_CUT            = 642; 

  // Endcap strip
  const int E_STR_TYPE[STR_TYPE_NUM]   = {0,1}; // 0 -> X, 1 -> Y

  // Barrel gas gap
  const double B_GASGAP_RB[B_LAY_NUM] = {1700,1770,1840,1910,1990,2070,2190,2310,2430};


#endif
