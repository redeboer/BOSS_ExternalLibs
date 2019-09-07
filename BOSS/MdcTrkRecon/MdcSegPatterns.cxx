// MdcSegPatterns.cxx
// 

// Interface Dependencies ----------------------------------------------

#include "MdcTrkRecon/MdcSegPatterns.h"

// Implementation Dependencies -----------------------------------------

#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include "MdcTrkRecon/countBits.h"

// End Implementation Dependencies -------------------------------------

MdcSegPatterns::MdcSegPatterns(int useAllAmbig) {
  
  patt4[0] = 0225; 
  patt4[1] = 0125; 
  patt4[2] = 0115; 
  patt4[3] = 055; 
  patt4[4] = 0226; 
  patt4[5] = 0126; 
  patt4[6] = 0116; 
  patt4[7] = 056;

  patt3[0] = 0224; 
  patt3[1] = 0221; 
  patt3[2] = 0205; 
  patt3[3] = 025; 
  patt3[4] = 0124; 
  patt3[5] = 0121; 
  patt3[6] = 0105; 
  patt3[7] = 0114;		   
  patt3[8] = 0111; 
  patt3[9] = 015; 
  patt3[10] = 054; 
  patt3[11] = 051; 
  patt3[12] = 045; 
  patt3[13] = 0206; 
  patt3[14] = 026;		   
  patt3[15] = 0106; 
  patt3[16] = 0112; 
  patt3[17] = 016; 
  patt3[18] = 052; 
  patt3[19] = 046;
  ambigPatt4 = new int*[8];
  assert(ambigPatt4 != 0);
  int i;
  for (i = 0; i < 8; i++) {
    ambigPatt4[i] = new int[16];
    assert(ambigPatt4[i] != 0);
    if (useAllAmbig) {
      for (int j = 0; j < 16; j++) ambigPatt4[i][j] = 1;
    } else {
      for (int j = 0; j < 16; j++) ambigPatt4[i][j] = 0;
    }
  }
  ambigPatt3 = new int*[20];
  assert(ambigPatt3 != 0);
  for (i = 0; i < 20; i++) {
    ambigPatt3[i] = new int[8];
    assert(ambigPatt3[i] != 0);
    if (useAllAmbig) {
      for (int j = 0; j < 8; j++) ambigPatt3[i][j] = 1;
    } else {
      for (int j = 0; j < 8; j++) ambigPatt3[i][j] = 0;
    }
  }

  if (!useAllAmbig) {
    ambigPatt4[0][0] = ambigPatt4[0][1] = ambigPatt4[0][3] = 
      ambigPatt4[0][7] = ambigPatt4[0][8] = ambigPatt4[0][12] = 
      ambigPatt4[0][14] = ambigPatt4[0][15] = 1;
    ambigPatt4[1][8] = ambigPatt4[1][9] = ambigPatt4[1][11] = 1;
    ambigPatt4[2][4] = ambigPatt4[2][5] = ambigPatt4[2][12] = 
      ambigPatt4[2][13] = 1;
    ambigPatt4[3][1] = ambigPatt4[3][9] = ambigPatt4[3][13] = 1;
    ambigPatt4[4][2] = ambigPatt4[4][6] = ambigPatt4[4][14] = 1;
    ambigPatt4[5][2] = ambigPatt4[5][3] = ambigPatt4[5][10] = 
      ambigPatt4[5][11] = 1;
    ambigPatt4[6][4] = ambigPatt4[6][6] = ambigPatt4[6][7] = 1;
    ambigPatt4[7][0] = ambigPatt4[7][1] = ambigPatt4[7][3] = 
      ambigPatt4[7][7] = ambigPatt4[7][8] = ambigPatt4[7][12] = 
      ambigPatt4[7][14] = ambigPatt4[7][15] = 1;
    
    ambigPatt3[0][0] = ambigPatt3[0][1] = ambigPatt3[0][3] = 
      ambigPatt3[0][4] = ambigPatt3[0][6] = ambigPatt3[0][7] = 1;
    ambigPatt3[1][0] = ambigPatt3[1][1] = ambigPatt3[1][3] = 
      ambigPatt3[1][4] = ambigPatt3[1][6] = ambigPatt3[1][7] = 1;
    ambigPatt3[2][0] = ambigPatt3[2][1] = ambigPatt3[2][3] = 
      ambigPatt3[2][4] = ambigPatt3[2][6] = ambigPatt3[2][7] = 1;
    ambigPatt3[3][0] = ambigPatt3[3][1] = ambigPatt3[3][3] = 
      ambigPatt3[3][4] = ambigPatt3[3][6] = ambigPatt3[3][7] = 1;
    ambigPatt3[4][1] = ambigPatt3[4][4] = ambigPatt3[4][5] = 1;
    ambigPatt3[5][4] = ambigPatt3[5][5] = 1;
    ambigPatt3[6][0] = ambigPatt3[6][1] = ambigPatt3[6][4] = 
      ambigPatt3[6][5] = ambigPatt3[6][7] = 1;
    ambigPatt3[7][2] = ambigPatt3[7][3] = ambigPatt3[7][6] = 1;
    ambigPatt3[8][2] = ambigPatt3[8][3] = ambigPatt3[8][6] = 
      ambigPatt3[8][7] =    ambigPatt3[8][0] = 1;
    ambigPatt3[9][1] = ambigPatt3[9][4] = ambigPatt3[9][5] = 1;
    ambigPatt3[10][0] = ambigPatt3[10][1] = ambigPatt3[10][3] = 
      ambigPatt3[10][4] = ambigPatt3[10][6] = ambigPatt3[10][7] = 1;
    ambigPatt3[11][1] = ambigPatt3[11][5] = ambigPatt3[11][7] = 
      ambigPatt3[11][0] = ambigPatt3[11][4] = 1;
    ambigPatt3[12][4] = ambigPatt3[12][5] = 1;
    ambigPatt3[13][2] = ambigPatt3[13][6] = 1;
    ambigPatt3[14][2] = ambigPatt3[14][3] = ambigPatt3[14][6] = 1;
    ambigPatt3[15][0] = ambigPatt3[15][2] = ambigPatt3[15][3] = 
      ambigPatt3[15][6] = ambigPatt3[15][7] = 1;
    ambigPatt3[16][2] = ambigPatt3[16][3] = 1;
    ambigPatt3[17][0] = ambigPatt3[17][1] = ambigPatt3[17][3] = 
      ambigPatt3[17][4] = ambigPatt3[17][6] = ambigPatt3[17][7] = 1;
    ambigPatt3[18][0] = ambigPatt3[18][1] = ambigPatt3[18][3] = 
      ambigPatt3[18][4] = ambigPatt3[18][6] = ambigPatt3[18][7] = 1;
    ambigPatt3[19][0] = ambigPatt3[19][1] = ambigPatt3[19][3] = 
      ambigPatt3[19][4] = ambigPatt3[19][6] = ambigPatt3[19][7] = 1;
  }

  // For hit combos with allowed patterns; allocate space for list of 
  // patterns, fill list and record number in npattx[i]
  int j, nbit;
  unsigned match;
  for (i = 0; i < 256; i++) {
    npatt4[i] = 0;
    npatt3[i] = 0;
    // loop through the 8 patterns, seeing how many match
    for (j = 0; j < 8; j++) {
      match = i & patt4[j];
      nbit = countbits(match);
      if (nbit == 4) npatt4[i]++;
    }
    allowedPatt4[i] = new int [npatt4[i]];
    // Add the allowed patterns to the list for this hit combination
    int nstored = 0;
    for (j = 0; j < 8; j++) {
      match = i & patt4[j];
      nbit = countbits(match);
      if (nbit == 4) {
	allowedPatt4[i][nstored] = j;
	nstored++;
      }
    }
    
    // Repeat for 3-hit patterns
    
    for (j = 0; j < 20; j++) {
      match = i & patt3[j];
      nbit = countbits(match);
      if (nbit == 3) npatt3[i]++;
    }
    allowedPatt3[i] = new int [npatt3[i]];
    // Add the allowed patterns to the list for this hit combination
    nstored = 0;
    for (j = 0; j < 20; j++) {
      match = i & patt3[j];
      nbit = countbits(match);
      if (nbit == 3) {
	allowedPatt3[i][nstored] = j;
	nstored++;
      }
    }
  }

  return;
}

MdcSegPatterns::~MdcSegPatterns() { 
  int i;
  for (i = 0; i < 256; i++) {
    delete [] allowedPatt4[i];
    delete [] allowedPatt3[i];
  }
  for (i = 0; i < 8; i++) delete [] ambigPatt4[i];
  for (i = 0; i < 20; i++) delete [] ambigPatt3[i];
  delete [] ambigPatt4;
  delete [] ambigPatt3;
}






